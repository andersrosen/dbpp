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

#include "MetaFunctions.h"
#include "Result.h"
#include "adapter/Statement.h"
#include "adapter/Types.h"

#include <memory>
#include <string>
#include <vector>

namespace Dbpp {

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
class DBPP_EXPORTED Statement {
    DBPP_NO_COPY_SEMANTICS(Statement);
    friend class Connection;
    friend class BindHelper;

protected:
    Adapter::StatementPtr impl_;

public:
    using Iterator = StatementIterator;
    using iterator = Iterator;

    ~Statement() = default;

    /// \brief Move constructor
    ///
    /// \since v1.0.0
    Statement(Statement&&) noexcept;

    /// \brief Move assignment
    ///
    /// \since v1.0.0
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

    /// \brief Executes the statement or steps to the next result
    ///
    /// \return A result object, representing the next set of values
    ///
    /// \since v1.0.0
    [[nodiscard]]
    Result step();

    /// \brief Returns the SQL statement string represented by this object
    ///
    /// \since v1.0.0
    [[nodiscard]]
    std::string sql() const;

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

protected:
    template <typename... Ts>
    void bind(Ts&&... parameters) {
        impl_->preBind(sizeof...(Ts));
        std::size_t numBound = 0;
        try {
            ((impl_->bind(std::forward<Ts>(parameters)), ++numBound), ...);
        } catch (...) {
            impl_->postBind(sizeof...(Ts), numBound);
            throw;
        }
        impl_->postBind(sizeof...(Ts), numBound);
    }

    explicit Statement(Adapter::StatementPtr p);
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
    using Iterator = StatementTupleIterator<Ts...>;
    using iterator = Iterator;

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
class DBPP_EXPORTED StatementIterator {
    DBPP_NO_COPY_SEMANTICS(StatementIterator);

    friend class Statement;

private:
    Statement* stmt_ = nullptr;
    Result res_;

    explicit StatementIterator(Statement* statement);

public:
    using ValueType = Result;
    using value_type = ValueType;

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
    inline Result& operator*() { return res_; }

    /// \brief Dereferencing operator
    ///
    /// \since v1.0.0
    [[nodiscard]]
    inline Result* operator->() { return &res_; }

    /// \brief Checks if two iterators are equal
    ///
    /// \since v1.0.0
    [[nodiscard]]
    inline bool operator==(const StatementIterator& that) const { return stmt_ == that.stmt_; }

    /// \brief Checks if two iterators are different
    ///
    /// \since v1.0.0
    [[nodiscard]]
    inline bool operator!=(const StatementIterator& that) const  { return !(*this == that); }

    /// \brief Increments the iterator, which means stepping to the next result
    ///
    /// \since v1.0.0
    StatementIterator& operator++();
};

/// \brief Allows iteration over results of a statement, as tuples
///
/// \since v1.0.0
template <typename... Ts>
class StatementTupleIterator {
    DBPP_NO_COPY_SEMANTICS(StatementTupleIterator);

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
    /// \note This operator overload is only included for completeness.
    ///       It can't really be used, since the only two members of tuple
    ///       (assignment and swap) are not const, so they can't be called
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
