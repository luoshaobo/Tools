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
 *  \file     response.h
 *  \brief    Transfer Manager Response interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_RESPONSE_H_
#define FSM_TRANSFERMANAGER_RESPONSE_H_

// ** INCLUDES *****************************************************************
#include "response.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <memory>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "transfer_manager_types.h"
#include "message.h"


namespace fsm
{

namespace transfermanager
{

/**
 * \brief Remote-side response associated with a request.
 *
 * Reponse is conceptually based on the core aspects of the future pattern; this enabled clients to own an object that
 * represents the result an ongoing operation. C++11 standard offers such a construct with the limitation that the
 * result can never be partially retrieved.
 * Taking into account the design of the HTTP protocol, a client can take decisions on how to process a response without
 * requiring the full response to be available. One of such particularities include header-fields directives that can
 * change the way a client will react to a response.
 * Even in situations when small messages are passed between the client and it's host, the client is enabled to
 * wait or read the full response without additional house-keeping.
 */
class Response : public Message
{
public:
    /**
     * \brief Internal state of the future response.
     */
    enum class State
    {
        kUndefined,         ///< Uninitialized value.

        kConnectionError,   ///< Transport-layer connection error (ex, timeout, conenction refused).
        kRequestQueued,     ///< The associated request has been queued but no response has been generated.
        kStatusCode,        ///< Status line for the response is available and can be read.
        kHeaderFields,      ///< All header fields are available and can be read.
        kBodyUpdate,        ///< A message-body update occured. Multiple message-body updates can occur until full.
        kFull,              ///< The full response is available.

        kMax                ///< Maximum value for this enum. Keep this last.
    };

    /**
     * \brief HTTP 1.1 protocol Status-Line code.
     *
     * For documentation purposes please see the publicly available IETF RFC 2616.
     */
    enum class StatusCode
    {
        kUndefined,
        kContinue = 100,
        kSwitchingProtocols = 101,
        kOk = 200,
        kCreated = 201,
        kAccepted = 202,
        kNonAuthoritativeInformation = 203,
        kNoContent = 204,
        kResetContent = 205,
        kPartialContent = 206,
        kMultipleChoices = 300,
        kMovedPermanently = 301,
        kFound = 302,
        kSeeOther = 303,
        kNotModified = 304,
        kUseProxy = 305,
        // 306 code is marked as reserved
        kTemporaryRedirect = 307,
        kBadRequest = 400,
        kUnauthorized = 401,
        kPaymentRequired = 402,
        kForbidden = 403,
        kNotFound = 404,
        kMethodNotAllowed = 405,
        kNotAcceptable = 406,
        kProxyAuthentificationRequired = 407,
        kRequestTimeout = 408,
        kConflict = 409,
        kGone = 410,
        kLengthRequired = 411,
        kPreconditionFailed = 412,
        kRequestEntityTooLarge = 413,
        kRequestUriTooLong = 414,
        kUnsupportedMediaType = 415,
        kRequestRangeNotSatisfiable = 416,
        kExpectationFailed = 417,
        kInternalServerError = 500,
        kNotImplemented = 501,
        kBadGateway = 502,
        kServiceUnavailable = 503,
        kGatewayTimeout = 504,
        kHttpVersionNotSupported = 505,
        // own definition (proprietary code):
        kStaleCache = 900,  ///< due to connection error a stale cache entry has been returned
        kMax
    };

    /**
     * \brief Destructor.
     */
    virtual ~Response() { }

