/**
 * Copyright (C) 2018 Continental AG and subsidiaries
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     vocmo_signal_source.cc
 *  \brief    Vocmo signal source.
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signal_sources/vocmo_signal_source.h"

#include <algorithm>
#include <mutex>

#include "dlt/dlt.h"
#include "voc_framework/signals/signal_factory.h"

#include "inadifcontroller.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

void Print_network_sig_status(NADIF_SIG_QUAL_DATA sig_data)
{
    switch (sig_data.network_type)
    {
        case NADIF_NETWORK_TYPE_UNKNOWN:
            DLT_LOG_STRING(dlt_libfsm,DLT_LOG_INFO, "network_type:NADIF_NETWORK_TYPE_UNKNOWN" );
            break;
        case NADIF_NETWORK_CDMA_1X:
            DLT_LOG_STRING(dlt_libfsm,DLT_LOG_INFO,"Network_type: NADIF_NETWORK_CDMA_1X");
            break;
        case NADIF_NETWORK_CDMA_EVDO:
            DLT_LOG_STRING(dlt_libfsm,DLT_LOG_INFO,"Network_type: NADIF_NETWORK_CDMA_EVDO");
            break;
        case NADIF_NETWORK_WCDMA:
            DLT_LOG_STRING(dlt_libfsm,DLT_LOG_INFO,"Network_type: NADIF_NETWORK_WCDMA");
            break;
        case NADIF_NETWORK_GSM:
            DLT_LOG_STRING(dlt_libfsm,DLT_LOG_INFO,"Network_type: NADIF_NETWORK_GSM");
            break;
        case NADIF_NETWORK_LTE:
            DLT_LOG_STRING(dlt_libfsm,DLT_LOG_INFO,"Network_type: NADIF_NETWORK_LTE");
            break;
        default:
            DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_INFO,"Network_type: %d",sig_data.network_type);
            break;
    }
    DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_INFO,"rssi: %d", sig_data.rssi);
    DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_INFO,"ber: %d", sig_data.ber);
    DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_INFO,"raw_rssi: %d", sig_data.raw_rssi);
    DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_INFO,"rscp: %d", sig_data.rscp);
    DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_INFO,"rsrp: %d", sig_data.rsrp);
}

VocmoSignalSource::VocmoSignalSource()
{
    ResultCode result = ComaGetCommunicationMgrProxy();

    if (ResultCode::kSuccess == result)
    {
        result = ComaGetCommunicator();
    }

    if (ResultCode::kSuccess != result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(): Failed to init COMA", __FUNCTION__);
#ifndef VOC_TESTS
        raise(SIGTERM);
#endif
    }
}

VocmoSignalSource::~VocmoSignalSource()
{
    std::unique_lock<std::mutex> lock(mutex_);

    // Release COMA communicator
    if (communication_mgr_)
    {
        communication_mgr_->releaseCommunicator(communicator_->getCommunicatorId());
    }

    // Unregister from used signal sources
    if (receivers_count_)
    {
        ble_signal_source_->UnregisterSignalReceiver(static_cast<SignalReceiverInterface&>(*this));

        CarAccessSignalSource& car_access_signal_source = CarAccessSignalSource::GetInstance();
        car_access_signal_source.UnregisterSignalReceiver(static_cast<SignalReceiverInterface&>(*this));
    }
}

void VocmoSignalSource::GetAllPairedUsers(std::vector<UserId>& users)
{
    // Re-use the CarAccessSignalSource to not create redundant resources, it has a static UserManager instance
    CarAccessSignalSource& car_access_signal_source = CarAccessSignalSource::GetInstance();
    car_access_signal_source.GetAllPairedUsers(users);
}

VocmoSignalSource::ResultCode VocmoSignalSource::ComaGetCommunicationMgrProxy()
{
    ResultCode result = ResultCode::kSuccess;

    // Get COMA communication manager proxy if not present
    if (!communication_mgr_)
    {
        communication_mgr_ = ::createCommunicationManagerProxy("VocmoSignalSource", "libcomaclient.so");

        if (!communication_mgr_)
        {
            result = ResultCode::kFailed;
        }
    }

    // If not yet done - initialize the proxy
    if (ResultCode::kSuccess == result)
    {
        ICommunicationManagerProxy::InitialisationStatus init_status = communication_mgr_->getInitStatus();

        // Initialize the proxy only if status is not SUCCESS, SUCCESS means it's already initialized
        if (ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS != init_status)
        {
            result = ComaInitCommunicationMgrProxy();
        }
    }

    if (ResultCode::kSuccess == result)
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "%s(): COMA communication manager init success.",
                        __FUNCTION__);
    }
    else
    {
        communication_mgr_ = nullptr;  // We mustn't call destroy on COMA proxy because it's shared with ssm and transfermgr

        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): COMA communication manager init failed with ResultCode %d.",
                        __FUNCTION__, static_cast<int>(result));
    }

    return result;
}

VocmoSignalSource::ResultCode VocmoSignalSource::ComaInitCommunicationMgrProxy()
{
    ResultCode result = ResultCode::kSuccess;
    bool init_result = false;
    RequestStatus coma_req_status;

    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();

    init_result = communication_mgr_->init(coma_req_status,
        std::bind(&VocmoSignalSource::ComaInitCommunicationMgrProxyCallback, this, std::placeholders::_1, cv));

    if (!init_result)
    {
        result = ResultCode::kFailed;
    }
    else
    {
        // Wait until either:
        // 1. cv was notified by init_callback, or
        // 2. cv->wait_for has timed_out
        bool wait_status = cv->wait_for(lock,
                                        std::chrono::seconds(kComaTimeout),
                                        [this]{ return coma_init_callback_received_; });

        if (!wait_status)
        {
            result = ResultCode::kTimeout;
        }
        else if (ICommunicationManagerProxy::INITIALISATIONSTATUS_SUCCESS != communication_mgr_->getInitStatus())
        {
            result = ResultCode::kFailed;
        }
    }

    if (ResultCode::kSuccess == result)
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "%s(): COMA communication manager init success.",
                        __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): COMA communication manager init failed, ResultCode=%d.",
                        __FUNCTION__, static_cast<int>(result));
    }

    return result;
}

void VocmoSignalSource::ComaInitCommunicationMgrProxyCallback(ResponseComaInitClient& response,
                                                              std::shared_ptr<std::condition_variable> cv)
{
    coma_init_callback_received_ = true;

    if (Status::STATUS_SUCCESS != response.status())
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): COMA communication manager init failed, status=%d.",
                        __FUNCTION__,
                        static_cast<int>(response.status()));
    }

    cv->notify_one();
}

VocmoSignalSource::ResultCode VocmoSignalSource::ComaGetCommunicator()
{
    ResultCode result = ResultCode::kSuccess;

    ResourceDescriptor res_descriptor;
    res_descriptor.m_type = ResourceDescriptor::RES_TYPE_CALL;

    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();

    communication_mgr_->getCommunicator(res_descriptor, communicator_,
        std::bind(&VocmoSignalSource::ComaGetCommunicatorCallback, this, std::placeholders::_1, cv));

    bool wait_status = cv->wait_for(lock,
            std::chrono::seconds(kComaTimeout),
            [this]{ return get_communicator_callback_received_; });

    if (!wait_status)
    {
        result = ResultCode::kTimeout;
    }
    else if (!communicator_activated_)
    {
        result = ResultCode::kFailed;
    }

    if (ResultCode::kSuccess == result)
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "%s(): COMA get communicatior success.",
                        __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): COMA get communicatior failed, ResultCode=%d.",
                        __FUNCTION__, static_cast<int>(result));
    }

    return result;
}

void VocmoSignalSource::ComaGetCommunicatorCallback(ICommunicatorProxy::EventCommunicatorActivationChanged& event,
                                                    std::shared_ptr<std::condition_variable> cv)
{
    if (true == event.activated)
    {
        communicator_activated_ = true;
    }

    get_communicator_callback_received_ = true;

    cv->notify_one();
}

void VocmoSignalSource::RegisterSignalReceiver(SignalReceiverInterface& signal_receiver)
{
    std::lock_guard<std::mutex> lg(mutex_);

    // Call the parent class function
    SignalSource::RegisterSignalReceiver(signal_receiver);

    if (receivers_count_ == 0)
    {
        ble_signal_source_ = new BleSignalSource;
        ble_signal_source_->RegisterSignalReceiver(static_cast<SignalReceiverInterface&>(*this));

        CarAccessSignalSource& car_access_signal_source = CarAccessSignalSource::GetInstance();
        car_access_signal_source.RegisterSignalReceiver(static_cast<SignalReceiverInterface&>(*this));

        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "VocmoSignalSource::%s: First signal receiver added",
                        __FUNCTION__);
    }

    ++receivers_count_;
}

void VocmoSignalSource::UnregisterSignalReceiver(SignalReceiverInterface& signal_receiver)
{
    std::lock_guard<std::mutex> lg(mutex_);

    // Call the parent class function
    SignalSource::UnregisterSignalReceiver(signal_receiver);

    if (receivers_count_ > 0)
    {
        --receivers_count_;
    }

    if (receivers_count_ == 0)
    {
        ble_signal_source_->UnregisterSignalReceiver(static_cast<SignalReceiverInterface&>(*this));
        delete ble_signal_source_;

        CarAccessSignalSource& car_access_signal_source = CarAccessSignalSource::GetInstance();
        car_access_signal_source.UnregisterSignalReceiver(static_cast<SignalReceiverInterface&>(*this));

        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "VocmoSignalSource::%s: Last signal receiver removed",
                        __FUNCTION__);
    }
}

void VocmoSignalSource::ProcessSignal(std::shared_ptr<Signal> signal)
{
    DLT_LOG_STRINGF(dlt_libfsm,
                    DLT_LOG_INFO,
                    "VocmoSignalSource::%s received signal: %s, broadcasting to receivers.",
                    __FUNCTION__, signal->ToString().c_str());

    BroadcastSignal(signal);
}

#if 0
bool VocmoSignalSource::GeelySendMessage(std::shared_ptr<fsm::Signal> signal, int priority)
{
    DLT_LOG_STRING(dlt_libfsm,
                    DLT_LOG_ERROR,
                    "ERROR!!!!!!!!!!!!!!!!!!! should not running here, "
                    "VocmoSignalSource::GeelySendMessage(std::shared_ptr<fsm::Signal> signal, int priority)");

    assert(0);
    return false;
}
#endif

bool VocmoSignalSource::GeelySendMessage(std::shared_ptr<fsm::SignalPack> signal, bool sms, int priority)
{
    bool result = true;

    DLT_LOG_STRING(dlt_libfsm,
                    DLT_LOG_DEBUG,
                    "VocmoSignalSource::GeelySendMessage___ called");


    RequestStatus request_status;
    SendDescription send_description;

    if (communicator_)
    {
        CarAccessSignalSource& car_access_signal_source = CarAccessSignalSource::GetInstance();
        fsm::Signal *tmp_signal = signal->GetSignal();
        if(tmp_signal == NULL)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "VocmoSignalSource::%s,signal->GetSignal() is NULL", __FUNCTION__);
            return false;
        }
        std::string topic = car_access_signal_source.GetUplinkTopic(tmp_signal->GetSignalType());
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VocmoSignalSource::%s,topic:%s\n", __FUNCTION__,topic.c_str());

        send_description.mutable_sendmethodmqtt()->set_mqtttopic(topic);
        send_description.mutable_sendmethodmqtt()->set_activated(true);

        send_description.mutable_sendmethodbluetoothle()->set_activated(true);

        send_description.mutable_sendmethodsms()->set_activated(sms);
        std::string temp_isdn = read_isdn();
        if (temp_isdn[temp_isdn.length() - 1] == '\n')
        {
            DLT_LOG_STRINGF(dlt_libfsm,
                                DLT_LOG_ERROR,
                                "%s(): temp_isdn is end with n",
                                __FUNCTION__);    
            temp_isdn = temp_isdn.substr(0, temp_isdn.length() - 1);
        }
        send_description.mutable_sendmethodsms()->set_phonenumbersmsc(temp_isdn.c_str());  //lijing-test , todo

        //lijing-test todo
        send_description.add_useridlist(1);

        ICommunicatorData* communicator_data = communicator_->getICommunicatorData();

        std::lock_guard<std::mutex> lg(mutex_);

        // Reset send_response_received_ before sending
        send_response_received_ = false;

        // Reset signal_encoding_failed_ before sending
        signal_encoding_failed_ = false;

        std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();

        // Send the message
        communicator_data->sendMessage(request_status,
                send_description,
                std::bind(&VocmoSignalSource::GeelyHandleSendMessageCallback, this, std::placeholders::_1, signal, cv),
                std::bind(&VocmoSignalSource::GeelyHandleSendMessageResponse, this, std::placeholders::_1, cv));

        std::mutex cv_mtx;
        std::unique_lock<std::mutex> cv_lock(cv_mtx);

        // Wait until:
        // 1. time-out, or
        // 2. signal encoding fails in the pack callback, or
        // 3. response callback is received
        bool wait_status = cv->wait_for(cv_lock, std::chrono::seconds(kComaTimeout60),
                                        [this]{ return (send_response_received_ || signal_encoding_failed_); });

        // Time-out
        if (!wait_status)
        {
            result = false;

            DLT_LOG_STRINGF(dlt_libfsm,
                            DLT_LOG_ERROR,
                            "%s(): Send failed, time-out when waiting for sendMessage().",
                            __FUNCTION__);
        }
        else
        {
            // Encode failed in the pack callback
            if (signal_encoding_failed_)
            {
                result = false;

                DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): Signal encode failed.",
                        __FUNCTION__);
            }
            // Response callback received
            else if (send_response_received_)
            {
                if (com::contiautomotive::communicationmanager::messaging::internal::Status::STATUS_SUCCESS !=
                        send_status_)
                {
                    result = false;

                    DLT_LOG_STRINGF(dlt_libfsm,
                            DLT_LOG_ERROR,
                            "%s(): Send failed, Status=%d.",
                            __FUNCTION__, static_cast<int>(send_status_));
                }
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): Status=%d. send_response_received_=%d",
                        __FUNCTION__, static_cast<int>(send_status_), send_response_received_);
            }
        }
    }
    else
    {
        result = false;

        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): Send failed, ICommunicator is not initialized.",
                        __FUNCTION__);
    }

    if (result)
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "%s(): Send success.",
                        __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "%s(): Send failed.",
                        __FUNCTION__);
    }


    return result;
}

void VocmoSignalSource::GeelyHandleSendMessageCallback(CallbackCommunicatorDataSendMessageToUsers& callback_data,
                                                  std::shared_ptr<fsm::SignalPack> signal,
                                                  std::shared_ptr<std::condition_variable> cv)
{
    // Get the recipients list and set it in the signal before encoding

    DLT_LOG_STRING(dlt_libfsm,
        DLT_LOG_DEBUG,
        "VocmoSignalSource::GeelyHandleSendMessageCallback___, called");

    long send_method = callback_data.sendmethod();
    DLT_LOG_STRINGF(dlt_libfsm,
            DLT_LOG_DEBUG,
            "%s(): send_method = %d",
            __FUNCTION__, send_method);   

    std::shared_ptr<SignalPack> pack_signal = signal;
    if (pack_signal.get() == NULL)
    {
        DLT_LOG_STRING(dlt_libfsm,
            DLT_LOG_ERROR,
            "VocmoSignalSource::GeelyHandleSendMessageCallback___, pack_signal is null");
        signal_encoding_failed_ = true;
        return;
    }

    std::shared_ptr<std::vector<unsigned char>> encoded_signal;
    if (send_method == SendingMethod::SENDINGMETHOD_MQTT)
    {
        encoded_signal = SignalFactory::GeelyEncodeV1(pack_signal);
    }
    else if (send_method == SendingMethod::SENDINGMETHOD_SMS)
    {
        encoded_signal = SignalFactory::GlySmsEncodeV1(pack_signal);
    }
    else if (send_method == SendingMethod::SENDINGMETHOD_BLE)
    {
        encoded_signal = SignalFactory::GeelyEncodeV1(pack_signal);
    }
    else
    {
       DLT_LOG_STRINGF(dlt_libfsm,
           DLT_LOG_ERROR,
           "%s(), send_method error, = %d", __FUNCTION__, send_method);
       signal_encoding_failed_ = true;
       return;
    }
    // Restore the original recipients list
    // ccm_signal->SetRecipients(recipients_orig);

    if (encoded_signal && encoded_signal->size() != 0)
    {
        // COMA takes the payload as std::string so create one
        std::string callback_payload(encoded_signal->begin(), encoded_signal->end());

        // Finally put the string to callback_data
        callback_data.set_payload(callback_payload);
    }
    else
    {
        // Encode failed, notify the cv in SendMessage so that it returns with error immediately
        // instead of waiting for COMA response callback
        signal_encoding_failed_ = true;
        cv->notify_one();
        DLT_LOG_STRINGF(dlt_libfsm,
                DLT_LOG_ERROR,
                "%s(): signal_encoding_failed_=%d",
                __FUNCTION__, signal_encoding_failed_);

    }

}


void VocmoSignalSource::GeelyHandleSendMessageResponse(ResponseCommunicatorDataSendMessageToUsers& response,
                                                  std::shared_ptr<std::condition_variable> cv)
{
    send_status_ = response.status();
    send_response_received_ = true;
    DLT_LOG_STRINGF(dlt_libfsm,
        DLT_LOG_DEBUG,
        "%s(): Status=%d. send_response_received_=%d",
        __FUNCTION__, static_cast<int>(send_status_), send_response_received_);

    cv->notify_one();
}

bool VocmoSignalSource::SendMessage(std::shared_ptr<PayloadInterface> signal, int priority)
{
    bool result = true;

    RequestStatus request_status;
    SendDescription send_description;

    if (communicator_)
    {
        CarAccessSignalSource& car_access_signal_source = CarAccessSignalSource::GetInstance();

        std::string oid(signal ? signal->GetIdentifier().payload_identifier.oid : 0);
        std::string topic(car_access_signal_source.GetUplinkTopic(oid, priority));

        send_description.mutable_sendmethodmqtt()->set_mqtttopic(topic);
        send_description.mutable_sendmethodmqtt()->set_activated(true);

        send_description.mutable_sendmethodbluetoothle()->set_activated(true);

        // get recipients from PayloadInterface and add to to send_description
        PayloadInterface::CodecPayload codec_payload = PayloadInterface::CodecPayload();
        signal->GetEmptyPayload(codec_payload);
        std::vector<fsm::UserId>& recipients = codec_payload.codec_data.ccm_data.recipients;
        std::for_each(recipients.begin(), recipients.end(),
                [&send_description](UserId user_id) { send_description.add_useridlist(static_cast<int>(user_id)); });

        ICommunicatorData* communicator_data = communicator_->getICommunicatorData();

        std::lock_guard<std::mutex> lg(mutex_);

        // Reset send_response_received_ before sending
        send_response_received_ = false;

        // Reset signal_encoding_failed_ before sending
        signal_encoding_failed_ = false;

        std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();

        // Send the message
        communicator_data->sendMessage(request_status,
                send_description,
                std::bind(&VocmoSignalSource::HandleSendMessageCallback, this, std::placeholders::_1, signal, cv),
                std::bind(&VocmoSignalSource::HandleSendMessageResponse, this, std::placeholders::_1, cv));

        std::mutex cv_mtx;
        std::unique_lock<std::mutex> cv_lock(cv_mtx);

        // Wait until:
        // 1. time-out, or
        // 2. signal encoding fails in the pack callback, or
        // 3. response callback is received
        bool wait_status = cv->wait_for(cv_lock, std::chrono::seconds(kComaTimeout),
                                        [this]{ return (send_response_received_ || signal_encoding_failed_); });

        // Time-out
        if (!wait_status)
        {
            result = false;

            DLT_LOG_STRINGF(dlt_libfsm,
                            DLT_LOG_ERROR,
                            "%s(): Send failed, time-out when waiting for sendMessage().",
                            __FUNCTION__);
        }
        else
        {
            // Encode failed in the pack callback
            if (signal_encoding_failed_)
            {
                result = false;

                DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): Signal encode failed.",
                        __FUNCTION__);
            }
            // Response callback received
            else if (send_response_received_)
            {
                if (com::contiautomotive::communicationmanager::messaging::internal::Status::STATUS_SUCCESS !=
                        send_status_)
                {
                    result = false;

                    DLT_LOG_STRINGF(dlt_libfsm,
                            DLT_LOG_ERROR,
                            "%s(): Send failed, Status=%d.",
                            __FUNCTION__, static_cast<int>(send_status_));
                }
            }
        }
    }
    else
    {
        result = false;

        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): Send failed, ICommunicator is not initialized.",
                        __FUNCTION__);
    }

    if (result)
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "%s(): Send success.",
                        __FUNCTION__);
    }

    return result;
}

void VocmoSignalSource::HandleSendMessageCallback(CallbackCommunicatorDataSendMessageToUsers& callback_data,
                                                  std::shared_ptr<PayloadInterface> signal,
                                                  std::shared_ptr<std::condition_variable> cv)
{
    // Get the recipients list and set it in the signal before encoding
    std::vector<UserId> recipients;

    for(int i = 0; i < callback_data.useridlist_size(); ++i)
    {
        recipients.push_back(static_cast<UserId>(callback_data.useridlist(i)));
    }

    std::shared_ptr<CCMEncodable> ccm_signal = std::dynamic_pointer_cast<CCMEncodable>(signal);

    if (ccm_signal)
    {
        // Store the signal's original list of recipients and restore it after the encoding is finished, otherwise
        // if COMA has sent us callback_data with subset of original recipients list, the original signal's recipients
        // list would be overwritten.
        std::vector<UserId> recipients_orig = ccm_signal->GetRecipients();

        ccm_signal->SetRecipients(recipients);

        // Encode
        std::shared_ptr<std::vector<unsigned char>> encoded_signal = SignalFactory::Encode(signal);

        // Restore the original recipients list
        ccm_signal->SetRecipients(recipients_orig);

        if (encoded_signal)
        {
            // COMA takes the payload as std::string so create one
            std::string callback_payload(encoded_signal->begin(), encoded_signal->end());

            // Finally put the string to callback_data
            callback_data.set_payload(callback_payload);
        }
        else
        {
            // Encode failed, notify the cv in SendMessage so that it returns with error immediately
            // instead of waiting for COMA response callback
            signal_encoding_failed_ = true;
            cv->notify_one();
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "%s(): Incorrect signal type, can't encode.",
                        __FUNCTION__);
    }
}

void VocmoSignalSource::HandleSendMessageResponse(ResponseCommunicatorDataSendMessageToUsers& response,
                                                  std::shared_ptr<std::condition_variable> cv)
{
    send_status_ = response.status();
    send_response_received_ = true;
    cv->notify_one();
}

bool VocmoSignalSource::GetSimCurrentProfile(fsm::MTAData_t& mta)
{
   ResultCode result = ResultCode::kSuccess;

   if (communication_mgr_ != 0)
   {
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO,"mta GetMTAData");
    {
        RequestStatus requestStatus;
        std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);
        send_response_received_ = false;

        std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
        communication_mgr_->getNadIfController()->getSimCurrentProfileData(requestStatus,std::bind(&VocmoSignalSource::HandleGetSimCurrentProfile, this, _1, std::ref(mta), cv));

        bool wait_status = cv->wait_for(lock,std::chrono::seconds(kComaTimeout60),[this]{ return send_response_received_; });
        if (!wait_status)
        {
           result = ResultCode::kTimeout;
        }
        else if (!communicator_activated_)
        {
           result = ResultCode::kFailed;
        }

        if (ResultCode::kSuccess == result)
        {
            DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_INFO,"%s(): mta success.",__FUNCTION__);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_ERROR,"%s(): mta failed, ResultCode=%d.",__FUNCTION__, static_cast<int>(result));
        }
    }
  }
  else
  {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): Send failed, ICommunicationManager is not initialized.",
                        __FUNCTION__);
        result = ResultCode::kFailed;
  }

  return result == ResultCode::kSuccess;
}

void VocmoSignalSource::HandleGetSimCurrentProfile(NADIF_GET_SIM_CURRENT_PROFILE_INFO_RESP& response,
                                                   fsm::MTAData_t& mta,
                                                   std::shared_ptr<std::condition_variable> cv)
{
    DLT_LOG_STRINGF(dlt_libfsm,
        DLT_LOG_INFO, "%s: mta Send success. phone_number:%s imsi:%s sim_serial_number:%s sim_carrier_name:%s",
        __FUNCTION__,
        response.sim_profile_info.phone_number,
        response.sim_profile_info.imsi,
        response.sim_profile_info.sim_serial_number,
        response.sim_profile_info.sim_carrier_name );

     //maybe need check response items maxlength?
     mta.icc_id =(char*)&response.sim_profile_info.sim_serial_number;
     mta.imsi = (char*)&response.sim_profile_info.imsi;
     mta.msisdn = (char*)&response.sim_profile_info.phone_number;

     send_response_received_ = true;
     cv->notify_one();
}

bool VocmoSignalSource::GetNadIfNetworkSignalQuality(MTAData_t& mta)
{
   ResultCode result = ResultCode::kSuccess;

   if (communication_mgr_ != 0)
   {
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO,"mta GetNadIfNetworkSignalQuality");
    {
        RequestStatus requestStatus;
        std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);
        send_response_received_ = false;

        std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
        communication_mgr_->getNadIfController()->getNetworkSignalQualityData(requestStatus,std::bind(&VocmoSignalSource::HandleCallbackNetworkSignalQuality, this, _1, std::ref(mta), cv));
        bool wait_status = cv->wait_for(lock,std::chrono::seconds(kComaTimeout60),[this]{ return send_response_received_; });
        if (!wait_status)
        {
           result = ResultCode::kTimeout;
        }
        else if (!communicator_activated_)
        {
           result = ResultCode::kFailed;
        }

        if (ResultCode::kSuccess == result)
        {
            DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_INFO,"%s(): mta success.",__FUNCTION__);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm,DLT_LOG_ERROR,"%s(): mta failed, ResultCode=%d.",__FUNCTION__, static_cast<int>(result));
        }
    }
  }
  else
  {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "%s(): Send failed, ICommunicationManager is not initialized.",
                        __FUNCTION__);
        result = ResultCode::kFailed;
  }

  return result == ResultCode::kSuccess;
}

void VocmoSignalSource::HandleCallbackNetworkSignalQuality(NADIF_GET_SIG_QUAL_RESP& response,
                                                      MTAData_t& mta,
                                                      std::shared_ptr<std::condition_variable> cv)
{
   DLT_LOG_STRING(dlt_libfsm,DLT_LOG_INFO,"mta HandleCallbackNetworkSignalQuality");
   mta.mobile_network = "unknown";

   if(response.result == NADIF_RESULT_SUCCESS)
   {
       // how to select a  mobile_network ?
       // 0:NADIF_CDMA
       // 1:NADIF_EVDO
       // 2:NADIF_GSM
       // 3:NADIF_WCDMA
       // 4:NADIF_LTE
       for(int i = (NADIF_MAX_NETWORK_TYPES-1) ;i >=0 ; --i)
       {
           Print_network_sig_status(response.sig_qual_data[i]);

           switch(response.sig_qual_data[i].network_type)
           {
             case NADIF_NETWORK_TYPE_UNKNOWN:
                mta.mobile_network = "unknown";
                break;
             case NADIF_NETWORK_CDMA_1X:
                mta.mobile_network = "3G";
                break;
             case NADIF_NETWORK_CDMA_EVDO:
                mta.mobile_network = "3G";
                break;
             case NADIF_NETWORK_WCDMA:
                mta.mobile_network = "3G";
                break;
             case NADIF_NETWORK_LTE:
                mta.mobile_network = "4G";
                break;
             case NADIF_NETWORK_GSM:
                mta.mobile_network = "2G";
                break;
             default:
                mta.mobile_network = "unknown";
                break;
            }

           if(response.sig_qual_data[i].network_type != NADIF_NETWORK_TYPE_UNKNOWN)
           {
             break;
           }
       }
   }

   send_response_received_ = true;
   cv->notify_one();
}

std::string VocmoSignalSource::read_isdn()
{
    std::string isdn;
    isdn.clear();
    FILE * pFile = fopen ("/data/lijing.txt","r");
    if (pFile == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm,
            DLT_LOG_ERROR,
            "%s(): get isdn error, open file error, return default phone number, 18502163077", __FUNCTION__);
        isdn = "18502163077";
        return isdn;
    }
    char mystring [100];
    if ( fgets (mystring , 100 , pFile) != NULL )
    {
        isdn = mystring;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm,
            DLT_LOG_ERROR,
            "%s(): get isdn error, return default phone number, 18502163077", __FUNCTION__);
        isdn = "18502163077";
    }

    
    fclose (pFile);
    return isdn;
}


} // namespace fsm
/** \}    end of addtogroup */
