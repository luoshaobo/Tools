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
 *  \file     default_cache.cpp
 *  \brief    Transfer Manager Default Cache fallback implementation.
 *  \author   Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

// ** INCLUDES *****************************************************************
#include "default_cache.h"

#include <string>
#include <memory>
#include <tuple>
#include <sstream>  // for std::istringstream
#include <iomanip>  // for std::get_time
#include <locale>  // for std::locale
#include <ctime>   // for std::tm
#include <cinttypes>
#include "dlt/dlt.h"

#include "http_cache_db.h"
#include "sql_defines.h"
#include "transfer_response.h"
#include "cache_utils.h"

DLT_IMPORT_CONTEXT(dlt_libfsmtm);


namespace fsm
{

namespace transfermanager
{

DefaultCache::DefaultCache()
{
}


DefaultCache::~DefaultCache()
{
    CloseHttpCacheDb();
}


bool DefaultCache::HasValidCache(const std::string uri)
{
    bool have_valid_cache = false;
    if (uri.length() >= 0)
    {
        std::shared_ptr <CacheValidityDbRecord> validity_record;
        DbErrorCode db_err_code = GetHttpCacheDb().GetValidityRecord(uri, validity_record);
        if (((db_err_code == DbErrorCode::kNone) || (db_err_code == DbErrorCode::kRowReturn))
              && (validity_record.get() != nullptr))
        {
            std::time_t received_timestamp;
            std::time_t expiry_timestamp;
            std::string etag;
            bool valid;
            std::tie(received_timestamp, expiry_timestamp, etag, valid) = *validity_record;
            have_valid_cache = valid && ! IsCacheStale(expiry_timestamp, received_timestamp);
        }
        else
        if (db_err_code == DbErrorCode::kDoneReturn)
        {
            have_valid_cache = false;  // record not found
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s GetValidityRecord(%s) not found",\
                __func__, uri.c_str());
        }
        else
        {
            // database error
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s GetValidityRecord(%s) failed: %i/%li",\
                __func__, uri.c_str(), static_cast<int>(db_err_code),\
                static_cast<int32_t>(GetHttpCacheDb().ExtendedSqlErrorGet()) );
            have_valid_cache = false;
        }
    }
    else
    {  // empty URI is invalid
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s empty URI provided", __func__);
        have_valid_cache = false;
    }
    DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_INFO, "%s(%s) has %svalid cache",\
        __func__, uri.c_str(), have_valid_cache?"":"in");
    return have_valid_cache ;
}


bool DefaultCache::PrepareGetRequestFromCache(Request &request)
{
    bool success_overall;
    std::string uri;
    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_INFO, __func__);
    ErrorCode error_code = request.GetUri(uri);
    success_overall = (error_code == ErrorCode::kNone) && (uri.size() > 0);
    if (success_overall)
    {
        bool success_local;
        std::string etag;
        // try to get and add etag / receive timestamp, even if one call fails the other should be called.
        // but if one call fails this should be signalled to the caller.
        success_local = GetEtagFromCache(uri, etag);
        success_overall = success_overall && success_local;
        if (success_local)
        {
            success_local = request.AddHeaderField(Message::FieldKey::kIfNoneMatch, etag) == ErrorCode::kNone;  // add etag to the request
            success_overall = success_overall && success_local;
        }
        std::string recevied_timestamp_str;
        success_local = GetReceivedTimestampFromCache(uri, recevied_timestamp_str);
        success_overall = success_overall && success_local;
        if (success_local)
        {
            success_local = request.AddHeaderField(Message::FieldKey::kIfModifiedSince, recevied_timestamp_str) == ErrorCode::kNone;
            success_overall = success_overall && success_local;
        }
    }
    return success_overall;
}


