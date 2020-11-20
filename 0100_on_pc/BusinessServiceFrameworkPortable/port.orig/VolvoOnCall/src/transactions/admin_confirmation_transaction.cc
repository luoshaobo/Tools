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
 *  \file     admin_confirmation_transaction.h
 *  \brief    Device pairing admin confirmation transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "transactions/admin_confirmation_transaction.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

AdminConfirmationTransaction::AdminConfirmationTransaction(std::shared_ptr<fsm::Signal> initial_signal) :
    fsm::SmartTransaction(kNew)
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    StateMap state_map = {{kNew,              {nullptr,               // state function
                                               SignalFunctionMap(),   // signal function map
                                               {kAwaitingResponse}}}, // valid transitions
                          {kAwaitingResponse, {nullptr,               // state function
                                               SignalFunctionMap(),   // signal function map
                                               {kDone}}},             // valid transitions
                          {kDone,             {nullptr,               // state function
                                               SignalFunctionMap(),   // signal function map
                                               {}}}};                 // valid transitions

    state_map[kNew].signal_function_map[fsm::Signal::kDevicePairingConfirmationByAdminRequest] =
        std::bind(&AdminConfirmationTransaction::HandleDevicePairingConfirmationByAdminRequest,
                  this,
                  std::placeholders::_1);

    state_map[kAwaitingResponse].signal_function_map[fsm::Signal::kDevicePairingConfirmationByAdminDeviceResponse] =
        std::bind(&AdminConfirmationTransaction::HandleDevicePairingConfirmationByAdminDeviceResponse,
                  this,
                  std::placeholders::_1);

    SetStateMap(state_map);

    if (initial_signal)
    {
        MapTransactionId(initial_signal->GetTransactionId().GetSharedCopy());
        EnqueueSignal(initial_signal);
    }
}

bool AdminConfirmationTransaction::HandleDevicePairingConfirmationByAdminRequest(std::shared_ptr<fsm::Signal> signal)
{
    // Current stub impl emulates successful admin confirmation.

    bool return_value = true;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    // TODO:
    // Send request for admin confirmation (TBD) to admin user specified in request

    SetState(kAwaitingResponse);

    return return_value;
}

bool AdminConfirmationTransaction::HandleDevicePairingConfirmationByAdminDeviceResponse(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = false;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    // TODO:
    // Propagate response to IHU via DevicePairingConfirmationByAdminNotify

    SetState(kDone);

    return return_value;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
