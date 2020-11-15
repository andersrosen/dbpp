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

#include "Exception.h"
#include "adapter/Result.h"
#include "adapter/Types.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace Dbpp {

class Statement;
class Result;

namespace Detail {

    // Trait to check if class T is std::optional
    template <typename T>
    struct IsOptional : public std::false_type {};

    template <typename T>
    struct IsOptional<std::optional<T>> : public std::true_type {};

    template <typename T>
    inline constexpr bool IsOptionalV = IsOptional<T>::value;

    // Trait to check if class T has a static dbppGet member function
    template <typename T, typename=void>
    struct HasStaticDbppGetMethod : std::false_type{};

    template <typename T>
    struct HasStaticDbppGetMethod<T, std::void_t<decltype(T::dbppGet(std::declval<Result&>(), std::declval<int>()))>>
    : std::true_type{};

    template <typename T>
    inline constexpr bool HasStaticDbppGetMethodV = HasStaticDbppGetMethod<T>::value;

} // namespace Detail

/// \brief Represents a single result (row) of a query
///
/// \since v1.0.0
class DBPP_EXPORTED Result {
    DBPP_NO_COPY_SEMANTICS(Result);

    friend class Statement;

private:
    Adapter::ResultPtr impl_;

    template <typename TupleT, std::size_t I>
    std::tuple_element_t<I, TupleT> getColumn()
    {
        using ElementT = typename std::tuple_element_t<I, TupleT>;
        return get<ElementT>(I);
    }

    template <typename TupleT, std::size_t... Is>
    TupleT getColumns(std::index_sequence<Is...> /* is */)
    {
        return TupleT(getColumn<TupleT, Is>()...);
    }

    explicit Result(Adapter::ResultPtr p);

public:
    /// \brief Default constructor
    ///
    /// \since v1.0.0
    Result() = default;

    /// \brief Move constructor
    ///
    /// \since v1.0.0
    Result(Result &&) noexcept;

    ~Result() = default;

    /// \brief Move constructor
    ///
    /// \since v1.0.0
    Result &operator=(Result &&) noexcept;

    /// \brief Conversion to bool. False if the result is empty, true otherwise
    ///
    /// \since v1.0.0
    inline explicit operator bool() const { return !empty(); }

    /// \brief Checks if the specified column is NULL
    ///
    /// \param columnIndex The zero-based index of the column to check
    /// \return True if the column was NULL, true otherwise
    ///
    /// \since v1.0.0
    [[nodiscard]]
    bool isNull(int columnIndex) const;

