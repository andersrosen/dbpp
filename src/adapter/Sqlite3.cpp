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
#include "dbpp/Exception.h"
#include "dbpp/Sqlite3.h"
#include "dbpp/adapter/Connection.h"
#include "dbpp/adapter/Result.h"
#include "dbpp/adapter/Statement.h"

#include <filesystem>
#include <functional>
#include <limits>
#include <string_view>
#include <unordered_map>

namespace Dbpp::Sqlite3 {

using Sqlite3HandleT = std::shared_ptr<struct sqlite3>;
using StmtHandleT = std::shared_ptr<sqlite3_stmt>;

class Sqlite3Error final : public Dbpp::ErrorWithCode {
public:
    using ErrorWithCode::ErrorWithCode;

    const char* what() const noexcept override  {
        static std::string msg;
        msg = std::string(ErrorWithCode::what()) + ": " + sqlite3_errstr(static_cast<int>(code));
        return msg.c_str();
    }
};

static void throwOnError(int errcode, const std::string& message) {
    if (errcode != SQLITE_OK)
        throw Sqlite3Error(errcode, message);
}

class Result;

struct ColInfo {
    int numCols = 0;
    bool namesAvailable = false;
    std::unordered_map<std::string, int> names;
};
using ColInfoPtr = std::shared_ptr<ColInfo>;

class Result final : public Adapter::Result {
private:
    Sqlite3HandleT connection_;
    StmtHandleT stmt_;
    ColInfoPtr colInfo_;
    int colIndex_ = -1;

    template <class T>
    bool doGetInt(T& out, int index = -1) {
        if (index < 0)
            index = ++colIndex_;
        if (isNull(index))
            return false;

        sqlite3_int64 val = sqlite3_column_int64(stmt_.get(), index);

        if (std::numeric_limits<T>::is_signed) {
            T tmp = static_cast<T>(val);
            if (static_cast<sqlite3_int64>(tmp) != val)
                throw std::bad_cast();
            out = tmp;
        } else {
            auto uval = static_cast<unsigned long long>(val);
            T tmp = static_cast<T>(uval);
            if (static_cast<unsigned long long>(tmp) != uval)
                throw std::bad_cast();
            out = tmp;
        }
        return true;
    }

    template <class T>
    bool doGetReal(T& out, int index = -1) {
        if (index < 0)
            index = ++colIndex_;
        if (isNull(index))
            return false;

        out = static_cast<T>(sqlite3_column_double(stmt_.get(), index));
        return true;
    }

    bool doGetBlob(std::vector<unsigned char>& var, int index = -1) {
        if (index < 0)
            index = ++colIndex_;
        if (isNull(index))
            return false;

        auto datap = static_cast<const unsigned char *>(sqlite3_column_blob(stmt_.get(), index));
        auto datasize = sqlite3_column_bytes(stmt_.get(), index);
        var.clear();
        var.reserve(static_cast<std::vector<unsigned char>::size_type>(datasize));
        var.insert(var.end(), datap, datap + datasize);
        return true;
    }

public:
    Result(Sqlite3HandleT connection, StmtHandleT stmt, ColInfoPtr colinfo)
            : connection_(std::move(connection)), stmt_(std::move(stmt)), colInfo_(std::move(colinfo)) {
    }

    bool getColumn(int index, short& var) override { return doGetInt(var, index); }
    bool getColumn(int index, int& var) override { return doGetInt(var, index); }
    bool getColumn(int index, long& var) override { return doGetInt(var, index); }
    bool getColumn(int index, long long& var) override { return doGetInt(var, index); }
    bool getColumn(int index, unsigned short& var) override { return doGetInt(var, index); }
    bool getColumn(int index, unsigned int& var) override { return doGetInt(var, index); }
    bool getColumn(int index, unsigned long& var) override { return doGetInt(var, index); }
    bool getColumn(int index, unsigned long long& var) override { return doGetInt(var, index); }
    bool getColumn(int index, float& var) override { return doGetReal(var, index); }
    bool getColumn(int index, double& var) override { return doGetReal(var, index); };

    bool getColumn(int index, std::string& var) override {
        if (isNull(index))
            return false;
        var = reinterpret_cast<const char *>(sqlite3_column_text(stmt_.get(), index));
        return true;
    }

    bool getColumn(int index, std::filesystem::path& var) override {
        if (isNull(index))
            return false;
        var = reinterpret_cast<const char *>(sqlite3_column_text(stmt_.get(), index));
        return true;
    }

    bool getColumn(int index, std::vector<unsigned char>& var) override {
        return doGetBlob(var, index);
    }

