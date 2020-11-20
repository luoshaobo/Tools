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
 *  \file     unittest_vocmo_signal_source.cc
 *  \brief    FoundationServiceManager/voc_framework: VocmoSignalSource tests
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <gtest/gtest.h>

#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signals/car_access_signal.h"
#include "signals/bcc_cl_002_signal.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/signal_factory.h"
#include "usermanager_types.h"
#include "coma_mock.h"
#include "utils.h"
#include "../../ccm/certUtils.h"

#include "../../ccm/stub/keystore_stub.h"



/******************************************************************************
 * STATIC FUNCTIONS
 ******************************************************************************/

MATCHER_P(IsResourceDescriptorType, type, "") {return arg.m_type == type;}
/**
 * Prepare test environment for SendMessage tests
 */
static void SendMessagePrepare(CommunicatorProxyMock** communicator_mock_out,
                              CommunicatorDataMock** communicator_data_mock_out)
{
    CommunicationManagerProxyMock* coma_mgr_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_mgr_fake = coma_mgr_mock->GetCommunicationManagerProxyFake();

    CommunicatorProxyMock* communicator_mock = coma_mgr_fake->GetCommunicatorProxyMock();

    CommunicatorDataMock* communicator_data_mock = communicator_mock->GetCommunicatorDataMock();

    CommunicatorDataFake* communicator_data_fake = communicator_data_mock->GetCommunicatorDataFake();

    // Setup expectations before creating an instance of VocmoSignalSource

    // Setup coma_mgr_mock to act as communication manager is already initialized
    EXPECT_CALL(*coma_mgr_mock, getInitStatus())
        .WillOnce(::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS));

    // Expect that test object will call getCommunicator() with resource type RES_TYPE_CALL
    EXPECT_CALL(*coma_mgr_mock,
            getCommunicator(IsResourceDescriptorType(ResourceDescriptor::RES_TYPE_CALL),
                            ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::getCommunicator));

    // Delegate sendMessage() calls to CommunicatorData fake class which contains the test logic (spawn threads
    // executing callbacks, etc.)
    EXPECT_CALL(*communicator_data_mock, sendMessage(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillRepeatedly(::testing::Invoke(communicator_data_fake, &CommunicatorDataFake::sendMessage));

    // Expect the VocmoSignalSource object will release the communicator (by id) it aquired
    const int communicator_id = 1;
    EXPECT_CALL(*communicator_mock, getCommunicatorId())
        .WillOnce(::testing::Return(communicator_id));
    EXPECT_CALL(*coma_mgr_mock, releaseCommunicator(communicator_id));

    *communicator_mock_out = communicator_mock;
    *communicator_data_mock_out = communicator_data_mock;
}


/******************************************************************************
 * TEST CASES
 ******************************************************************************/


/**
 * Test SendMessage().
 *
 * TEST PASSES:
 *      All expected calls done.
 * TEST FAILS:
 *      Something fails.
 */
TEST (VocmoSignalSourceTest, SendMessage)
{
    //
    // Prepare test environment
    //
    CommunicatorProxyMock* communicator_mock;
    CommunicatorDataMock* communicator_data_mock;

    SendMessagePrepare(&communicator_mock, &communicator_data_mock);

    CommunicatorDataFake* communicator_data_fake = communicator_data_mock->GetCommunicatorDataFake();

    // Create an instance of VocmoSignalSource
    fsm::VocmoSignalSource vocmo_signal_source;

    // Create a vector of users
    std::vector<fsm::UserId> users;
    // Add a user
    fsm::UserId user = static_cast<fsm::UserId>(1);
    users.push_back(user);
    // Add another user
    user = static_cast<fsm::UserId>(2);
    users.push_back(user);
    // Add yet another user
    user = static_cast<fsm::UserId>(3);
    users.push_back(user);

    CallbackCommunicatorDataSendMessageToUsers callback_parameter;
    ResponseCommunicatorDataSendMessageToUsers response_parameter;

    // Setup fake callback parameter with expected vector of users
    for (std::vector<fsm::UserId>::iterator it = users.begin() ; it != users.end(); ++it)
    {
        callback_parameter.add_useridlist(static_cast<::google::protobuf::int32>(*it));
    }
    communicator_data_fake->SetFakeCallbackParameter(callback_parameter);

    //
    // Test SendMessage with sendMessage() result STATUS_SUCCESS
    //

    // Setup fake response with status STATUS_SUCCESS
    response_parameter.set_status(
            com::contiautomotive::communicationmanager::messaging::internal::Status::STATUS_SUCCESS);

    communicator_data_fake->SetFakeResponseParameter(response_parameter);

    // Expect that test object will call getCommunicatorData()
    EXPECT_CALL(*communicator_mock, getICommunicatorData());

    // Create a test CCM message
    fsm::GenerateCerts();
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<volvo_on_call::BccCl002Signal> signal =
        volvo_on_call::BccCl002Signal::CreateBccCl002Signal(transaction_id, 1);
    signal->SetRecipients(users);
    signal->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kCreated);

    // Send the CCM message
    bool result = vocmo_signal_source.SendMessage(signal);

    // Verify the above operation succeeded (as the fake response status was set to STATUS_SUCCESS)
    EXPECT_EQ(true, result);


    //
    // Test SendMessage with sendMessage() result STATUS_FAILED
    //
    // Setup fake response with status STATUS_FAILED
    response_parameter.set_status(
            com::contiautomotive::communicationmanager::messaging::internal::Status::STATUS_FAILED);

    communicator_data_fake->SetFakeResponseParameter(response_parameter);

    // Expect that test object will call getCommunicatorData()
    EXPECT_CALL(*communicator_mock, getICommunicatorData());

    // Send the CCM message
    result = vocmo_signal_source.SendMessage(signal);

    // Verify the above operation failed (as the fake response status was set to STATUS_FAILED)
    EXPECT_EQ(false, result);
}

