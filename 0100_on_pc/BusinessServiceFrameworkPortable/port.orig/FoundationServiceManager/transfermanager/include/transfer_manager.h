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
 *  \file     transfer_manager.h
 *  \brief    Transfer Manager interface.
 *  \author   Iulian Sirghi & Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_H_
#define FSM_TRANSFERMANAGER_H_

// ** INCLUDES *****************************************************************
#include "transfer_manager.h"

#include <string>
#include <mutex>
#include <memory>
#include <cstddef>

#include "icommunicatorproxy.h"
#include "icommunicationmanagerproxy.h"

#include "transfer_manager_types.h"
#include "icache.h"
#include "request.h"
#include "response.h"


namespace fsm
{

namespace transfermanager
{

/**
 * \brief Call this method to initialise DLT tracing for transfer manager.
 */
void FSM_TransferManager_Initialise();


/**
 * \brief Call this method to terminate DLT tracing for transfer manager.
 */
void FSM_TransferManager_Terminate();


class TransferResponse;

/**
 * \brief Client-side Foundation Services Transfer Service counterpart.
 *
 * Abstracts over Foundation Services Transfer Service Client-side HTTP specifications by offering clients a RESTful
 * access interface to Cloud resources. By assuming the responsability to manage Transport-layer resources,
 * several key aspects of HTTP Application-layer protocol and Transfer Service specifications, clients are presented
 * with a interface of reduced complexity.
 *
 * As concerns such as scalability and performance are usually affected by network traffic, the possiblity to store
 * responses to requests can have a beneficial impact on client applications; Decisions on how to operate the cache
 * implementation is achieved by following HTTP 1.1 Cache specification (https://tools.ietf.org/html/rfc7234)
 * together with Foundation Services HTTP specification. RFC 7234 Cache directives are interpreted from the response
 * header-fields in order to deduce the operations dispatched to the cache interface. Typically, the cache is the first
 * layer where the response to a request is searched for; if the cache reports a miss, fallback to communication channel
 * is performed where the host is interrogated for the response. Only if the communication channel fails an error is
 * reported to the client. As far as the cache implementation is concerned, API clients are enabled to provide custom
 * cache implementations for application flexibility.
 *
 * Specifying a default host at object construction time will help in realizing requests that are formatted via relative
 * URIs.
 */
class TransferManager
{
public:
    /**
     * \brief Constructor.
     *
     * If no cache interface pointer is supplied by caller, TransferManager will fallback to a default cache
     * implementation.
     *
     * \param[in] host default host address we are to communicate with.
     * \param[in] cache cache implementation.
     */
    TransferManager(const std::string& host, std::shared_ptr<ICache> cache = 0);

    /**
     * \brief TransferManager constructor.
     */
    TransferManager();

    /**
     * \brief Destructor
     */
    ~TransferManager();

    /**
     * \brief Returns the cache implementation.
     *
     * If default cache implementation fallback occured, even if the cache implementation is null at object creation
     * time, a pointer to the default cache implementation will be returned; as a result, caller is to always expect
     * non-null pointer and is not required to check the pointer prior to accessing members of the interface instance.
     *
     * \return pointer to the cache implementation.
     */
    std::shared_ptr<ICache> GetCache();

    /**
     * \brief Dispatches a Read resource request to the remote host.
     *
     * \param[in] request encapsulates the details of the Read operation. Might be changed (added etag field)
     * \param[out] response remote-side future pattern operation result.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kBadValue if request object is in an invalid state,\n
     *         ErrorCode.kTimeout on Transport-layer failure,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode Read(Request& request, std::shared_ptr<Response>& response);

    /**
     * \brief Sets a globally available default host for instances that do not have an explicit one.
     *
     * \param[in] host default host
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kBadValue if host is invalid,\n
     *         ErrorCode.kNone on success.
     */
    static ErrorCode SetDefaultHost(const std::string& host);

    /**
     * \brief Retrieves the globally available default host for instances that do not have an explicit one.
     *
     * \param[out] host default host
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kNone on success.
     */
    static ErrorCode GetDefaultHost(std::string& host);

    /**
     * \brief Sets the valid flag in the http cache.
     * The flag can be set to tag this cache entry as invalid/stale although according to the rules in RFC7232 it is
     * not invalid/stale.
     *
     * \param[in] uri  uri (partly or full) of the cache entry to toggle the flag
     * \param[in] valid  value to set
     *
     * \return ErrorCode.kNone on success.
     *         ErrorCode.kBadState  record for uri not found or any other internal error
     */
    ErrorCode SetValidCacheFlag(const std::string uri, const bool valid);

