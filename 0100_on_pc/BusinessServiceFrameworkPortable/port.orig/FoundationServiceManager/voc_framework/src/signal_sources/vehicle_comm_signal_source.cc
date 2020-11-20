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
 *  \file     vehicle_comm_signal_source.cc
 *  \brief    Vehicle Comm signal source class.
 *  \author   Florian Schindler
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */


#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "vehiclecomm/vehicle_comm_interface.hpp"
#include "voc_framework/features/feature.h"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "voc_framework/signals/signal.h"
#include "dlt/dlt.h"
#include <memory>  // std::shared_ptr

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

VehicleCommSignalSource::VehicleCommSignalSource() : ivc_(vc::IVehicleComm::GetInstance())
{
    vc::ReturnValue vcretval = ivc_.Init(std::bind(&VehicleCommSignalSource::ResponseCallback, this, std::placeholders::_1, std::placeholders::_2),
                                    std::bind(&VehicleCommSignalSource::EventCallback, this, std::placeholders::_1));
    if (vcretval != vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "VehicleCommClient::Init failed: %s\n", \
            vc::ReturnValueStr[vcretval]);
    }
    vcretval = ivc_.EnableGeneralEvents();
    if (vcretval != vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "VehicleCommClient::Enable general events failed: %s\n", \
            vc::ReturnValueStr[vcretval]);
    }
}

VehicleCommSignalSource::~VehicleCommSignalSource()
{
}


VehicleCommSignalSource& VehicleCommSignalSource::GetInstance()
{
    static VehicleCommSignalSource instance;
    return instance;
}


vc::IVehicleComm& VehicleCommSignalSource::GetVehicleCommClientObject()
{
  return ivc_;
}


