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

#include "adapter/Types.h"
#include "Statement.h"

#include <string>
#include <string_view>

namespace Dbpp {

/// \brief A connection object represents a connection to a database
///
/// Before you can use this library, you need to have at least
/// one connection object, that you use to communicate with the
/// database.
///
/// \since v1.0.0
///
/// \includeexamplewithoutput{ConnectionClass.cpp}
class Connection {
    friend class Adapter::Connection;

private:
    Adapter::ConnectionPtr impl;

    Statement createStatement(std::string_view sql) const;

public:
    Connection(const Connection &c) = delete;
    Connection &operator=(const Connection &) = delete;

    /// \brief Construct a connection object
    ///
    /// This constructor is typically called from a driver specific creation
    /// function, such as sqlite3::open().
    ///
    /// \param connection A driver-specific connection
    /// \since v1.0.0
    Connection(Adapter::ConnectionPtr connection);

    /// Move constructor
    /// \since v1.0.0
    Connection(Connection &&that) noexcept;

    /// Move assignment
    /// \since v1.0.0
    Connection &operator=(Connection &&that);

    /// Destructor
    /// \since v1.0.0
    ~Connection() = default;

    /// \brief Creates a new statement for the supplied string
    ///
    /// \param sql An SQL statement string
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A statement object
    ///
    /// \since v1.0.0
    template <typename... Args>
    Statement prepare(std::string_view sql, Args... args) {
        Statement st = createStatement(sql);
        (st.bind(args), ...);
        return st;
    }

    /// \brief Creates and executes an SQL statement
    ///
    /// The statement will be executed once, and the first row of the result set is
    /// returned, if any. This is handy for executing statements such as INSERT or
    /// DELETE FROM, where there won't be multiple resulting rows to iterate over.
    ///
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A result object, which contains the first row of the result set (if any)
    ///
    /// \since v1.0.0
    template <typename... Args>
    Result exec(std::string_view sql, Args... args) {
        Statement st = prepare(sql);
        (st.bind(args), ...);
        return st.step();
    }

    /// \brief Creates and executes an SQL statement, returning a single value
    ///
    /// This is a convenience method that can be used when it is expected that the
    /// statement returns exactly one value.
    ///
    /// \tparam T The type of the return value
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A single value
    ///
    /// \since v1.0.0
    template <typename T, typename... Args>
    T get(std::string_view sql, Args... args) {
        auto row = exec(sql, args...);
        if (row.columnCount() != 1)
            throw Dbpp::Error(std::string("get() expects a single column Result. Statement: ") + std::string{sql});
        return row.template get<T>(0);
    }

    /// \brief Creates and executes an SQL statement, returning a single optional value
    ///
    /// This is a convenience method that can be used when it is expected that the
    /// statement returns exactly one value, or nothing.
    ///
    /// \tparam T The type of the return value
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A single value (if any)
    ///
    /// \since v1.0.0
    template <typename T, typename... Args>
    std::optional<T> getOptional(std::string_view sql, Args... args) {
        auto row = exec(sql, args...);
        if (row.columnCount() != 1)
            throw Dbpp::Error(std::string("getOptional() expects a single column Result. Statement: ") + std::string{sql});
        return row.template getOptional<T>(0);
    }

    /// \brief Begins a transaction
    ///
    /// \since v1.0.0
    void begin();

    /// \brief Commits a transaction
    ///
    /// \since v1.0.0
    void commit();

    /// \brief Rolls back a transaction
    ///
    /// \since v1.0.0
    void rollback();

    /// \brief Returns the name of the driver in use
    ///
    /// \since v1.0.0
    const std::string &driverName() const;
};

/// \brief RAII class for transaction handling
///
/// Instantiate a transaction object to begin a transaction, and call commit on it to commit.
/// If it goes out of scope before commit has been called, the transaction will be rolled back automatically.
///
/// \since v1.0.0
class Transaction {
private:
    Connection &db_;
    bool committed{false};

public:
    /// \brief Constructor. Begins a transaction
    ///
    /// \param db The db connection object
    ///
    /// \since v1.0.0
    inline explicit Transaction(Connection &db) : db_(db) {
        db_.begin();
    }

    /// \brief Destructor. Rolls back the transaction unless commit() has been called
    ///
    /// \since v1.0.0
    inline ~Transaction() {
        if (!committed)
            db_.rollback();
    }

    /// \brief Commits the transaction
    ///
    /// \since v1.0.0
    inline void commit() {
        db_.commit();
        committed = true;
    }
};

}