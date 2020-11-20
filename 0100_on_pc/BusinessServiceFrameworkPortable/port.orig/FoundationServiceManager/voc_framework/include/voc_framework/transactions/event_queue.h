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
 *  \file     event_queue.h
 *  \brief    event queue template class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup SW_package
 *  \{
 */

#ifndef VOC_FRAMEWORK_TRANSACTIONS_EVENT_QUEUE_H_
#define VOC_FRAMEWORK_TRANSACTIONS_EVENT_QUEUE_H_


#include <condition_variable>
#include <mutex>
#include <deque>
#include <vector>
#include <thread>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{
template<typename T>
class EventQueue
{
 public:

    /**
     * \brief Destructs the event queue object. Will perform consumer thread
     *        stopping, unless the thread has been already stopped. As the destructor
     *        is blocked until the consumer thread completes, it is recommended
     *        to stop the event queue either by calling Stop() or by returning
     *        false from HandleSignal() prior to destructing the transaction.
     *
     */
    virtual ~EventQueue ()
    {
        Terminate();
    };

    /**
     * \brief Adds new event to the event queue. The event is either
     *        enqueued for later processing or immediately rejected.
     *  \param[in]  event - event for processing.
     *  \return false if rejected (e.g. transaction finalizing), true if enqueued for processing.
     */
    bool EnqueueEvent (T event)
    {
        //event should be enqueued only if the event queue is
        //active and the signal is relevant for it
        if (IsActive())
        {
            std::lock_guard<std::mutex> lock(event_queue_mutex_);

            //signals are added to the end of queue, except timeouts
            event_queue_.push_back(event);

            event_queue_synchronizer_.notify_one();
            return true;
        }
        return false;
    }

    /**
     * \brief Inserts new event if front of the transaction queue.
     *  \param[in]  event - event for processing.
     *  \return false if rejected (e.g. transaction finalizing), true if enqueued in front for processing.
     */
    bool EnqueueEventAsFirst (T event)
    {
        //signal should be enqueued only if the transaction is
        //active and the signal is relevant for it
        if ( IsActive())
        {
            std::lock_guard<std::mutex> lock(event_queue_mutex_);

            //put in front of the queue
            event_queue_.push_front(event);

            event_queue_synchronizer_.notify_one();
            return true;
        }
        return false;
    };

    /**
     * \brief Informs the event queue to stop its operations. Should be used to
     *        terminate an ongoing event queue. Does not need to be called if
     *        the event queue has been requested to stop by returning false
     *        from HandleEvent()
     */
    void Stop()
    {
        if (IsActive() )
        {
            finalizing_ = true;
            event_queue_synchronizer_.notify_one();
        }
    }

    /**
     * \brief Informs if the event queue is active and accepting new events,
     *        or if the termination of the event queue has been initiated.
     *        It is meant for informational purposes, does not need to be checked
     *        prior to enqueuing new events.
     *  \return false if event queue is stopping, true if active.
     */
    bool IsActive()
    {
        return !finalizing_;
    }

    /**
     * \brief Informs if the event queue has completed and can be destroyed. This
     *        should always be checked prior to destroying the transaction object,
     *        as destructor will wait for event queue to complete thus blocking
     *        the executing thread.
     *  \return true if event queue has completed and can be destroyes, false otherwise.
     */
    bool HasCompleted()
    {
        return completed_;
    };


 private:
    /** \brief Queue containing events for processing */
    std::deque<T> event_queue_;

    /** \brief Consumer thread where the actual event processing takes place */
    std::thread event_consumer_thread_;

    /** \brief Mutex synchronizing access to the signal_queue */
    std::mutex event_queue_mutex_;

    /** \brief Condition variable used to wake up the consumer thread */
    std::condition_variable event_queue_synchronizer_;

    /** \brief Event queue state variable: active (false) or stopping (true)  */
    bool finalizing_;

    /** \brief Event queue state variable: completed (true) or active/stopping (false)  */
    bool completed_;

    /** \brief Consumer thread logic  */
    void ConsumeEvents ()
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_VERBOSE,"ConsumeEvents enters");

        //consumer thread loops as long as the transaction is not finalizing
        while (finalizing_ == false )
        {
            T event;
            {
                //get the event from the queue
                std::unique_lock<std::mutex> lck(event_queue_mutex_);

                //upon wake up check if not spurious wakeup. Wake up if there are events to process
                //or if the transaction is finalizing
                event_queue_synchronizer_.wait(lck, [&]{return ( ( event_queue_.size()!=0 ) || finalizing_ );});

                //if finalizing then we should exit the loop
                if (finalizing_ == true)
                {
                    continue;
                }

                //get oldest signal from the queue
                event = event_queue_.front();
                event_queue_.pop_front();
            }

            if ( !HandleEvent(event) )
            {
                //set finalizing when the last event has been processed
                //(false returned to indicate shutdown request)
                finalizing_ = true;
            }
        }
        //set state to completed and finish the execution
        completed_ = true;
        return;
    };


 protected:

    /**
     * \brief Constructor is hidden, as only subclasses should be instatiated.
     *        Subclasses must make sure to execute this constructor as well.
     */
    EventQueue ()
    {
        finalizing_ = false;  //used to signal that shutdown is requested
        completed_ = false;   //used to signal that shutdown is completed

        event_consumer_thread_ = std::thread(&EventQueue::ConsumeEvents, this);
    };

    /**
     * \brief Event handling logic, this method must be overriden in the subclasses.
     * \param[in]  event Event for processing.
     * \return true if further events are expected, false when no more events
     *         are expected and the event queue can be terminated
     */
    virtual bool HandleEvent (T event)
    {
        return false;
    };

    /**
     * \brief Makes sure that the consumer thread has successfully completed
     *        This method MUST be called at latest in the destructor of classes
     *        inheriting (directly or idirectly) from EventQueue
     */
    virtual void Terminate () final
    {
        if ( IsActive() )
        {
           Stop();
        }

        if (event_consumer_thread_.joinable())
        {
            event_consumer_thread_.join();
        }
    };
};

} // namespace fsm

#endif //VOC_FRAMEWORK_TRANSACTIONS_EVENT_QUEUE_H_

/** \}    end of addtogroup */
