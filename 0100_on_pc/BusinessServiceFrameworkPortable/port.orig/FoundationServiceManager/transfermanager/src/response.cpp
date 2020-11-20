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
 *  \file     response.cpp
 *  \brief    Transfer Manager Response implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup transfermanager
 *  \{
 */

// **** INCLUDES ***************************************************************
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

Response::Response()
 : Message(),
   state_(State::kUndefined),
   response_(),
   local_sync_(),
   ev_sync_(),
   ev_(),
   content_byte_count_(0),
   content_read_byte_count_(0),
   status_code_(StatusCode::kUndefined)
{
}

ErrorCode Response::GetResponseBytesAvailable(std::size_t& bytes_available) const
{
    ErrorCode err_code = CheckState();

    // only proceed when state is considered okay.
    if (err_code == ErrorCode::kNone)
    {
        local_sync_.lock();
        bytes_available = response_.str().length();
        local_sync_.unlock();
    }

    return err_code;
}

ErrorCode Response::GetContentByteCount(std::size_t& byte_count) const
{
    std::size_t bytes_available = 0; // unused.

    // Wait for the header fields to be available.
    // If the header fields are available, it means we got the "Content-Length" already reported;
    // the above means content_byte_count_ is set.
    ErrorCode err_code = WaitFor(State::kHeaderFields, bytes_available);

    if (err_code == ErrorCode::kNone)
    {
        local_sync_.lock();
        byte_count = content_byte_count_;
        local_sync_.unlock();
    }

    return err_code;
}

ErrorCode Response::GetContentBytesAvailable(std::size_t& bytes_available) const
{
    State state;

    ErrorCode err_code = GetState(state);

    if (err_code == ErrorCode::kNone)
    {
        // State is okay - check if HTTP Content-Length is yet available.

        if (state < State::kHeaderFields)
        {
            // as header-fields are not yet available, we cannot deduce how
            // many bytes are available in message body; we shall report zero.
            bytes_available = 0;
        }
        else
        {
            local_sync_.lock();
            bytes_available = content_read_byte_count_;
            local_sync_.unlock();
        }
    }

    return err_code;
}

ErrorCode Response::GetStatusCode(StatusCode& status_code) const
{
    State state;
    std::size_t available_bytes; // unused

    ErrorCode err_code = GetState(state);

    err_code = err_code == ErrorCode::kNone && state < State::kStatusCode ?
              WaitFor(State::kStatusCode, available_bytes) : err_code;

    local_sync_.lock();
    status_code = status_code_;
    local_sync_.unlock();

    return err_code;
}

ErrorCode Response::GetState(State& state) const
{
    ErrorCode err_code = ErrorCode::kNone;

    // if state is not within kUndefined-kMax, then we're in a bad state, clearly.
    // otherwise, just return the state (where kConnectionError would be okay to return).
    local_sync_.lock();
    err_code = !(state_ > State::kUndefined && state_ < State::kMax) ? ErrorCode::kBadState : err_code;
    state = err_code == ErrorCode::kNone ? state_ : state;
    local_sync_.unlock();

    return err_code;
}

ErrorCode Response::Wait(State& state, std::size_t& bytes_available, const std::chrono::milliseconds& timeout) const
{
    ErrorCode err_code = GetState(state);

    if (err_code == ErrorCode::kNone)
    {
        if (state != State::kUndefined
            && state != State::kFull
            && state != State::kMax)
        {
            std::unique_lock<std::mutex> lk(ev_sync_);

            bool timed_out = false;

            if (timeout == std::chrono::milliseconds(0))
            {
                ev_.wait(lk);
            }
            else
            {
                timed_out = ev_.wait_for(lk, timeout) == std::cv_status::timeout ? true : false;
            }

            if (!timed_out)
            {
                local_sync_.lock();
                bytes_available = content_read_byte_count_;
                local_sync_.unlock();
            }
            else
            {
                err_code = ErrorCode::kTimeout;
            }
        }
    }

    return err_code;
}

ErrorCode Response::WaitFor(const State expected_state, std::size_t& bytes_available,
                            const std::chrono::milliseconds& timeout) const
{
    State current_state = State::kUndefined;

    ErrorCode err_code = GetState(current_state);

    if (err_code == ErrorCode::kNone)
    {

        if (current_state < expected_state)
        {
            do
            {
                err_code = Wait(current_state, bytes_available, timeout);
            } while (err_code == ErrorCode::kNone && current_state < expected_state);
        }

        if (err_code == ErrorCode::kNone)
        {
            local_sync_.lock();
            bytes_available = content_read_byte_count_;
            local_sync_.unlock();
        }
    }

    return err_code;
}

ErrorCode Response::Read(std::ostream& data, std::size_t& actual_bytes_read, std::size_t byte_count) const
{
    ErrorCode err_code;
    State state;

    err_code = GetState(state);
    uint32_t bytes_to_read = byte_count;

    do
    {
        std::size_t total_bytes = 0;

        err_code = WaitFor(State::kBodyUpdate, actual_bytes_read);
        err_code = err_code == ErrorCode::kNone ? GetContentByteCount(total_bytes) : err_code;

        bytes_to_read = byte_count == 0 ? total_bytes : byte_count;

    } while (err_code == ErrorCode::kNone && state != State::kFull && actual_bytes_read < bytes_to_read);

    if (err_code == ErrorCode::kNone)
    {
        local_sync_.lock();
        data << response_.rdbuf();
        local_sync_.unlock();
    }

    return err_code;
}

ErrorCode Response::AddHeaderField(FieldKey key, const std::string& value)
{
    std::lock_guard<std::mutex> lk_guard(local_sync_);

    return Message::AddHeaderField(key, value);
}

ErrorCode Response::RemoveHeaderField(FieldKey key)
{
    std::lock_guard<std::mutex> lk_guard(local_sync_);

    return Message::RemoveHeaderField(key);
}

ErrorCode Response::GetHeader(std::string& header_fields) const
{
    std::lock_guard<std::mutex> lk_guard(local_sync_);

    return Message::GetHeader(header_fields);
}

ErrorCode Response::GetHeader(std::vector<std::string>& header_fields) const
{
    std::lock_guard<std::mutex> lk_guard(local_sync_);

    return Message::GetHeader(header_fields);
}

ErrorCode Response::GetHeaderFieldValues(FieldKey key, std::vector<std::string>& values) const
{
    std::lock_guard<std::mutex> lk_guard(local_sync_);

    return Message::GetHeaderFieldValues(key, values);
}

ErrorCode Response::GetBody(std::string& message_body) const
{
    std::stringstream message_body_stream;
    ErrorCode err_code = ErrorCode::kNone;
    std::size_t actual_bytes; // unused.

    err_code = Read(message_body_stream, actual_bytes);

    if (err_code == ErrorCode::kNone)
    {
        message_body.append(message_body_stream.str());
    }

    return err_code;
}

ErrorCode Response::CheckState() const
{
    ErrorCode err_code = ErrorCode::kNone;

    // if state if within kUndefined and kMax, we shall consider ourselves in a valid state,
    // with the exception of kConnectionError.
    local_sync_.lock();
    err_code = !(state_ > State::kUndefined && state_ < State::kMax) ? ErrorCode::kBadState : err_code;
    err_code = state_ == State::kConnectionError ? ErrorCode::kBadState : err_code;
    local_sync_.unlock();

    return err_code;
}

} // namespace transfermanager
} // namespace fsm

/** \}    end of addtogroup */
