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

#include "driver/Types.h"
#include "Statement.h"
#include <memory>

namespace Dbpp {

/// A connection object represents a connection to a database
///
/// Before you can use this library, you need to have at least
/// one connection object, that you use to communicate with the
/// database.
///
/// \includeexamplewithoutput{ConnectionClass}
class Connection {
    friend class Driver::Connection;

private:
    Driver::ConnectionPtr impl;

public:
    Connection(const Connection &c) = delete;
    Connection &operator=(const Connection &) = delete;

    /// Construct a connection object.
    /// This constructor is typically called from a driver specific creation
    /// function, such as sqlite3::open()
    Connection(Driver::ConnectionPtr c);
    /// Move constructor
    Connection(Connection &&that) noexcept;
    /// Move assignment
    Connection &operator=(Connection &&that);
    /// Destructor
    ~Connection() = default;

    /// Creates a new statement for the supplied string
    Statement prepare(const std::string &sql);

    /// Creates a new statement for the supplied string
    ///
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A statement object
    ///
    /// \example
    /// auto st = db.prepare("SELECT * FROM user WHERE dept_id = ?", department_id);
    /// for (row : st) {
    /// ...
    /// }
    ///
    template <typename... Args>
    Statement prepare(const std::string &sql, Args... args) {
        Statement st = prepare(sql);
        (st.bind(args), ...);
        return st;
    }

    /// Creates and executes an SQL statement
    ///
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A result object, which contains the first row of the result set (if any)
    template <typename... Args>
    Result exec(const std::string &sql, Args... args) {
        Statement st = prepare(sql);
        (st.bind(args), ...);
        return st.step();
    }

    /// Creates and executes an SQL statement, returning a single value
    ///
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A single value
    template <typename T, typename... Args>
    T get(const std::string &sql, Args... args) {
        auto row = exec(sql, args...);
        if (row.columnCount() != 1)
            throw Dbpp::Error(std::string("get() expects a single column Result. Statement: ") +  sql);
        return row.template get<T>(0);
    }

    /// Creates and executes an SQL statement, returning a single value
    ///
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A single value (if any)
    template <typename T, typename... Args>
    std::optional<T> getOptional(const std::string& sql, Args... args) {
        auto row = exec(sql, args...);
        if (row.columnCount() != 1)
            throw Dbpp::Error(std::string("getOptional() expects a single column Result. Statement: ") +  sql);
        return row.template getOptional<T>(0);
    }

    /// Begins a transaction
    void begin();

    /// Commits a transaction
    void commit();

    /// Rolls back a transaction
    void rollback();

    /// Returns the name of the driver in use
    const std::string &driverName() const;
};

/// RAII class for transaction handling
///
/// Instantiate a transaction object to begin
/// a transaction, and call commit on it to commit.
/// If it goes out of scope before commit has been
/// called, the transaction will be rolled back automatically.
///
/// @inlineex{Use of transaction object,transaction.cxx}
class Transaction {
private:
    Connection &c;
    bool committed{false};

public:
    /// Constructor. Begins a transaction
    ///
    /// \param c The db connection object
    inline Transaction(Connection &c) : c(c) {
        c.begin();
    }

    /// Rolls back the transaction unless commit() has been called.
    inline ~Transaction() {
        if (!committed)
            c.rollback();
    }

    /// Commits the transaction
    inline void commit() {
        c.commit();
        committed = true;
    }
};

}
