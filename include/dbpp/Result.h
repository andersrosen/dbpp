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

#include "Exception.h"
#include "driver/Result.h"
#include "driver/Types.h"

#include <filesystem>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace Dbpp {

class Statement;
class Result;

namespace Detail {

    ///@{
    /// Trait to check if class T is std::optional
    template <typename T>
    struct IsOptional : public std::false_type {};

    template <typename T>
    struct IsOptional<std::optional<T>> : public std::true_type {};

    template <typename T>
    inline constexpr bool IsOptionalV = IsOptional<T>::value;

    ///@}

    ///@{
    /// Trait to check if class T has a static dbpp_get member function
    template <typename T, typename=void>
    struct HasStaticDbppGetMethod : std::false_type{};

    template <typename T>
    struct HasStaticDbppGetMethod<T, std::void_t<decltype(T::dbppGet(std::declval<Result&>(), std::declval<int>()))>>
    : std::true_type{};

    template <typename T>
    inline constexpr bool HasStaticDbppGetMethodV = HasStaticDbppGetMethod<T>::value;
    ///@}

}

/// Represents a single result (row) of a query
class Result {
    friend class Statement;
private:
    Driver::ResultPtr impl;

protected:
    explicit Result(Driver::ResultPtr p);

public:
    Result() = default;
    Result(const Result &) = delete;
    Result(Result &&) noexcept ;
    ~Result() = default;
    Result &operator=(const Result &) = delete;
    Result &operator=(Result &&) noexcept;

    /// Conversion to bool. False if the result is empty, true otherwise
    inline explicit operator bool () {return !empty();}

    bool isNull(int colindex);

    ///@{
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \param index The zero-based index of the value.
    /// \return False if the value was NULL, true otherwise.
    bool get(short &out, int index);
    bool get(int &out, int index);
    bool get(long &out, int index);
    bool get(long long &out, int index);

    bool get(unsigned short &out, int index);
    bool get(unsigned int &out, int index);
    bool get(unsigned long &out, int index);
    bool get(unsigned long long &out, int index);

    bool get(float &out, int index);
    bool get(double &out, int index);

    bool get(std::string &out, int index);
    bool get(std::vector<unsigned char> &out, int index);
    bool get(std::filesystem::path &out, int index);
    ///@}

    /// Retrieves a value of type T from the specified column in the result.
    /// Throws if the value is NULL, unless T is std::optional.
    ///
    /// \tparam T The type of the value to return
    /// \param columnIndex The column to return
    /// \return The value of the specified column
    template <typename T>
    typename std::enable_if_t<Detail::HasStaticDbppGetMethodV<T>, T>
    get(int columnIndex) {
        return T::dbppGet(*this, columnIndex);
    }

    template <typename T>
    typename std::enable_if_t<
        not Detail::IsOptional<T>::value and not Detail::HasStaticDbppGetMethodV<T>,
        T>
    get(int columnIndex) {
        T val;
        if (!get(val, columnIndex))
            throw Dbpp::Error("Column value was NULL in retrieval");
        return val;
    }

    template <typename T>
    std::enable_if_t<
        Detail::IsOptionalV<T> and Detail::HasStaticDbppGetMethodV<typename T::value_type>,
        T>
    get(int columnIndex) {
        if (isNull(columnIndex))
            return std::nullopt;
        return get<typename T::value_type>(columnIndex);
    }

    template <typename T>
    typename std::enable_if_t<
        Detail::IsOptionalV<T> and not Detail::HasStaticDbppGetMethodV<typename T::value_type>,
        T>
    get(int columnIndex) {
        typename T::value_type val;
        if (!get(val, columnIndex))
            return std::nullopt;
        return val;
    }
    ///@}

    /// Retrieves an optional value of type T from the specified column in the result.
    ///
    /// This is equivalent to get<std::optional<T>(column_index)
    ///
    /// \tparam T The type of the value to return
    /// \param columnIndex The column to return
    /// \return The value of the specified column
    template <typename T>
    std::optional<T> getOptional(int columnIndex) {
        return get<std::optional<T>>(columnIndex);
    }

    /// Retrieves a value of type T from the specified column in the result.
    /// Throws if the value is NULL, unless T is std::optional.
    ///
    /// \tparam T The type of the value to return
    /// \param columnName The column to return
    /// \return The value of the specified column
    template <typename T>
    T get(std::string_view columnName) {
        return get<T>(columnIndex(columnName));
    }

    /// Retrieves an optional value of type T from the specified column in the result.
    ///
    /// This is equivalent to get<std::optional<T>(columnName)
    ///
    /// \tparam T The type of the value to return
    /// \param columnName The column to return
    /// \return The value of the specified column
    template <typename T>
    std::optional<T> getOptional(std::string_view columnName) {
        return getOptional<T>(columnIndex(columnName));
    }

    ///@{
    /// Returns a column's value or, if it was NULL, the provided default value.
    /// Throws if there is no such column in the row.
    ///
    /// \tparam T The type to be returned
    /// \param columnIndex The column to be returned
    /// \param defaultValue The value to return if the result was NULL
    /// \return The value of the specified column, or the default value
    template <typename T>
    typename std::enable_if_t<not Detail::IsOptionalV<T> and not Detail::HasStaticDbppGetMethodV<T>, T>
    value_or(int columnIndex, T defaultValue) {
        get(defaultValue, columnIndex);
        return defaultValue;
    }

    template <typename T>
    typename std::enable_if_t<not Detail::IsOptionalV<T> and Detail::HasStaticDbppGetMethodV<T>, T>
    value_or(int columnIndex, T defaultValue) {
        if (isNull(columnIndex))
            return defaultValue;
        return T::dbppGet(*this, columnIndex);
    }
    ///@}

    /// Returns true if the result is empty, false otherwise
    bool empty();

    /// Returns the number of columns (values) in the result
    int columnCount();

    /// Returns the name of the column at the specified index
    std::string columnName(int index);

    /// Returns the index of the specified column
    int columnIndex(std::string_view columnName);

    /// Returns true if the result has a column of the specified name
    bool hasColumn(std::string_view columnName);

    /// Returns the last insert id
    long long getInsertId() { return impl->getInsertId(""); }

    /// Returns the last insert id
    ///
    /// \param sequenceName Name of the sequence holding the ID
    /// \return The last sequence ID
    long long getInsertId(const std::string& sequenceName) { return impl->getInsertId(sequenceName); }
};

} // namespace Dbpp
