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

/** @file ihu_server.cpp
 * This file implements a test binary simulating IHU
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <string.h>
#include <signal.h>

#include "vc_utility.hpp"

#include "ihu_server.hpp"

namespace vc {

// Private

// Overrides - SomeIPServers

// Methods for the client-part

void IHUServer::HandleAvailability(bool is_available)
{
    LOG(LOG_DEBUG, "IHUServer: %s (available = %d).", __FUNCTION__, is_available);
    
    is_available_ = is_available;
}

void IHUServer::HandleResponse(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "IHUServer: %s.", __FUNCTION__);
    
    UNUSED(msg);
}

void IHUServer::HandleRequestAck(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "IHUServer: %s.", __FUNCTION__);
    
    UNUSED(msg);
}

void IHUServer::HandleEvent(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "IHUServer: %s (someip_id = 0x%x).", __FUNCTION__, msg->get_method());

    EventID id = SomeIPEventIDToEventID((vsomeip::event_t)msg->get_method());
    std::shared_ptr<vsomeip::payload> pl = msg->get_payload();
    MessageBase *m = InitMessage((MessageID)id, MESSAGE_EVENT, -1, msg->get_session(),
                        ENDPOINT_UNKNOWN, ENDPOINT_IHU, (const unsigned char*)(pl->get_data()));
    LOG(LOG_DEBUG, "IHUServer: %s: Incoming event: %s.", __FUNCTION__, EventStr[(EventID)m->id_]);
    PrintMessage(m);
    delete m;
}

// Methods for the server-part

void IHUServer::HandleRequest(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "IHUServer: %s.", __FUNCTION__);

    RequestID id = SomeIPMethodIDToRequestID(msg->get_method());
    std::shared_ptr<vsomeip::payload> pl = msg->get_payload();
    MessageBase *m = InitMessage((MessageID)id, MESSAGE_REQUEST, -1, msg->get_session(),
                        ENDPOINT_UNKNOWN, ENDPOINT_IHU, (const unsigned char*)(pl->get_data()));
    LOG(LOG_DEBUG, "IHUServer: %s: Incoming request: %s.", __FUNCTION__, VCRequestStr[(RequestID)m->id_]);
    PrintMessage(m);

    bool return_ack = true;
    bool return_res = true;
    switch (id) {
    case REQ_SENDPOSNFROMSATLTCON: {
        Message<ReqSendPosnFromSatltCon> *mm = (Message<ReqSendPosnFromSatltCon>*)m;
        return_ack = true;
        return_res = true;
        break;
    }
    // Test
    case REQ_IHUTESTMSG: {
        Message<ReqIHUTestMsg> *mm = (Message<ReqIHUTestMsg>*)m;
        return_ack = mm->message.return_ack;
        return_res = mm->message.return_res;
        break;
    }
    default:
        LOG(LOG_ERR, "IHUServer: %s: Unknown request!", __FUNCTION__);
        break;
    }

    if (msg->get_return_code() != vsomeip::return_code_e::E_OK) {
        LOG(LOG_ERR, "IHUServer: %s: Received request with error (error = %d type = %s session = %d)!",
            __FUNCTION__, msg->get_return_code(), ReturnCodeToStr(msg->get_return_code()), msg->get_session());
    }

    // Send request-ack
    if (!return_ack) {
         LOG(LOG_DEBUG, "IHUServer: %s: MT_REQUEST_ACK NOT sent (session = %d).", __FUNCTION__, msg->get_session());
        return;
    }
    std::shared_ptr<vsomeip::message> ackmsg = vsomeip::runtime::get()->create_request_ack(msg);
    ackmsg->set_message_type(vsomeip::message_type_e::MT_REQUEST_ACK);
    SendSomeIPMessageAck(ackmsg);
    LOG(LOG_DEBUG, "IHUServer: %s: MT_REQUEST_ACK sent (session = %d)", __FUNCTION__, msg->get_session());

    // Send response

    if (!return_res) {
        LOG(LOG_DEBUG, "IHUServer: %s: RESPONSE NOT sent (session = %d).", __FUNCTION__, msg->get_session());
        return;
    }

    MessageBase *m_res;
    switch (id) {
    case REQ_SENDPOSNFROMSATLTCON: {
        m_res = InitMessage((MessageID)RES_REQUESTSENT, MESSAGE_RESPONSE, -1, -1,
                        ENDPOINT_IHU, ENDPOINT_UNKNOWN, NULL);

        break;
    }
    // Test
    case REQ_IHUTESTMSG: {
        ResIHUTestMsg res;
        res.id = 88;
        strcpy(res.city, "Regensburg");
        m_res = InitMessage((MessageID)RES_IHUTESTMSG, MESSAGE_RESPONSE, -1, -1,
                        ENDPOINT_IHU, ENDPOINT_UNKNOWN, CREATE_DATA(res));

        break;
    }
    default:
        std::cout << "IHUServer: Unknown message-ID!";
        return;
    }

    SendResponse(msg, m_res);

    delete m_res;
}



// Public

IHUServer::IHUServer()
    : SomeIPServer(IHU_SERVICE_ID, LAN_INSTANCE_ID,
            [this](bool is_available) {
                    return HandleAvailability(is_available);
            },
            [this](const std::shared_ptr<vsomeip::message> &msg) {
                    return HandleResponse(msg);
            },
            [this](const std::shared_ptr<vsomeip::message> &msg) {
                    return HandleRequestAck(msg);
            },
            [this](const std::shared_ptr<vsomeip::message> &msg) {
                    return HandleEvent(msg);
            })
{
    LOG(LOG_DEBUG, "IHUServer: %s.", __FUNCTION__);
}

IHUServer::~IHUServer()
{
    LOG(LOG_DEBUG, "IHUServer: %s.", __FUNCTION__);
}

ReturnValue IHUServer::Init()
{
    LOG(LOG_DEBUG, "IHUServer: %s.", __FUNCTION__);

    AddEvent("TestEvent", IHU_EVENTGROUP_ID, EVENT_TESTFROMIHU);

    return LANBase::Init();
}

ReturnValue IHUServer::Start()
{
    LOG(LOG_DEBUG, "IHUServer: %s.", __FUNCTION__);

    ReturnValue ret = LANBase::Start();
    if (ret != RET_OK) {
            LOG(LOG_DEBUG, "IHUServer: Failed to initialize LANBase (error = %d)!", __FUNCTION__, ret);
            return ret;
    }

    while (state_ != STATE_EXITING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        EventTestFromIHU from_ihu;
        from_ihu.ihu = 66;
        MessageBase *m = InitMessage((MessageID)EVENT_TESTFROMIHU, MESSAGE_EVENT, -1, -1,
                            ENDPOINT_IHU, ENDPOINT_UNKNOWN, (const unsigned char*)(&from_ihu));

        ReturnValue ret = SendMessage(LAN_SERVICE_ID, m);
        if (ret != RET_OK)
            LOG(LOG_WARN, "IHUServer: %s: Failed to send event (error = %s) !", __FUNCTION__, ReturnValueStr[ret]);
    }
}

ReturnValue IHUServer::Stop()
{
    LOG(LOG_DEBUG, "IHUServer: %s.", __FUNCTION__);

    return LANBase::Stop();
}

} // namespace vc

// Main

vc::IHUServer *ihu_server(nullptr);

void SignalHandler(int signum) {
    vc::LOG(vc::LOG_DEBUG, "IHUServer: %s: SIGINT (%d) -> good bye!", __FUNCTION__, signum);
    if(ihu_server != nullptr)
        ihu_server->~IHUServer();
    signal(signum, SIG_DFL);
    raise(signum);
}

int main(int argc, char **argv) {

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGSEGV, SignalHandler);
    //signal(SIGABRT, SignalHandler);

    vc::IHUServer ihus;
    ihu_server = &ihus;

    if (ihu_server->Init() != vc::RET_OK) {
        vc::LOG(vc::LOG_DEBUG, "IHUServer: %s: Failed to initialize the IHU-server!", __FUNCTION__);
        return 1;
    }

    if (ihu_server->Start() != vc::RET_OK) {
        vc::LOG(vc::LOG_DEBUG, "IHUServer: %s: Failed to start the IHU-server!", __FUNCTION__);
        return 1;
    }
    return 0;
}

