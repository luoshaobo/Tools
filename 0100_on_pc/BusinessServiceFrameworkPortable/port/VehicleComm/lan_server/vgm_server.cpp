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

#include <string.h>
#include <signal.h>

#include "vc_utility.hpp"

#include "vgm_server.hpp"

namespace vc {

// Private

// Overrides - SomeIPServers

// Methods for the client-part

void VGMServer::HandleAvailability(bool is_available)
{
    LOG(LOG_DEBUG, "VGMServer: %s (available = %d).", __FUNCTION__, is_available);
    
    is_available_ = is_available;
}

void VGMServer::HandleResponse(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "VGMServer: %s.", __FUNCTION__);
    
    UNUSED(msg);
}

void VGMServer::HandleRequestAck(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "VGMServer: %s.", __FUNCTION__);
    
    UNUSED(msg);
}

void VGMServer::HandleEvent(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "VGMServer: %s (someip_id = 0x%x).", __FUNCTION__, msg->get_method());

    EventID id = SomeIPEventIDToEventID((vsomeip::event_t)msg->get_method());
    std::shared_ptr<vsomeip::payload> pl = msg->get_payload();
    MessageBase *m = InitMessage((MessageID)id, MESSAGE_EVENT, -1, msg->get_session(),
                        ENDPOINT_UNKNOWN, ENDPOINT_VGM, (const unsigned char*)(pl->get_data()));
    LOG(LOG_DEBUG, "VGMServer: %s: Incoming event: %s.", __FUNCTION__, EventStr[(EventID)m->id_]);
    PrintMessage(m);
    delete m;
}

// Methods for the server-part

void VGMServer::HandleRequest(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "VGMServer: %s.", __FUNCTION__);

    RequestID id = SomeIPMethodIDToRequestID(msg->get_method());
    std::shared_ptr<vsomeip::payload> pl = msg->get_payload();
    MessageBase *m = InitMessage((MessageID)id, MESSAGE_REQUEST, -1, msg->get_session(),
                        ENDPOINT_UNKNOWN, ENDPOINT_VGM, (const unsigned char*)(pl->get_data()));
    LOG(LOG_DEBUG, "VGMServer: %s: Incoming request: %s.", __FUNCTION__, VCRequestStr[(RequestID)m->id_]);
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
    case REQ_VGMTESTMSG: {
        Message<ReqVGMTestMsg> *mm = (Message<ReqVGMTestMsg>*)m;
        return_ack = mm->message.return_ack;
        return_res = mm->message.return_res;
        break;
    }
    default:
        LOG(LOG_ERR, "VGMServer: %s: Unknown request!", __FUNCTION__);
        break;
    }

    if (msg->get_return_code() != vsomeip::return_code_e::E_OK) {
        LOG(LOG_ERR, "VGMServer: %s: Received request with error (error = %d type = %s session = %d)!",
            __FUNCTION__, msg->get_return_code(), ReturnCodeToStr(msg->get_return_code()), msg->get_session());
    }

    // Send request-ack
    if (!return_ack) {
         LOG(LOG_DEBUG, "VGMServer: %s: MT_REQUEST_ACK NOT sent (session = %d).", __FUNCTION__, msg->get_session());
        return;
    }
    std::shared_ptr<vsomeip::message> ackmsg = vsomeip::runtime::get()->create_request_ack(msg);
    ackmsg->set_message_type(vsomeip::message_type_e::MT_REQUEST_ACK);
    SendSomeIPMessageAck(ackmsg);
    LOG(LOG_DEBUG, "VGMServer: %s: MT_REQUEST_ACK sent (session = %d)", __FUNCTION__, msg->get_session());

    // Send response

    if (!return_res) {
        LOG(LOG_DEBUG, "VGMServer: %s: RESPONSE NOT sent (session = %d).", __FUNCTION__, msg->get_session());
        return;
    }

    MessageBase *m_res;
    switch (id) {
    case REQ_SENDPOSNFROMSATLTCON: {
        m_res = InitMessage((MessageID)RES_REQUESTSENT, MESSAGE_RESPONSE, -1, -1,
                        ENDPOINT_VGM, ENDPOINT_UNKNOWN, NULL);

        break;
    }
    // Test
    case REQ_VGMTESTMSG: {
        ResVGMTestMsg res;
        res.id = 66;
        strcpy(res.company, "Volvo");
        m_res = InitMessage((MessageID)RES_VGMTESTMSG, MESSAGE_RESPONSE, -1, -1,
                        ENDPOINT_VGM, ENDPOINT_UNKNOWN, CREATE_DATA(res));

        break;
    }
    default:
        std::cout << "VGMServer: Unknown message-ID!";
        return;
    }

    SendResponse(msg, m_res);

    delete m_res;
}



// Public

VGMServer::VGMServer()
    : SomeIPServer(VGM_SERVICE_ID, LAN_INSTANCE_ID,
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
    LOG(LOG_DEBUG, "VGMServer: %s.", __FUNCTION__);
}

VGMServer::~VGMServer()
{
    LOG(LOG_DEBUG, "VGMServer: %s.", __FUNCTION__);
}

ReturnValue VGMServer::Init()
{
    LOG(LOG_DEBUG, "VGMServer: %s.", __FUNCTION__);

    AddEvent("TestEvent", VGM_EVENTGROUP_ID, EVENT_TESTFROMVGM);

    return LANBase::Init();
}

ReturnValue VGMServer::Start()
{
    LOG(LOG_DEBUG, "VGMServer: %s.", __FUNCTION__);

    ReturnValue ret = LANBase::Start();
    if (ret != RET_OK) {
            LOG(LOG_DEBUG, "VGMServer: Failed to initialize LANBase (error = %d)!", __FUNCTION__, ret);
            return ret;
    }

    while (state_ != STATE_EXITING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        EventTestFromVGM from_vgm;
        from_vgm.vgm = 66;
        MessageBase *m = InitMessage((MessageID)EVENT_TESTFROMVGM, MESSAGE_EVENT, -1, -1,
                            ENDPOINT_VGM, ENDPOINT_UNKNOWN, (const unsigned char*)(&from_vgm));

        ReturnValue ret = SendMessage(LAN_SERVICE_ID, m);
        if (ret != RET_OK)
            LOG(LOG_WARN, "VGMServer: %s: Failed to send event (error = %s) !", __FUNCTION__, ReturnValueStr[ret]);
    }
}

ReturnValue VGMServer::Stop()
{
    LOG(LOG_DEBUG, "VGMServer: %s.", __FUNCTION__);

    return LANBase::Stop();
}

} // namespace vc

// Main

vc::VGMServer *vgm_server(nullptr);

void SignalHandler(int signum) {
    vc::LOG(vc::LOG_DEBUG, "VGMServer: %s: SIGINT (%d) -> good bye!", __FUNCTION__, signum);
    if(vgm_server != nullptr)
        vgm_server->~VGMServer();
    signal(signum, SIG_DFL);
    raise(signum);
}

int main(int argc, char **argv) {

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGSEGV, SignalHandler);
    //signal(SIGABRT, SignalHandler);

    vc::VGMServer vgms;
    vgm_server = &vgms;

    if (vgm_server->Init() != vc::RET_OK) {
        vc::LOG(vc::LOG_DEBUG, "IHUServer: %s: Failed to initialize the VGM-server!", __FUNCTION__);
        return 1;
    }

    if (vgm_server->Start() != vc::RET_OK) {
        vc::LOG(vc::LOG_DEBUG, "IHUServer: %s: Failed to start the VGM-server!", __FUNCTION__);
        return 1;
    }
    return 0;
}

