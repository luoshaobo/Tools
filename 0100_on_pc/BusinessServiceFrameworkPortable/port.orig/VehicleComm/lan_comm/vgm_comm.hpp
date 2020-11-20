#ifndef VGM_COMM_HPP
#define VGM_COMM_HPP

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
 * This file handles communication with the VGM over vSomeIP
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/


#include <vsomeip/vsomeip.hpp>
#include "vsomeip/primitive_types.hpp"

#include "vehicle_comm_messages.hpp"
#include "vc_message_queue.hpp"
#include "vc_session_list.hpp"
#include "vc_common.hpp"

#include "lan_comm.hpp"

namespace vc {

/**
    @brief VC-endpoint handling communication with the VGM

    This class is a VC-endpoint handling requests to the VGM (SomeIP-bus) and
    responses and events from the VGM to any other VC-endpoint
*/
class VGMComm {
private:
    // Remote side (VGM)
    const vsomeip::service_t server_id_ = VGM_SERVICE_ID;
    const vsomeip::eventgroup_t eventgroup_id_ = VGM_EVENTGROUP_ID;
    const std::vector<EventID> events_ = {EVENT_TESTFROMVGM};

    MessageQueue *mq_;                              /**< The main message-queue, belonging to the VehicleComm-instance */
    LANComm *lan_;

    vsomeip::state_type_e state_;
    bool is_available_;

    // Client-callbacks
    void HandleAvailability(bool _is_available);
    void HandleResponse(const std::shared_ptr<vsomeip::message> &msg);
    void HandleRequestAck(const std::shared_ptr<vsomeip::message> &msg);
    void HandleEvent(const std::shared_ptr<vsomeip::message> &message);

public:
    VGMComm(MessageQueue *mq, LANComm *lan);
    ~VGMComm();

    // Methods for the life-cycle
    ReturnValue Init();
    ReturnValue Start();
    ReturnValue Stop();

    ReturnValue SendMessage(MessageBase *m);
};

} // namespace vc

#endif // VGM_COMM_HPP
