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
 *  \file     signal_source.h
 *  \brief    VOC Service signal source base class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_SIGNAL_SOURCE_H_

#include "voc_framework/signal_sources/signal_receiver_interface.h"
#include "voc_framework/signals/signal.h"

#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace fsm
{

class SignalSource
{

 public:

    /**
     * \brief Add feature to the list of features which want to receive signals.
     *        \deprecated This function is deprecated and replaced by RegisterSignalReceiver.
     * \param[in] feature The feature to add.
     * \return None.
     */
    void AddFeature (SignalReceiverInterface& feature);

    /**
     * \brief Remove feature from the list of features which want to receive signals.
     *        \deprecated This function is deprecated and replaced by UnregisterSignalReceiver.
     * \param[in] feature The feature to remove.
     * \return None.
     */
    void RemoveFeature (SignalReceiverInterface& feature);

    /**
     * \brief Add signal receiver to the list of signal receivers which want
     *        to receive signals.
     * \param[in] signal_receiver The signal receiver to add.
     * \return None.
     */
    virtual void RegisterSignalReceiver (SignalReceiverInterface& signal_receiver);

    /**
     * \brief Remove signal receiver from the list of signal receivers which want
     *        to receive signals.
     * \param[in] signal_receiver The signal receiver to remove.
     * \return None.
     */
    virtual void UnregisterSignalReceiver (SignalReceiverInterface& signal_receiver);

    /**
     * \brief Broadcast a signal to all add signal receivers.
     * \param[in] signal - The signal to broadcast.
     * \return None
     */
    void BroadcastSignal (std::shared_ptr<Signal> signal);

    virtual ~SignalSource () {};

 private:

    std::vector<std::reference_wrapper<SignalReceiverInterface>> receivers_;

    /**
     * \brief Mutex guarding access to the list of signal receivers
     */
    std::mutex  receivers_mutex_;

    /** \brief Condition variable used to synchronize access to receivers list */
    std::condition_variable receivers_synchronizer_;

    /**
     * \brief constant used to indicate that there is an active modifier
     */
    const int ACTIVE_MODIFIER = -1;

    /**
     * \brief ACTIVE_MODIFIER means active modifier, otherwise keeps the count of currently ongoing broadcasting sessions.
     */
    int locks_ = 0;

    /**
     * \brief Aquires a broadcaster lock. Broadcaster can run in parallel with other broadcasters
     *        but not with a modifier
     */
    void LockBroadcaster();


    /**
     * \brief Aquires a modifier lock. Modifiers can only run exclusively, cannot run iin parallel
     *        with broadcasters or other modifiers
     */
    void LockModifier();

    /**
     * \brief Releases the lock (both for modifiers and broadcasters)
     */
    void ReleaseLock();

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
