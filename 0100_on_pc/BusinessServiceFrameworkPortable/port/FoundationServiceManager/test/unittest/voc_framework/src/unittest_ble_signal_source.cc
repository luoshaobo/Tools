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
 *  \file     unittest_ble_signal_source.cc
 *  \brief    FoundationServiceManager, voc_framework, ble_signal_source, Test Cases
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <gtest/gtest.h>


#include "voc_framework/features/feature.h"
#include "voc_framework/signal_sources/ble_signal_source.h"
#include "voc_framework/signal_sources/signal_source.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/transactions/transaction.h"
#include "voc_framework/transactions/transaction_id.h"
#include "coma_mock.h"

/******************************************************************************
 * Static functions
 ******************************************************************************/

/**
 * Verify that expected calls are done when attaching to COMA events.
 */
static void VerifyAttachComaEvents(CommunicationManagerProxyMock* coma_mock, uint8_t expected_nr_of_calls)
{
    EXPECT_CALL(*coma_mock,
        attachEvent(ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_MESSAGE_ARRIVED,
                    ::testing::_))
        .Times(expected_nr_of_calls);
    EXPECT_CALL(*coma_mock,
        attachEvent(ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_CONNECTED_DEVICES_CHANGED,
                    ::testing::_))
        .Times(expected_nr_of_calls);
}

/**
 * Verify that expected calls are done when detaching from COMA events.
 */
static void VerifyDetachComaEvents(CommunicationManagerProxyMock* coma_mock, uint8_t expected_nr_of_calls)
{
    EXPECT_CALL(*coma_mock,
        detachEvent(ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_MESSAGE_ARRIVED,
                    ::testing::_))
        .Times(expected_nr_of_calls);
    EXPECT_CALL(*coma_mock,
        detachEvent(ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_CONNECTED_DEVICES_CHANGED,
                    ::testing::_))
        .Times(expected_nr_of_calls);
}

/******************************************************************************
 * TESTS
 ******************************************************************************/

/**
 * Verify that correct COMA calls are done after creating BleSignalSource instance while Coma is already initialized.
 *
 * TEST PASSES:
 *      Expected calls to COMA done.
 * TEST FAILS:
 *      Expected calls to COMA not done.
 */
TEST (BleSignalSourceTest, CreateInstanceNoComaInit)
{
    CommunicationManagerProxyMock* coma_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    uint8_t expected_nr_of_calls = 1;

    // Set expectations

    // Setup coma_mock to act as communication manager is already initialized
    EXPECT_CALL(*coma_mock, getInitStatus())
        .WillOnce(::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS));

    // Verify that test object doesn't initialize COMA
    EXPECT_CALL(*coma_mock, init(::testing::_, ::testing::_)).Times(0);

    // Verify the test object attaches to proper COMA events
    VerifyAttachComaEvents(coma_mock, expected_nr_of_calls);

    // Verify the test object detaches from proper COMA events upon destruction
    VerifyDetachComaEvents(coma_mock, expected_nr_of_calls);

    // Creating an instance of BleSignalSource will start execution of test object's code
    fsm::BleSignalSource ble_signal_source;
}

/**
 * Verify that correct COMA calls are done when COMA proxy needs to be initialized, and init succeeds.
 *
 * TEST PASSES:
 *      Expected calls to COMA done.
 * TEST FAILS:
 *      Expected calls to COMA not done.
 */
TEST (BleSignalSourceTest, GetInstanceComaInitSuccess)
{
    CommunicationManagerProxyMock* coma_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_fake = coma_mock->GetCommunicationManagerProxyFake();

    uint8_t expected_nr_of_calls = 1;

    ::testing::Sequence s1;

    // Set fake init() callback response
    ResponseComaInitClient response;
    response.set_status(Status::STATUS_SUCCESS);
    coma_fake->SetFakeInitCallbackResponse(response);

    // Set expectations

    // Setup coma_mock to act as it's in INITIALISATIONSTATUS_NOTSTARTED state
    EXPECT_CALL(*coma_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_NOTSTARTED));

    // Test object should call init()
    EXPECT_CALL(*coma_mock, init(::testing::_, ::testing::_)).
        WillOnce(
                DoAll(
                    ::testing::Invoke(coma_fake, &CommunicationManagerProxyFake::init),
                    ::testing::Return(true)));

    // Setup coma_mock to act as it's in INITIALISATIONSTATUS_SUCCESS state
    EXPECT_CALL(*coma_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS));

    // Verify the test object attaches to proper COMA events
    VerifyAttachComaEvents(coma_mock, expected_nr_of_calls);

    // Verify the test object detaches from proper COMA events upon destruction
    VerifyDetachComaEvents(coma_mock, expected_nr_of_calls);

    // Finally let the dogs out
    fsm::BleSignalSource ble_signal_source;
}

