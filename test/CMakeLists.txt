add_executable(test_dbpp testrunner.cpp)
target_sources(test_dbpp PRIVATE
    Persons.cpp
    Persons.h
    TestConnection.cpp
    TestResult.cpp
    TestStatement.cpp
)

target_link_libraries(test_dbpp PRIVATE dbpp::dbpp Catch2::Catch2)

include(Catch)
catch_discover_tests(test_dbpp)