void DefaultCache::HandleOkResponseInCache(const std::shared_ptr<TransferResponse> response)
{
    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_INFO, __func__);
    if ( HasDontCacheControlHeader(response) )
    {
        // tags, which forbid to cache something have precedence and also implies we have to delete the cache record
        if (response->GetUri().size() > 0)
        {
            GetHttpCacheDb().DelDbRecord(response->GetUri());
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_INFO, "%s(%s) forbids caching, record deleted",\
                __func__, response->GetUri().c_str());
        }
        else
        {
             DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s() empty uri provided, ignored", __func__);
        }
    }
    else
    {
        std::time_t expiry_timestamp;  // return value from GetExpiryTimestampFromHeader, absolute or relative
        bool expiry_timestamp_relative_value;
        // update http cache only if there are received and expiry timestamps
        std::time_t received_timestamp = GetReceivedTimestamp(response);
        if ( (received_timestamp != kInvalidTimestamp) &&
             GetExpiryTimestampFromHeader(response, expiry_timestamp, expiry_timestamp_relative_value) )
        {
            WriteResponseToCache(response, expiry_timestamp, expiry_timestamp_relative_value, received_timestamp);
        }
        else
        {
            // neither no-cache-control headers nor expiry headers: don't touch database
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_INFO, "%s(%s) does not contain any cache control header",\
                __func__, response->GetUri().c_str());
        }
    }
}


void DefaultCache::HandleNotModifiedInCache(const std::shared_ptr<TransferResponse> response)
{
    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_INFO, __func__);
    if ( HasDontCacheControlHeader(response) )
    {
        if (response->GetUri().size() > 0)
        {
            // tags, which forbid to cache something have precedence and also implies we have to delete the cache record
            GetHttpCacheDb().DelDbRecord(response->GetUri());
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_INFO, "%s(%s) forbids caching, record deleted",\
                __func__, response->GetUri());
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s() empty uri provided, ignored", __func__);
        }
    }
    else
    {
        std::time_t expiry_timestamp;  // return value from GetExpiryTimestampFromHeader, absolute or relative
        bool expiry_timestamp_relative_value;
        // update http cache only if there are received and expiry timestamps
        std::time_t received_timestamp = GetReceivedTimestamp(response);
        if ( (received_timestamp != kInvalidTimestamp) &&
             GetExpiryTimestampFromHeader(response, expiry_timestamp, expiry_timestamp_relative_value) )
        {
            UpdateValidityFlagsInCache(response, expiry_timestamp, expiry_timestamp_relative_value, received_timestamp);
        }
        else
        {
            // neither no-cache-control headers nor expiry headers: don't touch database
            // no valid cache control tags found; this is not allowed according to the RFC 7232, hence don't touch the database
            // RFC 7232: "The server generating a 304 response MUST generate any of the
            // following header fields that would have been sent in a 200 (OK)
            // response to the same request: Cache-Control, Content-Location, Date,
            // ETag, Expires, and Vary."
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s(%s) does not contain cache control header, ignored",\
                __func__, response->GetUri().c_str());
        }
    }
}


bool DefaultCache::FillResponseFromCache(std::shared_ptr<TransferResponse> response, Response::StatusCode status_code)
{
    bool success = false;

    const std::string uri = response->GetUri();
    if (uri.size() > 0)
    {
        std::shared_ptr <CacheDbRecord> db_record;
        DbErrorCode db_err_code = GetHttpCacheDb().GetDbRecord(uri, db_record);
        if (((db_err_code == DbErrorCode::kNone) || (db_err_code == DbErrorCode::kRowReturn)) && db_record.get() != nullptr)
        {
            // determine length of payload:
            std::size_t payload_offset = std::get<kBlobOffset>(*db_record);
            #define BLOB_VECTOR  (std::get<kBlobIdx>(*db_record))  // simplify access and avoid copying to a temp variable
            std::size_t content_length = BLOB_VECTOR.size() - payload_offset;
            response->SetContentLength(content_length);
            // set special status code for response from cache:
            response->SetStatusCode(status_code);
            // copy data to response stream:s
            std::stringstream response_stream(std::string(BLOB_VECTOR.begin(), BLOB_VECTOR.end()) );
            response->Update(response_stream);
            // note: response->Done() to notify about finished action is done somwehere else
            success = true;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_INFO, "%s(%s) OK (%i)",\
                __func__, uri.c_str(), static_cast<int>(db_err_code));
            #undef BLOB_VECTOR
        }
        else
        {  // not existing record or database error is an issue!
            success = false;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s GetDbRecord(%s) failed: %i/%li",\
                __func__, uri.c_str(), static_cast<int>(db_err_code), static_cast<int32_t>(GetHttpCacheDb().ExtendedSqlErrorGet()) );
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s() empty uri provided, ignored", __func__);
    }
    return success;
}