    /**
     * \brief Retrieves the number of bytes currently available in the response.
     *
     * The number retrieved represents the byte count of the full response; clearly stated it's the size of the response
     * components (status-line, header-fields, message body).
     *
     *
     * \param[out] bytes_available number of bytes currently available in the response.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy the operation,\n
     *         ErrorCode.kTimeout on Transport-layer failure,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetResponseBytesAvailable(std::size_t& bytes_available) const;

    /**
     * \brief Retrieves the full message body byte count.
     *
     * From the HTTP specifications, all responses contain a "Content-Length" field that mark the total number of bytes
     * in the message body. Clients can use this information to prepare reading operation (ex, pre-allocate resources).
     * If this information is requested but the "Content-Length" header field is not yet available, caller is blocked
     * until the mentioned header field is available in the response stream.
     *
     * \param[out] byte_count total number of bytes of the message body.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy the operation,\n
     *         ErrorCode.kTimeout on Transport-layer failure,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetContentByteCount(std::size_t& byte_count) const;

    /**
     * \brief Retrieves the byte count currently available in the message body.
     *
     * In order to support interactive client data processing, responses will get populated with data as soon as packages are
     * available. Reporting the total and the available byte count in the message body immediately allows clients to
     * get a clearer picture of how much data can be read immediately (non-blocking) for the next processing operation.
     *
     * \param[out] bytes_available number of bytes currently available in the message body.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy the operation,\n
     *         ErrorCode.kTimeout on Transport-layer failure,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetContentBytesAvailable(std::size_t& bytes_available) const;

    /**
     * \brief Retrieves the HTTP status code deduced from the status-line.
     *
     * If this information is requested but the "Status line" is not yet available, caller is blocked until it is in
     * the response stream.
     *
     *
     * \param[out] status_code HTTP response status code.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy the operation,\n
     *         ErrorCode.kTimeout on Transport-layer failure,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetStatusCode(StatusCode& status_code) const;

    /**
     * \brief Retrieves current internal state.
     * The mentioned internal state is reported immediately.
     *
     * \param[out] state current internal state of the response.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy the operation,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetState(State& state) const;

    /**
     * \brief Wait on any internal state update.
     *
     * \warning on Transport-layer failure, check the reported internal state because ErrorCode.kTimeout is used to
     * notify caller the reason for unblock is timeout expiry. (Additionally, \see State.kConnectionError)
     *
     * Blocking call until next internal state update. If clients need to wait for partial response, they
     * can wait/poll for the state that offers their needed information ex (status line, header fields or body update).
     * If transfering big web resources, the message body can be updated via several network packages; as such, clients
     * may need to write the current partial response somewhere else (ex, persistent storage) no matter the state of the
     * HTTP message.
     * Another advantage is that clients can perform response processing as soon as the data is available.
     * Additionally, clients can offer a timeout on internal state update. If no update occurs on the set amount of
     * milliseconds, caller will get unblocked. Waiting indefintely on a internal state update is achieved by setting
     * the timeout to zero.
     *
     *
     * \param[out] state internal state update that triggered the unblock.
     * \param[out] bytes_available nr of bytes available in the response stream at the moment the state update occured.
     * \param[in] timeout timeout in milliseconds for internal state update.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy the operation,\n
     *         ErrorCode.kTimeout on timeout expiry,\n
     *         ErrorCode.kNone on success (within supplied timeout).
     */
    ErrorCode Wait(State& state, std::size_t& bytes_available,
                   const std::chrono::milliseconds& timeout = std::chrono::milliseconds(0)) const;

    /**
     * \brief Wait on a specific internal state update.
     *
     * \warning on Transport-layer failure, check the reported internal state because ErrorCode.kTimeout is used to
     * notify caller the reason for unblock is timeout expiry (\see State.kConnectionError).
     *
     * Blocking call until the caller-specified response state. If clients need to process just a part of the response
     * they can wait for the state that is associated with that response. Clients are enabled to, for example, process
     * the header fields and make decisions without having to wait for the full response.
     *
     * \param[in] expected_state explicit internal state that is being waited on.
     * \param[out] bytes_available nr of bytes available in the response stream at the moment the state update occured.
     * \param[in] timeout in milliseconds for internal state update.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy the operation,\n
     *         ErrorCode.kTimeout on timeout expiry,\n
     *         ErrorCode.kNone on success (within supplied timeout).
     */
    ErrorCode WaitFor(const State expected_state, std::size_t& bytes_available,
                      const std::chrono::milliseconds& timeout = std::chrono::milliseconds(0)) const;

