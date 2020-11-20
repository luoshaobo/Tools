#ifndef VC_WATCHDOG_HPP
#define VC_WATCHDOG_HPP

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

/** @file vc_watchdog.hpp
 * This file handles the watchdog functionality for VehicleComm.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        15-Dec-2016
 ***************************************************************************/

#include <mutex>
#include <thread>
#include <atomic>

namespace vc {

typedef enum {
    WD_THREAD_STATE_SLEEPING,
    WD_THREAD_STATE_RUNNING,
    WD_THREAD_STATE_UNKNOWN
} WatchdogState;

typedef enum {
    WD_THREAD_VEHICLECOMM,
    WD_THREAD_VGMTIMER,
    WD_THREAD_VUCTIMER,
    WD_THREAD_MAX
} WatchdogThreads;

const char *const WatchdogThreadsStr[] = {
       "VehicleComm",
       "VGMTimer",
       "VUCTimer",
};


/**
    @brief Watchdog functionality for VehicleComm

    This class is handling watchdog functionality towards systemd
*/
class Watchdog {
private:
    std::thread loop_thread_; /**< Thread used by watchdog to reset timer in systemd  */
    uint8_t sleep_seconds_; /**< Set to how often the Watchdog should reset timer in systemd  */
    uint8_t thread_state_[WD_THREAD_MAX]; /**< Save the watchdog state of thread  */
    std::mutex mtx_;
    std::atomic<bool> exit_;

    void Loop();
    bool CheckThreads();
    void SetRunningStateUnknown();
    Watchdog(); // Prevent construction
    Watchdog(const Watchdog&); // Prevent construction by copying
    Watchdog& operator=(const Watchdog&); // Prevent assignment
    ~Watchdog(); // Prevent unwanted destruction

public:
    static Watchdog& GetInstance();
    void SetState(WatchdogThreads thread, WatchdogState state);
};

} // namespace vc

#endif //VC_WATCHDOG_HPP
