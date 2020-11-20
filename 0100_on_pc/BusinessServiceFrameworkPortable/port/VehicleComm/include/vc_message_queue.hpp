#ifndef VC_MESSAGE_QUEUE_HPP
#define VC_MESSAGE_QUEUE_HPP

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

/** @file vc_message_queue.hpp
 * This file implements a VehicleComm message queue
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <queue>
#include <mutex>
#include <condition_variable>

#include "vc_common.hpp"

namespace vc {

/**
    @brief A queue of Messages

    This class is a queue of Messages and associated operations.
    It is instantiated in the VehicleComm-class and stores all messages
    that is on their way to be routed to a VC-endpoints.
*/
class MessageQueue
{
    private:
    bool new_messages_;
    std::mutex mq_mutex_;
    std::condition_variable *cv_;

    public:
    std::queue<MessageBase*> messages_;

    MessageQueue();
    MessageQueue(std::condition_variable *vc);

    size_t Size();
    ReturnValue AddMessage(MessageBase *m);
    void RemoveMessage();
    MessageBase *GetMessage();
    bool NewMessages();
    void SetNewMessages(bool newStatus);
};

} // namespace vc

#endif // VC_MESSAGE_QUEUE_HPP