ErrorCode DefaultCache::SetValidFlag(const std::string full_uri, const bool valid)
{
    ErrorCode err_code;
    if (full_uri.size() > 0)
    {
        const std::vector<char> valid_vector(1, static_cast<char>(valid ? '1' : '0'));
        CachePairDbRecord value_pair(kSqlValid, valid_vector);
        DbErrorCode db_err_code = GetHttpCacheDb().UpdatePairDbRecord(full_uri, value_pair);
        if ((db_err_code == DbErrorCode::kNone) || (db_err_code == DbErrorCode::kRowReturn))
        {
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_INFO, "%s UpdatePairDbRecord(%s, %i) OK",\
                __func__, full_uri.c_str(), static_cast<int>(valid) );
            err_code = ErrorCode::kNone;
        }
        else
        {  // record not found is an issue
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s UpdatePairDbRecord(%s) failed: %i%li",\
                __func__, full_uri.c_str(), static_cast<int>(db_err_code), static_cast<int32_t>(GetHttpCacheDb().ExtendedSqlErrorGet()) );
            err_code = ErrorCode::kBadState;
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s() empty uri provided, ignored", __func__);
        err_code = ErrorCode::kBadState;
    }
    return err_code;
}


ErrorCode DefaultCache::GetValidFlag(const std::string full_uri, bool &valid)
{
    ErrorCode err_code = ErrorCode::kBadValue;
    if (full_uri.size() > 0)
    {
        CachePairDbRecord value_pair;
        value_pair.first = kSqlValid;

        DbErrorCode db_err_code = GetHttpCacheDb().GetPairDbDataRecord(full_uri, value_pair);

        if ( ((db_err_code == DbErrorCode::kNone) || (db_err_code == DbErrorCode::kRowReturn)) &&
             (value_pair.second.size() == 1) &&  // check for reasonable values
             ( (value_pair.second[0] == '0') ||
               (value_pair.second[0] == '1') ) )
        {
            if (value_pair.second[0] == '0')
            {
                valid = false;
            }
            else
            {
                valid = true;
            }
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_INFO, "%s GetPairDbDataRecord(%s): %i OK",\
                __func__, full_uri.c_str(), static_cast<int>(valid) );
            err_code = ErrorCode::kNone;
        }
        else
        if (db_err_code == DbErrorCode::kDoneReturn)
        {
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s GetPairDbDataRecord(%s) not found",\
                __func__, full_uri.c_str());
        }
        else
        {
            // don't touch return value here
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s GetPairDbDataRecord(%s) failed: %i%li",\
                __func__, full_uri.c_str(), static_cast<int>(db_err_code), static_cast<int32_t>(GetHttpCacheDb().ExtendedSqlErrorGet()) );
            err_code = ErrorCode::kBadState;
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s() empty uri provided, ignored", __func__);
    }
    return err_code;
}


