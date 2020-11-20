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
 *  \file     ip_command_broker_signal_source.cc
 *  \brief    VOC Service IPCommandBroker signal source.
 *  \author   Axel Fagerstedt, Florian Schindler
 *  \todo     there is lots of similar calls, could they be unified (e.g. macros, template) to avoid much typing?
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"

#include <algorithm>
#include <map>
#include <functional>

#include "dlt/dlt.h"
#include "ipcb_IClient.hpp"
#include "ipcb_ITelematicsService.hpp"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

/**
 * \brief Callback proxy to be registered with IPCommandBroker.
 *        Needed as the interface expects c style callback functions.
 * \param[in] notification_data The privacy notification data.
 * \param[in] request_id        The request id.
 * \return None.
 */
static void PrivacyNotificationCbProxy(PrivacyNotificationType *notification_data,
                                        uint64_t request_id)
{
    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.PrivacyNotificationCb(notification_data, request_id);
}


/**
 * \brief Callback proxy to be registered with IPCommandBroker.
 *        Needed as the interface expects c style callback functions.
 *  for internal use only
 *
 * \param[in] data  The payload data, see \ref DeadReckonedPositionResponse
 * \param[in] request_id  The request id (should be mirrored back to us)
 */
static void DeadReckonedPositionResponseCbProxy(DeadReckonedPositionResponse *data,
                                        uint64_t request_id)
{
    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.DeadReckonedPositionResponseCb(data, request_id);
}


/**
 * \brief Callback proxy to be registered with IPCommandBroker.
 *        Needed as the interface expects c style callback functions.
 *  for internal use only
 *
 * \param[in] data  The payload data, see \ref GNSSPositionDataResponse
 * \param[in] request_id  The request id (should be mirrored back to us)
 */
static void GNSSPositionDataResponseCbProxy(GNSSPositionDataResponse *data,
                                            uint64_t request_id)
{
    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.GNSSPositionDataResponseCb(data, request_id);
}

static void THEFTNoticeNotificationCbProxy(bool antithftActvnNotif,
                                                        uint32_t alrmTrgSrc,
                                                        bool snsrSoundrBattBackedFailr,
                                                        uint32_t alrmSt,
                                                        bool snsrInclnFailr,
                                                        bool snsrIntrScanrFailr)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, antithftActvnNotif is %d", __FUNCTION__, antithftActvnNotif);
    TheftNoticeData theft;
    theft.antithftActvnNotif = antithftActvnNotif;
    theft.alrmTrgSrc = alrmTrgSrc;
    theft.snsrSoundrBattBackedFailr = snsrSoundrBattBackedFailr;
    theft.alrmSt = alrmSt;
    theft.snsrInclnFailr = snsrInclnFailr;
    theft.snsrSoundrBattBackedFailr = snsrIntrScanrFailr;
    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.THEFTNoticeNotificationCb(theft);
}

static void customerInfoMessaging_serviceCb(OperationId operationId, OperationType operationType, uint64_t requestId, DataBase* data)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s - %04X.%02d.%02d", __FUNCTION__, operationId, operationType, requestId);

    if(operationId == OperationId::CustomerInfoMessaging && operationType == OperationType::NOTIFICATION)
    {
        CustomerInfoMessaging_Notification_Data* pdata = dynamic_cast<CustomerInfoMessaging_Notification_Data*>(data);
        bool antithftActvnNotif = pdata->antithftActvnNotif;
        uint32_t alrmTrgSrc = (uint32_t)pdata->alrmSts1.alrmTrgSrc;
        bool snsrSoundrBattBackedFailr = pdata->alrmSts1.snsrSoundrBattBackedFailr;
        uint32_t alrmSt = (uint32_t)pdata->alrmSts1.alrmSt;
        bool snsrIntrScanrFailr = pdata->alrmSts1.snsrIntrScanrFailr;
        bool snsrInclnFailr = pdata->alrmSts1.snsrInclnFailr;
        THEFTNoticeNotificationCbProxy(antithftActvnNotif, alrmTrgSrc, snsrSoundrBattBackedFailr, alrmSt, snsrInclnFailr, snsrIntrScanrFailr);
    }
}


static void TelmDshbNotificationCbProxy(OpTelmDshb_NotificationCyclic_Data TelmDshb_data)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s", __FUNCTION__);
    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.TelmDshbNotificationCb(TelmDshb_data);
}

