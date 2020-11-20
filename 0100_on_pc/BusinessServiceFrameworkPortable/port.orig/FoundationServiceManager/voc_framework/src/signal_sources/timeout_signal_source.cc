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
 *
 *  \file     timeout_signal_source.cc
 *  \brief    VOC Service timeout signal source class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signal_sources/timeout_signal_source.h"

#include "dlt/dlt.h"

#include <algorithm>

DLT_IMPORT_CONTEXT(dlt_libfsm);


namespace fsm
{

//Only one timeout signal source instance should exist in the system
TimeoutSignalSource& TimeoutSignalSource::GetInstance()
{
    static TimeoutSignalSource instance;
    return instance;
}


TimeoutTransactionId TimeoutSignalSource::RequestTimeout (std::shared_ptr<TimeoutReceiver> timeout_receiver,
                                                          std::chrono::seconds delay,
                                                          bool periodic)
{
    std::lock_guard<std::mutex> lock(timeouts_mutex_);

    //calculate time to issue timeout
    std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now() + delay;

    //make sure to notify the timeout handler when queue is empty or if the new timeout is the earliest
    bool notify = (timeout_requests_.size() == 0) || (time < timeout_requests_.begin()->time);

    //this transaction id is autoinitialized and therefore guaranteed unique
    TimeoutTransactionId timeout_id;

    //add the new timeout request
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Registering timeout with id: %d", timeout_id.GetId());
    timeout_requests_.insert({timeout_receiver, time, timeout_id, periodic, delay});

    //sort requests according to their issuing times
   /* std::sort(timeout_requests_.begin(),
              timeout_requests_.end(),
              [](const TimeoutRequest& a, const TimeoutRequest& b)
              {
                  return (a.time) > (b.time);
              });*/

    //if notification required - notify
    if (notify)
    {
        timeout_synchronizer.notify_one();
    }

    return timeout_id;
}

void TimeoutSignalSource::RequestTimeout (std::shared_ptr<TimeoutReceiver> timeout_receiver,
                                                          TimeoutTransactionId timeout_id,
                                                          std::chrono::seconds delay,
                                                          bool periodic)
{
    std::lock_guard<std::mutex> lock(timeouts_mutex_);

    //calculate time to issue timeout
    std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now() + delay;

    //make sure to notify the timeout handler when queue is empty or if the new timeout is the earliest
    bool notify = (timeout_requests_.size() == 0) || (time < timeout_requests_.begin()->time);

    //add the new timeout request
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Registering timeout with id: %lld", timeout_id.GetId());
    timeout_requests_.insert({timeout_receiver, time, timeout_id, periodic, delay});

    //if notification required - notify
    if (notify)
    {
        timeout_synchronizer.notify_one();
    }
}

bool TimeoutSignalSource::CompareTimeoutRequests(const TimeoutSignalSource::TimeoutRequest& a, const TimeoutSignalSource::TimeoutRequest& b)
{
    return (a.time) < (b.time);
}

TimeoutSignalSource::TimeoutSignalSource(): timeout_requests_(CompareTimeoutRequests)
{
    finalizing = false;  //used to signal that thread shutdown is requested

    //create timeout handler thread
    timeout_thread = std::thread(&TimeoutSignalSource::TimeoutHandler, this);

}

TimeoutSignalSource::~TimeoutSignalSource()
{
    finalizing = true;
    timeout_synchronizer.notify_one();

    if (timeout_thread.joinable())
    {
        timeout_thread.join();
    }
}


void TimeoutSignalSource::TimeoutHandler()
{
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE, "TimeoutSignalSource enters");

    //consumer thread loops as long as the TimeoutSignalSource is not finalizing
    while (finalizing == false)
    {

            //get the signal from the queue
            std::unique_lock<std::mutex> lck(timeouts_mutex_);

            if (timeout_requests_.size() == 0)
            {
                //if no requests present then wait for simple notify
                timeout_synchronizer.wait(lck,
                                          [&]{ return timeout_requests_.size() != 0 || finalizing; });
            }
            else
            {
                //if some requests present then wait until the first timeout
                timeout_synchronizer.wait_until(lck,
                                                timeout_requests_.begin()->time,
                                                [&]{ return timeout_requests_.size() != 0 || finalizing; });
            }

            //if finalizing then we should exit the loop
            if (finalizing == true)
            {
                continue;
            }

            //find all timeouts that should be issued
            while (timeout_requests_.size() > 0 &&
                   timeout_requests_.begin()->time <= std::chrono::steady_clock::now() )
            {
                //get the request to process
                TimeoutRequest served_request = *timeout_requests_.begin();

                //if receiver has expired just ignore and remove the timeout request
                if (served_request.timeout_receiver.expired())
                {
                    timeout_requests_.erase(timeout_requests_.begin());
                    continue;
                }

                //create the timeout signal
                std::shared_ptr<TimeoutSignal> timeout_signal
                        = std::make_shared<TimeoutSignal>(served_request.timeout_id);

                //retrieve the associated timeout receiver
                std::shared_ptr<TimeoutReceiver> timeout_receiver = served_request.timeout_receiver.lock();

                if (timeout_signal && timeout_receiver)
                {
                    //Enqueue the timeout signal.
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                                    "Issuing timeout with id: %lld",
                                    served_request.timeout_id.GetId());
                    timeout_receiver->HandleTimeout(timeout_signal);
                }


                //remove timeout from the queue
                timeout_requests_.erase(timeout_requests_.begin());

                //re-add with new time if periodic with updated time
                if (served_request.periodic)
                {
                    //delay from previously computed time to minimize the impact of processing delays
                    served_request.time = served_request.time + served_request.delay;

                    //add the new timeout request
                    timeout_requests_.insert(served_request);

                }

            }
    }

    return;
}


void TimeoutSignalSource::RemoveTimeout (TimeoutTransactionId transaction_id)
{
    std::lock_guard<std::mutex> lock(timeouts_mutex_);

    for (auto it = timeout_requests_.begin(); it != timeout_requests_.end(); ) {
        if (it->timeout_id == transaction_id )
        {
            it = timeout_requests_.erase(it);
        }
        else {
            ++it;
        }
    }

}


void TimeoutSignalSource::RemoveTimeouts (std::shared_ptr<TimeoutReceiver> timeout_receiver)
{
    std::lock_guard<std::mutex> lock(timeouts_mutex_);
    for (auto it = timeout_requests_.begin(); it != timeout_requests_.end(); ) {
        if (it->timeout_receiver.expired() || it->timeout_receiver.lock() == timeout_receiver) {
            it = timeout_requests_.erase(it);
        }
        else {
            ++it;
        }
    }
}

/**
 * Removes timeouts associated with expired timeout receivers. This method can be called
 * if such situation appears and we want to avoid unnecessary wakeups of the timeout handler.
 * Note that requests from expired receivers are otherwise ignored when processed, so this
 * method is purely to avoid unecessary wakeups.
 */
void TimeoutSignalSource::RemoveExpiredTimeouts ()
{
    std::lock_guard<std::mutex> lock(timeouts_mutex_);

    for (auto it = timeout_requests_.begin(); it != timeout_requests_.end(); ) {
        if (it->timeout_receiver.expired())
        {
            it = timeout_requests_.erase(it);
        }
        else {
            ++it;
        }
    }
}

} // namespace fsm
/** \}    end of addtogroup */