bool DefaultCache::WriteResponseToCache(const std::shared_ptr<TransferResponse> response,
    const std::time_t expiry_timestamp, const bool expiry_timestamp_relative_value,
    const std::time_t received_timestamp)
{
    bool success = false;
    if (response->GetUri().size() > 0)
    {
        std::time_t expiry_timestamp_abs = kInvalidTimestamp;  // absolute expiry time stamp

        const std::string uri = response->GetUri();
        expiry_timestamp_abs = CalculateExpiryTimestamp(response, expiry_timestamp, expiry_timestamp_relative_value, received_timestamp);
        if (expiry_timestamp_abs != kInvalidTimestamp)
        {
            // get BLOB:
            const std::string data_str = response->GetFullData();
            std::vector<char> data(data_str.begin(), data_str.end());
            // get offset of data:
            std::size_t payload_offset = static_cast<std::size_t>(response->GetPayloadOffset());
            // get ETag (if present):
            std::string etag;
            GetETagFromHeader(response, etag);
            // prepare and update database entry:
            CacheDbRecord db_record = std::make_tuple(uri, received_timestamp, expiry_timestamp_abs, data, payload_offset, etag , true);
            DbErrorCode db_err_code = GetHttpCacheDb().AddDbRecord(db_record);
            if ((db_err_code == DbErrorCode::kNone) || (db_err_code == DbErrorCode::kRowReturn)
                 || (db_err_code == DbErrorCode::kDoneReturn))
            {
                success = true;
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s AddDbRecord(%s) OK",\
                    __func__, uri.c_str());
            }
            else
            {
                success = false;
                // database error
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s AddDbRecord(%s) failed: %i/%li",\
                    __func__, uri.c_str(), static_cast<int>(db_err_code), static_cast<int32_t>(GetHttpCacheDb().ExtendedSqlErrorGet()) );
            }
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s(%s) not able to calculate expiry timestamp.",\
                __func__, uri.c_str());
            success = false;
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s() empty uri provided, ignored", __func__);
    }
    return success;
}


bool DefaultCache::UpdateValidityFlagsInCache(const std::shared_ptr<TransferResponse> response,
    const std::time_t expiry_timestamp, const bool expiry_timestamp_relative_value, const std::time_t received_timestamp)
{
    bool success = false;
    std::time_t expiry_timestamp_abs = kInvalidTimestamp;  // absolute expiry time stamp

    const std::string uri = response->GetUri();
    if (uri.size() > 0)
    {
        expiry_timestamp_abs = CalculateExpiryTimestamp(response, expiry_timestamp, expiry_timestamp_relative_value, received_timestamp);
        if (expiry_timestamp_abs != kInvalidTimestamp)
        {
            // get ETag (if present):
            std::string etag;
            GetETagFromHeader(response, etag);
            // prepare and update database entry:
            CacheValidityDbRecord validity_record;
            validity_record = std::make_tuple(received_timestamp, expiry_timestamp_abs, etag, true);
            DbErrorCode db_err_code = GetHttpCacheDb().UpdateValidityRecord(uri, validity_record);
            if ( (db_err_code == DbErrorCode::kNone) || (db_err_code == DbErrorCode::kRowReturn)
                 || (db_err_code == DbErrorCode::kDoneReturn) )
            {
                success = true;
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s(%s) OK",\
                    __func__, uri.c_str());
            }
            else
            {
                // database error
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s UpdateValidityRecord(%s) failed: %i/%li",\
                    __func__, uri.c_str(), static_cast<int>(db_err_code), static_cast<int32_t>(GetHttpCacheDb().ExtendedSqlErrorGet()) );
            }
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s(%s) not able to calculate expiry timestamp.",\
                __func__, uri.c_str());
            success = false;
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s() empty uri provided, ignored", __func__);
        success = false;
    }
    return success;
}


bool DefaultCache::IsCacheStale(const std::time_t expiry_timestamp, const std::time_t received_timestamp)
{
    bool is_stale = false;
    std::time_t cur_time = kInvalidTimestamp;

    if (expiry_timestamp == kInvalidTimestamp)
    {  // it should normally not happen that there is a cache entry with an invalid timestamp
        is_stale = true;
    }
    if (! is_stale && (received_timestamp != kInvalidTimestamp) )
    {
        // check whether cache was stale even at time of reception:
        is_stale = received_timestamp >= expiry_timestamp;
    }
    if (! is_stale)
    {
        cur_time = std::time(nullptr);
        //ToDo: need to check whether there is valid UTC-time in the system; now have a simple plausibility check
        std::tm threshold_time = { 0, 0, 0, 1, 1, (2017-1900), 0, 0, 0};  // current time should be newer than 2017-01-01
        // according RFC: if no valid time is available then cache is stale
        is_stale = (cur_time == kInvalidTimestamp)  || (cur_time < mktime(&threshold_time));
        if (! is_stale)
        {
            is_stale = cur_time >= expiry_timestamp;
            // see https://tools.ietf.org/html/rfc7234
            // "The calculation to determine if a response is fresh is:
            // response_is_fresh = (freshness_lifetime > current_age)"
        }
    }
    DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s(exp=%" PRIi64 ", rcv=%" PRIi64 ") cur=%" PRIi64 ", is_stale=%s",
      __func__,  static_cast<int64_t>(expiry_timestamp), static_cast<int64_t>(received_timestamp), \
      static_cast<int64_t>(cur_time), is_stale?"Y":"N");
    return is_stale;
}


std::time_t DefaultCache::CalculateExpiryTimestamp(const std::shared_ptr<TransferResponse> response,
        const std::time_t expiry_timestamp, const bool expiry_timestamp_relative_value, const std::time_t received_timestamp)
{
    std::time_t expiry_timestamp_abs = kInvalidTimestamp;
    bool success;

    if (expiry_timestamp != kInvalidTimestamp)  // check input parameter
    {
        if (expiry_timestamp_relative_value)
        {
            if (received_timestamp != kInvalidTimestamp)
            {
                expiry_timestamp_abs = received_timestamp + expiry_timestamp;
            }
            else
            {
                // no absolute expiry time stamp is an issue, which forbids caching
                expiry_timestamp_abs = kInvalidTimestamp;
            }
        }
        else  // absolute value, take it as it is
        {
            expiry_timestamp_abs = expiry_timestamp;
        }
    }
    else
    {  // invalid parameter
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s(%s) invalid parameter", __func__, response->GetUri().c_str());
        expiry_timestamp_abs = kInvalidTimestamp;
    }
    return expiry_timestamp_abs;
}


std::time_t DefaultCache::GetReceivedTimestamp(const std::shared_ptr<TransferResponse> response)
{
    std::time_t received_timestamp = kInvalidTimestamp;
    bool have_timestamp_in_header = GetReceivedTimestampFromHeader(response, received_timestamp);
    if (!have_timestamp_in_header)  // only if no server provided timestamp is availble then use own clock (cf. RFC)
    {  // if not provided then use system time, which must be in UTC (if valid)
        std::time_t cur_time = std::time(nullptr);
        //ToDo: need to check whether there is valid time in the system; now have a simple plausibility check
        std::tm threshold_time = { 0, 0, 0, 1, 1, (2017-1900), 0, 0, 0};  // current time should be newer than 2017-01-01
        // according RFC: if no valid time is available then don't use cache
        if ( (cur_time != kInvalidTimestamp) && (cur_time > mktime(&threshold_time)) )
        {
            received_timestamp = cur_time;
        }
        else
        {  // means no valid receive_timestamp at all: bad!
            received_timestamp = kInvalidTimestamp;
        }
    }
    return received_timestamp;
}


bool DefaultCache::GetExpiryTimestampFromHeader(const std::shared_ptr<TransferResponse> response,
    std::time_t &expiry_timestamp, bool &relative_value)
{
    bool found = false;
    CacheControlValueResult found_cache_control_value = CacheControlValueResult::kNotFound;
    std::shared_ptr<std::string> cache_control_string = std::make_shared<std::string>();

    const std::vector<std::string> value_array = {"s-maxage=", "max-age="};  // this order!
    found_cache_control_value = GetCacheControlValue(response, value_array, cache_control_string);
    if ( (found_cache_control_value == CacheControlValueResult::kFound) && (cache_control_string.get() != nullptr) )
    {
        std::string::size_type pos_eq = cache_control_string->find_first_of("=");
        if ( (pos_eq != std::string::npos) && (pos_eq < cache_control_string->size()) )  // the first expression must be always true
        {   // extract and convert the nummeric part after the '=' character:
            unsigned long long num_value;
            if (Str2uint(cache_control_string->substr(pos_eq + 1), num_value))
            {
                expiry_timestamp = static_cast<std::time_t>(num_value);
                relative_value = true;
                found_cache_control_value = CacheControlValueResult::kFound;
            }
            else
            {  // Str2uint failed (e.g. invalid characters in string)
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s URI='%s' parsing '%s' failed",\
                    __func__, response->GetUri().c_str(), *cache_control_string);
                found_cache_control_value = CacheControlValueResult::kSyntaxError;
            }
        }
        else
        {  // only "s-maxage=", no nummeric value(s) present
            found_cache_control_value = CacheControlValueResult::kSyntaxError;
        }
    }
    if (found_cache_control_value == CacheControlValueResult::kNotFound)
    {
        // Check and evaluate "Expires:" field
        std::vector<std::string> expires_fields;
        ErrorCode error_code = response->GetHeaderFieldValues(Message::FieldKey::kExpires, expires_fields);
        if (error_code == ErrorCode::kNone)
        {
            if (expires_fields.size() == 1)
            {
                std::time_t ts;
                if (GetTimestampFromString(expires_fields[0], ts))
                {
                    expiry_timestamp = ts;
                    relative_value = false;
                    found_cache_control_value = CacheControlValueResult::kFound;
                }
                else
                {
                    DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s URI='%s' parsing '%s' failed",\
                        __func__, response->GetUri().c_str(), expires_fields[0].c_str());
                    found_cache_control_value = CacheControlValueResult::kSyntaxError;
                }
            }
            else
            {  // only *one* "Expires:" tag is allowed!
                found_cache_control_value = CacheControlValueResult::kSyntaxError;
            }
        }
        else
        {
            // "Expires:" field not found, that may happen
            found_cache_control_value = CacheControlValueResult::kNotFound;
        }
    }
    // final words:
    switch (found_cache_control_value)
    {
        case CacheControlValueResult::kFound:
            found = true;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s result = %" PRIi64 " %s",\
                __func__, static_cast<int64_t>(expiry_timestamp), relative_value?"rel":"abs");
        break;
        case CacheControlValueResult::kNotFound:
            found = false;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s no expiry time stamp found in '%s'",\
                __func__, response->GetUri().c_str());
        break;
        case CacheControlValueResult::kSyntaxError:
            found = false;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s reply for URI '%s' has syntax errors!",\
                __func__, response->GetUri().c_str());
        break;
    }
    return found;
}


bool DefaultCache::GetReceivedTimestampFromCache(const std::string uri, std::string &received_timestamp_str)
{
    bool success;
    if (uri.length() >= 0)
    {
        std::shared_ptr <CacheValidityDbRecord> validity_record;
        DbErrorCode db_err_code = GetHttpCacheDb().GetValidityRecord(uri, validity_record);
        if (((db_err_code == DbErrorCode::kNone) || (db_err_code == DbErrorCode::kRowReturn))
              && (validity_record.get() != nullptr))
        {
            success = true;
            std::time_t received_timestamp = std::get<kRcvTimestampVidx>(*validity_record);

            std::tm* gmt = std::gmtime(&received_timestamp);
            if (gmt != nullptr)
            {
                char ts[100];
                std::size_t ret = std::strftime(ts, sizeof(ts), "%a, %d %b %Y %H:%M:%S %Z", gmt);
                success = ret != 0;
                if (success)
                {
                    received_timestamp_str = ts;
                    DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s GetValidityRecord(%s): '%s' OK (%li)",\
                        __func__, uri.c_str(), received_timestamp_str.c_str(), static_cast<int32_t>(db_err_code));
                }
                else
                {
                    DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s(%s) strftime(%" PRIi64 "): failed",\
                        __func__, uri.c_str(), static_cast<int64_t>(received_timestamp));
                }
            }
            else
            {
                success = false;
            }
        }
        else
        if (db_err_code == DbErrorCode::kDoneReturn)
        {
            success = false;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s GetValidityRecord(%s) not found",\
                __func__, uri.c_str());
        }
        else
        {
            // not found in DB; don't touch return value
            success = false;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s GetValidityRecord(%s): failed: %i/%li",\
                __func__, uri.c_str(), static_cast<int>(db_err_code), static_cast<int32_t>(GetHttpCacheDb().ExtendedSqlErrorGet()));
        }
    }
    else
    {
        success = false;
    }
    return success;
}


bool DefaultCache::GetReceivedTimestampFromHeader(const std::shared_ptr<TransferResponse> response, std::time_t &received_timestamp)
{
    bool found = false;
    std::vector<std::string> received_timestamp_strs;

    ErrorCode error_code = response->GetHeaderFieldValues(Message::FieldKey::kDate, received_timestamp_strs);
    if (error_code == ErrorCode::kNone)
    {
        if (received_timestamp_strs.size() == 1)
        {
            if (GetTimestampFromString(received_timestamp_strs[0], received_timestamp))
            {
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s URI='%s' result=%" PRIi64,\
                    __func__, response->GetUri().c_str(), static_cast<int64_t>(received_timestamp));
                found = true;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s URI='%s' parsing '%s' failed",\
                  __func__, response->GetUri().c_str(), received_timestamp_strs[0]);
                found = false;
            }
        }
        else
        {  // more than one field with timestamp is a syntax error
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s more than one 'Date=' field found in header for URI='%s'.",\
                __func__, response->GetUri().c_str());
            found = false;
        }
    }
    else
    {  // not a problem
      DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s no 'Date:' field found in header for URI='%s', err_code=%i.",\
            __func__, response->GetUri().c_str(), static_cast<int>(error_code));
        found = false;
    }
    return found;
}


bool DefaultCache::GetEtagFromCache(const std::string uri, std::string &etag)
{
    bool success;
    if (uri.length() >= 0)
    {
        CachePairDbRecord value_pair;
        value_pair.first = kSqlEtag;
        DbErrorCode db_err_code = GetHttpCacheDb().GetPairDbDataRecord(uri, value_pair);
        if ((db_err_code == DbErrorCode::kNone) || (db_err_code == DbErrorCode::kRowReturn))
        {
            success = true;
            etag.assign(value_pair.second.data(), value_pair.second.size());
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s GetPairDbDataRecord(%s): '%s' OK (%li)",\
                __func__, uri.c_str(), etag.c_str(), static_cast<int>(db_err_code));
        }
        else
        if (db_err_code == DbErrorCode::kDoneReturn)
        {
            success = false;  // record not found
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s GetPairDbDataRecord(%s) not found",\
                __func__, uri.c_str());
        }
        else
        {
            success = false;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s GetPairDbDataRecord(%s): failed: %i/%li",\
                __func__, uri.c_str(), static_cast<int>(db_err_code), static_cast<int32_t>(GetHttpCacheDb().ExtendedSqlErrorGet()));
        }
    }
    else
    {
        success = false;
    }
    return success;
}


bool DefaultCache::GetETagFromHeader(const std::shared_ptr<TransferResponse> response, std::string &etag)
{
    bool found = false;
    std::vector<std::string> etags;

    ErrorCode error_code = response->GetHeaderFieldValues(Message::FieldKey::kETag, etags);
    if (error_code == ErrorCode::kNone)
    {
        if (etags.size() != 0)
        {
            etag = "";
            for (std::size_t i = 0; i < etags.size(); i++)
            {  // concatinate found etags in one string
                etag.append(etags[i] + " ");
            }
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s ETag for URI='%s' = '%s'.",\
                __func__, response->GetUri().c_str(), etag.c_str());
            found = true;
        }
        else
        {  // no ETag field found
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s no ETag field found in header for URI='%s'.",\
                __func__, response->GetUri().c_str());
            found = false;
        }
    }
    else
    {  // not a problem
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s no ETag field found in header for URI='%s', err_code= %i.",\
            __func__, response->GetUri().c_str(), static_cast<int>(error_code));
        found = false;
    }
    return found;
}


bool DefaultCache::HasDontCacheControlHeader(const std::shared_ptr<TransferResponse> response)
{
    const std::vector<std::string> tag_array = {"no-cache", "no-store"};
    CacheControlValueResult found_cache_control_value = GetCacheControlValue(response, tag_array);
    if (found_cache_control_value == CacheControlValueResult::kNotFound)
    {
        // Search for "Pragma: no-cache"
        std::vector<std::string> pragma;
        ErrorCode error_code = response->GetHeaderFieldValues(Message::FieldKey::kPragma, pragma);
        if ( (error_code == ErrorCode::kNone) && (pragma.size() > 0) )
        {
            // search for "no-cache" value and if there is more than one occurance:
            for (std::string::size_type i = 0, num_tags_found = 0; i < pragma.size(); i++)
            {
                if (pragma[i].find("no-cache") != std::string::npos)
                {  // found tag!
                    found_cache_control_value = CacheControlValueResult::kFound;
                    num_tags_found++;
                    if (num_tags_found > 1)
                    {
                        found_cache_control_value = CacheControlValueResult::kSyntaxError;
                        break;
                    }
                    // note: no break, because we should double check for more than one occurance
                }
            }  // for cache_controls
        }
        else
        {
            //nothing found, no issue
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s no 'Pragma:' field found in header for URI='%s'",\
                __func__, response->GetUri().c_str());
        }
    }
    bool found = false;
    switch (found_cache_control_value)
    {
        case CacheControlValueResult::kFound:
            found = true;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s result don't-cache-control header found in '%s' ",\
                __func__, response->GetUri().c_str());
        break;
        case CacheControlValueResult::kNotFound:
            found = false;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s no don't-cache-control header found in '%s'",\
                __func__, response->GetUri().c_str());
        break;
        case CacheControlValueResult::kSyntaxError:
            found = false;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_WARN, "%s reply for URI '%s' has syntax errors!",\
                __func__, response->GetUri().c_str());
        break;
    }
    return found;
}


