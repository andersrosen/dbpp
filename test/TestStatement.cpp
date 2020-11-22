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
        Statement st = db.statement("SELECT COUNT(*) FROM person WHERE id = ? OR id = ?");
        st.bind(persons.johnDoe().id);
        Statement newStatement{std::move(st)};
        newStatement.bind(persons.janeDoe().id);
        auto result = newStatement.step();
        REQUIRE(result);
        REQUIRE(result.get<int>(0) == 2);

        st = db.statement("SELECT SUM(age) FROM person WHERE id = ? OR id = ?");
        st.bind(persons.johnDoe().id);
        newStatement = std::move(st);
        newStatement.bind(persons.janeDoe().id);
        result = newStatement.step();
        REQUIRE(result);
        REQUIRE(result.get<int>(0) == persons.johnDoe().age + persons.janeDoe().age);
    }

    SECTION("Statement begin(), end()") {
        Statement st = db.statement("SELECT * FROM person WHERE id IS NULL"); // no hits - id column defined as NOT NULL
        Statement::iterator beginning = st.begin();
        Statement::iterator end = st.end();
        REQUIRE(beginning == end);

        st = db.statement("SELECT * FROM person WHERE id = ?", persons.johnDoe().id);
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

        auto st = db.statement("INSERT INTO testing_bind (intcol, realcol) VALUES (?, ?)");
        st.bind(nullptr);
        st.bind(std::nullptr_t{});
        auto id = st.step().getInsertId();
        auto [intVal, realVal] = db.get<std::optional<int>, std::optional<float>>("SELECT intcol, realcol FROM testing_bind WHERE id = ?", id);
        REQUIRE_FALSE(intVal.has_value());
        REQUIRE_FALSE(realVal.has_value());

        st = db.statement("INSERT INTO testing_bind (intcol, realcol) VALUES (?, ?)");
        st.bind(intVal);
        st.bind(realVal);
        id = st.step().getInsertId();
        auto [intVal2, realVal2] = db.get<std::optional<int>, std::optional<float>>("SELECT intcol, realcol FROM testing_bind WHERE id = ?", id);
        REQUIRE_FALSE(intVal2.has_value());
        REQUIRE_FALSE(realVal2.has_value());

        st = db.statement("INSERT INTO testing_bind (intcol, realcol) VALUES (?, ?)");
        st.bindNull();
        st.bind(13.4);
        id = st.step().getInsertId();
        auto [intVal3, realVal3] = db.get<std::optional<int>, std::optional<float>>("SELECT intcol, realcol FROM testing_bind WHERE id = ?", id);
        REQUIRE_FALSE(intVal3.has_value());
        REQUIRE(realVal3.has_value());
        REQUIRE(*realVal3 == Approx(static_cast<float>(13.4)));

        st = db.statement("INSERT INTO testing_bind (intcol, realcol) VALUES (?, ?)");
        intVal.reset();
        realVal = static_cast<float>(3.14);
        st.bind(intVal);
        st.bind(realVal);
        id = st.step().getInsertId();
        auto [intVal4, realVal4] = db.get<std::optional<int>, std::optional<float>>("SELECT intcol, realcol FROM testing_bind WHERE id = ?", id);
        REQUIRE_FALSE(intVal4.has_value());
        REQUIRE(realVal4.has_value());
        REQUIRE(*realVal4 == Approx(static_cast<float>(3.14)));
    }

    SECTION("bind(), integer values") {
        Transaction tr(db);
        db.exec("CREATE table testing_bind("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " col INTEGER"
                ")");

        auto checkBindInt = [&db](auto val) {
            using ValT = decltype(val);

            auto st = db.statement("INSERT INTO testing_bind (col) VALUES (?)");
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

          auto st = db.statement("INSERT INTO testing_bind (col) VALUES (?)");
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
          auto st = db.statement("INSERT INTO testing_bind (col) VALUES (?)");
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
        auto st = db.statement("INSERT INTO testing_bind (col) VALUES (?)");
        st.bind(blob);
        auto id = st.step().getInsertId();

        auto valFromDb = db.get<std::vector<std::uint8_t>>("SELECT col FROM testing_bind WHERE id = ?", id);
        REQUIRE(valFromDb == blob);
    }

    SECTION("bind(), custom type") {
        class MyCustomType {
            std::string str_;

            public:
            explicit MyCustomType(std::string_view str)
            : str_(str)
            {}

            void dbppBind(Statement &st) const {
                st.bind(str_);
            }
        };

        MyCustomType custom{persons.johnDoe().name};
        auto st = db.statement("SELECT COUNT(*) FROM person WHERE name = ?");
        st.bind(custom);
        auto res = st.step();
        int count = res.get<int>(0);
        REQUIRE(count == 1);
    }

    SECTION("bind(), optional custom type") {
        class MyCustomId {
            std::int64_t id_;

            public:
            explicit MyCustomId(std::int64_t id)
                : id_(id)
            {}

            void dbppBind(Statement &st) const {
                st.bind(id_);
            }
        };

        std::optional<MyCustomId> unsetId;
        auto st1 = db.statement("SELECT COUNT(*) FROM person WHERE id = ?");
        st1.bind(unsetId);
        auto res = st1.step();
        int count = res.get<int>(0);
        REQUIRE(count == 0);

        std::optional<MyCustomId> setId{persons.johnDoe().id};
        auto st2 = db.statement("SELECT COUNT(*) FROM person WHERE id = ?");
        st2.bind(setId);
        res = st2.step();
        count = res.get<int>(0);
        REQUIRE(count == 1);
    }

    SECTION("sql()") {
        auto st = db.statement("SELECT * FROM person WHERE age = ?", persons.janeDoe().id);
        REQUIRE(st.sql() == "SELECT * FROM person WHERE age = ?");
    }
}

