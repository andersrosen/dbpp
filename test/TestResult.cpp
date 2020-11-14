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

class MyCustomId {
    std::int64_t id_;

    explicit MyCustomId(std::int64_t id)
    : id_(id)
    {}

public:
    static MyCustomId dbppGet(Result& res, int columnIndex) {
        return MyCustomId(res.get<int>(columnIndex));
    }

    static const MyCustomId DefaultId;

    [[nodiscard]]
    std::int64_t getId() const { return id_; }
};

const MyCustomId MyCustomId::DefaultId(10000);

TEST_CASE("Result", "[api]") {
    Persons persons;
    Connection& db = persons.db;
    persons.populate();

    SECTION("empty()") {
        Result res1;
        REQUIRE(res1.empty());

        Result res2 = db.exec("SELECT COUNT(*) FROM person");
        REQUIRE_FALSE(res2.empty());
    }

    SECTION("isNull()") {
        auto empty = db.exec("SELECT age, spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
        auto res = std::move(empty);
        REQUIRE_FALSE(res.empty());

        REQUIRE_FALSE(res.isNull(0));
        REQUIRE(res.get<int>(0) == persons.andersSvensson().age);
        REQUIRE(res.isNull(1));

        REQUIRE_FALSE(res.isNull("age"));
        REQUIRE(res.get<int>("age") == persons.andersSvensson().age);
        REQUIRE(res.isNull("spouse_id"));

        REQUIRE_THROWS_AS(empty.isNull(0), Error); // NOLINT - intentional use after move
        REQUIRE_THROWS_AS(empty.isNull("age"), Error); // NOLINT - intentional use after move
    }

    SECTION("columnCount()") {
        Result empty = db.exec("SELECT age FROM person");
        Result res = std::move(empty);

        REQUIRE_THROWS_AS(empty.columnCount(), Error); // NOLINT - intentional use after move
        REQUIRE(res.columnCount() == 1);

        res = db.exec("SELECT age, name FROM person");
        REQUIRE_FALSE(res.empty());
        REQUIRE(res.columnCount() == 2);
    }

    SECTION("columnName()") {
        Result empty = db.exec("SELECT id AS person_id, age, name FROM person");
        Result res = std::move(empty);

        REQUIRE_THROWS_AS(empty.columnName(0), Error); // NOLINT - intentional use after move
        REQUIRE(res.columnName(0) == "person_id");
        REQUIRE(res.columnName(1) == "age");
        REQUIRE(res.columnName(2) == "name");
    }

    SECTION("hasColumn()") {
        Result empty = db.exec("SELECT id AS person_id, age FROM person");
        Result res = std::move(empty);

        REQUIRE_FALSE(empty.hasColumn("age")); // NOLINT - intentional use after move
        REQUIRE(res.hasColumn("person_id"));
        REQUIRE(res.hasColumn("age"));
        REQUIRE_FALSE(res.hasColumn("name"));
    }

    SECTION("columnIndex()") {
        Result empty = db.exec("SELECT id AS person_id, age, name FROM person");
        Result res = std::move(empty);

        REQUIRE_THROWS_AS(empty.columnIndex("age"), Error); // NOLINT - intentional use after move
        REQUIRE(res.columnIndex("person_id") == 0);
        REQUIRE(res.columnIndex("age") == 1);
        REQUIRE(res.columnIndex("name") == 2);
        REQUIRE_THROWS_AS(res.columnIndex("does_not_exist"), Error);
    }

    SECTION("Move semantics") {
        Result res1 = db.exec("SELECT * FROM person WHERE id = ?", persons.johnDoe().id);
        REQUIRE(res1.get<int>("age") == persons.johnDoe().age);
        Result res2{std::move(res1)};
        REQUIRE_FALSE(res2.empty());
        REQUIRE(res1.empty()); // NOLINT - intentional use after move
        REQUIRE(res2.get<int>("age") == persons.johnDoe().age);

        Result res3;
        REQUIRE(res3.empty());
        res3 = std::move(res1);
        REQUIRE(res3.empty());
        res3 = std::move(res2);
        REQUIRE_FALSE(res3.empty());
    }

    SECTION("get(i, out)") {
        Transaction tr(db);
        db.exec("CREATE TABLE get_test ("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " intval INTEGER,"
                " realval REAL,"
                " strval TEXT,"
                " blobval BLOB"
                ")");

        const int intVal = 14;
        const float realVal = 3.14;
        const std::string strVal = "/tmp/a/string/thats/also/a/path";
        std::vector<std::uint8_t> blobVal(1024);
        std::iota(blobVal.begin(), blobVal.end(), 0);

        const auto intId = db.exec("INSERT INTO get_test (intval) VALUES (?)", intVal).getInsertId();
        const auto realId = db.exec("INSERT INTO get_test (realval) VALUES (?)", realVal).getInsertId();
        const auto strId = db.exec("INSERT INTO get_test (strval) VALUES (?)", strVal).getInsertId();
        const auto blobId = db.exec("INSERT INTO get_test (blobval) VALUES (?)", blobVal).getInsertId();

        SECTION("Return false if value is null, and don't modify the output variable")
        {
            // Select the column with id = realId to make sure we get a null result
            auto intRes = db.exec("SELECT intval FROM get_test WHERE id = ?", realId);
            REQUIRE_FALSE(intRes.empty());

            short shortOut = intVal * 2;
            REQUIRE_FALSE(intRes.get(0, shortOut));
            REQUIRE(shortOut == intVal * 2);

            int intOut = intVal * 2;
            REQUIRE_FALSE(intRes.get(0, intOut));
            REQUIRE(intOut == intVal * 2);

            long longOut = intVal * 2;
            REQUIRE_FALSE(intRes.get(0, longOut));
            REQUIRE(longOut == intVal * 2);

            long long longlongOut = intVal * 2;
            REQUIRE_FALSE(intRes.get(0, longlongOut));
            REQUIRE(longlongOut == intVal * 2);

            unsigned short ushortOut = intVal * 2;
            REQUIRE_FALSE(intRes.get(0, ushortOut));
            REQUIRE(ushortOut == intVal * 2);

            unsigned int uintOut = intVal * 2;
            REQUIRE_FALSE(intRes.get(0, uintOut));
            REQUIRE(uintOut == intVal * 2);

            unsigned long ulongOut = intVal * 2;
            REQUIRE_FALSE(intRes.get(0, ulongOut));
            REQUIRE(ulongOut == intVal * 2);

            unsigned long long ulonglongOut = intVal * 2;
            REQUIRE_FALSE(intRes.get(0, ulonglongOut));
            REQUIRE(ulonglongOut == intVal * 2);

            // Select the column with id = intId to make sure we get a null result
            auto realRes = db.exec("SELECT realval FROM get_test WHERE id = ?", intId);
            REQUIRE_FALSE(realRes.empty());

            float floatOut = realVal * 2;
            REQUIRE_FALSE(realRes.get(0, floatOut));
            REQUIRE(floatOut == realVal * 2);

            float doubleOut = realVal * 2;
            REQUIRE_FALSE(realRes.get(0, doubleOut));
            REQUIRE(doubleOut == realVal * 2);

            // Select the column with id = intId to make sure we get a null result
            auto strRes = db.exec("SELECT strval FROM get_test WHERE id = ?", intId);
            REQUIRE_FALSE(strRes.empty());

            std::string strOut = strVal + strVal;
            REQUIRE_FALSE(strRes.get(0, strOut));
            REQUIRE(strOut == strVal + strVal);

            std::filesystem::path pathOut = "/tmp/foo/bar.txt";
            REQUIRE_FALSE(strRes.get(0, pathOut));
            REQUIRE(pathOut == "/tmp/foo/bar.txt");

            // Select the column with id = intId to make sure we get a null result
            auto blobRes = db.exec("SELECT blobval FROM get_test WHERE id = ?", intId);
            REQUIRE_FALSE(blobRes.empty());

            std::vector<std::uint8_t> blobOut(10);
            std::iota(blobOut.begin(), blobOut.end(), 0);
            REQUIRE_FALSE(blobRes.get(0, blobOut));
            REQUIRE(blobOut == std::vector<std::uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
        }

        SECTION("Return true if value is not null, and set the output variable")
        {
            auto intRes = db.exec("SELECT intval FROM get_test WHERE id = ?", intId);
            REQUIRE_FALSE(intRes.empty());

            short shortOut = intVal * 2;
            REQUIRE(intRes.get(0, shortOut) == true);
            REQUIRE(shortOut == intVal);

            int intOut = intVal * 2;
            REQUIRE(intRes.get(0, intOut) == true);
            REQUIRE(intOut == intVal);

            long longOut = intVal * 2;
            REQUIRE(intRes.get(0, longOut) == true);
            REQUIRE(longOut == intVal);

            long long longlongOut = intVal * 2;
            REQUIRE(intRes.get(0, longlongOut) == true);
            REQUIRE(longlongOut == intVal);

            unsigned short ushortOut = intVal * 2;
            REQUIRE(intRes.get(0, ushortOut) == true);
            REQUIRE(ushortOut == intVal);

            unsigned int uintOut = intVal * 2;
            REQUIRE(intRes.get(0, uintOut) == true);
            REQUIRE(uintOut == intVal);

            unsigned long ulongOut = intVal * 2;
            REQUIRE(intRes.get(0, ulongOut) == true);
            REQUIRE(ulongOut == intVal);

            unsigned long long ulonglongOut = intVal * 2;
            REQUIRE(intRes.get(0, ulonglongOut) == true);
            REQUIRE(ulonglongOut == intVal);

            auto realRes = db.exec("SELECT realval FROM get_test WHERE id = ?", realId);
            REQUIRE_FALSE(realRes.empty());

            float floatOut = realVal * 2;
            REQUIRE(realRes.get(0, floatOut) == true);
            REQUIRE(floatOut == realVal);

            float doubleOut = realVal * 2;
            REQUIRE(realRes.get(0, doubleOut) == true);
            REQUIRE(doubleOut == realVal);

            auto strRes = db.exec("SELECT strval FROM get_test WHERE id = ?", strId);
            REQUIRE_FALSE(strRes.empty());

            std::string strOut = strVal + strVal;
            REQUIRE(strRes.get(0, strOut) == true);
            REQUIRE(strOut == strVal);

            std::filesystem::path pathOut = "/tmp/foo/bar.txt";
            REQUIRE(strRes.get(0, pathOut) == true);
            REQUIRE(pathOut == strVal);

            auto blobRes = db.exec("SELECT blobval FROM get_test WHERE id = ?", blobId);
            REQUIRE_FALSE(blobRes.empty());

            std::vector<std::uint8_t> blobOut(10);
            std::iota(blobOut.begin(), blobOut.end(), 0);
            REQUIRE(blobRes.get(0, blobOut) == true);
            REQUIRE(blobOut == blobVal);
        }

        SECTION("Throw if empty or getting column that doesn't exist") {
            auto empty = db.exec("SELECT intval, realval FROM get_test WHERE id = ?", intId);
            auto res = std::move(empty);
            REQUIRE_FALSE(res.empty());

            int intOut = 0;
            REQUIRE_THROWS_AS(empty.get(0, intOut), Error); // NOLINT - intentional use after move
            REQUIRE_THROWS_AS(res.get(2, intOut), Error);
        }
    }

    SECTION("get(i) and getOptional(i)") {
        SECTION("get(i), standard types") {
            auto res = db.exec("SELECT id, name, spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
            REQUIRE_FALSE(res.empty());

            auto id = res.get<int>(0);
            REQUIRE(id == persons.andersSvensson().id);
            id = res.get<int>("id");
            REQUIRE(id == persons.andersSvensson().id);
            auto name = res.get<std::string>(1);
            REQUIRE(name == persons.andersSvensson().name);
            name = res.get<std::string>("name");
            REQUIRE(name == persons.andersSvensson().name);

            REQUIRE_THROWS_AS(res.get<int>(2), Error); // Column 2 is null
            REQUIRE_THROWS_AS(res.get<int>(4), Error); // Column 4 doesn't exist
            REQUIRE_THROWS_AS(res.get<int>("spouse_id"), Error); // null
            REQUIRE_THROWS_AS(res.get<int>("no column with this name"), Error);
        }

        SECTION("get(i), std::optional") {
            auto res = db.exec("SELECT id, name, spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
            REQUIRE_FALSE(res.empty());

            auto maybeId = res.get<std::optional<int>>(0);
            REQUIRE(maybeId.has_value());
            REQUIRE(*maybeId == persons.andersSvensson().id);
            maybeId = res.get<std::optional<int>>("id");
            REQUIRE(maybeId.has_value());
            REQUIRE(*maybeId == persons.andersSvensson().id);

            auto maybeName = res.get<std::optional<std::string>>(1);
            REQUIRE(maybeName.has_value());
            REQUIRE(maybeName == persons.andersSvensson().name);
            maybeName = res.get<std::optional<std::string>>("name");
            REQUIRE(maybeName.has_value());
            REQUIRE(maybeName == persons.andersSvensson().name);

            REQUIRE_THROWS_AS(res.get<std::optional<int>>(4), Error); // Column 4 doesn't exist
            REQUIRE_THROWS_AS(res.get<std::optional<int>>("no column with this name"), Error);
        }

        SECTION("getOptional(i), standard types") {
            auto res = db.exec("SELECT id, name, spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
            REQUIRE_FALSE(res.empty());

            auto maybeId = res.getOptional<int>(0);
            REQUIRE(maybeId.has_value());
            REQUIRE(*maybeId == persons.andersSvensson().id);
            maybeId = res.getOptional<int>("id");
            REQUIRE(maybeId.has_value());
            REQUIRE(*maybeId == persons.andersSvensson().id);

            auto maybeName = res.getOptional<std::string>(1);
            REQUIRE(maybeName.has_value());
            REQUIRE(*maybeName == persons.andersSvensson().name);
            maybeName = res.getOptional<std::string>("name");
            REQUIRE(maybeName == persons.andersSvensson().name);

            auto maybeSpouseId = res.getOptional<int>(2);
            REQUIRE_FALSE(maybeSpouseId.has_value());
            maybeSpouseId = res.getOptional<int>("spouse_id");
            REQUIRE_FALSE(maybeSpouseId.has_value());

            REQUIRE_THROWS_AS(res.getOptional<int>(4), Error); // Column 4 doesn't exist
            REQUIRE_THROWS_AS(res.getOptional<int>("no column with this name"), Error);
        }

        SECTION("get(i), custom type") {
            auto empty = db.exec("SELECT id, spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
            auto res = std::move(empty);
            REQUIRE_FALSE(res.empty());

            auto customId = res.get<MyCustomId>(0);
            REQUIRE(customId.getId() == persons.andersSvensson().id);

            customId = res.get<MyCustomId>("id");
            REQUIRE(customId.getId() == persons.andersSvensson().id);

            REQUIRE_THROWS_AS(res.get<MyCustomId>(1), Error); // Column 1 is null
            REQUIRE_THROWS_AS(res.get<MyCustomId>(4), Error); // Column 4 doesn't exist
            REQUIRE_THROWS_AS(res.get<MyCustomId>("spouse_id"), Error); // null
            REQUIRE_THROWS_AS(res.get<MyCustomId>("no column with this name"), Error);

            auto maybeCustomId = res.get<std::optional<MyCustomId>>(0);
            REQUIRE(maybeCustomId.has_value());
            REQUIRE(maybeCustomId->getId() == persons.andersSvensson().id);

            maybeCustomId = res.get<std::optional<MyCustomId>>("id");
            REQUIRE(maybeCustomId.has_value());
            REQUIRE(maybeCustomId->getId() == persons.andersSvensson().id);

            maybeCustomId = res.get<std::optional<MyCustomId>>("spouse_id");
            REQUIRE_FALSE(maybeCustomId.has_value());

            REQUIRE_THROWS_AS(res.get<std::optional<MyCustomId>>(4), Error); // Column 4 doesn't exist
            REQUIRE_THROWS_AS(res.get<std::optional<MyCustomId>>("no column with this name"), Error);

            REQUIRE_THROWS_AS(empty.get<MyCustomId>(0), Error); // NOLINT - intentional use after move
        }

        SECTION("getOptional(i), custom type") {
            auto res = db.exec("SELECT id, spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
            REQUIRE_FALSE(res.empty());

            auto maybeCustomId = res.getOptional<MyCustomId>(0);
            REQUIRE(maybeCustomId.has_value());
            REQUIRE(maybeCustomId->getId() == persons.andersSvensson().id);

            maybeCustomId = res.getOptional<MyCustomId>("id");
            REQUIRE(maybeCustomId.has_value());
            REQUIRE(maybeCustomId->getId() == persons.andersSvensson().id);

            maybeCustomId = res.getOptional<MyCustomId>(1);
            REQUIRE_FALSE(maybeCustomId.has_value());

            maybeCustomId = res.getOptional<MyCustomId>("spouse_id");
            REQUIRE_FALSE(maybeCustomId.has_value());

            REQUIRE_THROWS_AS(res.getOptional<MyCustomId>(4), Error); // Column 4 doesn't exist
            REQUIRE_THROWS_AS(res.getOptional<MyCustomId>("no column with this name"), Error);
        }
    }

    SECTION("valueOr") {
        auto res = db.exec("SELECT id, name, spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);
        REQUIRE_FALSE(res.empty());

        REQUIRE(res.valueOr<int>(0, 10000) == persons.andersSvensson().id);
        REQUIRE(res.valueOr<std::string>(1, "A default string") == persons.andersSvensson().name);
        REQUIRE(res.valueOr<int>(2, 10000) == 10000);

        REQUIRE(res.valueOr<MyCustomId>(0, MyCustomId::DefaultId).getId() == persons.andersSvensson().id);
        REQUIRE(res.valueOr<MyCustomId>(2, MyCustomId::DefaultId).getId() == 10000);
    }

    SECTION("toTuple") {
        auto res = db.exec("SELECT id, name, spouse_id FROM person WHERE id = ?", persons.andersSvensson().id);

        {
            const auto& [id, name, maybeSpouseId] = res.toTuple<int, std::string, std::optional<MyCustomId>>();
            REQUIRE(id == persons.andersSvensson().id);
            REQUIRE(name == persons.andersSvensson().name);
            REQUIRE_FALSE(maybeSpouseId.has_value());
        }

        {
            auto [id, name, maybeSpouseId] = res.toTuple<int, std::string, std::optional<MyCustomId>>();
            REQUIRE(id == persons.andersSvensson().id);
            REQUIRE(name == persons.andersSvensson().name);
            REQUIRE_FALSE(maybeSpouseId.has_value());
        }

        res = db.exec("SELECT id, name, spouse_id FROM person WHERE id = ?", persons.johnDoe().id);

        {
            const auto& [id, name, maybeSpouseId] = res.toTuple<int, std::string, std::optional<MyCustomId>>();
            REQUIRE(id == persons.johnDoe().id);
            REQUIRE(name == persons.johnDoe().name);
            REQUIRE(maybeSpouseId.has_value());
            REQUIRE(maybeSpouseId->getId() == *persons.johnDoe().spouseId);
        }

        {
            auto [id, name, maybeSpouseId] = res.toTuple<int, std::string, std::optional<MyCustomId>>();
            REQUIRE(id == persons.johnDoe().id);
            REQUIRE(name == persons.johnDoe().name);
            REQUIRE(maybeSpouseId.has_value());
            REQUIRE(maybeSpouseId->getId() == *persons.johnDoe().spouseId);
        }
    }

    SECTION("Conversion operator, to tuple") {
        auto foo = [&](const std::tuple<int, std::string>& idAndName) {
            const auto &[id, name] = idAndName;
            REQUIRE(id == persons.johnDoe().id);
            REQUIRE(name == persons.johnDoe().name);
        };

        auto res = db.exec("SELECT id, name FROM person WHERE id = ?", persons.johnDoe().id);
        foo(res);

        auto [id, name] = static_cast<std::tuple<int, std::string>>(res);
        REQUIRE(id == persons.johnDoe().id);
        REQUIRE(name == persons.johnDoe().name);
    }
}
