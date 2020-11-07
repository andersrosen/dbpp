#include <catch2/catch.hpp>
#include <cstring>
#include <dbpp/Sqlite3.h>
#include <dbpp/dbpp.h>
#include <iostream>
#include <optional>

using namespace Dbpp;

TEST_CASE("Basic usage", "[sqlite]") {
    Connection db = Sqlite3::open(":memory:", Sqlite3::OpenMode::ReadWriteCreate);

    // FIXME: Write some good tests!
}
