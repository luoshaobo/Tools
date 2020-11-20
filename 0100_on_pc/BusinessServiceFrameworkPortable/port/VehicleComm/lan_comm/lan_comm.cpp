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

#include "local_config_interface.hpp"

namespace vc {

// Private

ReturnValue LANComm::ResendSomeIPMessage(vsomeip::service_t service_id, MessageBase *m)
{
    // Create SomeIP-message
    std::shared_ptr<vsomeip::message> msg;
    ReturnValue ret = CreateSomeIPRequest(service_id, m, msg);
#if 1 //nieyj fix klocwork 166402
    if (!ret || (!msg))
#else
    if (!ret)
#endif
        return ret;
    
    ret = SendSomeIPMessageRaw(msg);
    if (!ret)
        slist_.UpdateExtSessionID((RequestID)m->id_, msg->get_session());

    return ret;
}

// Overrides - LANBase

// Methods for the server-part

void LANComm::OnStateChange(LANBaseState new_state)
{
    LOG(LOG_DEBUG, "LANComm: %s (new_state = %d)", __FUNCTION__, new_state);

    if (new_state == STATE_REGISTERED) {
        for (std::list<ServiceClient>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
            LOG(LOG_DEBUG, "LANComm: %s: Requesting client-service %s (service = 0x%x).", __FUNCTION__,
                    it->client_name.c_str(), it->service_id);
            app_->request_service(it->service_id, it->instance_id); 

            std::set<vsomeip::eventgroup_t> its_groups;
            its_groups.insert(it->eventgroup_id);
            
            for (std::vector<EventID>::const_iterator it_ev = it->events->begin(); it_ev != it->events->end(); ++it_ev)
                RequestEvent(it->service_id, its_groups, *it_ev);

            app_->subscribe(it->service_id, it->instance_id, it->eventgroup_id);
        }
        
        for (std::list<ServiceEvent>::const_iterator it = events.begin(); it != events.end(); ++it)
            OfferEvent(it->service_id, it->instance_id, it->eventgroup_id, it->event_id);
    }
}

void LANComm::OnRequest(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "LANComm: %s.", __FUNCTION__);

    UNUSED(msg);
}

// Methods for the client-part

