/**
* Copyright (C) 2016 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file lan_comm.cpp
 * This file handles communication with the LAN over vSomeIP
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include "vc_utility.hpp"
#include "lan_comm.hpp"

namespace vc {

// Private

/**
    @brief SomeIP-app main loop.

    This is the main-thread for LANBase, which handles the runtime of vSomeIP.
*/
void LANBase::AppLoop()
{
    LOG(LOG_DEBUG, "LANBase: %s (app = %s).", __FUNCTION__, app_->get_name().c_str());

    app_->offer_service(service_id_, instance_id_);
    app_->start();
}

// Methods for the SomeIP-app

void LANBase::OnState(vsomeip::state_type_e state_someip)
{
    LOG(LOG_DEBUG, "LANBase: %s (state_someip = %s).", __FUNCTION__,
            state_someip == vsomeip::state_type_e::ST_REGISTERED ? "Registered" : "Deregistered");

    LANBaseState new_state = state_;

    if ((state_someip == vsomeip::state_type_e::ST_REGISTERED) && (state_ != STATE_REGISTERED)) {
        if (state_ < STATE_STARTED)
            LOG(LOG_WARN, "LANBase: %s: SomeIP-state goes to ST_REGISTERED when LANBase state is in %s.",
                    __FUNCTION__, LANBaseStateStr[state_]);

        new_state = STATE_REGISTERED;
    };

    OnStateChange(new_state);

    ChangeState(new_state);
}

void LANBase::OnAvailability(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available)
{
    LOG(LOG_DEBUG, "LANBase: %s (service = 0x%x, available = %d.", __FUNCTION__, service, is_available);

    OnAvailabilityChange(service, instance, is_available);
}

void LANBase::OnMessage(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "LANBase: %s.", __FUNCTION__);

    // Request, Response-ack
    if ((msg->get_message_type() == vsomeip::message_type_e::MT_REQUEST) ||
        (msg->get_message_type() == vsomeip::message_type_e::MT_RESPONSE_ACK)) {
        LOG(LOG_DEBUG, "LANBase: %s: Invalid message received in client-part (message_type = %d)!",
                __FUNCTION__, msg->get_message_type());
        return;
    }

    // Event
    if (msg->get_message_type() == vsomeip::message_type_e::MT_NOTIFICATION) {
        LOG(LOG_INFO, "LANBase: %s: MT_NOTIFICATION received (method: %s[0x%x], ext_session: %d)!", __FUNCTION__,
                SomeIPIDStr[msg->get_method()], msg->get_method(), msg->get_session());

        OnEvent(msg);
        return;
    }

    // Request-ack
    if (msg->get_message_type() == vsomeip::message_type_e::MT_REQUEST_ACK) {
        LOG(LOG_INFO, "LANBase: %s: MT_REQUEST_ACK received (method: %s[0x%x], ext_session: %d)!", __FUNCTION__,
                SomeIPIDStr[msg->get_method()], msg->get_method(), msg->get_session());

        OnRequestAck(msg);
        return;
    }

    // Response
    if (msg->get_message_type() == vsomeip::message_type_e::MT_RESPONSE) {
        std::shared_ptr<vsomeip::message> ackmsg = vsomeip::runtime::get()->create_response_ack(msg);
        LOG(LOG_INFO, "LANBase: %s: MT_RESPONSE_ACK Sent (method: %s[0x%x], ext_session: %d)!", __FUNCTION__,
                SomeIPIDStr[msg->get_method()], msg->get_method(), msg->get_session());

        OnResponse(msg);
        return;
    }
}

void LANBase::OnMessage_Server(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "LANBase: %s.", __FUNCTION__);

    // Response, Request-ack
    if ((msg->get_message_type() == vsomeip::message_type_e::MT_RESPONSE) ||
        (msg->get_message_type() == vsomeip::message_type_e::MT_REQUEST_ACK)) {
        LOG(LOG_DEBUG, "LANBase: %s: Invalid message received in server-part (message_type = %d)!",
                __FUNCTION__, msg->get_message_type());
        return;
    }

    // Request
    if (msg->get_message_type() == vsomeip::message_type_e::MT_REQUEST) {
        LOG(LOG_DEBUG, "LANBase: %s: MT_REQUEST received (session = %d).", __FUNCTION__, msg->get_session());

        OnRequest(msg);
        return;
    }

    // Response-ack
    if (msg->get_message_type() == vsomeip::message_type_e::MT_RESPONSE_ACK) {
        LOG(LOG_DEBUG, "LANBase: %s: MT_RESPONSE_ACK received (session = %d).", __FUNCTION__, msg->get_session());
        return;
    }
}



