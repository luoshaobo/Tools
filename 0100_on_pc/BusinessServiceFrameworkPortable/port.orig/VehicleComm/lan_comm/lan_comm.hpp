#ifndef LAN_COMM_HPP
#define LAN_COMM_HPP

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

/** @file lan_comm.hpp
 * This file handles communication with the LAN over vSomeIP
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
#include <stdio.h>
#include <string.h>

#include <vsomeip/vsomeip.hpp>
#include "vsomeip/primitive_types.hpp"

#include "vehicle_comm_messages.hpp"
#include "vc_message_queue.hpp"
#include "vc_session_list.hpp"
#include "vc_common.hpp"
#include "vc_watchdog.hpp"

#include "lan_base.hpp"

namespace vc {

#define ACK_TIMEOUT_MS      3000
#define ACK_TRIES           2
#define RESPONSE_TIMEOUT_MS 2000
#define RESPONSE_TRIES      2
#define TIME_RESOLUTION_MS  100

#define LAN_INSTANCE_ID     0x0777

// Local side (TCAM)
#define LAN_SERVICE_ID      0x0111
#define LAN_EVENTGROUP_ID   0x3356

// Remote side (VGM)
#define VGM_SERVICE_ID      0x0666
#define VGM_EVENTGROUP_ID   0x4456

// Remote side (IHU)
#define IHU_SERVICE_ID      0x0888
#define IHU_EVENTGROUP_ID   0x4455

/**
    @brief VC-endpoint handling communication with the LAN

    This class is a VC-endpoint handling requests to the LAN (SomeIP-bus) and
    responses and events from the LAN to any other VC-endpoint
*/
class LANComm : public LANBase {
  private:
    std::list<ServiceEvent> events;
    std::list<ServiceClient> clients;
    MessageQueue *mq_;                              /**< The main message-queue, belonging to the VehicleComm-instance */
    SessionList slist_;                             /**< Session list to store outgoing messages before confirmation */

    ReturnValue ResendSomeIPMessage(vsomeip::service_t service_id, MessageBase *m);

    // Overrides - LANBase
    void OnStateChange(LANBaseState new_state);
    void OnAvailabilityChange(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available);
    void OnRequest(const std::shared_ptr<vsomeip::message> &msg);
    void OnResponse(const std::shared_ptr<vsomeip::message> &msg);
    void OnRequestAck(const std::shared_ptr<vsomeip::message> &msg);
    void OnEvent(const std::shared_ptr<vsomeip::message> &msg);

  protected:
    // Overrides - LANBase
    ReturnValue OnSendEvent(MessageBase *m);
    ReturnValue OnSendRequest(vsomeip::service_t service_id, MessageBase *m);

  public:
    LANComm(MessageQueue *mq);
    ~LANComm();

    // Methods for the life-cycle
    ReturnValue Init();
    ReturnValue Start();
    ReturnValue Stop();

    ReturnValue SendMessage(MessageBase *m);
    ReturnValue SendMessage(vsomeip::service_t service_id, MessageBase *m);

    // Methods for the session-list
    void AckRequest(int ext_session_id);
    long RemoveExtSessionID(int ext_session_id);

    // Methods for the client-part
    void SubscribeEventGroup(vsomeip::service_t service_id, vsomeip::eventgroup_t eventgroup_id);
    void RegisterClient(const std::string &name, vsomeip::service_t service_id, vsomeip::eventgroup_t eventgroup_id,
                    const std::vector<EventID> &events_,
                    std::function<void(bool is_available)> HandleAvailability,
                    std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleResponse,
                    std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleRequestAck,
                    std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleEvent);
    void UnregisterClient(vsomeip::service_t service_id);
    
    // Methods for the server-part
    void AddEvent(std::string name, vsomeip::eventgroup_t eventgroup_id, EventID event_id);
};

} // namespace vc
#endif // LAN_COMM_HPP
