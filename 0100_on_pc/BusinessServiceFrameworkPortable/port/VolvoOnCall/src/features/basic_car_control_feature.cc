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
 *  \file     basic_car_control_feature.cc
 *  \brief    VOC Service BasicCarControl feature.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */



#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/transactions/transaction_id.h"

#include "features/basic_car_control_feature.h"
#include "signals/basic_car_control_signal.h"
#include "signals/bcc_cl_001_signal.h"
#include "signals/bcc_cl_010_signal.h"
//#include "signals/geely/gly_vds_svt_signal.h"
#include "signals/geely/gly_vds_svt_signal.h"

#include "signals/geely/gly_vds_res_signal.h"
//#include "signals/geely/gly_vds_rdl_signal.h"
#include "signals/geely/gly_vds_rvs_signal.h"
#include "signals/geely/gly_vds_mcu_signal.h"
#include "signals/geely/gly_vds_rvdc_signal.h"
#include "signals/geely/gly_vds_rmc_signal.h"
#include "transactions/car_locator_advert_transaction.h"
#include "svt/svt.h"
#include "remote_ctrl/remote_ctrl_door_tailgate.h"
#include "remote_ctrl/remote_ctrl_flash_horn.h"
#include "remote_ctrl/remote_ctrl_pm25_enquire.h"
#include "remote_ctrl/remote_ctrl_window_roof.h"
#include "theft/theft_notification_transaction.h"

#ifndef VOC_TESTS
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "transactions/car_locator_map_request_transaction.h"
#endif

#include "voc_framework/signal_sources/internal_signal_source.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"

#include "voc_framework/signals/vdmsg_signal_types.h"

#include "signals/vdmsg_test_signal.h"

#include <fstream>
#include <thread>
#include <ios>

#include "dlt/dlt.h"

#include "asn.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string kBasicCarControlFeatureName = "BasicCarControl";


BasicCarControlFeature::BasicCarControlFeature() : fsm::Feature(kBasicCarControlFeatureName),
                                                            m_appsLcm_(AppsLcm::GetInstance())
{
    //TODO: We will not get the cloud resource until 707, for now enable everything by default
    //and register with the signal source immidiatly
#ifndef VOC_TESTS
    car_locator_position_update = std::make_shared<CarLocatorPositionUpdateTransaction>();
#endif
    mRmsTransaction = std::make_shared<RmsTransaction>();

    //register signals with the signal factory
    fsm::SignalFactory::RegisterCcmFactory(BccCl001Signal::oid, BccCl001Signal::CreateBccCl001Signal);
    fsm::SignalFactory::RegisterCcmFactory(BccCl010Signal::oid, BccCl010Signal::CreateBccCl010Signal);
    fsm::SignalFactory::RegisterCcmFactory(BasicCarControlSignal::oid, BasicCarControlSignal::CreateBasicCarControlSignal);

    //svt module
    //fsm::SignalFactory::RegisterVdmsgFactory(fsm::kVDServiceSvt, GlyVdsSvtSignal::CreateGlyVdsSvtSignal);

    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceSvt,
                                            GlyVdsSvtSignal::CreateGlyVdsSvtSignal);

    //fsm::SignalFactory::RegisterVdmsgFactory(fsm::kVDServiceSvt, GlyVdsSvtSignal::CreateGlyVdsSvtSignal);

    //res(remote start) module
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceRes,
                                            GlyVdsResSignal::CreateGlyVdsResSignal);
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceRce,
                                            GlyVdsRceSignal::CreateGlyVdsRceSignal);

    //mcu(configuration update) module
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceMcu, GlyVdsMcuSignal::CreateGlyVdsMcuSignal);

    //rvs(remote vehicle status) module
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceRvs, GlyVdsRvsSignal::CreateGlyVdsRvsSignal);

    //fota module
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceFota, GlyVdsDLAgentSignal::CreateGlyVdsDLAgentSignal);

    //remote ctrl module
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceRdl, GlyVdsRmcSignal::CreateGlyVdsRDLSignal);
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceRdu, GlyVdsRmcSignal::CreateGlyVdsRDUSignal);
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceRws, GlyVdsRmcSignal::CreateGlyVdsRWSSignal);
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceRpp, GlyVdsRmcSignal::CreateGlyVdsRPPSignal);
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceRhl, GlyVdsRmcSignal::CreateGlyVdsRHLSignal);

    //rvdc module
    fsm::SignalFactory::Register_CreateSignalFunction(fsm::kVDServiceRvc, GlyVdsRvdcSignal::CreateGlyVdsRvdcSignal);

    // Register to VocmoSignalSource to receive signals from MQTT and BLE
    vocmo_signal_source_.RegisterSignalReceiver(*this);

    // Register to InternalSignalSource to receive internal signals
    fsm::InternalSignalSource& internal_signal_source = fsm::InternalSignalSource::GetInstance();
    internal_signal_source.RegisterSignalReceiver(*this);

