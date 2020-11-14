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

Persons::Persons()
: db(Dbpp::Sqlite3::open(":memory:"))
{}

void
Persons::populate() {
    db.exec("CREATE TABLE person ("
            " id INTEGER PRIMARY KEY AUTOINCREMENT,"
            " name TEXT NOT NULL,"
            " age INTEGER NOT NULL,"
            " spouse_id INTEGER REFERENCES person(id)"
            ")");

    johnDoe_.id = db.exec("INSERT INTO person (name, age) VALUES ('John Doe', 48)").getInsertId();
    janeDoe_.id = db.exec("INSERT INTO person (name, age) VALUES ('Jane Doe', 45)").getInsertId();
    andersSvensson_.id = db.exec("INSERT INTO person (name, age) VALUES ('Anders Svensson', 38)").getInsertId();
    db.exec("UPDATE person set spouse_id = ? WHERE id = ?", janeDoe_.id, johnDoe_.id);
    db.exec("UPDATE person set spouse_id = ? WHERE id = ?", johnDoe_.id, janeDoe_.id);
    johnDoe_.spouseId = janeDoe_.id;
    janeDoe_.spouseId = johnDoe_.id;
}
