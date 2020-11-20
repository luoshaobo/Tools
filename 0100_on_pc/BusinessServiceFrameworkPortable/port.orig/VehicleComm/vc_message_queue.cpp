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

/** @file vc_message_queue.cpp
 * This file implements a VehicleComm message queue
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include "vc_utility.hpp"

#include "vc_message_queue.hpp"

namespace vc {

// Public

MessageQueue::MessageQueue()
{
    LOG(LOG_INFO, "MessageQueue: %s: New MessageQueue at %d.", __FUNCTION__, this);
    //std::lock_guard<std::mutex> poolLock(pool_mutex_);
    new_messages_ = false;
}

MessageQueue::MessageQueue(std::condition_variable *cv)
{
    LOG(LOG_INFO, "MessageQueue: %s: New MessageQueue at %d.", __FUNCTION__, this);
    new_messages_ = false;
    cv_ = cv;
}

/**
    @brief Return the size/length of the queue.

    This method is used to get the current size/length of the queue.
 
    @return     The number of elements in the queue
*/
size_t MessageQueue::Size()
{
    return messages_.size();
}

/**
    @brief Add a Message to the queue.

    This method is called by the user to add an entry to the queue.
 
    @param[in]  m          The Message to be added
    @return     A VC return value
*/
ReturnValue MessageQueue::AddMessage(MessageBase *m)
{
    if (m == NULL) {
        LOG(LOG_DEBUG, "MessageQueue %s: NULL-message!", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }
    LOG(LOG_DEBUG, "MessageQueue: %s (id = %s, type = %s, unique_id = %d, client_id = %d, dst = %s)",
            __FUNCTION__, MessageStr(m->id_, m->type_), MessageTypeStr[m->type_], m->unique_id_, m->client_id_, EndpointStr[m->dst_]);
    std::lock_guard<std::mutex> mq_lock(mq_mutex_);
    messages_.push(m);
    SetNewMessages(true);
    if(cv_ != NULL)
        cv_->notify_all();
    return RET_OK;
}

/**
    @brief Pop a message from the queue.

    This method is called by the user to return a pointer to the first message on queue.
 
    @return     Pointer to the message first in queue
*/
MessageBase *MessageQueue::GetMessage()
{
    //LOG(LOG_DEBUG, "MessageQueue: %s.", __FUNCTION__);
    std::lock_guard<std::mutex> mq_lock(mq_mutex_);
    if (messages_.size() == 0) {
        LOG(LOG_INFO, "MessageQueue: %s: No messages in queue!", __FUNCTION__);
        return NULL;
    }

    return messages_.front();
}

/**
    @brief Remove a message from the queue.

    This method is called by the user to pop the first message from the queue.
 
    @return     A VC return value
*/
void MessageQueue::RemoveMessage()
{
    //LOG(LOG_DEBUG, "MessageQueue: %s.", __FUNCTION__);
    std::lock_guard<std::mutex> mq_lock(mq_mutex_);
    messages_.pop();
    if (messages_.size() == 0)
        new_messages_ = false;
}

/**
    @brief Indicate that the queue has new messages.

    This method is called by the user to set the new_messages_ member, indicating
    that there are new messages since the last time checked.
 
    @param[in]  m          The new value for the new-messages-indicator
*/
void MessageQueue::SetNewMessages(bool newStatus)
{
    this->new_messages_ = newStatus;
}

bool MessageQueue::NewMessages()
{
    return new_messages_;
}

}
