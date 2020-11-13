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

#pragma once

#include <dbpp/dbpp.h>
#include <dbpp/Sqlite3.h>

class Persons {
    struct Person
    {
        std::string name;
        int age;
        std::int64_t id;
        std::optional<std::int64_t> spouseId;
    };

    Person johnDoe_{"John Doe", 48};
    Person janeDoe_{"Jane Doe", 45};
    Person andersSvensson_{"Anders Svensson", 38};

public:
    Dbpp::Connection db;

    inline static const auto Count = 3;
    inline const Person& johnDoe() { return johnDoe_; }
    inline const Person& janeDoe() { return janeDoe_; }
    inline const Person& andersSvensson() { return andersSvensson_; }

    Persons();
    void populate();
};
