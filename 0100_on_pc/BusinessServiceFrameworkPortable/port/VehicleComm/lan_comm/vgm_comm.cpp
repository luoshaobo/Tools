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
 * This file handles communication with the VGM over vSomeIP
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include "vc_utility.hpp"

#include "vgm_comm.hpp"

namespace vc {

// Client-callbacks

void VGMComm::HandleAvailability(bool is_available)
{
    LOG(LOG_DEBUG, "VGMComm: %s (available = %d).", __FUNCTION__, is_available);
    
    is_available_ = is_available;
}

void VGMComm::HandleResponse(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "VGMComm: %s.", __FUNCTION__);

    // Send response-ack to server
    std::shared_ptr<vsomeip::message> msg_ack = vsomeip::runtime::get()->create_response_ack(msg);
    ReturnValue ret = lan_->SendSomeIPMessageAck(msg_ack);
    if (ret != RET_OK)
        LOG(LOG_WARN, "VGMComm: %s: Failed to send response-ack (error = %s)!", __FUNCTION__, ReturnValueStr[ret]);

    // If response contains error code, create an error-response
    if (msg->get_return_code() != vsomeip::return_code_e::E_OK) {
        long session_id = lan_->RemoveExtSessionID(msg->get_session());
        LOG(LOG_DEBUG, "VGMComm: %s: Error received (error = %s[%d])!",
                __FUNCTION__, lan_->ReturnCodeToStr(msg->get_return_code()), msg->get_return_code());

        MessageBase *m = InitMessage((MessageID)RET_ERR_EXTERNAL, MESSAGE_ERROR, 0, session_id,
                                        ENDPOINT_VGM, ENDPOINT_TS, 0);
        if (m == NULL) {
            LOG(LOG_WARN, "VGMComm: %s: Failed to create error-response!",
                    __FUNCTION__);
            return;
        }
        if (mq_->AddMessage(m) != RET_OK) {
            LOG(LOG_WARN, "VGMComm: %s: Failed to add error-response to message-queue!", __FUNCTION__);
        }
        return;
    }

    ResponseID response_id = lan_->SomeIPMethodIDToResponseID(msg->get_method());
    if (response_id == RES_UNKNOWN)
        return;
        
    LOG(LOG_DEBUG, "VGMComm: Incoming message (0x%x.0x%x -> 0x%x/%d)   message_type: %s   return_code: %s \
            type: %s.", msg->get_service(), msg->get_instance(), msg->get_client(),
            msg->get_session(), lan_->MessageTypeToStr(msg->get_message_type()),
            lan_->ReturnCodeToStr(msg->get_return_code()), ResponseStr[response_id]);

    long session_id = lan_->RemoveExtSessionID(msg->get_session());

    std::shared_ptr<vsomeip::payload> pl = msg->get_payload();
    MessageBase *m = InitMessage((MessageID)response_id, MESSAGE_RESPONSE, 0, session_id, ENDPOINT_VGM,
                                ENDPOINT_TS, reinterpret_cast<const unsigned char*>(pl->get_data()));
    if (m == NULL)
        LOG(LOG_WARN, "VGMComm: %s InitMessage returned NULL not adding to queue.", __FUNCTION__);
    else
        (void)mq_->AddMessage(m);
}

void VGMComm::HandleRequestAck(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "VGMComm: %s.", __FUNCTION__);

    // If ack set timers to wait for response instead of ack
    lan_->AckRequest(msg->get_session());
}

void VGMComm::HandleEvent(const std::shared_ptr<vsomeip::message> &msg)
{
    LOG(LOG_DEBUG, "VGMComm: %s (event = %s).", __FUNCTION__, SomeIPIDStr[msg->get_method()]);

    EventID event_id = lan_->SomeIPEventIDToEventID(msg->get_method());
    if (event_id == EVENT_UNKNOWN)
        return;
        
    long session_id = lan_->RemoveExtSessionID(msg->get_session());
    std::shared_ptr<vsomeip::payload> pl = msg->get_payload();
    MessageBase *m = InitMessage((MessageID)event_id, MESSAGE_EVENT, 0, session_id,
                            ENDPOINT_VGM, ENDPOINT_TS, reinterpret_cast<const unsigned char*>(pl->get_data()));
    if (m == NULL)
        LOG(LOG_WARN, "VGMComm: %s InitMessage returned NULL not adding to queue.", __FUNCTION__);
    else
        (void)mq_->AddMessage(m);
}




// Public

VGMComm::VGMComm(MessageQueue *mq, LANComm *lan)
{
    LOG(LOG_DEBUG, "VGMComm: %s.", __FUNCTION__);

    mq_ = mq;
    lan_ = lan;
    state_ = vsomeip::state_type_e::ST_DEREGISTERED;
    is_available_ = false;
}

VGMComm::~VGMComm()
{
    LOG(LOG_DEBUG, "VGMComm: %s.", __FUNCTION__);

    lan_->UnregisterClient(server_id_);
}

// Methods for the life-cycle

/**
    @brief Initialize LANBase.

    Do all initialization stuff needed for LANBase.
*/
ReturnValue VGMComm::Init()
{
    LOG(LOG_DEBUG, "VGMComm: %s.", __FUNCTION__);

    lan_->RegisterClient("VGMComm", server_id_, eventgroup_id_, events_,
            [this](bool is_available) {
                    return VGMComm::HandleAvailability(is_available);
            },
            [this](const std::shared_ptr<vsomeip::message> &msg) {
                    return VGMComm::HandleResponse(msg);
            },
            [this](const std::shared_ptr<vsomeip::message> &msg) {
                    return VGMComm::HandleRequestAck(msg);
            },
            [this](const std::shared_ptr<vsomeip::message> &msg) {
                    return VGMComm::HandleEvent(msg);
            });
            
    return RET_OK;
}

/**
    @brief Start execution of VGMComm.

    Start the execution of VGMComm.
    @return     A VC return-value
*/
ReturnValue VGMComm::Start()
{
    LOG(LOG_DEBUG, "VGMComm: %s.", __FUNCTION__);
    
    return RET_OK;
}

/**
    @brief Stop execution of VGMComm.

    Stop the execution of VGMComm.

    @return     A VC return-value
*/
ReturnValue VGMComm::Stop()
{
    LOG(LOG_DEBUG, "VGMComm: %s.", __FUNCTION__);

   return RET_OK;
}

/**
    @brief Send message to LAN.

    This method is used by the user of LANComm to send a Message to the LAN.
    @param[in]  m              Message to send to lan
    @param[in]  error          If this message already been sent once (used by sessionlist)
    @return     A VC return-value
*/
ReturnValue VGMComm::SendMessage(MessageBase *m)
{
    LOG(LOG_DEBUG, "VGMComm: %s.", __FUNCTION__);

    if (m->id_ == (MessageID)REQ_SENDPOSNFROMSATLTCON) {
        LOG(LOG_DEBUG, "VGMComm: %s: Request converted to event.", __FUNCTION__);
        MessageBase *m_ev = CopyMessage(m);
        if (m_ev == NULL)
            return RET_ERR_INTERNAL;
        m_ev->type_ = MESSAGE_EVENT;
        m_ev->id_ = (MessageID)EVENT_POSNFROMSATLTCON;
        return lan_->SendMessage(server_id_, m_ev);
    }
    
    return lan_->SendMessage(server_id_, m);
}

} // namespace vc
