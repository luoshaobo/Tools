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
 *  \file     unittest_transactions.cc
 *  \brief    Volvo On Call, Transactions, Test Cases
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <gtest/gtest.h>
#include "test_classes.h"

#include "voc_framework/signals/vehicle_comm_signal.h"
#include "voc_framework/transactions/transaction_id.h"


#include "signals/bcc_cl_001_signal.h"
#include "signals/ca_dpa_020_signal.h"
#include "signals/aes_csr_actor_voc_frame.h"
#include "signals/ecdhe_request_voc_frame.h"
#include "signals/ecdhe_approval_voc_frame.h"
#include "transactions/admin_confirmation_transaction.h"
#include "transactions/car_locator_advert_transaction.h"
#include "transactions/device_pairing_transaction.h"
#include "transactions/key_confirmation_transaction.h"
#include "transactions/role_selected_notification_transaction.h"
#include "ble_manager_mock.h"


#include <chrono>

namespace voc_transactions_test
{

using namespace volvo_on_call;
using namespace voc_test_classes;

class TestSignal : public fsm::Signal
{
public:

TestSignal (fsm::TransactionId& transaction_id, fsm::Signal::SignalType type) :
    fsm::Signal::Signal(transaction_id, type) {}

    std::string ToString() { return "TestSignal"; };
};

void WaitForSignalProcessing()
{
    std::chrono::milliseconds sleep_time(50);
    std::this_thread::sleep_for(sleep_time);
}

void WaitForTimeout(int seconds)
{
    std::chrono::milliseconds sleep_time(seconds*1000);
    std::this_thread::sleep_for(sleep_time);
}

class TestDevicePairingTransaction : public DevicePairingTransaction
{
public:

    TestDevicePairingTransaction(std::shared_ptr<fsm::Signal> initial_signal) :
        DevicePairingTransaction(initial_signal) {}

    void MapTransactionId(std::shared_ptr<fsm::TransactionId> transaction_id)
    {
        DevicePairingTransaction::MapTransactionId(transaction_id);
    }
};

/******************************************************************************
 * TESTS
 ******************************************************************************/

/**
 * Verify that newly created car advert transaction accepts the BCC_CL_001 signal
 *
 * TEST PASSES:
 *      BCCCl001Signal is accepted by the CarAdvertTransaction
 * TEST FAILS:
 *      Injected signal is not accepted by the CarAdvertTransaction
 */
TEST (TransactionsTest, CreateCarAdvertTransactionSignal)
{
    fsm::CCMTransactionId transaction_id;
    CarLocatorAdvertTransaction car_locator_advert;

    std::shared_ptr<TestBccCl001Signal> signal = TestBccCl001Signal::CreateTestBccCl001Signal
        (transaction_id, 1);
    signal->SetAdvert(BccCl001Signal::Honk);

    EXPECT_TRUE(car_locator_advert.WantsSignal(signal));
}

/**
 * Walk through the expected flow of RoleSelectedNotificationTransaction
 * and verify that it accepts signals and completes as expected.
 */
TEST (TransactionsTest, RoleSelectedNotificationTransaction)
{
    // Create ble pairing service mock
    BLEM::IPairingService *ble_pairing_service_if = BLEM::IBLEManager::GetPairingServiceInterface();
    BLEM::PairingServiceMock *ble_pairing_service_mock = dynamic_cast<BLEM::PairingServiceMock*>(ble_pairing_service_if);

    fsm::VehicleCommTransactionId transaction_id;

    fsm::ResRoleSelectedNotify res_role_selected_notify;
    res_role_selected_notify.role_admin = true;
    std::shared_ptr<fsm::Signal> role_selected_notify = std::make_shared<fsm::RoleSelectedSignal>(fsm::Signal::kRoleSelectedNotify,
                                                                                                  transaction_id,
                                                                                                  res_role_selected_notify,
                                                                                                  vc::RET_OK);
    std::shared_ptr<fsm::Signal> ca_del_002 =
        std::make_shared<TestSignal>(transaction_id, VocSignalTypes::kCaDel002Signal);
    std::shared_ptr<fsm::Signal> ca_del_010 =
        std::make_shared<TestSignal>(transaction_id, VocSignalTypes::kCaDel010Signal);
    std::shared_ptr<fsm::Signal> ca_cat_002 =
        std::make_shared<TestSignal>(transaction_id, VocSignalTypes::kCaCat002Signal);

    // Walk through states, ensure expected signals can be enqueued and result
    // in expected state change, ensure unexpected signals can not be enqueued.

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, GenerateBDAK());

    RoleSelectedNotificationTransaction transaction( role_selected_notify);

    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 1); //kAwaitingCsr, should be result of initial signal
    ASSERT_FALSE(transaction.HasCompleted());

    ASSERT_TRUE(transaction.EnqueueSignal(ca_del_002));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 1); //kAwaitingCsr, should be result of ca_del_002 (retries)
    ASSERT_FALSE(transaction.HasCompleted());

    ASSERT_TRUE(transaction.EnqueueSignal(ca_del_010));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 2); //kAwaitingCatalogueResponse
    ASSERT_FALSE(transaction.HasCompleted());

    ASSERT_TRUE(transaction.EnqueueSignal(ca_cat_002));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 3); //kDone
    ASSERT_TRUE(transaction.HasCompleted());
}

/**
 * Verify the vehicle certificates are generated during HandleRoleSelectedNotify().
 */
