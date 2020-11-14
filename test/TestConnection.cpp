// Copyright (C) 2020 Anders Rosén (panrosen@gmail.com)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
// USA

#include "Persons.h"

#include <catch2/catch.hpp>

using namespace Dbpp;

TEST_CASE("Connection", "[api]") {
    Persons persons;
    Connection &db = persons.db;

    SECTION("Connection::exec()") {
        db.exec("CREATE TABLE person ("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " name TEXT NOT NULL,"
                " age INTEGER NOT NULL,"
                " spouse_id INTEGER REFERENCES person(id)"
                ")");

        REQUIRE_THROWS_AS(db.exec("INSERT INTO person (name, age) VALUES ('John Doe')"), Error);
        REQUIRE_THROWS_AS(db.exec("INSERT INTO person (name, age) VALUES ('John Doe', ?)", 48, 15), PlaceholderOutOfRange);

        auto result = db.exec("INSERT INTO person (name, age) VALUES ('John Doe', 48)");
        REQUIRE(result.empty());
        REQUIRE(result.getInsertId() == 1);
        result = db.exec("INSERT INTO person (name, age) VALUES ('Jane Doe', ?)", 45);
        REQUIRE(result.empty());
        REQUIRE(result.getInsertId() == 2);
        result = db.exec("INSERT INTO person (name, age) VALUES (?, ?)", "Anders Svensson", 38);
        REQUIRE(result.empty());
        REQUIRE(result.getInsertId() == 3);

        result = db.exec("SELECT COUNT(*) FROM person WHERE age < ?", 40);
        REQUIRE(result);
        REQUIRE(result.get<int>(0) == 1);

        result = {}; // Result will keep the statement alive if not cleared, thus locking the DB
        db.exec("DROP TABLE person");
    }

    persons.populate();

    SECTION("Connection move constructor and move assignment") {
        auto movedDb = std::move(db);
        REQUIRE(movedDb.get<int>("SELECT COUNT(*) FROM person") == persons.Count);

        db = std::move(movedDb);
    }

    SECTION("Connection::prepare()") {
        auto stmt = db.prepare("SELECT COUNT(*) FROM person");
        auto result = stmt.step();
        REQUIRE(result);
        REQUIRE(result.get<int>(0) == persons.Count);

        stmt = db.prepare("SELECT name FROM person WHERE id = ?", persons.johnDoe().id);
        result = stmt.step();
        REQUIRE(result);
        REQUIRE(result.get<std::string>(0) == persons.johnDoe().name);

        stmt = db.prepare("SELECT id FROM person WHERE name = ?", persons.johnDoe().name);
        result = stmt.step();
        REQUIRE(result);
        REQUIRE(result.get<int>(0) == persons.johnDoe().id);

        REQUIRE_THROWS_AS(db.prepare("SELECT COUNT"), Error);
        REQUIRE_THROWS_AS(db.prepare("SELECT age FROM person WHERE id = ?", persons.johnDoe().id, 888), PlaceholderOutOfRange);
    }

    SECTION("Connection::get<T>(), where T is a basic type") {
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == 3);
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person WHERE name = 'John Doe'") == 1);
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person WHERE name = ?", persons.johnDoe().name.c_str()) == 1);
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person WHERE name = ?", persons.johnDoe().name) == 1);
        REQUIRE(db.get<short>("SELECT COUNT(*) FROM person WHERE name = ?", persons.johnDoe().name) == static_cast<short>(1));
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person WHERE name = ? AND age = ?", persons.johnDoe().name, persons.johnDoe().age) == 1);
        REQUIRE(db.get<std::string>("SELECT name FROM person WHERE id = ?", persons.andersSvensson().id) == persons.andersSvensson().name);
        REQUIRE_THROWS_AS(db.get<int>("SELECT COUNT"), Error);
        REQUIRE_THROWS_AS(db.get<int>("SELECT age FROM person WHERE id = ?"), Error);
        REQUIRE_THROWS_AS(db.get<int>("SELECT age FROM person WHERE id = ?", persons.johnDoe().id, 888), PlaceholderOutOfRange);
        REQUIRE_THROWS_AS(db.get<int>("SELECT * FROM person WHERE id = ?", persons.johnDoe().id), Error);
    }

    SECTION("Connection::get<T>(), where T is std::optional") {
        auto val = db.get<std::optional<int>>("SELECT spouse_id FROM person WHERE id = ?", persons.johnDoe().id);
        REQUIRE(val);
        REQUIRE(*val == persons.janeDoe().id);
        val = db.get<std::optional<int>>("SELECT spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
        REQUIRE(!val);
        REQUIRE_THROWS_AS(db.get<std::optional<int>>("SELECT COUNT"), Error);
        REQUIRE_THROWS_AS(db.get<std::optional<int>>("SELECT age FROM person WHERE id = ?"), Error);
        REQUIRE_THROWS_AS(db.get<std::optional<int>>("SELECT age FROM person WHERE id = ?", persons.johnDoe().id, 888), PlaceholderOutOfRange);
    }

    SECTION("Connection::getOptional<T>(), where T is a basic type") {
        auto val = db.getOptional<int>("SELECT spouse_id FROM person WHERE id = ?", persons.johnDoe().id);
        REQUIRE(val);
        REQUIRE(*val == persons.johnDoe().spouseId);
        val = db.getOptional<int>("SELECT spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
        REQUIRE(!val);
        REQUIRE_THROWS_AS(db.getOptional<int>("SELECT COUNT"), Error);
        REQUIRE_THROWS_AS(db.getOptional<int>("SELECT age FROM person WHERE id = ?"), Error);
        REQUIRE_THROWS_AS(db.getOptional<int>("SELECT age FROM person WHERE id = ?", persons.johnDoe().id, 888), PlaceholderOutOfRange);
        REQUIRE_THROWS_AS(db.getOptional<int>("SELECT * FROM person WHERE id = ?", persons.johnDoe().id), Error);
    }

    SECTION("Connection::get<Ts...>") {
        {
            const auto& [name, age, maybeSpouseId] = db.get<std::string, int, std::optional<std::int64_t>>(
                "SELECT name, age, spouse_id FROM person WHERE id = ?", persons.johnDoe().id);
            REQUIRE(name == persons.johnDoe().name);
            REQUIRE(age == persons.johnDoe().age);
            REQUIRE(maybeSpouseId);
            REQUIRE(*maybeSpouseId == persons.johnDoe().spouseId);
        }

        {
            const auto& [name, age, maybeSpouseId] = db.get<std::string, int, std::optional<std::int64_t>>(
                "SELECT name, age, spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
            REQUIRE(name == persons.andersSvensson().name);
            REQUIRE(age == persons.andersSvensson().age);
            REQUIRE(!maybeSpouseId);
        }
    }

    SECTION("Connection::begin(), commit(), rollback()") {
        db.begin();
        db.exec("INSERT INTO person (name, age) VALUES ('Donald Duck', 86)");
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == persons.Count + 1);
        db.rollback();
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == persons.Count);

        db.begin();
        auto id = db.exec("INSERT INTO person (name, age) VALUES ('James Smith', 103)").getInsertId();
        db.commit();
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == persons.Count + 1);
        db.exec("DELETE FROM person WHERE id = ?", id);
    }

    SECTION("Transaction class") {
        {
            Transaction tr(db);
            db.exec("INSERT INTO person (name, age) VALUES ('Donald Duck', 86)");
            REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == persons.Count + 1);
        }
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == persons.Count);

        long long id = -1;
        {
            Transaction tr(db);
            id = db.exec("INSERT INTO person (name, age) VALUES ('James Smith', 103)").getInsertId();
            tr.commit();
        }
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == persons.Count + 1);
        db.exec("DELETE FROM person WHERE id = ?", id);
    }

    SECTION("Connection::adapterName()") {
        REQUIRE(db.adapterName() == "sqlite3");
    }
}
