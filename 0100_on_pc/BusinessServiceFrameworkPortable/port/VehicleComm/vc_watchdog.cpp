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

/** @file vc_watchdog.cpp
 * This file handles the watchdog functionality for VehicleComm.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        15-Dec-2016
 ***************************************************************************/

#include <iostream>
#include <mutex>
#include <thread>
#include <stdio.h>
#include <sys/stat.h>
#include <systemd/sd-daemon.h>

#include "include/vc_log.hpp"
#include "include/vc_watchdog.hpp"

namespace vc {

/**
    @brief Watchdog constructor
    
    Initiates the Watchdog with values set by systemd.
*/
Watchdog::Watchdog()
{
    char *wdog_usecs_env = NULL;
    unsigned int wdog_secs = 0;
    sleep_seconds_ = 0;
    exit_ = false;

    LOG(LOG_DEBUG, "Watchdog: %s.", __FUNCTION__);

    for(int i = 0; i < WD_THREAD_MAX; i++)
        thread_state_[i] = WD_THREAD_STATE_SLEEPING;

    LOG(LOG_DEBUG, "Watchdog: %s: before setup_watchdog", __FUNCTION__);

    wdog_usecs_env = getenv("WATCHDOG_USEC");
    if (wdog_usecs_env != NULL) {
        wdog_secs = (atoi(wdog_usecs_env) / 1000000) / 3;
        if (wdog_secs) {
            sleep_seconds_ = wdog_secs;
        }
    }

    LOG(LOG_DEBUG, "Watchdog:  watchdog %s: env=%sus, pat=%usec",
    (wdog_secs)? "set" : "not set", wdog_usecs_env, wdog_secs);

    if(sleep_seconds_ > 0) {
        loop_thread_ = std::thread(&Watchdog::Loop, this);
        pthread_setname_np(loop_thread_.native_handle(), "Watchdog");
    }
    else
        LOG(LOG_WARN, "Watchdog: WATCHDOG_USEC probably not set");

}

/**
    @brief Watchdog destructor
*/
Watchdog::~Watchdog()
{
    exit_ = true;
    LOG(LOG_DEBUG, "Watchdog: %s.", __FUNCTION__);
}

/**
    @brief Reset running state to state unknown 

    This method puts all running threads to state unknown
*/
void Watchdog::SetRunningStateUnknown()
{
    LOG(LOG_DEBUG, "Watchdog: %s.", __FUNCTION__);
    std::lock_guard<std::mutex> guard(mtx_);
    for(int i = 0; i < WD_THREAD_MAX; i++) {
        if(thread_state_[i] == WD_THREAD_STATE_RUNNING){
            thread_state_[i] = WD_THREAD_STATE_UNKNOWN;
        }
    }
}

/**
    @brief Check states of threads

    This method checks if any of the watchdog threads are in unknown state
    @return     Returns true if frozen
*/
bool Watchdog::CheckThreads()
{
    bool thread_is_frozen = false;

    LOG(LOG_DEBUG, "Watchdog: %s.", __FUNCTION__);

    std::lock_guard<std::mutex> guard(mtx_);
    for(int i = 0; i < WD_THREAD_MAX; i++) {
        if(thread_state_[i] == WD_THREAD_STATE_UNKNOWN) {
            thread_is_frozen = true;
            LOG(LOG_WARN, "Watchdog: thread (%s) seems blocked", WatchdogThreadsStr[i]);
        }
    }
    return thread_is_frozen;
}

/**
    @brief Watchdog loop

    This method loops forever and resets the watchdog timer if none if the threads are stuck.
*/
void Watchdog::Loop()
{
    LOG(LOG_DEBUG, "Watchdog: %s.", __FUNCTION__);
    while(!exit_) {
        if(!CheckThreads()) {  //Check so no of the threads state is still unknown
            sd_notify(0, "WATCHDOG=1");
            LOG(LOG_DEBUG, "Watchdog: sd_notify called");
        }
        SetRunningStateUnknown();
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_seconds_*1000));
    }
}


/**
    @brief Set the state of a certain thread

    This method sets the state of a certain watchdog thread.
    @param[in]  thread      Watchdog thread representation
    @param[in]  state       Thread state
*/
void Watchdog::SetState(WatchdogThreads thread, WatchdogState state)
{
    std::lock_guard<std::mutex> guard(mtx_);
    if(thread < WD_THREAD_MAX)
        thread_state_[thread] = state;
}

/**
    @brief Get the singleton instance of Watchdog
*/
Watchdog& Watchdog::GetInstance()
{
    LOG(LOG_DEBUG, "Watchdog: %s.", __FUNCTION__);
    static Watchdog inst;
    return inst;
}

} // namespace vc
