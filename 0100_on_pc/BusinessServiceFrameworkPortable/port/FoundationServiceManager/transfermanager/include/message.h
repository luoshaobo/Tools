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
 *  \file     message.h
 *  \brief    HTTP Message base-class interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_MESSAGE_H_
#define FSM_TRANSFERMANAGER_MESSAGE_H_

// ** INCLUDES *****************************************************************
#include "message.h"

#include <vector>
#include <array>
#include <string>

#include "transfer_manager_types.h"


namespace fsm
{

namespace transfermanager
{

/**
 * \brief Wraps around required information about the request or response, or about the object sent in the message body.
 *
 * Information about the message body is described using HTTP header field-keys. Structural representation of key-value
 * pairs offers possibility of object reuse and pre-validation against Foundation Services Transfer Service client-side
 * specifications.
 *
 * \warning a field-key is considered to exist if and only if it has at least one value associated. If a field-key does
 *          not exist, it will not be present in the HTTP header fields content.
 */
class Message
{
public:

    /**
     * \brief HTTP Field-keys referenced in Foundation Services HTTP Specification.
     *
     * Literals within have a direct mapping to HTTP header fields. All mentioned literals can have one or more values
     * associated. The expected instance numerical space is (kUndefined, kMax).
     */
    enum class FieldKey
    {
        kUndefined,                 ///< Uninitialized value.
        kHost,                      ///< Host field-key.
        kAccept,                    ///< Accept field-key.
        kAcceptEncoding,            ///< Accept-Encoding field-key.
        kAcceptLanguage,            ///< Accept-Language field-key.
        kUserAgent,                 ///< User-Agent field-key.
        kContentType,               ///< Content-Type field-key.
        kContentLength,             ///< Content-Length field-key.
        kContentEncoding,           ///< Content-Encoding field-key.
        kXCcSource,                 ///< X-CC-Source field-key.
        kID,                        ///< ID field-key.
        kETag,                      ///< ETag field-key.
        kDate,                      ///< Date field-key.
        kStrictTransportSecurity,   ///< Strict-Transport-Security field-key.
        kLocation,                  ///< Location field-key.
        kCacheControl,              ///< Cache-Control field-key.
        kExpires,                   ///< Expires field-key
        kPragma,                    ///< Pragma field-key
        kIfNoneMatch,               ///< If-None-Match field-key (request only)
        kIfModifiedSince,           ///< If-Modified-Since field-key (request only)
        kMax                        ///< Maximum value for this enum. Keep this last.
    };

    /**
     * \brief Destructor
     */
    virtual ~Message() { }

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
    virtual ErrorCode AddHeaderField(FieldKey key, const std::string& value);

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
    virtual ErrorCode GetHeaderFieldValues(FieldKey key, std::vector<std::string>& values) const;

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
    virtual ErrorCode RemoveHeaderField(FieldKey key);

    /**
     * \brief Computes and retrieves the full HTTP header fields.
     *
     * \param[out] header_fields string the full HTTP header fields content gets appended to.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy operation,\n
     *         ErrorCode.kNone on success.
     */
    virtual ErrorCode GetHeader(std::string& header_fields) const;

    /**
     * \brief Computes and retrieves the HTTP header fields as separate lines.
     *
     * \param[out] header_fields vector of HTTP computed header fields.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy operation,\n
     *         ErrorCode.kNone on success.
     */
    virtual ErrorCode GetHeader(std::vector<std::string>& header_fields) const;

    /**
     * \brief Computes an retrieves the full HTTP message body.
     *
     * \param[out] message_body string the full HTTP message body content gets appended to.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy operation,\n
     *         ErrorCode.kNone on success.
     */
    virtual ErrorCode GetBody(std::string& message_body) const = 0;

protected:
    /**
     * \brief Default constructor
     */
    Message() = default;

    /**
     * \brief Computes a regex that can match accepted HTTP header fields.
     *
     * \return string representation of HTTP header fields regex
     */
    static std::string ComputeHeaderRegex();

    std::array<std::vector<std::string>,
               static_cast<std::size_t>(FieldKey::kMax)> fields_; ///< lookup via field-key to vector of string values.

    static const std::array<std::string,
                      static_cast<std::size_t>(FieldKey::kMax)> field_names_; ///< lookup via field-key to name.

    /**
     * \brief Default copy construction.
     *
     * \param[in] other message to copy.
     */
    Message(const Message& other) = default;

    /**
     * \brief Default assignment operator.
     *
     * \param[in] other message to copy.
     */
    Message& operator=(const Message& other) = default;
};

} // namespace transfermanager
} // namespace fsm

#endif // #ifndef FSM_TRANSFERMANAGER_MESSAGE_H_

/** \}    end of addtogroup */
