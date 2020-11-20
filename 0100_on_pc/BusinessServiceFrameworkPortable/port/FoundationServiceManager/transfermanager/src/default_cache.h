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
 *  \file     default_cache.h
 *  \brief    Transfer Manager Default Cache fallback interface.
 *  \author   Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_DEFAULT_CACHE_H_
#define FSM_TRANSFERMANAGER_DEFAULT_CACHE_H_

/// ** INCLUDES *****************************************************************

#include "default_cache.h"

#include <string>
#include <memory>

#include "icache.h"
#include "http_cache_db.h"
#include "transfer_response.h"
#include "request.h"


namespace fsm
{

namespace transfermanager
{

/**
 * \brief Transfer Manager default cache implementation.
 */
class DefaultCache : public ICache
{

public:

    /**
     * \brief Default constructor.
     */
    DefaultCache();

    /**
     * \brienf Default destructor.
     */
    virtual ~DefaultCache();

    /**
     * \brief Checks whether there is a valid and usable http cache entry for the given URI
     * According to RFC 7234 this method also checks whether there is a valid system time, if not: cache is invalid
     * URI is supposed to be full and without leading/trailing spaces
     *
     * \param[in] uri  uri to search for
     *
     * \return true if there is a valid useable cache entry, false if cache is considered as stale
     */
    virtual bool HasValidCache(const std::string uri);

    /**
     * \brief Prepares request with HTTP flags from cache database; adds etag and If-Modified-Since header values
     *
     * \param[in,out] request  request; must contain valid URI. Might be changed also in case of failure!
     *
     * \return true in case all (!) actions have been done successfully, otherwise false.
     */
    virtual bool PrepareGetRequestFromCache(Request &request);

    /**
     * \brief handles a 200 "OK" response, including http cache database action
     * CAUTION: http cache record might be deleted wihin this method!
     *
     * \param[in] response  needed with parsed (extracted) header fields and saved uri
     *
     */
    virtual void HandleOkResponseInCache(const std::shared_ptr<TransferResponse> response);

    /**
     * \brief handles a 304 "not modified" response, including http cache database actions
     * CAUTION: in the unlikely event that the http return code is 304 and constains also a header line, which forbids
     * caching then the cache record will be deleted wihin this method!
     *
     * \param[in] response  needed with parsed (extracted) header fields and saved uri
     *
     */
    virtual void HandleNotModifiedInCache(const std::shared_ptr<TransferResponse> response);

    /**
     * \brief Fills the given response object with data from cache
     *
     * \param[in] response  needed with parsed (extracted) header fields and saved uri
     * \param[in] status_code http code to be returned to the waiting client
     *
     * \return true if entry for uri passed in response was found and updated http cache database; false if not or in case of any other error
     */
    virtual bool FillResponseFromCache(std::shared_ptr<TransferResponse> response, Response::StatusCode status_code);

    /**
     * \brief Sets the valid flag in the http cache.
     * The flag can be set to tag this cache entry as invalid/stale although according to the rules in RFC7232 it is
     * not invalid/stale.
     *
     * \param[in] full_uri  full uri of the cache entry to toggle the flag
     * \param[in] valid  value to set
     *
     * \return ErrorCode.kNone on success.
     *         ErrorCode.kBadState  record for uri not found or any other internal error
     */
    virtual ErrorCode SetValidFlag(const std::string full_uri, const bool valid);

    /**
     * \brief Gets the valid flag from the http cache.
     * The flag is used to tag this cache entry as invalid/stale although according to the rules in RFC7232 it is
     * not invalid/stale.
     *
     * \param[in] full_uri  full uri of the cache entry to get the flag
     * \param[out] valid  value to get
     *
     * \return ErrorCode.kNone on success.
     *         ErrorCode.kBadState  entry for URI not found or any other internal error
     */
    virtual ErrorCode GetValidFlag(const std::string full_uri, bool &valid);

protected:

    const std::time_t kInvalidTimestamp = (std::time_t)-1;

    /**
     * \brief Class, which contains search result for "Cache-Control" tags
     */
    enum class CacheControlValueResult
    {
        kNotFound,    ///< no cache control foun, which matches the search request tag
        kFound,       ///< one cache control found, which matches the search request tag
        kSyntaxError  ///< syntax error detected, i.e. more than one tag present
    };

