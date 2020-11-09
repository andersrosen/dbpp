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

#include <catch2/catch.hpp>
#include <cstring>
#include <dbpp/Sqlite3.h>
#include <dbpp/dbpp.h>
#include <optional>

using namespace Dbpp;

class Util {
    public:
    Connection db;

    int personCount = 0;
    int64_t johnDoeId = -1;
    int64_t janeDoeId = -1;
    int64_t andersSvenssonId = -1;

    Util()
    : db(Sqlite3::open(":memory:"))
    {
    }

    void populate() {
        db.exec("CREATE TABLE person ("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " name TEXT NOT NULL,"
                " age INTEGER NOT NULL,"
                " spouse_id INTEGER REFERENCES person(id)"
                ")");

        johnDoeId = db.exec("INSERT INTO person (name, age) VALUES ('John Doe', 48)").getInsertId();
        ++personCount;
        janeDoeId = db.exec("INSERT INTO person (name, age) VALUES ('Jane Doe', 45)").getInsertId();
        ++personCount;
        andersSvenssonId = db.exec("INSERT INTO person (name, age) VALUES ('Anders Svensson', 38)").getInsertId();
        ++personCount;
        db.exec("UPDATE person set spouse_id = ? WHERE id = ?", janeDoeId, johnDoeId);
        db.exec("UPDATE person set spouse_id = ? WHERE id = ?", johnDoeId, janeDoeId);
    }
};

