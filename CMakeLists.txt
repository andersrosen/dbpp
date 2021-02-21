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

set(CMAKE_DEBUG_POSTFIX -d)

# External Dependencies. Note that the adapters have additional dependencies defined in their own CMakeLists.txt
find_package(Doxygen)
add_subdirectory(thirdparty)

# Main library / API
add_subdirectory(dbpp)

# Adapters
add_subdirectory(dbpp-sqlite3)

# Tests
add_subdirectory(test)

# When generating docs we'll always include everything, even if not all adapters are built
set(DBPP_DOXYGEN_INPUT_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/dbpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dbpp-sqlite3/include
)
add_subdirectory(docs)

if (DBPP_ENABLE_INSTALL)
    # Note that the sub directories include install instructions for their own stuff

    # Meta files that should always be installed
    install(
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
            ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT dbpp_Runtime
    )
endif()

# Packaging
if (MASTER_PROJECT)
    add_subdirectory(packaging)
endif()
