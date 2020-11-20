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
 *  \file     message.cpp
 *  \brief    HTTP Message base-class implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup transfermanager
 *  \{
 */

// **** INCLUDES ***************************************************************
#include "message.h"

#include <vector>
#include <array>
#include <string>
#include <sstream>

#include "transfer_manager_types.h"


namespace fsm
{

namespace transfermanager
{

const std::array<std::string, static_cast<std::size_t>(Message::FieldKey::kMax)>
    Message::field_names_ = {
                             "Undefined",
                             "Host",
                             "Accept",
                             "Accept-Encoding",
                             "Accept-Language",
                             "User-Agent",
                             "Content-Type",
                             "Content-Length",
                             "Content-Encoding",
                             "X-Cc-Source",
                             "ID",
                             "ETag",
                             "Date",
                             "Strict-Transport-Security",
                             "Location",
                             "Cache-Control",
                             "Expires",
                             "Pragma",
                             "If-None-Match",
                             "If-Modified-Since"
};

std::string Message::ComputeHeaderRegex()
{
    std::string regex("(");

    const std::uint32_t kMaxFieldIndex = field_names_.size();

    for (std::uint32_t field_index = 1; field_index < kMaxFieldIndex; ++field_index)
    {
        const std::string kSeparator(field_index != kMaxFieldIndex - 1 ? "|" : "");

        regex += field_names_[field_index];
        regex += kSeparator;
    }

    regex += "): (.*)\r";

    return regex;
}

ErrorCode Message::AddHeaderField(FieldKey key, const std::string& value)
{
    ErrorCode err_code = ErrorCode::kNone;

    if (!(key > FieldKey::kUndefined  && key < FieldKey::kMax))
    {
        // key not within expected range.
        err_code = ErrorCode::kBadKey;
    }
    else
    {
        fields_[static_cast<std::uint32_t>(key)].push_back(value);
    }

    return err_code;
}

ErrorCode Message::GetHeaderFieldValues(FieldKey key, std::vector<std::string>& values) const
{
    ErrorCode err_code = ErrorCode::kNone;

    if (!(key > FieldKey::kUndefined  && key < FieldKey::kMax))
    {
        // key not within expected range.
        err_code = ErrorCode::kBadKey;
    }
    else
    {
        const std::vector<std::string>& field_values = fields_[static_cast<std::uint32_t>(key)];

        if (!field_values.size())
        {
            // Field-key has no references.
            err_code = ErrorCode::kBadKey;
        }
        else
        {
            values = field_values;
        }
    }

    return err_code;
}

ErrorCode Message::RemoveHeaderField(FieldKey key)
{
    ErrorCode err_code = ErrorCode::kNone;

    if (!(key > FieldKey::kUndefined  && key < FieldKey::kMax))
    {
        // key not within expected range.
        err_code = ErrorCode::kBadKey;
    }
    else
    {
        std::vector<std::string>& values = fields_[static_cast<std::uint32_t>(key)];

        if (!values.size())
        {
            // values are not present, nothing to clear.
            err_code = ErrorCode::kBadKey;
        }
        else
        {
            values.clear();
        }
    }

    return err_code;
}

ErrorCode Message::GetHeader(std::string& header_fields) const
{

    const std::uint32_t kKeyIdMax = static_cast<std::uint32_t>(FieldKey::kMax);
    const std::string kEndLine = "\r\n";
    const std::string kEndVal  = ",";

    std::ostringstream formatted_fields;

    std::uint32_t key_id = static_cast<std::uint32_t>(FieldKey::kUndefined);

    // iterate through all header fields and
    // append the values in formatted_fields.
    for (++key_id; key_id < kKeyIdMax; ++key_id)
    {
        const std::vector<std::string>& key_vals = fields_[key_id];

        if (key_vals.size())
        {
            const std::vector<std::string>::size_type kMaxKeyIndex = key_vals.size();
            std::vector<std::string>::size_type key_index = 0;

            formatted_fields << field_names_[key_id] << ": ";

            for (; key_index < kMaxKeyIndex; ++key_index)
            {
                 // if adding the last value, don't append anything else.
                 const std::string endVal = key_index == kMaxKeyIndex - 1 ? "" : kEndVal;

                formatted_fields << key_vals[key_index] << endVal;
            }

            formatted_fields << kEndLine;
        }
    }

    header_fields.append(formatted_fields.str());

    return ErrorCode::kNone;
}

ErrorCode Message::GetHeader(std::vector<std::string>& header_fields) const
{

    const std::uint32_t kKeyIdMax = static_cast<std::uint32_t>(FieldKey::kMax);
    const std::string kEndVal  = ",";

    std::uint32_t key_id = static_cast<std::uint32_t>(FieldKey::kUndefined);

    // iterate through all header fields and
    // append the values in formatted_fields.
    for (++key_id; key_id < kKeyIdMax; ++key_id)
    {
        const std::vector<std::string>& key_vals = fields_[key_id];

        if (key_vals.size())
        {
            std::ostringstream formatted_field;

            const std::vector<std::string>::size_type kMaxKeyIndex = key_vals.size();
            std::vector<std::string>::size_type key_index = 0;

            formatted_field << field_names_[key_id] << ": ";

            for (; key_index < kMaxKeyIndex; ++key_index)
            {
                 // if adding the last value, don't append anything else.
                 const std::string endVal = key_index == kMaxKeyIndex - 1 ? "" : kEndVal;

                formatted_field << key_vals[key_index] << endVal;
            }

            header_fields.push_back(formatted_field.str());
        }
    }

    return ErrorCode::kNone;
}

} // namespace transfermanager
} // namespace


/** \}    end of addtogroup */

