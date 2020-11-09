cmake_minimum_required(VERSION 3.15)

# Find out if we're part of a bigger project
set(MASTER_PROJECT OFF)
if (NOT DEFINED PROJECT_NAME)
    set(MASTER_PROJECT ON)
endif()

# Get version from the main header
file(READ include/dbpp/config.h dbpp_config_h)
if (NOT dbpp_config_h MATCHES "DBPP_VERSION ([0-9]+)([0-9][0-9])([0-9][0-9])")
  message(FATAL_ERROR "Cannot get DBPP_VERSION from dbpp/config.h")
endif ()
# Use math to skip leading zeroes if any.
math(EXPR CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_MATCH_1})
math(EXPR CPACK_PACKAGE_VERSION_MINOR ${CMAKE_MATCH_2})
math(EXPR CPACK_PACKAGE_VERSION_PATCH ${CMAKE_MATCH_3})

project(dbpp VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" LANGUAGES CXX C)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

include(GNUInstallDirs)

# Settings
set(DBPP_PKGCONFIG_DIR "${CMAKE_INSTALL_DATADIR}/pkgconfig" CACHE PATH
    "Installation directory for pkgconfig (.pc) files.")
set(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
option(DBPP_ENABLE_INSTALL "Generate install target" ${MASTER_PROJECT})
option(DBPP_ENABLE_TESTS "Build tests" ${MASTER_PROJECT})
option(DBPP_ENABLE_DOCS "Generate documentation using Doxygen" ${MASTER_PROJECT})
option(DBPP_ENABLE_DEV_MODE "Enable strict compiler warnings" OFF)
option(DBPP_ENABLE_SANITIZERS "Use address, leak and undefined behavior sanitizers" ${DBPP_ENABLE_DEV_MODE})
option(DBPP_ENABLE_COVERAGE "Compile with code coverage analysis" OFF)
option(DBPP_USE_BUNDLED_SQLITE "Use the version of SQLite that's bundled with dbpp" OFF)

# CMake script include path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# External Dependencies
find_package(Doxygen)
if (NOT DBPP_USE_BUNDLED_SQLITE)
    find_package(SQLite3 REQUIRED)
endif()

add_subdirectory(thirdparty)

# Target definition
set(CMAKE_DEBUG_POSTFIX "-d")

add_library(dbpp)
add_library(${PROJECT_NAME}::dbpp ALIAS dbpp)

target_compile_features(dbpp PUBLIC cxx_std_17)

if (${BUILD_SHARED_LIBS})
    target_compile_definitions(dbpp PUBLIC $<BUILD_INTERFACE:BUILDING_DBPP_DLL>)
endif()

set_target_properties(dbpp PROPERTIES
    VERSION ${PROJECT_VERSION} SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR}
)

target_link_libraries(dbpp PUBLIC SQLite::SQLite3)

if (DBPP_ENABLE_DEV_MODE)
    target_compile_options(dbpp PRIVATE
        -Wall
        -pedantic
        -pedantic-errors
        -Wextra
        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wdisabled-optimization
        -Werror
        -Wfloat-equal
        -Wformat=2
        -Wformat-nonliteral
        -Wformat-security
        -Wformat-y2k
        -Wimport
        -Winit-self
        -Winline
        -Winvalid-pch
        -Wmissing-field-initializers
        -Wmissing-format-attribute
        -Wmissing-include-dirs
        -Wmissing-noreturn
        -Wpacked
        -Wpointer-arith
        -Wredundant-decls
        -Wshadow
        -Wstack-protector
        -Wstrict-aliasing=2
        -Wswitch-default
        -Wswitch-enum
        -Wunreachable-code
        -Wunused
        -Wunused-parameter
        -Wvariadic-macros
        -Wwrite-strings
    )
endif()

if (DBPP_ENABLE_COVERAGE)
    target_compile_options(dbpp PUBLIC -coverage)
    target_link_libraries(dbpp PUBLIC gcov)
endif()

if (DBPP_ENABLE_SANITIZERS)
    target_compile_options(dbpp PUBLIC -fsanitize=address -fsanitize=leak -fsanitize=undefined -g)
    target_link_options(dbpp PUBLIC -fsanitize=address -fsanitize=leak -fsanitize=undefined -g)
endif()

add_subdirectory(include)
add_subdirectory(src)

# Testing
if (DBPP_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Docs
if (DBPP_ENABLE_DOCS AND DOXYGEN_FOUND)
    add_subdirectory(docs)
endif()

if (DBPP_ENABLE_INSTALL)
    # Generate CMake package configuration files, to support find_package()
    set(version_config "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake")
    set(project_config "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake")
    set(targets_export_name "dbpp-targets")
    set(namespace "${PROJECT_NAME}::")
    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(${version_config}
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake.in
        ${project_config}
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    # Also support pkgconfig
    configure_file(
        ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc
        @ONLY
    )

    # Installation
    install(TARGETS dbpp
        EXPORT ${targets_export_name}
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(EXPORT ${targets_export_name}
        FILE ${targets_export_name}.cmake
        NAMESPACE ${namespace}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
    install(
        FILES
            ${project_config}
            ${version_config}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
    install(
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
            ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )
    install(
        FILES
            ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION ${DBPP_PKGCONFIG_DIR}
    )
endif()

# Packaging
if (MASTER_PROJECT)
    set(CPACK_PROJECT_URL "https://github.com/andersrosen/dbpp")
    set(CPACK_PACKAGE_VENDOR  "Anders Rosén")
    set(CPACK_PACKAGE_CONTACT "Anders Rosén <panrosen@gmail.com>")

    set(CPACK_GENERATOR "TBZ2")

    set(CPACK_SOURCE_GENERATOR "TBZ2")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")

    set(CPACK_SOURCE_IGNORE_FILES
        "/\\\\.git/"
        "/\\\\.gitmodules"
        "/build/"
        "/cmake-build-.*"
        "/cmake-install-.*"
        "/\\\\.idea"
        "/\\\\.DS_Store"
    )

    include(CPack)
endif()