    bool empty() const override {
        return sqlite3_data_count(stmt_.get()) <= 0;
    }

    int columnCount() const override {
        return colInfo_->numCols;
    }

    std::string columnName(int index) const override {
        return sqlite3_column_name(stmt_.get(), index);
    }

    int columnIndexByName(std::string_view name) const override {
        if (!colInfo_->namesAvailable) {
            for (int i=0; i<colInfo_->numCols; ++i) {
                colInfo_->names[std::string(sqlite3_column_name(stmt_.get(), i))] = i;
            }
            colInfo_->namesAvailable = true;
        }
        auto it = colInfo_->names.find(std::string(name));
        if (it == colInfo_->names.end())
            return -1;
        return it->second;
    }

    bool isNull(int index) const override {
        if (index < 0 || index >= colInfo_->numCols)
            throw std::runtime_error("Column index out of bounds");
        if (empty())
            throw std::runtime_error("Attempted column access in empty result");
        return sqlite3_column_type(stmt_.get(), index) == SQLITE_NULL;
    }

    long long getInsertId(std::string_view /* sequenceName */) override {
        // FIXME: Non-empty sequenceName?
        return sqlite3_last_insert_rowid(connection_.get());
    }
};

class Statement final : public Adapter::Statement {
private:
    Sqlite3HandleT connection_handle;
    StmtHandleT handle;
    ColInfoPtr colInfo;

    static void throwOnBindError(int errcode) {
        if (errcode == SQLITE_RANGE)
            throw PlaceholderOutOfRange("Binding value to non-existent placeholder");
        throwOnError(errcode, "Error when binding value to placeholder");
    }

    template <class T>
    void do_bind_int(T val, int position) {
        if constexpr(sizeof(T) >= sizeof(sqlite3_int64)) {
            if (val > static_cast<T>(std::numeric_limits<sqlite3_int64>::max()))
                throw Error("Can't bind value, since it's larger than the greatest signed 64-bit integer");
        }

        int res = sqlite3_bind_int64(handle.get(), position+1, static_cast<sqlite3_int64>(val));
        throwOnBindError(res);
    }

public:
    Statement(Sqlite3HandleT conn, std::string_view sql)
    : connection_handle(std::move(conn)) {
        sqlite3_stmt *stmt;
        int res = sqlite3_prepare_v2(connection_handle.get(),
                sql.data(), static_cast<int>(sql.length()),
                &stmt, nullptr);
        throwOnError(res, std::string{"Failed to prepare statement "} + std::string{sql});
        handle = StmtHandleT(stmt, sqlite3_finalize);
        colInfo = std::make_shared<ColInfo>();
        colInfo->numCols = sqlite3_column_count(handle.get());
    }

    void bindNull(int position) override {
        int res = sqlite3_bind_null(handle.get(), position+1);
        throwOnBindError(res);
    }
    void bind(short val, int position) override { do_bind_int(val, position); }
    void bind(int val, int position) override { do_bind_int(val, position); }
    void bind(long val, int position) override { do_bind_int(val, position); }
    void bind(long long val, int position) override { do_bind_int(val, position); }
    void bind(unsigned short val, int position) override { do_bind_int(val, position); }
    void bind(unsigned int val, int position) override { do_bind_int(val, position); }
    void bind(unsigned long val, int position) override { do_bind_int(val, position); }
    void bind(unsigned long long val, int position) override { do_bind_int(val, position); }
    void bind(float val, int position) override {
        int res = sqlite3_bind_double(handle.get(), position+1,
                                      static_cast<double>(val));
        throwOnBindError(res);
    }
    void bind(double val, int position) override {
        int res = sqlite3_bind_double(handle.get(), position+1, val);
        throwOnBindError(res);
    }
    void bind(const std::string &val, int position) override {
        int res = sqlite3_bind_text(handle.get(), position + 1, val.c_str(),
                                    static_cast<int>(val.length()), SQLITE_TRANSIENT);
        throwOnBindError(res);
    }
    void bind(std::string_view val, int position) override {
        int res = sqlite3_bind_text(handle.get(), position + 1, val.data(),
                                static_cast<int>(val.length()), SQLITE_TRANSIENT);
        throwOnBindError(res);
    }
    void bind(const std::vector<unsigned char> &val, int position) override {
        int res = sqlite3_bind_blob(handle.get(), position + 1,
                                    val.data(), static_cast<int>(val.size()), SQLITE_TRANSIENT);
        throwOnBindError(res);
    }

    std::string sql() const override {
        return sqlite3_sql(handle.get());
    }

