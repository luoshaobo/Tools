/**
 * Copyright (C) 2017-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     http_cache_db.cc
 *  \brief    HTTP cache_db engine implementation.
 *  \author   Maksym Mozok
 *
 *  \addtogroup transfermanager
 *  \{
 */

/// ** INCLUDES *****************************************************************
#include "http_cache_db.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <ctime>
#include <atomic>
#include <functional>
#include <sqlite3.h>

#include "persistence_client_library.h"
#include "fsm_persist_data_mgr.h"
#include "sql_defines.h"


//default flags
const int32_t kSqlite3Flags = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

namespace fsm
{

namespace transfermanager
{

HttpCacheDb::HttpCacheDb():db_handle_(nullptr),extended_sql_error_(0)
{
    fsm::fsmpersistdatamgr::FsmPersistDataMgr persistant_data_manager;
    if(persistant_data_manager.FsmPersistDataInit())
    {
        db_name_ = persistant_data_manager.FsmPersistDataGet(kDbNameKey);
        db_path_ = persistant_data_manager.FsmPersistDataPathGet(E_PCL_LDBID_PUBLIC);
        Init(db_path_,db_name_);
    }
}

HttpCacheDb::HttpCacheDb(const std::string path_to_db,
                         const std::string db_name):db_handle_(nullptr),
                                                    extended_sql_error_(0)
{
    fsm::fsmpersistdatamgr::FsmPersistDataMgr persistent_data_manager;

    if(!persistent_data_manager.FsmPersistDataInit())
    {
        return;
    }
    else
    {
       if( path_to_db.empty() )
       {
           //Read and save path from persistence
           db_path_ = persistent_data_manager.FsmPersistDataPathGet(E_PCL_LDBID_PUBLIC);
       }
       else
       {
           db_path_= path_to_db;
       }

       if( db_name.empty() )
       {
           //Read and save db name from persistence
           db_name_ = persistent_data_manager.FsmPersistDataGet(kDbNameKey);
       }
       else
       {
           db_name_ = db_name;
       }

       Init(db_path_, db_name_);
    }
}

HttpCacheDb::~HttpCacheDb()
{
    Done();
}

DbErrorCode HttpCacheDb::ErrorSet(int_fast32_t sqlerror)
{
    DbErrorCode rc = DbErrorCode::kNone;
    extended_sql_error_.store(sqlerror, std::memory_order_release);

    switch(sqlerror)
    {
        case SQLITE_OK:
        rc = DbErrorCode::kNone;
            break;
        case SQLITE_ERROR:
        rc = DbErrorCode::kUndefined;
            break;
        case SQLITE_INTERNAL:
        rc = DbErrorCode::kInternalError;
            break;
        case SQLITE_PERM:
        rc = DbErrorCode::kPermisionError;
            break;
        case SQLITE_ABORT:
        rc = DbErrorCode::kAbortError;
            break;
        case SQLITE_BUSY:
        rc = DbErrorCode::kBusyError;
            break;
        case SQLITE_NOMEM:
        rc = DbErrorCode::kNoMemError;
            break;
        case SQLITE_READONLY:
        rc = DbErrorCode::kROError;
            break;
        case SQLITE_IOERR:
        rc = DbErrorCode::kIOError;
            break;
        case SQLITE_CORRUPT:
        rc = DbErrorCode::kDBCorruptError;
            break;
        case SQLITE_FULL:
        rc = DbErrorCode::kFsFullError;
            break;
        case SQLITE_ROW:
        rc = DbErrorCode::kRowReturn;
            break;
        case SQLITE_DONE:
        rc = DbErrorCode::kDoneReturn;
            break;
        default:
        rc = DbErrorCode::kMax;
            break;
    }
    return rc;
}

int HttpCacheDb::GetColumnType(std::string &columnname)
{
    int rc = 0;

    if(columnname == kSqlUrl ||
        columnname == kSqlEtag)
    {
        rc = SQLITE3_TEXT;
    }

    if(columnname == kSqlRcvdTimestamp ||
        columnname == kSqlExpTimestamp ||
        columnname == kSqlBlobDataOffset ||
        columnname == kSqlValid)
    {
        rc = SQLITE_INTEGER;
    }

    if(columnname == kSqlDatablob)
    {
        rc = SQLITE_BLOB;
    }

    return rc;
}

DbErrorCode HttpCacheDb::GetRowIdByUrl(const std::string &_url, int64_t &rowid)
{
    DbErrorCode rc = DbErrorCode::kNone;
    sqlite3_stmt *statement = nullptr;
    int8_t retry_counter =10;
    std::string select_str = kSqlGetROWID;                  //bind ?1 parameter

    if(rc == DbErrorCode::kNone)
    {
        extended_sql_error_= sqlite3_prepare_v2(db_handle_,
            select_str.c_str(),
            static_cast<int>(select_str.size()),
            &statement,
            nullptr);
        rc = ErrorSet(extended_sql_error_);

        if(DbErrorCode::kNone == rc)
        {
            extended_sql_error_= sqlite3_bind_text(statement,
                1,
                static_cast<const char*>(_url.c_str()),
                static_cast<int>(_url.size()),
                SQLITE_TRANSIENT);
            rc = ErrorSet(extended_sql_error_);
        }

        if(DbErrorCode::kNone == rc)
        {
            do
            {
                extended_sql_error_ = sqlite3_step(statement);

                if(DbErrorCode::kRowReturn == ErrorSet(extended_sql_error_))
                {
                    rowid = sqlite3_column_int64(statement,kColumnZeroIdx);
                    break;
                }
                if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
                    break;
                --retry_counter;
            }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
        }

        if(statement)
        {
            sqlite3_finalize(statement);
        }
    }
    return ErrorSet(extended_sql_error_);
}

DbErrorCode HttpCacheDb::GetDbRecordByRowId(const int64_t rowid, std::shared_ptr<CacheDbRecord> &data_to_get)
{
    DbErrorCode rc = DbErrorCode::kNone;
    CacheDbRecord data_to_select;
    int8_t retry_counter =10;
    std::string _url;
    std::vector<char> datablob;
    sqlite3_stmt *statement = nullptr;
    std::string etags;
    const unsigned char *rec = nullptr;
    const void *rec_blob = nullptr;
    const char *ptr= nullptr;
    size_t recsize = 0;
    std::string select_str = kSqlGetByROWID;               //bind ?1 parameter

    if(rowid > lastinsertedrowid_.load(std::memory_order_acquire))
    {
        rc = DbErrorCode::kInternalError;
    }

    if(rc == DbErrorCode::kNone)
    {
        extended_sql_error_= sqlite3_prepare_v2(db_handle_,
            select_str.c_str(),
            static_cast<int>(select_str.size()),
            &statement,
            nullptr);
        rc = ErrorSet(extended_sql_error_);

        if(DbErrorCode::kNone == rc)
        {
            extended_sql_error_ = sqlite3_bind_int64(statement,
                1,
                rowid);
            rc = ErrorSet(extended_sql_error_);
        }
        if(DbErrorCode::kNone == rc)
        {
            do
            {
                extended_sql_error_ = sqlite3_step(statement);

                if(DbErrorCode::kRowReturn == ErrorSet(extended_sql_error_))
                {//Record found in db
                    //url
                    rec = sqlite3_column_text(statement,kUrlIdx);
                    recsize = static_cast<size_t>(sqlite3_column_bytes(statement,kUrlIdx));
                    std::get<kUrlIdx>(data_to_select) = _url.assign(reinterpret_cast<const char*>(rec),recsize);
                    //receive timestamp
                    std::get<kRcvTimestampIdx>(data_to_select) = sqlite3_column_int64(statement,kRcvTimestampIdx);
                    //expiration timestamp
                    std::get<kExpTimestampIdx>(data_to_select) = sqlite3_column_int64(statement,kExpTimestampIdx);
                    //Blob extract
                    rec_blob = sqlite3_column_blob(statement,kBlobIdx);
                    recsize = static_cast<size_t>(sqlite3_column_bytes(statement,kBlobIdx));
                    ptr = static_cast<const char*>(rec_blob);
                    std::get<kBlobIdx>(data_to_select).assign(ptr,(ptr+recsize));
                    std::get<kBlobOffset>(data_to_select) =
                    static_cast<std::size_t>(sqlite3_column_int(statement, kBlobOffset));
                    //Etags extract
                    rec = sqlite3_column_text(statement,kEtagIdx);
                    recsize = static_cast<size_t>(sqlite3_column_bytes(statement,kEtagIdx));
                    std::get<kEtagIdx>(data_to_select) = etags.assign(reinterpret_cast<const char*>(rec),recsize);
                    //Validity flag
                    std::get<kValidIdx>(data_to_select) = sqlite3_column_int(statement,kValidIdx);
                    break; //Found data in returned resultset
                }
                if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
                {
                    //No data found
                    break;
                }
                --retry_counter;
            }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
        }
        if(DbErrorCode::kRowReturn == ErrorSet(extended_sql_error_))
        {
            //Make extracted data
            data_to_get = std::make_shared<CacheDbRecord>(std::move(data_to_select));
        }

        if(statement)
        {
            sqlite3_finalize(statement);
        }
    }
    return ErrorSet(extended_sql_error_);
}

DbErrorCode HttpCacheDb::Init(const std::string path_to_db, const std::string db_name)
{
    std::string dbpath = std::move(path_to_db);
    std::string dbname = std::move(db_name);
    //trying to prevent wrong (strlen + \0) size
    if('\0' == dbpath.at(dbpath.size()-1))  //Workaround for non properly set buffer size from tpPCL_getWriteablePath
    {
        dbpath.pop_back();
        dbpath.shrink_to_fit();
    }
    if('\0' == dbname.at(dbname.size()-1))
    {
        dbname.pop_back();
        dbname.shrink_to_fit();
    }

    if(nullptr != db_handle_)
    {
        extended_sql_error_ = sqlite3_close_v2(db_handle_);
        db_handle_ = nullptr;
    }

    extended_sql_error_ = sqlite3_open_v2((dbpath + "/" + dbname).c_str(), &db_handle_, kSqlite3Flags, NULL);

    if(fsm::transfermanager::DbErrorCode::kNone == ErrorSet(extended_sql_error_))
    {
        extended_sql_error_ = sqlite3_exec(db_handle_,
            (kSqlCreate +kSqlIfNotExists + kCacheTableName + kSqlTableSstruct).c_str(), nullptr, nullptr, nullptr);
    }
    return ErrorSet(extended_sql_error_);
}

void HttpCacheDb::Done(void)
{
    if(nullptr != db_handle_)
    {
        extended_sql_error_ = sqlite3_close_v2(db_handle_);
        db_handle_ = nullptr;
    }
    return;
}

DbErrorCode HttpCacheDb::AddDbRecord(CacheDbRecord &data_to_add)
{
    int8_t retry_counter =10;
    std::string add_cmd_str;
    std::string blob_cmd_str;
    std::string _url;
    sqlite3_stmt *statement = nullptr;
    std::time_t received_time_t = 0;
    std::time_t exp_time_t = 0;
    std::vector<char> _blob_t;
    std::size_t size_t_offset = 0;
    std::string etags;
    bool _valid = false;

    //extract data from tuple
    std::tie(_url,received_time_t,exp_time_t,_blob_t,size_t_offset,etags,_valid) = data_to_add;

    add_cmd_str = kSqlInsertReplace + kCacheTableName + " (" + kSqlUrl + "," + kSqlRcvdTimestamp +
    "," + kSqlExpTimestamp + "," + kSqlDatablob + ","+ kSqlBlobDataOffset +","
    + kSqlEtag + "," + kSqlValid + ") " + "VALUES ( $1,$2,$3,$4,$5,$6,$7 )";

    extended_sql_error_ =sqlite3_prepare_v2(db_handle_,
        add_cmd_str.c_str(),
        static_cast<int>(add_cmd_str.size()),
        &statement,
        nullptr);

    if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
    {
        extended_sql_error_ = sqlite3_bind_text(statement,
            1, // Must match $1 at add_cmd_str
            static_cast<const char*>(_url.c_str()),
            static_cast<int>(_url.size()),
            SQLITE_TRANSIENT);
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int64(statement,
                2,// Must match $2 at add_cmd_str
                received_time_t);
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int64(statement,
                3,// Must match $3 at add_cmd_str
                exp_time_t);
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_blob(statement,
                4,// Must match $4 at add_cmd_str
                static_cast<const char*>(_blob_t.data()),
                static_cast<int>(_blob_t.size()),
                SQLITE_TRANSIENT);
        }
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int(statement,
                5,// Must match $5 at add_cmd_str
                static_cast<std::size_t>(size_t_offset));
        }
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_text(statement,
                6,// Must match $6 at add_cmd_str
                static_cast<const char*>(etags.c_str()),
                static_cast<int>(etags.size()),
                SQLITE_TRANSIENT);
        }
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int(statement,
                7,// Must match $7 at add_cmd_str
                static_cast<int>(_valid));
        }
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            do
            {
                extended_sql_error_ = sqlite3_step(statement);

                if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
                {
                    lastinsertedrowid_.store(sqlite3_last_insert_rowid(db_handle_), std::memory_order_release);
                    break;
                }
                --retry_counter;
            }while(0 < retry_counter &&
                (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
        }

        if(statement)
        {
            sqlite3_finalize(statement);
        }
    }

    return ErrorSet(extended_sql_error_);
}