// Protected

void LANBase::ChangeState(LANBaseState state_new)
{
    LOG(LOG_DEBUG, "LANBase: State changes from %s to %s", LANBaseStateStr[state_], LANBaseStateStr[state_new]);
    state_ = state_new;
}



// Public

LANBase::LANBase(int service_id, int instance_id)
        : app_(vsomeip::runtime::get()->create_application())
{
    LOG(LOG_DEBUG, "LANBase: %s.", __FUNCTION__);

    service_id_ = service_id;
    instance_id_ = instance_id;
}

LANBase::~LANBase()
{
    LOG(LOG_DEBUG, "LANBase: %s (app = %s).", __FUNCTION__, app_->get_name().c_str());

    Stop();

    mutex_.lock();

    ChangeState(STATE_EXITING);

    app_->unregister_state_handler();

    // Clean-up server-stuff
    app_->unregister_message_handler(service_id_, instance_id_, vsomeip::ANY_METHOD);
    app_->release_service(service_id_, instance_id_);

    mutex_.unlock();

    if(app_thread_.joinable())
        app_thread_.join();
}

RequestID LANBase::SomeIPMethodIDToRequestID(vsomeip::method_t vsomeip_id)
{
    RequestID request_id = REQ_UNKNOWN;
    for (int i = 0; i < LENGTH(someip_message_lookup); i++)
        if (someip_message_lookup[i].message_type == MESSAGE_REQUEST)
            if (someip_message_lookup[i].vsomeip_id == vsomeip_id)
                return (RequestID)someip_message_lookup[i].message_id;

    LOG(LOG_DEBUG, "SomeIPServer: %s: Unable to find matching response-ID (vsomeip_id = %s[%d])!",
            __FUNCTION__, SomeIPIDStr[vsomeip_id], vsomeip_id);
    return request_id;
}

vsomeip::method_t LANBase::RequestIDToSomeIPMethod(RequestID request_id)
{
    vsomeip::method_t method_id = 0;
    for (int i = 0; i < LENGTH(someip_message_lookup); i++)
        if (someip_message_lookup[i].message_type == MESSAGE_REQUEST)
            if (someip_message_lookup[i].message_id == (MessageID)request_id)
                return someip_message_lookup[i].vsomeip_id;

    LOG(LOG_DEBUG, "SomeIPServer: %s: Unable to find matching vSomeIP-ID (request_id = %s[%d])!",
            __FUNCTION__, VCRequestStr[request_id], request_id);
    return method_id;
};

ResponseID LANBase::SomeIPMethodIDToResponseID(vsomeip::method_t vsomeip_id)
{
    ResponseID response_id = RES_UNKNOWN;
    for (int i = 0; i < LENGTH(someip_message_lookup); i++)
        if (someip_message_lookup[i].message_type == MESSAGE_RESPONSE)
            if (someip_message_lookup[i].vsomeip_id == vsomeip_id)
                return (ResponseID)someip_message_lookup[i].message_id;

    LOG(LOG_DEBUG, "SomeIPServer: %s: Unable to find matching response-ID (vsomeip_id = %s[%d])!",
            __FUNCTION__, SomeIPIDStr[vsomeip_id], vsomeip_id);
    return response_id;
};

vsomeip::method_t LANBase::ResponseIDToSomeIPMethod(ResponseID response_id)
{
    vsomeip::method_t method_id = 0;
    for (int i = 0; i < LENGTH(someip_message_lookup); i++)
        if (someip_message_lookup[i].message_type == MESSAGE_RESPONSE)
            if (someip_message_lookup[i].message_id == (MessageID)response_id)
                return someip_message_lookup[i].vsomeip_id;

    LOG(LOG_DEBUG, "SomeIPServer: %s: Unable to find matching vSomeIP-ID (response_id = %s[%d])!",
            __FUNCTION__, ResponseStr[response_id], response_id);
    return method_id;
};

