#ifndef VC_BLE_INTERFACE_HPP
#define VC_BLE_INTERFACE_HPP

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
class IBLE {
  public:
    // Requests
    virtual ReturnValue Request_BleDisconnect(ReqBleDisconnect *req, long session_id) = 0;
    virtual ReturnValue Request_BleDisconnectAll(long session_id) = 0;
    virtual ReturnValue Request_BleGenerateBdak(long session_id) = 0;
    virtual ReturnValue Request_BleGetBdak(long session_id) = 0;
    virtual ReturnValue Request_BleConfigDatacomService(ReqBleCfgDataCommService *req, long session_id) = 0;
    virtual ReturnValue Request_BleConfigIbeacon(ReqBleCfgIbeacon *req, long session_id) = 0;
    virtual ReturnValue Request_BleConfigGenericAccess(ReqBleCfgGenAcc *req, long session_id) = 0;
    virtual ReturnValue Request_BleStartPairingService(ReqBleStartPairService *req, long session_id) = 0;
    virtual ReturnValue Request_BleStopPairingService(long session_id) = 0;
    virtual ReturnValue Request_BleWakeupResponse(long session_id) = 0;
    virtual ReturnValue Request_BleDataAccessAuthConfirmed(ReqBleDataAccAuthCon *req, long session_id) = 0;
    virtual ReturnValue Request_BleStartAdvertising(ReqBleStartAdvertising *req, long session_id) = 0;
    virtual ReturnValue Request_BleStopAdvertising(long session_id) = 0;
};

}; // namespace vc

#endif // VC_BLE_INTERFACE_HPP
