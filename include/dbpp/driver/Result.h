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

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace Dbpp::Driver {

/// Interface class for the result of a query
class Result {
public:
    virtual bool getColumn(int columnIndex, short& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, int& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, long& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, long long& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, unsigned short& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, unsigned int& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, unsigned long& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, unsigned long long& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, float& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, double& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, std::string& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, std::filesystem::path& outputVariable) = 0;
    virtual bool getColumn(int columnIndex, std::vector<unsigned char>& outputVariable) = 0;

    virtual bool empty() const = 0;
    virtual int columnCount() const = 0;
    virtual std::string columnName(int columnIndex) const = 0;
    virtual int columnIndexByName(std::string_view columnName) const = 0;
    virtual bool isNull(int columnIndex) const = 0;
    virtual long long getInsertId(const std::string& sequenceName) = 0;
};

}
