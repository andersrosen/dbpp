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

StatementIterator::StatementIterator(Statement* statement)
: stmt_(statement), res_(statement->step()) {
    if (res_.empty()) {
        // Become the end iterator
        stmt_ = nullptr;
        res_ = Result();
    }
}

Statement::Statement(Adapter::StatementPtr p)
: impl_(std::move(p)) {}

Statement::Statement(Statement&& that) noexcept
: impl_(std::move(that.impl_))
{}

Statement& Statement::operator=(Statement&& that) noexcept {
    impl_ = std::move(that.impl_);

    return *this;
}

StatementIterator Statement::begin() {
    return StatementIterator(this);
}

StatementIterator Statement::end() {
    return StatementIterator();
}

void Statement::preBind(std::size_t providedParameterCount) {
    impl_->preBind(providedParameterCount);
}

void Statement::postBind(std::size_t providedParameterCount, std::size_t boundParameterCount) {
    impl_->postBind(providedParameterCount, boundParameterCount);
}

void Statement::doBind(std::nullptr_t) {
    impl_->bindNull();
}

void Statement::doBind(short v) {
    impl_->bind(v);
}

void Statement::doBind(int v) {
    impl_->bind(v);
}

void Statement::doBind(long v) {
    impl_->bind(v);
}

void Statement::doBind(long long v) {
    impl_->bind(v);
}

void Statement::doBind(unsigned short v) {
    impl_->bind(v);
}

void Statement::doBind(unsigned int v) {
    impl_->bind(v);
}

void Statement::doBind(unsigned long v) {
    impl_->bind(v);
}

void Statement::doBind(unsigned long long v) {
    impl_->bind(v);
}

void Statement::doBind(float v) {
    impl_->bind(v);
}

void Statement::doBind(double v) {
    impl_->bind(v);
}

void Statement::doBind(const std::string& v) {
    impl_->bind(v);
}

void Statement::doBind(std::string_view value) {
    impl_->bind(value);
}

void Statement::doBind(const char* v) {
    impl_->bind(std::string{v});
}

void Statement::doBind(const std::pair<const unsigned char*, std::size_t>& bytes) {
    impl_->bind(bytes);
}

Result Statement::step() {
    return Result(impl_->step());
}

std::string Statement::sql() const {
    return impl_->sql();
}

//////////////////////////////////////////////////////////////////////////////

StatementIterator& StatementIterator::operator++() {
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

} // namespace Dbpp

