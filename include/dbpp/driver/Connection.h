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

#include "../Connection.h"
#include <string>
#include "Types.h"

namespace Dbpp::Driver {

/// Interface class for the connection of a driver
class Connection {
    public:
    /// Creates a new statement for the supplied string
    virtual StatementPtr prepare(const std::string &sql) = 0;

    /// Begins a transaction
    virtual void begin() = 0;

    /// Commits a transaction
    virtual void commit() = 0;

    /// Rolls back a transaction
    virtual void rollback() = 0;

    /// Returns the name of the driver
    virtual const std::string &driverName() const = 0;

    inline static ConnectionPtr getImpl(Dbpp::Connection& db) {
        return db.impl;
    }
};

}
