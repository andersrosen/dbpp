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
#include <dbpp/PlaceholderBinder.h>
#include <dbpp/adapter/Statement.h>

#include <variant>
#include <vector>

namespace Dbpp {

class Connection;

/// \brief Allows incremental construction of statements
class DBPP_EXPORT StatementBuilder final {
    friend class Connection;

    class PlaceholderValues : public PlaceholderBinder {
        public:
        using Value = std::variant<
            std::nullptr_t,
            char, unsigned char, signed char,
            short, unsigned short,
            int, unsigned int,
            long, unsigned long,
            long long, unsigned long long,
            float, double,
            std::string,
            std::vector<std::byte>>;
        std::vector<Value> values;

        template <typename... Ts>
        explicit PlaceholderValues(Ts&&... vals)
        {
            values.reserve(sizeof...(vals));
            PlaceholderBinder& b = *this; // Not sure why I need to upcast in order to get the template methods
            (b.bind(std::forward<Ts>(vals)), ...);
        }

        void bind(std::nullptr_t) override;
        void bind(short value) override;
        void bind(int value) override;
        void bind(long value) override;
        void bind(long long int value) override;
        void bind(unsigned short value) override;
        void bind(unsigned int value) override;
        void bind(unsigned long value) override;
        void bind(unsigned long long int value) override;
        void bind(float value) override;
        void bind(double value) override;
        void bind(std::string_view value) override;
        void bind(const std::pair<const unsigned char*, std::size_t>& data) override;
    };

    std::string sql_;
    PlaceholderValues placeholderValues_;

    public:
    /// \brief Constructs a StatementBuilder from the supplied SQL string and placeholder values
    ///
    /// \tparam Ts The types of the placeholder values. These are normally deducted automatically
    /// \param sql An SQL statement string
    /// \param placeholderValues A list of values to be bound to placeholders in the SQL statement string
    ///
    /// \since v1.0.0
    template <typename... Ts>
    explicit StatementBuilder(std::string_view sql, Ts&&... placeholderValues)
    : sql_(sql)
    , placeholderValues_(std::forward<Ts>(placeholderValues)...)
    {}

    /// \brief Appends a string and placeholder values to the existing SQL statement string and values
    ///
    /// \tparam Ts The types of the placeholder values. These are normally deducted automatically
    /// \param sql An SQL statement string to be appended
    /// \param placeholderValues A list of values to be bound to placeholders in the SQL statement string
    ///
    /// \since v1.0.0
    template <typename... Ts>
    void append(std::string_view sql, Ts&&... placeholderValues)
    {
        sql_ += sql;
        PlaceholderBinder &b = placeholderValues_; // Not sure why I need to upcast in order to get the template methods
        (b.bind(std::forward<Ts>(placeholderValues)), ...);
    }

    /// \brief Returns the current SQL string
    ///
    /// \return The current SQL statement string associated with this object
    ///
    /// \since v1.0.0
    [[nodiscard]]
    inline const std::string& sql() const & {
        return sql_;
    }

    /// \brief Returns the number of placeholder values bound to this object
    ///
    /// \return The number of placeholder values bound to this object
    ///
    /// \since v1.0.0
    [[nodiscard]]
    inline std::size_t valueCount() const {
        return placeholderValues_.values.size();
    }

    private:
    void bindToStatement(Adapter::Statement &stmt) const;
};

}