TEST (TransactionsTest, GenerateVehicleCertificates)
{
    // Create ble pairing service mock
    BLEM::IPairingService *ble_pairing_service_if = BLEM::IBLEManager::GetPairingServiceInterface();
    BLEM::PairingServiceMock *ble_pairing_service_mock = dynamic_cast<BLEM::PairingServiceMock*>(ble_pairing_service_if);

    fsm::VehicleCommTransactionId transaction_id;
    fsm::Keystore keystore;
    fsm::ReturnCode ret = fsm::ReturnCode::kUndefined;
    STACK_OF(X509) *certificates = sk_X509_new_null();
    X509 *cert_ca = nullptr;
    X509 *cert_actor = nullptr;
    X509 *cert_tls = nullptr;

    // Delete vehicle certificates from Keystore and make sure we are starting clean
    // CA
    keystore.DeleteCertificates(nullptr, fsm::Keystore::CertificateRole::kVehicleCa);
    ret = keystore.GetCertificates(nullptr, fsm::Keystore::CertificateRole::kVehicleCa);
    ASSERT_EQ(ret, fsm::ReturnCode::kNotFound);
    // Actor
    keystore.DeleteCertificate(nullptr, fsm::Keystore::CertificateRole::kVehicleActor);
    ret = keystore.GetCertificate(nullptr, fsm::Keystore::CertificateRole::kVehicleActor);
    ASSERT_EQ(ret, fsm::ReturnCode::kNotFound);
    // TLS server
    keystore.DeleteCertificate(nullptr, fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer);
    ret = keystore.GetCertificate(nullptr, fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer);
    ASSERT_EQ(ret, fsm::ReturnCode::kNotFound);

    fsm::ResRoleSelectedNotify res_role_selected_notify;
    res_role_selected_notify.role_admin = true;
    std::shared_ptr<fsm::Signal> role_selected_notify = std::make_shared<fsm::RoleSelectedSignal>(fsm::Signal::kRoleSelectedNotify,
                                                                                                  transaction_id,
                                                                                                  res_role_selected_notify,
                                                                                                  vc::RET_OK);

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, GenerateBDAK());

    // Start the transaction with initial RoleSelectedSignal
    RoleSelectedNotificationTransaction transaction(role_selected_notify);
    WaitForSignalProcessing();

    // The above transaction should have put the vehicle CA cert in Keystore, check it
    ret = keystore.GetCertificates(certificates, fsm::Keystore::CertificateRole::kVehicleCa);
    ASSERT_EQ(ret, fsm::ReturnCode::kSuccess);

    // There should be only one vehicle CA certificate
    ASSERT_EQ(sk_X509_num(certificates), 1);

    // Save cert to a PEM file for eventual manual inspection
    FILE * f_ca = fopen("vehicle_ca_cert.pem", "wb");
    cert_ca = sk_X509_pop(certificates);

    if (f_ca)
    {
        if (cert_ca)
        {
            PEM_write_X509(f_ca, cert_ca);
            X509_free(cert_ca);
        }
        fclose(f_ca);
    }

    sk_X509_free(certificates);

    // The above transaction should have also put the actor and tls certificates in Keystore, check it
    ret = keystore.GetCertificate(&cert_actor, fsm::Keystore::CertificateRole::kVehicleActor);
    ASSERT_EQ(ret, fsm::ReturnCode::kSuccess);
    ASSERT_NE(cert_actor, nullptr);

    ret = keystore.GetCertificate(&cert_tls, fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer);
    ASSERT_EQ(ret, fsm::ReturnCode::kSuccess);
    ASSERT_NE(cert_tls, nullptr);

    // Save certs to a PEM file for eventual manual inspection
    FILE * f_actor = fopen("vehicle_actor_cert.pem", "wb");
    FILE * f_tls = fopen("vehicle_tls_cert.pem", "wb");

    if (f_actor)
    {
        if (cert_actor)
        {
            PEM_write_X509(f_actor, cert_actor);
            X509_free(cert_actor);
        }
        fclose(f_actor);
    }

    if (f_tls)
    {
        if (cert_tls)
        {
            PEM_write_X509(f_tls, cert_tls);
            X509_free(cert_tls);
        }
        fclose(f_tls);
    }
}

/**
 * Walk through the expected flow of KeyConfirmationTransaction
 * and verify that it accepts signals and completes as expected.
 */
TEST (TransactionsTest, KeyConfirmationTransactionSuccess)
{
    fsm::VehicleCommTransactionId transaction_id;

    std::shared_ptr<fsm::Signal> number_of_keys_inside_vehicle_request =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kNumberOfKeysInsideVehicleRequest);
    std::shared_ptr<fsm::Signal> keys_found =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kKeysFound);
    std::shared_ptr<fsm::Signal> timeout =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kTimeout);

    // Walk through states, ensure expected signals can be enqueued and result
    // in expected state change, ensure unexpected signals can not be enqueued.

    KeyConfirmationTransaction transaction( number_of_keys_inside_vehicle_request);

    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 1); //kSearchingForKeys, should be result of initial signal
    ASSERT_FALSE(transaction.HasCompleted());

    ASSERT_TRUE(transaction.EnqueueSignal(keys_found));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 2); //kDone, should be result of keys_found
    ASSERT_TRUE(transaction.HasCompleted());
}

/**
 * Walk through the expected abort flow of KeyConfirmationTransaction
 * and verify that it accepts signals and completes as expected.
 */
TEST (TransactionsTest, KeyConfirmationTransactionAbort)
{
    fsm::VehicleCommTransactionId transaction_id;

    std::shared_ptr<fsm::Signal> number_of_keys_inside_vehicle_request =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kNumberOfKeysInsideVehicleRequest);
    std::shared_ptr<fsm::Signal> keys_found =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kKeysFound);
    std::shared_ptr<fsm::Signal> timeout =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kTimeout);

    // Walk through states, ensure expected signals can be enqueued and result
    // in expected state change, ensure unexpected signals can not be enqueued.

    KeyConfirmationTransaction transaction( number_of_keys_inside_vehicle_request);

    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 1); //kSearchingForKeys, should be result of initial signal
    ASSERT_FALSE(transaction.HasCompleted());

    ASSERT_TRUE(transaction.EnqueueSignal(number_of_keys_inside_vehicle_request));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 2); //kDone, should be result of number_of_keys_inside_vehicle_request
    ASSERT_TRUE(transaction.HasCompleted());
}

/**
 * Walk through the expected timeout flow of KeyConfirmationTransaction
 * and verify that it accepts signals and completes as expected.
 */
TEST (TransactionsTest, KeyConfirmationTransactionTimeout)
{
    fsm::VehicleCommTransactionId transaction_id;

    std::shared_ptr<fsm::Signal> number_of_keys_inside_vehicle_request =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kNumberOfKeysInsideVehicleRequest);
    std::shared_ptr<fsm::Signal> keys_found =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kKeysFound);
    std::shared_ptr<fsm::Signal> timeout =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kTimeout);

    // Walk through states, ensure expected signals can be enqueued and result
    // in expected state change, ensure unexpected signals can not be enqueued.

    KeyConfirmationTransaction transaction( number_of_keys_inside_vehicle_request);

    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 1); //kSearchingForKeys, should be result of initial signal
    ASSERT_FALSE(transaction.HasCompleted());

    ASSERT_TRUE(transaction.EnqueueSignal(timeout));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 2); //kDone, should be result of timeout
    ASSERT_TRUE(transaction.HasCompleted());
}

