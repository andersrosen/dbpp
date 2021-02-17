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
#include <dbpp/MetaFunctions.h>
#include <dbpp/util.h>
#include <dbpp/adapter/Types.h>

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Dbpp {

class PlaceholderBinder;

namespace Detail {

    template <class T, typename = void>
    struct HasDbppBindMethod : std::false_type {};

    template <class T>
    struct HasDbppBindMethod<T, std::void_t<decltype(std::declval<const T&>().dbppBind(std::declval<PlaceholderBinder&>()))>>
    : std::true_type
    {};

    // Trait to check if class T has a dbppBind member function
    template <class T>
    inline constexpr bool HasDbppBindMethodV = HasDbppBindMethod<T>::value;

} // namespace Detail

class PlaceholderBinder {
    public:
    virtual ~PlaceholderBinder() = default;

    /// \brief Binds NULL to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \since v1.0.0
    virtual void bind(std::nullptr_t) = 0;

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

    /// \brief Binds a value to the next placeholder (typically a question mark) in the SQL statement
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
    virtual void bind(const std::pair<const unsigned char*, std::size_t>& data) = 0;

    /// \brief Binds a vector of bytes as a blob to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// \param blobValue The bytes to bind
    ///
    /// \since v1.0.0
    template <typename T, typename=std::enable_if_t<Detail::IsOneOfV<T, char, signed char, unsigned char, std::byte>>>
    void bind(const std::vector<T>& value) {
        bind(std::pair(reinterpret_cast<const unsigned char*>(value.data()), static_cast<std::size_t>(value.size())));
    }

    /// \brief Binds a custom object to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// This method is available for objects of any class T that define the method
    /// void T::dbppBind(Dbpp::PlaceholderBinder& binder).
    ///
    /// \param customObject The object to bind
    ///
    /// \since v1.0.0
    template<typename T, typename std::enable_if_t<Detail::HasDbppBindMethodV<T>, int> = 0>
    void bind(const T& customObject) {
        customObject.dbppBind(*this);
    }

    /// \brief Binds an optional value to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// If the optional has no value, NULL will be bound.
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    template<class T>
    void bind(const std::optional<T>& value) {
        if (value.has_value())
            bind(value.value());
        else
            bind(nullptr);
    }

    /// \brief Binds a value stored in a unique_ptr to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// If the unique_ptr is not set, NULL will be bound.
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    template<class T>
    void bind(const std::unique_ptr<T>& value) {
        if (value)
            bind(*value);
        else
            bind(nullptr);
    }

    /// \brief Binds a value referenced by a shared_ptr to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// If the shared_ptr is not set, NULL will be bound.
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    template<class T>
    void bind(const std::shared_ptr<T>& value) {
        if (value)
            bind(*value);
        else
            bind(nullptr);
    }

    /// \brief Binds a value referenced by a weak_ptr to the next placeholder (typically a question mark) in the SQL statement
    ///
    /// If the weak_ptr is not valid, NULL will be bound.
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    template<class T>
    void bind(const std::weak_ptr<T>& value) {
        if (auto shared = value.lock())
            bind(*shared);
        else
            bind(nullptr);
    }
};

}
