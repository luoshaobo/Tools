/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file  	unittest_dla_transaction.cc
// This file handles DLA Unittest
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Lei Li
// @Init date   22-Mar-2019
///////////////////////////////////////////////////////////////////

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <gtest/gtest.h>

#include "download_agent/car_downloadagent_transaction.h"
#include "voc_framework/signals/ip_command_broker_signal.h"

namespace voc_dla_transaction_test
{

using namespace fsm;
using namespace volvo_on_call;

class DLA_TEST : public testing::Test
{
public:
	static void SetUpTestCase()
        {
	    dla_trans = new CarDownloadAgentTransaction();
	}
	static void TearDownTestCase() 
	{
	    delete dla_trans;
	}
	virtual void SetUp()
	{

	}
	virtual void TearDown()
	{

	}

	static CarDownloadAgentTransaction* dla_trans;
};
  
CarDownloadAgentTransaction* DLA_TEST::dla_trans = nullptr;

//Test default state
TEST_F(DLA_TEST, StateTest)
{
   ASSERT_EQ(dla_trans->GetState(), CarDownloadAgentTransaction::kHandleSignal);
}

//Test Sync Request
TEST_F(DLA_TEST, SyncRequest)
{
    IpCommandBrokerTransactionId transaction_id;
    OpGeneric_Request_Data *pData = new OpGeneric_Request_Data();
    Signal::SignalType signal_type = Signal::kOTAAssignmentSyncReq;
    std::shared_ptr<Signal> signal = GenericRequestSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(dla_trans->HandleOTAAssignmentSyncRequest(signal));
}

//Test Download Consent
TEST_F(DLA_TEST, DownloadConsent)
{
    IpCommandBrokerTransactionId transaction_id;
    OpDownloadConsent_Notification_Data *pData = new OpDownloadConsent_Notification_Data();
    pData->installationorder.uuid = "TEST";
    Signal::SignalType signal_type = Signal::kDownloadConsentNotification;
    std::shared_ptr<Signal> signal = DownloadConsentSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(dla_trans->HanldeDownloadConsent(signal));
}

//Test Query Storage
TEST_F(DLA_TEST, QueryStorage)
{
    IpCommandBrokerTransactionId transaction_id;
    OpOTAQueryAvailableStorage_Response_Data *pData = new OpOTAQueryAvailableStorage_Response_Data();
    Signal::SignalType signal_type = Signal::kOTAQueryAvailableStorageResp;
    std::shared_ptr<Signal> signal = QueryAvailableStorageRespSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(dla_trans->HanldeOTAQueryAvailableStorageResp(signal));
}

//Test Assignment Notification
TEST_F(DLA_TEST, AssignmentNotification)
{
    IpCommandBrokerTransactionId transaction_id;
    OpOTAAssignmentNotification_Notification_Data *pData = new OpOTAAssignmentNotification_Notification_Data();
    pData->newstatus = "TEST";
    Signal::SignalType signal_type = Signal::kOTAAssignmentNotification;
    std::shared_ptr<Signal> signal = AssignmentNotificationDataSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_FALSE(dla_trans->HandleOTAAssignmentNotification(signal));
}

//Test HMI Language Settings
TEST_F(DLA_TEST, HMILanguageSettings)
{
    IpCommandBrokerTransactionId transaction_id;
    OpOTAHMILanguageSettings_Notification_Data *pData = new OpOTAHMILanguageSettings_Notification_Data();
    Signal::SignalType signal_type = Signal::kOTAHMILanguageSettingsNotification;
    std::shared_ptr<Signal> signal = OTAHMILanguageSettingsSignal::CreateIpCommandBrokerSignal(*pData, transaction_id, signal_type);
	
    ASSERT_TRUE(dla_trans->HanldeOTAHMILanguageSettings(signal));
}

} //namespace