TEST_CASE("Statement iteration", "[api]") {
    Persons persons;
    persons.populate();
    Connection& db = persons.db;

    const auto expectedTotalAge =
        persons.johnDoe().age
        + persons.janeDoe().age
        + persons.andersSvensson().age;

    const auto expectedConcatenatedNames =
        persons.johnDoe().name
        + persons.janeDoe().name
        + persons.andersSvensson().name;

    SECTION("range-for as Result objects") {
        int rowCount = 0;
        int totalAge = 0;
        std::string concatenatedNames;
        auto st = db.statement("SELECT * FROM person ORDER BY id ASC");
        for (auto& row : st) {
            ++rowCount;
            totalAge += row.get<int>("age");
            concatenatedNames += row.get<std::string>("name");
        }
        REQUIRE(rowCount == persons.Count);
        REQUIRE(totalAge == expectedTotalAge);
        REQUIRE(concatenatedNames == expectedConcatenatedNames);
    }

    SECTION("Empty range-for as Result objects") {
        int rowCount = 0;
        auto st = db.statement("SELECT * FROM person WHERE name = ?", "There is no one with this name");
        for (auto& row : st)
            ++rowCount;
        REQUIRE(rowCount == 0);
    }

    SECTION("Default-constructed StatementIterator is the end iterator") {
        StatementIterator endIterator;
        Statement noMatches = db.statement("SELECT * FROM person WHERE name = ?", "There is no one with this name");

        REQUIRE(noMatches.begin() == endIterator);
        REQUIRE(noMatches.end() == endIterator);
    }

    SECTION("Dereference StatementIterator") {
        Statement st = db.statement("SELECT * FROM person WHERE id = ?", persons.johnDoe().id);
        auto it = st.begin();
        REQUIRE(it->get<int>("age") == persons.johnDoe().age);
        auto &row = *it;
        REQUIRE(row.get<std::string>("name") == persons.johnDoe().name);
    }

    SECTION("range-for as tuples") {
        int rowCount = 0;
        int totalAge = 0;
        std::string concatenatedNames;
        auto st = db.statement("SELECT name, age FROM person ORDER BY id ASC");
        for (const auto &[name, age] : std::move(st).as<std::string, int>()) {
            ++rowCount;
            totalAge += age;
            concatenatedNames += name;
        }
        REQUIRE(rowCount == persons.Count);
        REQUIRE(totalAge == expectedTotalAge);
        REQUIRE(concatenatedNames == expectedConcatenatedNames);
    }

    SECTION("Empty range-for as tuples") {
        int rowCount = 0;
        auto st = db.statement("SELECT name, age FROM person WHERE name = ?", "There is no one with this name");
        for (const auto& [name, age] : std::move(st).as<std::string, int>())
            ++rowCount;
        REQUIRE(rowCount == 0);
    }

    SECTION("Default-constructed StatementTupleIterator is the end iterator") {
        StatementTupleIterator<std::string, int> endIterator;
        auto noMatches = db.statement("SELECT name, age FROM person WHERE name = ?", "There is no one with this name").as<std::string, int>();

        REQUIRE(noMatches.begin() == endIterator);
        REQUIRE(noMatches.end() == endIterator);
    }

    SECTION("Dereference StatementIterator") {
        auto st = db.statement("SELECT name, age FROM person WHERE id = ?", persons.johnDoe().id).as<std::string, int>();
        auto it = st.begin();

        const auto &t = *it;
        REQUIRE(std::get<0>(t) == persons.johnDoe().name);

        // Dereferencing using operator -> can't be tested, since the only
        // members in std::tuple are assignment and swap, and they require the
        // tuple to not be const
    }
}

TEST_CASE("PreparedStatement", "[api]") {
    Persons persons;
    persons.populate();
    Connection& db = persons.db;

    SECTION("reset()") {
        auto st = db.preparedStatement("SELECT name FROM person WHERE id = ?");
        st.rebind(persons.johnDoe().id);
        auto res = st.step();
        REQUIRE(res.get<std::string>(0) == persons.johnDoe().name);

        st.reset();
        auto res2 = st.step();
        REQUIRE(res2.get<std::string>(0) == persons.johnDoe().name);

        st.rebind(persons.janeDoe().id);
        auto res3 = st.step();
        REQUIRE(res3.get<std::string>(0) == persons.janeDoe().name);
    }
}