EventID LANBase::SomeIPEventIDToEventID(vsomeip::event_t vsomeip_id)
{
    EventID event_id = EVENT_UNKNOWN;
    for (int i = 0; i < LENGTH(someip_message_lookup); i++)
        if (someip_message_lookup[i].message_type == MESSAGE_EVENT)
            if (someip_message_lookup[i].vsomeip_id == vsomeip_id)
                return (EventID)someip_message_lookup[i].message_id;

    LOG(LOG_DEBUG, "SomeIPServer: %s: Unable to find matching event-ID (vsomeip_id = %s[%d])!",
            __FUNCTION__, SomeIPIDStr[vsomeip_id], vsomeip_id);
    return event_id;
}

vsomeip::event_t LANBase::EventIDToSomeIPEventID(EventID event_id)
{
    vsomeip::event_t vsomeip_id = 0;
    for (int i = 0; i < LENGTH(someip_message_lookup); i++)
        if (someip_message_lookup[i].message_type == MESSAGE_EVENT)
            if (someip_message_lookup[i].message_id == (MessageID)event_id)
                return someip_message_lookup[i].vsomeip_id;

    LOG(LOG_DEBUG, "SomeIPServer: %s: Unable to find matching vSomeIP-ID (event_id = %s[%d])!",
            __FUNCTION__, EventStr[event_id], event_id);
    return vsomeip_id;
};

const char* LANBase::MessageTypeToStr(vsomeip::message_type_e msgtype)
{
    switch (msgtype) {
    case vsomeip::message_type_e::MT_REQUEST:
        return "MT_REQUEST";
    case vsomeip::message_type_e::MT_REQUEST_NO_RETURN:
        return "MT_REQUEST_NO_RETURN";
    case vsomeip::message_type_e::MT_NOTIFICATION:
        return "MT_NOTIFICATION";
    case vsomeip::message_type_e::MT_REQUEST_ACK:
        return "MT_REQUEST_ACK";
    case vsomeip::message_type_e::MT_REQUEST_NO_RETURN_ACK:
        return "MT_REQUEST_NO_RETURN_ACK";
    case vsomeip::message_type_e::MT_NOTIFICATION_ACK:
        return "MT_NOTIFICATION_ACK";
    case vsomeip::message_type_e::MT_RESPONSE:
        return "MT_RESPONSE";
    case vsomeip::message_type_e::MT_ERROR:
        return "MT_ERROR";
    case vsomeip::message_type_e::MT_RESPONSE_ACK:
        return "MT_RESPONSE_ACK";
    case vsomeip::message_type_e::MT_ERROR_ACK:
        return "MT_ERROR_ACK";
    case vsomeip::message_type_e::MT_UNKNOWN:
        return "MT_UNKNOWN";
    default:
        LOG(LOG_WARN, "LANBase: Message type error");
        return "LANBase Message type error";
    }
}

const char* LANBase::ReturnCodeToStr(vsomeip::return_code_e returncode)
{
    switch (returncode) {
    case vsomeip::return_code_e::E_OK:
        return "E_OK";
    case vsomeip::return_code_e::E_NOT_OK:
        return "E_NOT_OK";
    case vsomeip::return_code_e::E_UNKNOWN_SERVICE:
        return "E_UNKNOWN_SERVICE";
    case vsomeip::return_code_e::E_UNKNOWN_METHOD:
        return "E_UNKNOWN_METHOD";
    case vsomeip::return_code_e::E_NOT_READY:
        return "E_NOT_READY";
    case vsomeip::return_code_e::E_NOT_REACHABLE:
        return "E_NOT_REACHABLE";
    case vsomeip::return_code_e::E_TIMEOUT:
        return "E_TIMEOUT";
    case vsomeip::return_code_e::E_WRONG_PROTOCOL_VERSION:
        return "E_WRONG_PROTOCOL_VERSION";
    case vsomeip::return_code_e::E_WRONG_INTERFACE_VERSION:
        return "E_WRONG_INTERFACE_VERSION";
    case vsomeip::return_code_e::E_MALFORMED_MESSAGE:
        return "E_MALFORMED_MESSAGE";
    case vsomeip::return_code_e::E_WRONG_MESSAGE_TYPE:
        return "E_WRONG_MESSAGE_TYPE";
    case vsomeip::return_code_e::E_UNKNOWN:
        return "E_UNKNOWN";
    default:
        LOG(LOG_WARN, "LANBase Message type error");
        return "LANBase Message type error";
    }
}

