target_include_directories(dbpp
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_sources(dbpp PRIVATE
    dbpp/dbpp.h
    dbpp/Connection.h
    dbpp/Exception.h
    dbpp/MetaFunctions.h
    dbpp/Result.h
    dbpp/PlaceholderBinder.h
    dbpp/PreparedStatement.h
    dbpp/Statement.h
    dbpp/StatementBuilder.h
    dbpp/adapter/Connection.h
    dbpp/adapter/PreparedStatement.h
    dbpp/adapter/Result.h
    dbpp/adapter/Statement.h
    dbpp/adapter/Types.h
)

if (DBPP_ENABLE_INSTALL)
    install(DIRECTORY dbpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
