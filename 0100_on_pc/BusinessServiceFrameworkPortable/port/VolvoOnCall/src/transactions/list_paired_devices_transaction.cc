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
 *  \file     list_paired_devices_transaction.h
 *  \brief    Device pairing list paired devices transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "transactions/list_paired_devices_transaction.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

bool ListPairedDevicesTransaction::WantsSignal(std::shared_ptr<fsm::Signal> signal)
{
    // We are currently a oneshot transaction which does not track a transaction id
    // thus we only care about the signal type.
    return signal->GetSignalType() == fsm::Signal::kListPairedDevicesRequest;
}

bool ListPairedDevicesTransaction::HandleSignal(std::shared_ptr<fsm::Signal> signal)
{
    // by default close down after handling, we are oneshot.
    bool return_value = false;

    if (signal->GetSignalType() != fsm::Signal::kListPairedDevicesRequest)
    {
        // Do not terminate ourselves due to unexpected signals.
        return_value = true;

        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "ListPairedDevicesTransaction received unexpected signal, ignoring");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP Event getPairedDevices received");
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: entered Activity provideDevicePairing");
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "ListPairedDevicesTransaction, not implemented");

        // something like:
        // std::shared_pointer<IUserManager> user_manager = IUserManager::CreateUserManager();
        // std::vector<User> users;
        // user_manager.GetUsers(users, UserManager::UserRoles::kAdmin);
        // user_manager.GetUsers(users, UserManager::UserRoles::kUser);
        // send ListOfPairedDevicesResponse(users);
        // ToDo when implemented (and also "failed"):  DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> VGM : send list of paired devices : OK");
    }
    return return_value;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
