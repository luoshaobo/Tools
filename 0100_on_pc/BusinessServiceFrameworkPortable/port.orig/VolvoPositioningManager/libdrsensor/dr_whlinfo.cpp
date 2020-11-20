///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file dr_whlinfo.c
// This file handles the DR sensor data
// @project     GLY_TCAM
// @subsystem   libdrsensor
// @author      Jianhui Li
// @Init date   31-Dec-2018
///////////////////////////////////////////////////////////////////
#include <memory.h>
#include <mutex>

#include "dr_whlinfo.h"
#include "ipcb_IClient.hpp"
#include "dr_log.h"

std::mutex m_VehDataMutex;
static tpDR_vehData vehData_buffer;
static IpcbIGNSSService* ipcb_ignss_service = nullptr;

////////////////////////////////////////////////////////////
// @brief : WhlInfo Notification Cyclic Callback
// @param[in]  whlInfo 		wheel info
// @return     void
// @author     Jianhui Li, Dec 31, 2018
////////////////////////////////////////////////////////////
static void DR_whlInfoNotificationCyclicCb(DRWhlInfo* whlInfo)
{
    DR_LOG_INFO("DR_SENSOR: %s", __FUNCTION__);
     /* demo
    cb_data.yaw =  2;
    cb_data.wheel_data.RR_dir = DR_WHEEL_FORWARD;
    cb_data.wheel_data.RL_dir = DR_WHEEL_FORWARD;
    cb_data.wheel_data.RR_cnt = 80; //speed of 10m/s for ticks rev = 160, width of wheel = 637 cm
    cb_data.wheel_data.RL_cnt = 80; //speed of 10m/s for ticks rev = 160, width of wheel = 637 cm
    cb_data.sensor_data_status |= DR_YAW_AVAILABLE | DR_WHEEL_AVAILABLE | DR_WHEEL_VALID | DR_YAW_VALID;
    */

    std::lock_guard<std::mutex> lck(m_VehDataMutex);

    memset(&vehData_buffer, 0, sizeof(tpDR_vehData));

    // un-used
    // whlInfo.vehMtnSt.vehMtnStChks = 1;
    // whlInfo.vehMtnSt.vehMtnStCntr = 1;
    // whlInfo.whlCircum = 0.62;

    DRVehMtnSt2 vehMtnSt2 = whlInfo->vehMtnSt.vehMtnSt2;
    DRWhlRotToothCntr* cntr = &whlInfo->whlRotToothCntr;
      
    if((vehMtnSt2 == vehMtnSt2_RollgFwdVal1) || (vehMtnSt2 == vehMtnSt2_RollgFwdVal2))
    {
        vehData_buffer.wheel_data.FR_dir = DR_WHEEL_FORWARD;
        vehData_buffer.wheel_data.FL_dir = DR_WHEEL_FORWARD;
        vehData_buffer.wheel_data.RL_dir = DR_WHEEL_FORWARD;
        vehData_buffer.wheel_data.RR_dir = DR_WHEEL_FORWARD;
    }
    else if((vehMtnSt2 == vehMtnSt2_RollgBackwVal1) || (vehMtnSt2 == vehMtnSt2_RollgBackwVal2))
    {
        vehData_buffer.wheel_data.FR_dir = DR_WHEEL_BACKWARD;
        vehData_buffer.wheel_data.FL_dir = DR_WHEEL_BACKWARD;
        vehData_buffer.wheel_data.RL_dir = DR_WHEEL_BACKWARD;
        vehData_buffer.wheel_data.RR_dir = DR_WHEEL_BACKWARD;     
    }
    else /*if(vehMtnSt2 == vehMtnSt2_Ukwn ||
        vehMtnSt2 == vehMtnSt2_StandStillVal1 ||
        vehMtnSt2 == vehMtnSt2_StandStillVal1 ||
        vehMtnSt2 == vehMtnSt2_StandStillVal1 )*/
    {
        vehData_buffer.wheel_data.FR_dir = DR_WHEEL_UNKNOWN;
        vehData_buffer.wheel_data.FL_dir = DR_WHEEL_UNKNOWN;
        vehData_buffer.wheel_data.RL_dir = DR_WHEEL_UNKNOWN;
        vehData_buffer.wheel_data.RR_dir = DR_WHEEL_UNKNOWN;     
    }

    // default tick count is 1 when wheel cycling once.
    vehData_buffer.wheel_data.FL_cnt = cntr->whlRotToothCntrFrntLe;
    vehData_buffer.wheel_data.FR_cnt = cntr->whlRotToothCntrFrntRi;
    vehData_buffer.wheel_data.RL_cnt = cntr->whlRotToothCntrReLe;
    vehData_buffer.wheel_data.FR_cnt = cntr->whlRotToothCntrReRi;
    vehData_buffer.speed = 0;
    vehData_buffer.yaw = 0;
    vehData_buffer.sensor_data_status |= DR_WHEEL_AVAILABLE | DR_WHEEL_VALID /*| DR_YAW_AVAILABLE |DR_YAW_VALID*/;   
}

////////////////////////////////////////////////////////////
// @brief : Registers WhlInfo Notification Cyclic Callback
// @param[in]  None
// @return     True if callback has been registered successfully
// @author     Jianhui Li, Dec 31, 2018
////////////////////////////////////////////////////////////
bool DrWhlInfoInit()
{
    DR_LOG_INFO("DR_SENSOR: %s", __FUNCTION__);
    ipcb_ignss_service = &IpcbIClient::GetGNSSService();
    if(ipcb_ignss_service)
    {
        ipcb_ignss_service->RegisterWhlInfoNotificationCyclicCb(DR_whlInfoNotificationCyclicCb);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////
// @brief : Deregisters WhlInfo Notification Cyclic Callback
// @param[in]  None
// @return     void
// @author     Jianhui Li, Dec 31, 2018
////////////////////////////////////////////////////////////
void DrWhlInfoDeInit()
{
    DR_LOG_INFO("DR_SENSOR: %s", __FUNCTION__);
    if(ipcb_ignss_service)
    {
        ipcb_ignss_service->DeregisterWhlInfoNotificationCyclicCb();
    } 
    ipcb_ignss_service = nullptr;
}

////////////////////////////////////////////////////////////
// @brief : Get DR WhlInfo Interface
// @param[in]  None
// @return     vehicle data
// @author     Jianhui Li, Dec 31, 2018
////////////////////////////////////////////////////////////
tpDR_vehData* DrGetDrWhlInfo()
{
    DR_LOG_INFO("DR_SENSOR: %s", __FUNCTION__);

    std::lock_guard<std::mutex> lck(m_VehDataMutex);
    
    if(ipcb_ignss_service)
    {
        return &vehData_buffer;
    }
    else
    {
        memset(&vehData_buffer, 0, sizeof(tpDR_vehData));
    }
    return &vehData_buffer;
}