/**
 * Walk through the expected flow of AdminConfirmationTransaction
 * and verify that it accepts signals and completes as expected.
 */
TEST (TransactionsTest, AdminConfirmationTransactionSuccess)
{
    fsm::VehicleCommTransactionId transaction_id;

    std::shared_ptr<fsm::Signal> device_pairing_confirmation_by_admin_request =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kDevicePairingConfirmationByAdminRequest);
    std::shared_ptr<fsm::Signal> device_pairing_confirmation_by_admin_device_response =
        std::make_shared<TestSignal>(transaction_id, fsm::Signal::kDevicePairingConfirmationByAdminDeviceResponse);

    // Walk through states, ensure expected signals can be enqueued and result
    // in expected state change, ensure unexpected signals can not be enqueued.

    AdminConfirmationTransaction transaction( device_pairing_confirmation_by_admin_request);

    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 1); //kAwaitingResponse, should be result of initial signal
    ASSERT_FALSE(transaction.HasCompleted());

    ASSERT_TRUE(transaction.EnqueueSignal(device_pairing_confirmation_by_admin_device_response));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction.GetState(), 2); //kDone, should be result of response
    ASSERT_TRUE(transaction.HasCompleted());
}

TEST (TransactionsTest, DevicePairingTransactionSuccessVSDP)
{
    // setup keystore with default certs so its "populated"
    fsm::SetDefaultCerts();

    // setup ids to use for transaction
    fsm::CCMTransactionId ccm_transaction_id;
    fsm::VehicleCommTransactionId vc_transaction_id;

    CaDpa020Signal::DevicePairingIdType pairing_id_type =
        CaDpa020Signal::DevicePairingIdType::kDevicePairingIdTypeUuid;
    std::string pairing_id = "this is ! a UUID";

    // setup voc frame codec
    VocFrameCodec codec;

    // Create ble pairing service mock
    BLEM::IPairingService *ble_pairing_service_if = BLEM::IBLEManager::GetPairingServiceInterface();
    BLEM::PairingServiceMock *ble_pairing_service_mock = dynamic_cast<BLEM::PairingServiceMock*>(ble_pairing_service_if);
    ble_pairing_service_mock->DelegateToFake();
    const BLEM::ConnectionID ble_conn_id = 1;

    // generate a client public key
    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;
    std::vector<unsigned char> secret;
    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));

    // setup test signals

    std::shared_ptr<fsm::Signal> timeout =
        std::make_shared<TestSignal>(ccm_transaction_id, fsm::Signal::kTimeout);

    vc::Empty empty;
    std::shared_ptr<fsm::Signal> pairing_visibility_request = std::make_shared<fsm::PairingVisibilityRequestSignal>(
        fsm::Signal::BasicSignalTypes::kPairingVisibilityRequest, vc_transaction_id, empty, vc::RET_OK);

    // Create CA-DPA-002 with successful response
    std::shared_ptr<CaDpa002Signal> ca_dpa_002 =
        std::make_shared<CaDpa002Signal>(ccm_transaction_id, 1);
    ca_dpa_002->SetInitiated();

    // Create CA-DPA-020 containing a ECDHE Request Voc Frame
    std::shared_ptr<EcdheRequestVocFrame> request_frame =
        std::make_shared<EcdheRequestVocFrame>(peer_key);

    std::shared_ptr<CaDpa020Signal> ecdhe_request =
        std::make_shared<CaDpa020Signal>(ccm_transaction_id, 1);

    ASSERT_TRUE(ecdhe_request->SetDevicePairingId(pairing_id_type,
                                                  pairing_id));
    ASSERT_TRUE(ecdhe_request->SetFrame(codec, *request_frame));

    // Crate CA-DPA-020 containing ECDHE Approval
    std::shared_ptr<EcdheApprovalVocFrame> approval_frame =
        std::make_shared<EcdheApprovalVocFrame>();

    std::shared_ptr<CaDpa020Signal> ecdhe_approval =
        std::make_shared<CaDpa020Signal>(ccm_transaction_id, 1);

    ASSERT_TRUE(ecdhe_approval->SetDevicePairingId(pairing_id_type,
                                                   pairing_id));
    ASSERT_TRUE(ecdhe_approval->SetFrame(codec, *approval_frame));

    // Create CA-DPA-020 containing actor CSR
    X509_REQ* request_actor = fsm::ut_GetCsrFromPEM(fsm::CLIENT_TWO_CSR_PEM.c_str());

    std::shared_ptr<AesCsrActorVocFrame> csr_actor_frame =
        std::make_shared<AesCsrActorVocFrame>(request_actor);

    std::shared_ptr<CaDpa020Signal> csr_actor =
        std::make_shared<CaDpa020Signal>(ccm_transaction_id, 1);

    ASSERT_TRUE(csr_actor->SetDevicePairingId(pairing_id_type,
                                              pairing_id));

    // Create CA-DPA-020 containing nearfield CSR
    X509_REQ* request_nearfield = fsm::ut_GetCsrFromPEM(fsm::CLIENT_TWO_CSR_PEM.c_str());

    std::shared_ptr<AesCsrNearfieldVocFrame> csr_nearfield_frame =
        std::make_shared<AesCsrNearfieldVocFrame>(request_nearfield);

    std::shared_ptr<CaDpa020Signal> csr_nearfield =
        std::make_shared<CaDpa020Signal>(ccm_transaction_id, 1);

    ASSERT_TRUE(csr_nearfield->SetDevicePairingId(pairing_id_type,
                                                  pairing_id));

    // Create CA-CAT-002 containing success
    std::shared_ptr<TestCaCat002Signal> ca_cat_002 =
        TestCaCat002Signal::CreateTestCaCat002Signal(ccm_transaction_id, 1);

    ca_cat_002->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kCreated,
                           "success");

    // setup VC signals
    fsm::ResCodeConfirmationByUserNotify code_confirmation_by_user_data;  //ToDo: replace with proper type of VehicleComm (when available)
    code_confirmation_by_user_data.confirmation = true;
    std::shared_ptr<fsm::Signal> code_confirmation_by_user =
        std::make_shared<fsm::CodeConfirmationByUserNotifySignal>(fsm::Signal::kCodeConfirmationByUserNotify,
                                                                  vc_transaction_id,
                                                                  code_confirmation_by_user_data,
                                                                  vc::RET_OK);

    // Walk through currently mocked device pairing flow and ensure
    // signals can be enqueued and result in expected state changes.
    EXPECT_CALL(*ble_pairing_service_mock, StartPairingService());

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, RegisterDataCallback(::testing::_));
    EXPECT_CALL(*ble_pairing_service_mock, RegisterConnectionStateCallback(::testing::_));

    // Create Transaction

    std::shared_ptr<TestDevicePairingTransaction> transaction
        = std::make_shared<TestDevicePairingTransaction>(pairing_visibility_request);

    // As we currently do not have unittest ability to trap signals sent by TCAM
    // we can not respond with correct transaction/session ids, so we need
    // to map the one we will use.
    transaction->MapTransactionId(ccm_transaction_id.GetSharedCopy());

    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);
    ASSERT_FALSE(transaction->HasCompleted());

    // Test incoming BLE connection callbacks while in states kNew or kAdvertising
    // BLE connected/disconnected should not cause any state transitions
    ble_pairing_service_mock->SetConnectionState(ble_conn_id, BLEM::CONNECTION_STATE_CONNECTED);
    ble_pairing_service_mock->SendConnectionState();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);
    ble_pairing_service_mock->SetConnectionState(ble_conn_id, BLEM::CONNECTION_STATE_DISCONNECTED);
    ble_pairing_service_mock->SendConnectionState();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);

    // Simulate receiving visibility timeout response from VC
    // Expect the transaction will call StartPairingService after getting the timeouts
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);
    ASSERT_FALSE(transaction->HasCompleted());

    //wait for 1 second timeout
    std::chrono::seconds sleep_time(2);
    std::this_thread::sleep_for(sleep_time);

    // TODO (Dariusz): make sure we've sent CA-DPA-001
    // Simulate receiving CA DPA 002 from the cloud
    ASSERT_TRUE(transaction->EnqueueSignal(ca_dpa_002));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);
    ASSERT_FALSE(transaction->HasCompleted());

    // Simulate receiving CA-DPA-020 with ECDHE req from the cloud
    ASSERT_TRUE(transaction->EnqueueSignal(ecdhe_request));
    WaitForSignalProcessing();
    // TODO (Dariusz): make sure we've sent our ECDHE req to the client
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kEcdheApproval);
    ASSERT_FALSE(transaction->HasCompleted());

    // Incoming BLE connection callbacks should be ignored while pairing over VSDP
    // BLE connected
    ble_pairing_service_mock->SetConnectionState(1, BLEM::CONNECTION_STATE_CONNECTED);
    ble_pairing_service_mock->SendConnectionState();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kEcdheApproval);
    // BLE disconnected
    ble_pairing_service_mock->SetConnectionState(1, BLEM::CONNECTION_STATE_DISCONNECTED);
    ble_pairing_service_mock->SendConnectionState();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kEcdheApproval);

    // finish setting up codec by adding the now generated keys to it
    // tcam key should really come in response to client, but we dont
    // have a ssm mock to trap it
    ASSERT_TRUE(codec.SetEncryptionKeys(transaction->ecdhe_ephemeral_key_,
                                        peer_key,
                                        transaction->ecdhe_shared_secret_));
    // also finalize encrypted frame ca_dpa_020 singals now codec is complete
    ASSERT_TRUE(csr_actor->SetFrame(codec, *csr_actor_frame));
    ASSERT_TRUE(csr_nearfield->SetFrame(codec, *csr_nearfield_frame));

    // Simulate receiving CA-DPA-020 with ECDHE approval from the cloud
    ASSERT_TRUE(transaction->EnqueueSignal(ecdhe_approval));
    WaitForSignalProcessing();
    // Simulate receiving code confirmation by user notify
    ASSERT_TRUE(transaction->EnqueueSignal(code_confirmation_by_user));
    WaitForSignalProcessing();
    // TODO (Dariusz): make sure we've sent our ECDHE approval to the client
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kCreateClientCertificates);
    ASSERT_FALSE(transaction->HasCompleted());

    // Expect the transaction will call BLEM GetBDAK()
    EXPECT_CALL(*ble_pairing_service_mock, GetBDAK(::testing::_)).WillOnce(::testing::Return(BLEM::BLEM_OK));
    ASSERT_TRUE(transaction->EnqueueSignal(csr_actor));
    WaitForSignalProcessing();
    ASSERT_TRUE(transaction->EnqueueSignal(csr_nearfield));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kFinalizePairing);
    ASSERT_FALSE(transaction->HasCompleted());

    // Expect the transaction will call StopPairingRequest and deregister callbacks when it's finished
    EXPECT_CALL(*ble_pairing_service_mock, StopPairingService()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterDataCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterConnectionStateCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    ASSERT_TRUE(transaction->EnqueueSignal(ca_cat_002));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kDone);
    ASSERT_TRUE(transaction->HasCompleted());
}