DbErrorCode HttpCacheDb::DelDbRecord(const std::string &url)
{
    sqlite3_stmt *statement = nullptr;
    int8_t retry_counter =10;
    std::string del_str = kSqlDelete + kCacheTableName + kSqlWhere + kSqlUrl + "=?1";
    extended_sql_error_= sqlite3_prepare_v2(db_handle_,
        del_str.c_str(),
        static_cast<int>(del_str.size()),
        &statement,
        nullptr);
    if(ErrorSet(extended_sql_error_)== DbErrorCode::kNone)
    {
        extended_sql_error_ = sqlite3_bind_text(statement,
            1,
            static_cast<const char*>(url.c_str()),
            static_cast<int>(url.size()),
            SQLITE_TRANSIENT);
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            do
            {
                extended_sql_error_ = sqlite3_step(statement);

                if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
                {
                    lastinsertedrowid_.store(sqlite3_last_insert_rowid(db_handle_), std::memory_order_release);
                    break;
                }
                --retry_counter;
            }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
        }

        if(statement)
        {
            sqlite3_finalize(statement);
        }
    }

    return ErrorSet(extended_sql_error_);
}

DbErrorCode HttpCacheDb::UpdateDbRecord(const std::string &url, const CacheDbRecord &data_to_update)
{
    int8_t retry_counter =10;
    sqlite3_stmt *statement = nullptr;
    std::string _url;
    std::time_t received_time_t_ = 0;
    std::time_t exp_time_t_ = 0;
    std::vector<char> _blob_t_;
    std::size_t size_t_offset = 0;
    std::string etags;
    bool _valid_ = false;
    std::string update_str = kSqlUpdate + kCacheTableName + kSqlSet + kSqlRcvdTimestamp
    +"=?2, " + kSqlExpTimestamp +"=?3, " + kSqlDatablob + "=?4, " +kSqlBlobDataOffset +"=?5, "
    + kSqlEtag +"=?6, " + kSqlValid +"=?7 " + kSqlWhere +kSqlUrl + "=?1";

    //extract data from tuple
    std::tie(_url,received_time_t_,exp_time_t_,_blob_t_,size_t_offset,etags,_valid_) = data_to_update;

    extended_sql_error_= sqlite3_prepare_v2(db_handle_,
        update_str.c_str(),
        static_cast<int>(update_str.size()),
        &statement,
        nullptr);
    if(ErrorSet(extended_sql_error_)== DbErrorCode::kNone)
    {
        extended_sql_error_ = sqlite3_bind_text(statement,
            1,// Must match $1 at add_cmd_str
            static_cast<const char*>(url.c_str()),
            static_cast<int>(url.size()),
            SQLITE_TRANSIENT);
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int64(statement,
                2,
                received_time_t_);
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int64(statement,
                3,
                exp_time_t_);
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_blob(statement,
                4,
                static_cast<const char*>(_blob_t_.data()),
                static_cast<int>(_blob_t_.size()),
                SQLITE_TRANSIENT);
        }
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int(statement,
                5,
                static_cast<std::size_t>(size_t_offset));
        }
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_text(statement,
                6,
                static_cast<const char*>(etags.c_str()),
                static_cast<int>(etags.size()),
                SQLITE_TRANSIENT);
        }
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int(statement,
                7,
                static_cast<int>(_valid_));
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            do
            {
                extended_sql_error_ = sqlite3_step(statement);

                if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
                {
                    lastinsertedrowid_.store(sqlite3_last_insert_rowid(db_handle_), std::memory_order_release);
                    break;
                }
                --retry_counter;
            }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
        }

        if(statement)
        {
            sqlite3_finalize(statement);
        }
    }
    return ErrorSet(extended_sql_error_);
}

