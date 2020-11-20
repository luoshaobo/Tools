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
 *  \file     unittest_ipcb_signals.cc
 *  \brief    Volvo On Call, IPCommandBroker signals, Test Cases
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#include "test_classes.h"
#include "voc_framework/signals/ip_command_broker_signal.h"

#include <gtest/gtest.h>

namespace voc_ipcb_test
{

using namespace fsm;
using namespace voc_test_classes;

static bool TestPrivacyNotificationType(bool car_stat_upload,
                                        bool location_services,
                                        std::string file_name)
{
    IpCommandBrokerTransactionId transaction_id(
                                     IpCommandBrokerTransactionId::kUndefinedTransactionId);
    fsm::Signal::SignalType signal_type = fsm::Signal::kPrivacyNotificationSignal;

    const PrivacyNotificationType original_payload = {car_stat_upload, location_services};
    std::shared_ptr<fsm::PrivacyNotificationSignal> signal =
        fsm::PrivacyNotificationSignal::CreateIpCommandBrokerSignal(original_payload,
                                                               transaction_id,
                                                               signal_type);
    PrivacyNotificationType payload = signal->GetPayload();

    std::ostringstream buffer;
    buffer << static_cast<int>(signal_type) << " ";
    buffer << IpCommandBrokerTransactionId::kUndefinedTransactionId << " ";
    buffer << std::boolalpha << car_stat_upload << " ";
    buffer << std::boolalpha << location_services;
    std::string buffer_string = buffer.str();

    DumpMessageFile(file_name, buffer_string.c_str(), buffer_string.size());

    return (payload.CarStatUploadEn == original_payload.CarStatUploadEn) &&
        (payload.LocationServicesEn == original_payload.LocationServicesEn);
}

TEST (IpcbSignalsTest, TestPrivacyNotificationType)
{
    EXPECT_TRUE(TestPrivacyNotificationType(false, false,
                                            "ipcb_privacy_notification_false_false.bin"));
    EXPECT_TRUE(TestPrivacyNotificationType(false, true,
                                            "ipcb_privacy_notification_false_true.bin"));
    EXPECT_TRUE(TestPrivacyNotificationType(true, false,
                                            "ipcb_privacy_notification_true_false.bin"));
    EXPECT_TRUE(TestPrivacyNotificationType(true, true,
                                            "ipcb_privacy_notification_true_true.bin"));
}

} // namespace

/** \}    end of addtogroup */