static void VehicleDataUploadServiceCb(OperationId operationId, OperationType operationType, uint64_t requestId, DataBase* data)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s - %04X.%02d.%02d", __FUNCTION__, operationId, operationType, requestId);
    if(operationId == OperationId::TelmDhsb && operationType == OperationType::NOTIFICATION_CYCLIC)
    {
        OpTelmDshb_NotificationCyclic_Data* pdata = dynamic_cast<OpTelmDshb_NotificationCyclic_Data*>(data);
        TelmDshbNotificationCbProxy(*pdata);
    }
}

static void RMSNotificationCbProxy(RemoteMonitoringInfo_t *rms_data)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive rms data", __FUNCTION__);
    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.RMSNotificationCb(rms_data);
}

static void HvBattVoltageCbProxy(uint32_t *arrydata,
                                                  uint32_t arryDataSize,
                                                  uint32_t hvBattIDc1,
                                                  uint32_t hvBattNr,
                                                  uint32_t hvBattUDc,
                                                  uint32_t hvBattVoltMaxSerlNr,
                                                  uint32_t hvBattVoltMinSerlNr,
                                                  uint32_t packNr)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive HvBattVoltage data", __FUNCTION__);
    HvBattVoltageData hbv;
    hbv.arrydata = arrydata;
    hbv.arryDataSize = arryDataSize;
    hbv.hvBattIDc1 = hvBattIDc1;
    hbv.hvBattNr = hvBattNr;
    hbv.hvBattUDc = hvBattUDc;
    hbv.hvBattVoltMaxSerlNr = hvBattVoltMaxSerlNr;
    hbv.hvBattVoltMinSerlNr = hvBattVoltMinSerlNr;
    hbv.packNr = packNr;

    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.HvBattVoltageCb(hbv);
}

static void HvBattTempCbProxy(uint32_t *arrydata,
                                               uint32_t arryDataSize,
                                               uint32_t hvBattCellTNr,
                                               uint32_t hvBattCellTSerlNr,
                                               uint32_t hvBattNr,
                                               uint32_t hvBattTMaxSerlNr,
                                               uint32_t hvBattTMinSerlNr,
                                               uint32_t packNr)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive HvBattTemp data", __FUNCTION__);
    HvBattTempData hbt;
    hbt.arrydata = arrydata;
    hbt.arryDataSize = arryDataSize;
    hbt.hvBattCellTNr = hvBattCellTNr;
    hbt.hvBattCellTSerlNr = hvBattCellTSerlNr;
    hbt.hvBattNr = hvBattNr;
    hbt.hvBattTMaxSerlNr = hvBattTMaxSerlNr;
    hbt.hvBattTMinSerlNr = hvBattTMinSerlNr;
    hbt.packNr = packNr;
    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.HvBattTempCb(hbt);
}

static void VehSoHCheckResponseCbProxy(int32_t srvTrig,
                                             int32_t dayToSrv,
                                             int32_t engHrToSrv,
                                             int32_t dstToSrv,
                                             bool srvRst)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, srvTrig is %d", __FUNCTION__, srvTrig);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, dayToSrv is %d", __FUNCTION__, dayToSrv);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, engHrToSrv is %d", __FUNCTION__, engHrToSrv);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, dstToSrv is %d", __FUNCTION__, dstToSrv);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, srvRst is %d", __FUNCTION__, srvRst);
    VehSoHCheckData vehSoHCheckData;
    memset(&vehSoHCheckData, 0, sizeof(VehSoHCheckData));
    vehSoHCheckData.srvTrig = srvTrig;
    vehSoHCheckData.dayToSrv = dayToSrv;
    vehSoHCheckData.engHrToSrv = engHrToSrv;
    vehSoHCheckData.dstToSrv = dstToSrv;
    vehSoHCheckData.srvRst = srvRst;
    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.VehSoHCheckResponseCb(vehSoHCheckData);
}

static void VehSoHCheckNotifyCbProxy(int32_t srvTrig,
                                             int32_t dayToSrv,
                                             int32_t engHrToSrv,
                                             int32_t dstToSrv,
                                             bool srvRst)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, srvTrig is %d", __FUNCTION__, srvTrig);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, dayToSrv is %d", __FUNCTION__, dayToSrv);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, engHrToSrv is %d", __FUNCTION__, engHrToSrv);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, dstToSrv is %d", __FUNCTION__, dstToSrv);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, srvRst is %d", __FUNCTION__, srvRst);
    VehSoHCheckData vehSoHCheckData;
    memset(&vehSoHCheckData, 0, sizeof(VehSoHCheckData));
    vehSoHCheckData.srvTrig = srvTrig;
    vehSoHCheckData.dayToSrv = dayToSrv;
    vehSoHCheckData.engHrToSrv = engHrToSrv;
    vehSoHCheckData.dstToSrv = dstToSrv;
    vehSoHCheckData.srvRst = srvRst;
    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.VehSoHCheckNotifyCb(vehSoHCheckData);
}