DbErrorCode HttpCacheDb::UpdateValidityRecord(const std::string &url, const CacheValidityDbRecord &data_to_update)
{
    int8_t retry_counter =10;
    sqlite3_stmt *statement = nullptr;
    std::time_t received_time_t_ = 0;
    std::time_t exp_time_t_ = 0;
    std::string etags;
    bool _valid_ = false;
    std::string update_str = kSqlUpdate + kCacheTableName + kSqlSet + kSqlRcvdTimestamp
    +"=?2, " + kSqlExpTimestamp +"=?3, " + kSqlEtag +"=?4, " + kSqlValid +"=?5 " + kSqlWhere +kSqlUrl + "=?1";

    //extract data from tuple
    std::tie(received_time_t_,exp_time_t_,etags,_valid_) = data_to_update;

    extended_sql_error_= sqlite3_prepare_v2(db_handle_,
        update_str.c_str(),
        static_cast<int>(update_str.size()),
        &statement,
        nullptr);
    if(ErrorSet(extended_sql_error_)== DbErrorCode::kNone)
    {
        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_text(statement,
                1,
                static_cast<const char*>(url.c_str()),
                static_cast<int>(url.size()),
                SQLITE_TRANSIENT);
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int64(statement,
                2,
                received_time_t_);
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int64(statement,
                3,
                exp_time_t_);
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_text(statement,
                4,
                static_cast<const char*>(etags.c_str()),
                static_cast<int>(etags.size()),
                SQLITE_TRANSIENT);
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            extended_sql_error_ = sqlite3_bind_int(statement,
                5,
                static_cast<int>(_valid_));
        }

        if(ErrorSet(extended_sql_error_) == DbErrorCode::kNone)
        {
            do
            {
                extended_sql_error_ = sqlite3_step(statement);

                if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
                {
                    lastinsertedrowid_.store(sqlite3_last_insert_rowid(db_handle_), std::memory_order_release);
                    break;
                }
                --retry_counter;
            }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
        }
        if(statement)
        {
            sqlite3_finalize(statement);
        }
    }
    return ErrorSet(extended_sql_error_);
}

