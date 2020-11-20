///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file vdmsg_signal_types.cc

// @project		GLY_TCAM
// @subsystem	FSM
// @author		linhaixia
// @Init date	26-Sep-2018
///////////////////////////////////////////////////////////////////

#include <string>
#include "voc_framework/signals/vdmsg_signal_types.h"

namespace fsm
{

namespace VdsSignalType
{
    std::string toString(int type)
    {
        switch (type)
        {
            case kVDServiceMtp: return "MTP";
            case kVDServiceMtu: return "MTU";
            case kVDServiceMcu: return "MCU";
            case kVDServiceMpm: return "MPM";
            case kVDServiceMta: return "MTA";
            case kVDServiceMul: return "MUL";
            case kVDServiceMrs: return "MRS";
            case kVDServiceRin: return "RIN";
            case kVDServiceRdl: return "RDL";
            case kVDServiceRdu: return "RDU";
            case kVDServiceRtl: return "RTL";
            case kVDServiceRtu: return "RTU";
            case kVDServiceRes: return "RES";
            case kVDServiceRvi: return "RVI";
            case kVDServiceRcc: return "RCC";
            case kVDServiceRce: return "RCE";
            case kVDServiceRhl: return "RHL";
            case kVDServiceRsh: return "RSH";
            case kVDServiceRws: return "RWS";
            case kVDServiceRpc: return "RPC";
            case kVDServiceRpp: return "RPP";
            case kVDServiceRen: return "REN";
            case kVDServiceRec: return "REC";
            case kVDServiceCec: return "CEC";
            case kVDServiceCep: return "CEP";
            case kVDServiceCbc: return "CBC";
            case kVDServiceCic: return "CIC";
            case kVDServiceSvt: return "SVT";
            case kVDServiceStn: return "STN";
            case kVDServiceRvs: return "RVS";
            case kVDServiceTrs: return "TRS";
            case kVDServiceJou: return "JOU";
            case kVDServiceHec: return "HEC";
            case kVDServiceDia: return "DIA";
            case kVDServiceOta: return "OTA";
            case kVDServiceOsc: return "OSC";
            case kVDServiceVun: return "VUN";
            case kVDServiceWcr: return "WCR";
            case kVDServiceRsv: return "RSV";
            case kVDServiceMcp: return "MCP";
            case kVDServiceMsr: return "MSR";
            case kVDServiceFota:return "FOTA";
            case kVDServiceRms: return "RMS";
            case kVDServiceRvc: return "RVC";
            case KVDServiceTest: return "TEST";
            default:return "UNDEFINED";
        }
        
    }
}

}



































