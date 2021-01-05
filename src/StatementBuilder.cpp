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

#include "dbpp/StatementBuilder.h"

void
Dbpp::StatementBuilder::PlaceholderValues::bind(std::nullptr_t value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(short value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(int value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(long value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(long long int value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(unsigned short value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(unsigned int value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(unsigned long value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(unsigned long long int value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(float value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(double value) {
    values.emplace_back(value);
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(std::string_view value) {
    values.emplace_back(std::string(value));
}

void
Dbpp::StatementBuilder::PlaceholderValues::bind(const std::pair<const unsigned char*, std::size_t>& data) {

    values.emplace_back(std::vector<std::byte>(
        reinterpret_cast<const std::byte*>(data.first), // NOLINT
        reinterpret_cast<const std::byte*>(data.first) + data.second)); // NOLINT
}

void
Dbpp::StatementBuilder::bindToStatement(Adapter::Statement &stmt) const {
    std::size_t boundCount = 0;
    stmt.preBind(placeholderValues_.values.size());
    try {
        for (const auto &val : placeholderValues_.values) {
            std::visit([&boundCount, &stmt](auto&& v) {
              stmt.bind(std::forward<decltype(v)>(v));
              ++boundCount;
            }, val);
        }
    } catch (...) {
        stmt.postBind(placeholderValues_.values.size(), boundCount);
        throw;
    }
    stmt.postBind(placeholderValues_.values.size(), boundCount);
}
