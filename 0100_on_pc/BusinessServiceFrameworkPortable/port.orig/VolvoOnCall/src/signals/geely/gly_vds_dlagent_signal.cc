///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  file gly_vds_dlagent_signal.cc
//  geely download agent signal.

// @project     GLY_TCAM
// @subsystem   application
// @author      ShenJiaojiao
// @Init date   27-Feb-2019
///////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "timestamp.h"
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_dlagent_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/dlagent_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

#define RETURN_TRUE (0)
#define RETURN_FALSE (-1)

std::shared_ptr<fsm::Signal> GlyVdsDLAgentSignal::CreateGlyVdsDLAgentSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    GlyVdsDLAgentSignal* signal = new GlyVdsDLAgentSignal(transaction_id, vdServiceRequest);

    return std::shared_ptr<GlyVdsDLAgentSignal>(signal);
}

GlyVdsDLAgentSignal::GlyVdsDLAgentSignal (fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest):
                   fsm::VdmSignal(transaction_id, fsm::kVDServiceFota)
{
    m_fotaData = nullptr;
    bool result = UnpackPayload(vdServiceRequest);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, request payload success.\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, request payload fail.\n", __FUNCTION__);
    }
    m_fotaAssignmentNotification = nullptr;
    m_availableAssignmentRequestData = nullptr;
    m_installationSummary = nullptr;
    m_installationSummary = nullptr;
}


GlyVdsDLAgentSignal::~GlyVdsDLAgentSignal ()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: GlyVdsDLAgentSignal::%s", __FUNCTION__);
}

bool GlyVdsDLAgentSignal::UnpackPayload(void * vdServiceRequest)
{
    DLAgentSignalAdapt adapt(vdServiceRequest);

    std::shared_ptr<AppDataBase> appData = adapt.UnpackService();
    m_fotaData= std::dynamic_pointer_cast<fsm::FOTA_Data>(appData);

    if( m_fotaData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): m_fotaData = nullptr", __FUNCTION__);
        return false;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->serviceCommand:%d. \n", m_fotaData->serviceCommand);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->dataType:%d. \n", m_fotaData->dataType);

    return true;
}


bool GlyVdsDLAgentSignal::SetAssignmentNotificationPayload(fsm::FotaAssignmentNotification_Data msg)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: GlyVdsDLAgentSignal::%s", __FUNCTION__);

    m_fotaAssignmentNotification = std::make_shared<fsm::FotaAssignmentNotification_Data>();
    if (m_fotaAssignmentNotification == nullptr)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: m_SvtFotaAssignmentNotification is null.");
        return false;
    }
    *m_fotaAssignmentNotification = msg;

    return true;
}

bool GlyVdsDLAgentSignal::SetAvailableAssignmentRequestPayload(fsm::AvailableAssignmentRequest_Data &msg)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: %s ", __FUNCTION__);

    m_availableAssignmentRequestData = std::make_shared<fsm::AvailableAssignmentRequest_Data>();
    if (m_availableAssignmentRequestData == nullptr){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA: %s std::make_shared<AvailableAssignmentRequest_Data> failed.", __FUNCTION__);
        return false;
    }
    *m_availableAssignmentRequestData = msg;
    m_availableAssignmentRequestData->header.isInitiator = true;
    return true;
}

bool GlyVdsDLAgentSignal::SetInstallationSummaryNotificationPayload(const fsm::InstallationSummary_Data& msg)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: GlyVdsDLAgentSignal::%s", __FUNCTION__);

    m_installationSummary = std::make_shared<fsm::InstallationSummary_Data>();
    if (m_installationSummary == nullptr)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: m_installationSummary is null.");
        return false;
    }
    *m_installationSummary = msg;
    return true;
}

bool GlyVdsDLAgentSignal::SetExceptionReportNotificationPayload(const fsm::EcuExceptionReport_Data& msg)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: GlyVdsDLAgentSignal::%s", __FUNCTION__);

    m_exceptionReport = std::make_shared<fsm::EcuExceptionReport_Data>();
    if (m_exceptionReport == nullptr)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: m_exceptionReport is null.");
        return false;
    }
    *m_exceptionReport = msg;
    return true;
}

int GlyVdsDLAgentSignal::PackGeelyAsn(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: GlyVdsDLAgentSignal::%s", __FUNCTION__);

    int result = RETURN_FALSE;

    if (vdServiceRequest == NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA: %s vdServiceRequest is null.", __FUNCTION__);
        return result;
    }

    if(m_fotaAssignmentNotification != nullptr)
    {
        DLAgentSignalAdapt adapt((void *)vdServiceRequest);
        bool ret = adapt.SetAssignmentNotificationPayload(*m_fotaAssignmentNotification);
        result = (ret == true ? RETURN_TRUE : RETURN_FALSE);
    }
    else if (m_availableAssignmentRequestData != nullptr)
    {
        DLAgentSignalAdapt adapt((void *)vdServiceRequest);
        bool ret = adapt.SetAvailableAssignmentRequestPayload(*m_availableAssignmentRequestData);
        return (ret == true? RETURN_TRUE : RETURN_FALSE);
    }
    else if(m_installationSummary != nullptr)
    {
        DLAgentSignalAdapt adapt((void *)vdServiceRequest);
        bool ret = adapt.SetInstallationSummaryPayload(*m_installationSummary);
        result = (ret == true ? RETURN_TRUE : RETURN_FALSE);
    }
    else if(m_exceptionReport != nullptr)
    {
        DLAgentSignalAdapt adapt((void *)vdServiceRequest);
        bool ret = adapt.SetExceptionReportResultPayload(*m_exceptionReport);
        result = (ret == true ? RETURN_TRUE : RETURN_FALSE);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA: %s error in else.", __FUNCTION__);
    }

    return result;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
