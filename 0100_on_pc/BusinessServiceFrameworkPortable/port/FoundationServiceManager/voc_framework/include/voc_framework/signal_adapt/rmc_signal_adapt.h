///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file rmc_signal_adapt.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Nie Yujin
// @Init date	13-Mar-2019
///////////////////////////////////////////////////////////////////

#ifndef RMC_SIGNAL_ADAPT_H
#define RMC_SIGNAL_ADAPT_H

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
//#include "signal_adapt_base.h"

#define RMC_RDL_DOOR                0x00000001
#define RMC_RDU_DOOR                0x00000002
#define RMC_RDL_TARGET              0x00000004
#define RMC_RDU_TARGET              0x00000008
#define RMC_RDL_PASSPORT            0x00000010
#define RMC_RDU_PASSPORT            0x00000020

#define RMC_DOOR_DRIVER             0x00000001
#define RMC_DOOR_PASSENGER          0x00000002
#define RMC_DOOR_DRIVER_REAR        0x00000004
#define RMC_DOOR_PASSENGER_REAR     0x00000008

#define RMC_TARGET_TRUNK            0x00000001
#define RMC_TARGET_HOOD             0x00000002
#define RMC_TARGET_TANKFLAP         0x00000004
#define RMC_TARGET_CHARGLID_FRONT   0x00000008
#define RMC_TARGET_CHARGLID_REAR    0x00000010
#define RMC_TARGET_PRIVATELOCK      0x00000020

namespace fsm
{
    class SignalAdaptBase1
    {
    public:
        SignalAdaptBase1(void * vdsObj);
    };
    class VDServiceRequest_Model
    {
    };
    class ServiceResult_Model
    {
    };
    class LockStatus_Model
    {
    };
    class PollutionStatus_Model
    {

    };
    class BasicVehicleStatus_Model
    {

    };
    class Position_Model
    {

    };

class RmcSignalAdapt: public SignalAdaptBase1
{

protected:
    ////////////////////////////////////////////////////////////
    // @brief :unpack rvdc RequestBody. 
    // @return     True if successfully set, false otherwise
    // @author     nieyujin, 26-Feb-2019
    ////////////////////////////////////////////////////////////
    //bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData) override;

    bool SetServiceResult(const RemoteCtrlBasicResult &appData, ServiceResult_Model &vdsData);
    bool SetDoorOpenStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetDoorLockStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetHoodStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetTrunkStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetCentralLockStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetWinStatusStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetWinWarningStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetWinPosStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetSunroofStsPos(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetCurtainStsPos(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetVentilateStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData);
    bool SetPollutionStatus(const RemoteCtrlBasicStatus &appData, PollutionStatus_Model &vdsData);
    bool SetSpeedStatus(const RemoteCtrlBasicStatus &appData, BasicVehicleStatus_Model &vdsData);
    bool SetPositionStatus(const RemoteCtrlPosition &appData, Position_Model &vdsData);
public:

    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return     
    // @author     nieyujin, 26-Feb-2019
    ////////////////////////////////////////////////////////////
    RmcSignalAdapt(void* vdsObj) : SignalAdaptBase1(vdsObj){};
    bool SetResultPayload(const RemoteCtrlBasicResult &result);
    bool SetDoorsStatus(const RemoteCtrlBasicStatus &response, const RemoteCtrlBasicResult &result);
    bool SetWinStatus(const RemoteCtrlBasicStatus &response, const RemoteCtrlBasicResult &result);
    bool SetPM25Status(const RemoteCtrlBasicStatus &response, const RemoteCtrlBasicResult &result);
    bool SetHALStatus(const RemoteCtrlBasicStatus &response, const RemoteCtrlBasicResult &result);
private:

};
}
#endif

