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

    add_library(sqlite3)
    add_library(SQLite::SQLite3 ALIAS sqlite3)
    target_link_libraries(sqlite3 PUBLIC Threads::Threads ${CMAKE_DL_LIBS})
    target_sources(sqlite3 PRIVATE
        sqlite/sqlite3.c
        sqlite/sqlite3.h
        sqlite/sqlite3ext.h
    )
    target_include_directories(sqlite3 PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sqlite>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    if (DBPP_ENABLE_INSTALL)
        install(TARGETS sqlite3
            EXPORT dbpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
        install(
            FILES
                sqlite/sqlite3.h
                sqlite/sqlite3ext.h
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
    endif()
endif()