DefaultCache::CacheControlValueResult DefaultCache::GetCacheControlValue(
    const std::shared_ptr<TransferResponse> response,
    const std::vector<std::string> value_array,
    std::shared_ptr<std::string> cache_control_string)
{
    bool syntax_error = false;  // means one tag occures more than once; thus according to RFC this tag is invalid
    CacheControlValueResult result = CacheControlValueResult::kNotFound;
    std::vector<std::string> cache_controls;

    ErrorCode error_code = response->GetHeaderFieldValues(Message::FieldKey::kCacheControl, cache_controls);
    if ( (error_code == ErrorCode::kNone) && (cache_controls.size() > 0) )
    {
        // search for cache control header tag
        std::string::size_type pos_in_vector = 0;
        int num_tags_found = 0;
        for (auto const& cur_tag : value_array)
        {
            // search if there is more than one occurance:
            for (std::string::size_type i = 0; i < cache_controls.size(); i++)
            {
                if (cache_controls[i].find(cur_tag) != std::string::npos)
                {  // found tag!
                    pos_in_vector = i;  // save index for later usage
                    num_tags_found++;
                    if (num_tags_found > 1)
                    {
                        syntax_error = true;
                        break;
                    }
                    // note: no break, because we should double check for more than one occurance
                }
            }  // for cache_control
            if (num_tags_found == 1)
            {
                if (cache_control_string != nullptr)
                {
                    *cache_control_string = cache_controls[pos_in_vector];
                }
                result = CacheControlValueResult::kFound;
                break;
            }
            else
            if (num_tags_found == 0)
            {
                continue;  // may happen; search for next tag
            }
            if (syntax_error)
            {
                result = CacheControlValueResult::kSyntaxError;
                break;
            }
        }  // for value_array
    }
    return result;
}


