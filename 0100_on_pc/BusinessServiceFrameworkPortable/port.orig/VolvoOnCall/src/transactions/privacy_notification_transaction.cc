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
 *  \file     privacy_notification_transaction.cc
 *  \brief    VOC Service privacy notification transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "transactions/privacy_notification_transaction.h"
#include "voc_framework/signals/ip_command_broker_signal.h"

#include "dlt/dlt.h"

#ifndef VOC_TESTS
#include "fsm_persist_data_mgr.h"
#endif

#include <sstream>


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

//TODO: This transactoin si to be considered a temporary solution for 706

const std::string PrivacyNotificationTransaction::kPrivacySettingsKey = "privacy_settings";

bool PrivacyNotificationTransaction::WantsSignal(std::shared_ptr<fsm::Signal> signal)
{
    // We are currently a oneshot transaction which does not track a transaction id
    // thus we only care about the signal type.
    return signal->GetSignalType() == fsm::Signal::kPrivacyNotificationSignal;
}

bool PrivacyNotificationTransaction::HandleSignal(std::shared_ptr<fsm::Signal> signal)
{
    // by default close down after handling, we are oneshot.
    bool return_value = false;

    if (signal->GetSignalType() != fsm::Signal::kPrivacyNotificationSignal)
    {
        // Do not terminate ourselves due to unexpected signals.
        return_value = true;

        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "PrivacyNotificationTransaction received unexpected signal, ignoring");
    }
    else
    {
#ifndef VOC_TESTS
        std::shared_ptr<fsm::PrivacyNotificationSignal> privacy_notification_signal =
            std::static_pointer_cast<fsm::PrivacyNotificationSignal>(signal);

        const PrivacyNotificationType& settings = privacy_notification_signal->GetPayload();

        std::ostringstream buffer;
        buffer << std::boolalpha << settings.CarStatUploadEn << " ";
        buffer << std::boolalpha << settings.LocationServicesEn;
        std::string buffer_string = buffer.str();

        fsm::fsmpersistdatamgr::FsmPersistDataMgr persist_manager;
        bool success = persist_manager.FsmPersistDataInit();
        if (success)
        {
            success = persist_manager.FsmPersistDataSet(kPrivacySettingsKey, buffer_string);
        }
        if (success)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                            "PrivacyNotificationTransaction saved privacy settings, "
                            "car_stat_upload %s, location_services: %s",
                            settings.CarStatUploadEn ? "true" : "false",
                            settings.LocationServicesEn ? "true" : "false");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                           "PrivacyNotificationTransaction failed to save settings");
        }
#else
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "PrivacyNotificationTransaction test mode, ignoring");
#endif
    }
    return return_value;
}

bool PrivacyNotificationTransaction::GetPrivacySettings(bool& car_stats_upload, bool& location_services)
{
    bool return_value = false;
#ifndef VOC_TESTS
    std::string settings;

    fsm::fsmpersistdatamgr::FsmPersistDataMgr persist_manager;

    return_value = persist_manager.FsmPersistDataInit();
    if (return_value)
    {
        settings = persist_manager.FsmPersistDataGet(kPrivacySettingsKey);

        std::istringstream settings_iss(settings);
        settings_iss >> std::boolalpha >> car_stats_upload >> location_services;
    }
    else
    {
        car_stats_upload = false;
        location_services = false;
    }
#endif
    return return_value;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
