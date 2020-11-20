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
 *  \file     key_confirmation_transaction.h
 *  \brief    Device pairing key confirmation transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "transactions/key_confirmation_transaction.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

KeyConfirmationTransaction::KeyConfirmationTransaction(std::shared_ptr<fsm::Signal> initial_signal) : fsm::SmartTransaction(kNew)
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    StateMap state_map = {{kNew,              {nullptr,               // state function
                                               SignalFunctionMap(),   // signal function map
                                               {kSearchingForKeys}}}, // valid transitions
                          {kSearchingForKeys, {nullptr,               // state function
                                               SignalFunctionMap(),   // signal function map
                                               {kDone}}},             // valid transitions
                          {kDone,             {nullptr,               // state function
                                               SignalFunctionMap(),   // signal function map
                                               {}}}};                 // valid transitions

    state_map[kNew].signal_function_map[fsm::Signal::kNumberOfKeysInsideVehicleRequest] =
        std::bind(&KeyConfirmationTransaction::HandleNumberOfKeysInsideVehicleRequest,
                  this,
                  std::placeholders::_1);

    state_map[kSearchingForKeys].signal_function_map[fsm::Signal::kNumberOfKeysInsideVehicleRequest] =
        std::bind(&KeyConfirmationTransaction::HandleNumberOfKeysInsideVehicleRequest,
                  this,
                  std::placeholders::_1);

    state_map[kSearchingForKeys].signal_function_map[fsm::Signal::kKeysFound] =
        std::bind(&KeyConfirmationTransaction::HandleKeysFoundNotification,
                  this,
                  std::placeholders::_1);

    state_map[kSearchingForKeys].signal_function_map[fsm::Signal::kTimeout] =
        std::bind(&KeyConfirmationTransaction::HandleTimeout,
                  this,
                  std::placeholders::_1);


    SetStateMap(state_map);

    if (initial_signal)
    {
        MapTransactionId(initial_signal->GetTransactionId().GetSharedCopy());
        EnqueueSignal(initial_signal);
    }
}

bool KeyConfirmationTransaction::HandleNumberOfKeysInsideVehicleRequest(std::shared_ptr<fsm::Signal> signal)
{
    // Current stub impl emulates successful key search.

    bool return_value = true;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());


    if (GetState() == kNew)
    {
        // TODO:
        // Request CEM to start search (TBD)
        // Start Timer
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> VGM : get localized car keys : n/a");  // ToDo: substitute "n/a" with the result of that call
        SetState(kSearchingForKeys);
    }
    else if (GetState() == kSearchingForKeys)
    {
        // TODO:
        // Request CEM to stop search (TBD)
        // Stop timer

        return_value = false;
        SetState(kDone);
    }

    return return_value;
}

bool KeyConfirmationTransaction::HandleKeysFoundNotification(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = false;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    // TODO:
    // Issue NumerOfKeysInsideVehicleNotify
    // Check which user role is beeing paired, RoleSelectedNotificationTransaction.GetRole()
    // or if possible check signal data
    // if (role == admin && all registered keys found) ||
    //    (role == user && at least one key found)
    //         Request CEM to stop search (TBD)
    //         Stop timer
    //         Goto kDone
    // ToDo in case 'else': add DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SOME_IP send SOME_IP_send(error,IHU)");

    SetState(kDone);

    return return_value;
}

bool KeyConfirmationTransaction::HandleTimeout(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = false;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    // TODO:
    // Issue NumerOfKeysInsideVehicleNotify
    // Request CEM to stop search (TBD)
    // Stop timer

    SetState(kDone);

    return return_value;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
