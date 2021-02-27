option(DBPP_ENABLE_TESTS "Build tests" ${MASTER_PROJECT})

if (DBPP_ENABLE_TESTS)
    enable_testing()

    add_executable(test_dbpp testrunner.cpp)
    target_sources(test_dbpp PRIVATE
        Persons.cpp
        Persons.h
        TestConnection.cpp
        TestResult.cpp
        TestStatement.cpp
        TestStatementBuilder.cpp
    )

    target_link_libraries(test_dbpp PRIVATE dbpp::Sqlite3 Catch2::Catch2)

    include(Catch)
    catch_discover_tests(test_dbpp)
endif()
