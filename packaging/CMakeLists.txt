set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Should always be set to true to ensure proper escaping, according to Professional CMake by Craig Scott
set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PROJECT_URL "https://github.com/andersrosen/dbpp")
set(CPACK_PACKAGE_VENDOR  "Anders Rosén")
set(CPACK_PACKAGE_CONTACT "Anders Rosén <panrosen@gmail.com>")

set(CPACK_SOURCE_GENERATOR TBZ2)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")

set(CPACK_SOURCE_IGNORE_FILES
    /build/
    /cmake-build-.*/
    /cmake-install-.*/
    \\.git/
    \\.idea/
    \\.DS_Store
    \\.swp
    \\.orig
    \\.gitmodules
)
