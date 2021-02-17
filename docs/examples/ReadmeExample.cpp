#include <iostream>
#include <dbpp/dbpp.h>
#include <dbpp/sqlite3/Sqlite3.h>

int main() {
    // Set up the connection to the database
    auto db = Dbpp::Sqlite3::open(":memory:");

    db.exec("CREATE TABLE persons (id INTEGER PRIMARY KEY NOT NULL, name TEXT, age INTEGER)");
    db.exec("INSERT INTO persons (name, age) VALUES (?, ?)", "John Doe", 42);
    db.exec("INSERT INTO persons (name, age) VALUES (?, ?)", "Jane Doe", 39);
    db.exec("INSERT INTO persons (name, age) VALUES (?, ?)", "Kiddo Doe", 3);

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
    const int someId = 2;
    const auto& [name, age] = db.get<std::string, int>("SELECT name, age FROM persons WHERE id = ?", someId);
}

