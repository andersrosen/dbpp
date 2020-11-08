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
#include "Types.h"

#include <string_view>

namespace Dbpp::Adapter {

/// \brief Abstract base class for the connection of a driver
///
/// \since v1.0.0
class Connection {
public:
    /// \brief Creates a new statement for the supplied string
    ///
    /// \since v1.0.0
    virtual StatementPtr prepare(std::string_view sql) = 0;

    /// \brief Begins a transaction
    ///
    /// \since v1.0.0
    virtual void begin() = 0;

    /// \brief Commits a transaction
    ///
    /// \since v1.0.0
    virtual void commit() = 0;

    /// \brief Rolls back a transaction
    ///
    /// \since v1.0.0
    virtual void rollback() = 0;

    /// \brief Returns the name of the driver
    ///
    /// \since v1.0.0
    virtual const std::string& adapterName() const = 0;

    /// \brief Retrieves a shared pointer to the adapter-specific connection object
    ///
    /// This function can be used when implementing extra functions for an adapter
    /// that are not part of the generic interface provided by dbpp, such as the
    /// backup() function in Dbpp::Sqlite3
    ///
    /// \param db The database connection object
    ///
    /// \return A shared pointer to the adapter-specific object
    ///
    /// \since v1.0.0
    inline static ConnectionPtr getImpl(Dbpp::Connection& db) {
        return db.impl_;
    }
};

} // namespace Dbpp::Adapter
