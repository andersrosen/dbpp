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

TEST_CASE("Statement", "[api]") {
    Persons persons;
    Connection& db = persons.db;
    persons.populate();

    SECTION("Statement move semantics") {
        Statement st = db.prepare("SELECT COUNT(*) FROM person WHERE id = ? OR id = ?");
        st.bind(persons.johnDoe().id);
        Statement newStatement{std::move(st)};
        newStatement.bind(persons.janeDoe().id);
        auto result = newStatement.step();
        REQUIRE(result);
        REQUIRE(result.get<int>(0) == 2);

        st = db.prepare("SELECT SUM(age) FROM person WHERE id = ? OR id = ?");
        st.bind(persons.johnDoe().id);
        newStatement = std::move(st);
        newStatement.bind(persons.janeDoe().id);
        result = newStatement.step();
        REQUIRE(result);
        REQUIRE(result.get<int>(0) == persons.johnDoe().age + persons.janeDoe().age);
    }

    SECTION("Statement begin(), end()") {
        Statement st = db.prepare("SELECT * FROM person WHERE id IS NULL"); // no hits - id column defined as NOT NULL
        Statement::iterator beginning = st.begin();
        Statement::iterator end = st.end();
        REQUIRE(beginning == end);

        st = db.prepare("SELECT * FROM person WHERE id = ?", persons.johnDoe().id);
        beginning = st.begin();
        end = st.end();
        REQUIRE(beginning != end);

        ++beginning;
        REQUIRE(beginning == end);
    }

    SECTION("bind(), null values and optional") {
        Transaction tr(db);
        db.exec("CREATE table testing_bind("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " intcol INTEGER,"
                " realcol REAL"
                ")");

        auto st = db.prepare("INSERT INTO testing_bind (intcol, realcol) VALUES (?, ?)");
        st.bind(nullptr);
        st.bind(std::nullptr_t{});
        auto id = st.step().getInsertId();
        auto [intVal, realVal] = db.get<std::optional<int>, std::optional<float>>("SELECT intcol, realcol FROM testing_bind WHERE id = ?", id);
        REQUIRE_FALSE(intVal.has_value());
        REQUIRE_FALSE(realVal.has_value());

        st = db.prepare("INSERT INTO testing_bind (intcol, realcol) VALUES (?, ?)");
        st.bind(intVal);
        st.bind(realVal);
        id = st.step().getInsertId();
        auto [intVal2, realVal2] = db.get<std::optional<int>, std::optional<float>>("SELECT intcol, realcol FROM testing_bind WHERE id = ?", id);
        REQUIRE_FALSE(intVal2.has_value());
        REQUIRE_FALSE(realVal2.has_value());

        st = db.prepare("INSERT INTO testing_bind (intcol, realcol) VALUES (?, ?)");
        st.bindNull();
        st.bind(13.4);
        id = st.step().getInsertId();
        auto [intVal3, realVal3] = db.get<std::optional<int>, std::optional<float>>("SELECT intcol, realcol FROM testing_bind WHERE id = ?", id);
        REQUIRE_FALSE(intVal3.has_value());
        REQUIRE(realVal3.has_value());
        REQUIRE(*realVal3 == Approx(13.4));

        st = db.prepare("INSERT INTO testing_bind (intcol, realcol) VALUES (?, ?)");
        intVal.reset();
        realVal = 3.14;
        st.bind(intVal);
        st.bind(realVal);
        id = st.step().getInsertId();
        auto [intVal4, realVal4] = db.get<std::optional<int>, std::optional<float>>("SELECT intcol, realcol FROM testing_bind WHERE id = ?", id);
        REQUIRE_FALSE(intVal4.has_value());
        REQUIRE(realVal4.has_value());
        REQUIRE(*realVal4 == Approx(3.14));
    }

    SECTION("bind(), integer values") {
        Transaction tr(db);
        db.exec("CREATE table testing_bind("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " col INTEGER"
                ")");

        auto checkBindInt = [&db](auto val) {
            using ValT = decltype(val);

            auto st = db.prepare("INSERT INTO testing_bind (col) VALUES (?)");
            st.bind(val);
            auto id = st.step().getInsertId();

            auto valFromDb = db.get<ValT>("SELECT col FROM testing_bind WHERE id = ?", id);
            REQUIRE(valFromDb == val);
        };

        checkBindInt(static_cast<short>(14));
        checkBindInt(static_cast<int>(15));
        checkBindInt(static_cast<long>(16));
        checkBindInt(static_cast<long long>(17));
        checkBindInt(static_cast<unsigned short>(14));
        checkBindInt(static_cast<unsigned int>(15));
        checkBindInt(static_cast<unsigned long>(16));
        checkBindInt(static_cast<unsigned long long>(17));
    }

    SECTION("bind(), real values") {
        Transaction tr(db);
        db.exec("CREATE table testing_bind("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " col REAL"
                ")");

        auto checkBindReal = [&db](auto val) {
          using ValT = decltype(val);

          auto st = db.prepare("INSERT INTO testing_bind (col) VALUES (?)");
          st.bind(val);
          auto id = st.step().getInsertId();

          auto valFromDb = db.get<ValT>("SELECT col FROM testing_bind WHERE id = ?", id);
          REQUIRE(valFromDb == Approx(val));
        };

        checkBindReal(static_cast<float>(17.49));
        checkBindReal(static_cast<double>(944.53));
    }

    SECTION("bind(), strings") {
        Transaction tr(db);
        db.exec("CREATE table testing_bind("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " col TEXT"
                ")");

        auto checkBind = [&db](auto val) {
          auto st = db.prepare("INSERT INTO testing_bind (col) VALUES (?)");
          st.bind(val);
          auto id = st.step().getInsertId();

          auto valFromDb = db.get<std::string>("SELECT col FROM testing_bind WHERE id = ?", id);
          REQUIRE(valFromDb == val);
        };

        checkBind("A string");
        checkBind(std::string{"Another string"});
        checkBind(std::string_view{"A third string"});
    }

    SECTION("bind(), blob") {
        Transaction tr(db);
        db.exec("CREATE table testing_bind("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " col BLOB"
                ")");

        std::vector<std::uint8_t> blob(1024);
        for (unsigned int i = 0; i < 1024; ++i) {
            blob[i] = static_cast<std::uint8_t>(i & 0xff);
        }
        auto st = db.prepare("INSERT INTO testing_bind (col) VALUES (?)");
        st.bind(blob);
        auto id = st.step().getInsertId();

        auto valFromDb = db.get<std::vector<std::uint8_t>>("SELECT col FROM testing_bind WHERE id = ?", id);
        REQUIRE(valFromDb == blob);
    }

    SECTION("reset()") {
        auto st = db.prepare("SELECT name FROM person WHERE id = ?", persons.johnDoe().id);
        auto res = st.step();
        REQUIRE(res.get<std::string>(0) == persons.johnDoe().name);

        st.reset();
        auto res2 = st.step();
        REQUIRE(res2.get<std::string>(0) == persons.johnDoe().name);

        st.reset(persons.janeDoe().id);
        auto res3 = st.step();
        REQUIRE(res3.get<std::string>(0) == persons.janeDoe().name);
    }

    SECTION("sql()") {
        auto st = db.prepare("SELECT * FROM person WHERE age = ?", persons.janeDoe().id);
        REQUIRE(st.sql() == "SELECT * FROM person WHERE age = ?");
    }
}