static void OTAServiceProxy(OperationId id, OperationType type,uint64_t requestId, DataBase* data)
{
    std::shared_ptr<Signal> signal = nullptr;

    if (OperationId::OTAQueryAvailableStorage == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg OTAQueryAvailableStorage.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id(requestId);
        OpOTAQueryAvailableStorage_Response_Data *pData = dynamic_cast<OpOTAQueryAvailableStorage_Response_Data*>(data);
        Signal::SignalType signal_type = Signal::kOTAQueryAvailableStorageResp;
        signal = QueryAvailableStorageRespSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::OTAAssignmentNotification == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg OTAAssignmentNotification.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpOTAAssignmentNotification_Notification_Data *pData = dynamic_cast<OpOTAAssignmentNotification_Notification_Data*>(data);
        Signal::SignalType signal_type = Signal::kOTAAssignmentNotification;
        signal = AssignmentNotificationDataSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::OTAAssignmentSync == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg OTAAssignmentSync.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpGeneric_Request_Data *pData = dynamic_cast<OpGeneric_Request_Data*>(data);
        Signal::SignalType signal_type = Signal::kOTAAssignmentSyncReq;
        signal = GenericRequestSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::OTAHMILanguageSettings == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg OTAHMILanguageSettings.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpOTAHMILanguageSettings_Notification_Data *pData = dynamic_cast<OpOTAHMILanguageSettings_Notification_Data*>(data);
        Signal::SignalType signal_type = Signal::kOTAHMILanguageSettingsNotification;
        signal = OTAHMILanguageSettingsSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::DownloadConsent == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg DownloadConsent.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpDownloadConsent_Notification_Data *pData = dynamic_cast<OpDownloadConsent_Notification_Data*>(data);
        Signal::SignalType signal_type = Signal::kDownloadConsentNotification;
        signal = DownloadConsentSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::OTAInstallationSummary== id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg OTAInstallationSummary.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpOTAInstallationSummary_Notification_Data *pData = dynamic_cast<OpOTAInstallationSummary_Notification_Data*>(data);
        Signal::SignalType signal_type = Signal::kOTAInstallationSummaryNotification;
        signal = InstallationSummaryNotificationSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::InstallationConsent == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg InstallationConsent.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpInstallationConsent_Notification_Data *pData = dynamic_cast<OpInstallationConsent_Notification_Data*>(data);
        Signal::SignalType signal_type = Signal::kInstallationConsentNotification;
        signal = InstallationConsentSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::OTAAssignBOOT == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg OTAAssignBOOT.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpOTAAssignBOOT_Response_Data *pData = dynamic_cast<OpOTAAssignBOOT_Response_Data*>(data);
        Signal::SignalType signal_type = Signal::kOTAAssignBOOTResp;
        signal = OTAAssignBOOTRespSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::OTAWriteAssignmentData == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg DownloadConsent.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpOTAWriteAssignmentData_Response_Data *pData = dynamic_cast<OpOTAWriteAssignmentData_Response_Data*>(data);
        Signal::SignalType signal_type = Signal::kOTAWriteAssignmentDataResp;
        signal = OTAWriteAssignmentDataRespSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::OTAExceptionReports == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg OTAExceptionReports.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpOTAExceptionReports_Notification_Data *pData = dynamic_cast<OpOTAExceptionReports_Notification_Data*>(data);
        Signal::SignalType signal_type = Signal::kOTAExceptionReportNotification;
        signal = OTAExceptionReportNotificationSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s, receive unknown OTA data OpId=%d.", __FUNCTION__,id);
    }

    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.BroadcastSignal(signal);
}

