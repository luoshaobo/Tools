/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     http_cache_db.h
 *  \brief    HTTP cache db interaction engine
 *  \author   Maksym Mozok
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_HTTP_CACHE_DB_H_
#define FSM_TRANSFERMANAGER_HTTP_CACHE_DB_H_

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

namespace fsm
{

namespace transfermanager
{

//Constants and data definitions

//complete tuple fields indexes definitions constants
const std::size_t kUrlIdx= 0;
const std::size_t kRcvTimestampIdx = 1;
const std::size_t kExpTimestampIdx = 2;
const std::size_t kBlobIdx = 3;
const std::size_t kBlobOffset = 4;
const std::size_t kEtagIdx = 5;
const std::size_t kValidIdx = 6;
const std::size_t kMaxDbColumnsIndex = 7;

//validity record field indexes
const std::size_t kRcvTimestampVidx = 0;
const std::size_t kExpTimestampVidx = 1;
const std::size_t kEtagVidx = 2;
const std::size_t kValidVidx = 3;
const std::size_t kMaxValidColumsIndex = 4;

//row pair record indexes
const std::size_t kColumnNameIdx = 0;
const std::size_t kDataIdx = 1;
const std::size_t kMaxPairColumsIndex = 1; //We assume the return dataset has only one column and index must be 1

const std::size_t kColumnZeroIdx = 0;

typedef enum class DbErrorCode       //DBCacheError codes defines
{
    kNone,                   ///< Operation/transaction was executed with success.
    kUndefined = (1),        ///< Undefineded error.
    kInternalError = (2),    ///< Internal error.
    kPermisionError = (3),   ///< Permission error to open db.
    kAbortError = (4),       ///< Operation aborted before completion error.
    kBusyError = (5),        ///< DB can't be writen error.
    kNoMemError = (7),       ///< Mem alloc error to open db.
    kROError = (8),          ///< RO mode set, error to opened db.
    kIOError = (10),         ///< FS I/O error to open db.
    kDBCorruptError = (11),  ///< DB file corrupt error to open db.
    kFsFullError = (13),     ///< Storage full error to open db.
    kRowReturn = (100),      ///< Next row available from db. Is in some cases also a success code.
    kDoneReturn = (101),     ///< No more data available from db. Is in some cases also a success code.
    kMax                     ///< Maximum value type should hold. Keep this last.
} DbErrorCode_t;

typedef std::tuple<std::string,         ///< url
                   std::time_t,         ///< received_timestamp
                   std::time_t,         ///< expiry_timestamp
                   std::vector<char>,   ///< BLOB data
                   std::size_t,         ///< start of payload contents (to simplify access of payload)
                   std::string,         ///< etag ideally concatenated strings as we can use them for the get request
                   bool                 ///< valid
                   > CacheDbRecord;     ///< dataset which contains full tuple

typedef std::tuple<std::time_t,         ///< received_timestamp
                   std::time_t,         ///< expiry_timestamp
                   std::string,         ///< etag ideally concatinated strings as we can use them for the get request
                   bool                 ///< valid
                  > CacheValidityDbRecord;  ///< dataset for rw and validity get/update

typedef std::pair<std::string,          ///< column name
                  std::vector<char>     ///< data buffer
                  > CachePairDbRecord;  ///< dataset which contains one data field

    /**
     * \brief This class implements layer for interacting with sqlite3
     *        database engine. It locates, creates, updates transfermanager's
     *        http cache database and also provides higher level API for manipulating
     *        cache db data.
     *        DbName and Path can be customized by calling Init call,
     *        but previously opened connection is going to be closed
     *        and new one will be created.
     *        Default constructor uses FsmPersistenceMgr to retrieve
     *        application's Local or Public persistence path.
     *
     */
class HttpCacheDb
{
    std::string db_name_;
    std::string db_path_;
    sqlite3 *db_handle_ = nullptr;
    std::atomic_int_fast64_t lastinsertedrowid_;
    std::atomic_int_fast32_t extended_sql_error_;
    /**
     * \brief Private error mapper between sqlite errors to DbErrorCode.
     */
    DbErrorCode ErrorSet(int_fast32_t sqlerror);
    /**
     * \brief Private column type setter to sqlite type defines.
     */
    int GetColumnType(std::string &column_name);

    public:
    /**
     * \brief Default constructor.
     *        Which opens the http cache data base from its default location.
     *        Default location, E_PCL_LDBID_PUBLIC, has read from persistent storage.
     *        From FsmPersistDataMgr both E_PCL_LDBID_LOCAL and E_PCL_LDBID_PUBLIC
     *        are available. Default database name has listed as cache_db_name_key in
     *        fsm_persist_data_mgr/persistence_config_fsm_ssm.xml
     *        where all fsm peresistent storage data have defined.
     */
    HttpCacheDb();

    /**
     * \brief Parametrized constructor.
     *
     * \param[in] path_to_db string path to custom folder where db should be located.
     * \param[in] db_name string custom db filename.
     *
     */
     HttpCacheDb(const std::string path_to_db, const std::string db_name);

    /**
     * \brief Destructor.
     */
    ~HttpCacheDb();

    /**
     * \brief Parametrized initialiser.
     *        It closes previously opened database.
     *
     * \param[in] path_to_db string path to custom folder where db should be located.
     * \param[in] db_name string custom db filename.
     *
     * \return DbErrorCode::kNone successfully initialised.
     */
    DbErrorCode Init(const std::string path_to_db, const std::string db_name);