static bool VocFrameSignal2VoCBaseFrame(BLEM::IVoCBaseFrame **voc_base_frame,
                                        VocFrameEncodableInterface& voc_frame_signal,
                                        VocFrameCodec& codec)
{
    bool result = false;
    std::vector<unsigned char> encoded_buffer;
    uint16_t frame_length = 0;
    BLEM::VoCBaseFrameType frame_type = BLEM::VOCBASEFRAME_TYPE_INVALID;

    switch (voc_frame_signal.GetFrameType())
    {
        case VocFrameType::kEcdheRequest:
            frame_type = BLEM::VOCBASEFRAME_TYPE_ECDHE_REQUEST;
            break;
        case VocFrameType::kEcdheApproval:
            frame_type = BLEM::VOCBASEFRAME_TYPE_ECDHE_APPROVAL;
            break;
        case VocFrameType::kAesCsrActor:
            frame_type = BLEM::VOCBASEFRAME_TYPE_AES_CSR_ACTOR;
            break;
        case VocFrameType::kAesCsrNearfield:
            frame_type = BLEM::VOCBASEFRAME_TYPE_AES_CSR_NEARFIELD;
            break;
        default:
            break;
    }

    result = codec.Encode(encoded_buffer, voc_frame_signal);

    if (result)
    {
        frame_length = encoded_buffer.size() - kVocFrameHeaderSize;
        *voc_base_frame = BLEM::IVoCBaseFrame::CreateVoCBaseFrame(frame_type, frame_length);
        result = (*voc_base_frame)->AddData(reinterpret_cast<char*>(&(encoded_buffer[kVocFrameHeaderSize])), frame_length);
    }

    return result;
}

