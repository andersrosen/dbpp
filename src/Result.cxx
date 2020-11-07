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

#include "dbpp/adapter/Result.h"
#include "dbpp/adapter/Types.h"
#include "dbpp/Result.h"

template <class T>
static bool doGet(Dbpp::Adapter::ResultPtr& p, T& out, int index) {
    if (!p)
        throw Dbpp::Error("Attempted access of values in empty Result");
    return p->getColumn(index, out);

}

namespace Dbpp {

Result::Result(Adapter::ResultPtr p)
    : impl(std::move(p))
{}

Result::Result(Dbpp::Result &&that) noexcept
    : impl(std::move(that.impl))
{}

Result& Result::operator=(Dbpp::Result &&that) noexcept
{
    impl = std::move(that.impl);
    return *this;
}

bool Result::isNull(int columnIndex) const {
    return impl->isNull(columnIndex);
}

bool Result::get(int index, short& out) { return doGet(impl, out, index); }
bool Result::get(int index, int& out) { return doGet(impl, out, index); }
bool Result::get(int index, long& out) { return doGet(impl, out, index); }
bool Result::get(int index, long long& out) { return doGet(impl, out, index); }
bool Result::get(int index, unsigned short& out) { return doGet(impl, out, index); }
bool Result::get(int index, unsigned int& out) { return doGet(impl, out, index); }
bool Result::get(int index, unsigned long& out) { return doGet(impl, out, index); }
bool Result::get(int index, unsigned long long& out) { return doGet(impl, out, index); }
bool Result::get(int index, float& out) { return doGet(impl, out, index); }
bool Result::get(int index, double& out) { return doGet(impl, out, index); }
bool Result::get(int index, std::string& out) { return doGet(impl, out, index); }
bool Result::get(int index, std::vector<unsigned char>& out) { return doGet(impl, out, index); }
bool Result::get(int index, std::filesystem::path& out) { return doGet(impl, out, index); }

bool Result::empty() const {
    if (!impl)
        return false;
    return impl->empty();
}

int Result::columnCount() const {
    if (!impl)
        throw Error("Empty Result");
    return impl->columnCount();
}

std::string Result::columnName(int columnIndex) const {
    if (!impl)
        throw Error("Empty Result");
    return impl->columnName(columnIndex);
}

bool Result::hasColumn(std::string_view colname) const {
    if (!impl)
        return false;
    auto idx = impl->columnIndexByName(colname);
    return idx >= 0;
}

int Result::columnIndexByName(std::string_view columnName) const {
    if (!impl)
        throw Error("Empty Result");
    auto idx = impl->columnIndexByName(columnName);
    if (idx < 0)
        throw Error(std::string("Result has no column named ") + std::string(columnName));
    return idx;
}

}