/**
 * Verify that signals are delivered to registered signal receivers. Send a signal from coma mock and
 * verify it has been received by registered test signal receiver.
 *
 * TEST PASSES:
 *      Signal received.
 *
 * TEST FAILS:
 *      Signal not received.
 */
TEST (VocmoSignalSourceTest, ReceiveSignal)
{
    CommunicationManagerProxyMock* coma_mgr_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_mgr_fake = coma_mgr_mock->GetCommunicationManagerProxyFake();

    CommunicatorProxyMock* communicator_mock = coma_mgr_fake->GetCommunicatorProxyMock();

    // Setup coma_mgr_mock to act as communication manager is already initialized
    EXPECT_CALL(*coma_mgr_mock, getInitStatus())
        .WillOnce(::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS));

    // Expect that test object will call getCommunicator() with resource type RES_TYPE_CALL
    EXPECT_CALL(*coma_mgr_mock,
            getCommunicator(IsResourceDescriptorType(ResourceDescriptor::RES_TYPE_CALL),
                            ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::getCommunicator));

    fsm::VocmoSignalSource vocmo_signal_source;

    // Registering a signal receiver should trigger BleSignalSource registration to COMA
    EXPECT_CALL(*coma_mgr_mock, getInitStatus()).
        WillOnce(::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS));

    // attachEvent should be called two times (for data and connection status events)
    // save the callback interfaces for it for later use to push events
    EXPECT_CALL(*coma_mgr_mock, attachEvent(
                ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_MESSAGE_ARRIVED, ::testing::_))
        .WillOnce(::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::attachEvent));
    EXPECT_CALL(*coma_mgr_mock, attachEvent(
                ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_CONNECTED_DEVICES_CHANGED, ::testing::_))
        .WillOnce(::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::attachEvent));

    // Create and register a test signal receiver
    TestSignalReceiverInterface receiver;
    vocmo_signal_source.RegisterSignalReceiver(receiver);

    // Create and encode a test CCM message (CarAccessSignal) that will be "received" from COMA/BleSignalSource
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<fsm::CarAccessSignal> signal =
        std::make_shared<fsm::CarAccessSignal>(transaction_id, 1);

    signal->SetAdressUri("/this_is_a_test_uri");
    signal->SetCatalogueUplink("catalogue_uplink");
    signal->SetCatalogueDownlink("catalogue_downlink");
    signal->SetDelegateUplink("delegate_uplink");
    signal->SetDelegateDownlink("delegate_downlink");
    signal->SetDevicePairingUplink("device_pairing_uplink");
    signal->SetDevicePairingDownlink("device_pairing_downlink");

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic;
    uplink_topic.topic = "uplink topic";
    uplink_topic.priority = 1;
    // services are optional, dont set any
    signal->AddUplinkTopic(uplink_topic);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic;
    downlink_topic.topic = "downlink topic";
    downlink_topic.priority = 1;
    // services are optional, dont set any
    signal->AddDownlinkTopic(downlink_topic);

    signal->SetRecipients(fsm::users);

    // Encode it
    fsm::GenerateCerts();
    std::vector<fsm::UserId> recipients; // no need to encrypt, leaving empty
    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);
    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    // Create a COMA Ble message with the encoded CCM message
    NotificationBluetoothLEIncomingMessage ble_message;
    std::string buffer_string(buffer->begin(), buffer->end());
    ble_message.set_payload(buffer_string);

    // Setup the signal factory so that it can decode the CCM message
    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_,
                                           fsm::CarAccessSignal::CreateCarAccessSignal);

    // Send the event to ble message callback interface
    coma_mgr_fake->SendEvent(&ble_message);
    ASSERT_EQ(receiver.process_signal_count_, 1);

    // Expect the VocmoSignalSource object will release the communicator (by id) it aquired
    const int communicator_id = 1;
    EXPECT_CALL(*communicator_mock, getCommunicatorId())
        .WillOnce(::testing::Return(communicator_id));
    EXPECT_CALL(*coma_mgr_mock, releaseCommunicator(communicator_id));

    fsm::FreeCerts();
}

