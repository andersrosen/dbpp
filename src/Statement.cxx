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

#include "dbpp/Statement.h"
#include "dbpp/driver/Statement.h"

namespace Dbpp {

template<class T>
static void do_bind(Driver::StatementPtr& p, T val, int& placeholder_pos) {
    p->bind(val, placeholder_pos++);
}

Statement::Statement(Driver::StatementPtr p)
: impl(std::move(p)) {}

Statement::Statement(Statement&& that) noexcept
: impl(std::move(that.impl)) {}

StatementIterator Statement::begin() {
    return StatementIterator(this);
}

StatementIterator Statement::end() {
    return StatementIterator();
}

void Statement::bindNull() {
    impl->bindNull(placeholderPosition++);
}

void Statement::bind(std::nullptr_t) {
    impl->bindNull(placeholderPosition++);
}

void Statement::bind(short v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(int v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(long v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(long long v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(unsigned short v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(unsigned int v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(unsigned long v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(unsigned long long v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(float v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(double v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(const std::string& v) {
    impl->bind(v, placeholderPosition++);
}

void Statement::bind(std::string_view sv) {
    impl->bind(sv, placeholderPosition++);
}

void Statement::bind(const char* v) {
    impl->bind(std::string{ v }, placeholderPosition++);
}

void Statement::bind(const std::vector<unsigned char>& v) {
    impl->bind(v, placeholderPosition++);
}

Result Statement::step() {
    return std::move(Result(impl->step()));
}

std::string Statement::sql() const {
    return impl->sql();
}

}
