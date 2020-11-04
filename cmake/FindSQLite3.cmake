find_path(SQLITE3_INCLUDE_DIR sqlite3.h)
find_library(SQLITE3_LIBRARY sqlite3)
mark_as_advanced(SQLITE3_INCLUDE_DIR SQLITE3_LIBRARY)

if (SQLITE3_INCLUDE_DIR AND EXISTS "${SQLITE3_INCLUDE_DIR}/sqlite3.h")
    # Get version from sqlite3.h
    file(READ "${SQLITE3_INCLUDE_DIR}/sqlite3.h" sqlite3_h)
    if (NOT sqlite3_h MATCHES "SQLITE_VERSION_NUMBER +([0-9]+)([0-9][0-9][0-9])([0-9][0-9][0-9])")
        message(FATAL_ERROR "Cannot get SQLITE_VERSION_NUMBER from sqlite3.h")
    endif ()
    # Use math to skip leading zeroes if any.
    math(EXPR SQLITE3_VERSION_MAJOR ${CMAKE_MATCH_1})
    math(EXPR SQLITE3_VERSION_MINOR ${CMAKE_MATCH_2})
    math(EXPR SQLITE3_VERSION_PATCH ${CMAKE_MATCH_3})
    set(SQLITE3_VERSION "${SQLITE3_VERSION_MAJOR}.${SQLITE3_VERSION_MINOR}.${SQLITE3_VERSION_PATCH}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3
    REQUIRED_VARS SQLITE3_INCLUDE_DIR SQLITE3_LIBRARY
    VERSION_VAR SQLITE3_VERSION
)

if (SQLite3_FOUND AND NOT TARGET SQLite3::SQLite3)
    add_library(SQLite3::SQLite3 UNKNOWN IMPORTED)
    set_property(TARGET SQLite3::SQLite3
        PROPERTY IMPORTED_LOCATION "${SQLITE3_LIBRARY}"
    )
endif()
