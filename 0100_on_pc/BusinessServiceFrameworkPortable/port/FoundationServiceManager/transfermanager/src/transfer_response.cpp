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
 *  \file     transfer_response.cpp
 *  \brief    Transfer Manager specialized response implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup transfermanager
 *  \{
 */

// **** INCLUDES ***************************************************************
#include "transfer_response.h"
#include <regex>


namespace fsm
{
namespace transfermanager
{

ErrorCode TransferResponse::Update(std::stringstream& data)
{

    State state;

    ErrorCode err_code = GetState(state);

    if (err_code == ErrorCode::kNone)
    {
        full_data_ = data.str();  // save full stream for database purpose
        payload_offset_ = 0;

        char character;

        while (state > State::kUndefined
               && state < State::kHeaderFields
               && (data >> std::noskipws >> character))
        {
            if (character == '\n')
            {
                ProcessHeaderLine();
                buffer_.str(std::string()); // reset stream

                err_code = GetState(state);
            }
            else
            {
                buffer_ << character;
            }
            payload_offset_++;
        }

        if (state >= State::kHeaderFields && state < State::kFull)
        {
            char buffer[1024];
            std::streamsize nr_read = 0;

            local_sync_.lock();
            do
            {
                nr_read = data.readsome(buffer, sizeof(buffer));
                response_.write(buffer, nr_read);
                content_read_byte_count_ += nr_read;

            } while (nr_read && content_read_byte_count_ < content_byte_count_);
            local_sync_.unlock();

            if (content_byte_count_ == content_read_byte_count_)
            {
                SetState(State::kFull);
            }
            else
            {
                SetState(State::kBodyUpdate);
            }
        }
    }

    return err_code;
}

void TransferResponse::Done()
{
    // There is the possibility that
    // the 'Content-Length' is missing from
    // header fields.
    // We manually set content byte count once the
    // response is fully populated.
    local_sync_.lock();
    content_byte_count_ = content_byte_count_ == 0 ? response_.str().length() :  content_byte_count_;
    local_sync_.unlock();

    SetState(State::kFull);
}

void TransferResponse::SetState(State state)
{
    if (state != State::kUndefined
         && state != State::kMax)
    {
        ev_sync_.lock();
        state_ = state;
        ev_sync_.unlock();

        ev_.notify_all(); //< wake-up threads waiting on state change.
    }
}

ErrorCode TransferResponse::SetStatusCode(StatusCode status_code)
{
    ErrorCode err_code = ErrorCode::kNone;

    if (state_ == State::kRequestQueued)
    {
        local_sync_.lock();
        status_code_ = status_code;
        local_sync_.unlock();

        // we have the status code, so transition to that state.
        SetState(State::kStatusCode);
    }
    else
    {
        err_code = ErrorCode::kBadState;
    }

    return err_code;
}

ErrorCode TransferResponse::SetContentLength(std::size_t content_length)
{
    ErrorCode err_code = ErrorCode::kNone;

    if (state_ == State::kRequestQueued)
    {
        local_sync_.lock();
        content_byte_count_ = content_length;
        local_sync_.unlock();
    }
    else
    {
        err_code = ErrorCode::kBadState;
    }

    return err_code;
}


std::string::size_type TransferResponse::GetPayloadOffset() const
{
    return payload_offset_;
}


void TransferResponse::SetUri(const std::string uri)
{
    uri_ = uri;
}


std::string TransferResponse::GetUri() const
{
    return uri_;
}


void TransferResponse::ProcessHeaderLine()
{
    std::regex status_line_regex("HTTP/\\d\\.\\d\\s+([\\d]+).*\r");
    std::smatch match;
    State state;

    if (GetState(state) == ErrorCode::kNone)
    {
        const std::string& kHeaderLine = buffer_.str();
        switch (state)
        {
            // If the request is queued, we read the status line in order to advance to the next state.
            case State::kRequestQueued:
            {
                if (std::regex_match(kHeaderLine, match, status_line_regex)
                    && match.size() == 2)
                {
                    std::stringstream status_code_stream(match[1]);
                    uint32_t status_code_number;
                    status_code_stream >> status_code_number;
                    StatusCode status_code(static_cast<StatusCode>(status_code_number));

                    SetStatusCode(status_code);
                    // Next step, get the headers.
                }
                else
                {
                    // no match! This is terrible.
                    // Most likely, we're not really getting HTTP,
                    // right now. We fail elegantly!
                    SetState(State::kConnectionError);
                }

                break;
            }

            // Once the status line is available, we are reading the header fields.
            case State::kStatusCode:
            {

                static const std::regex kHeaderFieldRegex(ComputeHeaderRegex());

                if (std::regex_match(kHeaderLine, match, kHeaderFieldRegex)
                    && match.size() == 3)
                {
                    std::string header_key = match[1];
                    std::string header_value = match[2];

                    std::array<std::string, static_cast<std::size_t>(Message::FieldKey::kMax)>::const_iterator it;

                    it = std::find(field_names_.begin(), field_names_.end(), header_key);
                    FieldKey key(static_cast<FieldKey>(it - field_names_.begin()));
                    AddHeaderField(key, header_value);

                    if (key == Message::FieldKey::kContentLength)
                    {
                        std::size_t content_length_header_value = 0;
                        std::stringstream value_stream(header_value);
                        value_stream >> content_length_header_value;

                        if (content_length_header_value > content_byte_count_)
                        {
                            local_sync_.lock();
                            content_byte_count_ = content_length_header_value;
                            local_sync_.unlock();
                        }
                    }
                }
                else if (kHeaderLine == std::string("\r"))
                {
                    // Header-fields are fully available.
                    SetState(State::kHeaderFields);
                }

                break;
            }
            default:
            {
                break;
            }
        }
    }
}

} // namespace transfermanager
} // namespace fsm

/** \}    end of addtogroup */
