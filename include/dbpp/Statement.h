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
#include "adapter/Types.h"

#include <memory>
#include <string>
#include <vector>

namespace Dbpp {

namespace Detail {

template<class T, class = void>
struct HasDbppBindMethod : std::false_type {};

template<class T>
struct HasDbppBindMethod<T, std::void_t<decltype(std::declval<const T&>().dbppBind(std::declval<Statement&>()))>>
: std::true_type {};

// Trait to check if class T has a dbppBind member function
template<class T>
inline constexpr bool HasDbppBindMethodV = HasDbppBindMethod<T>::value;

} // namespace Detail

class Connection;

template <typename... Ts>
class StatementTupleWrapper;

class StatementIterator;

template <typename... Ts>
class StatementTupleIterator;

/// \brief Represents a prepared SQL statement
///
/// You can bind values to it, and then execute it by either calling the step() method
/// or iterate over it using the begin() and end() methods.
///
/// \since v1.0.0
class Statement {
    friend class Connection;

private:
    Adapter::StatementPtr impl_;
    int placeholderPosition_ = 0;

    void doReset();
    void clearBindings();

    /// \brief Constructs a Statement object from an adapter-specific statement
    ///
    /// \since v1.0.0
    explicit Statement(Adapter::StatementPtr p);

public:
    using iterator = StatementIterator; // NOLINT

    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    /// \brief Move constructor
    ///
    /// \since v1.0.0
    Statement(Statement&&) noexcept;
    ~Statement() = default;

    Statement& operator=(Statement&&) noexcept;

    /// \brief Returns an iterator to the first result of this statement
    ///
    /// \since v1.0.0
    [[nodiscard]]
    iterator begin();

    /// \brief Returns the end iterator of the result set
    ///
    /// \since v1.0.0
    [[nodiscard]]
    iterator end();

    /// \brief Binds NULL to a placeholder (typically a question mark) in the SQL statement,
    /// and increments the index of the next placeholder to bind
    ///
    /// \since v1.0.0
    void bindNull();

