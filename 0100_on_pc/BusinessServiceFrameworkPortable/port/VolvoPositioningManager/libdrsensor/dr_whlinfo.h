/////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file mobuildapp_service.h
//  This file handles the communication with the VuC, using the OTP VCS-

// @project     GLY_TCAM
// @subsystem   libdrsensor
// @author      Jianhui Li
// @Init date   31-Dec-2018
////////////////////////////////////////////////////////////////////////////

#ifndef DR_WHLINFO
#define DR_WHLINFO

#include "dr_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

bool DrWhlInfoInit();
void DrWhlInfoDeInit();
tpDR_vehData* DrGetDrWhlInfo();

#ifdef __cplusplus
}
#endif

#endif