std::shared_ptr<Signal> VehicleCommSignalSource::VehicleCommSignalFactory(vc::MessageBase* vcmessagebase, vc::ReturnValue status)
{
    std::shared_ptr<Signal> signal = nullptr;
    if (vcmessagebase != NULL)
    {
        switch (vcmessagebase->type_)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,"VehicleCommSignal type=%d id=%d",vcmessagebase->type_,vcmessagebase->id_);
            
            case vc::MESSAGE_RESPONSE:
            {
                VehicleCommTransactionId transaction_id(vcmessagebase->session_id_);  // use provided (mirrored) transaction_id

//ToDo: add ResCodeConfirmationByUserNotify and ResRoleSelectedNotify with proper data of VehicleComm (when available)
                switch (static_cast<vc::ResponseID>(vcmessagebase->id_))
                {
                    case vc::RES_CARMODE:
                    {
                        vc::Message<vc::EventCarMode> *m = dynamic_cast<vc::Message<vc::EventCarMode>*>(vcmessagebase);
                        CarModeSignal* sig = new CarModeSignal(Signal::kCarMode, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::RES_HORNNLIGHT:
                    {
                        vc::Message<vc::ResHornNLight> *m = dynamic_cast<vc::Message<vc::ResHornNLight>*>(vcmessagebase);
                        HornNLightSignal* sig = new HornNLightSignal(Signal::kHornNLight, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::RES_CARUSAGEMODE:
                    {
                        vc::Message<vc::ResCarUsageMode> *m = dynamic_cast<vc::Message<vc::ResCarUsageMode>*>(vcmessagebase);
                        CarUsageModeSignal* sig = new CarUsageModeSignal(Signal::kCarUsageMode, transaction_id,\
                            m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::RES_GETCARCFGSTOLENVEHICLETRACKING:
                    {
                        vc::Message<vc::EventStolenVehicleTracking> *m = dynamic_cast<vc::Message<vc::EventStolenVehicleTracking>*>(vcmessagebase);
                        StolenVehicleTrackingSignal* sig = new StolenVehicleTrackingSignal(Signal::kStolenVehicleTracking, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
#if 1 // nieyj
                    case vc::RES_VFCACTIVATE:
                    {
                        vc::Message<vc::ResVFCActivate> *m = dynamic_cast<vc::Message<vc::ResVFCActivate>*>(vcmessagebase);
                        VFCActivateSignal* sig = new VFCActivateSignal(Signal::kVFCActivateSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
#endif
                    //uia93888 remote start engine
                    case vc::RES_RMTSTATUS:
                    {
                        vc::Message<vc::ResRMTStatus> *m = dynamic_cast<vc::Message<vc::ResRMTStatus>*>(vcmessagebase);
                        RMTCarStatesSignal* sig = new RMTCarStatesSignal(Signal::kRMTCarStatesSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::RES_RMTENGINE:
                    {
                        vc::Message<vc::Res_RMTEngine> *m = dynamic_cast<vc::Message<vc::Res_RMTEngine>*>(vcmessagebase);
                        RMTEngineSignal* sig = new RMTEngineSignal(Signal::kRMTEngineSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::RES_VINNUMBER:
                    {
                        vc::Message<vc::ResVINNumber> *m = dynamic_cast<vc::Message<vc::ResVINNumber>*>(vcmessagebase);
                        VINNumberSignal* sig = new VINNumberSignal(Signal::kVINNumberSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::RES_GETTCAMHWVERSION:
                    {
                        vc::Message<vc::ResGetTcamHwVer> *m = dynamic_cast<vc::Message<vc::ResGetTcamHwVer>*>(vcmessagebase);
                        TcamHwVerSignal* sig = new TcamHwVerSignal(Signal::kTcamHwVerSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::RES_ELECTENGLVL:
                    {
                        vc::Message<vc::ResElectEngLvl> * m = dynamic_cast<vc::Message<vc::ResElectEngLvl>*>(vcmessagebase);
                        ElectEngLvlSignal* sig = new ElectEngLvlSignal(Signal::kElectEngLvlSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    default:
                    {
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "VehicleCommSignal::VehicleCommSignalFactory unhandled ResponseId: %i ",\
                         static_cast<vc::ResponseID>(vcmessagebase->id_));
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "VehicleCommSignal::VehicleCommSignalFactory unhandled response_id: %i ",\
                            static_cast<vc::ResponseID>(vcmessagebase->id_));
                        break;
                    }
                }
                break;
            }
            case vc::MESSAGE_EVENT:
            {
                VehicleCommTransactionId transaction_id;  // here automatically generated
                switch (static_cast<vc::EventID>(vcmessagebase->id_))
                {
                    case vc::EVENT_ENDOFTRIP:
                    {
                        vc::Message<vc::Empty> *m = dynamic_cast<vc::Message<vc::Empty>*>(vcmessagebase);
                        EndOfTripSignal* sig = new EndOfTripSignal(Signal::kEndOfTrip, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
#if 1 // nieyj
                    case vc::EVENT_DOORLOCKSTATE:
                    {
                        vc::Message<vc::EventDoorLockState> *m = dynamic_cast<vc::Message<vc::EventDoorLockState>*>(vcmessagebase);
                        DoorLockStatusSignal* sig = new DoorLockStatusSignal(Signal::kDoorLockStatusSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_WINOPENSTATE:
                    {
                        vc::Message<vc::EventWinOpenState> *m = dynamic_cast<vc::Message<vc::EventWinOpenState>*>(vcmessagebase);
                        WinOpenStatusSignal* sig = new WinOpenStatusSignal(Signal::kWinOpenStatusSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_PMLVL:
                    {
                        vc::Message<vc::EventPMLevelState> *m = dynamic_cast<vc::Message<vc::EventPMLevelState>*>(vcmessagebase);
                        PMLvlStatusSignal* sig = new PMLvlStatusSignal(Signal::kPMLvlStatusSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
#endif
                    case vc::EVENT_CARUSAGEMODE:
                    {
                        vc::Message<vc::EventCarUsageMode> *m = dynamic_cast<vc::Message<vc::EventCarUsageMode>*>(vcmessagebase);
                        CarUsageModeSignal* sig = new CarUsageModeSignal(Signal::kCarUsageMode, transaction_id,\
                            m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    //uia93888 remote start engine
                    case vc::EVENT_RMTENGINESECURITYRANDOM:{
                        vc::Message<vc::Event_RMTEngineSecurityRandom> *m = dynamic_cast<vc::Message<vc::Event_RMTEngineSecurityRandom>*>(vcmessagebase);
                        RMTEngineSecurityRandomSignal* sig = new RMTEngineSecurityRandomSignal(Signal::kRMTEngineSecurityRandom, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_RMTENGINESECURITYRESULT:{
                        vc::Message<vc::Event_RMTEngineSecurityResult> *m = dynamic_cast<vc::Message<vc::Event_RMTEngineSecurityResult>*>(vcmessagebase);
                        RMTEngineSercurityResultSignal* sig = new RMTEngineSercurityResultSignal(Signal::kRMTEngineSecurityResult, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_RMTENGINE:
                    {
                        vc::Message<vc::Event_RMTEngine> *m = dynamic_cast<vc::Message<vc::Event_RMTEngine>*>(vcmessagebase);
                        RMTEngineSignal* sig = new RMTEngineSignal(Signal::kRMTEngineSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_DELAYENGINERUNNINGTIME:
                    {
                        vc::Message<vc::Res_DelayEngRunngTime> * m = dynamic_cast<vc::Message<vc::Res_DelayEngRunngTime>*>(vcmessagebase);
                        DelayEngRunngTimeSignal* sig = new DelayEngRunngTimeSignal(Signal::kDelayEngineRinningtimeSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_OPERATEREMOTECLIMATE:
                    {
                        vc::Message<vc::Res_OperateRMTClimate> * m = dynamic_cast<vc::Message<vc::Res_OperateRMTClimate>*>(vcmessagebase);
                        OperateRMTClimateSignal* sig = new OperateRMTClimateSignal(Signal::kRemoteCliamteSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_PARKINGCLIMATEOPER:{
                        vc::Message<vc::Res_ParkingClimateOper_S> * m = dynamic_cast<vc::Message<vc::Res_ParkingClimateOper_S>*>(vcmessagebase);
                        ParkingClimateOperSignal* sig = new ParkingClimateOperSignal(Signal::kParkingClimateOperSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_OPERATERMTSEATHEAT:{
                        vc::Message<vc::Res_OperateRMTSeatHeat_S> * m = dynamic_cast<vc::Message<vc::Res_OperateRMTSeatHeat_S>*>(vcmessagebase);
                        OperateRMTSeatHeatSignal* sig = new OperateRMTSeatHeatSignal(Signal::kRemoteSeatHeatSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_OPERATERMTSEATVENTILATION:{
                        vc::Message<vc::Res_OperateRMTSeatVenti_S> * m = dynamic_cast<vc::Message<vc::Res_OperateRMTSeatVenti_S>*>(vcmessagebase);
                        OperateRMTSeatVentiSignal* sig = new OperateRMTSeatVentiSignal(Signal::kRemoteSeatVentiSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    case vc::EVENT_OPERATERMTSTEERWHLHEAT:{
                        vc::Message<vc::Res_OperateRMTSteerWhlHeat_S> * m = dynamic_cast<vc::Message<vc::Res_OperateRMTSteerWhlHeat_S>*>(vcmessagebase);
                        OperateRMTSteerWhlHeatSignal* sig = new OperateRMTSteerWhlHeatSignal(Signal::kRemoteSteerWhlHeatSignal, transaction_id, m->message, status);
                        signal = std::shared_ptr<Signal>(sig);
                        break;
                    }
                    //uia93888 end
                    default:
                    {
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "VehicleCommSignal::VehicleCommSignalFactory unhandled EventId: %i ",\
                          static_cast<vc::EventID>(vcmessagebase->id_));
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "VehicleCommSignal::VehicleCommSignalFactory unhandled event_id: %i ",\
                            static_cast<vc::EventID>(vcmessagebase->id_));
                        break;
                    }
                }
                break;
            }
            default:
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "VehicleCommSignal::VehicleCommSignalFactory unhandled type_: %i ",\
                    static_cast<int>(vcmessagebase->type_));
                break;
            }
        }  // switch (vcmessagebase->type_)
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "VehicleCommSignal::VehicleCommSignalFactory vcmessagebase == NULL");
        // nevertheless forward empty pointer and error code to feature
    }
    return signal;
}


void VehicleCommSignalSource::ResponseCallback(vc::MessageBase* vcmessagebase, vc::ReturnValue status)
{
    if (status != vc::RET_OK)
    {
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "VehicleCommSignal::ResponseCallback response failed.\n");
    }
    std::shared_ptr<Signal> signal = VehicleCommSignalFactory(vcmessagebase, status);
    if (signal)
    {
        BroadcastSignal(signal);
    }

    // in general the original message must be deleted by callee, here it is degated to the shared_ptr.
}


void VehicleCommSignalSource::EventCallback(vc::MessageBase* vcmessagebase)
{
    ResponseCallback(vcmessagebase, vc::RET_OK);
}


}  // namespace fsm
/** \}    end of addtogroup */
