///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file gly_remote_config_signal.h
//	geely remote_config signal.

// @project		GLY_TCAM
// @subsystem	Application
// @author		linhaixia/zhouyou
// @Init date	11-Jan-2019
///////////////////////////////////////////////////////////////////

#include "voc_framework/signal_adapt/mcu_signal_adapt.h"
#include "signals/geely/gly_vds_mcu_signal.h"
#include "voc_framework/signals/signal.h"
#include "dlt/dlt.h"
#include <list>
#include <array>
#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <cstring>


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{


std::shared_ptr<fsm::Signal> GlyVdsMcuSignal::CreateGlyVdsMcuSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    GlyVdsMcuSignal* signal = new GlyVdsMcuSignal(transaction_id, vdServiceRequest);
    
    return std::shared_ptr<GlyVdsMcuSignal>(signal);
}

GlyVdsMcuSignal::GlyVdsMcuSignal (fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest):
                   fsm::VdmSignal(transaction_id, fsm::kVDServiceMcu)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsMcuSignal::%s\n", __FUNCTION__);
    
    bool result = UnpackPayload(vdServiceRequest);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,mcu request payload success.\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,mcu request payload fail.\n", __FUNCTION__);
    }
}

GlyVdsMcuSignal::~GlyVdsMcuSignal ()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsMcuSignal::%s\n", __FUNCTION__);
}


bool GlyVdsMcuSignal::UnpackPayload(void * vdServiceRequest)
{
    McuSignalAdapt adapt(vdServiceRequest);
    std::shared_ptr<AppDataBase> appData = adapt.UnpackService();
    m_mcuRequest = std::dynamic_pointer_cast<McuRequest_t>(appData);

    if( m_mcuRequest == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s(): m_mcuRequest == nullptr", __FUNCTION__);
        return false;
    }

   return true;
}

bool GlyVdsMcuSignal::SetResultPayload(McuServiceResult_t response)
{
     DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s() entered \n",  __FUNCTION__);
    if(m_mcuRequest == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() m_mcuRequest is null \n",  __FUNCTION__);
        return false;
    }

    response.header = m_mcuRequest->header;
    
    m_mcuServiceResult = std::make_shared<McuServiceResult_t>();
    if (m_mcuServiceResult == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() m_mcuServiceResult is null \n", __FUNCTION__);
        return false;
    }
    
    *m_mcuServiceResult = response;
    
    return true;
    
}


int GlyVdsMcuSignal::PackGeelyAsn(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s() entered \n",  __FUNCTION__);
    if (vdServiceRequest == NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() vdServiceRequest is null \n",  __FUNCTION__);
        return -1;
    }
    
    if(m_mcuServiceResult != nullptr)
    {
        McuSignalAdapt adapt((void *)vdServiceRequest);
        bool ret = adapt.SetResultPayload(*m_mcuServiceResult);
        return (ret == true? 0 : -1);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() error in else \n",  __FUNCTION__);
        return -1;
    }
    
    return -1;
}

}


