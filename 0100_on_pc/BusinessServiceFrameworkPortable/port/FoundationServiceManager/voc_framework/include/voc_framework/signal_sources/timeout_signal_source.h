/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     timeout_signal_source.h
 *  \brief    VOC Service timeout signal source class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_TIMEOUT_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_TIMEOUT_SIGNAL_SOURCE_H_

#include "voc_framework/signals/timeout_signal.h"
#include "voc_framework/signal_sources/timeout_receiver.h"

#include <atomic>
#include <condition_variable>
#include <chrono>
#include <memory>
#include <set>
#include <thread>


namespace fsm
{

class TimeoutSignalSource
{

 public:


    //Only one timeout signal source instance should exist in the system
    static TimeoutSignalSource& GetInstance();

    /**
     * \brief Request timeout for a given timeout receiver
     * \param[in] timeout_receiver receiver of the timeout
     * \param[in] delay time (in seconds) after which the timeout is to be issued.
     * \param[in] periodic indicates if the request is for periodic (true) or onetime (false) timeout. Defaults to false.
     * \return Transaction id of the newly created timeout, will be used in the timeout signal
     */
    TimeoutTransactionId RequestTimeout (std::shared_ptr<TimeoutReceiver> timeout_receiver,
                                         std::chrono::seconds delay,
                                         bool periodic = false);

    /**
     * \brief Request timeout for a given timeout receiver
     * \param[in] timeout_receiver receiver of the timeout
     * \param[in] timeout_id timeout id
     * \param[in] delay time (in seconds) after which the timeout is to be issued.
     * \param[in] periodic indicates if the request is for periodic (true) or onetime (false) timeout. Defaults to false.
     */
    void RequestTimeout (std::shared_ptr<TimeoutReceiver> timeout_receiver,
                                         TimeoutTransactionId timeout_id,
                                         std::chrono::seconds delay,
                                         bool periodic = false);

    /**
     * \brief Removes timeout request with a given timeout transaction id
     * \param[in] transaction_id id of the timeout to remove
     */
    void RemoveTimeout (TimeoutTransactionId transaction_id);


    /**
     * \brief Removes timeout requests for a given timeout receiver
     * \param[in] timeout_receiver timeout receiver for whom the timeout requests should be removed
     */
    void RemoveTimeouts (std::shared_ptr<TimeoutReceiver> timeout_receiver);

    /**
     * \brief Removes timeout requests for expired timeout receivers. This method can be called
     *        to avoid unnecessary wakeups of the timeout handler thread if we know that some of
     *        recipients have expired. Timeout handler is able to cope with expired recepients
     *        so this method is just to avoid unecessary wake ups.
     */
    void RemoveExpiredTimeouts ();


 private:

    /**
     * \brief Timeout request structure
     */
    struct TimeoutRequest
    {
        std::weak_ptr<TimeoutReceiver> timeout_receiver;  //*< receiver of the request
        std::chrono::steady_clock::time_point time;       //*< time to issue the request
        TimeoutTransactionId timeout_id;                  //*< id to use for the request
        bool periodic;                                    //*< periodic or onetime timeout
        std::chrono::seconds delay;                       //*< time after which the timeout should be issued
    };

    /**
     * \brief List of timeout requests
     */
    std::set<TimeoutRequest, bool (*)(const TimeoutRequest&, const TimeoutRequest&)> timeout_requests_;
    /**
     * \brief Mutex guarding access to the list of timeout requests
     */
    std::mutex  timeouts_mutex_;

    /** \brief  state variable: active (false) or stopping (true)  */
    std::atomic_bool finalizing;

    /**
     * \brief Timeout handler thread logic
     */
    void TimeoutHandler();

    /**
     * \brief Constructor, hidden as singleton
     */
    TimeoutSignalSource();

    /**
     * \brief Destructor
     */
    ~TimeoutSignalSource();


    /** \brief Timeout thread where the actual timeout handling takes place */
    std::thread timeout_thread;

    /** \brief Condition variable used to wake up the timeout thread */
    std::condition_variable timeout_synchronizer;

    static bool CompareTimeoutRequests(const TimeoutRequest& a, const TimeoutRequest& b);


};



} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_TIMEOUT_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