static void RVDCServiceProxy(OperationId id, OperationType type,uint64_t requestId, DataBase* data)
{
	DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s, id = %d, type = %d, request = %d", __FUNCTION__, id, type, requestId);

    IpCommandBrokerTransactionId transaction_id;
    
    switch (id)
    {
    case OperationId::RVDCMeasurementSynchronization:
    {
        Signal::SignalType signal_type = Signal::kRvdcReqMA;
        std::shared_ptr<Signal> signal = 
            RVDCMeasurementSynchronizationRequestSignal::CreateIpCommandBrokerSignal(
            *(OpRVDCMeasurementSynchronization_Request_Data*)data, transaction_id, signal_type);
        fsm::IpCommandBrokerSignalSource::GetInstance().BroadcastSignal(signal);
        break;
    }
    case OperationId::RVDCAssignmentNotificaiton:
    {
        Signal::SignalType signal_type = Signal::kRvdcNotifyAssign;
        std::shared_ptr<Signal> signal = 
            RVDCAssignmentNotificaitonSignal::CreateIpCommandBrokerSignal(
            *(OpRVDCAssignmentNotification_Notification_Data*)data, transaction_id, signal_type);
        fsm::IpCommandBrokerSignalSource::GetInstance().BroadcastSignal(signal);
        break;
    }
    case OperationId::RVDCGPSTime:
    {
        Signal::SignalType signal_type = Signal::kRvdcReqGPSTime;
        std::shared_ptr<Signal> signal = 
            RVDCGPSTimeSignal::CreateIpCommandBrokerSignal(
            *(OpGeneric_Request_Data*)data, transaction_id, signal_type);
        fsm::IpCommandBrokerSignalSource::GetInstance().BroadcastSignal(signal);
        break;
    }
    case OperationId::RVDCConnectivityStatus:
    {
        Signal::SignalType signal_type = Signal::kRvdcReqConnectivitySts;
        std::shared_ptr<Signal> signal = 
            RVDCConnectivityStatusSignal::CreateIpCommandBrokerSignal(
            *(OpGeneric_Request_Data*)data, transaction_id, signal_type);
        fsm::IpCommandBrokerSignalSource::GetInstance().BroadcastSignal(signal);
        break;
    }
    case OperationId::RVDCMeasurementDataPackage:
    {
        Signal::SignalType signal_type = Signal::kRvdcUploadMDP;
        std::shared_ptr<Signal> signal = 
            RVDCMeasurementDataPackageSignal::CreateIpCommandBrokerSignal(
            *(OpRVDCMeasurementDataPackage_Request_Data*)data, transaction_id, signal_type);
        fsm::IpCommandBrokerSignalSource::GetInstance().BroadcastSignal(signal);
        break;
    }
    case OperationId::RVDCAuthorizationStatusRequest:
    {
        Signal::SignalType signal_type = Signal::kRvdcReqAuthorizationSts;
        std::shared_ptr<Signal> signal = 
            RVDCAuthorizationStatusRequestSignal::CreateIpCommandBrokerSignal(
            *(OpGeneric_Request_Data*)data, transaction_id, signal_type);
        fsm::IpCommandBrokerSignalSource::GetInstance().BroadcastSignal(signal);
        break;
    }
    default:
        break;
    }
}

static void RemoteMonitoringSystemServiceProxy(OperationId id, OperationType type,uint64_t requestId, DataBase* data)
{
    std::shared_ptr<Signal> signal = nullptr;

    if (OperationId::RemoteMonitoringInfo == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg RemoteMonitoringInfo.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpRemoteMonitoringInfo_NotificationCyclic_Data *pData = dynamic_cast<OpRemoteMonitoringInfo_NotificationCyclic_Data*>(data);
        Signal::SignalType signal_type = Signal::kRemoteMonitoringInfo;
        signal = RemoteMonitoringInfoSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::HvBattVoltageData == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg HvBattVoltageData.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpHvBattVoltageData_Notification_Data *pData = dynamic_cast<OpHvBattVoltageData_Notification_Data*>(data);
        Signal::SignalType signal_type = Signal::kHvBattVoltageData;
        signal = HvBattVoltageDataSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::HvBattTempData == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg HvBattTempData.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpHvBattTempData_Notification_Data *pData = dynamic_cast<OpHvBattTempData_Notification_Data*>(data);
        Signal::SignalType signal_type = Signal::kHvBattTempData;
        signal = HvBattTempDataSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else if (OperationId::HvBattCod == id)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s, receive msg HvBattCod.", __FUNCTION__);
        IpCommandBrokerTransactionId transaction_id;
        OpHvBattCod_Notification_Data *pData = dynamic_cast<OpHvBattCod_Notification_Data*>(data);
        Signal::SignalType signal_type = Signal::kHvBattCod;
        signal = HvBattCodSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s, receive unknown RMS data OpId=%d.", __FUNCTION__,id);
    }

    IpCommandBrokerSignalSource& broker = IpCommandBrokerSignalSource::GetInstance();
    broker.BroadcastSignal(signal);
}

