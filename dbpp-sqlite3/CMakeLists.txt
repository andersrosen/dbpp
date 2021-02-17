option(DBPP_USE_BUNDLED_SQLITE "Use the version of SQLite that's bundled with dbpp" OFF)
if (NOT DBPP_USE_BUNDLED_SQLITE)
    find_package(SQLite3 REQUIRED)
endif()

add_library(dbpp-sqlite3)
add_library(dbpp::Sqlite3 ALIAS dbpp-sqlite3)
set_target_properties(dbpp-sqlite3 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_compile_features(dbpp-sqlite3 PUBLIC cxx_std_17)
# Don't export symbols unless told so
set_target_properties(dbpp-sqlite3 PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES hidden)
include(GenerateExportHeader)
generate_export_header(dbpp-sqlite3 EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/dbpp/sqlite3/exports.h)

target_sources(dbpp-sqlite3
    PRIVATE
        src/Sqlite3.cpp
        include/dbpp/sqlite3/Sqlite3.h
)

target_include_directories(dbpp-sqlite3
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>
)
set_property(
    TARGET dbpp-sqlite3
    PROPERTY PUBLIC_HEADER
        include/dbpp/sqlite3/Sqlite3.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/dbpp/sqlite3/exports.h
)

target_link_libraries(dbpp-sqlite3
    PUBLIC
        dbpp::dbpp
        SQLite::SQLite3
)

if (DBPP_ENABLE_INSTALL)
    # Install the libs and headers
    install(TARGETS dbpp-sqlite3
        EXPORT dbpp-sqlite3-exports
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT dbpp_sqlite3_Runtime
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT dbpp_sqlite3_Runtime
            NAMELINK_COMPONENT dbpp_sqlite3_Development
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT dbpp_sqlite3_Development
        PUBLIC_HEADER
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dbpp/sqlite3
            COMPONENT dbpp_sqlite3_Development
    )
    # Package target exports
    install(
        EXPORT dbpp-sqlite3-exports
        FILE dbpp-sqlite3-targets.cmake
        NAMESPACE dbpp::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dbpp
        COMPONENT dbpp_Development
    )
    configure_file(dbpp-sqlite3-config.cmake.in dbpp-sqlite3-config.cmake @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/dbpp-sqlite3-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dbpp
    )
endif()