    /// \brief Checks if the specified column is NULL
    ///
    /// \param columnName The name of the column to check
    /// \return True if the column was NULL, true otherwise
    ///
    /// \since v1.0.0
    [[nodiscard]]
    bool isNull(std::string_view columnName) const;

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, short& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, int& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, long& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, long long& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, unsigned short& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, unsigned int& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, unsigned long& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, unsigned long long& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, float& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, double& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, std::string& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, std::vector<unsigned char>& out);

    /// \brief Retrieves a value from the result
    ///
    /// Retrieves a value from the result. If the value was NULL, the
    /// output variable will not be touched, and false is returned.
    ///
    /// \param index The zero-based index of the value
    /// \param out Output variable where the value will be stored unless
    ///            it was NULL in the result
    /// \return False if the value was NULL, true otherwise
    ///
    /// \since v1.0.0
    bool get(int index, std::filesystem::path& out);

    /// \brief Retrieves a value of type T from the specified column in the result
    ///
    /// Throws if the value is NULL, unless T is std::optional.
    ///
    /// \tparam T The type of the value to return
    /// \param columnIndex The zero-based index of the column to return
    /// \return The value of the specified column
    ///
    /// \since v1.0.0
    template <typename T>
    [[nodiscard]]
    typename std::enable_if_t<Detail::HasStaticDbppGetMethodV<T>, T>
    get(int columnIndex) {
        if (!impl_)
            throw Error("Empty Result");

        return T::dbppGet(*this, columnIndex);
    }

    /// \brief Retrieves a value of type T from the specified column in the result
    ///
    /// Throws if the value is NULL, unless T is std::optional.
    ///
    /// \tparam T The type of the value to return
    /// \param columnIndex The zero-based index of the column to return
    /// \return The value of the specified column
    ///
    /// \since v1.0.0
    template <typename T>
    [[nodiscard]]
    typename std::enable_if_t<
        !Detail::IsOptional<T>::value && !Detail::HasStaticDbppGetMethodV<T>,
        T>
    get(int columnIndex) {
        T val;
        if (!get(columnIndex, val))
            throw Dbpp::Error("Column value was NULL in retrieval");
        return val;
    }

    /// \brief Retrieves a value of type T from the specified column in the result
    ///
    /// Throws if the value is NULL, unless T is std::optional.
    ///
    /// \tparam T The type of the value to return
    /// \param columnIndex The zero-based index of the column to return
    /// \return The value of the specified column
    ///
    /// \since v1.0.0
    template <typename T>
    [[nodiscard]]
    std::enable_if_t<
        Detail::IsOptionalV<T> && Detail::HasStaticDbppGetMethodV<typename T::value_type>,
        T>
    get(int columnIndex) {
        if (isNull(columnIndex))
            return std::nullopt;
        return get<typename T::value_type>(columnIndex);
    }

    /// \brief Retrieves a value of type T from the specified column in the result
    ///
    /// Throws if the value is NULL, unless T is std::optional.
    ///
    /// \tparam T The type of the value to return
    /// \param columnIndex The zero-based index of the column to return
    /// \return The value of the specified column
    ///
    /// \since v1.0.0
    template <typename T>
    [[nodiscard]]
    typename std::enable_if_t<
        Detail::IsOptionalV<T> && !Detail::HasStaticDbppGetMethodV<typename T::value_type>,
        T>
    get(int columnIndex) {
        typename T::value_type val;
        if (!get(columnIndex, val))
            return std::nullopt;
        return val;
    }

    /// \brief Retrieves an optional value of type T from the specified column in the result
    ///
    /// This is equivalent to get<std::optional<T>>(columnIndex)
    ///
    /// \tparam T The type of the value to return
    /// \param columnIndex The zero-based index of the column to return
    /// \return The value of the specified column
    ///
    /// \since v1.0.0
    template <typename T>
    [[nodiscard]]
    std::optional<T> getOptional(int columnIndex) {
        return get<std::optional<T>>(columnIndex);
    }

    /// \brief Retrieves a value of type T from the specified column in the result
    ///
    /// Throws if the value is NULL, unless T is std::optional.
    ///
    /// \tparam T The type of the value to return
    /// \param columnName The name of the column to return
    /// \return The value of the specified column
    ///
    /// \since v1.0.0
    template <typename T>
    [[nodiscard]]
    T get(std::string_view columnName) {
        return get<T>(columnIndex(columnName));
    }

    /// \brief Retrieves an optional value of type T from the specified column in the result
    ///
    /// This is equivalent to get<std::optional<T>>(columnName)
    ///
    /// \tparam T The type of the value to return
    /// \param columnName The name of the column to return
    /// \return The value of the specified column
    ///
    /// \since v1.0.0
    template <typename T>
    [[nodiscard]]
    std::optional<T> getOptional(std::string_view columnName) {
        return getOptional<T>(columnIndex(columnName));
    }

    /// \brief Returns a column's value or, if it was NULL, the provided default value
    ///
    /// Throws if there is no such column in the row.
    ///
    /// \tparam T The type to be returned
    /// \param columnIndex The zero-based index of the column to be returned
    /// \param defaultValue The value to return if the result was NULL
    /// \return The value of the specified column, or the default value
    ///
    /// \since v1.0.0
    template <typename T>
    [[nodiscard]]
    typename std::enable_if_t<!Detail::IsOptionalV<T> && !Detail::HasStaticDbppGetMethodV<T>, std::remove_reference_t<T>>
    valueOr(int columnIndex, std::remove_reference_t<T> defaultValue) {
        get(columnIndex, defaultValue);
        return defaultValue;
    }

    /// \brief Returns a column's value or, if it was NULL, the provided default value
    ///
    /// Throws if there is no such column in the row.
    ///
    /// \tparam T The type to be returned
    /// \param columnIndex The zero-based index of the column to be returned
    /// \param defaultValue The value to return if the result was NULL
    /// \return The value of the specified column, or the default value
    ///
    /// \since v1.0.0
    template <typename T>
    [[nodiscard]]
    typename std::enable_if_t<!Detail::IsOptionalV<T> && Detail::HasStaticDbppGetMethodV<T>, std::remove_reference_t<T>>
    valueOr(int columnIndex, std::remove_reference_t<T> defaultValue) {
        if (isNull(columnIndex))
            return defaultValue;
        return T::dbppGet(*this, columnIndex);
    }

    /// \brief Converts the result to a tuple
    ///
    /// \tparam Ts The type list of the tuple
    /// \return The result as std::tuple<Ts..>
    template <typename... Ts>
    [[nodiscard]]
    std::tuple<Ts...> toTuple()
    {
        return getColumns<std::tuple<Ts...>>(std::make_index_sequence<sizeof...(Ts)>{});
    }

    /// \brief Converts the result to a tuple
    template <typename... Ts>
    [[nodiscard]]
    operator std::tuple<Ts...>()
    {
        return toTuple<Ts...>();
    }

    /// \brief Checks if the result is empty
    ///
    /// \return True if the result is empty, false otherwise
    ///
    /// \since v1.0.0
    [[nodiscard]]
    bool empty() const;

    /// \brief Retrieves the number of columns (values) in the result
    ///
    /// \return The number of columns in the result
    ///
    /// \since v1.0.0
    [[nodiscard]]
    int columnCount() const;

    /// \brief Retrieves the name of the column at the specified index
    ///
    /// Throws if the index is out of range
    ///
    /// \param index The zero based index of the column
    /// \return The name of the column
    ///
    /// \since v1.0.0
    [[nodiscard]]
    std::string columnName(int index) const;

    /// \brief Retrieves the index of the specified column
    ///
    /// Throws if there is no such column in the result
    ///
    /// \param columnName The name of the column
    /// \return The index of the specified column
    ///
    /// \since v1.0.0
    [[nodiscard]]
    int columnIndex(std::string_view columnName) const;

    /// \brief Checks if the result has a column of the specified name
    ///
    /// \param columnName The name of the column to check for
    /// \return True if the result has such a column, false otherwise
    ///
    /// \since v1.0.0
    [[nodiscard]]
    bool hasColumn(std::string_view columnName) const;

    /// \brief Retrieves the last insert ID
    ///
    /// \return The ID of the last inserted row
    ///
    /// \since v1.0.0
    [[nodiscard]]
    inline long long getInsertId() { return impl_->getInsertId(""); }

    /// \brief Retrieves the last insert ID
    ///
    /// \param sequenceName Name of the sequence holding the ID
    /// \return The last ID in the sequence
    ///
    /// \since v1.0.0
    [[nodiscard]]
    inline long long getInsertId(std::string_view sequenceName) { return impl_->getInsertId(sequenceName); }
};

} // namespace Dbpp
