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
 *  \file     quark.cpp
 *  \brief    Quark data structure implementation
 *            association between string and int
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm_usermanager
 *  \{
*/

#include "quark.h"

namespace fsm
{

namespace usermanager_tool
{

Quark::Quark(const std::map<std::string, int>& values)
 : values_(values)
{

}

ReturnCode Quark::AtKey(const std::string& key, int& value)
{
    ReturnCode rc = ReturnCode::kSuccess;

    if (!values_.count(key))
    {
        rc = ReturnCode::kNotFound;
    }
    else
    {
        value = values_.at(key);
    }

    return rc;
}

ReturnCode Quark::AtValue(int value, std::string& key)
{
    ReturnCode rc = ReturnCode::kNotFound;

    std::map<std::string, int>::const_iterator it = values_.begin();

    for (; it != values_.end() && rc == ReturnCode::kNotFound; ++it)
    {
        if (it->second == value)
        {
            key = it->first;
            rc = ReturnCode::kSuccess;
        }
    }

    return rc;
}

const std::map<std::string, int>& Quark::GetMap() const
{
    return values_;
}

} // namespace usermanager_tool

} // namespace fsm

/** \}    end of addtogroup */
