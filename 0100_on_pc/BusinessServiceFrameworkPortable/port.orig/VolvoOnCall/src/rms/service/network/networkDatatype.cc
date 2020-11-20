////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file networkDatatype.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jiaojiao Shen
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <rms/service/network/networkDatatype.h>


namespace ndt {


//------------------------------------------------------------------------------
Payload& putTimeout(Payload& pl, uint8_t tm) {
    pl.insert(pl.begin(), tm);
    return pl;
}

//------------------------------------------------------------------------------
Payload& cutTimeout(Payload& pl) {
    pl.erase(pl.begin());
    return pl;
}

//------------------------------------------------------------------------------
uint8_t getTimeout(const Payload& pl) {
    return pl.front();
}

}