#ifndef VOC_TESTS
    fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
    vc_signal_source.AddFeature(*this);

    fsm::VehicleCommTransactionId vehicle_comm_request_id;
    vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
    igen.Request_CarUsageMode(vehicle_comm_request_id.GetId());

    // Subscribe for positioning signals
    fsm::IpCommandBrokerSignalSource& ipcb_signal_source = fsm::IpCommandBrokerSignalSource::GetInstance();

    ipcb_signal_source.RegisterSignal(fsm::Signal::kDeadReckonedPositionSignal);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kGNSSPositionDataSignal);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kTheftDataSignal);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kTelmDshb);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kRemoteMonitoringInfo);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kHvBattVoltageData);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kHvBattTempData);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kHvBattCod);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kVehSoHCheckResponse);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kVehSoHCheckNotify);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kOTAAssignmentNotification);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kOTAQueryAvailableStorageResp);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kOTAAssignmentSyncReq);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kOTAHMILanguageSettingsNotification);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kDownloadConsentNotification);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kInstallationConsentNotification);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kOTAAssignBOOTResp);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kOTAWriteAssignmentDataResp);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kOTAExceptionReportNotification);
    ipcb_signal_source.RegisterSignal(fsm::Signal::kRvdcSignal);

    ipcb_signal_source.AddFeature(*this);

    AddActiveTransactionIfUnique(car_locator_position_update);

    AddActiveTransactionIfUnique(mRmsTransaction);

    //TODO: until here
#endif

    // Subscribe for positioning signals
    fsm::VpomSignalSource& vpom_signal_source = fsm::VpomSignalSource::GetInstance();
    vpom_signal_source.RegisterSignal(fsm::Signal::kGNSSPositionDataSignal);
    vpom_signal_source.AddFeature(*this);

    /*std::shared_ptr<fsm::Signal> svtReStartSignal = std::dynamic_pointer_cast<volvo_on_call::SvtReStartSignal>();
    car_svt_request = std::make_shared<CarSvtRequestTransaction>(svtReStartSignal);
    car_svt_request->SetReStart();
    AddActiveTransaction(car_svt_request);*/

    /* JL need receive signal kCarUsageMode/kTelmRoadTrip/kTelmDshb/kGNSSPositionDataSignal/kEndOfTrip */
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: create journeylog state machine", __FUNCTION__);
    std::shared_ptr<CarJourneyLogTransaction> car_JL_log = std::make_shared<CarJourneyLogTransaction>();
    AddActiveTransaction(car_JL_log);

    /* DldAgt transaction */
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: create DownloadAgent transaction", __FUNCTION__);
    std::shared_ptr<CarDownloadAgentTransaction> carDldAgtTransaction = std::make_shared<CarDownloadAgentTransaction>();
    AddActiveTransaction(carDldAgtTransaction);

    /*RVS thread need init OK, for receive kTelmDshb signal*/
    if(rvs == 0 || !(rvs->IsActive()))
    {
        rvs = std::make_shared<Rvs>();
        AddActiveTransaction(rvs);
    }

    if(m_pRvdc == 0 || !(m_pRvdc->IsActive()))
    {
        m_pRvdc = std::make_shared<Rvdc>();
        AddActiveTransaction(m_pRvdc);
    }

    bool ret = m_appsLcm_.Init([this](LCMShutdownType sig){return BasicCarControlFeature::ShutdownNotification(sig);},
                    [this](std::shared_ptr<fsm::Signal> signal){return BasicCarControlFeature::HandleSignal(signal);});
    if (!ret) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature: %s: Failed to init life applications cycle manager!",
                __FUNCTION__);
    }
    // register with signal sources
    //TODO: removed as we will not have bcc cloud unitl 707

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s(): end", __FUNCTION__);

}


