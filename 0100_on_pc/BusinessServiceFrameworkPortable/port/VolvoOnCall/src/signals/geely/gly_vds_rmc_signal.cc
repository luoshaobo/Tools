///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file gly_vds_rmc_signal.cc
//	geely rmc signal.

// @project		GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	27-Feb-2019
///////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "timestamp.h"
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rmc_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
//#include "voc_framework/signal_adapt/rmc_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_voc);

#ifdef DLT_LOG_STRINGF
#undef DLT_LOG_STRINGF
#define DLT_LOG_STRINGF(...)
#endif // #ifdef DLT_LOG_STRINGF

namespace volvo_on_call
{
/************************************************************/
// @brief :create rdl signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Nie Yujin
/************************************************************/  
std::shared_ptr<fsm::Signal> GlyVdsRmcSignal::CreateGlyVdsRDLSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    GlyVdsRmcSignal* signal = new GlyVdsRmcSignal(transaction_id, vdServiceRequest, fsm::kVDServiceRdl);

    return std::shared_ptr<GlyVdsRmcSignal>(signal);
}

/************************************************************/
// @brief :create rdl signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Nie Yujin
/************************************************************/  
std::shared_ptr<fsm::Signal> GlyVdsRmcSignal::CreateGlyVdsRDUSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    GlyVdsRmcSignal* signal = new GlyVdsRmcSignal(transaction_id, vdServiceRequest, fsm::kVDServiceRdu);

    return std::shared_ptr<GlyVdsRmcSignal>(signal);
}
/************************************************************/
// @brief :create rdl signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Nie Yujin
/************************************************************/  
std::shared_ptr<fsm::Signal> GlyVdsRmcSignal::CreateGlyVdsRWSSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    GlyVdsRmcSignal* signal = new GlyVdsRmcSignal(transaction_id, vdServiceRequest, fsm::kVDServiceRws);

    return std::shared_ptr<GlyVdsRmcSignal>(signal);
}
/************************************************************/
// @brief :create rdl signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Nie Yujin
/************************************************************/  
std::shared_ptr<fsm::Signal> GlyVdsRmcSignal::CreateGlyVdsRPPSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    GlyVdsRmcSignal* signal = new GlyVdsRmcSignal(transaction_id, vdServiceRequest, fsm::kVDServiceRpp);

    return std::shared_ptr<GlyVdsRmcSignal>(signal);
}
/************************************************************/
// @brief :create rdl signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Nie Yujin
/************************************************************/  
std::shared_ptr<fsm::Signal> GlyVdsRmcSignal::CreateGlyVdsRHLSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    GlyVdsRmcSignal* signal = new GlyVdsRmcSignal(transaction_id, vdServiceRequest, fsm::kVDServiceRhl);

    return std::shared_ptr<GlyVdsRmcSignal>(signal);
}
/************************************************************/
// @brief :Constructs function.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     nieyujin, 27-Feb-2019
/************************************************************/
GlyVdsRmcSignal::GlyVdsRmcSignal (fsm::VdServiceTransactionId& transaction_id, void *vdServiceRequest, const SignalType signal_type)
    :fsm::VdmSignal(transaction_id, signal_type)
    ,m_type(RmcResponseTypeUnknown)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s", __FUNCTION__);
    bool result = UnpackPayload(vdServiceRequest);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,remote control request payload success.", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,remote control request payload fail.", __FUNCTION__);
    }

    m_rmcRequest = std::make_shared<fsm::RemoteCtrlBasicRequest>();
    m_rmcRequest->serviceId = signal_type;
}

/************************************************************/
// @brief :destruct function.
// @param[in]  none
// @return
// @author     nieyujin, 27-Feb-2019
/************************************************************/
GlyVdsRmcSignal::~GlyVdsRmcSignal ()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s", __FUNCTION__);

}