IpCommandBrokerSignalSource::IpCommandBrokerSignalSource()
{
    broker_initialized_ = IpcbIClient::Init();

    if (!broker_initialized_)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
            "IpCommandBrokerSignalSource, failed to initialize IPCB.");
    }
}

IpCommandBrokerSignalSource::~IpCommandBrokerSignalSource()
{
    if (broker_initialized_)
    {
        // deregister first:
        for (auto const &signal_map_element : active_subscriptions_)
        {
            UnregisterSignal(signal_map_element.first, true);  // force deregistering, regardless of reference counter
        }
        broker_initialized_ = ! IpcbIClient::Deinit();
    }
}

IpCommandBrokerSignalSource& IpCommandBrokerSignalSource::GetInstance()
{
    static IpCommandBrokerSignalSource instance;
    return instance;
}


IpcbIGNSSService& IpCommandBrokerSignalSource::GetIpcbGNSSServiceObject()
{
    return IpcbIClient::GetGNSSService();
}

IpcbIOTAService& IpCommandBrokerSignalSource::GetIpcbIOTAServiceObject()
{
    return IpcbIClient::GetOTAService();
}

IpcbIRVDCService& IpCommandBrokerSignalSource::GetIpcbRvdcServiceObject()
{
    return IpcbIClient::GetRVDCService();
}

bool IpCommandBrokerSignalSource::SubscribeForSignal(Signal::SignalType signal_type)
{
    bool return_value = RegisterSignal(signal_type);
    if (return_value)
    {
        switch (signal_type)
        {
            case Signal::kPrivacyNotificationSignal:
            {
                // send out our subscription:
                return_value = IpcbIClient::GetPrivacyService().PrivacySubscribe(
                                   SubscribeType::SubscribeStart,
                                   IpCommandBrokerTransactionId::kUndefinedTransactionId);
                                   // Undefined transaction id as this is not a request
                break;
            }
            default:
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,\
                    "IpCommandBrokerSignalSource, Subscribe, received unhandeled signal %u.", \
                    static_cast<unsigned int>(signal_type));
            }
        }  // switch
        if (! return_value)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,\
                "IpCommandBrokerSignalSource, SubscribeStart of signal %u failed", \
                static_cast<unsigned int>(signal_type));
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,\
            "IpCommandBrokerSignalSource RegisterSignals failed", \
            static_cast<unsigned int>(signal_type));
    }
    return return_value;
}


bool IpCommandBrokerSignalSource::RegisterSignals(std::vector<Signal::SignalType> requested_subscriptions)
{
    bool return_value = true;
    bool temp_return_value = true;

    if (broker_initialized_)
    {
        for (Signal::SignalType &signal_type : requested_subscriptions)
        {
            temp_return_value = RegisterSignal(signal_type);
            return_value = temp_return_value && return_value;  // keep false return_value
        }  // for
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
            "IpCommandBrokerSignalSource::RegisterSignals failed, not initialised");
        return_value = false;
    }
    return return_value;
}


