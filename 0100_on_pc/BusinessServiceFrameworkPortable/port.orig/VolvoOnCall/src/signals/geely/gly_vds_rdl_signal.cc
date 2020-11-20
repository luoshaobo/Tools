/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     gly_vds_rdl_signal.cc
 *  \brief    gly_vds_rdl_signal signal
 *  \author   Nie Yujin
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include <stdio.h>
#include "dlt/dlt.h"
#include "timestamp.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rdl_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

    /************************************************************/
    // @brief :create rdl signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Nie Yujin
    /************************************************************/  
    std::shared_ptr<fsm::Signal> GlyVdsRemoteCtrlBasicSignal::CreateGlyVdsRDLSignal(fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

        GlyVdsRemoteCtrlBasicSignal* signal = new GlyVdsRemoteCtrlBasicSignal(transaction_id, vdsService, fsm::kVDServiceRdl);

        return std::shared_ptr<GlyVdsRemoteCtrlBasicSignal>(signal);
    }

    /************************************************************/
    // @brief :create rdu signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    std::shared_ptr<fsm::Signal> GlyVdsRemoteCtrlBasicSignal::CreateGlyVdsRDUSignal(fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

        GlyVdsRemoteCtrlBasicSignal* signal = new GlyVdsRemoteCtrlBasicSignal(transaction_id, vdsService, fsm::kVDServiceRdu);

        return std::shared_ptr<GlyVdsRemoteCtrlBasicSignal>(signal);
    }

    /************************************************************/
    // @brief :create rtl signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    std::shared_ptr<fsm::Signal> GlyVdsRemoteCtrlBasicSignal::CreateGlyVdsRTLSignal(fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

        GlyVdsRemoteCtrlBasicSignal* signal = new GlyVdsRemoteCtrlBasicSignal(transaction_id, vdsService, fsm::kVDServiceRtl);

        return std::shared_ptr<GlyVdsRemoteCtrlBasicSignal>(signal);
    }

    /************************************************************/
    // @brief :create rtu signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    std::shared_ptr<fsm::Signal> GlyVdsRemoteCtrlBasicSignal::CreateGlyVdsRTUSignal(fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

        GlyVdsRemoteCtrlBasicSignal* signal = new GlyVdsRemoteCtrlBasicSignal(transaction_id, vdsService, fsm::kVDServiceRtu);

        return std::shared_ptr<GlyVdsRemoteCtrlBasicSignal>(signal);
    }

    /************************************************************/
    // @brief :create rws signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    std::shared_ptr<fsm::Signal> GlyVdsRemoteCtrlBasicSignal::CreateGlyVdsRWSSignal(fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

        GlyVdsRemoteCtrlBasicSignal* signal = new GlyVdsRemoteCtrlBasicSignal(transaction_id, vdsService, fsm::kVDServiceRws);

        return std::shared_ptr<GlyVdsRemoteCtrlBasicSignal>(signal);
    }

    /************************************************************/
    // @brief :create rpp signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    std::shared_ptr<fsm::Signal> GlyVdsRemoteCtrlBasicSignal::CreateGlyVdsRPPSignal(fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

        GlyVdsRemoteCtrlBasicSignal* signal = new GlyVdsRemoteCtrlBasicSignal(transaction_id, vdsService, fsm::kVDServiceRpp);

        return std::shared_ptr<GlyVdsRemoteCtrlBasicSignal>(signal);
    }

    /************************************************************/
    // @brief :create rhl signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    std::shared_ptr<fsm::Signal> GlyVdsRemoteCtrlBasicSignal::CreateGlyVdsRHLSignal(fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

        GlyVdsRemoteCtrlBasicSignal* signal = new GlyVdsRemoteCtrlBasicSignal(transaction_id, vdsService, fsm::kVDServiceRhl);

        return std::shared_ptr<GlyVdsRemoteCtrlBasicSignal>(signal);
    }
    
    /************************************************************/
    // @brief :Constructs function.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    GlyVdsRemoteCtrlBasicSignal::GlyVdsRemoteCtrlBasicSignal (fsm::VdServiceTransactionId& transaction_id,VDServiceRequest* vdsService, const SignalType signal_type):
    fsm::VdmSignal(transaction_id, signal_type),
        m_vdsService(vdsService),
        m_vdsResponse(nullptr)
    {
        bool result = UnpackPayload();
        if(result)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,get remote control request payload success.\n", __FUNCTION__);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,get remote control request payload fail.\n", __FUNCTION__);
        }
    }

    /************************************************************/
    // @brief :destruct function.
    // @param[in]  none
    // @return
    // @author     Nie Yujin
    /************************************************************/
    GlyVdsRemoteCtrlBasicSignal::~GlyVdsRemoteCtrlBasicSignal ()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s\n", __FUNCTION__);
        
        if (m_vdsService)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): free m_vdsService", __FUNCTION__);
            delete m_vdsService;
            m_vdsService = nullptr;
        }
        
        if (m_vdsResponse)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): free m_vdsResponse", __FUNCTION__);
            delete m_vdsResponse;
            m_vdsResponse = nullptr;
        }
    }

    /************************************************************/
    // @brief :unpack ASN1C structure.Currently used to receive ack msg.
    // @return     True if successfully set, false otherwise
    // @author     Nie Yujin
    /************************************************************/
    bool GlyVdsRemoteCtrlBasicSignal::UnpackPayload()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s", __FUNCTION__);
        
        m_rcbRequest = std::make_shared<RemoteCtrlBasicRequest_t>();

        if(m_rcbRequest == nullptr)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "GlyVdsRemoteCtrlBasicSignal: m_rcbRequest is null");
            return false;
        }

        if(m_vdsService == nullptr)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "GlyVdsRemoteCtrlBasicSignal: m_vdsService is null");
            return false;
        }

        if(m_vdsService->body == nullptr)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "GlyVdsRemoteCtrlBasicSignal: m_vdsService->body is null");
            return false;
        }

        if(m_vdsService->body->serviceData == nullptr)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "GlyVdsRemoteCtrlBasicSignal: m_vdsService->body->serviceData  is null");
            return false;
        }

        m_rcbRequest->serviceId = m_vdsService->body->serviceId;
        m_rcbRequest->serviceCommand = m_vdsService->body->serviceData->serviceCommand;