    /**
     * \brief Write the current response data into cache; adds or updates existing data
     * URI is supposed to be full and without leading/trailing spaces
     *
     * \param[in] response  needed with parsed (extracted) header fields, uri and data (header with payload)
     * \param[in] expiry_timestamp  same value as returned by \ref GetExpiryTimestampFromHeader
     * \param[in] expiry_timestamp_relative_value  same value as returned by \ref GetExpiryTimestampFromHeader
     * \param[in] received_timestamp  received time stamp (absolute)
     *
     * \return true writing succeeded, false in case of any error (incl. not able to calculate new absolute exipry timestamp)
     */
    bool WriteResponseToCache(const std::shared_ptr<TransferResponse> response,
        const std::time_t expiry_timestamp, const bool expiry_timestamp_relative_value,
        const std::time_t received_timestamp);

    /**
     * \brief Updates validity flags (expiry_timestamp, etag, valid_flag=true, if available: received timestamp) in
     * http cache database
     * In case it is not possible to calculate the expiry date the database is not touched.
     *
     * \param[in] response  needed with parsed (extracted) header fields and saved uri
     * \param[in] expiry_timestamp  same value as returned by \ref GetExpiryTimestampFromHeader
     * \param[in] expiry_timestamp_relative_value  same value as returned by \ref GetExpiryTimestampFromHeader
     * \param[in] received_timestamp  received time stamp (absolute)
     *
     * \return true if entry for uri was found and updated http cache database; false if not or in case of any other error
     */
    bool UpdateValidityFlagsInCache(const std::shared_ptr<TransferResponse> response,
        const std::time_t expiry_timestamp, const bool expiry_timestamp_relative_value,
        const std::time_t received_timestamp);

    /**
     * \brief checkes whether the expiry_timestamp is older than the current time.
     * Checks also for valid system time and valid received_timestamp.
     * If both are invalid than the cache is considered as stale.
     * All timestamps are in UTC (GMT).
     *
     * \param[in] expiry_timestamp  timestamp of expiry
     * \param[in] received_timestamp  timestamp when data was received (either provided by server or taken from internal clock)
     *
     * \return true if cache is stale  or any other error happened; false if not
     */
    bool IsCacheStale(const std::time_t expiry_timestamp, const std::time_t received_timestamp);

    /**
     * \brief Calculates new absolute expiry time stamp; needs valid system clock (as of 2017-12-15 clock is unreliable)
     *
     * \param[in] response  needed with parsed (extracted) header fields, uri and data (header with payload)
     * \param[in] expiry_timestamp  same value as returned by \ref GetExpiryTimestampFromHeader
     * \param[in] expiry_timestamp_relative_value  same value as returned by \ref GetExpiryTimestampFromHeader
     * \param[in] received_timestamp  received time stamp (absolute)
     *
     * \return new timestamp in case of success, kInvalidTimestamp in case of errors
     */
    std::time_t CalculateExpiryTimestamp(const std::shared_ptr<TransferResponse> response,
        const std::time_t expiry_timestamp, const bool expiry_timestamp_relative_value, const std::time_t received_timestamp);

    /**
     * \brief Gets the received timestamp either from http header or from valid system clock
     *
     * \param[in] response  needed with parsed (extracted) header fields
     *
     * \return new timestamp in case of success, kInvalidTimestamp in case of errors
     */
    std::time_t GetReceivedTimestamp(const std::shared_ptr<TransferResponse> response);

    /**
     * \brief checkes whether the header fields contains timestamps for expiry, checks for syntax validity
     * and returns the most relevant one (according to RFC 7234).
     * The following header fields are considered in this priority for expiry_timestamp and converted (if needed):
     * Cache-Control: s-maxage=3600   // relative value in seconds
     * Cache-Control: max-age=3600   // relative value in seconds
     * Expires: Thu, 07 Sep 2017 11:23:24 UTC   // absolute value
     *
     * \param[in] response  needed with parsed (extracted) header fields
     * \param[out] expiry_timestamp   found and calculated expiry timestamp from the header fields either relative or absolute
     * \param[out] relative_value  true: returned expiry_timestamp is a relative value, false: absolute value
     *
     * \return true if at least one valid fields for expiry_timestamp is found, false if not
     */
    bool GetExpiryTimestampFromHeader(const std::shared_ptr<TransferResponse> response,
        std::time_t &expiry_timestamp, bool &relative_value);

    /**
     * \brief gets the recevied timestamp value as string for the given URI from the http cache database
     * URI is supposed to be full and without leading/trailing spaces
     *
     * \param[in] uri  uri to search for
     * \param[out] received_timestamp_str  filled string with timestamp in format "Tue, 15 Nov 1994 08:12:31 GMT" \
     * according to  \ref https://tools.ietf.org/html/rfc7231#section-7.1.1.1  if found in DB, otherwise untouched
     *
     * \return true if entry for uri was found in http cache database; false if not or in case of any other error
     */
    bool GetReceivedTimestampFromCache(const std::string uri, std::string &received_timestamp_str);

