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
 *  \file     smart_transaction.cc
 *  \brief    VOC framework smart transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef UNIT_TESTS
#include "voc_framework/signals/ccm_decodable.h"
#endif

#include "voc_framework/transactions/smart_transaction.h"

#include <algorithm>

namespace fsm
{

//TODO: does state_map access have to be thread safe?

SmartTransaction::SmartTransaction (State initial_state) :
    Transaction(),
    state_(initial_state),
    state_map_()
{}

void SmartTransaction::AddState(State state, StateDescriptor& state_descriptor)
{
    state_map_[state] = state_descriptor;
}

void SmartTransaction::SetStateMap(StateMap state_map)
{
    state_map_ = state_map;
}

bool SmartTransaction::SetState(State state)
{
    bool return_value = false;

    // Check that we have a descriptor.
    auto iterator = state_map_.find(state);
    if (iterator != state_map_.end())
    {
        // Check that transition is legal.
        bool transition_legal = false;

        if (state_ == kUndefinedState)
        {
            // Special case, we go to any state
            transition_legal = true;
        }
        else
        {
            // Verify the descriptor for the current state allows this transition.
            StateDescriptor state_descriptor = state_map_[state_];
            if (std::find(state_descriptor.legal_transitions.begin(),
                          state_descriptor.legal_transitions.end(), state)
                != state_descriptor.legal_transitions.end())
            {
                transition_legal = true;
            }
        }

        if (transition_legal)
        {
            StateDescriptor state_descriptor = state_map_[state];
            State old_state = state_;
            state_ = state;
            return_value = true;

            // Call state function if provided.
            if (state_descriptor.state_function != nullptr)
            {
                state_descriptor.state_function(old_state, state);
            }
        }
    }

    return return_value;
}

SmartTransaction::State SmartTransaction::GetState()
{
    return state_;
}

bool SmartTransaction::WantsSignal(std::shared_ptr<Signal> signal)
{
    return WantsSignalInternal(signal, false);
}

bool SmartTransaction::HandleSignal(std::shared_ptr<Signal> signal)
{
    // Default to not stopping the transaction.
    bool return_value = true;

    State state = GetState();

    // State may have changed since enqueue, verify that we still want signal
    if (WantsSignalInternal(signal, true))
    {
        // We implicitly know that we have a descriptor and handler
        // as WantsSignal returned true, but for safety we verify here
        // also in case WantsSignal implementation changes in future.
        // If ever a performace issue this could be removed.
        auto state_iterator = state_map_.find(state);
        if (state_iterator != state_map_.end())
        {
            // Check if we have handler for signal.
            auto signal_function_iterator =
                state_map_[state].signal_function_map.find(signal->GetSignalType());
            if (signal_function_iterator != state_map_[state].signal_function_map.end())
            {
                SignalFunction signal_function =
                    state_map_[state].signal_function_map[signal->GetSignalType()];

                // Call the handler.
                if (signal_function != nullptr)
                {
                    return_value = signal_function(signal);
                }
            }
        }
    }

    return return_value;
}

bool SmartTransaction::WantsSignalInternal(std::shared_ptr<Signal> signal, bool consider_state)
{
    bool return_value = false;

    bool match_on_session = false;
    std::string session_id;

    bool match_on_type = false;
    Signal::SignalType signal_type = Signal::BasicSignalTypes::kTypeUndefined;

    signal_type = signal->GetSignalType();

    if (IsMappedSignalType(signal_type))
    {
        match_on_type = true;
    }
    else
    {
 #ifndef UNIT_TESTS
        // as a small optimization we only do this if we have not already matched on signal type
        std::shared_ptr<CCMDecodable> decodable_signal = std::dynamic_pointer_cast<CCMDecodable>(signal);

        if (decodable_signal && decodable_signal->GetSessionId(session_id))
        {
            match_on_session = IsMappedSessionId(session_id);
        }
#else
        match_on_session = false;
#endif
    }

    // Check that transaction id is mapped
    if (match_on_type || match_on_session || IsMappedTransactionId(signal->GetTransactionId()))
    {
        State state = GetState();

        for (auto state_descriptor_pair : state_map_)
        {
            if (!consider_state || state == state_descriptor_pair.first)
            {
                // Check if we have handler for signal.
                auto signal_function_iterator =
                    state_descriptor_pair.second.signal_function_map.find(signal->GetSignalType());
                if (signal_function_iterator != state_descriptor_pair.second.signal_function_map.end())
                {
                    // Everything checks out, we want this Signal.
                    return_value = true;
                    break;
                }
            }
        }
    }

    return return_value;
}



} // namespace fsm
/** \}    end of addtogroup */