void LANComm::OnAvailabilityChange(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available)
{
    LOG(LOG_DEBUG, "LANComm: %s (available = %d).", __FUNCTION__, is_available);

    UNUSED(instance);

    for (std::list<ServiceClient>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        if (service == it->service_id)
            it->HandleAvailability(is_available);
}

void LANComm::OnResponse(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "LANComm: %s.", __FUNCTION__);

    for (std::list<ServiceClient>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        if (msg->get_service() == it->service_id)
            it->HandleResponse(msg);
}

void LANComm::OnRequestAck(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "LANComm: %s.", __FUNCTION__);

    for (std::list<ServiceClient>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        if (msg->get_service() == it->service_id)
            it->HandleRequestAck(msg);
}

void LANComm::OnEvent(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "LANComm: %s (service = 0x%x, someip_id = %s).", __FUNCTION__, msg->get_service(),
            SomeIPIDStr[msg->get_method()]);

    for (std::list<ServiceClient>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        if (msg->get_service() == it->service_id)
            it->HandleEvent(msg);
}



// Protected

// Overrides - LANBase

ReturnValue LANComm::OnSendEvent(MessageBase *m)
{
    LOG(LOG_DEBUG, "LANComm: %s (type = %s, id = %s).", __FUNCTION__, MessageTypeStr[m->type_], MessageStr(m));

    if (state_ != STATE_REGISTERED)
        return RET_ERR_INVALID_STATE;

    vsomeip::event_t someip_id = EventIDToSomeIPEventID((EventID)m->id_);
    if (!someip_id)
        return RET_ERR_INVALID_ARG;

    std::shared_ptr<vsomeip::payload> payload;
    CreateSomeIPPayload(m, payload);
    return SendSomeIPEvent(someip_id, payload);
}

ReturnValue LANComm::OnSendRequest(vsomeip::service_t service_id, MessageBase *m)
{
    LOG(LOG_DEBUG, "LANComm: %s (type = %s, id = %s).", __FUNCTION__, MessageTypeStr[m->type_], MessageStr(m));

    ReturnValue ret;

    mutex_.lock();

    // Create SomeIP-message
    std::shared_ptr<vsomeip::message> msg;
    ret = CreateSomeIPRequest(service_id, m, msg);
#if 1 //nieyj fix klocwork 166403
    if (ret != RET_OK || (!msg))
#else
    if (ret != RET_OK)
#endif
        return ret;

    // Send SomeIP-message
    SendSomeIPMessageRaw(msg);

    ret = slist_.AddSession(m, msg->get_session(), AWAITING_ACK,
                        [this, service_id](MessageBase *m){return LANComm::ResendSomeIPMessage(service_id, m);});
    if (ret != RET_OK) {
        LOG(LOG_WARN, "LANComm: %s failed to add session: %s.", __FUNCTION__, ReturnValueStr[ret]);
        SendErrorMessage(mq_, m, ret);
        mutex_.unlock();
        return ret;
    }

    mutex_.unlock();
    return ret;
}



// Public

LANComm::LANComm(MessageQueue *mq)
        : LANBase(LAN_SERVICE_ID, LAN_INSTANCE_ID),
          slist_()
{
    LOG(LOG_DEBUG, "LANComm: %s.", __FUNCTION__);

    mq_ = mq;
    SessionCfgValues scfg = {};

    int value = ACK_TRIES;
    if (!lcfg::ILocalConfig::GetInstance()->GetInt("VehicleComm", "someip_ack_tries", value))
        LOG(LOG_WARN, "VuCComm: local_config value for someip_ack_tries, not found using default!");
    scfg.ack_tries = value;

    value = ACK_TIMEOUT_MS;
    if (!lcfg::ILocalConfig::GetInstance()->GetInt("VehicleComm", "someip_ack_timeout_ms", value))
        LOG(LOG_WARN, "VuCComm: local_config value for someip_ack_timeout_ms, not found using default!");
    scfg.ack_timeout_ms = value;

    value = RESPONSE_TRIES;
    if (!lcfg::ILocalConfig::GetInstance()->GetInt("VehicleComm", "someip_response_tries", value))
        LOG(LOG_WARN, "VuCComm: local_config value for someip_response_tries, not found using default!");
    scfg.response_tries = value;

    value = RESPONSE_TIMEOUT_MS;
    if (!lcfg::ILocalConfig::GetInstance()->GetInt("VehicleComm", "someip_response_timeout_ms", value))
        LOG(LOG_WARN, "VuCComm: local_config value for someip_response_timeout_ms, not found using default!");
    scfg.response_timeout_ms = value;

    value = TIME_RESOLUTION_MS;
    if (!lcfg::ILocalConfig::GetInstance()->GetInt("VehicleComm", "someip_time_resolution_ms", value))
        LOG(LOG_WARN, "VuCComm: local_config value for someip_time_resolution_ms, not found using default!");
    scfg.time_resolution_ms = value;
    
    ReturnValue ret = slist_.Init(mq_, WD_THREAD_VGMTIMER, scfg);
    if(ret != RET_OK)
        LOG(LOG_DEBUG, "LANComm: %s: Failed to initialize slist!", __FUNCTION__);

    slist_.Start();
}

LANComm::~LANComm()
{
    LOG(LOG_DEBUG, "LANComm: %s (app = %s).", __FUNCTION__, app_->get_name().c_str());

    slist_.Stop();

    // Clean-up client-stuff
    for (std::list<ServiceClient>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        LOG(LOG_DEBUG, "LANBase: %s: Releasing service-client %s.", __FUNCTION__, it->client_name.c_str());
        app_->release_service(it->service_id, it->instance_id); 
    }
}

ReturnValue LANComm::Init()
{
    LOG(LOG_DEBUG, "LANComm: %s.", __FUNCTION__);
    
    AddEvent("PosnFromSatltCon", LAN_EVENTGROUP_ID, EVENT_POSNFROMSATLTCON);

    return LANBase::Init();
}

ReturnValue LANComm::Start()
{
    LOG(LOG_DEBUG, "LANComm: %s.", __FUNCTION__);

    return LANBase::Start();
}

ReturnValue LANComm::Stop()
{
    LOG(LOG_DEBUG, "LANComm: %s.", __FUNCTION__);

    return LANBase::Stop();
}

/**
    @brief Send message to LAN.

    This method is used by the user of LANBase to send a Message to the LAN.
    @param[in]  m              Message to send to lan
    @return     A VC return-value
*/
ReturnValue LANComm::SendMessage(MessageBase *m)
{
    ReturnValue ret;
    LOG(LOG_DEBUG, "LANComm: %s.", __FUNCTION__);

    if (state_ != STATE_REGISTERED) {
        return RET_ERR_INVALID_STATE;
    }

    if (m->type_ != MESSAGE_EVENT) {
        LOG(LOG_WARN, "LANComm: %s: Wrong message-type! Expected event-type.", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }

    // Send the event
    std::shared_ptr<vsomeip::payload> payload;
    CreateSomeIPPayload(m, payload);
    ret = SendSomeIPEvent((EventID)m->id_, payload);
    return ret;
}

ReturnValue LANComm::SendMessage(vsomeip::service_t service_id, MessageBase *m)
{
    LOG(LOG_DEBUG, "LANComm: %s.", __FUNCTION__);

    return LANBase::SendMessage(service_id, m);
}

// Session-list methods

void LANComm::AckRequest(int ext_session_id)
{
    mutex_.lock();
    slist_.AckRequest(ext_session_id);
    mutex_.unlock();
}

long LANComm::RemoveExtSessionID(int ext_session_id)
{
    mutex_.lock();
    long ret = slist_.RemoveSession_ExtID(ext_session_id);
    mutex_.unlock();

    return ret;
}

// Methods for the client-part

void LANComm::RegisterClient(const std::string &name, vsomeip::service_t service_id, vsomeip::eventgroup_t eventgroup_id,
                    const std::vector<EventID> &events_,
                    std::function<void(bool is_available)> OnAvailability,
                    std::function<void(const std::shared_ptr<vsomeip::message> &msg)> OnResponse,
                    std::function<void(const std::shared_ptr<vsomeip::message> &msg)> OnRequestAck,
                    std::function<void(const std::shared_ptr<vsomeip::message> &msg)> OnEvent)
{
    LOG(LOG_DEBUG, "LANComm: %s (name = %s, service = 0x%x, eventgroup = 0x%x).", __FUNCTION__, name.c_str(),
            service_id, eventgroup_id);

    AddServiceHandler(service_id, LAN_INSTANCE_ID);

    clients.push_back(ServiceClient{name, service_id, LAN_INSTANCE_ID, eventgroup_id, &events_, OnAvailability, OnResponse, OnRequestAck, OnEvent});
}

void LANComm::UnregisterClient(vsomeip::service_t service_id)
{
    LOG(LOG_DEBUG, "LANComm: %s (service = 0x%x).", __FUNCTION__, service_id);

    app_->unregister_availability_handler(service_id, LAN_INSTANCE_ID);
    app_->unregister_message_handler(service_id, LAN_INSTANCE_ID, vsomeip::ANY_METHOD);
}

// Methods for the server-part

void LANComm::AddEvent(std::string name, vsomeip::eventgroup_t eventgroup_id, EventID event_id)
{
    events.push_back(ServiceEvent{name, service_id_, LAN_INSTANCE_ID, eventgroup_id, event_id});
}

} // namespace vc
