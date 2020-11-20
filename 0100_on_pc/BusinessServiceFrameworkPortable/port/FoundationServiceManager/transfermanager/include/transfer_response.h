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
 *  \file     transfer_response.h
 *  \brief    Transfer Manager specialized response interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_TRANSFER_RESPONSE_H_
#define FSM_TRANSFERMANAGER_TRANSFER_RESPONSE_H_

/// ** INCLUDES *****************************************************************

#include "transfer_response.h"

#include <sstream>
#include <string>
#include <cstddef>

#include "response.h"

namespace fsm
{

namespace transfermanager
{

/**
 * \brief Specialisation class of Response class
 * Associates a current transfer to a generic response object
 * Includes state handling for transfer and payload handling
 */
class TransferResponse : public Response
{
public:
    /**
     * \brief Default constructor.
     */
    TransferResponse() = default;

    /**
     * \brief Default constructor.
     */
    ~TransferResponse() = default;

    /**
     * \brief Updates the internal stream while transitioning internal states.
     *
     *
     * \param[in] data input stream to update the internal stream.
     *
     * \return ErrorCode.kNone on success.
     */
    ErrorCode Update(std::stringstream& data);

    /**
     * \brief Finishes all internal stream updates and transitions the object to State.kFull.
     */
    void Done();

    /**
     * \brief Manually transitions the object to the specified state while notifying all listeners.
     *
     * \param[in] state
     */
    void SetState(State state);

    /**
     * \brief Sets the status code of the response.
     *        Will update state, may only be called in
     *        state kRequestQueued.
     *
     *
     * \param[in] status_code The code to set.
     *
     * \return ErrorCode.kNone on success.
     */
    ErrorCode SetStatusCode(StatusCode status_code);

    /**
     * \brief Sets the content length of the response.
     *        For processing purposes will override any
     *        content_length header value unless header
     *        value is larger.
     *        May only be called in state kRequestQueued.
     *
     *
     * \param[in] content_length The length to set.
     *
     * \return ErrorCode.kNone on success.
     */
    ErrorCode SetContentLength(std::size_t content_length);

    /**
     * \brief Returns offset value of payload vs. header data in BLOB
     *
     * \return offset value
     */
    std::string::size_type GetPayloadOffset() const;

    /**
     * \brief Returns full http data (including header and payload) in a string
     *
     * \return payload
     */
    const std::string GetFullData() const
    {
        return full_data_;
    };

    /**
     * \brief Sets the URI value in response
     *
     * \param[in] uri to set
     */
    void SetUri(const std::string uri);

    /**
     * \brief Gets the URI value from response
     *
     * \return uri as a string
     */
    std::string GetUri() const;


private:
    /**
     * \brief Process the current line-buffer.
     */
    void ProcessHeaderLine();  ///< reads all header lines, parses key value pair and calls \ref AddHeaderField to store them

    std::stringstream buffer_; ///< line-buffering stream.

    std::string uri_;  ///< save uri; needed to save reply in cache

    std::string full_data_;  ///< including header lines and payload
    std::string::size_type payload_offset_ = 0;  ///< offset of payload data in full_data

};

} // namespace transfermanager
} // namespace fsm

#endif // #ifndef FSM_TRANSFERMANAGER_TRANSFER_RESPONSE_H_

/** \}    end of addtogroup */