TEST_CASE("Connection", "[api]") {
    Util util;
    Connection &db = util.db;

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

    util.populate();

    SECTION("Connection move constructor and move assignment") {
        auto movedDb = std::move(db);
        REQUIRE(movedDb.get<int>("SELECT COUNT(*) FROM PERSON") == util.personCount);

        db = std::move(movedDb);
    }

    SECTION("Connection::prepare()") {
        auto stmt = db.prepare("SELECT COUNT(*) FROM person");
        auto result = stmt.step();
        REQUIRE(result);
        REQUIRE(result.get<int>(0) == util.personCount);

        stmt = db.prepare("SELECT name FROM person WHERE id = ?", util.johnDoeId);
        result = stmt.step();
        REQUIRE(result);
        REQUIRE(result.get<std::string>(0) == "John Doe");

        stmt = db.prepare("SELECT id FROM person WHERE name = ?", "John Doe");
        result = stmt.step();
        REQUIRE(result);
        REQUIRE(result.get<int>(0) == util.johnDoeId);

        REQUIRE_THROWS_AS(db.prepare("SELECT COUNT"), Error);
        REQUIRE_THROWS_AS(db.prepare("SELECT age FROM person WHERE id = ?", util.johnDoeId, 888), PlaceholderOutOfRange);
    }

    SECTION("Connection::get<T>(), where T is a basic type") {
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == 3);
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person WHERE name = 'John Doe'") == 1);
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person WHERE name = ?", "John Doe") == 1);
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person WHERE name = ?", std::string{ "John Doe" }) == 1);
        REQUIRE(db.get<short>("SELECT COUNT(*) FROM person WHERE name = ?", std::string{ "John Doe" }) == static_cast<short>(1));
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person WHERE name = ? AND age = ?", "John Doe", 48) == 1);
        REQUIRE(db.get<std::string>("SELECT name FROM person WHERE id = ?", util.andersSvenssonId) == "Anders Svensson");
        REQUIRE_THROWS_AS(db.get<int>("SELECT COUNT"), Error);
        REQUIRE_THROWS_AS(db.get<int>("SELECT age FROM person WHERE id = ?"), Error);
        REQUIRE_THROWS_AS(db.get<int>("SELECT age FROM person WHERE id = ?", util.johnDoeId, 888), PlaceholderOutOfRange);
        REQUIRE_THROWS_AS(db.get<int>("SELECT * FROM person WHERE id = ?", util.johnDoeId), Error);
    }

    SECTION("Connection::get<T>(), where T is std::optional") {
        auto val = db.get<std::optional<int>>("SELECT spouse_id FROM person WHERE id = ?", util.johnDoeId);
        REQUIRE(val);
        REQUIRE(*val == util.janeDoeId);
        val = db.get<std::optional<int>>("SELECT spouse_id FROM person WHERE id = ?", util.andersSvenssonId);
        REQUIRE(!val);
        REQUIRE_THROWS_AS(db.get<std::optional<int>>("SELECT COUNT"), Error);
        REQUIRE_THROWS_AS(db.get<std::optional<int>>("SELECT age FROM person WHERE id = ?"), Error);
        REQUIRE_THROWS_AS(db.get<std::optional<int>>("SELECT age FROM person WHERE id = ?", util.johnDoeId, 888), PlaceholderOutOfRange);
    }

    SECTION("Connection::getOptional<T>(), where T is a basic type") {
        auto val = db.getOptional<int>("SELECT spouse_id FROM person WHERE id = ?", util.johnDoeId);
        REQUIRE(val);
        REQUIRE(*val == util.janeDoeId);
        val = db.getOptional<int>("SELECT spouse_id FROM person WHERE id = ?", util.andersSvenssonId);
        REQUIRE(!val);
        REQUIRE_THROWS_AS(db.getOptional<int>("SELECT COUNT"), Error);
        REQUIRE_THROWS_AS(db.getOptional<int>("SELECT age FROM person WHERE id = ?"), Error);
        REQUIRE_THROWS_AS(db.getOptional<int>("SELECT age FROM person WHERE id = ?", util.johnDoeId, 888), PlaceholderOutOfRange);
        REQUIRE_THROWS_AS(db.getOptional<int>("SELECT * FROM person WHERE id = ?", util.johnDoeId), Error);
    }

    SECTION("Connection::get<Ts...>") {
        {
            const auto& [name, age, maybeSpouseId] = db.get<std::string, int, std::optional<std::int64_t>>("SELECT name, age, spouse_id FROM person WHERE id = ?", util.johnDoeId);
            REQUIRE(name == "John Doe");
            REQUIRE(age == 48);
            REQUIRE(maybeSpouseId);
            REQUIRE(*maybeSpouseId == util.janeDoeId);
        }

        {
            const auto& [name, age, maybeSpouseId] = db.get<std::string, int, std::optional<std::int64_t>>("SELECT name, age, spouse_id FROM person WHERE id = ?", util.andersSvenssonId);
            REQUIRE(name == "Anders Svensson");
            REQUIRE(age == 38);
            REQUIRE(!maybeSpouseId);
        }
    }

    SECTION("Connection::begin(), commit(), rollback()") {
        db.begin();
        db.exec("INSERT INTO person (name, age) VALUES ('Donald Duck', 86)");
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == util.personCount + 1);
        db.rollback();
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == util.personCount);

        db.begin();
        auto id = db.exec("INSERT INTO person (name, age) VALUES ('James Smith', 103)").getInsertId();
        db.commit();
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == util.personCount + 1);
        db.exec("DELETE FROM person WHERE id = ?", id);
    }

    SECTION("Transaction class") {
        {
            Transaction tr(db);
            db.exec("INSERT INTO person (name, age) VALUES ('Donald Duck', 86)");
            REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == util.personCount + 1);
        }
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == util.personCount);

        int id;
        {
            Transaction tr(db);
            id = db.exec("INSERT INTO person (name, age) VALUES ('James Smith', 103)").getInsertId();
            tr.commit();
        }
        REQUIRE(db.get<int>("SELECT COUNT(*) FROM person") == util.personCount + 1);
        db.exec("DELETE FROM person WHERE id = ?", id);
    }

    SECTION("Connection::adapterName()") {
        REQUIRE(db.adapterName() == "sqlite3");
    }
}
