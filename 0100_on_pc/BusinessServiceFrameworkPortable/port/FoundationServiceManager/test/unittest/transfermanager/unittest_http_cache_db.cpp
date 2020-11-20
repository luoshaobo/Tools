/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     unittest_http_cache_db.cpp
 *  \brief    Foundation Services Manager http cache database unit tests with real underlying sqlite3 database
 *  \author   Florian Schindler
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include "http_cache_db.h"
#include "sql_defines.h"
#include "unittest_http_cache_db.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>


#define SQLLITE_TEST_DB_NAME "unittest_http_cache.sqlite3"
#define SQLLITE_TEST_DB_DIR "./"

void RemoveDatabaseFiles()
{
    // make sure we delete any existing test database
    remove(SQLLITE_TEST_DB_DIR  SQLLITE_TEST_DB_NAME);
    remove(SQLLITE_PERSISTENCY_LOCAL_TEST_DB_DIR  SQLLITE_PERSISTENCY_TEST_DB_NAME);
}


namespace fsm
{

namespace transfermanager
{

#define RANDOM_NEGATIVE_NUMBER -294931

TEST(TransferManager, http_cache_db_HttpCacheDb)
{
    // testing the default constructor (this database is not used later)
    HttpCacheDb db;

    // the following data must match the values in the stub of fsm_persist_data_mgr.cc
    ASSERT_TRUE(db.GetDbNameStr() == SQLLITE_PERSISTENCY_TEST_DB_NAME);
    ASSERT_TRUE(db.GetDbPathStr() == SQLLITE_PERSISTENCY_PUBLIC_TEST_DB_DIR);
    db.Done();
    RemoveDatabaseFiles();
}


TEST(TransferManager, http_cache_db_GetDbNameStr_GetDbPathStr)
{
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);

    ASSERT_TRUE(db.GetDbNameStr() == SQLLITE_TEST_DB_NAME);
    ASSERT_TRUE(db.GetDbPathStr() == SQLLITE_TEST_DB_DIR);
}


TEST(TransferManager, http_cache_db_AddDbRecord_GetDbRecord)
{
    // CAUTION: other test cases rely that this function has been executed, so keep it first
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // write data to empty database
    // create input data set:
    const std::string uri_in = "http://uri1";
    const std::time_t received_timestamp_in = 1234;
    const std::time_t expiry_timestamp_in = 9876;
    std::vector<char> data_in = {'b', 'l', 'o', 'b', 0x0D, 0x0A, 'u', 'r', 'i', '1'};
    std::size_t payload_offset_in = 5656;
    std::string etag_in = "this is etag 1";
    bool valid_in = true;
    // prepare and update database entry:
    CacheDbRecord db_record_in = std::make_tuple(uri_in, received_timestamp_in, expiry_timestamp_in, data_in, payload_offset_in, etag_in, valid_in);
    db_err_code = db.AddDbRecord(db_record_in);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);

    // read data from database and compare it with input data (should match)
    db_err_code = DbErrorCode::kMax;
    std::shared_ptr <CacheDbRecord> db_record_out;
    db_err_code = db.GetDbRecord(uri_in, db_record_out);
    ASSERT_EQ(db_err_code, DbErrorCode::kRowReturn);
    ASSERT_TRUE(db_record_in == *db_record_out);
}

TEST(TransferManager, http_cache_db_AddDbRecord_GetDbRecord_2)
{
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // write second set of data to existing database
    // create input data set:
    const std::string uri_in = "http://uri2";
    const std::time_t received_timestamp_in = 4321;
    const std::time_t expiry_timestamp_in = 6789;
    std::vector<char> data_in = {'b', 'l', 'o', 'b', 0x0D, 0x0A, 'u', 'r', 'i', '2'};
    std::size_t payload_offset_in = 6565;
    std::string etag_in = "this is etag 2";
    bool valid_in = false;
    CacheDbRecord db_record_in = std::make_tuple(uri_in, received_timestamp_in, expiry_timestamp_in, data_in, payload_offset_in, etag_in, valid_in);
    db_err_code = db.AddDbRecord(db_record_in);
    ASSERT_EQ(db_err_code, DbErrorCode::kDoneReturn);

    // read data from database and compare it with input data (should match)
    db_err_code = DbErrorCode::kMax;
    std::shared_ptr <CacheDbRecord> db_record_out;
    db_err_code = db.GetDbRecord(uri_in, db_record_out);
    ASSERT_EQ(db_err_code, DbErrorCode::kRowReturn);
    ASSERT_TRUE(db_record_in == *db_record_out);
}


