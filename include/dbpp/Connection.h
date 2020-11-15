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

#include "defs.h"

#include "adapter/Types.h"
#include "MetaFunctions.h"
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
class DBPP_EXPORTED Connection {
    DBPP_NO_COPY_SEMANTICS(Connection);

    friend class Adapter::Connection;

private:
    Adapter::ConnectionPtr impl_;

    Statement createStatement(std::string_view sql) const;

public:
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
    Connection &operator=(Connection &&that) noexcept;

    /// Destructor
    /// \since v1.0.0
    ~Connection() = default;

    /// \brief Creates a new statement for the supplied string
    ///
    /// \param sql An SQL statement string
    /// \param args A list of values to be bound to placeholders in the SQL statement string
    /// \return A Statement object
    ///
    /// \since v1.0.0
    template <typename... Args>
    Statement statement(std::string_view sql, Args... args) {
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
        Statement st = createStatement(sql);
        (st.bind(args), ...);
        return st.step();
    }

#ifdef DOXYGEN_SHOULD_SEE_THIS
    /// \brief Creates and executes an SQL statement, returning a single value
    ///
    /// This is a convenience method that can be used when it is expected that the
    /// statement returns exactly one value.
    ///
    /// \tparam T The type of the return value
    /// \tparam Args... The types of the additional arguments for binding values to placeholders (should be deduced automatically)
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A single value
    ///
    /// \since v1.0.0

    // Only for documentation purposes! This is ifdef:d out when compiling
    template <typename T, typename... Args>
    [[nodiscard]]
    T get(std::string_view sql, Args... args);


    /// \brief Creates and executes an SQL statement, returning a single result
    ///
    /// This is a convenience method that can be used when it is expected that the
    /// statement returns exactly one row in the result set, or when you are only
    /// interested in the first one.
    ///
    /// \tparam ReturnTypes The types of the columns to return
    /// \tparam Args... The types of the additional arguments for binding values to placeholders (should be deduced automatically)
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A tuple representing the result
    ///
    /// \since v1.0.0

    // Only for documentation purposes! This is ifdef:d out when compiling
    template <typename... ReturnTypes, typename... Args>
    [[nodiscard]]
    std::tuple<ReturnTypes...> get(std::string_view sql, Args... args);
#else
    // The function below is a bit of a hack. It would be nice to have two functions as in the
    // doxygen part above, but that won't compile since it will be ambiguous which of the two
    // functions to use. The workaround is to implement them both in a single function, which
    // adapts depending on the number of return types.

    // Even though this will not be read by doxygen, we add a doxygen style comment anyway,
    // since many IDEs will show this kind of comments

    /// \brief Creates and executes an SQL statement, returning a single value or a tuple
    ///
    /// \tparam ReturnType... The return type - if there are multiple types a tuple will be returned, otherwise a single value will be returned
    /// \tparam Args... The types of the additional arguments for binding values to placeholders (should be deduced automatically)
    /// \param sql An SQL statement
    /// \param args A list of values to be bound to placeholders in the SQL statement
    /// \return A single value or a tuple representing the result
    ///
    /// \since v1.0.0
    template <typename... ReturnType, typename... Args>
    [[nodiscard]]
    Detail::ScalarOrTupleT<ReturnType...> get(std::string_view sql, Args... args) {
        auto row = exec(sql, args...);
        if constexpr(Detail::IsScalarV<ReturnType...>) {
            // Return a single value
            if (row.columnCount() != 1)
                throw Dbpp::Error(std::string("get() expects a single column Result. Statement: ") + std::string{ sql });
            return row.template get<Detail::FirstTypeT<ReturnType...>>(0);
        } else {
            // Return it as a tuple
            return row.template toTuple<ReturnType...>();
        }
    }
#endif

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
    [[nodiscard]]
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
    const std::string& adapterName() const;
};

/// \brief RAII class for scoped transaction handling
///
/// Instantiate a transaction object to begin a transaction, and call commit on it to commit.
/// If it goes out of scope before commit has been called, the transaction will be rolled back automatically.
///
/// \since v1.0.0
class DBPP_EXPORTED Transaction {
    DBPP_NO_COPY_SEMANTICS(Transaction);
    DBPP_NO_MOVE_SEMANTICS(Transaction);

private:
    Connection &db_;
    bool committed_{false};

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
        if (!committed_)
            db_.rollback();
    }

    /// \brief Commits the transaction
    ///
    /// \since v1.0.0
    inline void commit() {
        db_.commit();
        committed_ = true;
    }
};

} // namespace Dbpp

