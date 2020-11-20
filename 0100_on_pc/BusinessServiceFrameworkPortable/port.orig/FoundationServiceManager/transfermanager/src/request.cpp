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
 *  \file     request.cpp
 *  \brief    Transfer Manager Request implementation.
 *  \author   Iulian Sirghi & Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

// **** INCLUDES ***************************************************************
#include "request.h"

#include <string>
#include <map>

#include "transfer_manager_types.h"
#include "transfer_manager.h"
#include "message.h"

namespace fsm
{

namespace transfermanager
{

Request::Request()
    : Message(),
      uri_(),
      parameters_(),
      absolute_uri_(false)

{
}

Request::Request(const std::string& uri)
    : Message(),
      uri_(uri),
      parameters_(),
      absolute_uri_(false)
{
    ComputeUri();
}

ErrorCode Request::AddParameter(const std::string& parameter, const std::string& value)
{
    return ErrorCode::kNotImplemented;
}

ErrorCode Request::SetParameter(const std::string& parameter, const std::string& value)
{
    return ErrorCode::kNotImplemented;
}

ErrorCode Request::GetParameter(const std::string& parameter, std::string& value) const
{
    return ErrorCode::kNotImplemented;
}

ErrorCode Request::GetParameters(std::vector<std::pair<std::string, std::string>>& parameters) const
{
    return ErrorCode::kNotImplemented;
}

ErrorCode Request::SetUri(const std::string& uri)
{
    uri_ = uri;
    ComputeUri();

    return ErrorCode::kNone;
}

ErrorCode Request::GetUri(std::string& uri) const
{
    uri.append(uri_);

    return ErrorCode::kNone;
}

ErrorCode Request::HasAbsoluteUri(bool& has_absolute_uri) const
{
    has_absolute_uri = absolute_uri_;

    return ErrorCode::kNone;
}

ErrorCode Request::GetBody(std::string& message_body) const
{
    return ErrorCode::kNotImplemented;
}

void Request::ComputeUri()
{
    TrimUri(uri_, uri_);  // remove leading and trailing white spaces
    // if the URI start with a slash, then it is considered relative.
    absolute_uri_ = uri_.size() != 0 && uri_[0] != '/' ? true : false;
}

} // namespace transfermanager
} // namespace fsm

/** \}    end of addtogroup */
