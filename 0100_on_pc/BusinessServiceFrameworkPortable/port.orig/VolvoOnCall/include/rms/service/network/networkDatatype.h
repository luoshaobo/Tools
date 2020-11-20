////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file networkDatatype.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <rms/sys/postoffice.h>


namespace ndt {

struct NetworkDataTm {
    uint8_t timeout;
    uint8_t data[0];
};

Payload& putTimeout(Payload& pl, uint8_t tm);
Payload& cutTimeout(Payload& pl);
uint8_t getTimeout(const Payload& pl);

}
