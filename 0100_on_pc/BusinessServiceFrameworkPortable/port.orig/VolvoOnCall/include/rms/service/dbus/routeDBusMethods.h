////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file routeDBusMethods.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
////////////////////////////////////////////////////////////////////////////

#ifndef PRJ_SOC_FORD_PACKAGE_XEVCDM_INCLUDE_SERVICE_DBUS_ROUTEDBUSMETHODS_H_
#define PRJ_SOC_FORD_PACKAGE_XEVCDM_INCLUDE_SERVICE_DBUS_ROUTEDBUSMETHODS_H_

#include <rms/dbus_interface/xEVDBusStuctures.h>

#ifdef __cplusplus
extern "C" {
#endif

void routeDBusPeriodicData(const PeriodicDBUSData* data);
void routeConnectionParams(const ServerConnectionParams* data);
void routeConfigurationParams(const xEVConfigurations* data);
void routeStartXEVCDM();
void routeStopXEVCDM();
void routeEnterWarningMode();
void routeWarningData(const WarningDbusData *warningData);
void routeConvertCableData(WarningDbusData *warningData, char* convertedData, uint32_t *size);
void routeSetCableACKStatusData(char status);
void routeGetNextCableData();

#ifdef __cplusplus
}
#endif

#endif /* PRJ_SOC_FORD_PACKAGE_XEVCDM_INCLUDE_SERVICE_DBUS_ROUTEDBUSMETHODS_H_ */