bool IpCommandBrokerSignalSource::RegisterSignal(Signal::SignalType signal_type)
{
    // guard against concurrent registrations
    std::lock_guard<std::mutex> lock(subscriptions_mutex_);
    bool return_value = true;

    if (broker_initialized_)
    {
        if ( active_subscriptions_.find(signal_type) == active_subscriptions_.end() )
        {  // if not in list: register callback
            switch (signal_type)
            {
                case Signal::kPrivacyNotificationSignal:
                {
                    return_value = IpcbIClient::GetPrivacyService().RegisterPrivacyNotificationCb(
                        &PrivacyNotificationCbProxy);
                    break;
                }
                case Signal::kDeadReckonedPositionSignal:
                {
                    return_value = IpcbIClient::GetGNSSService().RegisterDeadReckonedPositionResponseCb(
                        &DeadReckonedPositionResponseCbProxy);
                    break;
                }
                case Signal::kGNSSPositionDataSignal:
                {
                    return_value = IpcbIClient::GetGNSSService().RegisterGNSSPositionDataResponseCb(
                        &GNSSPositionDataResponseCbProxy);
                    break;
                }
                case Signal::kTheftDataSignal:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "kTheftDataSignal, register theft signal");
                    return_value = IpcbIClient::GetCustomerInfoMessagingService().registerCustomerInfoMessagingService(
                        (fgen)&customerInfoMessaging_serviceCb);
                    break;
                }
                case Signal::kTelmDshb:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received kTelmDshb info.");
                    return_value = IpcbIClient::GetVehicleDataUploadService().registerVehicleDataUploadService(
                        (fgen)&VehicleDataUploadServiceCb);
                    break;
                }
                case Signal::kRMSDataSignal:
                {
                       DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO,
                       "kRMSDataSignal, register rms signal");
                    return_value = IpcbIClient::GetTelematicsService().RegisterRemoteMonitoringInfoNotificationCycleCb(
                        &RMSNotificationCbProxy);
                    break;
                }
                case Signal::kHvBattVoltageSignal:
                {
                       DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO,
                       "kHvBattVoltageSignal, register HvBattVoltage signal");
                    return_value = IpcbIClient::GetTelematicsService().RegisterHvBattVoltageDataNotificationCb(
                        &HvBattVoltageCbProxy);
                    break;
                }
                case Signal::kHvBattTempSignal:
                {
                       DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO,
                       "kHvBattTempSignal, register HvBattTemp signal");
                    return_value = IpcbIClient::GetTelematicsService().RegisterHvBattTempDataNotificationCb(
                        &HvBattTempCbProxy);
                    break;
                }
                case Signal::kVehSoHCheckResponse:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received kVehSoHCheckResponse info.");
                    return_value = IpcbIClient::GetTelematicsService().RegisterVehicleSoHCheckResponseCb(
                        &VehSoHCheckResponseCbProxy);
                    break;
                }
                case Signal::kVehSoHCheckNotify:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received kVehSoHCheckNotify info.");
                    return_value = IpcbIClient::GetTelematicsService().RegisterVehicleSoHCheckNotificationCb(
                        &VehSoHCheckNotifyCbProxy);
                    break;
                }
                // RMS
                case Signal::kRemoteMonitoringInfo:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received kRemoteMonitoringInfo info.");
                    return_value = IpcbIClient::GetRemoteMonitoringSystemService().registerRemoteMonitoringSystemService((fgen)RemoteMonitoringSystemServiceProxy);
                    break;
                }
                case Signal::kHvBattVoltageData:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received kHvBattVoltageData info.");
                    return_value = IpcbIClient::GetRemoteMonitoringSystemService().registerRemoteMonitoringSystemService((fgen)RemoteMonitoringSystemServiceProxy);
                    break;
                }
                case Signal::kHvBattTempData:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received kHvBattTempData info.");
                    return_value = IpcbIClient::GetRemoteMonitoringSystemService().registerRemoteMonitoringSystemService((fgen)RemoteMonitoringSystemServiceProxy);
                    break;
                }
                case Signal::kHvBattCod:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received kHvBattCod info.");
                    return_value = IpcbIClient::GetRemoteMonitoringSystemService().registerRemoteMonitoringSystemService((fgen)RemoteMonitoringSystemServiceProxy);
                    break;
                }
                // DownloadAgent
                case Signal::kOTAAssignmentNotification:
                case Signal::kOTAQueryAvailableStorageResp:
                case Signal::kOTAAssignmentSyncReq:
                case Signal::kOTAHMILanguageSettingsNotification:
                case Signal::kDownloadConsentNotification:
                case Signal::kOTAInstallationSummaryNotification:
                case Signal::kInstallationConsentNotification:
                case Signal::kOTAAssignBOOTResp:
                case Signal::kOTAWriteAssignmentDataResp:
                case Signal::kOTAExceptionReportNotification:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received kOTAQueryAvailableStorageResp info.");
                    return_value = IpcbIClient::GetOTAService().registerOTAService((fgen)OTAServiceProxy);
                    break;
                }
                case Signal::kRvdcSignal:
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received kRvdcSignal info.");
                    return_value = IpcbIClient::GetRVDCService().registerRVDCService((fgen)RVDCServiceProxy);
                    break;
                }
                default:
                {
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::RegisterSignal, received unhandeled signal: %u", \
                        static_cast<unsigned int>(signal_type));
                    return_value = false;
                }
            }  // switch
            if (return_value)
            {
                // add to active_subscriptions_ with reference counter = 1
                active_subscriptions_[signal_type] = 1;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,\
                    "IpCommandBrokerSignalSource register callback for signal %u failed",
                    static_cast<unsigned int>(signal_type));
            }
        }
        else
        {
            // if in list then increment reference counter only
            active_subscriptions_[signal_type] ++;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
            "IpCommandBrokerSignalSource::RegisterSignal failed, not initialised");
        return_value = false;
    }
    return return_value;
}


