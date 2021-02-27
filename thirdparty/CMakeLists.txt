if (NOT TARGET Catch2::Catch2)
    if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/Catch2)
        add_subdirectory(Catch2)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/Catch2/contrib")
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
    else()
        find_package(Catch2)
    endif()
endif()

if (DBPP_USE_BUNDLED_SQLITE)
    find_package(Threads REQUIRED)

    add_library(dbpp_bundled_sqlite3)
    add_library(SQLite::SQLite3 ALIAS dbpp_bundled_sqlite3)
    target_link_libraries(dbpp_bundled_sqlite3 PUBLIC Threads::Threads ${CMAKE_DL_LIBS})
    set(PUBLIC_HEADERS
        sqlite/sqlite3.h
        sqlite/sqlite3ext.h
    )
    target_sources(dbpp_bundled_sqlite3 PRIVATE
        sqlite/sqlite3.c
        ${PUBLIC_HEADERS}
    )
    set_target_properties(dbpp_bundled_sqlite3 PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
    target_include_directories(dbpp_bundled_sqlite3 PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sqlite>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    if (DBPP_ENABLE_INSTALL)
        install(TARGETS dbpp_bundled_sqlite3
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
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                COMPONENT dbpp_sqlite3_Development
        )
    endif()
endif()
