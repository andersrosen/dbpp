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

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace Dbpp::Adapter {

/// \brief Interface class for the result of a query
///
/// \since v1.0.0
class Result {
public:

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, short& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, int& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, long& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, long long& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, unsigned short& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, unsigned int& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, unsigned long& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, unsigned long long& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, float& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, double& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, std::string& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the value will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, std::filesystem::path& outputVariable) = 0;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a blob value from the result. If it was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param columnIndex The zero-based index of the value
    /// \param outputVariable Output variable where the blob will be
    ///        stored unless it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool getColumn(int columnIndex, std::vector<unsigned char>& outputVariable) = 0;

    /// \brief Checks if the result is empty
    ///
    /// \return True if the result is empty, false otherwise
    ///
    /// \since v1.0.0
    virtual bool empty() const = 0;

    /// \brief Retrieves the number of columns (values) in the result
    ///
    /// \return The number of columns in the result
    ///
    /// \since v1.0.0
    virtual int columnCount() const = 0;

    /// \brief Retrieves the name of the column at the specified index
    ///
    /// Throws if the index is out of range
    ///
    /// \param columnIndex The zero based index of the column
    /// \return The name of the column
    ///
    /// \since v1.0.0
    virtual std::string columnName(int columnIndex) const = 0;

    /// \brief Retrieves the index of the specified column
    ///
    /// Throws if there is no such column in the result
    ///
    /// \param columnName The name of the column
    /// \return The index of the specified column
    ///
    /// \since v1.0.0
    virtual int columnIndexByName(std::string_view columnName) const = 0;

    /// \brief Checks if the specified column is NULL
    ///
    /// \param columnIndex The zero-based index of the column to check
    /// \return True if the column was NULL, true otherwise
    ///
    /// \since v1.0.0
    virtual bool isNull(int columnIndex) const = 0;

    /// \brief Retrieves the last insert ID
    ///
    /// \param sequenceName Name of the sequence holding the ID
    /// \return The last ID in the sequence
    ///
    /// \since v1.0.0
    virtual long long getInsertId(std::string_view sequenceName) = 0;

    virtual ~Result() = default;
};

}