bool IpCommandBrokerSignalSource::UnregisterSignals(std::vector<Signal::SignalType> requested_unsubscriptions)
{
    bool return_value = true;
    bool temp_return_value = true;

    if (broker_initialized_)
    {
        for (Signal::SignalType &signal_type : requested_unsubscriptions)
        {
            temp_return_value = UnregisterSignal(signal_type, false);
            return_value = temp_return_value && return_value;  // keep false return_value
        }  // for
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
            "IpCommandBrokerSignalSource::UnregisterSignals failed, not initialised");
        return_value = false;
    }
    return return_value;
}


bool IpCommandBrokerSignalSource::UnregisterSignal(Signal::SignalType signal_type, bool forced)
{
    bool return_value = true;

    std::lock_guard<std::mutex> lock(subscriptions_mutex_);  // guard against concurrent (un)registrations
    if (broker_initialized_)
    {
        if ( active_subscriptions_.find(signal_type) != active_subscriptions_.end() )
        {  // only if signal is already registered
            if ((active_subscriptions_[signal_type] > 1) && (!forced) )  // if forced: deregister regardless of ref counter
            {   // decrement reference counter only
                active_subscriptions_[signal_type] --;
            }
            else
            {  // reference counter was 1 or 0 (which should never happen): deregister signal
                switch (signal_type)
                {
                    case Signal::kPrivacyNotificationSignal:
                    {
                        return_value = IpcbIClient::GetPrivacyService().DeregisterPrivacyNotificationCb();
                        break;
                    }
                    case Signal::kDeadReckonedPositionSignal:
                    {
                        return_value = IpcbIClient::GetGNSSService().DeregisterDeadReckonedPositionResponseCb();
                        break;
                    }
                    case Signal::kGNSSPositionDataSignal:
                    {
                        return_value = IpcbIClient::GetGNSSService().DeregisterGNSSPositionDataResponseCb();
                        break;
                    }
                    case Signal::kTheftDataSignal:
                    {
                        IpcbIClient::GetCustomerInfoMessagingService().deregisterCustomerInfoMessagingService();
                        return_value = true;
                        break;
                    }

                    default:
                    {
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,\
                            "IpCommandBrokerSignalSource::UnregisterSignal received unhandeled signal: %u", \
                            static_cast<unsigned int>(signal_type));
                        return_value = false;
                    }
                }  // switch
                if (return_value)
                {   // unregister succeeded, now remove from list:
                    active_subscriptions_.erase(signal_type);
                }
                else
                {   // unregister failed
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,\
                        "IpCommandBrokerSignalSource deregister callback for signal %u failed",
                        static_cast<unsigned int>(signal_type));
                }
            }  // if have to deregister
        }  // if in list
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,\
                "IpCommandBrokerSignalSource::UnregisterSignal, signal %u not registered, ignored.", \
                static_cast<unsigned int>(signal_type));
            return_value = true;  // don't return error, this could happen at parallel requests
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
           "IpCommandBrokerSignalSource::UnregisterSignals failed, not initialised");
        return_value = false;
    }

    return return_value;
}


void IpCommandBrokerSignalSource::PrivacyNotificationCb(PrivacyNotificationType *notification_data,
                                                        uint64_t request_id)
{
    IpCommandBrokerTransactionId transaction_id(request_id);
    Signal::SignalType signal_type = Signal::kPrivacyNotificationSignal;

    std::shared_ptr<Signal> signal =
        PrivacyNotificationSignal::CreateIpCommandBrokerSignal(*notification_data,
                                                                transaction_id,
                                                                signal_type);
    BroadcastSignal(signal);
}


void IpCommandBrokerSignalSource::DeadReckonedPositionResponseCb(DeadReckonedPositionResponse *data,\
    uint64_t request_id)
{
    IpCommandBrokerTransactionId transaction_id(request_id);
    Signal::SignalType signal_type = Signal::kDeadReckonedPositionSignal;

    std::shared_ptr<Signal> signal =
        DeadReckonedPositionSignal::CreateIpCommandBrokerSignal(data->position,
                                                                transaction_id,
                                                                signal_type);
    BroadcastSignal(signal);
}


