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
 *  \file     cache_utils.h
 *  \brief    Transfer Manager Cache utilities
 *  \author   Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */

#ifndef FSM_TRANSFERMANAGER_CACHE_UTILS_H_
#define FSM_TRANSFERMANAGER_CACHE_UTILS_H_

/// ** INCLUDES *****************************************************************
#include "cache_utils.h"
#include <string>


namespace fsm
{
/**
 * @namespace fsm
 */
namespace transfermanager
{

/**
 * \brief Converts a string into a unsigned int value, skips whitespaces before and after the nummeric part
 *  anything else before and after the nummeric part is a parsing error
 *  Examples (input, value, return):
 *  "123", 123, true
 *  "  123   ", 123, true
 *  "AB  123   ", n/a, false
 *  "123  ab ", n/a, false
 *  "123  12 ", n/a, false
 *  "abc", n/a, false
 *  "", n/a, false
 *  "18446744073709551617", n/a, false   would cause an overflow
 *  "18446744073709551615", 18446744073709551615, true  ULLONG_MAX
 *
 * \param[in] str  string as input
 * \param[out] value nummeric value as output
 *
 * \return true if parsing succeeded, false in case of parsing errors or overflow
 */
bool Str2uint(const std::string str, unsigned long long int &value);


} // namespace transfermanager
} // namespace fsm

#endif // #ifndef FSM_TRANSFERMANAGER_CACHE_UTILS_H_

/** \}    end of addtogroup */
