#ifndef VC_DIAGNOSTICS_INTERFACE_HPP
#define VC_DIAGNOSTICS_INTERFACE_HPP

/**
* Copyright (C) 2016 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file blem_pairing_service_interface.hpp
 * This file implements a library to talk to VCanager (pairing service) over dbus
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        22-Dec-2017
 ***************************************************************************/

#include <functional>

#include "vc_common.hpp"
#include "vehicle_comm_types.hpp"

namespace vc {

/**
    @brief BLE-interface for external users.

    This interface is used to perform interactions with the VehicleComm, related to the BLE.
*/
class IDiagnostics {
  public:
    // Requests
    virtual ReturnValue Request_SwitchToFlashBootloader(long session_id) = 0;
    virtual ReturnValue Request_LastBackupBatteryState(long session_id) = 0;
    virtual ReturnValue Request_GetVucSwBuild(long session_id) = 0;
    virtual ReturnValue Request_GetBleSwBuild(long session_id) = 0;
    virtual ReturnValue Request_VucSwVersion(long session_id) = 0;
    virtual ReturnValue Request_BleSwVersion(long session_id) = 0;
    virtual ReturnValue Request_DtcEntries(ReqDtcEntries *req, long session_id) = 0;
    virtual ReturnValue Request_DtcCtlSettings(ReqDtcCtlSettings *req, long session_id) = 0;
    virtual ReturnValue Request_GetTcamHwVersion(long session_id) = 0;
    virtual ReturnValue Request_GetRfVersion(long session_id) = 0;
    virtual ReturnValue Request_GetFblSwBuild(long session_id) = 0;
    virtual ReturnValue Request_CarConfigFactoryRestore(long session_id) = 0;
    virtual ReturnValue Request_VucTraceConfig(ReqVucTraceConfig *req, long session_id) = 0;
    virtual ReturnValue Request_VucTraceControl(ReqVucTraceControl *req, long session_id) = 0;
    virtual ReturnValue Request_GetDIDGlobalSnapshotData(long session_id) = 0;
    virtual ReturnValue Request_DTCsDetected(ReqDTCsDetected *req, long session_id) = 0;
    virtual ReturnValue Request_CarconfigParameterFaults(long session_id) = 0;
    virtual ReturnValue Request_NetworkManagementHistory(long session_id) = 0;
    virtual ReturnValue Request_ProgramPreCheck(long session_id)  = 0;
    virtual ReturnValue Request_RolloverValue(ReqRolloverValue *req, long session_id) = 0;
    virtual ReturnValue Request_SetRolloverCfg(ReqSetRolloverCfg *req, long session_id) = 0;

};

}; // namespace vc

#endif // VC_DIAGNOSTICS_INTERFACE_HPP