void IpCommandBrokerSignalSource::GNSSPositionDataResponseCb(GNSSPositionDataResponse *data, uint64_t request_id)
{
    IpCommandBrokerTransactionId transaction_id(request_id);
    Signal::SignalType signal_type = Signal::kGNSSPositionDataSignal;

    std::shared_ptr<Signal> signal =
        GNSSPositionDataSignal::CreateIpCommandBrokerSignal(data->gnssdata,
                                                            transaction_id,
                                                            signal_type);
    BroadcastSignal(signal);
}

void IpCommandBrokerSignalSource::THEFTNoticeNotificationCb(TheftNoticeData theft)
{
    IpCommandBrokerTransactionId transaction_id;
    Signal::SignalType signal_type = Signal::kTheftDataSignal;

    std::shared_ptr<Signal> signal =
        THEFTNotificationSignal::CreateIpCommandBrokerSignal(theft,
                                                            transaction_id,
                                                            signal_type);
    BroadcastSignal(signal);

}

void IpCommandBrokerSignalSource::TelmDshbNotificationCb(OpTelmDshb_NotificationCyclic_Data TelmDshb_data)
{
    IpCommandBrokerTransactionId transaction_id;
    Signal::SignalType signal_type = Signal::kTelmDshb;

    std::shared_ptr<Signal> signal =
        TelmDshbSiganl::CreateIpCommandBrokerSignal(TelmDshb_data, transaction_id, signal_type);
    BroadcastSignal(signal);
}

void IpCommandBrokerSignalSource::RMSNotificationCb(RemoteMonitoringInfo_t  *rms_data)
{
    IpCommandBrokerTransactionId transaction_id;
    Signal::SignalType signal_type = Signal::kRMSDataSignal;

    std::shared_ptr<Signal> signal =
        RMSNotificationSignal::CreateIpCommandBrokerSignal(*rms_data,
                                                            transaction_id,
                                                            signal_type);
    BroadcastSignal(signal);
}

void IpCommandBrokerSignalSource::HvBattVoltageCb(HvBattVoltageData hbv)
{
    IpCommandBrokerTransactionId transaction_id;
    Signal::SignalType signal_type = Signal::kHvBattVoltageSignal;

    std::shared_ptr<Signal> signal =
        HvBattVoltageSignal::CreateIpCommandBrokerSignal(hbv,
                                                            transaction_id,
                                                            signal_type);
    BroadcastSignal(signal);
}

void IpCommandBrokerSignalSource::HvBattTempCb(HvBattTempData hbt)
{
    IpCommandBrokerTransactionId transaction_id;
    Signal::SignalType signal_type = Signal::kHvBattTempSignal;

    std::shared_ptr<Signal> signal =
        HvBattTempSignal::CreateIpCommandBrokerSignal(hbt,
                                                            transaction_id,
                                                            signal_type);
    BroadcastSignal(signal);
}

void IpCommandBrokerSignalSource::VehSoHCheckResponseCb(VehSoHCheckData data)
{
    IpCommandBrokerTransactionId transaction_id;
    Signal::SignalType signal_type = Signal::kVehSoHCheckResponse;

    std::shared_ptr<Signal> signal =
        VehSoHCheckSignal::CreateIpCommandBrokerSignal(data, transaction_id, signal_type);
    BroadcastSignal(signal);
}

void IpCommandBrokerSignalSource::VehSoHCheckNotifyCb(VehSoHCheckData data)
{
    IpCommandBrokerTransactionId transaction_id;
    Signal::SignalType signal_type = Signal::kVehSoHCheckNotify;

    std::shared_ptr<Signal> signal =
        VehSoHCheckSignal::CreateIpCommandBrokerSignal(data, transaction_id, signal_type);
    BroadcastSignal(signal);
}

bool IpCommandBrokerSignalSource::VehicleSoHCheckRequest(uint64_t request_id)
{
    bool return_value = false;

     DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,\
                        "IpCommandBrokerSignalSource::VehicleSoHCheckRequest, request_id = %ld", request_id);
    
    return_value = IpcbIClient::GetTelematicsService().VehicleSoHCheckResponseSetRequest(request_id);
    return return_value;
}

} // namespace fsm
/** \}    end of addtogroup */