#if 1 // temp
        unsigned int num = m_vdsService->body->serviceData->serviceParameters.getCount();

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RemoteCtr list number = %d", num);

        asncserviceParameterss* para = (asncserviceParameterss*)(m_vdsService->body->serviceData->serviceParameters.getFirstElement());
        
        for (unsigned int i = 0; (i < num || para); i++)
        {
            asnMAXUINT len = 0;
            asnbyte* data;

            para->key.readOctetString(&len, &data);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RemoteCtr key = %s, size = %d", data, len);
            if (strncmp("time.start", (char *)data, len) == 0)
            {
//                m_rcbRequest->timeStart = para.timestampVal->seconds;
            }
            if (strncmp("time.window", (char *)data, len) == 0)
            {
//                m_rcbRequest->timeWindow= para.timestampVal->seconds;
            }
            if (strncmp("rws", (char *)data, len) == 0)
            {
                m_rcbRequest->value= para->intVal;
            }
            if (strncmp("rhl", (char *)data, len) == 0)
            {
                m_rcbRequest->value= para->intVal;
            }
            if (strncmp("rws", (char *)data, len) == 0)
            {
                m_rcbRequest->value= para->intVal;
            }

            para = (asncserviceParameterss*)(para->getNextElement());
        }
#else
        unsigned int num = m_vdsService->body->serviceData->serviceParameters->list.count;

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RemoteCtr list number = %d", num);

        for (unsigned int i = 0; i < num; i++)
        {
            struct ServiceParameter para = *(m_vdsService->body->serviceData->serviceParameters->list.array[i]);

            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RemoteCtr key = %s, size = %d", para.key.buf, para.key.size);

            if (strncmp("time.start", (char *)para.key.buf, para.key.size) == 0)
            {
//                m_rcbRequest->timeStart = para.timestampVal->seconds;
            }
            if (strncmp("time.window", (char *)para.key.buf, para.key.size) == 0)
            {
//                m_rcbRequest->timeWindow= para.timestampVal->seconds;
            }
            if (strncmp("rws", (char *)para.key.buf, para.key.size) == 0)
            {
                m_rcbRequest->value= *(para.intVal);
            }
            if (strncmp("rhl", (char *)para.key.buf, para.key.size) == 0)
            {
                m_rcbRequest->value= *(para.intVal);
            }
            if (strncmp("rws", (char *)para.key.buf, para.key.size) == 0)
            {
                m_rcbRequest->value= *(para.intVal);
            }
        }