    /**
     * \brief Gets the valid flag from the http cache.
     * The flag is used to tag this cache entry as invalid/stale although according to the rules in RFC7232 it is
     * not invalid/stale.
     *
     * \param[in] uri  uri (partly or full) of the cache entry to get the flag
     * \param[out] valid  value to get
     *
     * \return ErrorCode.kNone on success.
     *         ErrorCode.kBadState  entry for URI not found or any other internal error
     */
    ErrorCode GetValidCacheFlag(const std::string uri, bool &valid);


private:
    /**
     * \brief Internal CoMa instance state.
     *
     * CoMa-related instances are lazy-initialized. We use the following enum to track the initialization state.
     */
    enum class ComaState
    {
        kUninitialized,             ///< CoMa is not initialized. Querying for the CommunicationManagerProxy is expected.
        kClientInitializing,        ///< CoMa client instance is initializing.
        kClientInitialized,         ///< CoMa client instance is initialized.
        kCommunicatorActivating,    ///< CoMa Proxy is activating.
        kCommunicatorActivated,     ///< CoMa Proxy is available, first data request will initialize the data interface.
        kError,                     ///< CoMa error.

        kMax                        ///< Maximum value for this enum. Keep this last.
    };

    /**
     * \brief Disabled copy-constructor.
     */
    TransferManager(const TransferManager& other);

    /**
     * \brief Disabled assignment operator.
     */
    TransferManager& operator=(const TransferManager& other);

    /**
     * \brief Passes a given request to CoMa and retrieves the HTTP headers and content.
     *
     * \param[in] request encapsulates the HTTP request details.
     * \param[in] response remote-side future pattern operation result.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kBadValue if request object is in an invalid state,\n
     *         ErrorCode.kTimeout on Transport-layer failure,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode SendGetRequest(const Request& request,
                                 std::shared_ptr<TransferResponse> response);

    /**
     * \brief Coma CommunicatorProxy activation callback.
     *
     * \param[in] event activation event
     */
    void ComaActivationCallback(ICommunicatorProxy::EventCommunicatorActivationChanged& event);

    /**
     *
     * \brief Coma initialization callback.
     *
     * \param [in] init_response Coma initialization response.
     */
    static void ComaInitializationCallback(ResponseComaInitClient& init_response);

    /**
     * \brief Coma https response callback.
     *
     * \param[in,out] response response that is to be filled with data.
     * \param[in] callback wrapped https package data.
     */
    void ComaHttpsResponseCallback(std::shared_ptr<TransferResponse> response_1,
                                          ResponseCommunicatorDataHttpsGet& callback);

    /**
     * \brief Initializes the CoMa CommunicationManagerProxy client-side instance.
     *
     * \return ErrorCode.kBadValue if CommunicationManagerProxy was retrieved null,\n
     *         ErrorCode.kNone on success.
     */
    static ErrorCode InitializeComaClient();

    /**
     * \brief Creates the CoMa CommunicationManagerProxy client-side instance.
     *
     * \return ErrorCode.kBadValue if CoMa client initialization failed,\n
     *         ErrorCode.kNone on success.
     */
    static ErrorCode CreateComaClient();

    /**
     * \brief Creates the CoMa Communicator and data instances.
     *
     * \return ErrorCode.kTimeout on activation timeout,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode CreateComaCommunicator();

    /**
     * \brief checks the internal state of CoMa instances.
     *
     * \return ErrorCode.kBadState if CoMa Manager Proxy is not available or if CoMa is in a bad state,\n
     *         ErrorCode.kNone on success.
     */
    static ErrorCode CheckState();

    /**
     * \brief Prepends either host_ or default host (from persistency) to the given uri
     *
     * \param[in]  uri_in  input string
     * \param[out]  uri_out  output string, with prepended host. Both parameters may point to the same object.
     *
     * \return ErrorCode.kNone on success. Any other error code from \ref GetDefaultHost
     *
     */
    ErrorCode MakeAbsoluteUri(const std::string uri_in, std::string &uri_out);

    std::string host_;                                                 ///< default host.
    std::shared_ptr<ICache> cache_;                                    ///< pointer to cache implementation.

    ComaState coma_com_proxy_state_;                                   ///< CoMa CommunicatorProxy state.
    ICommunicatorData* coma_com_data_;                                 ///< CoMa Communicator Data.
    ICommunicatorProxy* coma_com_;                                     ///< CoMa Communicator.
    static const std::string kDefaultHostKey;                          ///< keyname in db for default host.
    static ComaState coma_client_state_;                               ///< CoMa Client state.
    static std::mutex global_sync_;                                    ///< global synchronization primitive.
    static std::condition_variable coma_response_;                     ///< CoMa response condition variable.
    static ICommunicationManagerProxy* coma_com_mgr_;                  ///< CoMa Proxy.
    static std::uint32_t coma_ref_count_;                              ///< Coma Reference count.
};

/**
 * \brief removes leading and trailing spaces in a given uri
 *
 * \param[in]  uri_in  input string to trim
 * \param[out]  uri_out  output string, which contains the result.
 */
void TrimUri(const std::string uri_in, std::string &uri_out);

} // namespace transfermanager
} // namespace fsm

#endif // #ifndef FSM_TRANSFERMANAGER_H_

/** \}    end of addtogroup */