DbErrorCode HttpCacheDb::UpdatePairDbRecord(const std::string &url, const CachePairDbRecord &data_to_update)
{
    DbErrorCode rc = DbErrorCode::kNone;
    int8_t retry_counter =10;
    sqlite3_stmt *statement = nullptr;
    std::string column_name;
    std::time_t data_time_t = 0;
    std::vector<char> blob_t;
    std::string data_str;
    //extract data from tuple
    std::tie(column_name, blob_t) = data_to_update;

    std::string update_str = kSqlUpdate + kCacheTableName + kSqlSet + column_name +"=?2 "
    + kSqlWhere +kSqlUrl + "=?1";

    extended_sql_error_= sqlite3_prepare_v2(db_handle_,
        update_str.c_str(),
        static_cast<int>(update_str.size()),
        &statement,
        nullptr);
    rc = ErrorSet(extended_sql_error_);

    if(DbErrorCode::kNone == rc)
    {
        extended_sql_error_ = sqlite3_bind_text(statement,
            1,
            static_cast<const char*>(url.c_str()),
            static_cast<int>(url.size()),
            SQLITE_TRANSIENT);
        rc = ErrorSet(extended_sql_error_);
    }

    if(DbErrorCode::kNone == rc)
    {
        switch (GetColumnType(column_name))
        {
            case SQLITE3_TEXT:
            {
                data_str.assign(blob_t.data());
                extended_sql_error_ = sqlite3_bind_text(statement,
                    2,
                    static_cast<const char*>(data_str.c_str()),
                    static_cast<int>(data_str.size()),
                    SQLITE_TRANSIENT);
                break;
            }
            case SQLITE_INTEGER:
            {
                data_str.assign(blob_t.data());
                data_time_t = std::stol(data_str.c_str());
                extended_sql_error_ = sqlite3_bind_int(statement,
                    2,
                    data_time_t);
                break;
            }
            case SQLITE_BLOB:
            {
                extended_sql_error_ = sqlite3_bind_blob(statement,
                    2,
                    static_cast<const char*>(blob_t.data()),
                    static_cast<int>(blob_t.size()),
                    SQLITE_TRANSIENT);
                break;
            }
            default:
            {
                rc = DbErrorCode::kUndefined;
            }
        }
        if(DbErrorCode::kUndefined == rc)
        {
            ErrorSet(SQLITE_ERROR);
        }

        rc = ErrorSet(extended_sql_error_);
    }

    if(DbErrorCode::kNone == rc)
    {
        do
        {
            extended_sql_error_ = sqlite3_step(statement);

            if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
            {
                break;
            }
            --retry_counter;
        }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
        if(statement)
        {
            sqlite3_finalize(statement);
        }
        rc = ErrorSet(extended_sql_error_);
    }
    return rc;
}