    Adapter::ResultPtr step() override {
        int res = sqlite3_step(handle.get());
        if (res != SQLITE_DONE && res != SQLITE_ROW)
            throwOnError(res, "Failed to step/execute statement");
        return std::make_shared<Result>(connection_handle, handle, colInfo);
    }

    void reset() override {
        int res = sqlite3_reset(handle.get());
        throwOnError(res, "Failed to reset statement");
    }

    void clearBindings() override {
        int res = sqlite3_clear_bindings(handle.get());
        throwOnError(res, "Failed to clear statement bindings");
    }

};

class Connection final : public Adapter::Connection {
    friend Dbpp::Connection open(const std::filesystem::path &file, Sqlite3::OpenMode mode, Sqlite3::OpenFlag flags);
public:

private:
    Sqlite3HandleT handle;

public:
    Connection(const std::filesystem::path &filename, OpenMode mode, OpenFlag flags) {
        struct sqlite3 *conn;
        int res = sqlite3_open_v2(filename.c_str(), &conn, static_cast<int>(static_cast<unsigned int>(mode) | static_cast<unsigned int>(flags)), nullptr);
        if (res != SQLITE_OK) {
            if (conn != nullptr)
                sqlite3_close(conn);
            throw Sqlite3Error(res, "Failed to open database");
        }
        handle = Sqlite3HandleT(conn, sqlite3_close_v2);
    }

    const std::string &driverName() const override {
        static const std::string name{"sqlite3"};
        return name;
    }

    void begin() override {
        prepare("BEGIN")->step();
    }

    void commit() override {
        prepare("COMMIT")->step();
    }

    void rollback() override {
        prepare("ROLLBACK")->step();
    }

    Adapter::StatementPtr prepare(std::string_view sql) override {
        return std::make_shared<Statement>(handle, sql);
    }

    static std::shared_ptr<Connection> getImpl(Dbpp::Connection& db) {
        return std::dynamic_pointer_cast<Connection>(Adapter::Connection::getImpl(db));
    }

    void backup(const std::filesystem::path &file, int pagesPerStep, int sleepPerStepMs, std::function<void(int,int)> &progress_callback) {
        struct db_deleter {
            void operator()(struct sqlite3 *p) { sqlite3_close(p); }
        };
        std::unique_ptr<struct sqlite3, db_deleter> db_handle;
        {
            struct sqlite3 *db;
            auto res = sqlite3_open(file.c_str(), &db);
            if (res != SQLITE_OK) {
                if (db)
                    sqlite3_close(db);
                throwOnError(res, "Failed to open backup file");
            }
            db_handle.reset(db);
        }

        struct backup_deleter {
            void operator()(struct sqlite3_backup *backup_handle) { sqlite3_backup_finish(backup_handle); }
        };
        std::unique_ptr<struct sqlite3_backup, backup_deleter> backup_handle;
        backup_handle.reset(sqlite3_backup_init(db_handle.get(), "main", handle.get(), "main"));
        if (!backup_handle) {
            auto res = sqlite3_errcode(db_handle.get());
            throwOnError(res, "Failed to create backup handle");
        }

        for (;;) {
            auto res = sqlite3_backup_step(backup_handle.get(), pagesPerStep);
            if (res == SQLITE_OK)
                progress_callback(sqlite3_backup_remaining(backup_handle.get()),
                                  sqlite3_backup_pagecount(backup_handle.get()));
            if (res == SQLITE_OK || res == SQLITE_BUSY || res == SQLITE_LOCKED)
                sqlite3_sleep(sleepPerStepMs);
            else if (res == SQLITE_DONE)
                break;
            else
                throwOnError(res, "Backup operation failed");
        }
    }
};

Dbpp::Connection open(const std::filesystem::path &file, OpenMode mode, OpenFlag flags) {
    return Adapter::ConnectionPtr(new Sqlite3::Connection(file, mode, flags));
}

Dbpp::Connection open(const std::filesystem::path &file, OpenMode mode) {
    return open(file, mode, OpenFlag::None);
}

Dbpp::Connection open(const std::filesystem::path &file) {
return open(file, OpenMode::ReadWriteCreate, OpenFlag::None);
}

void backup(Dbpp::Connection &db, const std::filesystem::path &file, int pagesPerStep, int sleepTimePerStepMs, std::function<void(int,int)> progressCallback) {
    if (db.driverName() != "sqlite3")
        throw Error("dbpp::sqlite3::backup() can only be called with an sqlite3 connection");
    auto impl = Sqlite3::Connection::getImpl(db);
    impl->backup(file, pagesPerStep, sleepTimePerStepMs, progressCallback);
}

} // namespace Dbpp::Sqlite3
