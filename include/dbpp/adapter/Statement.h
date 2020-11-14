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
    /// \brief Binds NULL to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param placeholderIndex The zero-based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bindNull(int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(short value, int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(int value, int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(long value, int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(long long value, int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(unsigned short value, int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(unsigned int value, int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(unsigned long value, int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(unsigned long long value, int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(float value, int placeholderIndex) = 0;

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(double value, int placeholderIndex) = 0;

    /// \brief Binds a string to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(const std::string &value, int placeholderIndex) = 0;

    /// \brief Binds a string to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param value The value to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(std::string_view value, int placeholderIndex) = 0;

    /// \brief Binds a vector of bytes as a blob to a placeholder (typically a question mark) in the SQL statement
    ///
    /// \param blobValue The bytes to bind
    /// \param placeholderIndex The zero based index of the placeholder to bind
    ///
    /// \since v1.0.0
    virtual void bind(const std::vector<unsigned char> &blobValue, int placeholderIndex) = 0;

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

    /// \brief Resets the statement to its original state, while keeping the existing bindings
    ///
    /// \since v1.0.0
    virtual void reset() = 0;

    /// \brief Clears the existing bindings of the statement
    ///
    /// \since v1.0.0
    virtual void clearBindings() = 0;
};

} // namespace Dbpp::Adapter
