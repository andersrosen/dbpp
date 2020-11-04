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

#include "Connection.h"

#include <filesystem>
#include <functional>
#include <sqlite3.h>

namespace Dbpp::Sqlite3 {

enum class OpenMode {
    ReadOnly = SQLITE_OPEN_READONLY,
    ReadWrite = SQLITE_OPEN_READWRITE,
    ReadWriteCreate = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
};

enum class OpenFlag {
    None = 0,
    Uri = SQLITE_OPEN_URI,
    Memory = SQLITE_OPEN_MEMORY,
    NoMutex = SQLITE_OPEN_NOMUTEX,
    FullMutex = SQLITE_OPEN_FULLMUTEX,
    SharedCache = SQLITE_OPEN_SHAREDCACHE,
    PrivateCache = SQLITE_OPEN_PRIVATECACHE,
};

Connection open(const std::filesystem::path &file, OpenMode mode);
Connection open(const std::filesystem::path &file, OpenMode mode, OpenFlag flags);

void backup(Dbpp::Connection &db, const std::filesystem::path &file, unsigned int pages_per_step, unsigned int sleep_per_step_ms, std::function<void(int,int)> progress_callback);

}
