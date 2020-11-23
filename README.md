# dbpp
A simple database client library for C++

The purpose of dbpp is to provide an easy-to-use interface to SQL databases.
It is currently under development and the **interface is not stable**. So far
sqlite3 is the only supported database.

## Build status
[![Ubuntu Actions Status](https://github.com/andersrosen/dbpp/workflows/Ubuntu/badge.svg)](https://github.com/andersrosen/dbpp/actions?query=workflow%3AUbuntu)
[![MacOS Actions Status](https://github.com/andersrosen/dbpp/workflows/MacOS/badge.svg)](https://github.com/andersrosen/dbpp/actions?query=workflow%3AmacOS)
[![Windows Actions Status](https://github.com/andersrosen/dbpp/workflows/Windows/badge.svg)](https://github.com/andersrosen/dbpp/actions?query=workflow%3AWindows)

## Reference documentation

[Latest unreleased](https://andersrosen.github.io/dbpp/docs/main/index.html) (generated from the latest commit to main)<br>

## Usage
```c++
#include <dbpp/dbpp.h>
#include <dbpp/Sqlite3.h>

...
// Set up the connection to the database
auto db = Dbpp::Sqlite3::open(":memory:");

// You can iterate over the results of a query
for (auto&& row : db.statement("SELECT * FROM persons WHERE age > ?", 18)) {
    std::cout << "Name: " << row.get<std::string>("name")
              << ", age: " << row.get<int>("age") << std::endl;
}

// Another way of iteration, using tuples
for (auto &[name, age] : db.statement("SELECT name, age FROM persons WHERE age > ?", 18)
                         .as<std::string, int>()) {
    std::cout << "Name: " << name << ", age: " << age << std::endl;
}

// You can opt to execute a statement immediately, which is useful if it
// doesn't produce a result.
db.exec("DELETE FROM persons WHERE age < ?", 18);

// If you only query for a single value, you can use the get() method:
int numberOfAdults = db.get<int>("SELECT COUNT(*) FROM persons WHERE age >= ?", 18);

// If you only query for a single row, you can get it as a tuple
const int someId = 1234;
const auto& [name, age] = db.get<std::string, int>("SELECT name, age FROM persons WHERE id = ?", someId);
```

## Building and Installing

The library template example is using Catch2 for unit testing. It is added as
a git submodule.

To configure the project, assuming you want to use tmp/build as
the build directory:
```
cd dbpp
cmake -B /tmp/build
```

To build and test:
```
cmake --build /tmp/build
(cd /tmp/build && ctest) # Terse output
(cd /tmp/build && ctest --output-on-failure) # Shows output from tests that fail 
```

To install the project:
```
cmake --build /tmp/build --target install
```