// Googlemock matcher definitions
MATCHER_P(IsVocFrameType, type, "") {return arg->Type() == type;}

TEST (TransactionsTest, DevicePairingTransactionSuccessBLE)
{
    // setup keystore with default certs so its "populated"
    fsm::SetDefaultCerts();

    // setup ids to use for transaction
    fsm::CCMTransactionId ccm_transaction_id;
    fsm::VehicleCommTransactionId vc_transaction_id;

    CaDpa020Signal::DevicePairingIdType pairing_id_type =
        CaDpa020Signal::DevicePairingIdType::kDevicePairingIdTypeUuid;
    std::string pairing_id = "this is ! a UUID";

    // setup voc frame codec
    VocFrameCodec codec;

    // Create ble pairing service mock
    BLEM::IPairingService *ble_pairing_service_if = BLEM::IBLEManager::GetPairingServiceInterface();
    BLEM::PairingServiceMock *ble_pairing_service_mock = dynamic_cast<BLEM::PairingServiceMock*>(ble_pairing_service_if);
    ble_pairing_service_mock->DelegateToFake();

    // IVoCBaseFrame
    BLEM::IVoCBaseFrame *ble_frame = nullptr;

    const BLEM::ConnectionID ble_conn_id = 1;

    // generate a client public key
    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;
    std::vector<unsigned char> secret;
    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));

    // setup test signals
    std::shared_ptr<fsm::Signal> timeout =
        std::make_shared<TestSignal>(ccm_transaction_id, fsm::Signal::kTimeout);

    vc::Empty empty;
    std::shared_ptr<fsm::Signal> pairing_visibility_request = std::make_shared<fsm::PairingVisibilityRequestSignal>(
        fsm::Signal::BasicSignalTypes::kPairingVisibilityRequest, vc_transaction_id, empty, vc::RET_OK);

    std::shared_ptr<EcdheRequestVocFrame> request_frame =
        std::make_shared<EcdheRequestVocFrame>(peer_key);

    std::shared_ptr<EcdheApprovalVocFrame> approval_frame =
        std::make_shared<EcdheApprovalVocFrame>();

    X509_REQ* request_actor = fsm::ut_GetCsrFromPEM(fsm::CLIENT_TWO_CSR_PEM.c_str());

    std::shared_ptr<AesCsrActorVocFrame> csr_actor_frame =
        std::make_shared<AesCsrActorVocFrame>(request_actor);

    X509_REQ* request_nearfield = fsm::ut_GetCsrFromPEM(fsm::CLIENT_TWO_CSR_PEM.c_str());

    std::shared_ptr<AesCsrNearfieldVocFrame> csr_nearfield_frame =
        std::make_shared<AesCsrNearfieldVocFrame>(request_nearfield);

    std::shared_ptr<TestCaCat002Signal> ca_cat_002 =
        TestCaCat002Signal::CreateTestCaCat002Signal(ccm_transaction_id, 1);
    ca_cat_002->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kCreated,
                           "success");

    // setup VC signals
    fsm::ResCodeConfirmationByUserNotify code_confirmation_by_user_data;  //ToDo: replace with proper type of VehicleComm (when available)
    code_confirmation_by_user_data.confirmation = true;
    std::shared_ptr<fsm::Signal> code_confirmation_by_user =
        std::make_shared<fsm::CodeConfirmationByUserNotifySignal>(fsm::Signal::kCodeConfirmationByUserNotify,
                                                                  vc_transaction_id, \
                                                                  code_confirmation_by_user_data,
                                                                  vc::RET_OK);

    // Walk through currently mocked device pairing flow and ensure
    // signals can be enqueued and result in expected state changes.

    EXPECT_CALL(*ble_pairing_service_mock, StartPairingService());

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, RegisterDataCallback(::testing::_));
    EXPECT_CALL(*ble_pairing_service_mock, RegisterConnectionStateCallback(::testing::_));

    // Create Transaction
    std::shared_ptr<TestDevicePairingTransaction> transaction
        = std::make_shared<TestDevicePairingTransaction>(pairing_visibility_request);

    // As we currently do not have unittest ability to trap signals sent by TCAM
    // we can not respond with correct transaction/session ids, so we need
    // to map the one we will use.
    transaction->MapTransactionId(ccm_transaction_id.GetSharedCopy());

    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);
    ASSERT_FALSE(transaction->HasCompleted());

    // Pairing visibility timeout

    // Enqueue pairing visibility timeout response signal
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);
    ASSERT_FALSE(transaction->HasCompleted());

    // Make BLE manager calls connection callback
    ble_pairing_service_mock->SetConnectionState(ble_conn_id, BLEM::CONNECTION_STATE_CONNECTED);
    ble_pairing_service_mock->SendConnectionState();
    WaitForSignalProcessing();

    // ECDHE request
    // Create IVoCBaseFrame with ECDHE request
    ASSERT_TRUE(VocFrameSignal2VoCBaseFrame(&ble_frame, *request_frame, codec));
    // Expect the transaction will send ECDHE request using correct connection id
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_ECDHE_REQUEST)));
    // Make BLE manager mock send the frame to us
    ble_pairing_service_mock->SetFrame(ble_frame);
    ble_pairing_service_mock->SendFrame();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kEcdheApproval);
    ASSERT_FALSE(transaction->HasCompleted());

    // finish setting up codec by adding the now generated keys to it
    // tcam key should really come in response to client, but we dont
    // have a ssm mock to trap it
    ASSERT_TRUE(codec.SetEncryptionKeys(transaction->ecdhe_ephemeral_key_,
                                        peer_key,
                                        transaction->ecdhe_shared_secret_));

    // ECDHE approval
    // Create IVoCBaseFrame with ECDHE approval
    ASSERT_TRUE(VocFrameSignal2VoCBaseFrame(&ble_frame, *approval_frame, codec));
    // Make BLE manager mock send the frame to us
    ble_pairing_service_mock->SetFrame(ble_frame);
    ble_pairing_service_mock->SendFrame();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kEcdheApproval);
    ASSERT_FALSE(transaction->HasCompleted());

    // Code Confirmation by User
    // Expect the transaction will send ECDHE approval using correct connection id
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_ECDHE_APPROVAL)));
    // Enqueue code confirmation by user signal
    ASSERT_TRUE(transaction->EnqueueSignal(code_confirmation_by_user));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kCreateClientCertificates);
    ASSERT_FALSE(transaction->HasCompleted());

    // AES CSR ACTOR
    // Expect the transaction will send AES CERT Actor frame using correct connection id
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_AES_CERT_ACTOR)));
    // Create IVoCBaseFrame with AES CERT Actor
    ASSERT_TRUE(VocFrameSignal2VoCBaseFrame(&ble_frame, *csr_actor_frame, codec));
    // Make BLE manager mock send the frame to us
    ble_pairing_service_mock->SetFrame(ble_frame);
    ble_pairing_service_mock->SendFrame();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kCreateClientCertificates);
    ASSERT_FALSE(transaction->HasCompleted());

    // AES CSR NEARFIELD
    // Expect the transaction will send AES CAR Actor frame
    //                                  AES CA frame
    //                                  AES BDAK frame
    //                                  AES CERT Nearfield frame
    // using correct connection id
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_AES_CAR_ACTOR_CERT)));
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_AES_CERT_NEARFIELD)));
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_AES_CA)));
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_AES_BLE_DA_KEY)));
    // Expect the transaction will call GetBDAK
    EXPECT_CALL(*ble_pairing_service_mock, GetBDAK(::testing::_)).WillOnce(::testing::Return(BLEM::BLEM_OK));
    // Create IVoCBaseFrame with AES CERT Nearfield
    ASSERT_TRUE(VocFrameSignal2VoCBaseFrame(&ble_frame, *csr_nearfield_frame, codec));
    // Make BLE manager mock send the frame to us
    ble_pairing_service_mock->SetFrame(ble_frame);
    ble_pairing_service_mock->SendFrame();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kFinalizePairing);
    ASSERT_FALSE(transaction->HasCompleted());

    // CA CAT 002
    // Expect the transaction will call StopPairingRequest and deregister callbacks when it's finished
    EXPECT_CALL(*ble_pairing_service_mock, StopPairingService()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterDataCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterConnectionStateCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    // Enqueue ca cat 002 signal
    ASSERT_TRUE(transaction->EnqueueSignal(ca_cat_002));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kDone);
    ASSERT_TRUE(transaction->HasCompleted());
}