TEST(TransferManager, http_cache_db_UpdateDbRecord_OK)
{
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // CAUTION: make sure test case http_cache_db_AddDbRecord_GetDbRecord was executed before
    // update existing record 1
    // create a modified input data set:
    const std::string uri_in = "http://uri1";
    const std::time_t received_timestamp_in = 1324;
    const std::time_t expiry_timestamp_in = 9687;
    std::vector<char> data_in = {'w', 'x', 'y', 'z'};
    std::size_t payload_offset_in = 4343;
    std::string etag_in = "this is etag 1b";
    bool valid_in = false;
    CacheDbRecord db_record_in = std::make_tuple(uri_in, received_timestamp_in, expiry_timestamp_in, data_in, payload_offset_in, etag_in, valid_in);
    db_err_code = db.UpdateDbRecord(uri_in, db_record_in);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);

    // read data from database and compare it with input data (should match)
    db_err_code = DbErrorCode::kMax;
    std::shared_ptr <CacheDbRecord> db_record_out;
    db_err_code = db.GetDbRecord(uri_in, db_record_out);
    ASSERT_EQ(db_err_code,  DbErrorCode::kRowReturn);
    ASSERT_TRUE(db_record_in == *db_record_out);
}


TEST(TransferManager, http_cache_db_UpdateDbRecord_failure)
{
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // try to update data to non existing record 3 (should fail)
    // create input data set:
    const std::string uri_in = "http://uri3";
    const std::time_t received_timestamp_in = 1234;
    const std::time_t expiry_timestamp_in = 9876;
    std::vector<char> data_in = {'w', 'x', 'y', 'z'};
    std::size_t payload_offset_in = 5656;
    std::string etag_in = "this is etag 1b";
    bool valid_in = false;
    CacheDbRecord db_record_in = std::make_tuple(uri_in, received_timestamp_in, expiry_timestamp_in, data_in, payload_offset_in, etag_in, valid_in);
    db_err_code = db.UpdateDbRecord(uri_in, db_record_in);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);

    // try to read data from database for non existing record 3 (should fail)
    db_err_code = DbErrorCode::kMax;
    std::shared_ptr <CacheDbRecord> db_record_out;
    db_err_code = db.GetDbRecord(uri_in, db_record_out);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);
    ASSERT_TRUE(db_record_out == nullptr);
}


TEST(TransferManager, http_cache_db_DoesDbRecordExist_OK)
{
    // make sure http_cache_db_AddDbRecord_GetDbRecord_2 was called before
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // check whether second record exists
    const std::string uri_in = "http://uri2";
    bool record_exists = false;  // we expect true as result here, so predefine the opposite value
    db_err_code = db.DoesDbRecordExist(uri_in, record_exists);
    ASSERT_EQ(db_err_code,  DbErrorCode::kRowReturn);
    ASSERT_TRUE(record_exists);
}


TEST(TransferManager, http_cache_db_DoesDbRecordExist_failure)
{
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // check whether a non existing record exists (should fail)
    const std::string uri_in = "http://uri3";
    bool record_exists = true;  // we expect false as result here, so predefine the opposite value
    db_err_code = db.DoesDbRecordExist(uri_in, record_exists);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);
    ASSERT_FALSE(record_exists);
}


TEST(TransferManager, http_cache_db_UpdateValidityRecord_GetValidityRecord_OK)
{
    // make sure http_cache_db_AddDbRecord_GetDbRecord_2 was called before
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // update validity flags of record 1
    // create input data set:
    const std::string uri_in = "http://uri1";
    const std::time_t received_timestamp_in = 9999;
    const std::time_t expiry_timestamp_in = 8888;
    std::string etag_in = "this is etag 1c";
    bool valid_in = false;
    CacheValidityDbRecord validity_record_in = std::make_tuple(received_timestamp_in, expiry_timestamp_in, etag_in, valid_in);
    db_err_code = db.UpdateValidityRecord(uri_in, validity_record_in);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);

    // read validity flags from database and compare it with input data (should match)
    db_err_code = DbErrorCode::kMax;
    std::shared_ptr <CacheValidityDbRecord> validity_record_out;
    db_err_code = db.GetValidityRecord(uri_in, validity_record_out);
    ASSERT_EQ(db_err_code,  DbErrorCode::kRowReturn);
    ASSERT_TRUE(validity_record_in == *validity_record_out);
}


TEST(TransferManager, http_cache_db_UpdateValidityRecord_GetValidityRecord_failure)
{
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // update validity flags of a non existing record 3
    // create input data set:
    const std::string uri_in = "http://uri3";
    const std::time_t received_timestamp_in = 9999;
    const std::time_t expiry_timestamp_in = 8888;
    std::string etag_in = "this is etag 1c";
    bool valid_in = false;
    CacheValidityDbRecord validity_record_in = std::make_tuple(received_timestamp_in, expiry_timestamp_in, etag_in, valid_in);
    db_err_code = db.UpdateValidityRecord(uri_in, validity_record_in);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);  // should fail

    // read validity flags from database (should fail)
    db_err_code = DbErrorCode::kMax;
    std::shared_ptr <CacheValidityDbRecord> validity_record_out;
    db_err_code = db.GetValidityRecord(uri_in, validity_record_out);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);
}


