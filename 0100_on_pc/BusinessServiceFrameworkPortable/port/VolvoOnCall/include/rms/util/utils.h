///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file utils.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
/////////////////////////////////////////////////////////////////////////////

#ifndef TCC_INCLUDE_SYS_UTILS_H_
#define TCC_INCLUDE_SYS_UTILS_H_

#include <stdint.h>
#include <sstream>

#define DBG_CONVERT_MILLI_2_MICRO    (1000UL)
#define DBG_CONVERT_MILLI_2_NANO    (1000000UL)

typedef uint32_t DBG_TICK_TYPE;

void printDump(const uint8_t* const data, const uint32_t size);

template<typename T> std::string to_string(const T& n) {
    std::ostringstream stm;
    stm << n;
    return stm.str();
}

uint8_t calcCrc(const uint8_t* data, uint32_t size);

DBG_TICK_TYPE DBG_getTicks(bool myMiliTicks);

#endif /* TCC_INCLUDE_SYS_UTILS_H_ */
