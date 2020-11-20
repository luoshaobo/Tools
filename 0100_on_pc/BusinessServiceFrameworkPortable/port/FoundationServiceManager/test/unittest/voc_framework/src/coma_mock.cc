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
 *  \file     coma_mock.cc
 *  \brief    CommunicationManager mock
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup unittest
 *  \{
 */

#include "coma_mock.h"

#include <thread>

#include "voc_framework/signal_sources/ble_signal_source.h"


/**
 * CommunicationManagerProxyMock implementation
 */
CommunicationManagerProxyMock::CommunicationManagerProxyMock()
{
    coma_fake_ = new CommunicationManagerProxyFake;
}

CommunicationManagerProxyMock::~CommunicationManagerProxyMock()
{
    delete coma_fake_;
}

ICommunicationManagerProxy* createCommunicationManagerProxy(std::string name, std::string libraryPathFile)
{
    static CommunicationManagerProxyMock coma_mgr_proxy_mock;
    return static_cast<ICommunicationManagerProxy*>(&coma_mgr_proxy_mock);
}

CommunicationManagerProxyFake* CommunicationManagerProxyMock::GetCommunicationManagerProxyFake()
{
    return coma_fake_;
}

/**
 * CommunicationManagerProxyFake implementation
 */
CommunicationManagerProxyFake::CommunicationManagerProxyFake()
{
    communicator_mock_ = new CommunicatorProxyMock;
}

CommunicationManagerProxyFake::~CommunicationManagerProxyFake()
{
    delete communicator_mock_;
}

CommunicatorProxyMock* CommunicationManagerProxyFake::GetCommunicatorProxyMock()
{
    return communicator_mock_;
}

void CommunicationManagerProxyFake::SetFakeInitCallbackResponse(ResponseComaInitClient& response)
{
    coma_com_mgr_init_callback_response_ = response;
}

void CommunicationManagerProxyFake::SendEvent(const NotificationBluetoothLEIncomingMessage* event)
{
    if (ble_message_receiver_)
    {
        std::thread td(std::bind(&fsm::BleSignalSource::handleEvent,
                                 static_cast<fsm::BleSignalSource*>(ble_message_receiver_),
                                 std::placeholders::_1),
                       event);

        td.join();
    }
}

void CommunicationManagerProxyFake::init(RequestStatus& requestStatus,
                                         std::function<void(ResponseComaInitClient&)> memberFunction)
{
    std::thread td(memberFunction, std::ref(coma_com_mgr_init_callback_response_));
    td.detach();
}

void CommunicationManagerProxyFake::init_timeout(RequestStatus& requestStatus,
                                         std::function<void(ResponseComaInitClient&)> memberFunction)
{
    std::thread td(std::bind(&CommunicationManagerProxyFake::init_timeout_thread, this, std::placeholders::_1), memberFunction);
    td.detach();
}

void CommunicationManagerProxyFake::init_timeout_thread(
        std::function<void(ResponseComaInitClient&)> memberFunction)
{
    // Delay executing the callback so that the test object times-out
    std::this_thread::sleep_for (std::chrono::seconds(fsm::BleSignalSource::kComaTimeout + 1));
    memberFunction(coma_com_mgr_init_callback_response_);
}

void CommunicationManagerProxyFake::getCommunicator(ResourceDescriptor& descriptor,
        ICommunicatorProxy*& pCommunicator,
        std::function<void(ICommunicatorProxy::EventCommunicatorActivationChanged&)> callback)
{
    pCommunicator = static_cast<ICommunicatorProxy*>(communicator_mock_);

    std::thread td(callback, std::ref(coma_get_comunicator_callback_event_));
    td.detach();
}

void CommunicationManagerProxyFake::attachEvent(ICommunicationManagerProxyEvents::EventType type,
                                                ICommunicationManagerProxyEvents* callback)
{
    switch(type)
    {
        case ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_MESSAGE_ARRIVED:
        {
            ble_message_receiver_ = callback;
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 * CommunicatorProxyMock implementation
 */
CommunicatorProxyMock::CommunicatorProxyMock()
{
    communicator_data_mock_ = new CommunicatorDataMock;

    ON_CALL(*this, getICommunicatorData())
        .WillByDefault(::testing::Return(communicator_data_mock_));
}

CommunicatorProxyMock::~CommunicatorProxyMock()
{
    delete communicator_data_mock_;
}

CommunicatorDataMock* CommunicatorProxyMock::GetCommunicatorDataMock()
{
    return communicator_data_mock_;
}


/**
 * CommunicatorDataMock implementation
 */
CommunicatorDataMock::CommunicatorDataMock()
{
    communicator_data_fake_ = new CommunicatorDataFake;
}

CommunicatorDataMock::~CommunicatorDataMock()
{
    delete communicator_data_fake_;
}

CommunicatorDataFake* CommunicatorDataMock::GetCommunicatorDataFake()
{
    return communicator_data_fake_;
}


/**
 * CommunicatorDataFake implementation
 */
void CommunicatorDataFake::SetFakeCallbackParameter(CallbackCommunicatorDataSendMessageToUsers& parameter)
{
    callback_parameter_ = &parameter;
}

void CommunicatorDataFake::SetFakeResponseParameter(ResponseCommunicatorDataSendMessageToUsers& parameter)
{
    response_parameter_ = &parameter;
}

void CommunicatorDataFake::sendMessage(RequestStatus& requestStatus,
                                       SendDescription& send_description,
                                       std::function<void(CallbackCommunicatorDataSendMessageToUsers&)> callback,
                                       std::function<void(ResponseCommunicatorDataSendMessageToUsers&)> response)
{
    std::thread pack_callback_thread1(callback, std::ref(*callback_parameter_));
    pack_callback_thread1.detach();

    std::thread pack_callback_thread2(std::bind(&CommunicatorDataFake::callback_delayed, this, std::placeholders::_1),
                                      callback);
    pack_callback_thread2.detach();

    std::thread response_thread(std::bind(&CommunicatorDataFake::response_delayed, this, std::placeholders::_1), response);
    response_thread.detach();
}

void CommunicatorDataFake::callback_delayed(std::function<void(CallbackCommunicatorDataSendMessageToUsers&)> callback)
{
    // Delay executing the callback so that it comes after the first pack callback but before response callback
    std::this_thread::sleep_for (std::chrono::seconds(1));
    callback(*callback_parameter_);
}

void CommunicatorDataFake::response_delayed(std::function<void(ResponseCommunicatorDataSendMessageToUsers&)> response)
{
    // Delay executing the response callback so that it comes after the pack callback
    std::this_thread::sleep_for (std::chrono::seconds(2));
    response(*response_parameter_);
}


/**
 * RequestStatus (communicationmanagerrequeststatus.h) stubs
 */

RequestStatus::RequestStatus() {}


/**
 * Below there are stubs from ipcmessages.pb.h
 */

namespace com {
namespace contiautomotive {
namespace communicationmanager {
namespace messaging {
namespace internal {

/**
 * NotificationBluetoothLEIncomingMessage stubs
 */

NotificationBluetoothLEIncomingMessage::NotificationBluetoothLEIncomingMessage()
{
  SharedCtor();
}

void NotificationBluetoothLEIncomingMessage::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  receiveruserid_ = 0;
  payload_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

NotificationBluetoothLEIncomingMessage::~NotificationBluetoothLEIncomingMessage() {
  SharedDtor();
}

void NotificationBluetoothLEIncomingMessage::SharedDtor() {
  if (payload_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete payload_;
  }
}

NotificationBluetoothLEIncomingMessage* NotificationBluetoothLEIncomingMessage::New() const
{
    return new NotificationBluetoothLEIncomingMessage;
}

void NotificationBluetoothLEIncomingMessage::Clear() {}
bool NotificationBluetoothLEIncomingMessage::IsInitialized() const { return false; }

bool NotificationBluetoothLEIncomingMessage::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) { return false; }

int NotificationBluetoothLEIncomingMessage::ByteSize() const { return 0; }

void NotificationBluetoothLEIncomingMessage::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {}

::google::protobuf::uint8* NotificationBluetoothLEIncomingMessage::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const { return nullptr; }

void NotificationBluetoothLEIncomingMessage::CopyFrom(const ::google::protobuf::Message& from) {}
void NotificationBluetoothLEIncomingMessage::MergeFrom(const ::google::protobuf::Message& from) {}
void NotificationBluetoothLEIncomingMessage::SetCachedSize(int size) const {}

::google::protobuf::Metadata NotificationBluetoothLEIncomingMessage::GetMetadata() const {
    ::google::protobuf::Metadata metadata;
    return metadata;
}

/**
 * MqttTopic stubs
 */

MqttTopic::MqttTopic(MqttTopic const&) {}
MqttTopic::~MqttTopic() {}

MqttTopic* MqttTopic::New() const
{
    return nullptr;
}

void MqttTopic::Clear() {}

bool MqttTopic::IsInitialized() const
{
    return false;
}

bool MqttTopic::MergePartialFromCodedStream(::google::protobuf::io::CodedInputStream* input)
{
    return false;
}

int MqttTopic::ByteSize() const
{
    return 0;
}

void MqttTopic::SerializeWithCachedSizes(::google::protobuf::io::CodedOutputStream* output) const {}

::google::protobuf::uint8* MqttTopic::SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const
{
    return nullptr;
}

void MqttTopic::CopyFrom(const ::google::protobuf::Message& from) {}
void MqttTopic::MergeFrom(const ::google::protobuf::Message& from) {}
void MqttTopic::SetCachedSize(int size) const {}

::google::protobuf::Metadata MqttTopic::GetMetadata() const
{
    return ::google::protobuf::Metadata();
}


/**
 * MqttTopicIdentifier stubs
 */

MqttTopicIdentifier::MqttTopicIdentifier(MqttTopicIdentifier const&) {}
MqttTopicIdentifier::~MqttTopicIdentifier() {}

MqttTopicIdentifier* MqttTopicIdentifier::New() const
{
    return nullptr;
}

void MqttTopicIdentifier::Clear() {}

bool MqttTopicIdentifier::IsInitialized() const
{
    return false;
}

bool MqttTopicIdentifier::MergePartialFromCodedStream(::google::protobuf::io::CodedInputStream* input)
{
    return false;
}

int MqttTopicIdentifier::ByteSize() const
{
    return 0;
}

void MqttTopicIdentifier::SerializeWithCachedSizes(::google::protobuf::io::CodedOutputStream* output) const {}

::google::protobuf::uint8* MqttTopicIdentifier::SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const
{
    return nullptr;
}

void MqttTopicIdentifier::CopyFrom(const ::google::protobuf::Message& from) {}
void MqttTopicIdentifier::MergeFrom(const ::google::protobuf::Message& from) {}
void MqttTopicIdentifier::SetCachedSize(int size) const {}

::google::protobuf::Metadata MqttTopicIdentifier::GetMetadata() const
{
    return ::google::protobuf::Metadata();
}


/**
 * ResponseComaInitClient stubs
 */

ResponseComaInitClient::ResponseComaInitClient() {}
ResponseComaInitClient::~ResponseComaInitClient() {}

void ResponseComaInitClient::CopyFrom(const ResponseComaInitClient& from)
{
    set_status(from.status());
}

bool Status_IsValid(int value)
{
    return true;
}

ResponseComaInitClient* ResponseComaInitClient::New() const
{
    return nullptr;
}

void ResponseComaInitClient::Clear() {}

bool ResponseComaInitClient::IsInitialized() const
{
    return false;
}

bool ResponseComaInitClient::MergePartialFromCodedStream(::google::protobuf::io::CodedInputStream* input)
{
    return false;
}

int ResponseComaInitClient::ByteSize() const
{
    return 0;
}

void ResponseComaInitClient::SerializeWithCachedSizes(::google::protobuf::io::CodedOutputStream* output) const {}

::google::protobuf::uint8* ResponseComaInitClient::SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const
{
    return nullptr;
}

void ResponseComaInitClient::CopyFrom(const ::google::protobuf::Message& from) {}
void ResponseComaInitClient::MergeFrom(const ::google::protobuf::Message& from) {}
void ResponseComaInitClient::SetCachedSize(int size) const {}

::google::protobuf::Metadata ResponseComaInitClient::GetMetadata() const
{
    return ::google::protobuf::Metadata();
}


/**
 * SendDescription stubs
 */

SendDescription::SendDescription()
{
    SharedCtor();
}

void SendDescription::SharedCtor()
{
    _cached_size_ = 0;
    sendmethodbluetoothle_ = NULL;
    sendmethodmqtt_ = NULL;
    sendmethodsms_ = NULL;
    ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

SendDescription::~SendDescription () {}

SendDescription* SendDescription::New() const
{
    return nullptr;
}

void SendDescription::Clear() {}
bool SendDescription::IsInitialized() const { return false; }

bool SendDescription::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) { return false; }

int SendDescription::ByteSize() const { return 0; }

void SendDescription::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {}

::google::protobuf::uint8* SendDescription::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const { return nullptr; }

void SendDescription::CopyFrom(const ::google::protobuf::Message& from) {}
void SendDescription::MergeFrom(const ::google::protobuf::Message& from) {}
void SendDescription::SetCachedSize(int size) const {}

::google::protobuf::Metadata SendDescription::GetMetadata() const {
    ::google::protobuf::Metadata metadata;
    return metadata;
}


/**
 * SendMethodBle stubs
 */

SendMethodBle::SendMethodBle() {}
SendMethodBle::~SendMethodBle() {}

SendMethodBle* SendMethodBle::New() const
{
    return nullptr;
}

void SendMethodBle::Clear() {}
bool SendMethodBle::IsInitialized() const { return false; }

bool SendMethodBle::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) { return false; }

int SendMethodBle::ByteSize() const { return 0; }

void SendMethodBle::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {}

::google::protobuf::uint8* SendMethodBle::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const { return nullptr; }

void SendMethodBle::CopyFrom(const ::google::protobuf::Message& from) {}
void SendMethodBle::MergeFrom(const ::google::protobuf::Message& from) {}
void SendMethodBle::SetCachedSize(int size) const {}

::google::protobuf::Metadata SendMethodBle::GetMetadata() const {
    ::google::protobuf::Metadata metadata;
    return metadata;
}


/**
 * SendMethodMqtt stubs
 */

SendMethodMqtt::SendMethodMqtt() { mqtttopic_ = new std::string; }

SendMethodMqtt::~SendMethodMqtt() { delete mqtttopic_; }

SendMethodMqtt* SendMethodMqtt::New() const
{
    return new SendMethodMqtt;
}

void SendMethodMqtt::Clear() {}
bool SendMethodMqtt::IsInitialized() const { return false; }

bool SendMethodMqtt::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) { return false; }

int SendMethodMqtt::ByteSize() const { return 0; }

void SendMethodMqtt::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {}

::google::protobuf::uint8* SendMethodMqtt::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const { return nullptr; }

void SendMethodMqtt::CopyFrom(const ::google::protobuf::Message& from) {}
void SendMethodMqtt::MergeFrom(const ::google::protobuf::Message& from) {}
void SendMethodMqtt::SetCachedSize(int size) const {}

::google::protobuf::Metadata SendMethodMqtt::GetMetadata() const {
    ::google::protobuf::Metadata metadata;
    return metadata;
}


/**
 * CallbackCommunicatorDataSendMessageToUsers stubs
 */

CallbackCommunicatorDataSendMessageToUsers::CallbackCommunicatorDataSendMessageToUsers()
{
    payload_ = new std::string;
}

CallbackCommunicatorDataSendMessageToUsers::~CallbackCommunicatorDataSendMessageToUsers()
{
    delete payload_;
}

CallbackCommunicatorDataSendMessageToUsers* CallbackCommunicatorDataSendMessageToUsers::New() const
{
    return new CallbackCommunicatorDataSendMessageToUsers;
}

void CallbackCommunicatorDataSendMessageToUsers::Clear() {}
bool CallbackCommunicatorDataSendMessageToUsers::IsInitialized() const { return false; }

bool CallbackCommunicatorDataSendMessageToUsers::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) { return false; }

int CallbackCommunicatorDataSendMessageToUsers::ByteSize() const { return 0; }

void CallbackCommunicatorDataSendMessageToUsers::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {}

::google::protobuf::uint8* CallbackCommunicatorDataSendMessageToUsers::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const { return nullptr; }

void CallbackCommunicatorDataSendMessageToUsers::CopyFrom(const ::google::protobuf::Message& from) {}
void CallbackCommunicatorDataSendMessageToUsers::MergeFrom(const ::google::protobuf::Message& from) {}
void CallbackCommunicatorDataSendMessageToUsers::SetCachedSize(int size) const {}

::google::protobuf::Metadata CallbackCommunicatorDataSendMessageToUsers::GetMetadata() const {
    ::google::protobuf::Metadata metadata;
    return metadata;
}


/**
 * ResponseCommunicatorDataSendMessageToUsers stubs
 */

ResponseCommunicatorDataSendMessageToUsers::ResponseCommunicatorDataSendMessageToUsers() {}
ResponseCommunicatorDataSendMessageToUsers::~ResponseCommunicatorDataSendMessageToUsers() {}

ResponseCommunicatorDataSendMessageToUsers* ResponseCommunicatorDataSendMessageToUsers::New() const
{
    return new ResponseCommunicatorDataSendMessageToUsers;
}

void ResponseCommunicatorDataSendMessageToUsers::Clear() {}
bool ResponseCommunicatorDataSendMessageToUsers::IsInitialized() const { return false; }

bool ResponseCommunicatorDataSendMessageToUsers::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) { return false; }

int ResponseCommunicatorDataSendMessageToUsers::ByteSize() const { return 0; }

void ResponseCommunicatorDataSendMessageToUsers::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {}

::google::protobuf::uint8* ResponseCommunicatorDataSendMessageToUsers::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const { return nullptr; }

void ResponseCommunicatorDataSendMessageToUsers::CopyFrom(const ::google::protobuf::Message& from) {}
void ResponseCommunicatorDataSendMessageToUsers::MergeFrom(const ::google::protobuf::Message& from) {}
void ResponseCommunicatorDataSendMessageToUsers::SetCachedSize(int size) const {}

::google::protobuf::Metadata ResponseCommunicatorDataSendMessageToUsers::GetMetadata() const {
    ::google::protobuf::Metadata metadata;
    return metadata;
}


}  // namespace internal
}  // namespace messaging
}  // namespace communicationmanager
}  // namespace contiautomotive
}  // namespace com



