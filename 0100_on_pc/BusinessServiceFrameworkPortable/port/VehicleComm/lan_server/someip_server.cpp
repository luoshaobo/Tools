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

/** @file vgm_server.cpp
 * This file implements a test binary simulating VGM
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "vc_utility.hpp"
#include "someip_server.hpp"

namespace vc {

// Private

long MessageBase::unique_id_current = 0;

// Methods for the server-part

// Overrides - LANBase

void SomeIPServer::OnStateChange(LANBaseState new_state)
{
    LOG(LOG_DEBUG, "SomeIPServer: %s (new_state = %d)", __FUNCTION__, new_state);

    if (new_state == STATE_REGISTERED) {
        LOG(LOG_DEBUG, "LANComm: %s: Requesting client-service %s (service = 0x%x).", __FUNCTION__,
                client.client_name.c_str(), client.service_id);
        app_->request_service(client.service_id, client.instance_id); 

        std::set<vsomeip::eventgroup_t> its_groups;
        its_groups.insert(client.eventgroup_id);
        for (std::vector<EventID>::const_iterator it_ev = client.events->begin(); it_ev != client.events->end(); ++it_ev)
            RequestEvent(client.service_id, its_groups, *it_ev);
        app_->subscribe(client.service_id, client.instance_id, client.eventgroup_id);
            
        for (std::list<ServiceEvent>::const_iterator it = events.begin(); it != events.end(); ++it)
            OfferEvent(it->service_id, it->instance_id, it->eventgroup_id, it->event_id);

    }
}

void SomeIPServer::OnAvailabilityChange(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available)
{
    LOG(LOG_DEBUG, "SomeIPServer: %s (available = %d.", __FUNCTION__, is_available);

    UNUSED(service);
    UNUSED(instance);
}

void SomeIPServer::OnRequest(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "SomeIPServer: %s.", __FUNCTION__);

    HandleRequest(msg);
}


void SomeIPServer::HandleRequest(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "SomeIPServer: %s (virtual).", __FUNCTION__);
}


// Methods for the client-part

// Overrides - LANBase

void SomeIPServer::OnResponse(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "SomeIPServer: %s.", __FUNCTION__);

    HandleResponse(msg);
}

void SomeIPServer::OnRequestAck(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "SomeIPServer: %s.", __FUNCTION__);

    HandleRequestAck(msg);
}

void SomeIPServer::OnEvent(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "SomeIPServer: %s.", __FUNCTION__);

    HandleEvent(msg);
}

ReturnValue SomeIPServer::OnSendEvent(MessageBase *m)
{
    ReturnValue ret;
    LOG(LOG_DEBUG, "SomeIPServer: %s.", __FUNCTION__);
    if (state_ != STATE_REGISTERED) {
        delete(m);
        return RET_ERR_INVALID_STATE;
    }

    vsomeip::event_t someip_id = EventIDToSomeIPEventID((EventID)m->id_);
    if (!someip_id) {
        delete(m);
        return RET_ERR_INVALID_ARG;
    }

    std::shared_ptr<vsomeip::payload> payload;
    CreateSomeIPPayload(m, payload);
    ret = SendSomeIPEvent(someip_id, payload);
    delete(m);
    return ret;
}

ReturnValue SomeIPServer::OnSendRequest(vsomeip::service_t service_id, MessageBase *m)
{
    LOG(LOG_DEBUG, "SomeIPServer: %s.", __FUNCTION__);

    ReturnValue ret;

    //mutex_.lock();
    
    vsomeip::method_t vsomeip_id = RequestIDToSomeIPMethod((RequestID)m->id_);
    if (!vsomeip_id)
        return RET_ERR_INVALID_ARG;

    // Create SomeIP-message
    std::shared_ptr<vsomeip::message> msg;
    ret = CreateSomeIPRequest(service_id, m, msg);
    if (ret != RET_OK)
        return ret;

    // Send SomeIP-message
    SendSomeIPMessageRaw(msg);

    //mutex_.unlock();

    return ret;
}



// Protected

void SomeIPServer::AddEvent(std::string name, vsomeip::eventgroup_t eventgroup_id, EventID event_id)
{
    events.push_back(ServiceEvent{name, service_id_, LAN_INSTANCE_ID, eventgroup_id, event_id});
}

void SomeIPServer::SendResponse(const std::shared_ptr<vsomeip::message> &msg, MessageBase *m)
{
    LOG(LOG_DEBUG, "SomeIPServer: %s: Sending response-message %s (unique_id = %d, src = %s, dst = %s).",
            __FUNCTION__, MessageStr(m), m->unique_id_, EndpointStr[m->src_], EndpointStr[m->dst_]);

    std::shared_ptr<vsomeip::payload> its_payload = vsomeip::runtime::get()->create_payload();
    its_payload->set_data(m->GetData(), m->GetSize());
    std::shared_ptr<vsomeip::message> its_response = vsomeip::runtime::get()->create_response(msg);
    its_response->set_payload(its_payload);
    vsomeip::method_t vsomeip_id = ResponseIDToSomeIPMethod((ResponseID)m->id_);
    if (vsomeip_id == 0)
        return;
    LOG(LOG_DEBUG, "SomeIPServer: %s: Send vSomeIP-message %s[0x%x]!", __FUNCTION__, SomeIPIDStr[vsomeip_id],
            vsomeip_id);
    its_response->set_method(vsomeip_id);

    app_->send(its_response, true);
}



// Public

SomeIPServer::SomeIPServer(int service_id, int instance_id,
                        std::function<void(bool is_available)> HandleAvailability,
                        std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleResponse,
                        std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleRequestAck,
                        std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleEvent) :
        LANBase(service_id, instance_id),
        client{"TCAM", server_id_, LAN_INSTANCE_ID, eventgroup_id_, &events_, HandleAvailability, HandleResponse, HandleRequestAck, HandleEvent}
{
    AddServiceHandler(LAN_SERVICE_ID, instance_id);
}

SomeIPServer::~SomeIPServer()
{
    LOG(LOG_DEBUG, "SomeIPServer: %s.", __FUNCTION__);

    Stop();

    mutex_.lock();

    ChangeState(STATE_EXITING);

    app_->unregister_state_handler();

    // Clean-up server-stuff
    app_->stop_offer_service(service_id_, instance_id_);
    app_->unregister_message_handler(service_id_, instance_id_, vsomeip::ANY_METHOD);
    app_->release_service(service_id_, instance_id_);

    // Clean-up client-stuff
    app_->release_service(0x0111, instance_id_);

    mutex_.unlock();
}

ReturnValue SomeIPServer::Init()
{
    LOG(LOG_DEBUG, "SomeIPServer: %s (virtual).", __FUNCTION__);

    return LANBase::Init();
}

ReturnValue SomeIPServer::Start()
{
    LOG(LOG_DEBUG, "SomeIPServer: %s (virtual).", __FUNCTION__);

    return LANBase::Start();
}

ReturnValue SomeIPServer::Stop()
{
    LOG(LOG_DEBUG, "SomeIPServer: %s (virtual).", __FUNCTION__);

    return LANBase::Stop();
}

} // namespace vc
