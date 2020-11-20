/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     cache_utils.cpp
 *  \brief    Transfer Manager Cache utilities
 *  \author   Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

// ** INCLUDES *****************************************************************
#include "cache_utils.h"
#include <string>

namespace fsm
{

namespace transfermanager
{

bool Str2uint(const std::string str, unsigned long long int &value)
{
    bool result = false;  // true: success, false: parsing error

    std::string::size_type index = 0;  // character index of input string
    unsigned long long int temp_value = 0;  // contains the assembled nummeric value
    // leading whitespaces allowed and to be skipped
    while ( std::isspace(str[index]) && (index < str.size()) )
    {
        index++;
    }
    while ( std::isdigit(str[index]) && (index < str.size()) )
    {
        if (  __builtin_umulll_overflow(temp_value, 10u, &temp_value) ||
              __builtin_uaddll_overflow(temp_value, str[index] - '0', &temp_value) )
        // equivilant to: temp_value = temp_value * 10 + str[index] - '0'; but with check for overflow
        {
            result = false;
            break;
        }
        result = true;
        index++;
    }
    // trailing whitespaces allowed and to be skipped, everything else is a parsing error
    for (; result && (index < str.size()); index++)
    {
        if ( ! std::isspace(str[index]) )
        {
            result = false;
            break;
        }
    }
    if (result)
    {
        value = temp_value;
    }
    return result;
}

} // namespace transfermanager
} // namespace fsm

/** \}    end of addtogroup */