void LANBase::AddServiceHandler(vsomeip::service_t service_id, vsomeip::instance_t instance_id)
{
    LOG(LOG_DEBUG, "LANBase: %s (service = 0x%x).", __FUNCTION__, service_id);

    app_->register_availability_handler(service_id, instance_id, std::bind(&LANBase::OnAvailability, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    app_->register_message_handler(service_id, instance_id, vsomeip::ANY_METHOD, std::bind(&LANBase::OnMessage, this, std::placeholders::_1));
}

// Methods for the life-cycle

/**
    @brief Initialize LANBase.

    Do all initialization stuff needed for LANBase.
*/
ReturnValue LANBase::Init()
{
    LOG(LOG_DEBUG, "LANBase: %s.", __FUNCTION__);

    mutex_.lock();

    if (!app_->init()) {
        LOG(LOG_ERR, "LANBase: %s: Error initalizing vSomeIP!", __FUNCTION__);
        mutex_.unlock();
        return RET_ERR_EXTERNAL;
    }

    app_->register_state_handler(std::bind(&LANBase::OnState, this, std::placeholders::_1));
    app_->register_message_handler(service_id_, instance_id_, vsomeip::ANY_METHOD, std::bind(&LANBase::OnMessage_Server, this, std::placeholders::_1));

    ChangeState(STATE_INITIALIZED);

    mutex_.unlock();

    return RET_OK;
}

/**
    @brief Start execution of LANBase thread.

    Start the execution of the LANBase thread, responsible for sending incoming messages to
    the LAN, using the SendMessage-method, and also handle incoming messages from the LAN, using
    registered OnMessage-VGMRequestPair MessageLookup(RequestID request_idmethods.

    @return     A VC return-value
*/
ReturnValue LANBase::Start()
{
    LOG(LOG_DEBUG, "LANBase: %s.", __FUNCTION__);

    mutex_.lock();

    if (state_ != STATE_INITIALIZED)
        return RET_ERR_INVALID_STATE;

    app_thread_ = std::thread(&LANBase::AppLoop, this);

    ChangeState(STATE_STARTED);

    mutex_.unlock();
    
    return RET_OK;
}

/**
    @brief Stop execution of LANBase.

    Stop the SessionList thread so it can go out of context before destruction.

    @return     A VC return-value
*/
ReturnValue LANBase::Stop()
{
    LOG(LOG_DEBUG, "LANBase: %s.", __FUNCTION__);

    if (state_ < STATE_STARTED) 
        return RET_ERR_INVALID_STATE;

    mutex_.lock();

    app_->stop();
    app_->stop_offer_service(service_id_, instance_id_);

    ChangeState(STATE_INITIALIZED);

    mutex_.unlock();

   return RET_OK;
}

// Methods for the client-part

ReturnValue LANBase::RequestEvent(vsomeip::service_t service_id, std::set<vsomeip::eventgroup_t>&  eventgroups, EventID event_id)
{
    LOG(LOG_DEBUG, "LANComm: %s (event = %s[0x%x], service = 0x%x).", __FUNCTION__, EventStr[event_id], event_id, service_id);

    vsomeip::event_t someip_id = EventIDToSomeIPEventID((EventID)event_id);
    if (!someip_id) {
        LOG(LOG_DEBUG, "LANComm: %s: Client-event %s[0x%x] not found (service = 0x%x)!",
                __FUNCTION__, EventStr[event_id], event_id, service_id);
        return RET_ERR_INVALID_ARG;
    }
    LOG(LOG_DEBUG, "LANComm: %s: Client-event %s[0x%x] maps to someip-event %s[0x%x] (service = 0x%x).",
            __FUNCTION__, EventStr[event_id], event_id, SomeIPIDStr[someip_id], someip_id, service_id);
    
    app_->request_event(service_id, instance_id_, someip_id, eventgroups, false);
    
    return RET_OK;
}

ReturnValue LANBase::OfferEvent(vsomeip::service_t service_id, vsomeip::instance_t instance_id, 
                                vsomeip::eventgroup_t eventgroup_id, EventID event_id)
{
    LOG(LOG_DEBUG, "LANComm: %s (event = %s[0x%x], service = 0x%x).", __FUNCTION__, EventStr[event_id], event_id, service_id);

    vsomeip::event_t someip_id = EventIDToSomeIPEventID((EventID)event_id);
    if (!someip_id) {
        LOG(LOG_DEBUG, "LANComm: %s: Server-event %s[0x%x] not found (service = 0x%x)!",
                __FUNCTION__, EventStr[event_id], event_id, service_id);
        return RET_ERR_INVALID_ARG;
    }
    LOG(LOG_DEBUG, "LANComm: %s: Server-event %s[0x%x] maps to someip-event %s[0x%x] (service = 0x%x).",
            __FUNCTION__, EventStr[event_id], event_id, SomeIPIDStr[someip_id], someip_id, service_id);
    
    std::set<vsomeip::eventgroup_t> eventgroups;
    eventgroups.insert(eventgroup_id);
    app_->offer_event(service_id, instance_id, someip_id, eventgroups, false);
    
    return RET_OK;
}

ReturnValue LANBase::SendSomeIPMessageRaw(std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "LANBase: %s.", __FUNCTION__);

    app_->send(msg, true, true);

    return RET_OK;
}

ReturnValue LANBase::SendSomeIPEvent(vsomeip::event_t event_id, std::shared_ptr<vsomeip::payload> &data)
{
    LOG(LOG_DEBUG, "LANBase: %s (service_id = 0x%x, instance_id = 0x%x, event_id = 0x%x).", __FUNCTION__, service_id_, LAN_INSTANCE_ID, event_id);

    app_->notify(service_id_, LAN_INSTANCE_ID, event_id, data);

    return RET_OK;
}

ReturnValue LANBase::SendSomeIPMessageAck(std::shared_ptr<vsomeip::message> msg_ack)
{
    if (state_ != STATE_REGISTERED)
        return RET_ERR_INVALID_STATE;

    app_->send(msg_ack, true, false);

    return RET_OK;
}

void LANBase::CreateSomeIPPayload(MessageBase *m, std::shared_ptr<vsomeip::payload>& payload)
{
    // Create a vSomeIP payload from the Message-data
    payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(m->GetData(), (int)m->GetSize());
}

ReturnValue LANBase::CreateSomeIPRequest(vsomeip::service_t service_id, MessageBase *m, std::shared_ptr<vsomeip::message>& msg)
{
    vsomeip::method_t vsomeip_id = RequestIDToSomeIPMethod((RequestID)m->id_);
    if (!vsomeip_id)
        return RET_ERR_INVALID_ARG;

    std::shared_ptr<vsomeip::payload> payload;
    CreateSomeIPPayload(m, payload);

    msg = vsomeip::runtime::get()->create_request(false);
    msg->set_service(service_id);
    msg->set_instance(LAN_INSTANCE_ID);
    msg->set_payload(payload);
    msg->set_method(vsomeip_id);
    
    return RET_OK;
}

ReturnValue LANBase::SendMessage(vsomeip::service_t service_id, MessageBase *m)
{
    LOG(LOG_DEBUG, "LANBase: %s (service_id = 0x%x).", __FUNCTION__, service_id);

    if (m == NULL) {
        LOG(LOG_WARN, "LANBase: %s: Message is NULL!", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }

    if (state_ != STATE_REGISTERED) {
        return RET_ERR_INVALID_STATE;
    }
    
    switch (m->type_) {
    case MESSAGE_REQUEST: {
        LOG(LOG_DEBUG, "LANBase: %s: Sending request %s (unique_id = %d, src = %s, dst = %s).",
                __FUNCTION__, VCRequestStr[(RequestID)m->id_], m->unique_id_, EndpointStr[m->src_], EndpointStr[m->dst_]);
        return OnSendRequest(service_id, m);
    }
    case MESSAGE_EVENT: {
        LOG(LOG_DEBUG, "LANBase: %s: Sending event %s (unique_id = %d, src = %s).",
                __FUNCTION__, EventStr[m->id_], m->unique_id_, EndpointStr[m->src_]);
        return OnSendEvent(m);
    }
    default:
        LOG(LOG_WARN, "LANBase: %s: Unknown message-type (%d)!", __FUNCTION__, m->type_);
        return RET_ERR_INVALID_ARG;
    }

    return RET_ERR_INTERNAL;
}

} // namespace vc