#endif
        return true;

    }

    /************************************************************/
    // @brief :pack RemoteCtrlBasicResult_t into ASN1C structure. upload body msg.
    // @param[in]  response, RemoteCtrlBasicResult_t structure .
    // @return     True if successfully set, false otherwise
    // @author     Nie Yujin
    /************************************************************/  
    bool GlyVdsRemoteCtrlBasicSignal::SetResultPayload(RemoteCtrlBasicResult_t response)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s", __FUNCTION__);

        if(!SetHeadPayload())
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "GlyVdsRemoteCtrlBasicSignal: SetHeadPayload error");
            return false;
        }

        m_vdsResponse->optional.setPresence(asn_body);
        m_vdsResponse->body = new RequestBody();

        m_vdsResponse->body->optional.setPresence(asn_serviceId);
        m_vdsResponse->body->serviceId = m_rcbRequest->serviceId;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::serviceId = %d", m_vdsResponse->body->serviceId);
        
        m_vdsResponse->body->optional.setPresence(asn_serviceData);
        m_vdsResponse->body->serviceData = new ServiceData();
        
        m_vdsResponse->body->serviceData->optional.setPresence(asn_serviceResult);
        m_vdsResponse->body->serviceData->serviceResult = new ServiceResult();
        
        m_vdsResponse->body->serviceData->serviceResult->operationSucceeded = response.operationSucceeded;
        if(!response.operationSucceeded)
        {
            m_vdsResponse->body->serviceData->serviceResult->optional.setPresence(asn_error);
            m_vdsResponse->body->serviceData->serviceResult->error = new Error();
            m_vdsResponse->body->serviceData->serviceResult->error->code = response.errorCode;
            
            if(response.vehicleErrorCode > 0)
            {
                m_vdsResponse->body->serviceData->serviceResult->error->optional.setPresence(asn_vehicleErrorCode);
                m_vdsResponse->body->serviceData->serviceResult->error->vehicleErrorCode = response.vehicleErrorCode;
            }
            
            if(!response.message.empty())
            {
                m_vdsResponse->body->serviceData->serviceResult->error->optional.setPresence(asn_Error__message);
                m_vdsResponse->body->serviceData->serviceResult->error->message.copyOctetString(response.message.size(),(asnbyte *)response.message.c_str());

            }
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s completed.\n", __FUNCTION__);
        return true;
    }

    /************************************************************/
    // @brief :pack RemoteCtrlBasicStatus_t into ASN1C structure. upload body msg.
    // @param[in]  response, RemoteCtrlBasicStatus_t structure .
    // @return     True if successfully set, false otherwise
    // @author     Nie Yujin
    /************************************************************/ 
    bool GlyVdsRemoteCtrlBasicSignal::SetDoorStatusPayload(RemoteCtrlBasicResult_t result, RemoteCtrlBasicStatus_t response)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s", __FUNCTION__);

        if(!SetHeadPayload())
            return false;

        if(m_vdsResponse == nullptr)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "GlyVdsRemoteCtrlBasicSignal: m_vdsResponse is null");
            return false;
        }

        CreateVehicleStatus(m_vdsResponse);

        m_vdsResponse->body->serviceData->optional.setPresence(asn_serviceResult);
        m_vdsResponse->body->serviceData->serviceResult = new ServiceResult();
        
        m_vdsResponse->body->serviceData->serviceResult->operationSucceeded = result.operationSucceeded;
        if(!result.operationSucceeded)
        {
            m_vdsResponse->body->serviceData->serviceResult->optional.setPresence(asn_error);
            m_vdsResponse->body->serviceData->serviceResult->error = new Error();
            m_vdsResponse->body->serviceData->serviceResult->error->code = result.errorCode;
            
            if(result.vehicleErrorCode > 0)
            {
                m_vdsResponse->body->serviceData->serviceResult->error->optional.setPresence(asn_vehicleErrorCode);
                m_vdsResponse->body->serviceData->serviceResult->error->vehicleErrorCode = result.vehicleErrorCode;
            }
            
            if(!result.message.empty())
            {
                m_vdsResponse->body->serviceData->serviceResult->error->optional.setPresence(asn_Error__message);
                m_vdsResponse->body->serviceData->serviceResult->error->message.copyOctetString(result.message.size(),(asnbyte *)result.message.c_str());

            }
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s set result completed.\n", __FUNCTION__);
        
        CreateBasicVehicleStatus(m_vdsResponse->body->serviceData->vehicleStatus);
        SetBasicVehicleStatus(m_vdsResponse->body->serviceData->vehicleStatus->basicVehicleStatus, response);

        CreateAdditionalVehicleStatus(m_vdsResponse->body->serviceData->vehicleStatus);
        CreateDrivingSafetyStatus(m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus);
        SetDoorsStatus(m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->drivingSafetyStatus, response);

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s vehSpdIndcd = %d", __FUNCTION__, response.vehSpdIndcd);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s vehSpdIndcdQly = %d", __FUNCTION__, response.vehSpdIndcdQly);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorOpenStatusDriver = %d", __FUNCTION__, response.doorOpenStatusDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorOpenStatusPassenger = %d", __FUNCTION__, response.doorOpenStatusPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorOpenStatusDriverRear = %d", __FUNCTION__, response.doorOpenStatusDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorOpenStatusPassengerRear = %d", __FUNCTION__, response.doorOpenStatusPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorLockStatusDriver = %d", __FUNCTION__, response.doorLockStatusDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorLockStatusPassenger = %d", __FUNCTION__, response.doorLockStatusPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorLockStatusDriverRear = %d", __FUNCTION__, response.doorLockStatusDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorLockStatusPassengerRear = %d", __FUNCTION__, response.doorLockStatusPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s trunkOpenStatus = %d", __FUNCTION__, response.trunkOpenStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s trunkLockStatus = %d", __FUNCTION__, response.trunkLockStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s lockgCenStsForUsrFb = %d", __FUNCTION__, response.lockgCenStsForUsrFb);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s hoodSts = %d", __FUNCTION__, response.hoodSts);
        
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s completed.\n", __FUNCTION__);
        return true;

    }

    /************************************************************/
    // @brief :pack RemoteCtrlBasicStatus_t into ASN1C structure. upload body msg.
    // @param[in]  response, RemoteCtrlBasicStatus_t structure .
    // @return     True if successfully set, false otherwise
    // @author     Nie Yujin
    /************************************************************/ 
    bool GlyVdsRemoteCtrlBasicSignal::SetWinStatusPayload(RemoteCtrlBasicResult_t result, RemoteCtrlBasicStatus_t response)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s", __FUNCTION__);

        if(!SetHeadPayload())
            return false;

        if(m_vdsResponse == nullptr)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "GlyVdsRemoteCtrlBasicSignal: m_vdsResponse is null");
            return false;
        }

        CreateVehicleStatus(m_vdsResponse);

        m_vdsResponse->body->serviceData->optional.setPresence(asn_serviceResult);
        m_vdsResponse->body->serviceData->serviceResult = new ServiceResult();
        
        m_vdsResponse->body->serviceData->serviceResult->operationSucceeded = result.operationSucceeded;
        if(!result.operationSucceeded)
        {
            m_vdsResponse->body->serviceData->serviceResult->optional.setPresence(asn_error);
            m_vdsResponse->body->serviceData->serviceResult->error = new Error();
            m_vdsResponse->body->serviceData->serviceResult->error->code = result.errorCode;
            
            if(result.vehicleErrorCode > 0)
            {
                m_vdsResponse->body->serviceData->serviceResult->error->optional.setPresence(asn_vehicleErrorCode);
                m_vdsResponse->body->serviceData->serviceResult->error->vehicleErrorCode = result.vehicleErrorCode;
            }
            
            if(!result.message.empty())
            {
                m_vdsResponse->body->serviceData->serviceResult->error->optional.setPresence(asn_Error__message);
                m_vdsResponse->body->serviceData->serviceResult->error->message.copyOctetString(result.message.size(),(asnbyte *)result.message.c_str());

            }
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s set result completed.\n", __FUNCTION__);
        
        CreateBasicVehicleStatus(m_vdsResponse->body->serviceData->vehicleStatus);
        SetBasicVehicleStatus(m_vdsResponse->body->serviceData->vehicleStatus->basicVehicleStatus, response);

        CreateAdditionalVehicleStatus(m_vdsResponse->body->serviceData->vehicleStatus);
        CreateDrivingSafetyStatus(m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus);
        SetDoorsStatus(m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->drivingSafetyStatus, response);

        CreateClimateStatus(m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus);
        SetWinStatus(m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->climateStatus, response);

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s vehSpdIndcd = %d", __FUNCTION__, response.vehSpdIndcd);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s vehSpdIndcdQly = %d", __FUNCTION__, response.vehSpdIndcdQly);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorOpenStatusDriver = %d", __FUNCTION__, response.doorOpenStatusDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorOpenStatusPassenger = %d", __FUNCTION__, response.doorOpenStatusPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorOpenStatusDriverRear = %d", __FUNCTION__, response.doorOpenStatusDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorOpenStatusPassengerRear = %d", __FUNCTION__, response.doorOpenStatusPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorLockStatusDriver = %d", __FUNCTION__, response.doorLockStatusDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorLockStatusPassenger = %d", __FUNCTION__, response.doorLockStatusPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorLockStatusDriverRear = %d", __FUNCTION__, response.doorLockStatusDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s doorLockStatusPassengerRear = %d", __FUNCTION__, response.doorLockStatusPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s trunkOpenStatus = %d", __FUNCTION__, response.trunkOpenStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s trunkLockStatus = %d", __FUNCTION__, response.trunkLockStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s lockgCenStsForUsrFb = %d", __FUNCTION__, response.lockgCenStsForUsrFb);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s hoodSts = %d", __FUNCTION__, response.hoodSts);
        
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s winStatusDriver = %d", __FUNCTION__, response.winStatusDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s winStatusPassenger = %d", __FUNCTION__, response.winStatusPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s winStatusDriverRear = %d", __FUNCTION__, response.winStatusDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s winStatusPassengerRear = %d", __FUNCTION__, response.winStatusPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s sunroofOpenStatus = %d", __FUNCTION__, response.sunroofOpenStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s winPosDriver = %d", __FUNCTION__, response.winPosDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s winPosPassenger = %d", __FUNCTION__, response.winPosPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s winPosDriverRear = %d", __FUNCTION__, response.winPosDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s winPosPassengerRear = %d", __FUNCTION__, response.winPosPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s ventilateStatus = %d", __FUNCTION__, response.ventilateStatus);

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s completed.\n", __FUNCTION__);
        return true;

    }

    /************************************************************/
    // @brief :pack RemoteCtrlPollutionStatus_t into ASN1C structure. upload body msg.
    // @param[in]  response, RemoteCtrlBasicStatus_t structure .
    // @return     True if successfully set, false otherwise
    // @author     Nie Yujin
    /************************************************************/ 
    bool GlyVdsRemoteCtrlBasicSignal::SetPollutionStatusPayload(RemoteCtrlBasicResult_t result, RemoteCtrlBasicStatus_t response)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s", __FUNCTION__);

        if(!SetHeadPayload())
            return false;

        if(m_vdsResponse == nullptr)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "GlyVdsRemoteCtrlBasicSignal: m_vdsResponse is null");
            return false;
        }

        CreateVehicleStatus(m_vdsResponse);

        m_vdsResponse->body->serviceData->optional.setPresence(asn_serviceResult);
        m_vdsResponse->body->serviceData->serviceResult = new ServiceResult();
        
        m_vdsResponse->body->serviceData->serviceResult->operationSucceeded = result.operationSucceeded;
        if(!result.operationSucceeded)
        {
            m_vdsResponse->body->serviceData->serviceResult->optional.setPresence(asn_error);
            m_vdsResponse->body->serviceData->serviceResult->error = new Error();
            m_vdsResponse->body->serviceData->serviceResult->error->code = result.errorCode;
            
            if(result.vehicleErrorCode > 0)
            {
                m_vdsResponse->body->serviceData->serviceResult->error->optional.setPresence(asn_vehicleErrorCode);
                m_vdsResponse->body->serviceData->serviceResult->error->vehicleErrorCode = result.vehicleErrorCode;
            }
            
            if(!result.message.empty())
            {
                m_vdsResponse->body->serviceData->serviceResult->error->optional.setPresence(asn_Error__message);
                m_vdsResponse->body->serviceData->serviceResult->error->message.copyOctetString(result.message.size(),(asnbyte *)result.message.c_str());

            }
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s set result completed.\n", __FUNCTION__);

        CreateBasicVehicleStatus(m_vdsResponse->body->serviceData->vehicleStatus);
        SetBasicVehicleStatus(m_vdsResponse->body->serviceData->vehicleStatus->basicVehicleStatus, response);
        
        CreateAdditionalVehicleStatus(m_vdsResponse->body->serviceData->vehicleStatus);
        
        CreatePollutionStatus(m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus);
        SetPollutionStatus(m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->pollutionStatus, response);

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s vehSpdIndcd = %d", __FUNCTION__, response.vehSpdIndcd);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s vehSpdIndcdQly = %d", __FUNCTION__, response.vehSpdIndcdQly);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s interiorPM25 = %d", __FUNCTION__, response.interiorPM25);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s exteriorPM25 = %d", __FUNCTION__, response.exteriorPM25);

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s completed.\n", __FUNCTION__);
        return true;

    }
    
    /************************************************************/
    // @brief :pack RequestHeader.upload head msg.
    // @param[in]  
    // @return     True if successfully set, false otherwise
    // @author     Nie Yujin
    /************************************************************/ 
    bool GlyVdsRemoteCtrlBasicSignal::SetHeadPayload()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s\n", __FUNCTION__);

        if (m_vdsResponse)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s free_struct.m_vdsResponse addr:%0x\n", __FUNCTION__,m_vdsResponse);
            delete m_vdsResponse;
        }

        m_vdsResponse = new VDServiceRequest();
        
        VdmSignal::SetHeadPayload(m_vdsResponse);

        m_vdsResponse->header->optional.setPresence(asn_eventId);
        m_vdsResponse->header->eventId = new TimeStamp();
        
        m_vdsResponse->header->eventId->seconds = m_vdsService->header->eventId->seconds;
        
        m_vdsResponse->header->eventId->optional.setPresence(asn_milliseconds);
        m_vdsResponse->header->eventId->milliseconds= m_vdsService->header->eventId->milliseconds;

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s complete.\n", __FUNCTION__);
        
        return true;
    }

    /************************************************************/
    // @brief :pack RequestHeader.upload head msg.
    // @param[in]  
    // @return     True if successfully set, false otherwise
    // @author     Nie Yujin
    /************************************************************/ 
    bool GlyVdsRemoteCtrlBasicSignal::CreateBody(VDServiceRequest* req)
    {
        bool ret = true;
        
        if (req)
        {
            req->optional.setPresence(asn_body);
            if (req->body)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, body already created\n", __FUNCTION__);
            }
            else
            {
                req->body = new RequestBody();                
            }
        }
        else
        {
            ret = false;
        }
        
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }

    bool GlyVdsRemoteCtrlBasicSignal::CreateServiceData(VDServiceRequest* req)
    {
        bool ret = CreateBody(req);

        if (ret && req->body)
        {
            m_vdsResponse->body->optional.setPresence(asn_serviceId);
            m_vdsResponse->body->serviceId = m_rcbRequest->serviceId;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::serviceId = %d", m_vdsResponse->body->serviceId);
        
            req->body->optional.setPresence(asn_serviceData);
            if (req->body->serviceData)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, serviceData already created\n", __FUNCTION__);
            }
            else
            {
                req->body->serviceData = new ServiceData();
            }
            
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }

    bool GlyVdsRemoteCtrlBasicSignal::CreateVehicleStatus(VDServiceRequest* req)
    {
        bool ret = CreateServiceData(req);

        if (ret && req->body->serviceData)
        {
            req->body->serviceData->optional.setPresence(asn_vehicleStatus);
            if (req->body->serviceData->vehicleStatus)
            {
                delete req->body->serviceData->vehicleStatus;
            }
            req->body->serviceData->vehicleStatus = new VehicleStatus();
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
     }

    bool GlyVdsRemoteCtrlBasicSignal::CreateBasicVehicleStatus(VehicleStatus* vehicleStatus)
    {
        bool ret = true;

        if (vehicleStatus)
        {
            vehicleStatus->optional.setPresence(asn_basicVehicleStatus);
            if (vehicleStatus->basicVehicleStatus)
            {
                delete vehicleStatus->basicVehicleStatus;
            }
            vehicleStatus->basicVehicleStatus = new BasicVehicleStatus();
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }
    
    bool GlyVdsRemoteCtrlBasicSignal::CreateAdditionalVehicleStatus(VehicleStatus* vehicleStatus)
    {
        bool ret = true;

        if (vehicleStatus)
        {
            vehicleStatus->optional.setPresence(asn_additionalVehicleStatus);
            if (vehicleStatus->additionalVehicleStatus)
            {
                delete vehicleStatus->additionalVehicleStatus;
            }
            vehicleStatus->additionalVehicleStatus = new AdditionalVehicleStatus();
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }

    bool GlyVdsRemoteCtrlBasicSignal::CreateDrivingSafetyStatus(AdditionalVehicleStatus* additionalVehicleStatus)
    {
        bool ret = true;

        if (additionalVehicleStatus)
        {
            additionalVehicleStatus->optional.setPresence(asn_drivingSafetyStatus);
            if (additionalVehicleStatus->drivingSafetyStatus)
            {
                delete additionalVehicleStatus->drivingSafetyStatus;
            }
            additionalVehicleStatus->drivingSafetyStatus = new DrivingSafetyStatus();
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }

    bool GlyVdsRemoteCtrlBasicSignal::CreateClimateStatus(AdditionalVehicleStatus* additionalVehicleStatus)
    {
        bool ret = true;

        if (additionalVehicleStatus)
        {
            additionalVehicleStatus->optional.setPresence(asn_climateStatus);
            if (additionalVehicleStatus->climateStatus)
            {
                delete additionalVehicleStatus->climateStatus;
            }
            additionalVehicleStatus->climateStatus = new ClimateStatus();
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }

    bool GlyVdsRemoteCtrlBasicSignal::CreatePollutionStatus(AdditionalVehicleStatus* additionalVehicleStatus)
    {
        bool ret = true;

        if (additionalVehicleStatus)
        {
            additionalVehicleStatus->optional.setPresence(asn_pollutionStatus);
            if (additionalVehicleStatus->pollutionStatus)
            {
                delete additionalVehicleStatus->pollutionStatus;
            }
            additionalVehicleStatus->pollutionStatus = new PollutionStatus();
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }

    bool GlyVdsRemoteCtrlBasicSignal::SetBasicVehicleStatus(BasicVehicleStatus* basicVehicleStatus, RemoteCtrlBasicStatus_t response)
    {
        bool ret = true;
        
        if (basicVehicleStatus)
        {
            //set postion states
            basicVehicleStatus->position = new Position();
            
            if(CheckFieldsInAsnScope_long(response.position.latitude, -2147483648, 2147483647))
            {
                basicVehicleStatus->position->optional.setPresence(asn_latitude);
                basicVehicleStatus->position->latitude = response.position.latitude;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::position.latitude:%d -> is cross ASN scope(-2147483648, 2147483647), error!!!!", response.position.latitude);
            }

            if(CheckFieldsInAsnScope_long(response.position.longitude, -2147483648, 2147483647))
            {
                basicVehicleStatus->position->optional.setPresence(asn_longitude);
                basicVehicleStatus->position->longitude= response.position.longitude;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::position.longitude:%d -> is cross ASN scope(-2147483648, 2147483647), error!!!!", response.position.longitude);
            }

            if(CheckFieldsInAsnScope_long(response.position.altitude, -100, 8091))
            {
                basicVehicleStatus->position->optional.setPresence(asn_altitude);
                basicVehicleStatus->position->altitude = response.position.altitude;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::position.altitude:%d -> is cross ASN scope(-100, 8091), error!!!!", response.position.altitude);
            }

            if(CheckFieldsInAsnScope_int(response.position.posCanBeTrusted, 0, 1))
            {
                basicVehicleStatus->position->optional.setPresence(asn_posCanBeTrusted);
                basicVehicleStatus->position->posCanBeTrusted = response.position.posCanBeTrusted;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::position.posCanBeTrusted:%d -> is cross ASN scope(0, 1), error!!!!", response.position.posCanBeTrusted);
            }

            if(CheckFieldsInAsnScope_int(response.position.carLocatorStatUploadEn, 0, 1))
            {
                basicVehicleStatus->position->optional.setPresence(asn_carLocatorStatUploadEn);
                basicVehicleStatus->position->carLocatorStatUploadEn = response.position.carLocatorStatUploadEn;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::position.carLocatorStatUploadEn:%d -> is cross ASN scope(0, 1), error!!!!", response.position.carLocatorStatUploadEn);
            }

            if(CheckFieldsInAsnScope_int(response.position.marsCoordinates, 0, 1))
            {
                basicVehicleStatus->position->optional.setPresence(asn_marsCoordinates);
                basicVehicleStatus->position->marsCoordinates = response.position.marsCoordinates;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::position.marsCoordinates:%d -> is cross ASN scope(0, 1), error!!!!", response.position.marsCoordinates);
            }

            basicVehicleStatus->speed = response.vehSpdIndcd;

            basicVehicleStatus->optional.setPresence(asn_BasicVehicleStatus__speedValidity);
            basicVehicleStatus->speedValidity = response.vehSpdIndcdQly;
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }
    
    bool GlyVdsRemoteCtrlBasicSignal::SetDoorsStatus(DrivingSafetyStatus* drivingSafetyStatus, RemoteCtrlBasicStatus_t response)
    {
        bool ret = true;
        
        if (drivingSafetyStatus)
        {
            drivingSafetyStatus->optional.setPresence(asn_doorOpenStatusDriver);
            drivingSafetyStatus->doorOpenStatusDriver = response.doorOpenStatusDriver;

            drivingSafetyStatus->optional.setPresence(asn_doorOpenStatusPassenger);
            drivingSafetyStatus->doorOpenStatusPassenger = response.doorOpenStatusPassenger;

            drivingSafetyStatus->optional.setPresence(asn_doorOpenStatusDriverRear);
            drivingSafetyStatus->doorOpenStatusDriverRear = response.doorOpenStatusDriverRear;

            drivingSafetyStatus->optional.setPresence(asn_doorOpenStatusPassengerRear);
            drivingSafetyStatus->doorOpenStatusPassengerRear = response.doorOpenStatusPassengerRear;

            drivingSafetyStatus->optional.setPresence(asn_doorLockStatusDriver);
            drivingSafetyStatus->doorLockStatusDriver = response.doorLockStatusDriver;

            drivingSafetyStatus->optional.setPresence(asn_doorLockStatusPassenger);
            drivingSafetyStatus->doorLockStatusPassenger = response.doorLockStatusPassenger;

            drivingSafetyStatus->optional.setPresence(asn_doorLockStatusDriverRear);
            drivingSafetyStatus->doorLockStatusDriverRear = response.doorLockStatusDriverRear;

            drivingSafetyStatus->optional.setPresence(asn_doorLockStatusPassengerRear);
            drivingSafetyStatus->doorLockStatusPassengerRear = response.doorLockStatusPassengerRear;

            drivingSafetyStatus->optional.setPresence(asn_trunkOpenStatus);
            drivingSafetyStatus->trunkOpenStatus = response.trunkOpenStatus;

            //temp code for GLY-3674
            //value 1 map to 0 in VDS
            //value 2 map to 1 in VDS
            //other values not support, customer will update it.
            if (1 == response.trunkLockStatus)
            {
                drivingSafetyStatus->optional.setPresence(asn_trunkLockStatus);
                drivingSafetyStatus->trunkLockStatus = 0;
            }
            else if (2 == response.trunkLockStatus)
            {
                drivingSafetyStatus->optional.setPresence(asn_trunkLockStatus);
                drivingSafetyStatus->trunkLockStatus = 1;
            }
            else
            {
                // Nothing to do
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, trunkLockStatus(%d) is out of range", __FUNCTION__, response.trunkLockStatus);
            }

            drivingSafetyStatus->optional.setPresence(asn_centralLockingStatus);
            drivingSafetyStatus->centralLockingStatus = response.lockgCenStsForUsrFb;

            drivingSafetyStatus->optional.setPresence(asn_engineHoodOpenStatus);
            drivingSafetyStatus->engineHoodOpenStatus = response.hoodSts;
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }

    bool GlyVdsRemoteCtrlBasicSignal::SetWinStatus(ClimateStatus* climateStatus, RemoteCtrlBasicStatus_t response)
    {
        bool ret = true;
       
        if (climateStatus)
        {
            climateStatus->optional.setPresence(asn_winStatusDriver);
            climateStatus->winStatusDriver = response.winStatusDriver;

            climateStatus->optional.setPresence(asn_winStatusPassenger);
            climateStatus->winStatusPassenger = response.winStatusPassenger;

            climateStatus->optional.setPresence(asn_winStatusDriverRear);
            climateStatus->winStatusDriverRear = response.winStatusDriverRear;

            climateStatus->optional.setPresence(asn_winStatusPassengerRear);
            climateStatus->winStatusPassengerRear = response.winStatusPassengerRear;

            climateStatus->optional.setPresence(asn_sunroofOpenStatus);
            climateStatus->sunroofOpenStatus = response.sunroofOpenStatus;

            if(CheckFieldsInAsnScope_int(response.ventilateStatus, 0, 1))
            {
                climateStatus->optional.setPresence(asn_ventilateStatus);
                climateStatus->ventilateStatus = response.ventilateStatus;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::climateStatus->ventilateStatus:%d -> is cross ASN scope(0, 1), error!!!!", response.ventilateStatus);
            }

            climateStatus->optional.setPresence(asn_winPosDriver);
            climateStatus->winPosDriver = response.winPosDriver;

            climateStatus->optional.setPresence(asn_winPosPassenger);
            climateStatus->winPosPassenger = response.winPosPassenger;

            climateStatus->optional.setPresence(asn_winPosDriverRear);
            climateStatus->winPosDriverRear = response.winPosDriverRear;

            climateStatus->optional.setPresence(asn_winPosPassengerRear);
            climateStatus->winPosPassengerRear = response.winPosPassengerRear;

        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    } 

    bool GlyVdsRemoteCtrlBasicSignal::SetPollutionStatus(PollutionStatus* pollutionStatus, RemoteCtrlBasicStatus_t response)
    {
        bool ret = true;
        
        if (pollutionStatus)
        {
            pollutionStatus->optional.setPresence(asn_interiorPM25);
            pollutionStatus->interiorPM25 = response.interiorPM25;

            pollutionStatus->optional.setPresence(asn_exteriorPM25);
            pollutionStatus->exteriorPM25 = response.exteriorPM25;
        }
        else
        {
            ret = false;
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRemoteCtrlBasicSignal::%s, ret = %d\n", __FUNCTION__, ret);
        
        return ret;
    }

    bool GlyVdsRemoteCtrlBasicSignal::CheckFieldsInAsnScope_long(long inputValue, long minValue, long maxValue)
    {
        if((inputValue < minValue)||(inputValue > maxValue))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    bool GlyVdsRemoteCtrlBasicSignal::CheckFieldsInAsnScope_int(int inputValue, int minValue, int maxValue)
    {
        if((inputValue < minValue)||(inputValue > maxValue))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
} // namespace volvo_on_call

