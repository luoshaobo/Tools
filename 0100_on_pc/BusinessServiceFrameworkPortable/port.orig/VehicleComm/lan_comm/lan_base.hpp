#ifndef LAN_BASE_HPP
#define LAN_BASE_HPP

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

namespace vc {

/**
    @brief A list of all supported event messages.

    A EventID can be one of the events listed in this enum.
*/
typedef enum {
    SOMEIP_UNKNOWN,
    SOMEIP_EVENT_POSNFROMSATLTCON,
    SOMEIP_EVENT_TESTFROMVGM,
    SOMEIP_REQ_VGMTESTMSG,
    SOMEIP_RES_VGMTESTMSG,
    SOMEIP_EVENT_TESTFROMIHU,
    SOMEIP_REQ_IHUTESTMSG,
    SOMEIP_RES_IHUTESTMSG
} SomeIPID;

const char *const SomeIPIDStr[] = {
    "SOMEIP_UNKNOWN",
    "SOMEIP_EVENT_POSNFROMSATLTCON",
    "SOMEIP_EVENT_TESTFROMVGM",
    "SOMEIP_REQ_VGMTESTMSG",
    "SOMEIP_RES_VGMTESTMSG",
    "SOMEIP_EVENT_TESTFROMIHU",
    "SOMEIP_REQ_IHUTESTMSG",
    "SOMEIP_RES_IHUTESTMSG"
};

/**
    @brief Mapping between a VCRequest and a vSomeIP method.

    This struct represents a mapping of one VC request-ID to a vSomeIP method-ID.
*/
struct VGMRequestPair {
    MessageID message_id;
    uint16_t vsomeip_id;
    MessageType message_type;
};

/**
    @brief Lookup-table for VCRequests<->vSomeIP-methods.

    A list of all known mappings between VCRequests and vSomeIP-methods.
*/
const VGMRequestPair someip_message_lookup[] = {
    {
        .message_id = (MessageID)EVENT_POSNFROMSATLTCON,
        .vsomeip_id = SOMEIP_EVENT_POSNFROMSATLTCON,
        .message_type = MESSAGE_EVENT
    },
    {
        .message_id = (MessageID)EVENT_TESTFROMVGM,
        .vsomeip_id = SOMEIP_EVENT_TESTFROMVGM,
        .message_type = MESSAGE_EVENT
    },
    {
        .message_id = (MessageID)REQ_VGMTESTMSG,
        .vsomeip_id = SOMEIP_REQ_VGMTESTMSG,
        .message_type = MESSAGE_REQUEST
    },
    {
        .message_id = (MessageID)RES_VGMTESTMSG,
        .vsomeip_id = SOMEIP_RES_VGMTESTMSG,
        .message_type = MESSAGE_RESPONSE
    },
    {
        .message_id = (MessageID)EVENT_TESTFROMIHU,
        .vsomeip_id = SOMEIP_EVENT_TESTFROMIHU,
        .message_type = MESSAGE_EVENT
    },
    {
        .message_id = (MessageID)REQ_IHUTESTMSG,
        .vsomeip_id = SOMEIP_REQ_IHUTESTMSG,
        .message_type = MESSAGE_REQUEST
    },
    {
        .message_id = (MessageID)RES_IHUTESTMSG,
        .vsomeip_id = SOMEIP_RES_IHUTESTMSG,
        .message_type = MESSAGE_RESPONSE
    },
};

/**
    @brief Mapping between a vSomeIP-method and a VCResponse

    This struct represents a mapping of one vSomeIP-method to a VCResponse.
*/
struct VGMResponsePair {
    vsomeip::method_t method_id;
    ResponseID response_id;
};

typedef enum {
    STATE_UNKNOWN,
    STATE_INITIALIZED,
    STATE_STARTED,
    STATE_REGISTERED,
    STATE_EXITING
} LANBaseState;

const char *const LANBaseStateStr[] = {
    "STATE_UNKNOWN",
    "STATE_INITALIZED",
    "STATE_STARTED",
    "STATE_REGISTERED",
    "STATE_EXITING"
};

typedef struct {
    std::string event_name;
    int service_id;
    int instance_id;
    int eventgroup_id;
    EventID event_id;
} ServiceEvent;

typedef struct {
    std::string client_name;
    vsomeip::service_t service_id;
    vsomeip::instance_t instance_id;
    vsomeip::eventgroup_t eventgroup_id;
    const std::vector<EventID> *events;
    std::function<void(bool is_available)> HandleAvailability;
    std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleResponse;
    std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleRequestAck;
    std::function<void(const std::shared_ptr<vsomeip::message> &msg)> HandleEvent;
} ServiceClient;

/**
    @brief VC-endpoint handling communication with the LAN

    This class is a VC-endpoint handling requests to the LAN (SomeIP-bus) and
    responses and events from the LAN to any other VC-endpoint
*/
class LANBase {
  private:
    // Thread for SomeIP-app main loop
    std::thread app_thread_;