/**
 * ::google::protobuf stubs
 */

namespace google {
namespace protobuf {

Closure::~Closure() {}
namespace internal { FunctionClosure0::~FunctionClosure0() {} }
void GoogleOnceInitImpl(ProtobufOnceType* once, Closure* closure) {}

// ::google::protobuf::Message stubs
Message::~Message() {}

string Message::GetTypeName() const
{
    return string();
}

void Message::Clear() {}

bool Message::IsInitialized() const
{
    return false;
}

string Message::InitializationErrorString() const
{
    return string();
}

void Message::CheckTypeAndMergeFrom(const MessageLite& other) {}

bool Message::MergePartialFromCodedStream(io::CodedInputStream* input)
{
    return false;
}

int Message::ByteSize() const
{
    return 0;
}

void Message::SerializeWithCachedSizes(io::CodedOutputStream* output) const {}
void Message::CopyFrom(const Message& from) {}
void Message::MergeFrom(const Message& from) {}
void Message::DiscardUnknownFields() {}

int Message::SpaceUsed() const
{
    return 0;
}

void Message::SetCachedSize(int size) const {}


// ::google::protobuf::MessageLite stubs

MessageLite::~MessageLite() {}

string MessageLite::InitializationErrorString() const
{
    return string();
}

uint8* MessageLite::SerializeWithCachedSizesToArray(uint8* target) const
{
    return 0;
}


// ::google::protobuf::UnknownFieldSet stubs

UnknownFieldSet::UnknownFieldSet() {}
UnknownFieldSet::~UnknownFieldSet() {}


// ::google::protobuf::internal stubs

namespace internal {

const std::string* empty_string_ = new std::string;
ProtobufOnceType empty_string_once_init_;

void InitEmptyString() {
    empty_string_ = new string;
}

LogMessage::LogMessage(LogLevel level, const char* filename, int line) {}
LogMessage::~LogMessage() {}

LogMessage& LogMessage::operator<<(const char* value)
{
    return *this;
}

void LogFinisher::operator=(LogMessage& other) {}

}  // namespace internal

}  // namespace protobuf
}  // namespace google

/** \}    end of addtogroup */