/**
 * Verify that correct COMA calls are done when COMA proxy needs to be initialized, and init fails.
 *
 * TEST PASSES:
 *      Expected calls to COMA done.
 * TEST FAILS:
 *      Expected calls to COMA not done.
 */
TEST (BleSignalSourceTest, GetInstanceComaInitFailed)
{
    CommunicationManagerProxyMock* coma_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_fake = coma_mock->GetCommunicationManagerProxyFake();

    uint8_t expected_nr_of_calls = 0;

    ::testing::Sequence s1;

    // Set fake init() callback response
    ResponseComaInitClient response;
    response.set_status(Status::STATUS_SUCCESS);
    coma_fake->SetFakeInitCallbackResponse(response);

    // Set expectations

    // Setup coma_mock to act as it's in INITIALISATIONSTATUS_NOTSTARTED state
    EXPECT_CALL(*coma_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_NOTSTARTED));

    // Test object should call init()
    EXPECT_CALL(*coma_mock, init(::testing::_, ::testing::_)).
        WillOnce(
                DoAll(
                    ::testing::Invoke(coma_fake, &CommunicationManagerProxyFake::init),
                    ::testing::Return(true)));

    // Setup coma_mock to act as it's in INITIALISATIONSTATUS_FAILED state
    EXPECT_CALL(*coma_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_FAILED));

    // Verify the test object doesn't attach to COMA events
    VerifyAttachComaEvents(coma_mock, expected_nr_of_calls);

    // Verify the test object doesn't detach from COMA events upon destruction
    VerifyDetachComaEvents(coma_mock, expected_nr_of_calls);

    // Finally let the dogs out
    fsm::BleSignalSource ble_signal_source;
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
TEST (BleSignalSourceTest, GetInstanceComaInitCallbackAfterTimeout)
{
    CommunicationManagerProxyMock* coma_mock = static_cast<CommunicationManagerProxyMock*>(
            createCommunicationManagerProxy("", ""));

    CommunicationManagerProxyFake* coma_fake = coma_mock->GetCommunicationManagerProxyFake();

    uint8_t expected_nr_of_calls = 0;

    ::testing::Sequence s1;

    // Set fake init() callback response
    ResponseComaInitClient response;
    response.set_status(Status::STATUS_SUCCESS);
    coma_fake->SetFakeInitCallbackResponse(response);

    // Set expectations

    // Setup coma_mock to act as it's in INITIALISATIONSTATUS_NOTSTARTED state
    EXPECT_CALL(*coma_mock, getInitStatus())
        .InSequence(s1)
        .WillOnce(
                ::testing::Return(ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_NOTSTARTED));

    // Test object should call init(). This time redirect the call to fake init that sleeps kComaTimeout + 1
    // before calling callback
    EXPECT_CALL(*coma_mock, init(::testing::_, ::testing::_))
        .WillOnce(
                DoAll(
                    ::testing::Invoke(coma_fake, &CommunicationManagerProxyFake::init_timeout),
                    ::testing::Return(true)));

    // Verify the test object doesn't attach to COMA events
    VerifyAttachComaEvents(coma_mock, expected_nr_of_calls);

    // Verify the test object doesn't detach from COMA events upon destruction
    VerifyDetachComaEvents(coma_mock, expected_nr_of_calls);

    // Finally let the dogs out
    fsm::BleSignalSource ble_signal_source;
}

//TEST (BleSignalSourceTest, GetInstanceComaBadInitStatus)

/** \}    end of addtogroup */
