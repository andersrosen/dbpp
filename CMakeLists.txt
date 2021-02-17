cmake_minimum_required(VERSION 3.15)

# Find out if we're part of a bigger project
set(MASTER_PROJECT OFF)
if (NOT DEFINED PROJECT_NAME)
    set(MASTER_PROJECT ON)
endif()

project(dbpp VERSION 0.0.1 LANGUAGES CXX C)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

include(GNUInstallDirs)

# Settings. Note that additional settings are added by in the subdirectories CMakeLists.txt
option(DBPP_ENABLE_INSTALL "Generate install target" ${MASTER_PROJECT})

# CMake script include path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(DevModes)

if (NOT APPLE)
    # Tell the runtime loader to look for dependent libraries in the directory where this library is
    # See Craig Scott's CppCon 2019 talk for a good explanation: https://www.youtube.com/watch?v=m0DwB4OvDXk
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

# External Dependencies. Note that the adapters have additional dependencies defined in their own CMakeLists.txt
find_package(Doxygen)
add_subdirectory(thirdparty)

# Main library / API
add_subdirectory(dbpp)

# Adapters
add_subdirectory(dbpp-sqlite3)

#
add_subdirectory(test)
add_subdirectory(docs)

if (DBPP_ENABLE_INSTALL)
    # Note that the sub directories include install instructions for their own stuff

    # Meta files that should always be installed
    install(
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
            ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
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