    // SomeIP-app main loop
    void AppLoop();

    // Methods for the SomeIP-app
    void OnState(vsomeip::state_type_e _state);
    void OnAvailability(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available);
    void OnMessage_Server(const std::shared_ptr<vsomeip::message> &msg);
    void OnMessage(const std::shared_ptr<vsomeip::message> &msg);

  protected:
    int service_id_;
    int instance_id_;
    
    // SomeIP objects
    std::shared_ptr<vsomeip::application> app_;     /**< vSomeIP application */

    // Real-time handling
    std::mutex mutex_;
    LANBaseState state_ = STATE_UNKNOWN;            /**< Keep track of the internal state of LANBase */

    void ChangeState(LANBaseState state_new);

    // Methods for the server-part
    virtual void OnStateChange(LANBaseState new_state) = 0;
    virtual void OnAvailabilityChange(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available) = 0;
    virtual void OnRequest(const std::shared_ptr<vsomeip::message> &msg) = 0;

    // Methods for the client-part
    void AddServiceHandler(vsomeip::service_t service_id, vsomeip::instance_t instance_id);
    virtual void OnResponse(const std::shared_ptr<vsomeip::message> &msg) = 0;
    virtual void OnRequestAck(const std::shared_ptr<vsomeip::message> &msg) = 0;
    virtual void OnEvent(const std::shared_ptr<vsomeip::message> &msg) = 0;
    virtual ReturnValue OnSendRequest(vsomeip::service_t service_id, MessageBase *m) = 0;
    virtual ReturnValue OnSendEvent(MessageBase *m) = 0;

    // Methods for the life-cycle
    ReturnValue Init();
    ReturnValue Start();
    ReturnValue Stop();

public:
    LANBase(int service_id, int instance_id);
    ~LANBase();

    const char *ReturnCodeToStr(vsomeip::return_code_e returncode);
    const char *MessageTypeToStr(vsomeip::message_type_e msgtype);

    RequestID SomeIPMethodIDToRequestID(vsomeip::method_t vsomeip_id);  
    vsomeip::method_t RequestIDToSomeIPMethod(RequestID request_id);
    ResponseID SomeIPMethodIDToResponseID(vsomeip::method_t vsomeip_id);
    vsomeip::method_t ResponseIDToSomeIPMethod(ResponseID response_id);
    EventID SomeIPEventIDToEventID(vsomeip::event_t vsomeip_id);
    vsomeip::event_t EventIDToSomeIPEventID(EventID event_id);

    
    void CreateSomeIPPayload(MessageBase *m, std::shared_ptr<vsomeip::payload>& payload);
    ReturnValue CreateSomeIPRequest(vsomeip::service_t service_id, MessageBase *m, std::shared_ptr<vsomeip::message>& msg);

    ReturnValue RequestEvent(vsomeip::service_t service_id, std::set<vsomeip::eventgroup_t>&  eventgroups, EventID event_id);
    ReturnValue OfferEvent(vsomeip::service_t service_id, vsomeip::instance_t instance_id, 
                        vsomeip::eventgroup_t eventgroup_id, EventID event_id);
    ReturnValue SendSomeIPMessageRaw(std::shared_ptr<vsomeip::message> &msg);
    ReturnValue SendSomeIPEvent(vsomeip::event_t event_id, std::shared_ptr<vsomeip::payload> &data);
    ReturnValue SendSomeIPMessageAck(std::shared_ptr<vsomeip::message> msg_ack);
    ReturnValue SendMessage(vsomeip::service_t service_id, MessageBase *m);
};

} // namespace vc

#endif // LAN_BASE_HPP
