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

#include "Types.h"

#include <string>
#include <vector>

namespace Dbpp::Driver {

class Statement {
public:
    virtual void bindNull(int placeholderIndex) = 0;
    virtual void bind(short value, int placeholderIndex) = 0;
    virtual void bind(int value, int placeholderIndex) = 0;
    virtual void bind(long value, int placeholderIndex) = 0;
    virtual void bind(long long value, int placeholderIndex) = 0;
    virtual void bind(unsigned short value, int placeholderIndex) = 0;
    virtual void bind(unsigned int value, int placeholderIndex) = 0;
    virtual void bind(unsigned long value, int placeholderIndex) = 0;
    virtual void bind(unsigned long long value, int placeholderIndex) = 0;
    virtual void bind(float value, int placeholderIndex) = 0;
    virtual void bind(double value, int placeholderIndex) = 0;
    virtual void bind(const std::string &value, int placeholderIndex) = 0;
    virtual void bind(std::string_view value, int placeholderIndex) = 0;
    virtual void bind(const std::vector<unsigned char> &blobValue, int placeholderIndex) = 0;

    virtual std::string sql() const = 0;

    virtual ResultPtr step() = 0;
};

}
