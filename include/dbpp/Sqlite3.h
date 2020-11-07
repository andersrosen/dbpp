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

/// \brief Namespace specific to the SQLite3 adapter
///
/// \since v1.0.0
namespace Dbpp::Sqlite3 {

/// \brief Specifies the mode in which to open an SQLite3 database
///
/// \since v1.0.0
enum class OpenMode : unsigned int {
    ReadOnly = SQLITE_OPEN_READONLY, /// Open it in read only mode. Fail if it does not exist
    ReadWrite = SQLITE_OPEN_READWRITE, /// Open it in read-write mode. Fail if it does not exist
    ReadWriteCreate = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, /// Open it in read-write mode, and create it if it does not exist
};

/// \brief Specifies how to open an SQLite3 database
///
/// \since v1.0.0
enum class OpenFlag : unsigned int {
    None = 0,
    Uri = SQLITE_OPEN_URI, /// The filename can be interpreted as a URI if this flag is set
    Memory = SQLITE_OPEN_MEMORY, /// The database will be opened as an in-memory database
    NoMutex = SQLITE_OPEN_NOMUTEX, /// The new database connection will use the "multi-thread" threading mode. This means that separate threads are allowed to use SQLite at the same time, as long as each thread is using a different database connection
    FullMutex = SQLITE_OPEN_FULLMUTEX, /// The new database connection will use the "serialized" threading mode. This means the multiple threads can safely attempt to use the same database connection at the same time
    SharedCache = SQLITE_OPEN_SHAREDCACHE, /// The database is opened with shared cache enabled, overriding the default shared cache setting
    PrivateCache = SQLITE_OPEN_PRIVATECACHE, /// The database is opened with shared cache disabled, overriding the default shared cache setting

#ifdef SQLITE_OPEN_NOFOLLOW
    NoFollow = SQLITE_OPEN_NOFOLLOW, /// The database filename is not allowed to be a symbolic link
#endif
};

/// \brief Opens an SQLite3 database, and returns a connection to it
///
/// The database will be opened in read-write mode, and it will be created if it does not exist
///
/// \param file The filename of the database to open
/// \return A connection to the database
///
/// \since v1.0.0
Connection open(const std::filesystem::path &file);

/// \brief Opens an SQLite3 database, and returns a connection to it
///
/// \param file The filename of the database to open
/// \param mode Specifies if it should be opened read only, read-write, and if it should be created if it does not exist
/// \return A connection to the database
///
/// \since v1.0.0
Connection open(const std::filesystem::path &file, OpenMode mode);

/// \brief Opens an SQLite3 database, and returns a connection to it
///
/// \param file The filename or URI of the database to open
/// \param mode Specifies if it should be opened read only, read-write, and if it should be created if it does not exist
/// \param flags Flags affecting how the database is opened
/// \return A connection to the database
///
/// \since v1.0.0
Connection open(const std::filesystem::path &file, OpenMode mode, OpenFlag flags);

/// \brief Backs up an SQLite3 database
///
/// \param db A connection to the database that should be backed up
/// \param file The destination file in which to store the backup
/// \param pagesPerStep Number of pages to back up per step, before sleeping
/// \param sleepPerStepMs How long to sleep, in milliseconds, between the steps
///
/// \since v1.0.0
void backup(Dbpp::Connection &db, const std::filesystem::path &file, int pagesPerStep, int sleepPerStepMs);

/// \brief Backs up an SQLite3 database
///
/// \param db A connection to the database that should be backed up
/// \param file The destination file in which to store the backup
/// \param pagesPerStep Number of pages to back up per step, before sleeping and reporting progress
/// \param sleepPerStepMs How long to sleep, in milliseconds, between the steps
/// \param progressCallback A callback that will be called after each step, to report progress to the caller
///
/// \since v1.0.0
void backup(Dbpp::Connection &db, const std::filesystem::path &file, int pagesPerStep, int sleepPerStepMs, std::function<void(int,int)> progressCallback);

}
