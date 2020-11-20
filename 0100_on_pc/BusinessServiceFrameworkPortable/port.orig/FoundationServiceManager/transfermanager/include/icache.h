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
 *  \file     icache.h
 *  \brief    Transfer Manager Cache interface.
 *  \author   Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_ICACHE_H_
#define FSM_TRANSFERMANAGER_ICACHE_H_

/// ** INCLUDES *****************************************************************
#include "icache.h"

#include <string>
#include <memory>

#include "transfer_response.h"
#include "request.h"


namespace fsm
{
/**
 * @namespace fsm
 */
namespace transfermanager
{

/**
 * @namespace transfermanager
 */

/**
 * \brief This is the interface class for a http cache implementation
 *
 */
class ICache
{

public:

    /**
     * \brief Default destructor.
     */
    virtual ~ICache(){};

    /**
     * \brief Checks whether there is a valid (i.e. not expired/stale valid flag is "true")
     * and existing http cache entry for the given URI.
     * External users can set the valid flag (see \ref SetValidFlag) to false to force the cache even for a
     * non stale/none expired cache to go to the Internet and revalidate/refetch the data
     *
     * \param[in] uri  uri to search for
     *
     * \return true if there is a valid useable cache entry, false if cache is considered as stale
     */
    virtual bool HasValidCache(const std::string uri) = 0;

    /**
     * \brief Prepares request with HTTP flags from cache database
     *
     * \param[in,out] request  request; must contain valid URI. Might be changed also in case of failure!
     *
     * \return true in case all (!) actions have been done successfully, otherwise false.
     */
    virtual bool PrepareGetRequestFromCache(Request &request) = 0;

    /**
     * \brief handles a 200 "OK" response, including http cache database action
     * CAUTION: http cache record might be deleted wihin this method!
     *
     * \param[in] response  needed with parsed (extracted) header fields and saved uri
     *
     */
    virtual void HandleOkResponseInCache(const std::shared_ptr<TransferResponse> response) = 0;

    /**
     * \brief handles a 304 "not modified" response, including http cache database actions
     * CAUTION: in the unlikely event that the http return code is 304 and constains also a header line, which forbids
     * caching then the cache record will be deleted wihin this method!
     *
     * \param[in] response  needed with parsed (extracted) header fields and saved uri
     *
     */
    virtual void HandleNotModifiedInCache(const std::shared_ptr<TransferResponse> response) = 0;

    /**
     * \brief Fills the given response object with data from cache
     *
     * \param[in] response  needed with parsed (extracted) header fields and saved uri
     * \param[in] status_code http code to be returned to the waiting client
     *
     * \return true if entry for uri passed in response was found and updated http cache database; false if not or in case of any other error
     */
    virtual bool FillResponseFromCache(std::shared_ptr<TransferResponse> response, Response::StatusCode status_code) = 0;

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
    virtual ErrorCode SetValidFlag(const std::string full_uri, const bool valid) = 0;

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
    virtual ErrorCode GetValidFlag(const std::string full_uri, bool &valid) = 0;

};

} // namespace transfermanager
} // namespace fsm

#endif // #ifndef FSM_TRANSFERMANAGER_ICACHE_H_

/** \}    end of addtogroup */