    /**
     * \brief Reads the message-body from response stream.
     * Enables clients to read the message-body content from the response stream either via blocking or non-blocking
     * calls. As stream state related information can be retrieved (full response length, full message-body length),
     * clients can decide the preferred reading method.
     * In order to achieve blocking IO, a client can supply the full message body count without asserting if that
     * amount is indeed currently available. If byteCount is default value, above-mentioned behaviour is achieved.
     * For non-blocking IO, a client can retrieve the byte count currently available and pass that value as the byte
     * count to be read; that data is immediately available and will be immediately retrieved.
     * If reading while message body is being updated and error occurs, the actual bytes read are reported to the client
     * in order to offer an idea on how data will be missing in the response.
     *
     * \param[out] data output stream that will be appended with the number of bytes requested from the response stream.
     * \param[out] actual_bytes_read number of bytes that were outputed into the stream during the reading operation.
     * \param[in] byte_count number of bytes requested.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy the operation,\n
     *         ErrorCode.kTimeout on Transport-layer failure,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode Read(std::ostream& data, std::size_t& actual_bytes_read, std::size_t byte_count = 0) const;

    /**
     * \brief Retrieves all values associated with the field-key.
     *
     * \param[in] key field-key to retrieve values for.
     * \param[out] values vector field-key values are appended to.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kBadKey unexpected field-key (ex, not within expected numerical space or does not exist),\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetHeaderFieldValues(FieldKey key, std::vector<std::string>& values) const override;

    /**
     * \brief Computes and retrieves the full HTTP message body.
     *
     * \param[out] message_body string the full HTTP message body content gets appended to.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy operation,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetBody(std::string& message_body) const override;

protected:
    /**
     * \brief Default constructor.
     */
    Response();

    /**
     * \brief Associates values to a header field-key.
     *
     * If the field-key has no prior values associated, the field-key will be created and with the values assigned;
     * otherwise, supplied values will be appended to the existing values.
     *
     * \warning Specialized implementations can pre-validate caller information.
     * If a key cannot hold one of the passed values an error is generated and returned.
     *
     * \param[in] key field-key to associate values to.
     * \param[in] value to be appended to field-key.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kBadKey unexpected field-key (ex, not within expected numerical space),\n
     *         ErrorCode.kBadValue key cannot reference supplied value(s),\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode AddHeaderField(FieldKey key, const std::string& value) override;

    /**
     * \brief Removes field-key and all it's associated values.
     *
     * All values that we're previously assigned to the field-key are removed along with the key. On operation success,
     * the key is considered not to exist.
     *
     * \param[in] key field-key that is to be removed.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kBadKey unexpected field-key (ex, not within expected numerical space or does not exist),\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode RemoveHeaderField(FieldKey key) override;

    /**
     * \brief Computes and retrieves the full HTTP header fields.
     *
     * \param[out] header_fields string the full HTTP header fields content gets appended to.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy operation,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetHeader(std::string& header_fields) const override;

    /**
     * \brief Computes and retrieves the HTTP header fields as separate lines.
     *
     * \param[out] header_fields vector of HTTP computed header fields
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy operation,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetHeader(std::vector<std::string>& header_fields) const override;

    /**
     * \brief Checks the internal object state to determine if object is in a consistent, safe-to-use state.
     *
     * \return ErrorCode.kNone if object is in a consistent state and can be used.
     */
    ErrorCode CheckState() const;

    volatile State state_;                  ///< current internal state.
    mutable std::stringstream response_;    ///< response stream.
    mutable std::mutex local_sync_;         ///< local synchronization primitive.
    mutable std::mutex ev_sync_;            ///< sync for condition variable.
    mutable std::condition_variable ev_;    ///< local event.
    std::size_t content_byte_count_;        ///< HTTP-reported "Content-Length" header-field value.
    std::size_t content_read_byte_count_;   ///< number of available bytes from message body.
    StatusCode status_code_;                ///< HTTP response status code.
};

} // namespace transfermanager
} // namespace fsm

#endif // #ifndef FSM_TRANSFERMANAGER_RESPONSE_H_

/** \}    end of addtogroup */
