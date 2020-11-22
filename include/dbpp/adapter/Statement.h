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

#include "../defs.h"

#include "Types.h"

#include <string>
#include <vector>

namespace Dbpp::Adapter {

/// \brief Interface class for database adapters
///
/// \since v1.0.0
class Statement {
public:
    /// \brief Called before placeholder parameters will be bound
    ///
    /// This method is called by Statement objects before calling bindNull() or bind() repeatedly for each
    /// placeholder parameter. This can be used by the adapter to do any preparation needed.
    ///
    /// \param providedParameterCount The number of parameters that were provided
    virtual void preBind(std::size_t providedParameterCount) = 0;

    /// \brief Called after placeholder parameters have been bound
    ///
    /// This method is called the Statement objects after calls to bind() or bindNull(). This function
    /// will be called even if there was an exception while binding the parameters, in which case the
    /// boundParameterCount argument will be less than providedParameterCount.
    ///
    /// \param providedParameterCount The number of parameters that will be bound
    /// \param boundParameterCount The number of parameters that were bound before the call to this method
    virtual void postBind(std::size_t providedParameterCount, std::size_t boundParameterCount) = 0;

    /// \brief Binds NULL to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \since v1.0.0
    virtual void bindNull() = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(short value) = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(int value) = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(long value) = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(long long value) = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(unsigned short value) = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(unsigned int value) = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(unsigned long value) = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(unsigned long long value) = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(float value) = 0;

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(double value) = 0;

    /// \brief Binds a string to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(const std::string &value) = 0;

    /// \brief Binds a string to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    virtual void bind(std::string_view value) = 0;

    /// \brief Binds a sequence of bytes as a blob to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param blobValue The bytes to bind
    ///
    /// \since v1.0.0
    virtual void bind(const std::pair<const unsigned char*, std::size_t>& bytes) = 0;

    /// \brief Returns the SQL statement string represented by this object
    ///
    /// \since v1.0.0
    [[nodiscard]]
    virtual std::string sql() const = 0;

    /// \brief Executes the statement or steps to the next result
    ///
    /// \return A shared pointer to a result object, representing the next set of values
    ///
    /// \since v1.0.0
    [[nodiscard]]
    virtual ResultPtr step() = 0;
};

} // namespace Dbpp::Adapter