TEST(TransferManager, http_cache_db_UpdatePairDbRecord_GetPairDbDataRecord_OK)
{
    // make sure http_cache_db_AddDbRecord_GetDbRecord_2 was called before
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // update pair of record 1
    // create input data set:
    const std::string uri_in = "http://uri1";
    std::string etag_in = "this is etag 1d";
    const std::vector<char> etag_vector_in(etag_in.begin(), etag_in.end());
    CachePairDbRecord value_pair_in(kSqlEtag, etag_vector_in);
    db_err_code = db.UpdatePairDbRecord(uri_in, value_pair_in);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);

    // read pair from database and compare it with input data (should match)
    db_err_code = DbErrorCode::kMax;
    CachePairDbRecord value_pair_out;
    value_pair_out.first = kSqlEtag;
    db_err_code = db.GetPairDbDataRecord(uri_in, value_pair_out);

    ASSERT_EQ(db_err_code, DbErrorCode::kRowReturn);
    ASSERT_TRUE(value_pair_in == value_pair_out);
}


TEST(TransferManager, http_cache_db_UpdatePairDbRecord_GetPairDbDataRecord_2_OK)
{
    // make sure http_cache_db_AddDbRecord_GetDbRecord_2 was called before
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // update pair of record 1
    // create input data set:
    const std::string uri_in = "http://uri1";
    bool valid_flag = true;
    const std::vector<char> valid_flag_vector_in(1, static_cast<char>(valid_flag ? '1' : '0'));
    CachePairDbRecord value_pair_in(kSqlValid, valid_flag_vector_in);
    db_err_code = db.UpdatePairDbRecord(uri_in, value_pair_in);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);

    // read pair from database and compare it with input data (should match)
    db_err_code = DbErrorCode::kMax;
    CachePairDbRecord value_pair_out;
    value_pair_out.first = kSqlValid;
    db_err_code = db.GetPairDbDataRecord(uri_in, value_pair_out);

    ASSERT_EQ(db_err_code, DbErrorCode::kRowReturn);
    ASSERT_TRUE(value_pair_in == value_pair_out);
}


TEST(TransferManager, http_cache_db_UpdatePairDbRecord_GetPairDbDataRecord_failure)
{
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // try to update pair of non exisiting record 3
    // create input data set:
    const std::string uri_in = "http://uri3";
    std::string etag_in = "this is etag 1d";
    const std::vector<char> etag_vector_in(etag_in.begin(), etag_in.end());
    CachePairDbRecord value_pair_in(kSqlEtag, etag_vector_in);
    db_err_code = db.UpdatePairDbRecord(uri_in, value_pair_in);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);

    // try to read pair from non exisiting record 3 (should fail)
    db_err_code = DbErrorCode::kMax;
    CachePairDbRecord value_pair_out;
    value_pair_out.first = kSqlEtag;
    db_err_code = db.GetPairDbDataRecord(uri_in, value_pair_out);

    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);
    ASSERT_TRUE(value_pair_in != value_pair_out);  // should not match
}


TEST(TransferManager, http_cache_db_GetRowIdByUrl_GetDbRecordByRowId)
{
    // make sure http_cache_db_AddDbRecord_GetDbRecord_2 was called before
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // get rowid for record 1:
    const std::string uri_in = "http://uri1";
    int64_t rowid = RANDOM_NEGATIVE_NUMBER;
    db_err_code = db.GetRowIdByUrl(uri_in, rowid);
    ASSERT_EQ(db_err_code,  DbErrorCode::kRowReturn);
    ASSERT_TRUE(rowid != RANDOM_NEGATIVE_NUMBER);

    // get record with rowid from database and compare uri_out with input data (should match)
    db_err_code = DbErrorCode::kMax;
    std::shared_ptr <CacheDbRecord> db_record_out;
    db_err_code = db.GetDbRecordByRowId(rowid, db_record_out);
    ASSERT_EQ(db_err_code,  DbErrorCode::kRowReturn);
    ASSERT_TRUE(db_record_out != nullptr);
    std::string uri_out = std::get<kUrlIdx>(*db_record_out);
    ASSERT_TRUE(uri_in == uri_out);
}


// should be the last:
TEST(TransferManager, http_cache_db_DelDbRecord)
{
    // make sure http_cache_db_AddDbRecord_GetDbRecord_2 was called before
    HttpCacheDb db(SQLLITE_TEST_DB_DIR, SQLLITE_TEST_DB_NAME);
    DbErrorCode db_err_code = DbErrorCode::kMax;

    // check whether second record exists
    const std::string uri_in = "http://uri2";
    db_err_code = db.DelDbRecord(uri_in);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);

    // check whether this record is still readable (should not be)
    db_err_code = DbErrorCode::kMax;
    bool record_exists = true;  // we expect true as result here
    db_err_code = db.DoesDbRecordExist(uri_in, record_exists);
    ASSERT_EQ(db_err_code,  DbErrorCode::kDoneReturn);
    ASSERT_FALSE(record_exists);
}


} // namespace transfermanager
} // namespace fsm

int main(int argc, char** argv)
{
    RemoveDatabaseFiles();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/** \}    end of addtogroup */
