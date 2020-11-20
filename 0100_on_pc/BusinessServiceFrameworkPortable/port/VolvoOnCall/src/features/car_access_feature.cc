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
 *  \file     car_access_feature.cc
 *  \brief    VOC Service Car Access feature.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signal_sources/car_access_signal_source.h"


#include "features/car_access_feature.h"
#include "signals/ca_cat_002_signal.h"
#include "signals/ca_dpa_020_signal.h"
#include "signals/signal_types.h"
#include "transactions/admin_confirmation_transaction.h"
#include "transactions/catalogue_upload_transaction.h"
#include "transactions/device_pairing_transaction.h"
#include "transactions/key_confirmation_transaction.h"
#include "transactions/list_paired_devices_transaction.h"
#include "transactions/privacy_notification_transaction.h"
#include "transactions/role_selected_notification_transaction.h"
#include "mta/mta_transaction.h"
#include "applications_lcm/apps_lcm.h"

#ifndef VOC_TESTS
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#endif  // VOC_TESTS

#include "dlt/dlt.h"

#include <algorithm>

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{
//TODO: Unclear what this name actually is
const std::string kCarAccessFeatureName = "caraccess";

CarAccessFeature::CarAccessFeature() : fsm::Feature(kCarAccessFeatureName)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarAccessFeature::CarAccessFeature");

    //register signals with the signal factory
    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_, fsm::CarAccessSignal::CreateCarAccessSignal);
    fsm::SignalFactory::RegisterCcmFactory(CaCat002Signal::oid_, CaCat002Signal::CreateCaCat002Signal);
    fsm::SignalFactory::RegisterCcmFactory(CaDpa002Signal::oid_, CaDpa002Signal::CreateCaDpa002Signal);
    fsm::SignalFactory::RegisterCcmFactory(CaDpa020Signal::oid_, CaDpa020Signal::CreateCaDpa020Signal);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: create mta transaction", __FUNCTION__);
    std::shared_ptr<MTATransaction> transaction =std::make_shared<MTATransaction>();
    AddActiveTransaction(transaction);

#ifndef VOC_TESTS
    // Subscribe for privacy notification signals-
    // For now we run PrivacyNotifications as part of CarAccess.
    // Rationale: In VoCMO-Messages the privacy notification messages
    //            are listed under CarAccess.
    fsm::IpCommandBrokerSignalSource& ipcb_signal_source = fsm::IpCommandBrokerSignalSource::GetInstance();

    ipcb_signal_source.SubscribeForSignal(fsm::Signal::kPrivacyNotificationSignal);
    ipcb_signal_source.AddFeature(*this);

    //Subscribe for vehiclecomm signals
    fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
    vc_signal_source.AddFeature(*this);

    fsm::CarAccessSignalSource& car_access_signal_source = fsm::CarAccessSignalSource::GetInstance();
    car_access_signal_source.AddFeature(*this);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: end", __FUNCTION__);
#endif  // VOC_TESTS
}


void CarAccessFeature::HandleSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarAccessFeature::HandleSignal "
                    "type:%d signal: %s\n",
                    signal->GetSignalType(),
                    signal->ToString().c_str());

    switch (signal->GetSignalType())
    {
    case fsm::Signal::kPrivacyNotificationSignal :
    {
        std::shared_ptr<PrivacyNotificationTransaction> transaction =
            std::make_shared<PrivacyNotificationTransaction>();
        AddActiveTransaction(transaction);
        transaction->EnqueueSignal(signal);

        break;
    }
    case VocSignalTypes::kCatalogueSignal :
    {
        std::shared_ptr<CatalogueUploadTransaction> catalogue_transaction =
            std::make_shared<CatalogueUploadTransaction>();
        AddActiveTransaction(catalogue_transaction);
        catalogue_transaction->EnqueueSignal(signal);

        break;
    }
    case fsm::Signal::kListPairedDevicesRequest :
    {
        std::shared_ptr<ListPairedDevicesTransaction> transaction =
            std::make_shared<ListPairedDevicesTransaction>();
        AddActiveTransaction(transaction);
        transaction->EnqueueSignal(signal);

        break;
    }
    case fsm::Signal::kRoleSelectedNotify :
    {
        std::shared_ptr<RoleSelectedNotificationTransaction> transaction =
            std::make_shared<RoleSelectedNotificationTransaction>(signal);
        AddActiveTransaction(transaction);

        break;
    }
    case fsm::Signal::kNumberOfKeysInsideVehicleRequest :
    {
        //TODO: investigate signal to determine if this is start or abort
        std::shared_ptr<KeyConfirmationTransaction> transaction =
            std::make_shared<KeyConfirmationTransaction>(signal);
        AddActiveTransaction(transaction);

        break;
    }
    case fsm::Signal::kDevicePairingConfirmationByAdminRequest :
    {
        //TODO: investigate signal to determine if this is start or abort
        std::shared_ptr<AdminConfirmationTransaction> transaction =
            std::make_shared<AdminConfirmationTransaction>(signal);
        AddActiveTransaction(transaction);

        break;
    }
    case fsm::Signal::kPairingVisibilityRequest :
    {
        std::shared_ptr<DevicePairingTransaction> transaction =
            std::make_shared<DevicePairingTransaction>(signal);
        AddActiveTransaction(transaction);

        break;
    }
    default :
        BroadcastToTransactions(signal);
    }
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