    /**
     * \brief checks whether the header fields contains a "Date:" timestamps; checks for valid syntax
     * The following field is used for the received date (with example value):
     * Date: Thu, 31 Aug 2017 11:12:20 UTC
     *
     * \param[in] response  needed with parsed (extracted) header fields
     * \param[out] received_timestamp valid time stamp; untouched in case of errors
     *
     * \return true if a valid field with Date is found, false if not or in any error case
     */
    bool GetReceivedTimestampFromHeader(const std::shared_ptr<TransferResponse> response, std::time_t &received_timestamp);


    /**
     * \brief gets the etag value for the given URI from the http cache database
     * URI is supposed to be full and without leading/trailing spaces
     *
     * \param[in] uri  uri to search for
     * \param[out] etag  filled etag string if found in DB, otherwise untouched
     *
     * \return true if entry for uri was found in http cache database; false if not or in case of any other error
     */
    bool GetEtagFromCache(const std::string uri, std::string &etag);

    /**
     * \brief checkes whether the header fields contains ETags and returns it.
     * If more than one is found it concatinates them
     * The following field(s) are ETags (with example value):
     * ETag: "4ff6b11-ad6-44dde556b188e"
     *
     * \param[in] response  needed with parsed (extracted) header fields
     * \param[out] etag found etag strings, if more than one is present they are concatinated and separated by space
     *
     * \return true if at least one wtag was found, false if not or any error happend
     */
    bool GetETagFromHeader(const std::shared_ptr<TransferResponse> response, std::string &etag);

    /**
     * \brief checks whether the header fields contains at least one of the following values, which forbids caching;
     * Cache-Control: no-cache
     * Cache-Control: no-store
     * Cache-Control: must-revalidate
     * Pragma: no-cache
     *
     * \param[in] response  needed with parsed (extracted) header fields
     *
     * \return true if at least one of the values is found, false if not or in any error case
     */
    bool HasDontCacheControlHeader(const std::shared_ptr<TransferResponse> response);

    /**
     * \brief searches for key-value "Cache-Control: XXX" in response header and return its value XXX.
     * the list of values ("XXX") to search for is passed as a parameter
     * Example 1: if you want to search for the following tags
     * Cache-Control: no-cache
     * Cache-Control: no-store
     * You should provide a value_array = {"no-cache", "no-store"}
     * Here you are normally only interested in the presence of the tag itself and provide cache_control_string = nullptr
     * Example 2:
     * Cache-Control: max-age=1234
     * you might be interested to get the value "max-age=1234" back in parameter cache_control_string
     * Does also check whether more than one tag is present (which is a syntax error)
     *
     * \param[in] response  needed with parsed (extracted) header fields
     * \param[in] value_array  vector of values to search for
     * \param[out] cache_control_string  returned first value string (if found)
     *
     * \return \ref CacheControlValueResult
     */
    CacheControlValueResult GetCacheControlValue(
        const std::shared_ptr<TransferResponse> response,
        const std::vector<std::string> value_array,
        std::shared_ptr<std::string> cache_control_string = nullptr);

    /**
     * \brief Gets the HTTP Cache DB pointer; creates/opens DB if necessary
     * Original purpose was to have a singleton, which is now an ordinary member of the class
     *
     * \return pointer to Http Cache DB object
     */
    HttpCacheDb& GetHttpCacheDb();

    /**
     * \brief Closes the HTTP Cache DB pointer; must be called to properly close the underlying DB
     * Original purpose was to have a singleton, which is now an ordinary member of the class
     *
     */
    void CloseHttpCacheDb();

    /**
     * \brief parses given string for absolute time stamps like "Thu, 31 Aug 2017 11:12:20 UTC" and returns
     *  converted value in std::time_t format
     *
     * \param[in] timestamp_str  string with time stamp to be converted
     * \param[out] timestamp valid time stamp; untouched in case of errors
     *
     * \return true if conversion succeeds, false if not
     */
    bool GetTimestampFromString(const std::string timestamp_str, std::time_t& timestamp);

    HttpCacheDb http_cache_db_;  ///< object of http cache
};

} // namespace transfermanager
} // namespace fsm

#endif // #ifndef FSM_TRANSFERMANAGER_DEFAULT_CACHE_H_

/** \}    end of addtogroup */