void BasicCarControlFeature::HandleSignal(std::shared_ptr<fsm::Signal> signal)
{
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                       "BasicCarControlFeature - Incoming signal");

        if (signal == nullptr)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                       "signal is null, %s", __FUNCTION__);
            return;
        }

    switch (signal->GetSignalType())
    {
        case fsm::kVDServiceSvt:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "kVDServiceSvt, receive svt signal.");
            if(m_svt == 0 || !(m_svt->IsActive()))
            {
                m_svt = std::make_shared<Svt>();
                AddActiveTransaction(m_svt);
            }

            m_svt->MapSvtTransactionId(signal);
            m_svt->EnqueueSignal(signal);

            break;
        }
        case fsm::kVDServiceRvs:
        case fsm::Signal::kVehSoHCheckResponse:
        case fsm::Signal::kVehSoHCheckNotify:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "kVDServiceRvs, receive RVS signal.");
            /*
            if(rvs == 0 || !(rvs->IsActive()))
            {
                rvs = std::make_shared<Rvs>();
                AddActiveTransaction(rvs);
            }
            */
            rvs->MapRvsTransactionId(signal);
            rvs->EnqueueSignal(signal);
            break;
        }
        case fsm::kVDServiceRvc:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "kVDServiceRvc, receive RVDC signal.");

            m_pRvdc->MapRvdcTransactionId(signal);
            m_pRvdc->EnqueueSignal(signal);
            break;
        }
        case fsm::kVDServiceRdl:
        case fsm::kVDServiceRdu:
        case fsm::kVDServiceRtl:
        case fsm::kVDServiceRtu:
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "remote door control signal: %s", signal->ToString().c_str());

            std::shared_ptr<RemoteCtrlDoorTail> remote_ctrl_door_tailgate =
                std::make_shared<RemoteCtrlDoorTail>(signal);
            AddActiveTransaction(remote_ctrl_door_tailgate);
            remote_ctrl_door_tailgate->EnqueueSignal(signal);
            break;
        }
        case fsm::kVDServiceRhl:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "kVDServiceRhl");

            std::shared_ptr<RemoteCtrlFlashHorn> remote_ctrl_flash_horn =
                    std::make_shared<RemoteCtrlFlashHorn>(signal);
            AddActiveTransaction(remote_ctrl_flash_horn);
            remote_ctrl_flash_horn->EnqueueSignal(signal);
            break;
        }
        case fsm::kVDServiceRws:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "kVDServiceRws");

            std::shared_ptr<RemoteCtrlWinRoof> remote_ctrl_win =
                    std::make_shared<RemoteCtrlWinRoof>(signal);
            AddActiveTransaction(remote_ctrl_win);
            remote_ctrl_win->EnqueueSignal(signal);
            break;
        }
        case fsm::kVDServiceRpp:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "kVDServiceRpp");

            std::shared_ptr<RemoteCtrlPM25Enquire> remote_ctrl_rpp =
                    std::make_shared<RemoteCtrlPM25Enquire>(signal);
            AddActiveTransaction(remote_ctrl_rpp);
            remote_ctrl_rpp->EnqueueSignal(signal);
            break;
        }
        case fsm::Signal::kTheftDataSignal:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                       "theft: kTheftDataSignal, receive theft signal");
            if(theft_notify !=0 && theft_notify->IsActive())
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "theft second signal.");
                theft_notify->MapTheftTransactionId(signal);
                theft_notify->EnqueueSignal(signal);
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "theft first signal.");
                theft_notify = std::make_shared<TheftNotificationTransaction>(signal);
                AddActiveTransaction(theft_notify);
            }
            break;
        }
        case VocInternalSignalTypes::kAppStartEngineRequestSignal:
        case fsm::kVDServiceRes:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "enter BasicCarControlFeature::HandleSignal, RMT_Engine");
            if(m_pRes == 0 || !(m_pRes->IsActive()))
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                       "the RMT_Engine has not been exist!");
                m_pRes.reset(new RMT_Engine());
                m_pRes->Start( [this](std::shared_ptr<fsm::Signal> signal){return BasicCarControlFeature::HandleSignal(signal);} );
                AddActiveTransaction(m_pRes);
                m_pRes->CreateThreadSuccessFul();
            }
            m_pRes->AddTransactionIdToMap(signal);
            m_pRes->EnqueueSignal(signal);
            break;
            }
        case fsm::kVDServiceRce:{
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce.", __FUNCTION__);
            //_req_signal.reset();
            std::shared_ptr<GlyVdsRceSignal> _req_signal; // = std::dynamic_pointer_cast<GlyVdsRceSignal>(signal);
            _req_signal = std::dynamic_pointer_cast<GlyVdsRceSignal>(signal);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce........", __FUNCTION__);
            switch(_req_signal->GetServiceType()){
                case RMT_Climatization:{ //operate remote climate
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce. RemoteCliamtization", __FUNCTION__);
                    if (nullptr == m_pRMT_Climate || !(m_pRMT_Climate->IsActive())){
                        m_pRMT_Climate.reset(new RMT_Climate());
                        m_pRMT_Climate->BasicInit( [this](std::shared_ptr<fsm::Signal> signal){return BasicCarControlFeature::HandleSignal(signal);} );
                        AddActiveTransaction(m_pRMT_Climate);
                    }
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce. Get Object OK, current state: %d",
                                    __FUNCTION__, m_pRMT_Climate->GetState());
                    m_pRMT_Climate->AddTransactionIdToMap(_req_signal);
                    m_pRMT_Climate->EnqueueSignal(_req_signal);
                    break;
                    }
                case RMT_SEATHEAT:{ //operate remote seat heating
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce. RemoteSeatHeat", __FUNCTION__);
                    if (nullptr == m_pRMT_SeatHeat || !(m_pRMT_SeatHeat->IsActive())){
                        m_pRMT_SeatHeat.reset(new RMT_SeatHeat());
                        m_pRMT_SeatHeat->BasicInit( [this](std::shared_ptr<fsm::Signal> signal){return BasicCarControlFeature::HandleSignal(signal);} );
                        AddActiveTransaction(m_pRMT_SeatHeat);
                    }
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce. Get Object OK, current state: %d",
                                    __FUNCTION__, m_pRMT_SeatHeat->GetState());
                    m_pRMT_SeatHeat->AddTransactionIdToMap(_req_signal);
                    m_pRMT_SeatHeat->EnqueueSignal(_req_signal);
                    break;
                    }
                case RMT_SEATVENTI:{ //operate seat ventilation
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce. RemoteSeatVentilation", __FUNCTION__);
                    if (nullptr == m_pRMT_SeatVenti || !(m_pRMT_SeatVenti->IsActive())){
                        m_pRMT_SeatVenti.reset(new RMT_SeatVenti());
                        m_pRMT_SeatVenti->BasicInit( [this](std::shared_ptr<fsm::Signal> signal){return BasicCarControlFeature::HandleSignal(signal);} );
                        AddActiveTransaction(m_pRMT_SeatVenti);
                    }
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce. Get Object OK, current state: %d",
                                    __FUNCTION__, m_pRMT_SeatVenti->GetState());
                    m_pRMT_SeatVenti->AddTransactionIdToMap(_req_signal);
                    m_pRMT_SeatVenti->EnqueueSignal(_req_signal);
                    break;
                }
                case RMT_STEERWHEELHEAT:{ //operate seat ventilation
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce. RemoteSteerWheelHeat", __FUNCTION__);
                    if (nullptr == m_pRMT_SteerwhlHeat || !(m_pRMT_SteerwhlHeat->IsActive())){
                        m_pRMT_SteerwhlHeat.reset(new RMT_SteerWhlHeat());
                        m_pRMT_SteerwhlHeat->BasicInit( [this](std::shared_ptr<fsm::Signal> signal){return BasicCarControlFeature::HandleSignal(signal);} );
                        AddActiveTransaction(m_pRMT_SteerwhlHeat);
                    }
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce. Get Object OK, current state: %d",
                                    __FUNCTION__, m_pRMT_SteerwhlHeat->GetState());
                    m_pRMT_SteerwhlHeat->AddTransactionIdToMap(_req_signal);
                    m_pRMT_SteerwhlHeat->EnqueueSignal(_req_signal);
                    break;
                }
                default:{ //UNKNOW Service Type
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature::%s, kVDServiceRce, UNKNOW service type: %d",
                                    __FUNCTION__, _req_signal->GetServiceType());
                    break;
                    }
                }
                break;
            }
        case fsm::kVDServiceMcu:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "received kVDServiceMcu signal.");

            std::shared_ptr<RemoteConfig> remote_config = std::make_shared<RemoteConfig>(signal);
            AddActiveTransaction(remote_config);

            break;
        }