DbErrorCode HttpCacheDb::GetDbRecord(const std::string &url, std::shared_ptr<CacheDbRecord> &data_to_get)
{
    DbErrorCode rc = DbErrorCode::kNone;
    sqlite3_stmt *statement = nullptr;
    CacheDbRecord data_to_select;
    int8_t retry_counter =10;
    std::string _url;
    std::vector<char> datablob;
    std::string etags;
    const unsigned char *rec = nullptr;
    const void *rec_blob = nullptr;
    const char *ptr= nullptr;
    size_t recsize = 0;
    std::string select_str = kSqlSelect +"* FROM " + kCacheTableName + kSqlWhere +kSqlUrl + "=?1";

    extended_sql_error_= sqlite3_prepare_v2(db_handle_,
        select_str.c_str(),
        static_cast<int>(select_str.size()),
        &statement,
        nullptr);
    if(DbErrorCode::kNone == rc)
    {
        extended_sql_error_ = sqlite3_bind_text(statement,
            1,
            static_cast<const char*>(url.c_str()),
            static_cast<int>(url.size()),
            SQLITE_TRANSIENT);
        rc = ErrorSet(extended_sql_error_);
    }

    if(DbErrorCode::kNone == rc)
    {
        do
        {
            extended_sql_error_ = sqlite3_step(statement);

            if(DbErrorCode::kRowReturn == ErrorSet(extended_sql_error_) &&
                kMaxDbColumnsIndex == sqlite3_column_count(statement))
            {
                //url
                rec = sqlite3_column_text(statement,kUrlIdx);
                recsize = static_cast<size_t>(sqlite3_column_bytes(statement,kUrlIdx));
                std::get<kUrlIdx>(data_to_select) = _url.assign(reinterpret_cast<const char*>(rec),recsize);
                //receive timestamp
                std::get<kRcvTimestampIdx>(data_to_select) = sqlite3_column_int64(statement,kRcvTimestampIdx);
                //expiration timestamp
                std::get<kExpTimestampIdx>(data_to_select) = sqlite3_column_int64(statement,kExpTimestampIdx);
                //Blob extract
                rec_blob = sqlite3_column_blob(statement,kBlobIdx);
                recsize = static_cast<size_t>(sqlite3_column_bytes(statement,kBlobIdx));
                ptr = static_cast<const char*>(rec_blob);
                std::get<kBlobIdx>(data_to_select).assign(ptr,(ptr+recsize));
                std::get<kBlobOffset>(data_to_select) =
                static_cast<std::size_t>(sqlite3_column_int(statement, kBlobOffset));
                //Etags extract
                rec = sqlite3_column_text(statement,kEtagIdx);
                recsize = static_cast<size_t>(sqlite3_column_bytes(statement,kEtagIdx));
                std::get<kEtagIdx>(data_to_select) = etags.assign(reinterpret_cast<const char*>(rec),recsize);
                //Validity flag
                std::get<kValidIdx>(data_to_select) = sqlite3_column_int(statement,kValidIdx);
                //Make extracted data
                data_to_get = std::make_shared<CacheDbRecord>(std::move(data_to_select));
                break;
            }
            if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
            {
                break;
            }
            --retry_counter;
        }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
    }

    if(statement)
    {
        sqlite3_finalize(statement);
    }

    return ErrorSet(extended_sql_error_);
}

