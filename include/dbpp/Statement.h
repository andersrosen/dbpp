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

#include "Result.h"
#include "driver/Types.h"

#include <memory>
#include <string>
#include <vector>

namespace Dbpp {

namespace Detail {

///@{
/// Trait to check if class T has a dbpp_bind member function
template<class T, class = void>
struct HasDbppBindMethod : std::false_type {};

template<class T>
struct HasDbppBindMethod<T, std::void_t<decltype(std::declval<const T&>().dbpp_bind(std::declval<Statement&>()))>>
: std::true_type {};

template<class T>
inline constexpr bool hasDbppBindMethod = HasDbppBindMethod<T>::value;
///@}

} // namespace Detail

class Connection;

/// Allows iteration over results of a statement
class StatementIterator : public std::iterator<std::input_iterator_tag,
                              Statement, // Value type
                              size_t> {  // diff type
    friend class Statement;

private:
    Statement* stmt;
    Result res;

    explicit StatementIterator(Statement* s);

public:
    StatementIterator()
    : stmt(nullptr) {}; // Constructs an end iterator

    StatementIterator(const StatementIterator& that)
    : stmt(that.stmt) {}

    Result& operator*() { return res; }
    Result* operator->() { return &res; }

    bool operator==(StatementIterator& that) {
        // FIXME! More thorough comparison needed? What about the result obj?
        return stmt == that.stmt;
    }

    bool operator!=(StatementIterator& that) { return !(*this == that); }

    StatementIterator& operator++();
};

/// Represents an SQL statement. You can bind values to it
class Statement {
    friend class Connection;

private:
    Driver::StatementPtr impl;
    int placeholderPosition = 0;

protected:
    explicit Statement(Driver::StatementPtr p);

public:
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;
    Statement(Statement&&) noexcept;
    ~Statement() = default;

    /// Returns an iterator to the first result of this statement
    StatementIterator begin();
    /// Returns the end iterator of the result set
    StatementIterator end();

    ///@{
    /// Binds values to placeholders (question marks) in the SQL statement
    void bindNull();
    void bind(std::nullptr_t);
    void bind(short value);
    void bind(int value);
    void bind(long value);
    void bind(long long value);
    void bind(unsigned short value);
    void bind(unsigned int value);
    void bind(unsigned long value);
    void bind(unsigned long long value);
    void bind(float value);
    void bind(double value);
    void bind(const char* value);
    void bind(const std::string& value);
    void bind(std::string_view value);
    void bind(const std::vector<uint8_t>& value);

    template<typename T, typename std::enable_if_t<Detail::hasDbppBindMethod<T>, int> = 0>
    void bind(const T& value) {
        value.dbppBind(*this);
    }

    template<class T>
    void bind(const std::optional<T>& value) {
        if (value.has_value())
            bind(value.value());
        else
            bindNull();
    }
    ///@}

    /// Returns the SQL statement string represented by this object
    std::string sql() const;

    /// Executes the statement or steps to the next result
    ///
    /// \return A result object, representing the next set of values.
    Result step();
};

inline StatementIterator::StatementIterator(Statement* statement)
: stmt(statement), res(statement->step()) {
    if (res.empty()) {
        // Become the end iterator
        stmt = nullptr;
        res = Result();
    }
}

inline StatementIterator&
StatementIterator::operator++() {
    if (stmt) {
        res = stmt->step();
        if (res.empty()) {
            // Become the end iterator
            stmt = nullptr;
            res = Result();
        }
    }
    return *this;
}

}
