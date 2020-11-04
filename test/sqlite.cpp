#include <catch2/catch.hpp>
#include <dbpp/dbpp.h>
#include <dbpp/Sqlite3.h>
#include <optional>
#include <iostream>

using namespace Dbpp;

TEST_CASE("Basic usage", "[sqlite]") {
    Connection db = Sqlite3::open(":memory:", Sqlite3::OpenMode::ReadWriteCreate);

    SECTION("Tests of the user table") {
        db.exec("CREATE TABLE user ("
                "  id   INTEGER PRIMARY KEY AUTOINCREMENT,"
                "  uid  INTEGER NOT NULL,"
                "  username TEXT NOT NULL,"
                "  full_name TEXT"
                ")");

        db.prepare("INSERT INTO user (uid, username, full_name) VALUES (?,?,?)",
                   1, "user1", "user1 full name"
        ).step();
        db.exec("INSERT INTO user (uid, username, full_name) VALUES (?,?,?)", 23, "user2", nullptr);
        db.exec("INSERT INTO user (uid, username) VALUES (?, ?)", 38, "user3");

        SECTION("Select from user table") {
            int count = db.get<int>("SELECT COUNT(*) FROM user");
            REQUIRE(count == 3);

            auto st = db.prepare("SELECT uid, username, full_name FROM user ORDER BY uid ASC");

            auto row = st.step();
            int uid = row.get<int>("uid");
            auto username = row.get<std::string>("username");
//            std::optional<std::string> full_name = row["full_name"];

            REQUIRE(uid == 1);
            REQUIRE(username == "user1");
//            REQUIRE((bool) full_name == true);
//            REQUIRE(*full_name == "user1 full name");

            row = st.step();
            uid = row.get<int>("uid");
            username = row.get<std::string>("username");
//            std::optional<std::string> full_name2 = row["full_name"];

            REQUIRE(uid == 23);
            REQUIRE(username == "user2");
//            REQUIRE((bool) full_name2 == false);

            row = st.step();
            row = st.step();
            REQUIRE(row.empty());
        }

        SECTION("Iteration over result sets") {
            int count = 0;
            for (auto &row : db.prepare("SELECT * from user WHERE uid = -1")) {
                ++count;
            }
            REQUIRE(count == 0);

            count = 0;
            std::vector<int> uids = {1, 23, 38};
            std::vector<std::string> names = {"user1", "user2", "user3"};
            for (auto &row : db.prepare("SELECT uid, username FROM user")) {
                std::string name = row.get<std::string>("username");
                int uid = row.get<int>("uid");
                REQUIRE(name == names[count]);
                REQUIRE(uid == uids[count]);
                ++count;
            }
            REQUIRE(count == 3);
        }

        SECTION("Transactions") {
            {
                Transaction t(db);

                db.exec("INSERT INTO user (uid, username) VALUES (?,?)", 145, "user145");
                t.commit();
            }
            auto username = db.get<std::string>("SELECT username FROM user WHERE uid = ?", 145);
            REQUIRE(username == "user145");

            {
                Transaction t(db);

                db.exec("INSERT INTO user (uid, username) VALUES (?,?)", 146, "user146");
            }
            int count = db.get<int>("SELECT COUNT(*) FROM user WHERE uid = ?", 146);
            REQUIRE(count == 0);
        }

        SECTION("Optional & null") {
            std::optional<std::string> null_string;

            db.exec("INSERT INTO user (uid, username, full_name) VALUES (?,?,?)", 4711, "user4", null_string);
            auto row = db.exec("SELECT uid, username, full_name FROM user WHERE uid = ?", 4711);
            auto uid = row.get<int>("uid");
            auto username = row.get<std::string>("username");
//            std::optional<std::string> full_name = row["full_name"];

            REQUIRE(uid == 4711);
            REQUIRE(username == "user4");
//            REQUIRE(!full_name.has_value());

            std::optional<std::string> non_null_string = std::string{"user5 full name"};
            db.exec("INSERT INTO user (uid, username, full_name) VALUES (?,?,?)", 4715, "user5", non_null_string);
            row = db.exec("SELECT uid, username, full_name FROM user WHERE uid = ?", 4715);

            uid = row.get<int>("uid");
            username = row.get<std::string>("username");
            // full_name = row["full_name"]; // FIXME! ambiguous overload for some reason
//            std::optional<std::string> full_name2 = row["full_name"];

            REQUIRE(uid == 4715);
            REQUIRE(username == "user5");
//            REQUIRE(full_name2.has_value());
//            REQUIRE(*full_name2 == "user5 full name");
        }
    }

    SECTION("Value type tests") {
        Connection db = Sqlite3::open(":memory:", Sqlite3::OpenMode::ReadWriteCreate);

        db.exec("CREATE TABLE typetest ("
                "  id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                "  intcol INTEGER,"
                "  realcol REAL,"
                "  textcol TEXT,"
                "  blobcol BLOB"
                ")");

        THEN("We can insert a row and get the ID of it") {
            auto id = db.exec("INSERT INTO typetest (realcol, textcol) VALUES (?,?)", 2.3, "abc").getInsertId();
            REQUIRE(id == 1);
        }

        SECTION("Retrieve to native types, and std::string") {
            auto id = db.exec("INSERT INTO typetest (intcol, realcol, textcol) VALUES (?,?,?)", 14, 2.3, "abc").getInsertId();

            short s = db.get<short>("SELECT intcol FROM typetest WHERE id = ?", id);
            REQUIRE(s == 14);
            int i = db.get<int>("SELECT intcol FROM typetest WHERE id = ?", id);
            REQUIRE(i == 14);
            long l = db.get<long>("SELECT intcol FROM typetest WHERE id = ?", id);
            REQUIRE(l == 14);
            long long ll = db.get<long long>("SELECT intcol FROM typetest WHERE id = ?", id);
            REQUIRE(ll == 14);
            unsigned short us = db.get<unsigned short>("SELECT intcol FROM typetest WHERE id = ?", id);
            REQUIRE(us == 14);
            unsigned int ui = db.get<unsigned int>("SELECT intcol FROM typetest WHERE id = ?", id);
            REQUIRE(ui == 14);
            unsigned long ul = db.get<unsigned long>("SELECT intcol FROM typetest WHERE id = ?", id);
            REQUIRE(ul == 14);
            unsigned long long ull = db.get<unsigned long long>("SELECT intcol FROM typetest WHERE id = ?", id);
            REQUIRE(ull == 14);
            float f = db.get<float>("SELECT realcol FROM typetest WHERE id = ?", id);
            REQUIRE(f == Approx(2.3));
            double d = db.get<double>("SELECT realcol FROM typetest WHERE id = ?", id);
            REQUIRE(d == Approx(2.3));
            std::string t = db.get<std::string>("SELECT textcol FROM typetest WHERE id = ?", id);
            REQUIRE(t == "abc");
        }

        SECTION("Insert & get blob") {
            const int num_bytes = 2048;
            std::vector<unsigned char> vec;
            vec.reserve(num_bytes);
            for (int i=0; i<num_bytes; ++i) {
                vec.push_back(i & 255);
            }
            auto id = db.exec("INSERT INTO typetest (blobcol) VALUES (?)", vec).getInsertId();

            std::vector<unsigned char> vecout = db.get<std::vector<unsigned char>>("SELECT blobcol FROM typetest WHERE id = ?", id);
            for (int i=0; i<num_bytes; ++i) {
                REQUIRE(vecout[i] == (i & 255));
            }
        }
    }

}