DbErrorCode HttpCacheDb::GetValidityRecord(const std::string &url,
                                           std::shared_ptr <CacheValidityDbRecord> &data_to_get)
{
    DbErrorCode rc = DbErrorCode::kNone;
    sqlite3_stmt *statement = nullptr;
    CacheValidityDbRecord data_to_select;
    int8_t retry_counter =10;
    std::string etags;
    const unsigned char *rec = nullptr;
    size_t recsize = 0;
    bool data_found = false;
    std::string select_str = kSqlSelect + kSqlRcvdTimestamp + "," + kSqlExpTimestamp +","
    + kSqlEtag + "," + kSqlValid + " FROM " + kCacheTableName + kSqlWhere +kSqlUrl + "=?1";

    extended_sql_error_= sqlite3_prepare_v2(db_handle_,
        select_str.c_str(),
        static_cast<int>(select_str.size()),
        &statement,
        nullptr);
    rc = ErrorSet(extended_sql_error_);
    if(DbErrorCode::kNone == rc)
    {
        extended_sql_error_ = sqlite3_bind_text(statement,
            1,
            static_cast<const char*>(url.c_str()),
            static_cast<int>(url.size()),
            SQLITE_TRANSIENT);
    }

    if(ErrorSet(extended_sql_error_)==DbErrorCode::kNone)
    {
        do
        {
            extended_sql_error_ = sqlite3_step(statement);

            if(ErrorSet(extended_sql_error_) == DbErrorCode::kRowReturn &&
                sqlite3_column_count(statement) == kMaxValidColumsIndex)
            {
                //receive timestamp
                std::get<kRcvTimestampVidx>(data_to_select) = sqlite3_column_int64(statement, kRcvTimestampVidx);
                //expiration timestamp
                std::get<kExpTimestampVidx>(data_to_select) = sqlite3_column_int64(statement, kExpTimestampVidx);
                //Etags extract
                rec = sqlite3_column_text(statement,kEtagVidx);
                recsize = static_cast<size_t>(sqlite3_column_bytes(statement,kEtagVidx));
                std::get<kEtagVidx>(data_to_select) = etags.assign(reinterpret_cast<const char*>(rec),recsize);
                //Validity flag
                std::get<kValidVidx>(data_to_select) = sqlite3_column_int(statement,kValidVidx);
                data_found = true;
            }
            if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
            {
                break;
            }
            --retry_counter;
        }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
    }

    //Make extracted data
    if(data_found)
    {
        data_to_get = std::make_shared<CacheValidityDbRecord>(std::move(data_to_select));
    }

    if(statement)
    {
        sqlite3_finalize(statement);
    }
    return ErrorSet(extended_sql_error_);
}