/************************************************************/
// @brief :pack RemoteCtrlBasicResult into ASN1C structure. upload body msg.
// @param[in]  response, RemoteCtrlBasicResult structure .
// @return     True if successfully set, false otherwise
// @author     nieyujin, 27-Feb-2019
/************************************************************/
bool GlyVdsRmcSignal::SetResultPayload(fsm::RemoteCtrlBasicResult payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s", __FUNCTION__);

    payload.header = m_rmcRequest->header;
    payload.serviceId = m_rmcRequest->serviceId;
    m_rmcResult = std::make_shared<fsm::RemoteCtrlBasicResult>();
    if (m_rmcResult == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() m_rmcResult is null \n", 
            __FUNCTION__);
        return false;
    }
    *m_rmcResult = payload;
    return true;
}

/************************************************************/
// @brief :pack RemoteCtrlBasicStatus_t into ASN1C structure. upload body msg.
// @param[in]  response, RemoteCtrlBasicStatus_t structure .
// @return     True if successfully set, false otherwise
// @author     Nie Yujin
/************************************************************/ 
bool GlyVdsRmcSignal::SetRmcStatusPayload(RmcResponseType type, fsm::RemoteCtrlBasicStatus payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s", __FUNCTION__);
    m_type = type;
    payload.header = m_rmcRequest->header;
    payload.serviceId = m_rmcRequest->serviceId;
    m_rmcStatus = std::make_shared<fsm::RemoteCtrlBasicStatus>();
    if (m_rmcStatus == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() m_rmcStatus is null \n", 
            __FUNCTION__);
        return false;
    }
    *m_rmcStatus = payload;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s vehSpdIndcd = %d", __FUNCTION__, payload.vehSpdIndcd);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s vehSpdIndcdQly = %d", __FUNCTION__, payload.vehSpdIndcdQly);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s doorOpenStatusDriver = %d", __FUNCTION__, payload.doorOpenStatusDriver);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s doorOpenStatusPassenger = %d", __FUNCTION__, payload.doorOpenStatusPassenger);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s doorOpenStatusDriverRear = %d", __FUNCTION__, payload.doorOpenStatusDriverRear);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s doorOpenStatusPassengerRear = %d", __FUNCTION__, payload.doorOpenStatusPassengerRear);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s doorLockStatusDriver = %d", __FUNCTION__, payload.doorLockStatusDriver);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s doorLockStatusPassenger = %d", __FUNCTION__, payload.doorLockStatusPassenger);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s doorLockStatusDriverRear = %d", __FUNCTION__, payload.doorLockStatusDriverRear);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s doorLockStatusPassengerRear = %d", __FUNCTION__, payload.doorLockStatusPassengerRear);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s trunkOpenStatus = %d", __FUNCTION__, payload.trunkOpenStatus);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s trunkLockStatus = %d", __FUNCTION__, payload.trunkLockStatus);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s lockgCenStsForUsrFb = %d", __FUNCTION__, payload.lockgCenStsForUsrFb);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s hoodSts = %d", __FUNCTION__, payload.hoodSts);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s winStatusDriver = %d", __FUNCTION__, payload.winStatusDriver);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s winStatusPassenger = %d", __FUNCTION__, payload.winStatusPassenger);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s winStatusDriverRear = %d", __FUNCTION__, payload.winStatusDriverRear);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s winStatusPassengerRear = %d", __FUNCTION__, payload.winStatusPassengerRear);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s sunroofOpenStatus = %d", __FUNCTION__, payload.sunroofOpenStatus);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s sunroofPos = %d", __FUNCTION__, payload.sunroofPos);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s curtainOpenStatus = %d", __FUNCTION__, payload.curtainOpenStatus);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s curtainPos = %d", __FUNCTION__, payload.curtainPos);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s winPosDriver = %d", __FUNCTION__, payload.winPosDriver);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s winPosPassenger = %d", __FUNCTION__, payload.winPosPassenger);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s winPosDriverRear = %d", __FUNCTION__, payload.winPosDriverRear);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s winPosPassengerRear = %d", __FUNCTION__, payload.winPosPassengerRear);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s interiorPM25 = %d", __FUNCTION__, payload.interiorPM25);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s exteriorPM25 = %d", __FUNCTION__, payload.exteriorPM25);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s interiorPM25Level = %d", __FUNCTION__, payload.interiorPM25Level);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s exteriorPM25Level = %d", __FUNCTION__, payload.exteriorPM25Level);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s completed.\n", __FUNCTION__);
    return true;

}
/************************************************************/
// @brief :pack RVDC_Data into ASN1C structure. upload body msg.
// @param[in]  response, RVDC_Data structure .
// @return     0 if successfully set, -1 otherwise
// @author     nieyujin, 27-Feb-2019
/************************************************************/
int GlyVdsRmcSignal::PackGeelyAsn(void *vdServiceRequest)
{
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s", __FUNCTION__);
    //if (vdServiceRequest == NULL)
    //{
    //    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() vdServiceRequest is null", __FUNCTION__);
    //    return -1;
    //}

    //switch (m_type)
    //{
    //case RmcResponseTypeDoor:
    //{
    //    if(m_rmcStatus != nullptr)
    //    {
    //        fsm::RmcSignalAdapt adapt((void *)vdServiceRequest);
    //        bool ret = adapt.SetDoorsStatus(*m_rmcStatus, *m_rmcResult);
    //        return (ret == true? 0 : -1);
    //    }
    //    break;
    //}
    //case RmcResponseTypeWin:
    //{
    //    if(m_rmcStatus != nullptr)
    //    {
    //        fsm::RmcSignalAdapt adapt((void *)vdServiceRequest);
    //        bool ret = adapt.SetWinStatus(*m_rmcStatus, *m_rmcResult);
    //        return (ret == true? 0 : -1);
    //    }
    //    break;
    //}
    //case RmcResponseTypeRpp:
    //{
    //    if(m_rmcStatus != nullptr)
    //    {
    //        fsm::RmcSignalAdapt adapt((void *)vdServiceRequest);
    //        bool ret = adapt.SetPM25Status(*m_rmcStatus, *m_rmcResult);
    //        return (ret == true? 0 : -1);
    //    }
    //    break;
    //}
    //case RmcResponseTypeRhl:
    //{
    //    if(m_rmcResult != nullptr)
    //    {
    //        fsm::RmcSignalAdapt adapt((void *)vdServiceRequest);
    //        bool ret = adapt.SetHALStatus(*m_rmcStatus, *m_rmcResult);
    //        return (ret == true? 0 : -1);
    //    }
    //    break;
    //}
    //default:
    //{
    //    if(m_rmcResult != nullptr)
    //    {
    //        fsm::RmcSignalAdapt adapt((void *)vdServiceRequest);
    //        bool ret = adapt.SetResultPayload(*m_rmcResult);
    //        return (ret == true? 0 : -1);
    //    }
    //    break;
    //}
    //}
    
    return -1;
}

