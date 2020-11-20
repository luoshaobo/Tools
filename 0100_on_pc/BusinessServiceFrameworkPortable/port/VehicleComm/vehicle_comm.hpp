#ifndef VEHICLE_COMM_HPP
#define VEHICLE_COMM_HPP

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
 * This file handles the core-functions of VehicleComm.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <atomic>

//#include "vsomeip/vsomeip.hpp"
#include <vsomeip/vsomeip.hpp>
//#include "vsomeip.hpp"
#include "vehicle_comm_messages.hpp"

#include "vc_lcm.hpp"
#include "vc_watchdog.hpp"
#include "vc_data_storage.hpp"
#include "vc_message_processor.hpp"
#include "vc_message_queue.hpp"

#include "lan_comm/lan_comm.hpp"
#include "lan_comm/vgm_comm.hpp"
#include "lan_comm/ihu_comm.hpp"
#include "proxy_host/ts_comm.hpp"
#include "vuc_comm/vuc_comm.hpp"

#define LIFECYCLEMGR_SERVICE_NAME "com.contiautomotive.Lifecycle"
#define VC_LOCK_FILE "/var/run/vc-lock"
#define TRACE_SIZE 100

namespace vc {

/**
    @brief Main class in the VC module.

    This class is the central part of VC, linking all
    parts (VC-endpoints) together. The endpoints are separate
    modules to which a VC-message can be sent.
    Endpoints connected to the VC-endpoint:
        VuCComm
        VGMComm
        TSComm
        DataStorage
*/
class VehicleComm {
  private:
    static long session_id_next_;

    static void SignalHandler(int sig);             
    static void StackTraceSignalHandler(int sig);
    static void ServiceAvailable(const gchar* service_name);

    std::mutex mtx_;                               /**< Mutex for watch-dog */
    std::condition_variable cv_;                   /**< Used for wake-up of VC */
    MessageQueue mq_;                              /**< The main message-queue */

    DataStorage ds_;                               /**< Handling communication with DataStorage */
    VuCComm vuc_;                                  /**< Handling communication with the VuC */
    LANComm lan_;                                  /**< Handling communication with the VGM */
    VGMComm vgm_;
    IHUComm ihu_;
    TSComm ts_;                                    /**< Handling communication with the TS/clients */
    MessageProcessor mp_;                          /**< Handling communication with the RequestProcessor */
    Watchdog& wd_;                                 /**< Watch-dog stuff */
    LCM& lcm_;                                     /**< Power/life-cycle management */


    std::atomic<bool> exit_;                       /**< Set to exit VC-thread */

#ifdef VC_PRINTMEM
    void PrintMem();
#endif
    ReturnValue HandleMessage(MessageQueue &mq);
    ReturnValue Suspend();
    ReturnValue Resume();
    ReturnValue LCMHandler(VCLCMSignal sig);

  public:
    static VehicleComm& GetInstance();
    static long GetSessionID();

    VehicleComm();
    ~VehicleComm();

    ReturnValue Init();
    ReturnValue Start();
    ReturnValue Stop();
};

} // namespace vc

#endif // VEHICLE_COMM_HPP
