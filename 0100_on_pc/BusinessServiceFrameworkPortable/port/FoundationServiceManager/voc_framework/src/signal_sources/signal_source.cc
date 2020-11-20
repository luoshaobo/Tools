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
 *  \file     signal.cc
 *  \brief    VOC Service signal source base class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signal_sources/signal_source.h"

#include <algorithm>

namespace fsm
{

void SignalSource::RegisterSignalReceiver(SignalReceiverInterface& signal_receiver)
{

    LockModifier();

    receivers_.push_back(std::reference_wrapper<SignalReceiverInterface>(signal_receiver));
    //if any signal source specific initialization of functions is required
    //it may be provided here
    ReleaseLock();
    return;
}

void SignalSource::UnregisterSignalReceiver(SignalReceiverInterface& signal_receiver)
{
    LockModifier();

    receivers_.erase(std::remove_if(receivers_.begin(),
                                    receivers_.end(),
                                    [&](SignalReceiverInterface& stored_receiver)
                                    {
                                        return &stored_receiver == &signal_receiver;
                                    }),
                   receivers_.end());
    ReleaseLock();
    return;
}

void SignalSource::AddFeature(SignalReceiverInterface& feature)
{

    RegisterSignalReceiver(feature);
    return;
}

void SignalSource::RemoveFeature(SignalReceiverInterface& feature)
{
    UnregisterSignalReceiver(feature);
    return;
}

void SignalSource::BroadcastSignal(std::shared_ptr<Signal> signal)
{
    LockBroadcaster();


    for (SignalReceiverInterface& receiver : receivers_)
    {
        receiver.ProcessSignal(signal);
    }

    ReleaseLock();
}


void SignalSource::LockModifier()
{
    std::unique_lock<std::mutex> lock_receivers(receivers_mutex_);

    //upon wake up check if not spurious wakeup. Wake up if nobody active
    //TODO: there is a risk for modifier starvation if there is a constant
    //      inflow of broadcast requests. Current assumption is that this
    //      will not be a case. However, if this turns out to be an issue,
    //      the a suggestion is to extend the algorithm with a logic
    //      to not allow further broadcasters when there appears a new modifier.
    //      When the currently executing broadcasters end their execution the
    //      modifier would do its task and then the remaining broadcasters would
    //      be allowed to continue.
    receivers_synchronizer_.wait(lock_receivers, [&]{return ( locks_ == 0 ) ;});

    //-1 for list modifier
    locks_ = ACTIVE_MODIFIER;
}

void SignalSource::LockBroadcaster()
{
    std::unique_lock<std::mutex> lock_receivers(receivers_mutex_);

    //upon wake up check if not spurious wakeup. Wake up if no writer active
    receivers_synchronizer_.wait(lock_receivers, [&]{return ( locks_ != ACTIVE_MODIFIER ) ;});

    //increase read counter
    locks_++;
}

void SignalSource::ReleaseLock()
{
    std::lock_guard<std::mutex> lock_receivers(receivers_mutex_);

    //if list modifier is releasing the lock, set 0, otherwise decrease
    if (locks_ == ACTIVE_MODIFIER)
    {
        locks_ = 0;
    }
    else
    {
        locks_--;
    }

    //if locks become 0 notify all. This will either unblock modifiers that waited for broadcasters/other modifiers
    //to finish or broadcasters that were blocked by a list modifier.
    if (locks_ == 0)
    {
        receivers_synchronizer_.notify_all();
    }

}

} // namespace fsm
/** \}    end of addtogroup */
