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
#include <numeric>

using namespace Dbpp;

TEST_CASE("StatementBuilder", "[api]") {
    Persons persons;
    Connection& db = persons.db;
    persons.populate();

    SECTION("Create Statement from builder") {
        {
            StatementBuilder builder("SELECT count(*) FROM person");
            auto st = db.statement(builder);
            REQUIRE(st.step().get<int>(0) == persons.Count);

            builder.append(" WHERE age < ? AND age > ?", 48, 40);
            auto st2 = db.statement(builder);
            REQUIRE(st2.step().get<int>(0) == 1);
        }

        {
            StatementBuilder builder("SELECT count(*) FROM PERSON WHERE age < ?", 48);
            auto st = db.statement(builder);
            REQUIRE(st.step().get<int>(0) == 2);

            builder.append(" AND");
            builder.append(" age > ?", 40);
            auto st2 = db.statement(builder);
            REQUIRE(st2.step().get<int>(0) == 1);
        }
    }

    SECTION("Create PreparedStatement from builder") {
        StatementBuilder builder("SELECT count(*) FROM person");
        auto st = db.preparedStatement(builder);
        REQUIRE(st.step().get<int>(0) == persons.Count);

        builder.append(" WHERE age < ? AND age > ?", 48, 40);
        auto st2 = db.preparedStatement(builder);
        REQUIRE(st2.step().get<int>(0) == 1);

        st2.rebind(100, 40);
        REQUIRE(st2.step().get<int>(0) == 2);
    }

    SECTION("Custom type") {
        class MyCustomType {
            std::string str_;

            public:
            explicit MyCustomType(std::string_view str)
                : str_(str)
            {}

            void dbppBind(PlaceholderBinder &binder) const {
                binder.bind(str_);
            }
        };

        {
            MyCustomType custom{ persons.johnDoe().name };
            StatementBuilder builder("SELECT COUNT(*) FROM person WHERE name = ?", custom);
            auto st = db.statement(builder);
            REQUIRE(st.step().get<int>(0) == 1);
        }

        {
            MyCustomType custom{ persons.johnDoe().name };
            StatementBuilder builder("SELECT COUNT(*) FROM person");
            builder.append(" WHERE name = ?", custom);
            auto st = db.statement(builder);
            REQUIRE(st.step().get<int>(0) == 1);
        }
    }

    SECTION("Blob") {
        Transaction tr(db);
        db.exec("CREATE table testing_bind("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " col BLOB"
                ")");

        std::vector<std::uint8_t> blob(1024);
        std::iota(blob.begin(), blob.end(), 0);

        {
            StatementBuilder builder("INSERT INTO testing_bind (col) VALUES (?)", blob);
            auto st = db.statement(builder);
            auto id = st.step().getInsertId();

            auto valFromDb = db.get<std::vector<std::uint8_t>>("SELECT col FROM testing_bind WHERE id = ?", id);
            REQUIRE(valFromDb == blob);
        }

        {
            StatementBuilder builder("INSERT INTO testing_bind ");
            builder.append("(col) VALUES (?)", blob);
            auto st = db.statement(builder);
            auto id = st.step().getInsertId();

            auto valFromDb = db.get<std::vector<std::uint8_t>>("SELECT col FROM testing_bind WHERE id = ?", id);
            REQUIRE(valFromDb == blob);
        }
    }
}

