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

#include <dbpp/config.h>
#include <dbpp/exports.h>
#include <dbpp/Statement.h>
#include <dbpp/util.h>

namespace Dbpp {

/// \brief Represents a prepared statement
///
/// Prepared statements will often (but not always) be faster than ordinary statements
/// when reused several times.
///
/// They are used the same way as ordinary statements, except that they can be reset to
/// their initial state and executed again. Their placeholder parameters are set by calling
/// rebind(), which will reset the statement and then bind the provided parameters.
///
/// \since v1.0.0
class DBPP_EXPORT PreparedStatement final : public Statement {
    friend class Connection;

    explicit PreparedStatement(Adapter::PreparedStatementPtr p);

    void resetAndClearBindings();

public:
    /// \brief Resets the statement to its initial state, so it can be executed again
    ///
    /// Any bound placeholder parameters will keep their bound values.
    ///
    /// \since v1.0.0
    void reset();

    /// \brief Resets a statement while binding new values to placeholder parameters
    ///
    /// \since v1.0.0
    template <typename... Ts>
    void rebind(Ts&&... parameters) {
        resetAndClearBindings();
        bind(std::forward<Ts>(parameters)...);
    }
};

} // namespace Dbpp
