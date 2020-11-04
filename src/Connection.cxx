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
#include "dbpp/driver/Connection.h"

namespace Dbpp {

Connection::Connection(Driver::ConnectionPtr c)
: impl(std::move(c)) {}

Connection::Connection(Connection&& that) noexcept
: impl(std::move(that.impl)) {}

Connection& Connection::operator=(Connection&& that) {
    impl = std::move(that.impl);
    return *this;
}

Statement Connection::prepare(const std::string& sql) {
    return std::move(Statement(impl->prepare(sql)));
}

void Connection::begin() {
    impl->begin();
}

void Connection::commit() {
    impl->commit();
}

void Connection::rollback() {
    impl->rollback();
}

const std::string& Connection::driverName() const {
    return impl->driverName();
}

} // namespace Dbpp
