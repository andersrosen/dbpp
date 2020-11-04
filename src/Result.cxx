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

#include "dbpp/driver/Result.h"
#include "dbpp/driver/Types.h"
#include "dbpp/Result.h"

template <class T>
static bool doGet(Dbpp::Driver::ResultPtr& p, T& out, int index) {
    if (!p)
        throw Dbpp::Error("Attempted access of values in empty Result");
    return p->getColumn(index, out);

}

namespace Dbpp {

Result::Result(Driver::ResultPtr p)
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

bool Result::isNull(int colindex) {
    return impl->isNull(colindex);
}

bool Result::get(short &out, int index) { return doGet(impl, out, index); }
bool Result::get(int &out, int index) { return doGet(impl, out, index); }
bool Result::get(long &out, int index) { return doGet(impl, out, index); }
bool Result::get(long long &out, int index) { return doGet(impl, out, index); }
bool Result::get(unsigned short &out, int index) { return doGet(impl, out, index); }
bool Result::get(unsigned int &out, int index) { return doGet(impl, out, index); }
bool Result::get(unsigned long &out, int index) { return doGet(impl, out, index); }
bool Result::get(unsigned long long &out, int index) { return doGet(impl, out, index); }
bool Result::get(float &out, int index) { return doGet(impl, out, index); }
bool Result::get(double &out, int index) { return doGet(impl, out, index); }
bool Result::get(std::string &out, int index) { return doGet(impl, out, index); }
bool Result::get(std::vector<unsigned char> &out, int index) { return doGet(impl, out, index); }
bool Result::get(std::filesystem::path &out, int index) { return doGet(impl, out, index); }

bool Result::empty() {
    if (!impl)
        return false;
    return impl->empty();
}

int Result::columnCount() {
    if (!impl)
        throw Error("Empty Result");
    return impl->columnCount();
}

std::string Result::columnName(int index) {
    if (!impl)
        throw Error("Empty Result");
    return impl->columnName(index);
}

bool Result::hasColumn(std::string_view colname) {
    if (!impl)
        return false;
    auto idx = impl->columnIndexByName(colname);
    return idx >= 0;
}

int Result::columnIndex(std::string_view colname) {
    if (!impl)
        throw Error("Empty Result");
    auto idx = impl->columnIndexByName(colname);
    if (idx < 0)
        throw Error(std::string("Result has no column named ") + std::string(colname));
    return idx;
}

}