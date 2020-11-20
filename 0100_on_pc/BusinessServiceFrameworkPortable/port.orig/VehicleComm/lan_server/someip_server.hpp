#ifndef SOMEIP_SERVER_HPP
#define SOMEIP_SERVER_HPP

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

/** @file lan_server.hpp
 * This file handles the simulation of the IHU.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/


#include <thread>
#include <mutex>

#include <vsomeip/vsomeip.hpp>

#include "vc_common.hpp"
#include "lan_base.hpp"

namespace vc {

#define LAN_INSTANCE_ID 0x0777

// Remote side (TCAM)
#define LAN_SERVICE_ID  0x0111
#define LAN_EVENTGROUP_ID   0x3356

#define CREATE_DATA(x)  reinterpret_cast<const unsigned char*>(&x)

/**
    @brief Simulator of a LAN-node.

    This class is the simulation of a node (e.g. IHU, VGM) on a LAN,
*/
class SomeIPServer : public LANBase {
  private:
    std::list<ServiceEvent> events; // Events to offer as a service
    
    // Remote side (TCAM)
    const vsomeip::service_t server_id_ = LAN_SERVICE_ID;
    const vsomeip::eventgroup_t eventgroup_id_ = LAN_EVENTGROUP_ID;
    const std::vector<EventID> events_ = {EVENT_POSNFROMSATLTCON};
    ServiceClient client;

    // Methods for the server-part

    // Overrides - LANBase
    void OnStateChange(LANBaseState new_state) override;
    void OnAvailabilityChange(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available) override;
    void OnRequest(const std::shared_ptr<vsomeip::message> &msg) override;
    
    virtual void HandleRequest(const std::shared_ptr<vsomeip::message> &msg) = 0;
    
    
    // Methods for the client-part

    // Overrides - LANBase
    void OnResponse(const std::shared_ptr<vsomeip::message> &msg) override;
    void OnRequestAck(const std::shared_ptr<vsomeip::message> &msg) override;
    void OnEvent(const std::shared_ptr<vsomeip::message> &msg) override;
    ReturnValue OnSendEvent(MessageBase *m) override;
    ReturnValue OnSendRequest(vsomeip::service_t service_id, MessageBase *m) override;
    
  protected:
    bool is_available_;

    void SendResponse(const std::shared_ptr<vsomeip::message> &msg, MessageBase *m);
    void AddEvent(std::string name, vsomeip::eventgroup_t eventgroup_id, EventID event_id);
    
    virtual void HandleAvailability(bool _is_available) = 0;
    virtual void HandleResponse(const std::shared_ptr<vsomeip::message> &msg) = 0;
    virtual void HandleRequestAck(const std::shared_ptr<vsomeip::message> &msg) = 0;
    virtual void HandleEvent(const std::shared_ptr<vsomeip::message> &msg) = 0;

  public:
    SomeIPServer(int service_id, int instance_id,
                std::function<void(bool is_available)> HandleAvailability,
                std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleResponse,
                std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleRequestAck,
                std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleEvent);
    ~SomeIPServer();

    virtual ReturnValue Init();
    virtual ReturnValue Start();
    virtual ReturnValue Stop();
};

} // namespace vc

#endif // IHU_SERVER_HPP