TEST (TransactionsTest, DevicePairingTransactionFailureBLE)
{
    // setup keystore with default certs so its "populated"
    fsm::SetDefaultCerts();

    // setup ids to use for transaction
    fsm::CCMTransactionId ccm_transaction_id;
    fsm::VehicleCommTransactionId vc_transaction_id;

    CaDpa020Signal::DevicePairingIdType pairing_id_type =
        CaDpa020Signal::DevicePairingIdType::kDevicePairingIdTypeUuid;
    std::string pairing_id = "this is ! a UUID";

    // setup voc frame codec
    VocFrameCodec codec;

    // Create ble pairing service mock
    BLEM::IPairingService *ble_pairing_service_if = BLEM::IBLEManager::GetPairingServiceInterface();
    BLEM::PairingServiceMock *ble_pairing_service_mock = dynamic_cast<BLEM::PairingServiceMock*>(ble_pairing_service_if);
    ble_pairing_service_mock->DelegateToFake();

    // IVoCBaseFrame
    BLEM::IVoCBaseFrame *ble_frame = nullptr;

    const BLEM::ConnectionID ble_conn_id = 1;

    // generate a client public key
    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;
    std::vector<unsigned char> secret;
    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));

    // setup test signals
    std::shared_ptr<fsm::Signal> timeout =
        std::make_shared<TestSignal>(ccm_transaction_id, fsm::Signal::kTimeout);

    vc::Empty empty;
    std::shared_ptr<fsm::Signal> pairing_visibility_request = std::make_shared<fsm::PairingVisibilityRequestSignal>(
        fsm::Signal::BasicSignalTypes::kPairingVisibilityRequest, vc_transaction_id, empty, vc::RET_OK);

    std::shared_ptr<EcdheRequestVocFrame> request_frame =
        std::make_shared<EcdheRequestVocFrame>(peer_key);

    std::shared_ptr<EcdheApprovalVocFrame> approval_frame =
        std::make_shared<EcdheApprovalVocFrame>();

    X509_REQ* request_actor = fsm::ut_GetCsrFromPEM(fsm::CLIENT_TWO_CSR_PEM.c_str());

    std::shared_ptr<AesCsrActorVocFrame> csr_actor_frame =
        std::make_shared<AesCsrActorVocFrame>(request_actor);

    X509_REQ* request_nearfield = fsm::ut_GetCsrFromPEM(fsm::CLIENT_TWO_CSR_PEM.c_str());

    std::shared_ptr<AesCsrNearfieldVocFrame> csr_nearfield_frame =
        std::make_shared<AesCsrNearfieldVocFrame>(request_nearfield);

    std::shared_ptr<TestCaCat002Signal> ca_cat_002 =
        TestCaCat002Signal::CreateTestCaCat002Signal(ccm_transaction_id, 1);
    ca_cat_002->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kCreated,
                           "success");

    // setup VC signals
    fsm::ResCodeConfirmationByUserNotify code_confirmation_by_user_data;  //ToDo: replace with proper type of VehicleComm (when available)
    code_confirmation_by_user_data.confirmation = true;
    std::shared_ptr<fsm::Signal> code_confirmation_by_user =
        std::make_shared<fsm::CodeConfirmationByUserNotifySignal>(fsm::Signal::kCodeConfirmationByUserNotify,
                                                                  vc_transaction_id,
                                                                  code_confirmation_by_user_data,
                                                                  vc::RET_OK);

    // Walk through currently mocked device pairing flow and ensure
    // signals can be enqueued and result in expected state changes,
    // ensure unexpected signals can not be enqueued



    // Test different error scenarios



    //------------ TEST ---------
    //Registering data callback fails. In effect the transaction should terminate
    //in state kNew


    // As StartPairingService should never be called, so register no expectation on it
    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, RegisterDataCallback(::testing::_)).
        WillOnce(::testing::Return(BLEM::BLEM_ERR_INTERNAL));

    // RegisterDataCallback is called first and if fails RegisterConnectionStateCallback should not be called
    EXPECT_CALL(*ble_pairing_service_mock, RegisterConnectionStateCallback(::testing::_)).Times(0);

    // Create Transaction
    std::shared_ptr<TestDevicePairingTransaction> transaction
        = std::make_shared<TestDevicePairingTransaction>(pairing_visibility_request);

    WaitForSignalProcessing();
    //state should remaing kNew
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kNew);
    // The transaction should be finished
    ASSERT_TRUE(transaction->HasCompleted());


    //------------ TEST ---------
    //Test BLEM StartPairingService() fails. In effect the transaction should terminate
    //in state kNew

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, RegisterDataCallback(::testing::_));
    EXPECT_CALL(*ble_pairing_service_mock, RegisterConnectionStateCallback(::testing::_));

    // The transaction will call StartPairingService after getting timeouts - make it fail
    EXPECT_CALL(*ble_pairing_service_mock, StartPairingService()).WillOnce(::testing::Return(BLEM::BLEM_ERR_INTERNAL));
    // Expect the transaction will call BLEM StopPairingService() and deregister callbacks
    EXPECT_CALL(*ble_pairing_service_mock, StopPairingService());
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterDataCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterConnectionStateCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));

    // Create transaction
    transaction = std::make_shared<TestDevicePairingTransaction>(pairing_visibility_request);

    WaitForSignalProcessing();

    //transaction should terminate in state kNew
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kNew);
    ASSERT_TRUE(transaction->HasCompleted());


    //------------ TEST ---------
    //Test remote timeout and pairing timeout received in state kAdvertising.
    //The transaction should terminate after receiving the timeout

    // Transaction should call StartPairingService
    EXPECT_CALL(*ble_pairing_service_mock, StartPairingService());

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, RegisterDataCallback(::testing::_));
    EXPECT_CALL(*ble_pairing_service_mock, RegisterConnectionStateCallback(::testing::_));

    // Create a new transaction
    transaction = std::make_shared<TestDevicePairingTransaction>(pairing_visibility_request);

    // Make sure transaction gets to kAdvertising
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);

    // The transaction should send now CA-DPA-001 and wait for response from cloud

    // Expect the transaction will call BLEM StopPairingService() and deregister callbacks after getting timeout
    EXPECT_CALL(*ble_pairing_service_mock, StopPairingService());
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterDataCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterConnectionStateCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));

    // Wait timeout_remote
    WaitForTimeout(5);

    // The transaction should be finished
    ASSERT_TRUE(transaction->HasCompleted());


    //------------ TEST ---------
    //Test BLE disconnect callback after ECDHE request over BLE.
    //Transaction should terminate.

    // Transaction should call StartPairingService
    EXPECT_CALL(*ble_pairing_service_mock, StartPairingService());

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, RegisterDataCallback(::testing::_));
    EXPECT_CALL(*ble_pairing_service_mock, RegisterConnectionStateCallback(::testing::_));

    // Create a new transaction
    transaction = std::make_shared<TestDevicePairingTransaction>(pairing_visibility_request);

    transaction->MapTransactionId(ccm_transaction_id.GetSharedCopy());

    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);
    ASSERT_FALSE(transaction->HasCompleted());

    // Make BLE manager calls connection callback
    ble_pairing_service_mock->SetConnectionState(ble_conn_id, BLEM::CONNECTION_STATE_CONNECTED);
    ble_pairing_service_mock->SendConnectionState();
    WaitForSignalProcessing();

    // ECDHE request
    // Create IVoCBaseFrame with ECDHE request
    ASSERT_TRUE(VocFrameSignal2VoCBaseFrame(&ble_frame, *request_frame, codec));
    // Expect the transaction will send ECDHE request using correct connection id
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_ECDHE_REQUEST)));
    // Make BLE manager mock send the frame to us
    ble_pairing_service_mock->SetFrame(ble_frame);
    ble_pairing_service_mock->SendFrame();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kEcdheApproval);
    ASSERT_FALSE(transaction->HasCompleted());

    // Expect the transaction will call BLEM StopPairingService() and deregister callbacks
    EXPECT_CALL(*ble_pairing_service_mock, StopPairingService());
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterDataCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterConnectionStateCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));

    // Simulate BLE disconnect callback
    ble_pairing_service_mock->SetConnectionState(ble_conn_id, BLEM::CONNECTION_STATE_DISCONNECTED);
    ble_pairing_service_mock->SendConnectionState();
    WaitForSignalProcessing();

    //expect transaction to end
    ASSERT_TRUE(transaction->HasCompleted());


    //------------ TEST ---------
    //Test sending one of the signals in kCreateClientCertificates state fails
    //Transaction should terminate.

    // Create a new transaction
    EXPECT_CALL(*ble_pairing_service_mock, StartPairingService());

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, RegisterDataCallback(::testing::_));
    EXPECT_CALL(*ble_pairing_service_mock, RegisterConnectionStateCallback(::testing::_));
    // Create
    transaction = std::make_shared<TestDevicePairingTransaction>(pairing_visibility_request);

    transaction->MapTransactionId(ccm_transaction_id.GetSharedCopy());

    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);
    ASSERT_FALSE(transaction->HasCompleted());

    // Pairing visibility timeout
    // Expect the transaction will call StartPairingService after getting the timeouts
    // Make it fail
    // Enqueue pairing visibility timeout response signal
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kAdvertising);
    ASSERT_FALSE(transaction->HasCompleted());

    // Make BLE manager calls connection callback
    ble_pairing_service_mock->SetConnectionState(ble_conn_id, BLEM::CONNECTION_STATE_CONNECTED);
    ble_pairing_service_mock->SendConnectionState();
    WaitForSignalProcessing();

    // ECDHE request
    // Create IVoCBaseFrame with ECDHE request
    ASSERT_TRUE(VocFrameSignal2VoCBaseFrame(&ble_frame, *request_frame, codec));
    // Expect the transaction will send ECDHE request using correct connection id
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_ECDHE_REQUEST)));
    // Make BLE manager mock send the frame to us
    ble_pairing_service_mock->SetFrame(ble_frame);
    ble_pairing_service_mock->SendFrame();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kEcdheApproval);
    ASSERT_FALSE(transaction->HasCompleted());

    // finish setting up codec by adding the now generated keys to it
    // tcam key should really come in response to client, but we dont
    // have a ssm mock to trap it
    ASSERT_TRUE(codec.SetEncryptionKeys(transaction->ecdhe_ephemeral_key_,
                                        peer_key,
                                        transaction->ecdhe_shared_secret_));

    // ECDHE approval
    // Create IVoCBaseFrame with ECDHE approval
    ASSERT_TRUE(VocFrameSignal2VoCBaseFrame(&ble_frame, *approval_frame, codec));
    // Make BLE manager mock send the frame to us
    ble_pairing_service_mock->SetFrame(ble_frame);
    ble_pairing_service_mock->SendFrame();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kEcdheApproval);
    ASSERT_FALSE(transaction->HasCompleted());

    // Code Confirmation by User
    // Expect the transaction will send ECDHE approval using correct connection id
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_ECDHE_APPROVAL)));
    // Enqueue code confirmation by user signal
    ASSERT_TRUE(transaction->EnqueueSignal(code_confirmation_by_user));
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kCreateClientCertificates);
    ASSERT_FALSE(transaction->HasCompleted());

    // AES CSR ACTOR
    // Expect the transaction will send AES CERT Actor frame using correct connection id
    EXPECT_CALL(*ble_pairing_service_mock,
            SendData(ble_conn_id, IsVocFrameType(BLEM::VoCBaseFrameType::VOCBASEFRAME_TYPE_AES_CERT_ACTOR)));
    // Create IVoCBaseFrame with AES CERT Actor
    ASSERT_TRUE(VocFrameSignal2VoCBaseFrame(&ble_frame, *csr_actor_frame, codec));
    // Make BLE manager mock send the frame to us
    ble_pairing_service_mock->SetFrame(ble_frame);
    ble_pairing_service_mock->SendFrame();
    WaitForSignalProcessing();
    ASSERT_EQ(transaction->GetState(), DevicePairingTransaction::States::kCreateClientCertificates);
    ASSERT_FALSE(transaction->HasCompleted());

    // AES CSR NEARFIELD
    // Expect the transaction will terminate when BLEM SendData() fails
    EXPECT_CALL(*ble_pairing_service_mock, SendData(::testing::_, ::testing::_)).
        WillOnce(::testing::Return(BLEM::BLEM_ERR_INTERNAL));
    // Expect the transaction will call BLEM StopPairingService() and deregister callbacks
    EXPECT_CALL(*ble_pairing_service_mock, StopPairingService());
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterDataCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    EXPECT_CALL(*ble_pairing_service_mock, DeregisterConnectionStateCallback()).WillOnce(::testing::Return(BLEM::BLEM_OK));
    // Create IVoCBaseFrame with AES CERT Nearfield
    ASSERT_TRUE(VocFrameSignal2VoCBaseFrame(&ble_frame, *csr_nearfield_frame, codec));
    // Make BLE manager mock send the frame to us
    ble_pairing_service_mock->SetFrame(ble_frame);
    ble_pairing_service_mock->SendFrame();
    WaitForSignalProcessing();
    // The transaction should be finished
    ASSERT_TRUE(transaction->HasCompleted());
}

