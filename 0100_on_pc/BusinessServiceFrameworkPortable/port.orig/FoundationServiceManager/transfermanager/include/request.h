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
 *  \file     request.h
 *  \brief    Transfer Manager Request interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_REQUEST_H_
#define FSM_TRANSFERMANAGER_REQUEST_H_

// ** INCLUDES *****************************************************************
#include "request.h"

#include <string>
#include <map>

#include "transfer_manager_types.h"
#include "message.h"


namespace fsm
{

namespace transfermanager
{

/**
 * \brief Specialized HTTP message representing a remote resource.
 *
 * Encompasses a remote resource identifier and the way that resource is to be operated upon (via parameters). One of
 * the advantages of a self-contained type is possibility to reuse the instance across all associated CRUD operations.
 * The caracteristics of the request object (ex, if it's either relative or absolute) is taken internally. If the
 * request uri is formatted as a relative request, the default host address is taken from the TransferManager instance.
 */
class Request : public Message
{
public:
    /**
     * \brief Default constructor.
     */
    Request();

    /**
     * \brief Copy constructor
     */
    Request(const Request& other) = default;

    /**
     * \brief Constructor via URI.
     *
     * \param[in] uri absolute or relative to host URI.
     */
    Request(const std::string& uri);

    /**
     * Destructor.
     */
    ~Request() = default;

    /**
     * \brief Adds a resource key-value pair.
     *
     * \param[in] parameter name of parameter key
     * \param[in] value value associated with the key
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kBadKey key already has a value associated,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode AddParameter(const std::string& parameter, const std::string& value);

    /**
     * \brief Replaces the value of a previously added key.
     * The caller-supplied key must already be created by a previous AddParameter operation.
     *
     * \param[in] parameter name of parameter key
     * \param[in] value new value the key is to be associated with.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kBadKey if key does not exist,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode SetParameter(const std::string& parameter, const std::string& value);

    /**
     * \brief Retrieves the value of a previously added key-value pair.
     *
     * \param[in] parameter name of parameter key
     * \param[out] value value associated with the key.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kBadKey if key does not exist,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetParameter(const std::string& parameter, std::string& value) const;

    /**
     * \brief Retrieves all stored key-value pairs.
     *
     * \param[in] parameters vector key-value pairs will be appended to.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy the operation,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetParameters(std::vector<std::pair<std::string, std::string>>& parameters) const;

    /**
     * \brief Sets or resets the URI.
     *
     * \param[in] uri relative or absolute URI.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot be altered,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode SetUri(const std::string& uri);

    /**
     * \brief Retrieves the URI.
     *
     * \param[out] uri string the URI will be appended to.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and and cannot satisfy the operation,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetUri(std::string& uri) const;

    /**
     * \brief Determines if the stored uri is absolute.
     *
     * \param[out] has_absolute_uri true if uri is absolute, false if it's relative.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and and cannot satisfy the operation,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode HasAbsoluteUri(bool& has_absolute_uri) const;

    /**
     * \brief Computes and retrieves the full HTTP message body.
     *
     * \param[out] message_body string the full HTTP message body content gets appended to.
     *
     * \return ErrorCode.kBadState if object is in a ambiguous state and cannot satisfy operation,\n
     *         ErrorCode.kNone on success.
     */
    ErrorCode GetBody(std::string& message_body) const override;

private:
    void ComputeUri();                              ///< Determines if the internal uri is absolute of not.

    std::string uri_;                               ///< Absolute or relative uri.
    std::map<std::string, std::string> parameters_; ///< Mapping between parameter name and value.
    bool absolute_uri_;                             ///< precomputed absolute uri flag. True is uri is absolute.
};

} // namespace transfermanager
} // namespace fsm

#endif // #ifndef FSM_TRANSFERMANAGER_REQUEST_H_

/** \}    end of addtogroup */
