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
#include "dbpp/adapter/Statement.h"

namespace Dbpp {

template<class T>
static void doBind(Adapter::StatementPtr& p, T val, int& placeholder_pos) {
    p->bind(val, placeholder_pos++);
}

void Statement::doReset() {
    impl_->reset();
}

void Statement::clearBindings() {
    impl_->clearBindings();
}

Statement::Statement(Adapter::StatementPtr p)
: impl_(std::move(p)) {}

Statement::Statement(Statement&& that) noexcept
: impl_(std::move(that.impl_)) {}

StatementIterator Statement::begin() {
    return StatementIterator(this);
}

StatementIterator Statement::end() {
    return StatementIterator();
}

void Statement::bindNull() {
    impl_->bindNull(placeholderPosition_++);
}

void Statement::bind(std::nullptr_t) {
    impl_->bindNull(placeholderPosition_++);
}

void Statement::bind(short v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(int v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(long v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(long long v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(unsigned short v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(unsigned int v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(unsigned long v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(unsigned long long v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(float v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(double v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(const std::string& v) {
    impl_->bind(v, placeholderPosition_++);
}

void Statement::bind(std::string_view sv) {
    impl_->bind(sv, placeholderPosition_++);
}

void Statement::bind(const char* v) {
    impl_->bind(std::string{ v }, placeholderPosition_++);
}

void Statement::bind(const std::vector<unsigned char>& v) {
    impl_->bind(v, placeholderPosition_++);
}

Result Statement::step() {
    return std::move(Result(impl_->step()));
}

std::string Statement::sql() const {
    return impl_->sql();
}

}
