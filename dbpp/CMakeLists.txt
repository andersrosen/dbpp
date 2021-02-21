add_library(dbpp)
add_library(${PROJECT_NAME}::dbpp ALIAS dbpp)

target_sources(dbpp PRIVATE
    include/dbpp/dbpp.h
    include/dbpp/Connection.h
    include/dbpp/Exception.h
    include/dbpp/MetaFunctions.h
    include/dbpp/Result.h
    include/dbpp/PlaceholderBinder.h
    include/dbpp/PreparedStatement.h
    include/dbpp/Statement.h
    include/dbpp/StatementBuilder.h
    include/dbpp/util.h
    include/dbpp/adapter/Connection.h
    include/dbpp/adapter/PreparedStatement.h
    include/dbpp/adapter/Result.h
    include/dbpp/adapter/Statement.h
    include/dbpp/adapter/Types.h

    src/Connection.cpp
    src/PreparedStatement.cpp
    src/Result.cpp
    src/Statement.cpp
    src/StatementBuilder.cpp
)

target_compile_features(dbpp PUBLIC cxx_std_17)
target_include_directories(dbpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>
)
set_target_properties(dbpp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Don't export symbols unless told so
set_target_properties(dbpp PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES hidden
)

# Generated files
include(GenerateExportHeader)
generate_export_header(dbpp EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/dbpp/exports.h)
configure_file(config.h.in include/dbpp/config.h @ONLY)
set_property(
    TARGET dbpp
    PROPERTY PUBLIC_HEADER
        ${CMAKE_CURRENT_BINARY_DIR}/include/dbpp/exports.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/dbpp/config.h
)

if (DBPP_ENABLE_INSTALL)
    # Generate CMake package configuration files, to support find_package()
    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        dbpp-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/dbpp-config.cmake
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dbpp"
    )
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/dbpp-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/dbpp-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/dbpp-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dbpp
        COMPONENT dbpp_Development
    )

    # Libraries and executables
    install(TARGETS dbpp
        EXPORT dbpp-exports
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT dbpp_Runtime
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT dbpp_Runtime
            NAMELINK_COMPONENT dbpp_Development
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT dbpp_Development
        PUBLIC_HEADER
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dbpp
            COMPONENT dbpp_Development
    )

    # Package target exports
    install(EXPORT dbpp-exports
        FILE dbpp-targets.cmake
        NAMESPACE dbpp::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dbpp
        COMPONENT dbpp_Development
    )

    # Headers
    install(DIRECTORY include/dbpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT dbpp_Development
    )
endif()