/**
 * Verify that VocmoSignalSource registers/deregisters to/from BleSignalSource only when first/last
 * SignalReceiverInterface registers/deregisters to/from it.
 *
 * TEST PASSES:
 *      As explained above.
 *
 * TEST FAILS:
 *      Registration/deregistration not done as expected.
 */
TEST (VocmoSignalSourceTest, RegisterDeregisterSignalReceiver)
{
    CommunicationManagerProxyMock* coma_mgr_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_mgr_fake = coma_mgr_mock->GetCommunicationManagerProxyFake();

    CommunicatorProxyMock* communicator_mock = coma_mgr_fake->GetCommunicatorProxyMock();

    CommunicatorDataMock* communicator_data_mock = communicator_mock->GetCommunicatorDataMock();

    // Setup expectations before creating an instance of VocmoSignalSource
    // Setup coma_mgr_mock to act as communication manager is already initialized
    EXPECT_CALL(*coma_mgr_mock, getInitStatus())
        .WillOnce(::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS));

    // Expect that test object will call getCommunicator() with resource type RES_TYPE_CALL
    EXPECT_CALL(*coma_mgr_mock,
            getCommunicator(IsResourceDescriptorType(ResourceDescriptor::RES_TYPE_CALL),
                            ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::getCommunicator));

    // Expect the VocmoSignalSource object will release the communicator (by id) it aquired
    const int communicator_id = 1;
    EXPECT_CALL(*communicator_mock, getCommunicatorId())
        .WillOnce(::testing::Return(communicator_id));
    EXPECT_CALL(*coma_mgr_mock, releaseCommunicator(communicator_id));

    fsm::VocmoSignalSource vocmo_signal_source;

    // Registering a signal receiver should trigger BleSignalSource registration to COMA
    EXPECT_CALL(*coma_mgr_mock, getInitStatus()).
        WillOnce(::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS));

    // attachEvent should be called two times (for data and connection status events)
    // save the callback interfaces for it for later use to push events
    EXPECT_CALL(*coma_mgr_mock, attachEvent(
                ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_MESSAGE_ARRIVED, ::testing::_))
        .WillOnce(::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::attachEvent));
    EXPECT_CALL(*coma_mgr_mock, attachEvent(
                ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_CONNECTED_DEVICES_CHANGED, ::testing::_))
        .WillOnce(::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::attachEvent));

    TestSignalReceiverInterface receiver1, receiver2, receiver3;

    // As per expectations set above only one BleSignalSource registration should happen
    vocmo_signal_source.RegisterSignalReceiver(receiver1);
    vocmo_signal_source.RegisterSignalReceiver(receiver2);
    vocmo_signal_source.RegisterSignalReceiver(receiver3);

    // No COMA calls are expected until the last receiver unregisters
    vocmo_signal_source.UnregisterSignalReceiver(receiver3);
    vocmo_signal_source.UnregisterSignalReceiver(receiver2);

    // detachEvent should be called two times (for data and connection status events)
    EXPECT_CALL(*coma_mgr_mock, detachEvent(
                ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_MESSAGE_ARRIVED, ::testing::_))
        .Times(1);
    EXPECT_CALL(*coma_mgr_mock, detachEvent(
                ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_CONNECTED_DEVICES_CHANGED, ::testing::_))
        .Times(1);

    vocmo_signal_source.UnregisterSignalReceiver(receiver1);
}