TEST (TransactionsTest, DevicePairingPinGeneration1)
{
    // Create ble pairing service mock
    BLEM::IPairingService *ble_pairing_service_if = BLEM::IBLEManager::GetPairingServiceInterface();
    BLEM::PairingServiceMock *ble_pairing_service_mock = dynamic_cast<BLEM::PairingServiceMock*>(ble_pairing_service_if);

    // setup transaction
    fsm::VehicleCommTransactionId vc_transaction_id;
    std::shared_ptr<fsm::Signal> pairing_visibility_request =
        std::make_shared<TestSignal>(vc_transaction_id, fsm::Signal::kPairingVisibilityRequest);

    EXPECT_CALL(*ble_pairing_service_mock, StartPairingService());

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, RegisterDataCallback(::testing::_));
    EXPECT_CALL(*ble_pairing_service_mock, RegisterConnectionStateCallback(::testing::_));

    std::shared_ptr<TestDevicePairingTransaction>  transaction = std::make_shared<TestDevicePairingTransaction>(pairing_visibility_request);
    WaitForSignalProcessing();

    transaction->ecdhe_shared_secret_ = {0x23, 0x7d, 0xd0, 0xcc, 0x75, 0xfd, 0x83, 0x3b,
                                         0x8a, 0x44, 0x0e, 0x2f, 0x2d, 0x49, 0xe8, 0xd6,
                                         0x7e, 0x41, 0x16, 0xde, 0xbf, 0xc5, 0x8b, 0x58,
                                         0x79, 0xff, 0xb8, 0x80, 0x4d, 0xa3, 0x8e, 0x2e};

    std::string generated_pin;
    std::string expected_pin = "688599";

    ASSERT_TRUE(transaction->GetOnScreenSecret(generated_pin));

    ASSERT_EQ(generated_pin, expected_pin);
}

