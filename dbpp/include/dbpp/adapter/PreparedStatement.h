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
#include <dbpp/util.h>
#include <dbpp/adapter/Statement.h>

namespace Dbpp::Adapter {

/// \brief Interface class for database adapters
///
/// \since v1.0.0
class DBPP_EXPORT PreparedStatement : public Statement {
public:
    /// \brief Resets the statement to its original state, while keeping the existing bindings
    ///
    /// \since v1.0.0
    virtual void reset() = 0;

    /// \brief Resets the statement and clears the existing placeholder bindings of the statement
    ///
    /// \since v1.0.0
    virtual void resetAndClearBindings() = 0;
};

} // namespace Dbpp::Adapter