/**
 * Verify that correct COMA calls are done after creating VocmoSignalSource instance while Coma is already initialized.
 *
 * TEST PASSES:
 *      Expected calls to COMA done.
 * TEST FAILS:
 *      Expected calls to COMA not done.
 */
TEST (VocmoSignalSourceTest, CreateInstanceNoComaInit)
{
    CommunicationManagerProxyMock* coma_mgr_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_mgr_fake = coma_mgr_mock->GetCommunicationManagerProxyFake();

    CommunicatorProxyMock* communicator_mock = coma_mgr_fake->GetCommunicatorProxyMock();

    // Set expectations

    // Setup coma_mgr_mock to act as communication manager is already initialized
    EXPECT_CALL(*coma_mgr_mock, getInitStatus())
        .WillOnce(::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS));

    // Verify that test object doesn't initialize COMA
    EXPECT_CALL(*coma_mgr_mock, init(::testing::_, ::testing::_)).Times(0);

    // Expect that test object will call getCommunicator() with resource type RES_TYPE_CALL
    EXPECT_CALL(*coma_mgr_mock,
            getCommunicator(IsResourceDescriptorType(ResourceDescriptor::RES_TYPE_CALL),
                            ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::getCommunicator));

    // Creating an instance of VocmoSignalSource will start execution of test object's code
    fsm::VocmoSignalSource vocmo_signal_source;

    // Expect the VocmoSignalSource object will release the communicator (by id) it aquired
    const int communicator_id = 1;
    EXPECT_CALL(*communicator_mock, getCommunicatorId())
        .WillOnce(::testing::Return(communicator_id));
    EXPECT_CALL(*coma_mgr_mock, releaseCommunicator(communicator_id));
}

/**
 * Verify that correct COMA calls are done when COMA proxy needs to be initialized, and init succeeds.
 *
 * TEST PASSES:
 *      Expected calls to COMA done.
 * TEST FAILS:
 *      Expected calls to COMA not done.
 */