/**
 * Test that pin generation produces output according to exxamples in reqprod
 */
TEST (TransactionsTest, DevicePairingPinGeneration2)
{

    // Create ble pairing service mock
    BLEM::IPairingService *ble_pairing_service_if = BLEM::IBLEManager::GetPairingServiceInterface();
    BLEM::PairingServiceMock *ble_pairing_service_mock = dynamic_cast<BLEM::PairingServiceMock*>(ble_pairing_service_if);

    // setup transaction
    fsm::VehicleCommTransactionId vc_transaction_id;
    std::shared_ptr<fsm::Signal> pairing_visibility_request =
        std::make_shared<TestSignal>(vc_transaction_id, fsm::Signal::kPairingVisibilityRequest);

    EXPECT_CALL(*ble_pairing_service_mock, StartPairingService());

    // Expect the transaction will register BLEM callbacks
    EXPECT_CALL(*ble_pairing_service_mock, RegisterDataCallback(::testing::_));
    EXPECT_CALL(*ble_pairing_service_mock, RegisterConnectionStateCallback(::testing::_));

    std::shared_ptr<TestDevicePairingTransaction>  transaction = std::make_shared<TestDevicePairingTransaction>(pairing_visibility_request);
    WaitForSignalProcessing();


    transaction->ecdhe_shared_secret_ = {0xb8, 0xde, 0x7e, 0x16, 0x79, 0xff, 0xa3, 0x44,
                                         0xd6, 0x0e, 0x2f, 0x2d, 0x41, 0x80, 0x4d, 0x49,
                                         0xd0, 0xcc, 0x75, 0xbf, 0x8e, 0x2e, 0x23, 0x7d,
                                         0xfd, 0x83, 0x3b, 0x8a, 0xc5, 0x8b, 0x58, 0xe8};

    std::string generated_pin;
    std::string expected_pin = "777876";

    ASSERT_TRUE(transaction->GetOnScreenSecret(generated_pin));

    ASSERT_EQ(generated_pin, expected_pin);
}

} // namespace

/** \}    end of addtogroup */
