///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    @file gly_vds_res_signal.h
//    geely res signal.

// @project        GLY_TCAM
// @subsystem    Application
// @author        uia93888
// @Init date    3-Sep-2018
///////////////////////////////////////////////////////////////////

#ifndef APP_REMOTE_START_COMMON_
#define APP_REMOTE_START_COMMON_

#include <string>
#include "vehicle_comm_interface.hpp"
#include "voc_framework/signals/signal.h"

namespace volvo_on_call
{
    typedef std::function<void(std::shared_ptr<fsm::Signal>)> SendSingalCallBack;

    static const int GLY_TCAM_REQUEST_TIMER = 15;
    static const int GLY_CT_TSP_SET_RESPONSE_TIME_DEFAULT = 120; // unit: s, range: 0~600, dafault value: 120s
    static const int GLY_CT_TSP_SET_RESPONSE_TIME_MIN = 0;
    static const int GLY_CT_TSP_SET_RESPONSE_TIME_MAX = 600;

    static const int GLY_CLIMA_REQUEST_TIME_DEFAULT = 10000; //uint: ms, range: 500~30000, default value: 10000ms
    static const int GLY_CLIMA_REQUEST_TIME_MIN = 500;
    static const int GLY_CLIMA_REQUEST_TIME_MAX = 30000;

    static const int GLY_CLIMA_RPC_VFC_TIMEOUT_DEFAULT = 3;
    static const int GLY_CLIMA_RPC_VFC_TIMEOUT_MIX = 0;
    static const int GLY_CLIMA_RPC_VFC_TIMEOUT_MAX = 127;

    static const float GLY_CLIMA_TEMPERATURE_DEFAULT = 15.5;
    static const float GLY_CLIMA_TEMPERATURE_MIN = 0;
    static const float GLY_CLIMA_TEMPERATURE_MAX = 26;

    static const int GLY_TCAM_REMOTE_START_CLIAMTE_VFC_ACTIVE_TIME_DEFAULT = 15;

    static const uint8_t GLY_TCAM_REMOTE_START_ACTIVE_VFC_ID = 18;
    static const uint8_t GLY_TCAM_REMOTE_START_ACTIVE_VFC_SUCCESS = 0;   // operate VFC success

    typedef enum{
        kIncorrectState        = 1,
        kInvalidData           = 2,
        kInvalidFormat         = 3,
        kPowderModeNotCorrect  = 4,
        kConditionsNotCorrect  = 5,
        kExecutionFailure      = 6,
        kPermissionDenied      = 7,
        kTimeout               = 8,
        kBackupBatteryUsage    = 9
    } ASN_ErrorCode;

    typedef enum{
        RMT_NoService,
        RMT_Climatization,
        RMT_SEATVENTI,
        RMT_SEATHEAT,
        RMT_STEERWHEELHEAT
    } ServiceType;

    typedef enum{
        CMD_NoReq,
        CMD_Stop,
        CMD_Start,
        CMD_TimeoutStop // <*Rung timer is timeout, using it
    } OperateCommands;

    typedef enum{
        ResType_StrtStop,
        ResType_AutoStop,
        ResType_Err
    }RMT_ClimatizationResType_e;

    typedef struct {
        ServiceType m_serviceType;
        OperateCommands m_operateCmd;

        int m_telemClimaTemperature;
        int m_duration_time; //unit: seconds
        vc::TelmClimaTSetHmiCmptmtTSpSpcl m_level;

        int32_t m_RceHeat;
        int32_t m_RceVenti;
    } RMT_ClimatizationRequest_t;

    typedef struct {
        ServiceType m_serviceType;
        RMT_ClimatizationResType_e m_resType;
        OperateCommands m_operateCmd;
        bool m_operateSuccess;
        vc::ErsStrtApplSts m_engine_state;
        long m_speed;
        long m_chargeHvSts;

        vc::RemStrtClimaActv m_active;

        int m_errorCode;
        std::string m_message;
    } RMT_ClimatizationResponse_t;
}
#endif //APP_REMOTE_START_COMMON_