///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file gly_vds_svt_signal.cc
//	geely svt signal.

// @project		GLY_TCAM
// @subsystem	Application
// @author		linhaixia
// @Init date	3-Sep-2018
///////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <cstring>
#include "timestamp.h"
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_svt_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/svt_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

std::shared_ptr<fsm::Signal> GlyVdsSvtSignal::CreateGlyVdsSvtSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    GlyVdsSvtSignal* signal = new GlyVdsSvtSignal(transaction_id, vdServiceRequest);
    
    return std::shared_ptr<GlyVdsSvtSignal>(signal);
}

GlyVdsSvtSignal::GlyVdsSvtSignal (fsm::VdServiceTransactionId& transaction_id, 
                                    void *vdServiceRequest):
                   fsm::VdmSignal(transaction_id, fsm::kVDServiceSvt)
{
    bool result = UnpackPayload(vdServiceRequest);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,svt request payload success.\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,svt request payload fail.\n", __FUNCTION__);
    }
}


GlyVdsSvtSignal::~GlyVdsSvtSignal ()
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsSvtSignal::%s\n", __FUNCTION__);

#if 0    
    if (m_vdsService)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): free m_vdsService", __FUNCTION__);
        delete m_vdsService;
    }
    
    if (m_vdsResponse)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): free m_vdsResponse", __FUNCTION__);
        delete m_vdsResponse;
    }
#endif

}

bool GlyVdsSvtSignal::UnpackPayload(void * vdServiceRequest)
{
    SvtSignalAdapt adapt(vdServiceRequest);
    
    std::shared_ptr<AppDataBase> appData = adapt.UnpackService();
    m_svtRequest = std::dynamic_pointer_cast<SvtRequest_t>(appData);

    if( m_svtRequest == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): m_svtRequest == nullptr", __FUNCTION__);
        return false;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->serviceId:%d. \n", m_svtRequest->serviceId);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->serviceCommand:%d. \n", m_svtRequest->serviceCommand);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->scheduledTimeSeconds:%d. \n", m_svtRequest->scheduledTimeSeconds);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->scheduledTimeMilliseconds:%d. \n", m_svtRequest->scheduledTimeMilliseconds);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->endTimeSeconds:%d. \n", m_svtRequest->endTimeSeconds);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->endTimeMilliseconds:%d. \n", m_svtRequest->endTimeMilliseconds);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->interval:%d. \n", m_svtRequest->interval);

    return true;
}


bool GlyVdsSvtSignal::SetResultPayload(SvtServiceResult_t response)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s() entered \n", 
        __FUNCTION__);
    if(m_svtRequest == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() m_svtRequest is null \n", 
            __FUNCTION__);
        return false;
    }

    response.header = m_svtRequest->header;
    
    m_SvtServiceResult = std::make_shared<SvtServiceResult_t>();
    if (m_SvtServiceResult == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() m_SvtServiceResult is null \n", 
            __FUNCTION__);
        return false;
    }
    //std::memset(m_SvtServiceResult.get(), 0, sizeof(SvtServiceResult_t));
    *m_SvtServiceResult = response;
    return true;
}

bool GlyVdsSvtSignal::SetTrackPointPayload(SvtTrackPoint_t response)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s() entered \n", 
        __FUNCTION__);
    if(m_svtRequest == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() m_svtRequest is null \n", 
            __FUNCTION__);
        return false;
    }
    response.header = m_svtRequest->header;

    m_SvtTrackPoint = std::make_shared<SvtTrackPoint_t>();
    if (m_SvtTrackPoint == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() m_SvtTrackPoint is null \n", 
            __FUNCTION__);
        return false;
    }
    //std::memset(m_SvtTrackPoint.get(), 0, sizeof(m_SvtTrackPoint));
    *m_SvtTrackPoint = response;
    return true;
}

int GlyVdsSvtSignal::PackGeelyAsn(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s() entered \n", 
                    __FUNCTION__);
    if (vdServiceRequest == NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() vdServiceRequest is null \n", 
            __FUNCTION__);
        return -1;
    }
    if(m_SvtServiceResult != nullptr)
    {
        SvtSignalAdapt adapt((void *)vdServiceRequest);
        bool ret = adapt.SetResultPayload(*m_SvtServiceResult);
        return (ret == true? 0 : -1);
    }
    else if (m_SvtTrackPoint != nullptr)
    {
        SvtSignalAdapt adapt((void *)vdServiceRequest);
        bool ret = adapt.SetTrackPointPayload(*m_SvtTrackPoint);
        return (ret == true? 0 : -1);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() error in else \n", 
            __FUNCTION__);
        return -1;
    }
    
    return -1;
}


} // namespace volvo_on_call

/** \}    end of addtogroup */
