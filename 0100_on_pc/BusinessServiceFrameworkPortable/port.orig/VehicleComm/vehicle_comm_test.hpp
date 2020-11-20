#ifndef VEHICLE_COMM_TEST_HPP
#define VEHICLE_COMM_TEST_HPP

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

/** @file vehicle_comm.hpp
 * This file handles add-on test-functions for VehicleComm.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <atomic>
#include <thread>

#include "vc_message_queue.hpp"
#include "vc_common.hpp"

namespace vc {

class VehicleCommTest { 
    private:
    static std::thread loopThread;
    static MessageQueue *mq_;
    static std::atomic<bool> exit_;

    static void SendMessage();
    static void Loop();

    public:
    static void StartTestThread(MessageQueue *mq);
    static void StopTestThread();
};

} // namespace vc

#endif // VEHICLE_COMM_TEST_HPP