    /// \brief Binds NULL to a placeholder (typically a question mark) in the SQL statement,
    /// and increments the index of the next placeholder to bind
    ///
    /// \since v1.0.0
    void bind(std::nullptr_t);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(short value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(int value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(long value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(long long value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(unsigned short value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(unsigned int value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(unsigned long value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(unsigned long long value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(float value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(double value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(const char* value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(const std::string& value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL statement, and
    /// increments the index of the next placeholder to bind
    ///
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    void bind(std::string_view value);

    /// \brief Binds a vector of bytes as a BLOB to a placeholder (typically a question mark) in the SQL
    /// statement, and increments the index of the next placeholder to bind
    ///
    /// \param value The vector of bytes to bind as a blob
    ///
    /// \since v1.0.0
    void bind(const std::vector<std::uint8_t>& value);

    /// \brief Binds a value to a placeholder (typically a question mark) in the SQL
    /// statement, and increments the index of the next placeholder to bind
    ///
    /// This is only possible if T has a method dbppBind(Statement&) const that
    /// calls bind on the provided statement to actually bind a value.
    ///
    /// \tparam T the type of the value to bind
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    template<typename T, typename std::enable_if_t<Detail::HasDbppBindMethodV<T>, int> = 0>
    void bind(const T& value) {
        value.dbppBind(*this);
    }

    /// \brief Binds an optional value to a placeholder (typically a question mark) in the SQL
    /// statement, and increments the index of the next placeholder to bind.
    ///
    /// If the optional does not have a value, NULL will be bound instead
    ///
    /// \tparam T The underlying type of the optional
    /// \param value The value to bind
    ///
    /// \since v1.0.0
    template<class T>
    void bind(const std::optional<T>& value) {
        if (value.has_value())
            bind(value.value());
        else
            bindNull();
    }

    /// \brief Returns the SQL statement string represented by this object
    ///
    /// \since v1.0.0
    [[nodiscard]]
    std::string sql() const;

    /// \brief Executes the statement or steps to the next result
    ///
    /// \return A result object, representing the next set of values
    ///
    /// \since v1.0.0
    [[nodiscard]]
    Result step();

    /// \brief Resets the statement to its initial state, so it can be executed again
    ///
    /// \since v1.0.0
    template <typename... Args>
    void reset(Args... args)
    {
        doReset();
        if constexpr(sizeof...(args) > 0)
            clearBindings();
        (bind(args), ...);
    }

    /// \brief Allows retrieving results as tuples
    ///
    /// \tparam Ts The types of the columns in the result
    /// \return A wrapper class allowing iteration over tuples instead of Result objects
    ///
    /// \since v1.0.0
    template <typename... Ts>
    [[nodiscard]]
    StatementTupleWrapper<Ts...> as() && {
        return StatementTupleWrapper<Ts...>(std::move(*this));
    }
};

/// \brief A class that wraps Statement objects, to allow iteration over tuples instead of Result objects
///
/// \tparam Ts The types of the columns in the result
///
/// \since v1.0.0
template <typename... Ts>
class StatementTupleWrapper
{
    friend class Statement;

    Statement stmt_;

    explicit StatementTupleWrapper(Statement&& statement)
    : stmt_(std::move(statement))
    {}

public:
    using iterator = StatementTupleIterator<Ts...>; // NOLINT

    /// \brief Returns an iterator to the first result
    ///
    /// \since v1.0.0
    [[nodiscard]]
    iterator begin() {return iterator(&stmt_); }

    /// \brief Returns an iterator to the end of the results
    ///
    /// \since v1.0.0
    [[nodiscard]]
    iterator end() {return {};}
};

/// \brief Allows iteration over results of a statement
///
/// \since v1.0.0
class StatementIterator {
    friend class Statement;

private:
    Statement* stmt_ = nullptr;
    Result res_;

    explicit StatementIterator(Statement* statement)
    : stmt_(statement), res_(statement->step()) {
        if (res_.empty()) {
            // Become the end iterator
            stmt_ = nullptr;
            res_ = Result();
        }
    }

public:
    using value_type = Result; // NOLINT

    StatementIterator& operator=(const StatementIterator&) = delete;
    StatementIterator(const StatementIterator&) = delete;

    ~StatementIterator() = default;

    /// \brief Default constructor
    ///
    /// \since v1.0.0
    StatementIterator() = default; // Constructs an end iterator

    /// \brief Move constructor
    ///
    /// \since v1.0.0
    StatementIterator(StatementIterator&&) noexcept = default;

    /// \brief Move assignment
    ///
    /// \since v1.0.0
    StatementIterator& operator=(StatementIterator&&) noexcept = default;

    /// \brief Dereferencing operator
    ///
    /// \since v1.0.0
    [[nodiscard]]
    Result& operator*() { return res_; }

    /// \brief Dereferencing operator
    ///
    /// \since v1.0.0
    [[nodiscard]]
    Result* operator->() { return &res_; }

    /// \brief Checks if two iterators are equal
    ///
    /// \since v1.0.0
    [[nodiscard]]
    bool operator==(const StatementIterator& that) const { return stmt_ == that.stmt_; }

    /// \brief Checks if two iterators are different
    ///
    /// \since v1.0.0
    [[nodiscard]]
    bool operator!=(const StatementIterator& that) const  { return !(*this == that); }

    /// \brief Increments the iterator, which means stepping to the next result
    ///
    /// \since v1.0.0
    StatementIterator& operator++() {
        if (stmt_) {
            res_ = stmt_->step();
            if (res_.empty()) {
                // Become the end iterator
                stmt_ = nullptr;
                res_ = Result();
            }
        }
        return *this;
    }
};

/// \brief Allows iteration over results of a statement, as tuples
///
/// \since v1.0.0
template <typename... Ts>
class StatementTupleIterator {
    friend class StatementTupleWrapper<Ts...>;

    using TupleT = std::tuple<Ts...>;
    Statement* stmt_ = nullptr;
    TupleT tuple_;

    explicit StatementTupleIterator(Dbpp::Statement* statement)
    : stmt_(statement) {
        auto res = statement->step();
        if (res.empty()) {
            // Become the end iterator
            stmt_ = nullptr;
        } else {
            tuple_ = res.toTuple<Ts...>();
        }
    }

public:
    using value_type = std::tuple<Ts...>; // NOLINT

    StatementTupleIterator& operator=(const StatementTupleIterator&) = delete;
    StatementTupleIterator(const StatementTupleIterator&) = delete;

    ~StatementTupleIterator() = default;

    /// \brief Default constructor
    ///
    /// \since v1.0.0
    StatementTupleIterator() = default; // Constructs an end iterator

    /// \brief Move constructor
    ///
    /// \since v1.0.0
    StatementTupleIterator(StatementTupleIterator&&) noexcept = default;

    /// \brief Move assignment
    ///
    /// \since v1.0.0
    StatementTupleIterator& operator=(StatementTupleIterator&&) noexcept = default;

    /// \brief Dereferencing operator
    ///
    /// \since v1.0.0
    [[nodiscard]]
    const std::tuple<Ts...>& operator*() const { return tuple_; }

    /// \brief Dereferencing operator
    ///
    /// \since v1.0.0
    [[nodiscard]]
    const std::tuple<Ts...>* operator->() const { return &tuple_; }

    /// \brief Checks if two iterators are equal
    ///
    /// \since v1.0.0
    [[nodiscard]]
    bool operator==(const StatementTupleIterator& that) const { return stmt_ == that.stmt_; }

    /// \brief Checks if two iterators are different
    ///
    /// \since v1.0.0
    [[nodiscard]]
    bool operator!=(const StatementTupleIterator& that) const { return !(*this == that); }

    /// \brief Increments the iterator, which means stepping to the next result
    ///
    /// \since v1.0.0
    StatementTupleIterator& operator++() {
        if (stmt_) {
            auto res = stmt_->step();
            if (res.empty()) {
                // Become the end iterator
                stmt_ = nullptr;
            } else {
                tuple_ = res.toTuple<Ts...>();
            }
        }
        return *this;
    }
};

} // namespace Dbpp