TEST (VocmoSignalSourceTest, GetInstanceComaInitSuccess)
{
    CommunicationManagerProxyMock* coma_mgr_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_mgr_fake = coma_mgr_mock->GetCommunicationManagerProxyFake();

    CommunicatorProxyMock* communicator_mock = coma_mgr_fake->GetCommunicatorProxyMock();

    ::testing::Sequence s1;

    // Set fake init() callback response
    ResponseComaInitClient response;
    response.set_status(Status::STATUS_SUCCESS);
    coma_mgr_fake->SetFakeInitCallbackResponse(response);

    // Set expectations

    // Setup coma_mgr_mock to act as it's in INITIALISATIONSTATUS_NOTSTARTED state
    EXPECT_CALL(*coma_mgr_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_NOTSTARTED));

    // Test object should call init()
    EXPECT_CALL(*coma_mgr_mock, init(::testing::_, ::testing::_))
        .WillOnce(
                DoAll(
                    ::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::init),
                    ::testing::Return(true)));

    // Setup coma_mgr_mock to act as it's in INITIALISATIONSTATUS_SUCCESS state
    EXPECT_CALL(*coma_mgr_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS));

    // Expect that test object will call getCommunicator() with resource type RES_TYPE_CALL
    EXPECT_CALL(*coma_mgr_mock,
            getCommunicator(IsResourceDescriptorType(ResourceDescriptor::RES_TYPE_CALL),
                            ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::getCommunicator));

    // Create an instance of test object
    fsm::VocmoSignalSource vocmo_signal_source;

    // Expect the VocmoSignalSource object will release the communicator (by id) it aquired
    const int communicator_id = 1;
    EXPECT_CALL(*communicator_mock, getCommunicatorId())
        .WillOnce(::testing::Return(communicator_id));
    EXPECT_CALL(*coma_mgr_mock, releaseCommunicator(communicator_id));
}

/**
 * Verify that correct COMA calls are done when COMA proxy needs to be initialized, and init fails.
 *
 * TEST PASSES:
 *      Expected calls to COMA done.
 * TEST FAILS:
 *      Expected calls to COMA not done.
 */
TEST (VocmoSignalSourceTest, GetInstanceComaInitFailed)
{
    CommunicationManagerProxyMock* coma_mgr_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_mgr_fake = coma_mgr_mock->GetCommunicationManagerProxyFake();

    ::testing::Sequence s1;

    // Set fake init() callback response
    ResponseComaInitClient response;
    response.set_status(Status::STATUS_SUCCESS);
    coma_mgr_fake->SetFakeInitCallbackResponse(response);

    // Set expectations

    // Setup coma_mgr_mock to act as it's in INITIALISATIONSTATUS_NOTSTARTED state
    EXPECT_CALL(*coma_mgr_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_NOTSTARTED));

    // Test object should call init()
    EXPECT_CALL(*coma_mgr_mock, init(::testing::_, ::testing::_))
        .WillOnce(
                DoAll(
                    ::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::init),
                    ::testing::Return(true)));

    // Setup coma_mgr_mock to act as it's in INITIALISATIONSTATUS_FAILED state
    EXPECT_CALL(*coma_mgr_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_FAILED));

    // Create an instance of test object
    fsm::VocmoSignalSource vocmo_signal_source;
}

/**
 * Verify that correct COMA calls are done when COMA proxy needs to be initialized, and the init callback
 * comes after the test object has timed-out.
 *
 * TEST PASSES:
 *      Expected calls to COMA done.
 * TEST FAILS:
 *      Expected calls to COMA not done.
 */
TEST (VocmoSignalSourceTest, GetInstanceComaInitCallbackAfterTimeout)
{
    CommunicationManagerProxyMock* coma_mgr_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_mgr_fake = coma_mgr_mock->GetCommunicationManagerProxyFake();

    ::testing::Sequence s1;

    // Set fake init() callback response
    ResponseComaInitClient response;
    response.set_status(Status::STATUS_SUCCESS);
    coma_mgr_fake->SetFakeInitCallbackResponse(response);

    // Set expectations

    // Setup coma_mgr_mock to act as it's in INITIALISATIONSTATUS_NOTSTARTED state
    EXPECT_CALL(*coma_mgr_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_NOTSTARTED));

    // Test object should call init(). This time redirect the call to fake init that sleeps kComaTimeout + 1
    // before calling callback
    EXPECT_CALL(*coma_mgr_mock, init(::testing::_, ::testing::_))
        .WillOnce(
                DoAll(
                    ::testing::Invoke(coma_mgr_fake, &CommunicationManagerProxyFake::init_timeout),
                    ::testing::Return(true)));

    // Create an instance of test object
    fsm::VocmoSignalSource vocmo_signal_source;
}

/** \}    end of addtogroup */
