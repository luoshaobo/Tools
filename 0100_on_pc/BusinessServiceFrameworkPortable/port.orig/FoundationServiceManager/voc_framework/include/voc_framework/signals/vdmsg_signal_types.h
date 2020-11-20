///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file vdmsg_signal_types.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		linhaixia
// @Init date	26-Sep-2018
///////////////////////////////////////////////////////////////////

#ifndef VDMSG_SIGNAL_TYPES_H_
#define VDMSG_SIGNAL_TYPES_H_

#include <string>


namespace fsm
{

enum VDSSignalTypes
{
    kVDServiceMtp	= 1,
    kVDServiceMtu	= 2,
    kVDServiceMcu	= 4,
    kVDServiceMpm	= 5,
    kVDServiceMta	= 6,
    kVDServiceMul	= 7,
    kVDServiceMrs	= 8,
    kVDServiceRin	= 11,
    kVDServiceRdl	= 13,
    kVDServiceRdu	= 14,
    kVDServiceRtl	= 15,
    kVDServiceRtu	= 16,
    kVDServiceRes	= 17,
    kVDServiceRvi	= 18,
    kVDServiceRcc	= 19,
    kVDServiceRce	= 20,
    kVDServiceRhl	= 21,
    kVDServiceRsh	= 22,
    kVDServiceRws	= 23,
    kVDServiceRpc	= 24,
    kVDServiceRpp	= 25,
    kVDServiceRen	= 26,
    kVDServiceRec	= 27,
    kVDServiceCec	= 31,
    kVDServiceCep	= 32,
    kVDServiceCbc	= 33,
    kVDServiceCic	= 34,
    kVDServiceSvt	= 41,
    kVDServiceStn	= 44,
    kVDServiceRvs	= 51,
    kVDServiceTrs	= 52,
    kVDServiceJou	= 61,
    kVDServiceHec	= 62,
    kVDServiceDia	= 63,
    kVDServiceOta	= 67,
    kVDServiceOsc	= 68,
    kVDServiceVun	= 69,
    kVDServiceWcr	= 71,
    kVDServiceRsv	= 72,
    kVDServiceMcp	= 101,
    kVDServiceMsr	= 102,
    kVDServiceFota	= 200,
    kVDServiceRvc   = 250,
    kVDServiceRms	= 500,
    KVDServiceTest = 1000
};

namespace VdsSignalType
{
    ////////////////////////////////////////////////////////////
    // @brief :Returns the string literal for the passed enumerations.
    // @param[in]  type VDSSignalTypes enumerations
    // @return     the string literal for the passed enumerations.
    // @author     linhaixia, 26-Sep-2018
    ////////////////////////////////////////////////////////////
    std::string toString(int type);
}

}

#endif //VOC_SIGNAL_TYPES_SIGNAL_H_


