DbErrorCode HttpCacheDb::GetPairDbDataRecord(const std::string &url, CachePairDbRecord &data_to_get)
{
    DbErrorCode rc = DbErrorCode::kNone;
    sqlite3_stmt *statement = nullptr;
    int8_t retry_counter =10;
    std::string column_name;
    std::time_t data_time_t;
    std::vector<char> datablob;
    std::string data_str;
    std::string select_str;
    bool data_valid;
    const unsigned char *rec = nullptr;
    const void *rec_blob = nullptr;
    const char *ptr= nullptr;
    size_t recsize = 0;
    //extract columm_name from tuple
    std::tie(column_name, std::ignore) = data_to_get;

    if(0 != column_name.compare("*"))
    {
        select_str = kSqlSelect + column_name +" FROM " + kCacheTableName +
        kSqlWhere + kSqlUrl + "=?1";

        extended_sql_error_= sqlite3_prepare_v2(db_handle_,
            select_str.c_str(),
            static_cast<int>(select_str.size()),
            &statement,
            nullptr);
        rc = ErrorSet(extended_sql_error_);
    }
    else
    {
        rc = DbErrorCode::kInternalError;
    }

    if(DbErrorCode::kNone == rc)
    {
        extended_sql_error_ = sqlite3_bind_text(statement,
            1,
            static_cast<const char*>(url.c_str()),
            static_cast<int>(url.size()),
            SQLITE_TRANSIENT);
    }

    if(DbErrorCode::kNone == ErrorSet(extended_sql_error_))
    {
        do
        {
            extended_sql_error_ = sqlite3_step(statement);

            if(ErrorSet(extended_sql_error_) == DbErrorCode::kRowReturn)
            {
                if(DbErrorCode::kNone == rc)
                {
                    switch (GetColumnType(column_name))
                    {
                        case SQLITE3_TEXT:
                        {
                            //text field extract
                            rec = sqlite3_column_text(statement,kColumnZeroIdx);
                            recsize = static_cast<size_t>(sqlite3_column_bytes(statement,kColumnZeroIdx));
                            std::get<kDataIdx>(data_to_get).assign(rec,(rec+recsize));
                            break;
                        }
                        case SQLITE_INTEGER:
                        {
                            if(column_name == kSqlValid)   //Validity flag int extract
                            {
                                data_valid = sqlite3_column_int(statement,kColumnZeroIdx);
                                data_str = std::to_string(data_valid);
                            }
                            else                          //Int64 extract
                            {
                                data_time_t = sqlite3_column_int64(statement,kColumnZeroIdx);
                                data_str = std::to_string(data_time_t);
                            }
                            std::get<kDataIdx>(data_to_get).assign(data_str.data(),(data_str.data()+data_str.size()));
                            break;
                        }
                        case SQLITE_BLOB:
                        {
                            rec_blob = sqlite3_column_blob(statement,kColumnZeroIdx);
                            recsize = static_cast<size_t>(sqlite3_column_bytes(statement,kColumnZeroIdx));
                            ptr = static_cast<const char*>(rec_blob);
                            std::get<kDataIdx>(data_to_get).assign(ptr,(ptr+recsize));
                            break;
                        }
                        default:
                        {
                            rc = ErrorSet(SQLITE_ERROR);
                        }
                    }
                }
            }
            if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_) ||
               DbErrorCode::kUndefined == rc)
            {
                break;
            }
            --retry_counter;
        }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
    }
    if(statement)
    {
        sqlite3_finalize(statement);
    }
    return ErrorSet(extended_sql_error_);
}