#if 0
        case fsm::Signal::kCarUsageMode:
        {
            if (car_JL_log == nullptr)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: create journeylog", __FUNCTION__);
                car_JL_log = std::make_shared<CarJourneyLogTransaction>();
                AddActiveTransaction(car_JL_log);
            }
            car_JL_log->EnqueueSignal(signal);
            break;
        }
        case fsm::Signal::kTelmRoadTrip:
        case fsm::Signal::kTelmDshb:
        case fsm::Signal::kGNSSPositionDataSignal:
        case fsm::Signal::kEndOfTrip:
        {
            if (car_JL_log != nullptr)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: signal:%s", __FUNCTION__,signal->ToString().c_str());
                car_JL_log->EnqueueSignal(signal);
            }
            break;
        }
        case fsm::Signal::kTelmDshb:
        {
            if (mRmsTransaction != nullptr)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: signal:%s", __FUNCTION__,signal->ToString().c_str());
                mRmsTransaction->EnqueueSignal(signal);
            }
            break;
        }
#endif
        case fsm::Signal::kRemoteMonitoringInfo:
        case fsm::Signal::kHvBattVoltageData:
        case fsm::Signal::kHvBattTempData:
        case fsm::Signal::kHvBattCod:
        {
            if(mRmsTransaction != nullptr)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Receive RMS Upload signal:%s",signal->ToString().c_str());
                mRmsTransaction->EnqueueSignal(signal);
            }
            break;
        }
        default :
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Passing signal to active transactions, signal: %s",
                            signal->ToString().c_str());
            BroadcastToTransactions(signal);
            break;
        }
    }

}

void BasicCarControlFeature::ShutdownNotification(LCMShutdownType sig)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "BasicCarControlFeature: %s state: %s ", __FUNCTION__, LCMShutdownTypeStr[sig]);
}



} // namespace volvo_on_call
/** \}    end of addtogroup */