HttpCacheDb& DefaultCache::GetHttpCacheDb()
{
    return http_cache_db_;
}


void DefaultCache::CloseHttpCacheDb()
{
    GetHttpCacheDb().Done();
}


bool DefaultCache::GetTimestampFromString(const std::string timestamp_str, std::time_t& timestamp)
{
    bool success = false;

    std::istringstream ss(timestamp_str);
    // normally this locale should be set, but causes a crash in TCAM: ss.imbue(std::locale("en_US.UTF-8"));  // we expect date format in US-English
    // examples of timestamp string: "Thu, 31 Aug 2017 11:23:24 UTC", "Thu, 31 Aug 2017 11:16:50 GMT"
    struct std::tm timestamp_tm = {};
    ss >> std::get_time(&timestamp_tm, "%a, %d %b %Y %H:%M:%S");  // ignoring here the possible "UTC"/"GMT" part
    if (! ss.fail())
    {
        std::time_t ts = mktime(&timestamp_tm);
        if (ts != kInvalidTimestamp)
        {
            timestamp = ts;
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_DEBUG, "%s returned timestamp '%s' = %" PRIi64,\
                __func__, timestamp_str.c_str(), static_cast<int64_t>(timestamp) );
            success = true;
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s conversion of '%s' failed",\
                __func__, timestamp_str.c_str());
            success = false;
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s parsing '%s' failed",\
          __func__, timestamp_str.c_str());
        success = false;
    }
    return success;
}


} // namespace transfermanager
} // namespace fsm

/** \}    end of addtogroup */