DbErrorCode HttpCacheDb::DoesDbRecordExist(const std::string &url, bool &record_exist_flag)
{
    DbErrorCode rc = DbErrorCode::kNone;
    sqlite3_stmt *statement = nullptr;
    int8_t retry_counter =10;
    //sql select get valid flag for provided url
    std::string select_str = kSqlSelect + kSqlValid + " FROM " + kCacheTableName + kSqlWhere
                             + kSqlUrl + "=?1";

    extended_sql_error_= sqlite3_prepare_v2(db_handle_,
        select_str.c_str(),
        static_cast<int>(select_str.size()),
        &statement,
        nullptr);
    rc = ErrorSet(extended_sql_error_);

    if(DbErrorCode::kNone == rc)
    {
        extended_sql_error_ = sqlite3_bind_text(statement,
            1,
            static_cast<const char*>(url.c_str()),
            static_cast<int>(url.size()),
            SQLITE_TRANSIENT);
    }

    if(ErrorSet(extended_sql_error_)==DbErrorCode::kNone)
    {
        do
        {
            extended_sql_error_ = sqlite3_step(statement);

            if(ErrorSet(extended_sql_error_) == DbErrorCode::kRowReturn)
            {
                //Existence flag
                record_exist_flag = true;
                break;
            }
            if(DbErrorCode::kDoneReturn == ErrorSet(extended_sql_error_))
            {
                record_exist_flag = false;
                break;
            }
            --retry_counter;
        }while(0 < retry_counter && (DbErrorCode::kBusyError == ErrorSet(extended_sql_error_)));
    }

    if(statement)
    {
        sqlite3_finalize(statement);
    }
    return ErrorSet(extended_sql_error_);
}

} // namespace transfermanager
} // namespace fsm


/** \}    end of addtogroup */
