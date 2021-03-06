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

#include "dbpp/Connection.h"
#include "dbpp/adapter/Connection.h"

namespace Dbpp {

PreparedStatement
Connection::createPreparedStatement(std::string_view sql) const {
    return PreparedStatement(impl_->createPreparedStatement(sql));
}

Statement
Connection::createStatement(std::string_view sql) const {
    return Statement(impl_->createStatement(sql));
}

Connection::Connection(Adapter::ConnectionPtr c)
: impl_(std::move(c))
{}

Connection::Connection(Connection&& that) noexcept
: impl_(std::move(that.impl_))
{}

Connection& Connection::operator=(Connection&& that) noexcept {
    impl_ = std::move(that.impl_);
    return *this;
}

void Connection::begin() {
    impl_->begin();
}

void Connection::commit() {
    impl_->commit();
}

void Connection::rollback() {
    impl_->rollback();
}

const std::string& Connection::adapterName() const {
    return impl_->adapterName();
}

} // namespace Dbpp