    /**
     * \brief Symmetrical to Init De-Init.
     *        It closes previously opened database.
     *        Any additional cleanups should be placed here.
     *        It's also called from destructor.
     */
    void Done(void);

    /**
     * \brief Get latest opened db filename.
     *
     * \return db_name_
     */
    inline std::string GetDbNameStr(void) { return db_name_;}

    /**
     * \brief Get latest opened db path
     *
     * \return db_path_
     */
    inline std::string GetDbPathStr(void) { return db_path_;}

    /**
     * \brief Add (INSERT OR REPLACE) new record to http cachedb. URL must be valid and properly formated.
     *        Otherwise dead record will be added.
     *        If the record (determined by the url) already exists it will be REPLACED by new one.
     *
     * \param[in] data_to_add CacheDbRecord tuple with data to add.
     *
     * \return DbErrorCode::kNone or DbErrorCode::kDoneReturn successfully added/replaced.
     */
    DbErrorCode AddDbRecord(CacheDbRecord &data_to_add);

    /**
     * \brief remove record from cachedb.
     *        ToDo: deleted records cleanup? Compress db? FixMe!!!
     *
     * \param[in] url string record to delete.
     *
     * \return DbErrorCode::kDoneReturn successfully deleted.
     */
    DbErrorCode DelDbRecord(const std::string &url);

    /**
     * \brief Update existing record in cachedb. Full tuple must be filled.
     *
     * \param[in] url string record to update.
     * \param[in] data_to_update CacheDbRecord tuple with data
     *            Valid field must be set to proper value (TRUE is valid).
     *
     * \return DbErrorCode::kDoneReturn successfully updateded.
     */
    DbErrorCode UpdateDbRecord(const std::string &url, const CacheDbRecord &data_to_update);

    /**
     * \brief Update existing validity fields in cachedb. Full tuple must be filled.
     *
     * \param[in] url string record to update.
     * \param[in] data_to_update CacheValidityDbRecord tuple with data.
     *
     * \return DbErrorCode::kDoneReturn successfully updated.
     */
    DbErrorCode UpdateValidityRecord(const std::string &url, const CacheValidityDbRecord &data_to_update);

    /**
     * \brief Update existing field in cachedb. Full tuple must be filled.
     *
     * \param[in] url string record to update.
     * \param[in] data_to_update CachePairDbRecord tuple with data.
     *
     * \return DbErrorCode::kDoneReturn successfully updated.
     */
    DbErrorCode UpdatePairDbRecord(const std::string &url, const CachePairDbRecord &data_to_update);

    /**
     * \brief Get existing record in cachedb. Full tuple will be filled.
     *
     * \param[in] url string record to get.
     * \param[in,out] data_to_get provide empty CacheDbRecord tuple, which will be filled with the data.
     *
     * \return DbErrorCode::kRowReturn successfully returnd data_to_get.
     */
    DbErrorCode GetDbRecord(const std::string &url, std::shared_ptr <CacheDbRecord> &data_to_get);

    /**
     * \brief Get existing validity record from cachedb. Full tuple will be filled.
     *
     * \param[in] url string record to get.
     * \param[in,out] data_to_get empty CacheValidityDbRecord, which fill be filled with the data.
     *
     * \return DbErrorCode::kRowReturn successfully returnd.
     */
    DbErrorCode GetValidityRecord(const std::string &url, std::shared_ptr <CacheValidityDbRecord> &data_to_get);

    /**
     * \brief Get existing pair record field in cachedb. Pair will be filled.
     *
     * \param[in] url string record to get.
     * \param[in,out] data_to_get CachePairDbRecord tuple for data.
     *
     * \return DbErrorCode::kRowReturn successfully returned.
     */
    DbErrorCode GetPairDbDataRecord(const std::string &url, CachePairDbRecord &data_to_get);

    /**
     * \brief Record existence validator.
     *
     * \param[in] url string record to validate.
     * \param[out] record_exist true if record exists, otherwise false.
     *
     * \return DbErrorCode::kRowReturn successfully executed.
     */
     DbErrorCode DoesDbRecordExist(const std::string &url, bool &record_exist);

     /**
     * \brief Get extended sqlite error.
     *
     * \return raw sqlite error code.
     */
     inline int_fast32_t ExtendedSqlErrorGet(void)
     {
        return reinterpret_cast<int_fast32_t> (extended_sql_error_.load(std::memory_order_acquire));
     }

     /**
      * \brief Get rowid value by url.
      *
      * \param[in] url for looking at.
      * \param[out] rowid value to get.
      *
      * \return DbErrorCode::kNone successfully executed.
      *         DbErrorCode::kInternalError if error occured.
      */
     DbErrorCode GetRowIdByUrl(const std::string &_url, int64_t &rowid);

     /**
      * \brief Get complete DbRecord by rowid value.
      *
      * \param[in] rowid record to get.
      * \param[out] data_to_get CacheDbRecord valid record to get by rowid.
      *
      * \return DbErrorCode::kNone successfully executed.
      *         DbErrorCode::kInternalError if error occured.
      */
     DbErrorCode GetDbRecordByRowId(const int64_t rowid, std::shared_ptr<CacheDbRecord> &data_to_get);

     /**
     * \brief Get last inserted rowid.
     */
     inline void GetDbRecordLastRowId(int64_t &rowid)
     {
         rowid = lastinsertedrowid_.load(std::memory_order_acquire);
     }

};

} // namespace transfermanager
} // namespace fsm

#endif // FSM_TRANSFERMANAGER_HTTP_CACHE_DB_H_

/** \}    end of addtogroup */
