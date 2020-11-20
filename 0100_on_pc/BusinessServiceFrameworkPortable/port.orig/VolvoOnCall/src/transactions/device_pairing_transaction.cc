/**
 * Copyright (C) 2017-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     device_pairing_transaction.cc
 *  \brief    Device pairing transaction.
 *  \author   Axel Fagerstedt, et al.
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "transactions/device_pairing_transaction.h"

#include "dlt/dlt.h"

#include "keystore.h"
#include "usermanager_interface.h"
#include "voc_framework/signals/internal_signal.h"
#include "local_config_interface.hpp"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "voc_framework/signal_sources/car_access_signal_source.h"
#include "voc_framework/signal_sources/internal_signal_source.h"

#ifndef VOC_TESTS
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#endif

#include "voc.h"
#include "signals/ca_cat_001_signal.h"
#include "signals/ca_cat_002_signal.h"
#include "signals/ca_dpa_001_signal.h"
#include "signals/ca_dpa_020_signal.h"
#include "signals/aes_ble_da_key_voc_frame.h"
#include "signals/aes_ca_voc_frame.h"
#include "signals/aes_car_actor_cert_voc_frame.h"
#include "signals/aes_cert_actor_voc_frame.h"
#include "signals/aes_cert_nearfield_voc_frame.h"
#include "signals/ecdhe_approval_voc_frame.h"
#include "signals/signal_types.h"

#include "transactions/role_selected_notification_transaction.h"


DLT_IMPORT_CONTEXT(dlt_voc);

#include <chrono>

namespace volvo_on_call
{

#define LOG_ON_RETURN_FALSE(return_value) if (!return_value) { DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SIT: left Activity provideDevicePairing"); }

#define SIT_LOG_RECEIVED \
    if (pairing_channel_ == PairingChannel::kBle) \
    { \
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: BLE received API_handlePairingFrame"); \
    } \
    else if (pairing_channel_ == PairingChannel::kVsdp) \
    { \
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: VSDP received FS_ReceiveMessage"); \
    }

#define SIT_LOG_SEND(VOC_FRAME_ID) \
    if (pairing_channel_ == PairingChannel::kBle) \
    { \
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "SIT: BLE send API_sendVOCbaseframe(%s)", VOC_FRAME_ID); \
    } \
    else if (pairing_channel_ == PairingChannel::kVsdp) \
    { \
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: VSDP send FS_SendMessage(CA-DPA-021,MQTT) CA-DPA-021"); \
    }


DevicePairingTransaction::DevicePairingTransaction(std::shared_ptr<fsm::Signal> initial_signal) :
    fsm::SmartTransaction(kNew)
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    SignalFunction handle_pairing_visability =
        std::bind(&DevicePairingTransaction::HandlePairingVisibilityRequest,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_ca_dpa_002 =
        std::bind(&DevicePairingTransaction::HandleCaDpa002,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_ca_dpa_020 =
        std::bind(&DevicePairingTransaction::HandleCaDpa020,
                  this,
                  std::placeholders::_1);

    // We bind kBlePairingServiceTransactionId_ to enable
    // DevicePairingTransaction::HandleVocFrame to distinguish
    // between frames from ble and frames from CA-DPA-020
    //TODO: this is kinda hacky, once we do signal refactoring
    //      put the Sourcfe of the Signal into Singal which
    //      will at that point be a decoupled header.
    SignalFunction handle_voc_frame =
        std::bind(&DevicePairingTransaction::HandleVocFrame,
                  this,
                  std::placeholders::_1,
                  kBlePairingServiceTransactionId_);

    SignalFunction handle_ble_connection =
        std::bind(&DevicePairingTransaction::HandleBleConnection,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_code_confirmation =
        std::bind(&DevicePairingTransaction::HandleCodeConfirmationByUser,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_timeout =
        std::bind(&DevicePairingTransaction::HandleTimeout,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_ca_cat_002 =
        std::bind(&DevicePairingTransaction::HandleCaCat002,
                  this,
                  std::placeholders::_1);

    StateFunction finalize_pairing =
        std::bind(&DevicePairingTransaction::FinalizePairing,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2);

    StateFunction done =
        std::bind(&DevicePairingTransaction::Done,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2);

    StateMap state_map =
    //  {{state,                     {state_handler,
    //                                {{signal_type, signal_handler}},
    //                                {legal states to transition to}}}}
        {{kNew,                      {nullptr,
                                      {{fsm::Signal::kPairingVisibilityRequest, handle_pairing_visability},
                                       {VocSignalTypes::kBleConnectionSignal, handle_ble_connection}},
                                      {kAdvertising}}},

         {kAdvertising,              {nullptr,
                                      {{VocSignalTypes::kCaDpa002Signal, handle_ca_dpa_002},
                                       {fsm::Signal::kTimeout, handle_timeout},
                                       {VocSignalTypes::kCaDpa020Signal, handle_ca_dpa_020},
                                       {VocSignalTypes::kBleConnectionSignal, handle_ble_connection},
                                       {VocSignalTypes::kEcdheRequestVocFrame, handle_voc_frame}},
                                      {kEcdheApproval, kDone}}},

         {kEcdheApproval,            {nullptr,
                                      {{fsm::Signal::kCodeConfirmationByUserNotify, handle_code_confirmation},
                                       {fsm::Signal::kTimeout, handle_timeout},
                                       {VocSignalTypes::kCaDpa020Signal, handle_ca_dpa_020},
                                       {VocSignalTypes::kBleConnectionSignal, handle_ble_connection},
                                       {VocSignalTypes::kEcdheApprovalVocFrame, handle_voc_frame}},
                                      {kCreateClientCertificates, kDone}}},

         {kCreateClientCertificates, {nullptr,
                                      {{fsm::Signal::kTimeout, handle_timeout},
                                       {VocSignalTypes::kCaDpa020Signal, handle_ca_dpa_020},
                                       {VocSignalTypes::kBleConnectionSignal, handle_ble_connection},
                                       {VocSignalTypes::kAesCsrActorVocFrame, handle_voc_frame},
                                       {VocSignalTypes::kAesCsrNearfieldVocFrame, handle_voc_frame}},
                                      {kFinalizePairing, kDone}}},

         {kFinalizePairing,          {finalize_pairing,
                                      {{fsm::Signal::kTimeout, handle_timeout},
                                       {VocSignalTypes::kBleConnectionSignal, handle_ble_connection},
                                       {VocSignalTypes::kCaCat002Signal, handle_ca_cat_002}},
                                      {kDone}}},

         {kDone,                     {done,
                                      SignalFunctionMap(),
                                      {}}}};

    SetStateMap(state_map);

    // We need to map the Signal types for Voc Frames so
    // smart transaction does not discard them, this is needed
    // because Frames does not have TransactionIds.
    MapSignalType(kEcdheRequestVocFrame);
    MapSignalType(kEcdheApprovalVocFrame);
    MapSignalType(kAesCsrActorVocFrame);
    MapSignalType(kAesCsrNearfieldVocFrame);

    // Map signal type for BLE connection callback
    MapSignalType(kBleConnectionSignal);

    if (initial_signal)
    {
        MapTransactionId(initial_signal->GetTransactionId().GetSharedCopy());
        EnqueueSignal(initial_signal);
    }

    //generate pairing session id
    pairing_session_id_ = FsmGenerateId(CCM_SESSION_ID_LEN);
    MapSessionId(pairing_session_id_);

}

DevicePairingTransaction::~DevicePairingTransaction()
{
    if (ecdhe_ephemeral_key_)
    {
        EVP_PKEY_free(ecdhe_ephemeral_key_);
    }
    if (client_actor_certificate_)
    {
        X509_free(client_actor_certificate_);
    }
    if (client_nearfield_certificate_)
    {
        X509_free(client_nearfield_certificate_);
    }
}

void DevicePairingTransaction::OnBleManagerConnectionCallback(BLEM::ConnectionID connection_id,
                                                              BLEM::ConnectionState connection_state)
{
    std::shared_ptr<fsm::Signal> ble_signal = std::make_shared<BleConnectionSignal>(connection_id,
                                                                                    connection_state);

    if (!EnqueueSignal(ble_signal))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s Failed to enqueue BLE connection signal",
                __FUNCTION__);
    }
}

void DevicePairingTransaction::OnBleManagerFrame(BLEM::ConnectionID connection_id,
                                                 BLEM::IVoCBaseFrame* frame)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received data in state %d",
                    __FUNCTION__, GetState());

    if (frame)
    {
        // TODO: We could consider verifying that connection_id == ble_connection_id_
        //       here, but its a little scetchy for the ECDHE Rdequest frame.
        //       Better to wwait for signal refactoring, put it into the "signal header"
        //       and consider it in the worker thread when/if we in the future handle
        //       multiple connections.

        // Decode recevied frame into a signal and encode it on our signal queue.
        const char* data = frame->GetBuffer();
        uint16_t data_length = frame->Length();

        if (data) //Approval frame can be empty, else we should checkl data_length > 0
        {
            std::vector<unsigned char> data_vector(data, data + data_length + kVocFrameHeaderSize);
            std::shared_ptr<fsm::Signal> voc_frame = codec_.Decode(data_vector);

            if (!voc_frame)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s Failed to Decode voc frame recevied over BLE",
                                                __FUNCTION__);
            }
            else
            {
                if (!EnqueueSignal(voc_frame))
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s Failed to enqueue voc frame recevied over BLE",
                                                        __FUNCTION__);
                }
            }
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s Received bad data from Ble Manager pairing service frame",
                                        __FUNCTION__);
        }

        delete frame;
        frame = nullptr;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s Received bad frame from Ble Manager pairing service",
                                __FUNCTION__);
    }
}

bool DevicePairingTransaction::HandlePairingVisibilityRequest(std::shared_ptr<fsm::Signal> signal)
{
    // Default to not stopping transaction
    bool return_value = true;
    bool lc_rc = true;
    int local_config_int = 0;
    lcfg::ILocalConfig *localconfig(lcfg::ILocalConfig::GetInstance());

    if (nullptr == localconfig)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s can't get localconfig instance pointer",
                        __FUNCTION__);
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    // Create and map a VehicleComm transaction id.
    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
    std::make_shared<fsm::VehicleCommTransactionId>();

    MapTransactionId(vehicle_comm_request_id);

    // Initiate BLE pairing service
    ble_pairing_service_ = BLEM::IBLEManager::GetPairingServiceInterface();

    if (ble_pairing_service_ &&
        BLEM::BLEM_OK != ble_pairing_service_->RegisterDataCallback(
                             std::bind(&DevicePairingTransaction::OnBleManagerFrame,
                                       this,
                                       std::placeholders::_1,
                                       std::placeholders::_2)))
    {
        ble_pairing_service_ = nullptr;
    }

    if (ble_pairing_service_ &&
        BLEM::BLEM_OK != ble_pairing_service_->RegisterConnectionStateCallback(
                             std::bind(&DevicePairingTransaction::OnBleManagerConnectionCallback,
                                       this,
                                       std::placeholders::_1,
                                       std::placeholders::_2)))
    {
        ble_pairing_service_ = nullptr;
    }

    // Abort if we fail to initiate BLE becasue we will not be able to get the BDAK.
    if (!ble_pairing_service_)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SIT: SW_APP --> SW_VUC : provideBLEcommunication : failed");
        Terminate("Failed to initiate ble pairing service", true);
        return_value = false;
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_VUC : provideBLEcommunication : OK");
    }

    // Send the request
    if (return_value && (nullptr != localconfig))
    {
        lc_rc = localconfig->GetInt(kFSMLocalConfigSection,
                                    kPairingVisibilityTimeout,
                                    local_config_int);
        if(lc_rc)
        {
            visibility_timeout_= static_cast<std::chrono::seconds>(local_config_int);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Read visibility_timeout_ %ll sec from local_config",
            visibility_timeout_);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
            "Can't read from local_config. Using default visibility_timeout_ %ll sec.",
            visibility_timeout_);
        }

        lc_rc = localconfig->GetInt(kFSMLocalConfigSection,
                                    kRemotePairingVisibilityTimeout,
                                    local_config_int);
        if(lc_rc)
        {
            remote_timeout_ = static_cast<std::chrono::seconds>(local_config_int);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Read remote_timeout_ %ll sec from local_config",
            remote_timeout_);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
            "Can't read from local_config. Using default remote_timeout_ %ll sec",
            remote_timeout_);
        }
    }

    return_value = return_value && HandlePairingVisibilityTimeouts();

    LOG_ON_RETURN_FALSE(return_value);
    return return_value;
}

bool DevicePairingTransaction::HandlePairingVisibilityTimeouts(void)
{
    // Default to stopping transaction
    bool return_value = false;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s ", __FUNCTION__);

    //request both timers and spare the associated IDs

    //For pairing visibility start a timeout for a periodic tick.
    //The complete time will be counted from "ticks" in the timeout handling
    kPeriodic_tick_id_ = RequestTimeout(kPeriodic_tick_, true);
    MapTransactionId(kPeriodic_tick_id_.GetSharedCopy());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "SIT: Timer timeOut %i s started", visibility_timeout_);

    //Start a timeout to indicate when to announce to VSDP
    remote_visibility_id_ = RequestTimeout(remote_timeout_, false);
    MapTransactionId(remote_visibility_id_.GetSharedCopy());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "SIT: Timer VSDPdelay %i s started", remote_timeout_);

    // Start advertising over BLE
    if (ble_pairing_service_)
    {
        if (BLEM::BLEM_OK != ble_pairing_service_->StartPairingService())
        {
            Terminate("Failed to start BLE pairing service.", true);
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP Event BLE_pairingMode(true) sent");
            SetState(kAdvertising);
            return_value = true;
        }
    }

    LOG_ON_RETURN_FALSE(return_value);
    return return_value;
}

bool DevicePairingTransaction::HandleCaDpa002(std::shared_ptr<fsm::Signal> signal)
{
    // Default to not stopping transaction
    bool return_value = true;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    std::shared_ptr<CaDpa002Signal> ca_dpa_002_response = std::static_pointer_cast<CaDpa002Signal>(signal);

    CaDpa002Signal::FinishedStatus finished_status;

    if (ca_dpa_002_response->IsInitiated())
    {
        //map remote session id
        std::string remote_session_id;

        if (ca_dpa_002_response->GetRemoteConnectionSessionId(remote_session_id))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: VSDP received register pairing availability of TCAM");
            MapSessionId(remote_session_id);
            pairing_session_id_ = remote_session_id;
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "DevicePairingTransaction, no remote connection session id in ca-dpa-002" );
        }

        //map signals session id if present
        std::string signal_session_id;

        if (ca_dpa_002_response->GetSessionId(signal_session_id))
        {
            MapSessionId(signal_session_id);
        }
    }
    else if (ca_dpa_002_response->IsFinished(finished_status))
    {
        Terminate("received CA-DPA-002 indicating pairing finished", false);
        return_value = false;
    }

    LOG_ON_RETURN_FALSE(return_value);
    return return_value;
}

bool DevicePairingTransaction::HandleCaDpa020(std::shared_ptr<fsm::Signal> signal)
{
    // Default to not stopping transaction
    bool return_value = true;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    std::shared_ptr<CaDpa020Signal> ca_dpa_020 = std::dynamic_pointer_cast<CaDpa020Signal>(signal);
    std::shared_ptr<fsm::Signal> frame;
    fsm::CCMTransactionId transaction_id;

    if (ca_dpa_020)
    {
        frame = ca_dpa_020->GetVocFrame(codec_);
        transaction_id = dynamic_cast<const fsm::CCMTransactionId&>(ca_dpa_020->GetTransactionId());
    }

    if (!frame)
    {
        Terminate("failed to get frame", true);
        return_value = false;
    }
    else
    {
        return_value = HandleVocFrame(frame, transaction_id);
    }

    LOG_ON_RETURN_FALSE(return_value);
    return return_value;
}

bool DevicePairingTransaction::HandleBleConnection(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;

    std::shared_ptr<BleConnectionSignal> ble_signal = std::dynamic_pointer_cast<BleConnectionSignal>(signal);

    if (ble_signal)
    {
        State state = GetState();

        BLEM::ConnectionID connection_id = ble_signal->GetConnectionId();

        BLEM::ConnectionState connection_state = ble_signal->GetConnectionState();

        switch (connection_state)
        {
        case BLEM::CONNECTION_STATE_CONNECTED:
            switch (state)
            {
            case kNew:
            case kAdvertising:
                ble_connection_id_ = connection_id;
                break;
            default:
                if (pairing_channel_ == PairingChannel::kBle)
                {
                    Terminate("Received new BLE pairing service connection while already pairing via BLE.",
                              true);
                    return_value = false;
                }
                else
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                                   "Received new BLE pairing service connection while pairing via VSDP, ignoring.");
                }
            }
            break;
        case BLEM::CONNECTION_STATE_DISCONNECTED:
            switch (state)
            {
            case kEcdheApproval:
            case kCreateClientCertificates:
            case kFinalizePairing:
            case kDone:
                if (pairing_channel_ == PairingChannel::kBle)
                {
                    Terminate("Received BLE pairing service disconnection while pairing via BLE.", true);
                    return_value = false;
                }
            default:
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                               "Received BLE pairing service disconnection before pairing initiated, ignoring.");
            }
            break;
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                            "Received unexpected ble connection state event_ %d, ignoring.",
                            connection_state);
        }  // switch (connection_state)
    }  // if (ble_signal)
    else
    {
        Terminate("Bad BLE connection signal, terminating transaction.", true);
        return_value = false;
    }

    return return_value;
}

bool DevicePairingTransaction::HandleVocFrame(std::shared_ptr<fsm::Signal> frame, fsm::CCMTransactionId transaction_id)
{

    bool return_value = true;

    State state = GetState();

    // Ensure we only handle frames from the correct source
    // kBlePairingServiceTransactionId_ is unique and should
    // either on the initial request or when we have determined
    // that we are pairing over BLE:
    if ((transaction_id == kBlePairingServiceTransactionId_) &&
        (pairing_channel_ == PairingChannel::kVsdp))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                        "Ignoring frame %s, as we are pairing over VSDP but frame comes from BLE",
                        frame->ToString().c_str());
    }
    else
    {
        switch (frame->GetSignalType())
        {
        case kEcdheRequestVocFrame:
        {
            //ToDo: according to SyArch (2017-10-03) a security audit log shall be written if we receive from BLE a frame, which is NOT type 0x01
            if (state == kAdvertising)
            {
                std::shared_ptr<EcdheRequestVocFrame> request_frame =
                    std::dynamic_pointer_cast<EcdheRequestVocFrame>(frame);

                if (request_frame)
                {
                    //Determine if we are initiating pairing via BLE or VSDP
                    if (transaction_id == kBlePairingServiceTransactionId_)
                    {
                        pairing_channel_ = PairingChannel::kBle;
                    }
                    else
                    {
                        pairing_channel_ = PairingChannel::kVsdp;
                    }
                    SIT_LOG_RECEIVED;

                    if (!HandleEcdheRequest(request_frame, transaction_id))
                    {
                        Terminate("failed to handle ecdhe request", true);
                        return_value = false;
                    }
                    else
                    {
                        SetState(kEcdheApproval);
                    }
                }
                else
                {
                    Terminate("failed to get request frame", true);
                    return_value = false;
                }
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                                "kEcdheRequestVocFrame in bad state: %d, ignoring", state);
            }

            break;
        } // case kEcdheRequestVocFrame
        case kEcdheApprovalVocFrame:
        {
            if (state == kEcdheApproval)
            {
                SIT_LOG_RECEIVED;

                // store the fact this was received in instance data
                is_peer_approval_received_ = true;

                // store transaction id to use when responding
                ecdhe_approval_transaction_id_ = transaction_id;

                std::string pin;
                if (GetOnScreenSecret(pin))
                {
                    // ToDo: in fact both approvals may arrive in parallel and no order could be expected.
                    // TODO: actually send request through VC, for now we simply log.
                    // ToDo log when sending was successfully done: DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SOME_IP send SOME_IP_send(PIN,IHU)");
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                                    "SIGNAL TO IHU CodeConfirmationByUserRequest, pin: %s", pin.c_str());

                    MapTransactionId(default_ihu_transaction_id.GetSharedCopy());
                }
                else
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                                   "Failed to calculate pin.");
                    return_value = false;
                }
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                                "kEcdheRequestVocFrame in bad state: %d, ignoring", state);
            }

            break;
        } // case kEcdheApprovalVocFrame
        case kAesCsrActorVocFrame:
        {
            std::shared_ptr<AesCsrActorVocFrame> request_frame =
                std::dynamic_pointer_cast<AesCsrActorVocFrame>(frame);

            if (request_frame)
            {
                if (state == kCreateClientCertificates)
                {
                    SIT_LOG_RECEIVED;
                    // ToDo: before signing the cert it must be ensured that common names of AesCsrActorVocFrame and AesCsrNearfieldVocFrame match
                    return_value = HandleAesCsrActor(request_frame, transaction_id);
                }
                else
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                                    "kAesCsrActorVocFrame in bad state: %d, ignoring", state);
                }
            }
            else
            {
                Terminate("failed to get request frame", true);
                return_value = false;
            }

            break;
        } //case kAesCsrActorVocFrame:
        case kAesCsrNearfieldVocFrame:
        {
            std::shared_ptr<AesCsrNearfieldVocFrame> request_frame =
                std::dynamic_pointer_cast<AesCsrNearfieldVocFrame>(frame);

            if (request_frame)
            {
                if (state == kCreateClientCertificates)
                {
                    SIT_LOG_RECEIVED;
                    // ToDo: before signing the cert it must be ensured that common names of AesCsrActorVocFrame and AesCsrNearfieldVocFrame match
                    return_value = HandleAesCsrNearfield(request_frame, transaction_id);
                }
                else
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                                    "kAesCsrNearfieldVocFrame in bad state: %d, ignoring", state);
                }
            }
            else
            {
                Terminate("failed to get request frame", true);
                return_value = false;
            }

            break;
        } //case kAesCsrActorVocFrame:
        default:
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                            "Unexpected frame %s, ignoring", frame->ToString().c_str());
        }
        }
    } // is frame from right source

    LOG_ON_RETURN_FALSE(return_value);
    return return_value;
}

bool DevicePairingTransaction::HandleCodeConfirmationByUser(std::shared_ptr<fsm::Signal> signal)
{
    // Default to not stopping transaction
    bool return_value = true;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    // Store the fact this was received in instance data
    // TODO: actually check signal content once we have actual signal
    is_user_approval_received_ = true;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "SIT: simulated received SOME_IP_receive %s",\
        is_user_approval_received_?"PIN accept":"PIN deny");

    // Send ECDHE Approval to client
    std::shared_ptr<CaDpa021Signal> ca_dpa_021 =
        std::make_shared<CaDpa021Signal>(ecdhe_approval_transaction_id_, 1);
    if(ca_dpa_021)                      //  fix klocwork
    {
        ca_dpa_021->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);
        ca_dpa_021->SetDevicePairingId(pairing_id_type_, pairing_id_);
        ca_dpa_021->SetSessionId(pairing_session_id_);
        EcdheApprovalVocFrame approval_frame;

        if (!SendToRemoteClient(approval_frame, ca_dpa_021))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                           "Failed to send ecdhe approval.");
            return_value = false;
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                           "Sent ECDHE Approval.");
        }

        // if user confiramtion also received, move to next state
        if (return_value && is_peer_approval_received_)
        {
            SetState(kCreateClientCertificates);
        }
    }

    LOG_ON_RETURN_FALSE(return_value);
    return return_value;
}


bool DevicePairingTransaction::SendCaDpa001(bool stop)
{
    bool publishing_successful = true;

    std::shared_ptr<CaDpa001Signal> ca_dpa_001 =
        std::make_shared<CaDpa001Signal>(device_pairing_transaction_id_, 1);

    char buffer[32] = {0};
    device_pairing_transaction_id_.WriteToCString(buffer);

    if(ca_dpa_001)                 //   fix klocwork
    {
        if (!pairing_session_id_.empty())
        {
            ca_dpa_001->SetSessionId(pairing_session_id_);
        }
        if (!stop)
        {
            ca_dpa_001->SetCommandStart();
        }
        else
        {
            ca_dpa_001->SetCommandStop();
        }
        ca_dpa_001->SetDevicePairingId(pairing_id_type_, pairing_id_);

        //TODO: set vehicle information in ca_dpa_001, requires updated schema from vcc
        //      currenty VIN is INTEGER which crashes in mapp if we send it

        fsm::CarAccessSignalSource& car_access_signal_source = fsm::CarAccessSignalSource::GetInstance();

        MapTransactionId(device_pairing_transaction_id_.GetSharedCopy());

        // TODO: get CarAccess user? depends on decisions around backend app certificates
        if (car_access_signal_source.PublishMessageToBackend(ca_dpa_001, fsm::CarAccessSignalSource::kDevicePairing))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> VSDP : inform VSDP about pairing mode : OK");
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DevicePairingTransaction, CA-DPA-001 published");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> VSDP : inform VSDP about pairing mode : failed");
            publishing_successful = false;
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DevicePairingTransaction, "
                                                   "Failed to publish CA-DPA-001.");
        }
    }
    return publishing_successful;
}


bool DevicePairingTransaction::HandleTimeout(std::shared_ptr<fsm::Signal> signal)
{
    // Default to not stopping transaction
    bool return_value = true;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    // TODO:
    // if elapsed time > X : operation timed out
    //    - send PairingVisibilityCntdwnNotify to IHU to indicate timeout
    //    - send PairingStatusNotify to IHU to indicate abort
    // if periodic ping
    //    - send PairingVisibilityCntdwnNotify to IHU

    //We can receive 2 types of timeouts:
    //1. to start to advertise over VSDP (only relevant if we are still in the kAdvertising mode
    //2. tick towards IHU, countdown for the entire visibility, relevant always

    //check if we should start advertising over VSDP
    if (GetState() == kAdvertising && signal->GetTransactionId() == remote_visibility_id_ )
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: Timer VSDPdelay expired");
        if (true != SendCaDpa001())
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "SendCaDpa001() send failed.");
        }
    }
    else if (signal->GetTransactionId() == kPeriodic_tick_id_)  //check if this is a periodic tick
    {
        //if still time left then send notification to IHU
        if (visibility_timeout_ > kPeriodic_tick_)
        {
            //decrease the total counter
            visibility_timeout_ = visibility_timeout_ - kPeriodic_tick_;

            //TODO:send actual singals to IHU
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIGNAL TO IHU: PairingVisibilityCntdwnNotify");
        }
        else
        {
            //We have timed out
            visibility_timeout_ = std::chrono::seconds(0);

            //If we are here and we are not yet done, terminate
            if (GetState() != kDone)
            {
                //TODO:send actual singals to IHU
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: Timer timeOut expired");
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIGNAL TO IHU: PairingVisibilityCntdwnNotify, timed out");
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIGNAL TO IHU: PairingStatusNotify, abort");

                Terminate("Pairing has timed out, terminating", false);
                return_value = false;
            }
        }

    }

    LOG_ON_RETURN_FALSE(return_value);
    return return_value;
}

bool DevicePairingTransaction::HandleCaCat002(std::shared_ptr<fsm::Signal> signal)
{
    // Default to not stopping transaction
    bool return_value = true;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    std::shared_ptr<CaCat002Signal> ca_cat_002 = std::dynamic_pointer_cast<CaCat002Signal>(signal);

    bool success = ca_cat_002->IsSuccess();
    long code = success ? (long) ca_cat_002->GetSuccessCode() : (long) ca_cat_002->GetErrorCode();
    std::string message = ca_cat_002->GetInfoMessage();

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                    "DevicePairingTransaction, received ca-cat-002: result %s code: %d message %s\n",
                    success ? "SUCCESS":"FAILURE",
                    code,
                    message.c_str());

    if (success)
    {
        SetState(kDone);
    }
    else
    {
        Terminate("catalogue upload failed", true);
        return_value = false;
    }

    LOG_ON_RETURN_FALSE(return_value);
    return return_value;
}

void DevicePairingTransaction::FinalizePairing(State old_state, State new_state)
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s in state %d",
                    __FUNCTION__, GetState());

    bool success = true;
    fsm::ReturnCode rc;
    std::shared_ptr<fsm::UsermanagerInterface> usermanager;

    X509* vehicle_ca = nullptr;
    X509* vehicle_actor = nullptr;

    std::shared_ptr<fsm::Keystore> keystore = std::make_shared<fsm::Keystore>();
    success = keystore.get() != nullptr;
    if (success)
    {
        usermanager = fsm::UsermanagerInterface::Create(keystore);

        // Get certificates from keystore
        STACK_OF(X509) *certificates = sk_X509_new_null();
        rc = keystore->GetCertificates(certificates, fsm::Keystore::CertificateRole::kVehicleCa);
        if (rc != fsm::ReturnCode::kSuccess || sk_X509_num(certificates) != 1)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                           "Failed to get CA.");
            success = false;
        }
        else
        {
            vehicle_ca = sk_X509_pop(certificates);
        }
        sk_X509_free(certificates);

        rc = keystore->GetCertificate(&vehicle_actor, fsm::Keystore::CertificateRole::kVehicleActor);
        if (rc != fsm::ReturnCode::kSuccess)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                           "Failed to get Vehicle Actor.");
            success = false;
        }
    }
    // Send AES CA to client
    if (success)
    {
        std::shared_ptr<CaDpa021Signal> ca_dpa_021
            = std::make_shared<CaDpa021Signal>(finalize_pairing_transaction_id_, 2);
        if(ca_dpa_021)               // fix klocwork
        {
            ca_dpa_021->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);
            ca_dpa_021->SetDevicePairingId(pairing_id_type_, pairing_id_);
            ca_dpa_021->SetSessionId(pairing_session_id_);
            AesCaVocFrame frame(vehicle_ca);

            if (!SendToRemoteClient(frame, ca_dpa_021))
            {
                Terminate("failed to send ca", true);
                success = false;
            }
            else
            {
                SIT_LOG_SEND("0x08");
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                               "Sent AES CA.");
            }
        }
    }

    // Send AES CAR ACTOR to client
    if (success)
    {
        std::shared_ptr<CaDpa021Signal> ca_dpa_021 =
            std::make_shared<CaDpa021Signal>(finalize_pairing_transaction_id_, 3);
        if(ca_dpa_021)           // fix klocwork
        {
            ca_dpa_021->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);
            ca_dpa_021->SetDevicePairingId(pairing_id_type_, pairing_id_);
            ca_dpa_021->SetSessionId(pairing_session_id_);
            AesCarActorCertVocFrame frame(vehicle_actor);

            if (!SendToRemoteClient(frame, ca_dpa_021))
            {
                Terminate("failed to send car actor", true);
                success = false;
            }
            else
            {
                SIT_LOG_SEND("0x0B");
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                               "Sent AES CAR ACTOR.");
            }
        }
    }

    // Send AES BDAK to client
    if (success)
    {
        AesBleDaKeyVocFrame::BleDaKey bdak;

        if (ble_pairing_service_)
        {
            BLEM::BDAK blem_bdak;
            if (BLEM::BLEM_OK != ble_pairing_service_->GetBDAK(&blem_bdak))
            {
                Terminate("Failed to get BDAK.", true);
                success = false;
            }
            else
            {
                std::copy(std::begin(blem_bdak), std::end(blem_bdak), std::begin(bdak));
            }
        }

        if (success)
        {
            std::shared_ptr<CaDpa021Signal> ca_dpa_021 =
                std::make_shared<CaDpa021Signal>(finalize_pairing_transaction_id_, 4);
            if(ca_dpa_021)              // fix klocwork
            {
                ca_dpa_021->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);
                ca_dpa_021->SetDevicePairingId(pairing_id_type_, pairing_id_);
                ca_dpa_021->SetSessionId(pairing_session_id_);
                AesBleDaKeyVocFrame frame(bdak);

                if (!SendToRemoteClient(frame, ca_dpa_021))
                {
                    Terminate("failed to send bdak", true);
                    success = false;
                }
                else
                {
                    SIT_LOG_SEND("0x0A");
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                                   "Sent AES BDAK.");
                }
            }
        }
    }

    // Store new user
    fsm::UserId id = user_manager::kUndefinedUserId;  // value will be modified by AddUser call; assigned dummy value to satisfy Klocwork

    if (success)
    {
        fsm::UserRole role = RoleSelectedNotificationTransaction::GetSelectedRole();

        rc = usermanager->AddUser(id,
                                  role,
                                  client_actor_certificate_,
                                  client_nearfield_certificate_);
        if (rc != fsm::ReturnCode::kSuccess)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                           "Failed to add user.");
            success = false;
        }
    }

    if (success)
    {
        // Tell IHU we are done
        // TODO: actually signal when we have interface, for now we just log
        //ToDo: enable this when message to IHU was sent successfully: DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> VGM : inform IHU on pairing success : OK");
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                       "SIGNAL TO IHU: PairingStatusNotify done");
    }

    // Upload certs to catalogue
    if (success)
    {
        //TODO: consider uploading vehicle certs already in role selected transaction

        fsm::CCMTransactionId id;
        std::shared_ptr<CaCat001Signal> signal =
            CaCat001Signal::CreateCaCat001Signal(id, 1);
        signal->SetSessionId(pairing_session_id_);

        success = signal->AddCertificate(vehicle_ca);
        success = signal->AddCertificate(vehicle_actor);
        success = signal->AddCertificate(client_actor_certificate_);

        if (success)
        {
            fsm::CarAccessSignalSource& car_access =
                fsm::CarAccessSignalSource::GetInstance();

            //TODO: get CarAccess user once we have support for backend service users
            success = car_access.PublishMessageToBackend(signal,
                                                         fsm::CarAccessSignalSource::kCatalogue);
            if (!success)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> IHU : send certificate list to VSDP : failed");
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                               "Failed to send to catalogue.");
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> IHU : send certificate list to VSDP : OK");
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                               "Sent CA-CAT-001.");

                MapTransactionId(id.GetSharedCopy());

                //TODO: workaround as cloud seems to give us some
                //      unrelated transaction id back when responding
                //      with ca-cat-002
                MapSignalType(VocSignalTypes::kCaCat002Signal);
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                           "Failed to add certs to CA-CAT-001.");
        }
    }

    // Send car position to the newly added user
    if (success)
    {
        std::shared_ptr<std::vector<fsm::UserId>> users = std::make_shared<std::vector<fsm::UserId>>(1, id);

        fsm::InternalSignalTransactionId transaction_id;

        std::shared_ptr<fsm::InternalSignal<std::vector<fsm::UserId>>> position_update_request =
            std::make_shared<fsm::InternalSignal<std::vector<fsm::UserId>>>(transaction_id,
                                                                            VocSignalTypes::kPositionUpdateInternalSignal,
                                                                            users);

        fsm::InternalSignalSource& internal_signal_source = fsm::InternalSignalSource::GetInstance();

        internal_signal_source.BroadcastSignal(position_update_request);
    }

    if (!success)
    {
        Terminate("failed to finalize pairing", true);
    }

    if (vehicle_ca)
    {
        X509_free(vehicle_ca);
    }
    if (vehicle_actor)
    {
        X509_free(vehicle_actor);
    }
}

void DevicePairingTransaction::Done(State old_state, State new_state)
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s in state %d",
                    __FUNCTION__, GetState());

    // Clean-up
    Terminate("Pairing finished.", false);

    // TODO:
    // emit internal signal, "new device paired". TBD, no pattern for this, IPC....

    // The signal handlers are not guaranteed to stop this transaction
    // so enforce a stop here to be safe.
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: left Activity provideDevicePairing");
    Stop();
}

bool DevicePairingTransaction::HandleEcdheRequest(std::shared_ptr<EcdheRequestVocFrame> frame,
                                                  fsm::CCMTransactionId transaction_id)
{
    bool return_value = true;

    EVP_PKEY* peer_key = frame->GetKey();

    // calculate shared secret and update voc frame codec
    // this will cause our ephemeral key to be generated
    if (!EcdheCalculateSecret(peer_key))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : create public/private key pair for ECDHE : failed");
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                       "Failed to calculate shared secret.");
        return_value = false;
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : create public/private key pair for ECDHE : OK");
    }

    if (return_value && !codec_.SetEncryptionKeys(ecdhe_ephemeral_key_,
                                                  peer_key,
                                                  ecdhe_shared_secret_))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : calculate shared secret from device public key and TCAM public key : failed");
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                       "Failed to set aes keys.");
        return_value = false;
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : calculate shared secret from device public key and TCAM public key : OK");
    }

    // send ECDHE request to client
    if (return_value)
    {
        std::shared_ptr<CaDpa021Signal> ca_dpa_021 =
            std::make_shared<CaDpa021Signal>(transaction_id, 1);
        if(ca_dpa_021)             //      fix klocwork
        {
    			 
            ca_dpa_021->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);
            ca_dpa_021->SetDevicePairingId(pairing_id_type_, pairing_id_);
            ca_dpa_021->SetSessionId(pairing_session_id_);
            EcdheRequestVocFrame response_frame(ecdhe_ephemeral_key_);

            if (!SendToRemoteClient(response_frame, ca_dpa_021))
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                               "Failed to send ecdhe request.");
                return_value = false;
            }
            else
            {
                SIT_LOG_SEND("0x01");
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                               "Sent ECDHE Request.");
            }

        }
    }
    	

    if (peer_key)
    {
        EVP_PKEY_free(peer_key);
    }

    return return_value;
}

bool DevicePairingTransaction::EcdheCalculateSecret(EVP_PKEY* peer_key)
{
    bool return_value = true;

    if (!peer_key)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "peer_key parameter error in %s", __FUNCTION__);
        return_value = false;
    }

    Derived_Secret generated_secret = {0};
    EVP_PKEY *evpkey_parameters = nullptr;
    EVP_PKEY_CTX *gen_pkey_context = nullptr;
    EVP_PKEY_CTX *parameters_context = nullptr;
    EVP_PKEY_CTX *derived_secret_context = nullptr;

    //Parameters setup
    parameters_context = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (NULL == parameters_context)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "parameters_context_ alloc error in %s", __FUNCTION__);
        return_value = false;
    }
    if (return_value && EVP_PKEY_paramgen_init(parameters_context) <= 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "parameters_context_ init error in %s", __FUNCTION__);
        return_value = false;
    }
    if (return_value && EVP_PKEY_CTX_set_ec_paramgen_curve_nid(parameters_context,
                                                               kCurve_name_) <= 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "Set %i curve error in %s", kCurve_name_, __FUNCTION__);
        return_value = false;
    }
    if (return_value && EVP_PKEY_paramgen(parameters_context, &evpkey_parameters) <= 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Error in %s", __FUNCTION__);
        return_value = false;
    }

    // Generate our ephemeral key
    if (return_value)
    {
        gen_pkey_context = EVP_PKEY_CTX_new(evpkey_parameters, NULL);
    }
    if (return_value && (NULL == gen_pkey_context))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "gen_pkey_context_ alloc error in %s", __FUNCTION__);
        return_value = false;
    }
    if (return_value && EVP_PKEY_keygen_init(gen_pkey_context) <= 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "gen_pkey_context_ init error in %s", __FUNCTION__);
        return_value = false;
    }
    if (return_value && EVP_PKEY_keygen(gen_pkey_context, &ecdhe_ephemeral_key_) <= 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "ecdhe_ephemeral_key_ generation error in %s", __FUNCTION__);
        return_value = false;
    }

    // Calculate shared secret
    if (return_value)
    {
        derived_secret_context = EVP_PKEY_CTX_new(ecdhe_ephemeral_key_, NULL);
    }
    if (return_value && (NULL == derived_secret_context))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "derived_secret_context_ alloc error in %s", __FUNCTION__);
        return_value = false;
    }
    if (return_value && EVP_PKEY_derive_init(derived_secret_context) <= 0)
    {
       DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                       "derived_secret_context_ init error in %s", __FUNCTION__);
       return_value = false;
    }
    if (return_value && EVP_PKEY_derive_set_peer(derived_secret_context, peer_key) <= 0)
    {
       DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "derive_set_peer error in %s", __FUNCTION__);
       return_value = false;
    }
    if (return_value && EVP_PKEY_derive(derived_secret_context,
                                        NULL,
                                        &(generated_secret.secret_len)) <= 0)
    {
       DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Derived secret_len=%i. Error in %s",
                       generated_secret.secret_len, __FUNCTION__);
       return_value = false;
    }
    if (return_value)
    {
        generated_secret.secret_buffer =
            static_cast<unsigned char*>(OPENSSL_malloc(generated_secret.secret_len));
    }
    if (return_value && (NULL == generated_secret.secret_buffer))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "secret_buffer alloc error in %s", __FUNCTION__);
        return_value = false;
    }
    if (return_value && (EVP_PKEY_derive(derived_secret_context,
                                    generated_secret.secret_buffer,
                                    &generated_secret.secret_len)) <= 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "secret_buffer alloc error in %s", __FUNCTION__);
        return_value = false;
    }
    if (return_value)
    {
        ecdhe_shared_secret_.assign(generated_secret.secret_buffer,
                                    (generated_secret.secret_buffer+generated_secret.secret_len));
    }

    //Parameters cleanup
    if (evpkey_parameters)
    {
        EVP_PKEY_free(evpkey_parameters);
    }

    //Contexts cleanup
    if (gen_pkey_context)
    {
        EVP_PKEY_CTX_free(gen_pkey_context);
    }
    if (parameters_context)
    {
        EVP_PKEY_CTX_free(parameters_context);
    }
    if (derived_secret_context)
    {
        EVP_PKEY_CTX_free(derived_secret_context);
    }

    // free allocated buffers
    if (generated_secret.secret_buffer)
    {
        OPENSSL_free(generated_secret.secret_buffer);
    }

    return return_value;
}

bool DevicePairingTransaction::GetOnScreenSecret(std::string& secret)
{

    // Implements this pin generation algorithm
    // pin = generatePin(dek, DIGITS=6) :
    // md = sha256(dek)    // md is a 32-byte string
    // offset = low-order 4 bits of md[31]
    // p = md[offset] … md[offset+3]   // take four bytes at offset
    // q = least significant 31 bits of p
    // pin = q modulo (10^DIGITS)
    // return pin

    unsigned char sha256hash[SHA256_DIGEST_LENGTH];
    unsigned char* hash = sha256hash;

    const int kOffsetBase = 31;

    const uint32_t least_31_mask = 0x7FFFFFFF;
    const uint32_t offset_mask = 0xF;

    unsigned int offset = 0, pin = 0;
    uint32_t sample = 0;
    SHA256_CTX sha256;

    bool return_value = true;

    if (!SHA256_Init(&sha256))
    {
        return_value = false;
    }

    if (return_value && !SHA256_Update(&sha256,
                                       ecdhe_shared_secret_.data(),
                                       ecdhe_shared_secret_.size()))
    {
        return_value = false;
    }

    if (return_value && !SHA256_Final(hash, &sha256))
    {
        return_value = false;
    }

    if (return_value)
    {
        offset = static_cast<unsigned int> ((*(hash + kOffsetBase)) & offset_mask);

        // per REEQPROD "right most byte is LSB", build sample
        sample = (hash+offset)[3] + ((hash+offset)[2] << 8) + ((hash+offset)[1] << 16) + ((hash+offset)[0] << 24);

        sample = sample & least_31_mask;

        pin = sample % static_cast<unsigned int>(pow(10, kOnScreenPinSize));

        secret = std::to_string(pin);
    }

    return return_value;
}

bool DevicePairingTransaction::PublishToDevicePairingTopic(std::shared_ptr<CaDpa021Signal> message)
{
    fsm::CarAccessSignalSource& car_access = fsm::CarAccessSignalSource::GetInstance();

    fsm::CarAccessSignalSource::BackendTopic topic =
        fsm::CarAccessSignalSource::BackendTopic::kDevicePairing;

    //TODO: get CarAccess user once we have support for backend service users

    return car_access.PublishMessageToBackend(message, topic);
}

bool DevicePairingTransaction::SendToBlePairingService(const VocFrameEncodableInterface& frame)
{
    bool return_value = true;

    // We could consider validating whether we have a connection id
    // to send to before proceeding here. But we know that we will only
    // try to send over BLE if we receied over BLE, which can only happen
    // if some device connected, so by definition we will have a connection id.
    if (ble_pairing_service_)
    {
        std::vector<unsigned char> encoded_buffer;
        BLEM::VoCBaseFrameType ble_frame_type = BLEM::VOCBASEFRAME_TYPE_INVALID;
        uint16_t ble_frame_length = 0;

        // Encode the frame
        if (!codec_.Encode(encoded_buffer, frame))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to encode frame.");
            return_value = false;
        }

        if (return_value)
        {

            // We need to convert our frame to a BLEManager style frame.
            //TODO: hopefully this can be made nicer once final BLEManager interface is there.

            switch (frame.GetFrameType())
            {
            case VocFrameType::kEcdheRequest:
                ble_frame_type = BLEM::VOCBASEFRAME_TYPE_ECDHE_REQUEST;
                break;
            case VocFrameType::kEcdheApproval:
                ble_frame_type = BLEM::VOCBASEFRAME_TYPE_ECDHE_APPROVAL;
                break;
            case VocFrameType::kAesCertActor:
                ble_frame_type = BLEM::VOCBASEFRAME_TYPE_AES_CERT_ACTOR;
                break;
            case VocFrameType::kAesCertNearfield:
                ble_frame_type = BLEM::VOCBASEFRAME_TYPE_AES_CERT_NEARFIELD;
                break;
            case VocFrameType::kAesCa:
                ble_frame_type = BLEM::VOCBASEFRAME_TYPE_AES_CA;
                break;
            case VocFrameType::kAesBleDaKey:
                ble_frame_type = BLEM::VOCBASEFRAME_TYPE_AES_BLE_DA_KEY;
                break;
            case VocFrameType::kAesCarActorCert:
                ble_frame_type = BLEM::VOCBASEFRAME_TYPE_AES_CAR_ACTOR_CERT;
                break;
            default:
                // We do not expect to send any other types.
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Tried to send unexpected frame type.");
                return_value = false;
            }

            ble_frame_length = encoded_buffer.size() - kVocFrameHeaderSize;

            BLEM::IVoCBaseFrame* ble_frame = BLEM::IVoCBaseFrame::CreateVoCBaseFrame(ble_frame_type, ble_frame_length);

            if (ble_frame)
            {
                if (ble_frame->AddData(reinterpret_cast<char*>(&(encoded_buffer[kVocFrameHeaderSize])),
                                       ble_frame_length))
                {
                    // This gives control of the pointer to BLEManager and we must not free it after
                    // calling SendData.
                    if (BLEM::BLEM_OK != ble_pairing_service_->SendData(ble_connection_id_, ble_frame))
                    {
                        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to send frame over BLE.");
                        return_value = false;
                    }
                }
                else
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to add data to BLEManager VocBaseFrame.");

                    delete ble_frame;
                    ble_frame = nullptr;

                    return_value = false;
                }
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to create BLEManager VocBaseFrame.");
                return_value = false;
            }
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "Not sending frame over BLE because pairing service is not initialized.");
        return_value = false;
    }

    return return_value;
}

bool DevicePairingTransaction::SendToRemoteClient(const VocFrameEncodableInterface& frame,
                                                  std::shared_ptr<CaDpa021Signal> ca_dpa_021)
{
    bool return_value = true;

    if (pairing_channel_ == PairingChannel::kBle)
    {
        if (!SendToBlePairingService(frame))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to send frame over BLE");
            return_value = false;
        }
    }
    else if (pairing_channel_ == PairingChannel::kVsdp)
    {
        if (!ca_dpa_021->SetFrame(codec_, frame))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "failed to set frame in ca_dpa_021");
            return_value = false;
        }

        if (return_value && !PublishToDevicePairingTopic(ca_dpa_021))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to send frame over VSDP");
            return_value = false;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to send frame as pairing channel is unknown.");
        return_value = false;
    }

    return return_value;
}

bool DevicePairingTransaction::HandleAesCsrActor(std::shared_ptr<AesCsrActorVocFrame> frame,
                                                 fsm::CCMTransactionId transaction_id)
{
    bool return_value = true;

    X509_REQ* request = frame->GetRequest();
    if (!request)
    {
        Terminate("failed to get actor CSR from frame", true);
        return_value = false;
    }

    if (return_value && !SignCert(request, &client_actor_certificate_))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : sign both CSRs by Car Root CA certificate : failed");
        Terminate("failed to sign actor certificate", true);
        return_value = false;
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : sign both CSRs by Car Root CA certificate : OK");
    }

    if (return_value)
    {
        // Send AES Actor Cert to client
        std::shared_ptr<CaDpa021Signal> ca_dpa_021 =
            std::make_shared<CaDpa021Signal>(transaction_id, 1);
        if(ca_dpa_021)          // fix klocwork
		{
        	ca_dpa_021->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);
            ca_dpa_021->SetDevicePairingId(pairing_id_type_, pairing_id_);
            ca_dpa_021->SetSessionId(pairing_session_id_);
            AesCertActorVocFrame actor_frame(client_actor_certificate_);

            if (!SendToRemoteClient(actor_frame, ca_dpa_021))
            {
                Terminate("failed to send cert actor approval", true);
                return_value = false;
            }
            else
            {
                SIT_LOG_SEND("0x06");
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                               "Sent AES CERT ACTOR.");
            }
        }
    }

    if (return_value)
    {
        if (client_actor_certificate_ && client_nearfield_certificate_)
        {
            finalize_pairing_transaction_id_ = transaction_id;
            SetState(kFinalizePairing);
        }
    }

    return return_value;
}

bool DevicePairingTransaction::HandleAesCsrNearfield(std::shared_ptr<AesCsrNearfieldVocFrame> frame,
                                                     fsm::CCMTransactionId transaction_id)
{
    bool return_value = true;

    X509_REQ* request = frame->GetRequest();
    if (!request)
    {
        Terminate("failed to get nearfield CSR from frame", true);
        return_value = false;
    }

    if (return_value && !SignCert(request, &client_nearfield_certificate_))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : sign both CSRs by Car Root CA certificate : failed");
        Terminate("failed to sign nearfield certificate", true);
        return_value = false;
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : sign both CSRs by Car Root CA certificate : OK");
    }

    if (return_value)
    {
        // Send AES Nearfield Cert to client
        std::shared_ptr<CaDpa021Signal> ca_dpa_021 =
            std::make_shared<CaDpa021Signal>(transaction_id, 1);
        if(ca_dpa_021)          //  fix klocwork
        {
            ca_dpa_021->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);
            ca_dpa_021->SetDevicePairingId(pairing_id_type_, pairing_id_);
            ca_dpa_021->SetSessionId(pairing_session_id_);
            AesCertNearfieldVocFrame nearfield_frame(client_nearfield_certificate_);

            if (!SendToRemoteClient(nearfield_frame, ca_dpa_021))
            {
                Terminate("failed to send cert nearfield approval", true);
                return_value = false;
            }
            else
            {
                SIT_LOG_SEND("0x07");
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                               "Sent AES CERT NEARFIELD.");
            }
    	}
    }

    if (return_value)
    {
        if (client_actor_certificate_ && client_nearfield_certificate_)
        {
            finalize_pairing_transaction_id_ = transaction_id;
            SetState(kFinalizePairing);
        }
    }

    return return_value;
}

void DevicePairingTransaction::Terminate(std::string reason, bool abnormal)
{
    // TODO: Provide clean-up and abortion routines
    if (abnormal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "DevicePairingTransaction, aborting in state %d due to %s",
                        GetState(), reason.c_str());
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                        "DevicePairingTransaction, terminating in state %d due to %s",
                        GetState(), reason.c_str());
    }

    //Shutdown BLE
    if (ble_pairing_service_)
    {
        ble_pairing_service_->StopPairingService();
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP Event BLE_pairingMode(false) sent");

        ble_pairing_service_->DeregisterDataCallback();
        ble_pairing_service_->DeregisterConnectionStateCallback();

        // Note that ble_pairing_service_ should not be freed,
        // it is owned by BLEManager (That's right!)
    }

    // stop pairing in cloud
    // we do this always as we dont know from state if its started or not
    // if not startedd it should just fail
    SendCaDpa001(true);
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