/************************************************************/
// @brief :unpack ASN1C structure.
// @param[in]  ASN1C structur
// @return     True if successfully set, false otherwise
// @author     nieyujin, 27-Feb-2019
/************************************************************/
bool GlyVdsRmcSignal::UnpackPayload(void * vdServiceRequest)
{
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRmcSignal::%s", __FUNCTION__);
    //
    //fsm::RmcSignalAdapt adapt(vdServiceRequest);
    //
    //std::shared_ptr<AppDataBase> appData = adapt.UnpackService();
    //m_rmcRequest = std::dynamic_pointer_cast<fsm::RemoteCtrlBasicRequest>(appData);

    //if( m_rmcRequest == nullptr)
    //{
    //    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): m_rmcRequest == nullptr", __FUNCTION__);
    //    return false;
    //}

    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->serviceId:%d", m_rmcRequest->serviceId);
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->serviceCommand:%d", m_rmcRequest->serviceCommand);
    //
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->timeStart:%d", m_rmcRequest->timeStart);
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->timeWindow:%d", m_rmcRequest->timeWindow);
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->doorValue:%d", m_rmcRequest->doorValue);
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->targetValue:%d", m_rmcRequest->targetValue);
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->value:%d", m_rmcRequest->value);
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->pos:%d", m_rmcRequest->pos);

    return true;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
