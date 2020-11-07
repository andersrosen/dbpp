#include <iostream>
#include <dbpp/dbpp.h>
#include <dbpp/Sqlite3.h>

namespace Sqlite3 = Dbpp::Sqlite3;

int main(int argc, char **argv) {
    Dbpp::Connection db = Sqlite3::open(":memory:");

    db.exec("CREATE TABLE employee ("
            " id INTEGER PRIMARY KEY AUTOINCREMENT,"
            " name TEXT NOT NULL"
            ")");
    db.exec("INSERT INTO employee (name) VALUES (?)", "James Bond");
    db.exec("INSERT INTO employee (name) VALUES (?)", "Jason Bourne");
    db.exec("INSERT INTO employee (name) VALUES (?)", "Carl Hamilton");

    for (auto &&row : db.prepare("SELECT name, id FROM employee WHERE name NOT LIKE '%Hamilton'"))
        std::cout << "Agent " << row.get<int>("id") << ": " << row.get<std::string>("name") << std::endl;

    return 0;
}
