/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     unittest_ccm_signals.cc
 *  \brief    Volvo On Call, ccm signals, Test Cases
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#include <fstream>
#include <gtest/gtest.h>
#include <openssl/ssl.h>
#include <random>
#include <string>

#include <vector>
#include "test_classes.h"
#include <openssl/x509.h>

#define VOC_CCM_TEST 1

#include "signals/aes_ble_da_key_voc_frame.h"
#include "signals/basic_car_control.h"
#include "signals/basic_car_control_signal.h"
#include "signals/bcc_cl_001_signal.h"
#include "signals/bcc_cl_002_signal.h"
#include "signals/bcc_cl_020_signal.h"
#include "signals/ca_cat_001_signal.h"
#include "signals/ca_cat_002_signal.h"
#include "signals/ca_del_001_signal.h"
#include "signals/ca_del_002_signal.h"
#include "signals/ca_dpa_001_signal.h"
#include "signals/ca_dpa_002_signal.h"
#include "signals/ca_dpa_020_signal.h"
#include "signals/ca_dpa_021_signal.h"
#include "signals/ecdhe_approval_voc_frame.h"
#include "signals/signal_types.h"

#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signals/entry_point_signal.h"
#include "fscfgd/expiry_notification_signal.h"
#include "voc_framework/signals/features_signal.h"
#include "voc_framework/signals/response_info_data.h"


namespace voc_ccm_test
{

using namespace volvo_on_call;
using namespace voc_test_classes;


TEST (CCMSignalsTest, EncodeDecodeCaCat002)
{
    fsm::CCMTransactionId transaction_id;

    CA_CAT_002_t* asn1c_ca_cat_002 = nullptr;
    UTF8String_t info = {0};

    long int msgcode = 0;
    const std::string kResponseInfo_PR_success("ResponseInfoPRsuccess");
    const std::string kResponseInfo_PR_error("ResponseInfoPRerror");

    std::shared_ptr<TestCaCat002Signal> test_signal = TestCaCat002Signal::CreateTestCaCat002Signal(transaction_id, 1);

    if (nullptr == test_signal)
    {
        FAIL();
    }

    test_signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(test_signal);
    ASSERT_TRUE(buffer == nullptr);

    //Success
    test_signal->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kCreated,
                            kResponseInfo_PR_success);

    fsm::SetCertsForEncode();

    buffer = fsm::SignalFactory::Encode(test_signal);

    ASSERT_TRUE(buffer != nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_cat_002_signal_success.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(CaCat002Signal::oid_, CaCat002Signal::CreateCaCat002Signal);
    std::shared_ptr<fsm::Signal> decoded_signal_success = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    ASSERT_FALSE(nullptr == decoded_signal_success);

    ASSERT_EQ(typeid(*decoded_signal_success).hash_code(), typeid(CaCat002Signal).hash_code());
    ASSERT_EQ(decoded_signal_success->GetSignalType(), VocSignalTypes::kCaCat002Signal);

    std::shared_ptr<CaCat002Signal> typed_decoded_signal_success =
        std::static_pointer_cast<CaCat002Signal>(decoded_signal_success);

    fsm::ResponseInfoData::ResponseCodeSuccess success_decoded_code =
        typed_decoded_signal_success->GetSuccessCode();
    EXPECT_EQ(success_decoded_code, fsm::ResponseInfoData::ResponseCodeSuccess::kCreated);
    std::string success_decoded_info = typed_decoded_signal_success->GetInfoMessage();
    EXPECT_EQ(success_decoded_info.compare(kResponseInfo_PR_success), 0);

    //Error
    test_signal->SetError(fsm::ResponseInfoData::ResponseCodeError::kBadRequest,
                          kResponseInfo_PR_error);

    fsm::SetCertsForEncode();

    buffer = fsm::SignalFactory::Encode(test_signal);

    ASSERT_TRUE(buffer != nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_cat_002_signal_error.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(CaCat002Signal::oid_, CaCat002Signal::CreateCaCat002Signal);
    std::shared_ptr<fsm::Signal> decoded_signal_error = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    ASSERT_FALSE(nullptr == decoded_signal_error);

    ASSERT_EQ(typeid(*decoded_signal_error).hash_code(), typeid(CaCat002Signal).hash_code());
    ASSERT_EQ(decoded_signal_error->GetSignalType(), VocSignalTypes::kCaCat002Signal);

    std::shared_ptr<CaCat002Signal> typed_decoded_signal_error =
        std::static_pointer_cast<CaCat002Signal>(decoded_signal_error);


    fsm::ResponseInfoData::ResponseCodeError error_decoded_code =
        typed_decoded_signal_error->GetErrorCode();
    EXPECT_EQ(error_decoded_code, fsm::ResponseInfoData::ResponseCodeError::kBadRequest);
    std::string error_decoded_info = typed_decoded_signal_error->GetInfoMessage();
    EXPECT_EQ(error_decoded_info.compare(kResponseInfo_PR_error), 0);
}

TEST (CCMSignalsTest, EncodeDecodeCaDel002)
{
    fsm::CCMTransactionId transaction_id;

    CA_DEL_002_t* asn1c_ca_del_002 = nullptr;
    UTF8String_t info = {0};

    long int msgcode = 0;
    const std::string kResponseInfo_PR_success("ResponseInfoPRsuccess");
    const std::string kResponseInfo_PR_error("ResponseInfoPRerror");

    // Create test signal
    std::shared_ptr<TestCaDel002Signal> test_signal = TestCaDel002Signal::CreateTestCaDel002Signal(transaction_id, 1);

    if (nullptr == test_signal)
    {
        FAIL();
    }

    test_signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    // Verify that encoding an empty signal returns nullptr buffer
    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(test_signal);
    ASSERT_TRUE(buffer == nullptr);

    // Set result to success
    test_signal->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kCreated,
                            kResponseInfo_PR_success);

    fsm::SetCertsForEncode();

    // Encode
    buffer = fsm::SignalFactory::Encode(test_signal);

    DumpMessageFile("ca_del_002_signal_success.bin", (char*)(buffer->data()), buffer->size());

    ASSERT_TRUE(buffer != nullptr);
    ASSERT_GT(buffer->size(), 0);

    fsm::SetCertsForDecode();

    // Decode
        fsm::SignalFactory::RegisterCcmFactory(CaDel002Signal::oid_, CaDel002Signal::CreateCaDel002Signal);
    std::shared_ptr<fsm::Signal> decoded_signal_success = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());


    ASSERT_FALSE(nullptr == decoded_signal_success);

    // Verify encoded and decoded signals' types match
    ASSERT_EQ(typeid(*decoded_signal_success).hash_code(), typeid(CaDel002Signal).hash_code());
    ASSERT_EQ(decoded_signal_success->GetSignalType(), VocSignalTypes::kCaDel002Signal);

    std::shared_ptr<CaDel002Signal> typed_decoded_signal_success =
        std::static_pointer_cast<CaDel002Signal>(decoded_signal_success);

    // Verify the encoded and decoded result codes and infos match
    fsm::ResponseInfoData::ResponseCodeSuccess success_decoded_code =
        typed_decoded_signal_success->GetSuccessCode();
    EXPECT_EQ(success_decoded_code, fsm::ResponseInfoData::ResponseCodeSuccess::kCreated);
    std::string success_decoded_info = typed_decoded_signal_success->GetInfoMessage();
    EXPECT_EQ(success_decoded_info.compare(kResponseInfo_PR_success), 0);

    // Set result to error
    test_signal->SetError(fsm::ResponseInfoData::ResponseCodeError::kBadRequest,
                          kResponseInfo_PR_error);

    fsm::SetCertsForEncode();

    // Encode
    buffer = fsm::SignalFactory::Encode(test_signal);

    ASSERT_TRUE(buffer != nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_del_002_signal_error.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    // Decode
    fsm::SignalFactory::RegisterCcmFactory(CaDel002Signal::oid_, CaDel002Signal::CreateCaDel002Signal);
    std::shared_ptr<fsm::Signal> decoded_signal_error = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    ASSERT_FALSE(nullptr == decoded_signal_error);

    // Verify encoded and decoded signals' types match
    ASSERT_EQ(typeid(*decoded_signal_error).hash_code(), typeid(CaDel002Signal).hash_code());
    ASSERT_EQ(decoded_signal_error->GetSignalType(), VocSignalTypes::kCaDel002Signal);

    std::shared_ptr<CaDel002Signal> typed_decoded_signal_error =
        std::static_pointer_cast<CaDel002Signal>(decoded_signal_error);


    // Verify the encoded and decoded result codes and infos match
    fsm::ResponseInfoData::ResponseCodeError error_decoded_code =
        typed_decoded_signal_error->GetErrorCode();
    EXPECT_EQ(error_decoded_code, fsm::ResponseInfoData::ResponseCodeError::kBadRequest);
    std::string error_decoded_info = typed_decoded_signal_error->GetInfoMessage();
    EXPECT_EQ(error_decoded_info.compare(kResponseInfo_PR_error), 0);
}


TEST (CCMSignalsTest, EncodeDecodeBccFunc002Empty)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BasicCarControlSignal> signal = BasicCarControlSignal::CreateBasicCarControlSignalFromData
        (transaction_id, 1);

    signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_FALSE(buffer == nullptr);

    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("basic_car_control_signal_empty.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(BasicCarControlSignal::oid, BasicCarControlSignal::CreateBasicCarControlSignal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(BasicCarControlSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBasicCarControlSignal);

    std::shared_ptr<BasicCarControlSignal> typed_decoded_signal = std::static_pointer_cast<BasicCarControlSignal>(decoded_signal);

    //check car locator
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), signal->CarLocatorSupported());
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), false);

    //check address type
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), BasicCarControlSignal::AddressType::None);
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), signal->GetAddressType());

    //address getters should report false
    UUID data;
    std::string data2;
    ASSERT_EQ(typed_decoded_signal->GetAddressInstanceId(data), false);
    ASSERT_EQ(signal->GetAddressInstanceId(data), false);
    ASSERT_EQ(typed_decoded_signal->GetAddressUri(data2), false);
    ASSERT_EQ(signal->GetAddressUri(data2), false);
}

TEST (CCMSignalsTest, EncodeDecodeBccFunc002CarLocator)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BasicCarControlSignal> signal = BasicCarControlSignal::CreateBasicCarControlSignalFromData
        (transaction_id, 1);

    signal->SetCarLocatorSupport();

    signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_FALSE(buffer == nullptr);

    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("basic_car_control_signal_car_locator.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(BasicCarControlSignal::oid, BasicCarControlSignal::CreateBasicCarControlSignal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(BasicCarControlSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBasicCarControlSignal);

    std::shared_ptr<BasicCarControlSignal> typed_decoded_signal = std::static_pointer_cast<BasicCarControlSignal>(decoded_signal);

    //check car locator
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), signal->CarLocatorSupported());
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), true);

    //check address type
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), BasicCarControlSignal::AddressType::None);
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), signal->GetAddressType());
}

TEST (CCMSignalsTest, EncodeDecodeBccFunc002Uri)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BasicCarControlSignal> signal = BasicCarControlSignal::CreateBasicCarControlSignalFromData
        (transaction_id, 1);

    std::string uri = "setting some random text";

    signal->SetAdressUri(uri);

    signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_FALSE(buffer == nullptr);

    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("basic_car_control_signal_uri.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(BasicCarControlSignal::oid, BasicCarControlSignal::CreateBasicCarControlSignal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(BasicCarControlSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBasicCarControlSignal);

    std::shared_ptr<BasicCarControlSignal> typed_decoded_signal = std::static_pointer_cast<BasicCarControlSignal>(decoded_signal);

    //check car locator
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), signal->CarLocatorSupported());
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), false);

    //check address type
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), BasicCarControlSignal::AddressType::Uri);
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), signal->GetAddressType());


    std::string uri_comp;
    std::string uri_orig;

    //check return values on getters
    ASSERT_EQ(typed_decoded_signal->GetAddressUri(uri_comp), true);
    ASSERT_EQ(signal->GetAddressUri(uri_orig), true);

    //check address content
    ASSERT_EQ(uri.compare(uri_comp)==0, true);
    ASSERT_EQ(uri_orig.compare(uri_comp)==0, true);

    //make sure InstanceId getter is reported as false
    UUID data;
    ASSERT_EQ(typed_decoded_signal->GetAddressInstanceId(data), false);
    ASSERT_EQ(signal->GetAddressInstanceId(data), false);
}

TEST (CCMSignalsTest, EncodeDecodeBccFunc002InstanceId)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BasicCarControlSignal> signal = BasicCarControlSignal::CreateBasicCarControlSignalFromData
        (transaction_id, 1);

    UUID instanceId;

    for (int i=0;i< FS_UUID_LEN;i++) instanceId[i] = 2*i+3;

    signal->SetAdressInstanceId(instanceId);

    signal->SetRecipients(users);


    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_FALSE(buffer == nullptr);

    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("basic_car_control_signal_instanceId.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(BasicCarControlSignal::oid, BasicCarControlSignal::CreateBasicCarControlSignal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(BasicCarControlSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBasicCarControlSignal);

    std::shared_ptr<BasicCarControlSignal> typed_decoded_signal = std::static_pointer_cast<BasicCarControlSignal>(decoded_signal);

    //check car locator
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), signal->CarLocatorSupported());
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), false);

    //check address type
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), BasicCarControlSignal::AddressType::InstanceId);
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), signal->GetAddressType());

    //check address content

    UUID uuid_comp;
    UUID uuid_orig;
    //check return values on getters
    ASSERT_EQ(typed_decoded_signal->GetAddressInstanceId(uuid_comp), true);
    ASSERT_EQ(signal->GetAddressInstanceId(uuid_orig), true);

    //check thr content
    for (int i=0;i< FS_UUID_LEN;i++)
    {
      ASSERT_EQ(instanceId[i], uuid_comp[i]);
      ASSERT_EQ(uuid_orig[i], uuid_comp[i]);
    }

    //make sure URI getter is reported as false
    std::string data;
    ASSERT_EQ(typed_decoded_signal->GetAddressUri(data), false);
    ASSERT_EQ(signal->GetAddressUri(data), false);
}

TEST (CCMSignalsTest, EncodeDecodeBccFunc002Combined)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BasicCarControlSignal> signal = BasicCarControlSignal::CreateBasicCarControlSignalFromData
        (transaction_id, 1);

    std::string uri = "setting some yet another random text";
    signal->SetCarLocatorSupport();
    signal->SetAdressUri(uri);

    signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_FALSE(buffer == nullptr);

    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("basic_car_control_signal_combined.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(BasicCarControlSignal::oid, BasicCarControlSignal::CreateBasicCarControlSignal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();


    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(BasicCarControlSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBasicCarControlSignal);

    std::shared_ptr<BasicCarControlSignal> typed_decoded_signal = std::static_pointer_cast<BasicCarControlSignal>(decoded_signal);

    //check car locator
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), signal->CarLocatorSupported());
    ASSERT_EQ(typed_decoded_signal->CarLocatorSupported(), true);

    //check address type
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), BasicCarControlSignal::AddressType::Uri);
    ASSERT_EQ(typed_decoded_signal->GetAddressType(), signal->GetAddressType());

    //check address content
    std::string uri_comp;
    std::string uri_orig;
    typed_decoded_signal->GetAddressUri(uri_comp);
    signal->GetAddressUri(uri_orig);
    ASSERT_EQ(uri.compare(uri_comp)==0, true);
    ASSERT_EQ(uri_orig.compare(uri_comp)==0, true);
}

TEST (CCMSignalsTest, EncodeDecodeBCCCL001Empty)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<TestBccCl001Signal> signal = TestBccCl001Signal::CreateTestBccCl001Signal
        (transaction_id, 1);

    signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);
    ASSERT_TRUE(buffer == nullptr);
}

//generic setup for Advert tests
void TestAdvert(BccCl001Signal::Advert advertToTest, std::string filename)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<TestBccCl001Signal> signal = TestBccCl001Signal::CreateTestBccCl001Signal
        (transaction_id, 1);

    signal->SetAdvert(advertToTest);

    signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_FALSE(buffer == nullptr);

    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile(filename, (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(BccCl001Signal::oid, BccCl001Signal::CreateBccCl001Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(BccCl001Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl001Signal);

    std::shared_ptr<BccCl001Signal> typed_decoded_signal = std::static_pointer_cast<BccCl001Signal>(decoded_signal);

    //check advert
    ASSERT_EQ(typed_decoded_signal->GetSupportedAdvert() == advertToTest, true);
    ASSERT_EQ(typed_decoded_signal->GetSupportedAdvert(), signal->GetSupportedAdvert());
}

TEST (CCMSignalsTest, EncodeDecodeBCCCL001Honk)
{
    TestAdvert(BccCl001Signal::Honk, "bcc_cl_001_signal_honk.bin");
}

TEST (CCMSignalsTest, EncodeDecodeBCCCL001Flash)
{
    TestAdvert(BccCl001Signal::Flash, "bcc_cl_001_signal_flash.bin");
}

TEST (CCMSignalsTest, EncodeDecodeBCCCL001HonkAndFlash)
{
    TestAdvert(BccCl001Signal::HonkAndFlash, "bcc_cl_001_signal_honkandflash.bin");
}

//generic setup for Advert tests
void TestAdvertResponse(std::string filename, bool error, long code, bool msg=false)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl002Signal> signal = BccCl002Signal::CreateBccCl002Signal(transaction_id, 1);

    std::string message = "Here comes some message";

    signal->SetRecipients(users);

    if (error)
    {
        //for errors we need to take care of the special case RequestEntityTooLarge
        //for which the size shall be set
        int size = 0;
        if (code == fsm::ResponseInfoData::ResponseCodeError::kRequestEntityTooLarge)
        {
            size =34534534;
        }

        if (!msg)
        {
            signal->SetError((fsm::ResponseInfoData::ResponseCodeError)code,
                             std::string(),
                             size);
        }
        else
        {
            signal->SetError((fsm::ResponseInfoData::ResponseCodeError)code,
                             message,
                             size);
        }
    }
    else
    {
        if (!msg)
        {
            signal->SetSuccess((fsm::ResponseInfoData::ResponseCodeSuccess)code);
        }
        else
        {
            signal->SetSuccess((fsm::ResponseInfoData::ResponseCodeSuccess)code, message);
        }
    }

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_FALSE(buffer == nullptr);

    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile(filename.c_str(), (char*)(buffer->data()), buffer->size());

    fsm::CCMTransactionId transaction_id2;

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestBccCl002Signal::oid, TestBccCl002Signal::CreateTestBccCl002Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
    {
        FAIL();
    }

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl002Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl002Signal);

    std::shared_ptr<TestBccCl002Signal> typed_decoded_signal = std::static_pointer_cast<TestBccCl002Signal>(decoded_signal);

    //check signal state
    ASSERT_EQ(typed_decoded_signal->IsError(), error);
    ASSERT_NE(typed_decoded_signal->IsSuccess(), error);
    if (error)
    {
        ASSERT_EQ(typed_decoded_signal->GetErrorCode(), code);
    }
    else
    {
        ASSERT_EQ(typed_decoded_signal->GetSuccessCode(), code);
    }
    if (msg)
    {
        ASSERT_EQ(typed_decoded_signal->GetInfoMessage(), message);
    }
}


//generic setup for location response tests
//TODO: lots of duplication, implement ResponseInfo and then refactor into generic
//      response info test.
void TestLocationResponse(std::string filename, bool error, long code, bool msg=false)
{
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<BccCl011Signal> signal = BccCl011Signal::CreateBccCl011Signal(transaction_id, 1);

    std::string message = "Here comes some message";

    signal->SetRecipients(users);

    if (error)
    {
        //for errors we need to take care of the special case RequestEntityTooLarge
        //for which the size shall be set
        int size = 0;
        if (code == fsm::ResponseInfoData::ResponseCodeError::kRequestEntityTooLarge)
        {
            size = 34534534;
        }

        if (!msg)
        {
            signal->SetError((fsm::ResponseInfoData::ResponseCodeError)code,
                             std::string(),
                             size);
        }
        else
        {
            signal->SetError((fsm::ResponseInfoData::ResponseCodeError)code,
                             message,
                             size);
        }
    }
    else
    {
        if (!msg)
        {
            signal->SetSuccess((fsm::ResponseInfoData::ResponseCodeSuccess)code);
        }
        else
        {
            signal->SetSuccess((fsm::ResponseInfoData::ResponseCodeSuccess)code, message);
        }
    }

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);
    DumpMessageFile(filename.c_str(), (char*)(buffer->data()), buffer->size());

    fsm::CCMTransactionId transaction_id2;

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestBccCl011Signal::oid, TestBccCl011Signal::CreateTestBccCl011Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());


    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl011Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl011Signal);

    std::shared_ptr<TestBccCl011Signal> typed_decoded_signal = std::static_pointer_cast<TestBccCl011Signal>(decoded_signal);

    //check signal state
    ASSERT_EQ(typed_decoded_signal->IsError(), error);
    ASSERT_NE(typed_decoded_signal->IsSuccess(), error);
    if (error)
    {
        ASSERT_EQ(typed_decoded_signal->GetErrorCode(), code);
    }
    else
    {
        ASSERT_EQ(typed_decoded_signal->GetSuccessCode(), code);
    }
    if (msg)
    {
        ASSERT_EQ(typed_decoded_signal->GetInfoMessage(), message);
    }
}


const std::string kErrorNames[] = {"InvalidErrorCode",
                                   "BadRequest",
                                   "Unauthorized",
                                   "BadOption",
                                   "Forbidden",
                                   "NotFound",
                                   "MethodNotAllowed",
                                   "NotAcceptable",
                                   "PreconditionFailed",
                                   "RequestEntityTooLarge",
                                   "UnsupportedContentFormat",
                                   "InternalError",
                                   "BadGateway",
                                   "ServiceUnavailable",
                                   "GatewayTimeout",
                                   "ProxyingNotSupported",
                                   "NumberOfErrorCodes" };

const std::string kSuccessNames[] = { "InvalidSuccessCode",
                                      "Unspecified",
                                      "Created",
                                      "Deleted",
                                      "Valid",
                                      "Changed",
                                      "Content",
                                      "NumberOfSuccessCodes"};


TEST (CCMSignalsTest, EncodeDecodeBCCCL002ErrorCodes)
{
    for (long code = fsm::ResponseInfoData::ResponseCodeError::kBadRequest;
         code < fsm::ResponseInfoData::ResponseCodeError::kNumberOfErrorCodes;
         code++)
    {
      TestAdvertResponse("bcc_cl_002_signal_error_"+kErrorNames[code]+".bin", true, code);
    }
}

TEST (CCMSignalsTest, EncodeDecodeBCCCL002ErrorCodesWithMessage)
{
    for (long code = fsm::ResponseInfoData::ResponseCodeError::kBadRequest;
         code < fsm::ResponseInfoData::ResponseCodeError::kNumberOfErrorCodes;
         code++)
    {
       TestAdvertResponse("bcc_cl_002_signal_error_"+kErrorNames[code]+"_msg.bin", true, code, true);
    }
}

TEST (CCMSignalsTest, EncodeDecodeBCCCL002SuccessCodes)
{
    for (long code = fsm::ResponseInfoData::ResponseCodeSuccess::kCreated;
         code < fsm::ResponseInfoData::ResponseCodeSuccess::kNumberOfSuccessCodes;
         code++)
    {
       TestAdvertResponse("bcc_cl_002_signal_success_"+kSuccessNames[code]+".bin", false, code);
    }
}

TEST (CCMSignalsTest, EncodeDecodeBCCCL002SuccessCodesWithMessage)
{
    for (long code = fsm::ResponseInfoData::ResponseCodeSuccess::kCreated;
         code < fsm::ResponseInfoData::ResponseCodeSuccess::kNumberOfSuccessCodes;
         code++)
    {
       TestAdvertResponse("bcc_cl_002_signal_success_"+kSuccessNames[code]+"_msg.bin", false, code, true);
    }
}

TEST (CCMSignalsTest, EncodeDecodeBCCCL002IncorrectInput)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl002Signal> signal = BccCl002Signal::CreateBccCl002Signal(transaction_id, 1);

    signal->SetRecipients(users);

    //setting codes outside the scope should fail
    EXPECT_EQ(signal->SetError(fsm::ResponseInfoData::ResponseCodeError::kInvalidErrorCode) == false, true);
    EXPECT_EQ(signal->SetError(fsm::ResponseInfoData::ResponseCodeError::kNumberOfErrorCodes) == false, true);
    EXPECT_EQ(signal->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kInvalidSuccessCode) == false, true);
    EXPECT_EQ(signal->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kNumberOfSuccessCodes) == false, true);

    //missing parameter test
    EXPECT_EQ(signal->SetError(fsm::ResponseInfoData::ResponseCodeError::kRequestEntityTooLarge) == false, true); //missing size parameter

    //make sure that encode fails after incorrectly setting the state
    //Please note that if any of previous checks in this test fails then
    //the encoding test's result is not relevant

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_TRUE(buffer == nullptr);

}

TEST (CCMSignalsTest, EncodeDecodeBCCCL002EncodingErrorValueNotSet)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl002Signal> signal = BccCl002Signal::CreateBccCl002Signal(transaction_id, 1);

    signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_TRUE(buffer == nullptr);
}

/**
 * \brief Test that all setters behave as expected.
 */
TEST (CCMSignalsTest, TestSettersLocationData)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl020Signal> bcc_cl_020 =
        BccCl020Signal::CreateBccCl020Signal(transaction_id, 1);

    bcc_cl_020->SetRecipients(users);

    EXPECT_TRUE(bcc_cl_020->SetLatitude(45.0));
    EXPECT_TRUE(bcc_cl_020->SetLatitude(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetLatitude(-100.0));
    EXPECT_FALSE(bcc_cl_020->SetLatitude(200.0));

    EXPECT_TRUE(bcc_cl_020->SetLongitude(145.0));
    EXPECT_TRUE(bcc_cl_020->SetLongitude(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetLongitude(-200.0));
    EXPECT_FALSE(bcc_cl_020->SetLongitude(400.0));

    EXPECT_TRUE(bcc_cl_020->SetHeading(13));
    EXPECT_TRUE(bcc_cl_020->SetHeading(fsm::LocationData::kDefaultIntValue));
    EXPECT_FALSE(bcc_cl_020->SetHeading(-75));
    EXPECT_FALSE(bcc_cl_020->SetHeading(1000));

    EXPECT_TRUE(bcc_cl_020->SetLatitudeDelta1(0.01));
    EXPECT_TRUE(bcc_cl_020->SetLatitudeDelta1(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetLatitudeDelta1(-0.015));
    EXPECT_FALSE(bcc_cl_020->SetLatitudeDelta1(0.015));

    EXPECT_TRUE(bcc_cl_020->SetLatitudeDelta2(0.01));
    EXPECT_TRUE(bcc_cl_020->SetLatitudeDelta2(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetLatitudeDelta2(-0.015));
    EXPECT_FALSE(bcc_cl_020->SetLatitudeDelta2(0.015));

    EXPECT_TRUE(bcc_cl_020->SetLongitudeDelta1(0.01));
    EXPECT_TRUE(bcc_cl_020->SetLongitudeDelta1(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetLongitudeDelta1(-0.015));
    EXPECT_FALSE(bcc_cl_020->SetLongitudeDelta1(0.015));

    EXPECT_TRUE(bcc_cl_020->SetLongitudeDelta2(0.01));
    EXPECT_TRUE(bcc_cl_020->SetLongitudeDelta2(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetLongitudeDelta2(-0.015));
    EXPECT_FALSE(bcc_cl_020->SetLongitudeDelta2(0.015));

    EXPECT_TRUE(bcc_cl_020->SetSpeed(130));
    EXPECT_TRUE(bcc_cl_020->SetSpeed(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetSpeed(-10000.0));
    EXPECT_FALSE(bcc_cl_020->SetSpeed(250.0));

    EXPECT_TRUE(bcc_cl_020->SetClimb(0));
    EXPECT_TRUE(bcc_cl_020->SetClimb(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetClimb(-200.0));
    EXPECT_FALSE(bcc_cl_020->SetClimb(200.0));

    EXPECT_TRUE(bcc_cl_020->SetRollRate(1.0));
    EXPECT_TRUE(bcc_cl_020->SetRollRate(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetRollRate(-101));
    EXPECT_FALSE(bcc_cl_020->SetRollRate(101.0));

    EXPECT_TRUE(bcc_cl_020->SetPitchRate(2.0));
    EXPECT_TRUE(bcc_cl_020->SetPitchRate(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetPitchRate(-101.0));
    EXPECT_FALSE(bcc_cl_020->SetPitchRate(101.0));

    EXPECT_TRUE(bcc_cl_020->SetYawRate(3.0));
    EXPECT_TRUE(bcc_cl_020->SetYawRate(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(bcc_cl_020->SetYawRate(-101.0));
    EXPECT_FALSE(bcc_cl_020->SetYawRate(101.0));

    EXPECT_TRUE(bcc_cl_020->SetGNSSFixStatus(
        fsm::LocationData::GnssFixStatus::GNSS_FIX_STATUS_UNDEFINED));
    EXPECT_TRUE(bcc_cl_020->SetGNSSFixStatus(
        fsm::LocationData::GnssFixStatus::GNSS_FIX_STATUS_NO_FIX));
    EXPECT_TRUE(bcc_cl_020->SetGNSSFixStatus(
        fsm::LocationData::GnssFixStatus::GNSS_FIX_STATUS_TIME_FIX));
    EXPECT_TRUE(bcc_cl_020->SetGNSSFixStatus(
        fsm::LocationData::GnssFixStatus::GNSS_FIX_STATUS_TWO_D_FIX));
    EXPECT_TRUE(bcc_cl_020->SetGNSSFixStatus(
        fsm::LocationData::GnssFixStatus::GNSS_FIX_STATUS_THREE_D_FIX));
    EXPECT_FALSE(bcc_cl_020->SetGNSSFixStatus(
        static_cast<fsm::LocationData::GnssFixStatus>(1000)));

    EXPECT_TRUE(bcc_cl_020->SetUsedSatelliteSystem(
        fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_UNDEFINED));
    EXPECT_TRUE(bcc_cl_020->SetUsedSatelliteSystem(
        fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_GPS));
    EXPECT_TRUE(bcc_cl_020->SetUsedSatelliteSystem(
        fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_GLONASS));
    EXPECT_TRUE(bcc_cl_020->SetUsedSatelliteSystem(
        fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_GALILEO));
    EXPECT_TRUE(bcc_cl_020->SetUsedSatelliteSystem(
        fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_BEIDOU));
    EXPECT_FALSE(bcc_cl_020->SetUsedSatelliteSystem(
        static_cast<fsm::LocationData::SatelliteSystems>(1000)));

    struct tm time_stamp = {};
    time_stamp.tm_year = 27;
    time_stamp.tm_mon = 3;
    time_stamp.tm_mday = 14;

    // returns void, test we dont crash?
    bcc_cl_020->SetPositionCanBeTrusted(true);
    bcc_cl_020->SetPositionCanBeTrusted(false);
    bcc_cl_020->SetTimeStamp(time_stamp);
    bcc_cl_020->SetAltitude(8848.0);
    bcc_cl_020->SetPDop(0.0);
    bcc_cl_020->SetHDop(0.0);
    bcc_cl_020->SetVDop(0.0);
    bcc_cl_020->SetUsedSatellites(5);
    bcc_cl_020->SetTrackedSatellites(5);
    bcc_cl_020->SetVisibleSatellites(5);
    bcc_cl_020->SetSigmaHPosition(0.0);
    bcc_cl_020->SetSigmaAltitude(-0.1);
    bcc_cl_020->SetSigmaSpeed(0.2);
    bcc_cl_020->SetSigmaClimb(-0.4);
    bcc_cl_020->SetSigmaPressure(0.8);
    bcc_cl_020->SetSigmaHeading(-16);
    bcc_cl_020->SetDrStatus(false);
    bcc_cl_020->SetPressure(15750.0);
}

TEST(CCMSignalsTest, TestDeadReckonedSetterBccCl020)
{
    // Position
    const int32_t latitude = 1073741824;  // 90/360*2^32
    const int32_t longitude = -2147483648;  // (-180)/360*2^32
    const CoordinatesLongLat longlat = {longitude, latitude};
    // Speed
    const uint32_t speedKmph = 100;
    const uint32_t heading = 180;
    // Satellite info
    const uint32_t hdopX10 = 100;
    const uint32_t numSat = 20;
    const uint32_t drDistance = 500;
    const DateTime fixTime = {2017, 4, 3, 13, 51, 31};
    const DeadReckonedType drType = DeadReckonedType::drMapMatched;
    const GnssFixType fixType = GnssFixType::fix2D;

    DRPositionData data { longlat,
                          heading,
                          speedKmph,
                          hdopX10,
                          numSat,
                          fixTime,
                          fixType,
                          drType,
                          drDistance
                        };

    fsm::CCMTransactionId transaction_id;
    fsm::LocationData location_data_actual;

    location_data_actual.SetFromDRPositionData(data);

    // Initialize a fsm::LocationData object with expected values to compare against
    fsm::LocationData location_data_expected;
    location_data_expected.SetLatitude(90);
    location_data_expected.SetLongitude(-180);
    location_data_expected.SetHeading(heading);
    location_data_expected.SetSpeed(speedKmph * 1000/3600);  // Convert to m/s
    location_data_expected.SetUsedSatellites(numSat);
    location_data_expected.SetGNSSFixStatus(fsm::LocationData::GnssFixStatus::GNSS_FIX_STATUS_TWO_D_FIX);
    location_data_expected.SetDrStatus((drType == DeadReckonedType::drMapMatched) ? true : false);
    location_data_expected.SetHDop(hdopX10);
    // Date and time
    struct tm new_value = {
        static_cast<int>(fixTime.second),
        static_cast<int>(fixTime.minute),
        static_cast<int>(fixTime.hour),
        static_cast<int>(fixTime.day),
        static_cast<int>(fixTime.month-1),
        static_cast<int>(fixTime.year-1900),
        0,  // Not used
        0,  // Not used
        0   // Not used
    };
    location_data_expected.SetTimeStamp(new_value);

    EXPECT_TRUE(location_data_actual == location_data_expected);
}

TEST(CCMSignalsTest, TestGNSSDataSetterBccCl020)
{
    // Position
    const int32_t latitude = 1073741824;  // 90/360*2^32
    const int32_t longitude = -2147483648;  // (-180)/360*2^32
    const int32_t altitude = 100;  // In 1/10 m
    const CoordinatesLongLat longlat = {longitude, latitude};
    const GeographicalPosition position = {longlat, altitude};
    // Speed
    const int32_t speed = 30000;  // In 1/1000 m/s
    const int32_t horizontalVelocity = 100;
    const int32_t verticalVelocity = 3;
    const Velocity movement = {speed, horizontalVelocity, verticalVelocity};
    const uint32_t heading = 18000;  // In 1/100 degrees
    // Satellite info
    const NrOfSatellitesPerSystem nrOfSatellitesVisible = {10, 5, 10, 5, 10, 5};
    const NrOfSatellitesPerSystem nrOfSatellitesUsed = {6, 0, 3, 0, 0, 0};
    const SatelliteUsage satelliteInfo = {nrOfSatellitesVisible, nrOfSatellitesUsed};
    const GNSSUsage gnssStatus = {true, false, true, false, false, false};  // GPS and GALILEO
    const GnssFixType fixType = GnssFixType::fix2D;
    const GNSSStatus positioningStatus = {fixType, false, false};
    const DOPValues precision = {1,2,3,4};  // hdop, vdop, pdop, tdop
    const ReceiverChannelData receiverChannels = {0, 0, 0, 0};  // not used
    const DateTime utcTime = {2017, 4, 3, 13, 51, 31};
    const GPSSystemTime gpsTime = {0, 0};  // not used

    fsm::CCMTransactionId transaction_id;

    fsm::LocationData location_data_actual;

    GNSSData data { utcTime,
                    gpsTime,
                    position,
                    movement,
                    heading,
                    gnssStatus,
                    positioningStatus,
                    satelliteInfo,
                    precision,
                    receiverChannels
                  };

    location_data_actual.SetFromGNSSData(data);

    // Initialize a fsm::LocationData object with expected values to compare against
    fsm::LocationData location_data_expected;
    location_data_expected.SetLatitude(90);
    location_data_expected.SetLongitude(-180);
    location_data_expected.SetAltitude(altitude/10);  // Convert to m
    location_data_expected.SetHeading(heading/100);  // Convert to degrees
    location_data_expected.SetSpeed(speed/1000);  // Convert from 1/1000 m/s to m/s
    location_data_expected.SetUsedSatelliteSystem(fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_GPS);
    location_data_expected.SetUsedSatelliteSystem(fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_GALILEO);
    location_data_expected.SetUsedSatellites(6+3);  // accordingly to nrOfSatellitesUsed
    location_data_expected.SetVisibleSatellites(10+10);  // only GPS and GALILEO from nrOfSatellitesVisible
    location_data_expected.SetGNSSFixStatus(fsm::LocationData::GnssFixStatus::GNSS_FIX_STATUS_TWO_D_FIX);
    location_data_expected.SetDrStatus(false);
    location_data_expected.SetHDop(1);  // accordingly to precision
    location_data_expected.SetPDop(3);  // accordingly to precision
    // Date and time
    struct tm new_value = {
        static_cast<int>(utcTime.second),
        static_cast<int>(utcTime.minute),
        static_cast<int>(utcTime.hour),
        static_cast<int>(utcTime.day),
        static_cast<int>(utcTime.month-1),
        static_cast<int>(utcTime.year-1900),
        0,  // Not used
        0,  // Not used
        0   // Not used
    };
    location_data_expected.SetTimeStamp(new_value);

    EXPECT_TRUE(location_data_actual == location_data_expected);
}

/**
 * \brief Test basic operation of == operator
 */
TEST (CCMSignalsTest, TestEqualityOperatorLocationData)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl020Signal> bcc_cl_020_1 =
        BccCl020Signal::CreateBccCl020Signal(transaction_id, 1);

    std::shared_ptr<BccCl020Signal> bcc_cl_020_2 =
        BccCl020Signal::CreateBccCl020Signal(transaction_id, 1);


    // First test before setting any fields.

    EXPECT_TRUE(*bcc_cl_020_1 == *bcc_cl_020_2);

    // Now test with seiiting numeric field
    // comparisonj of int, doubloe, boolean and enums
    // all works teh same, so this will serve as test
    // for all such.

    EXPECT_TRUE(bcc_cl_020_1->SetLatitude(45.0));
    EXPECT_TRUE(bcc_cl_020_2->SetLatitude(45.0));

    EXPECT_TRUE(*bcc_cl_020_1 == *bcc_cl_020_2);

    EXPECT_TRUE(bcc_cl_020_2->SetLatitude(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_FALSE(*bcc_cl_020_1 == *bcc_cl_020_2);

    EXPECT_TRUE(bcc_cl_020_1->SetLatitude(fsm::LocationData::kDefaultDoubleValue));
    EXPECT_TRUE(*bcc_cl_020_1 == *bcc_cl_020_2);

    // Finally test with set data_time_stamp,
    // as that behaves in a special way.

    struct tm time_stamp_1 = {};
    time_stamp_1.tm_year = 27;
    time_stamp_1.tm_mon = 3;
    time_stamp_1.tm_mday = 14;

    struct tm time_stamp_2 = {};
    time_stamp_2.tm_year = 117;
    time_stamp_2.tm_mon = 2;
    time_stamp_2.tm_mday = 8;

    bcc_cl_020_1->SetTimeStamp(time_stamp_1);
    EXPECT_FALSE(*bcc_cl_020_1 == *bcc_cl_020_2);

    bcc_cl_020_2->SetTimeStamp(time_stamp_2);
    EXPECT_FALSE(*bcc_cl_020_1 == *bcc_cl_020_2);

    bcc_cl_020_2->SetTimeStamp(time_stamp_1);
    EXPECT_TRUE(*bcc_cl_020_1 == *bcc_cl_020_2);
}

/**
 * \brief Test encode/decode BccCl010.
 */
TEST (CCMSignalsTest, TestEncodeDecodeBccCl010)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<TestBccCl010Signal> bcc_cl_010 =
        TestBccCl010Signal::CreateTestBccCl010Signal(transaction_id, 1);

    // set a session id
    bcc_cl_010->SetSessionId("abcdabcdabcdabcd");

    bcc_cl_010->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(bcc_cl_010);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("bcc_cl_010.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(BccCl010Signal::oid, BccCl010Signal::CreateBccCl010Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(BccCl010Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl010Signal);

    std::string session_id;
    ASSERT_TRUE(bcc_cl_010->GetSessionId(session_id));
    ASSERT_EQ(session_id.compare("abcdabcdabcdabcd"), 0);

}

/**
 * \brief Test encode/decode BCC-CL-011 error response info.
 */
TEST (CCMSignalsTest, EncodeDecodeBCCCL011ErrorCodes)
{
    for (long code = fsm::ResponseInfoData::ResponseCodeError::kBadRequest;
         code < fsm::ResponseInfoData::ResponseCodeError::kNumberOfErrorCodes;
         code++)
    {
      TestLocationResponse("bcc_cl_011_signal_error_"+kErrorNames[code]+".bin", true, code);
    }
}

/**
 * \brief Test encode/decode BCC-CL-011 error response info with extra data.
 */
TEST (CCMSignalsTest, EncodeDecodeBCCCL011ErrorCodesWithMessage)
{
    for (long code = fsm::ResponseInfoData::ResponseCodeError::kBadRequest;
         code < fsm::ResponseInfoData::ResponseCodeError::kNumberOfErrorCodes;
         code++)
    {
       TestLocationResponse("bcc_cl_011_signal_error_"+kErrorNames[code]+"_msg.bin", true, code, true);
    }
}

/**
 * \brief Test encode/decode BCC-CL-011 success response info.
 */
TEST (CCMSignalsTest, EncodeDecodeBCCCL011SuccessCodes)
{
    for (long code = fsm::ResponseInfoData::ResponseCodeSuccess::kCreated;
         code < fsm::ResponseInfoData::ResponseCodeSuccess::kNumberOfSuccessCodes;
         code++)
    {
       TestLocationResponse("bcc_cl_011_signal_success_"+kSuccessNames[code]+".bin", false, code);
    }
}

/**
 * \brief Test encode/decode BCC-CL-011 success response info with extra data.
 */
TEST (CCMSignalsTest, EncodeDecodeBCCCL011SuccessCodesWithMessage)
{
    for (long code = fsm::ResponseInfoData::ResponseCodeSuccess::kCreated;
         code < fsm::ResponseInfoData::ResponseCodeSuccess::kNumberOfSuccessCodes;
         code++)
    {
       TestLocationResponse("bcc_cl_011_signal_success_"+kSuccessNames[code]+"_msg.bin", false, code, true);
    }
}

/**
 * \brief Test encode invalid BccCl011.
 */
TEST (CCMSignalsTest, TestEncodeInvalidBccCl011)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl011Signal> bcc_cl_011 = BccCl011Signal::CreateBccCl011Signal(transaction_id,
                                                                                      1);

    bcc_cl_011->SetRecipients(users);

    // first try with pristine signal should fail as response info
    // is mandatory
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(bcc_cl_011);
    ASSERT_TRUE(buffer == nullptr);

    // now try setting invalid response info
    EXPECT_FALSE(bcc_cl_011->SetError(fsm::ResponseInfoData::ResponseCodeError::kInvalidErrorCode));
    EXPECT_FALSE(bcc_cl_011->SetError(fsm::ResponseInfoData::ResponseCodeError::kNumberOfErrorCodes));
    EXPECT_FALSE(bcc_cl_011->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kInvalidSuccessCode));
    EXPECT_FALSE(bcc_cl_011->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kNumberOfSuccessCodes));
    EXPECT_FALSE(bcc_cl_011->SetError(fsm::ResponseInfoData::ResponseCodeError::kRequestEntityTooLarge)); //missing size parameter

    // verify encode still fails
    buffer = fsm::SignalFactory::Encode(bcc_cl_011);
    ASSERT_TRUE(buffer == nullptr);
}

/**
 * \brief Test encode/decode valid BccCl011 with invalid position data.
 */
TEST (CCMSignalsTest, TestEncodeDecodeValidResponseInvalidLocationBccCl011)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl011Signal> bcc_cl_011 =
        BccCl011Signal::CreateBccCl011Signal(transaction_id, 1);

    bcc_cl_011->SetRecipients(users);

    // set valid response info
    bcc_cl_011->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);


    // set a incomplete data set
    bcc_cl_011->SetPositionCanBeTrusted(true);
    EXPECT_TRUE(bcc_cl_011->SetLongitude(145.0));

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(bcc_cl_011);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("bcc_cl_011_minimum_location_no_optional_data.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestBccCl011Signal::oid, TestBccCl011Signal::CreateTestBccCl011Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl011Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl011Signal);

    std::shared_ptr<TestBccCl011Signal> typed_decoded_signal =
        std::static_pointer_cast<TestBccCl011Signal>(decoded_signal);

    EXPECT_FALSE(typed_decoded_signal->IsError());
    EXPECT_EQ(typed_decoded_signal->GetSuccessCode(),
              fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);

    //partially set data should have been omitted during encode
    EXPECT_FALSE(typed_decoded_signal->CompareLocationData(bcc_cl_011));

}

/**
 * \brief Test encode/decode minimum location data BccCl011.
 */
TEST (CCMSignalsTest, TestEncodeDecodeMinimumBccCl011)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl011Signal> bcc_cl_011 =
        BccCl011Signal::CreateBccCl011Signal(transaction_id, 1);

    bcc_cl_011->SetRecipients(users);

    // set valid response info
    bcc_cl_011->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);


    // first try minimum valid data set
    bcc_cl_011->SetPositionCanBeTrusted(true);
    EXPECT_TRUE(bcc_cl_011->SetLatitude(45.0));
    EXPECT_TRUE(bcc_cl_011->SetLongitude(145.0));
    EXPECT_TRUE(bcc_cl_011->SetHeading(13));

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(bcc_cl_011);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("bcc_cl_011_minimum_location_no_optional_data.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestBccCl011Signal::oid, TestBccCl011Signal::CreateTestBccCl011Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl011Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl011Signal);

    std::shared_ptr<TestBccCl011Signal> typed_decoded_signal =
        std::static_pointer_cast<TestBccCl011Signal>(decoded_signal);

    EXPECT_FALSE(typed_decoded_signal->IsError());
    EXPECT_EQ(typed_decoded_signal->GetSuccessCode(),
              fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);

    EXPECT_TRUE(typed_decoded_signal->CompareLocationData(bcc_cl_011));

    // now also try with optional data in minimum set
    EXPECT_TRUE(bcc_cl_011->SetLatitudeDelta1(0.001));
    EXPECT_TRUE(bcc_cl_011->SetLatitudeDelta2(0.002));
    EXPECT_TRUE(bcc_cl_011->SetLongitudeDelta1(0.003));
    EXPECT_TRUE(bcc_cl_011->SetLongitudeDelta2(0.004));

    fsm::SetCertsForEncode();

    buffer = fsm::SignalFactory::Encode(bcc_cl_011);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("bcc_cl_011_minimum_location_all_optional_data.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestBccCl011Signal::oid, TestBccCl011Signal::CreateTestBccCl011Signal);
    decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl011Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl011Signal);

    typed_decoded_signal = std::static_pointer_cast<TestBccCl011Signal>(decoded_signal);

    EXPECT_FALSE(typed_decoded_signal->IsError());
    EXPECT_EQ(typed_decoded_signal->GetSuccessCode(),
              fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);

    EXPECT_TRUE(typed_decoded_signal->CompareLocationData(bcc_cl_011));

}

/**
 * \brief Test encode extended location data BccCl011.
 */
TEST (CCMSignalsTest, TestEncodeExtendedBccCl011)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl011Signal> bcc_cl_011 =
        BccCl011Signal::CreateBccCl011Signal(transaction_id, 1);

    bcc_cl_011->SetRecipients(users);

    // set valid response info
    bcc_cl_011->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);

    // first try minimum valid data set
    bcc_cl_011->SetPositionCanBeTrusted(true);
    EXPECT_TRUE(bcc_cl_011->SetLatitude(45.0));
    EXPECT_TRUE(bcc_cl_011->SetLongitude(145.0));

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(bcc_cl_011);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("bcc_cl_011_extended_location_no_optional_data.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestBccCl011Signal::oid, TestBccCl011Signal::CreateTestBccCl011Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl011Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl011Signal);

    std::shared_ptr<TestBccCl011Signal> typed_decoded_signal =
        std::static_pointer_cast<TestBccCl011Signal>(decoded_signal);

    EXPECT_FALSE(typed_decoded_signal->IsError());
    EXPECT_EQ(typed_decoded_signal->GetSuccessCode(),
              fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);
    EXPECT_TRUE(typed_decoded_signal->CompareLocationData(bcc_cl_011));

    // now also try with optional data in extended set
    // test covers every data type the signal holds
    struct tm time_stamp = {};
    time_stamp.tm_year = 27;
    time_stamp.tm_mon = 3;
    time_stamp.tm_mday =14 ;

    EXPECT_TRUE(bcc_cl_011->SetSpeed(49.9));
    bcc_cl_011->SetUsedSatellites(9);
    bcc_cl_011->SetPositionCanBeTrusted(true);
    bcc_cl_011->SetTimeStamp(time_stamp);
    EXPECT_TRUE(bcc_cl_011->SetGNSSFixStatus(
        fsm::LocationData::GnssFixStatus::GNSS_FIX_STATUS_TWO_D_FIX));
    EXPECT_TRUE(bcc_cl_011->SetUsedSatelliteSystem(
        fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_GPS));
    EXPECT_TRUE(bcc_cl_011->SetUsedSatelliteSystem(
        fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_GALILEO));

    fsm::SetCertsForEncode();

    buffer = fsm::SignalFactory::Encode(bcc_cl_011);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("bcc_cl_011_extended_location_some_optional_data.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl011Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl011Signal);

    typed_decoded_signal = std::static_pointer_cast<TestBccCl011Signal>(decoded_signal);

    EXPECT_FALSE(typed_decoded_signal->IsError());
    EXPECT_EQ(typed_decoded_signal->GetSuccessCode(),
              fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);

    EXPECT_TRUE(typed_decoded_signal->CompareLocationData(bcc_cl_011));

    // Finally try to also append some minimumLocation only data.
    // These should not become part of the encoded message.

    EXPECT_TRUE(bcc_cl_011->SetLongitudeDelta1(0.001));
    EXPECT_TRUE(bcc_cl_011->SetLatitudeDelta1(0.002));

    fsm::SetCertsForEncode();

    buffer = fsm::SignalFactory::Encode(bcc_cl_011);
    EXPECT_GT(buffer->size(), 0);
    ASSERT_FALSE(buffer == nullptr);

    fsm::SetCertsForDecode();

    decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl011Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl011Signal);

    EXPECT_FALSE(typed_decoded_signal->IsError());
    EXPECT_EQ(typed_decoded_signal->GetSuccessCode(),
              fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);
    typed_decoded_signal = std::static_pointer_cast<TestBccCl011Signal>(decoded_signal);

    EXPECT_FALSE(typed_decoded_signal->CompareLocationData(bcc_cl_011));

}


/**
 * \brief Test encode invalid BccCl020.
 */
TEST (CCMSignalsTest, TestEncodeInvalidBccCl020)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl020Signal> bcc_cl_020 = BccCl020Signal::CreateBccCl020Signal(transaction_id,
                                                                                      1);

    bcc_cl_020->SetRecipients(users);

    // first try with pristine signal

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(bcc_cl_020);
    ASSERT_TRUE(buffer == nullptr);

    // now try to set only partial data
    bcc_cl_020->SetPositionCanBeTrusted(true);
    EXPECT_TRUE(bcc_cl_020->SetLatitude(45.0));

    buffer = fsm::SignalFactory::Encode(bcc_cl_020);
    ASSERT_TRUE(buffer == nullptr);

    // now try to set some extended only data
    bcc_cl_020->SetHDop(45.0);
    bcc_cl_020->SetUsedSatellites(2);

    buffer = fsm::SignalFactory::Encode(bcc_cl_020);
    ASSERT_TRUE(buffer == nullptr);

    // now also try to set invalid data
    EXPECT_FALSE(bcc_cl_020->SetLongitude(300.0));
    EXPECT_FALSE(bcc_cl_020->SetHeading(-45));

    buffer = fsm::SignalFactory::Encode(bcc_cl_020);
    ASSERT_TRUE(buffer == nullptr);
}

/**
 * \brief Test encode/decode minimum location data BccCl020.
 */
TEST (CCMSignalsTest, TestEncodeDecodeMinimumBccCl020)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl020Signal> bcc_cl_020 = BccCl020Signal::CreateBccCl020Signal(transaction_id,
                                                                                      1);

    bcc_cl_020->SetRecipients(users);

    // first try minimum valid data set
    bcc_cl_020->SetPositionCanBeTrusted(true);
    EXPECT_TRUE(bcc_cl_020->SetLatitude(45.0));
    EXPECT_TRUE(bcc_cl_020->SetLongitude(145.0));
    EXPECT_TRUE(bcc_cl_020->SetHeading(13));

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(bcc_cl_020);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("bcc_cl_020_minimum_location_no_optional_data.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestBccCl020Signal::oid, TestBccCl020Signal::CreateTestBccCl020Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl020Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl020Signal);

    std::shared_ptr<TestBccCl020Signal> typed_decoded_signal =
        std::static_pointer_cast<TestBccCl020Signal>(decoded_signal);

    EXPECT_TRUE(typed_decoded_signal->CompareLocationData(bcc_cl_020));
    EXPECT_EQ(typed_decoded_signal->GetStoreAndForwardOptions(), CCM_STORE_AND_FORWARD_LATEST);


    // now also try with optional data in minimum set
    EXPECT_TRUE(bcc_cl_020->SetLatitudeDelta1(0.001));
    EXPECT_TRUE(bcc_cl_020->SetLatitudeDelta2(0.002));
    EXPECT_TRUE(bcc_cl_020->SetLongitudeDelta1(0.003));
    EXPECT_TRUE(bcc_cl_020->SetLongitudeDelta2(0.004));

    fsm::SetCertsForEncode();

    buffer = fsm::SignalFactory::Encode(bcc_cl_020);
    ASSERT_GT(buffer->size(), 0);
    ASSERT_FALSE(buffer == nullptr);

    DumpMessageFile("bcc_cl_020_minimum_location_all_optional_data.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl020Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl020Signal);

    typed_decoded_signal = std::static_pointer_cast<TestBccCl020Signal>(decoded_signal);

    EXPECT_TRUE(typed_decoded_signal->CompareLocationData(bcc_cl_020));

}

/**
 * \brief Test encode extended location data BccCl020.
 */
TEST (CCMSignalsTest, TestEncodeExtendedBccCl020)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BccCl020Signal> bcc_cl_020 = BccCl020Signal::CreateBccCl020Signal(transaction_id,
                                                                                      1);

    bcc_cl_020->SetRecipients(users);

    // first try minimum valid data set
    bcc_cl_020->SetPositionCanBeTrusted(true);
    EXPECT_TRUE(bcc_cl_020->SetLatitude(45.0));
    EXPECT_TRUE(bcc_cl_020->SetLongitude(145.0));

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(bcc_cl_020);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("bcc_cl_020_extended_location_no_optional_data.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestBccCl020Signal::oid, TestBccCl020Signal::CreateTestBccCl020Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl020Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl020Signal);

    std::shared_ptr<TestBccCl020Signal> typed_decoded_signal =
        std::static_pointer_cast<TestBccCl020Signal>(decoded_signal);

    EXPECT_TRUE(typed_decoded_signal->CompareLocationData(bcc_cl_020));

    // now also try with optional data in extended set
    // test covers every data type the signal holds
    struct tm time_stamp = {};
    time_stamp.tm_year = 27;
    time_stamp.tm_mon = 3;
    time_stamp.tm_mday =14 ;

    EXPECT_TRUE(bcc_cl_020->SetSpeed(49.9));
    bcc_cl_020->SetUsedSatellites(9);
    bcc_cl_020->SetPositionCanBeTrusted(true);
    bcc_cl_020->SetTimeStamp(time_stamp);
    EXPECT_TRUE(bcc_cl_020->SetGNSSFixStatus(
        fsm::LocationData::GnssFixStatus::GNSS_FIX_STATUS_TWO_D_FIX));
    EXPECT_TRUE(bcc_cl_020->SetUsedSatelliteSystem(
        fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_GPS));
    EXPECT_TRUE(bcc_cl_020->SetUsedSatelliteSystem(
        fsm::LocationData::SatelliteSystems::SATELLITE_SYSTEMS_GALILEO));

    fsm::SetCertsForEncode();

    buffer = fsm::SignalFactory::Encode(bcc_cl_020);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("bcc_cl_020_extended_location_some_optional_data.bin", (char*)(buffer->data()), buffer->size());

    fsm::SetCertsForDecode();

    decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl020Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl020Signal);

    typed_decoded_signal = std::static_pointer_cast<TestBccCl020Signal>(decoded_signal);

    EXPECT_TRUE(typed_decoded_signal->CompareLocationData(bcc_cl_020));

    // Finally try to also append some minimumLocation only data.
    // These should not become part of the encoded message.

    EXPECT_TRUE(bcc_cl_020->SetLongitudeDelta1(0.001));
    EXPECT_TRUE(bcc_cl_020->SetLatitudeDelta1(0.002));

    fsm::SetCertsForEncode();

    buffer = fsm::SignalFactory::Encode(bcc_cl_020);
    EXPECT_GT(buffer->size(), 0);
    ASSERT_FALSE(buffer == nullptr);

    fsm::SetCertsForDecode();

    decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestBccCl020Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kBccCl020Signal);

    typed_decoded_signal = std::static_pointer_cast<TestBccCl020Signal>(decoded_signal);

    EXPECT_FALSE(typed_decoded_signal->CompareLocationData(bcc_cl_020));

}


void TestCaCat001(std::vector<X509*> input_certs, std::string filename = std::string())
{
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaCat001Signal> signal = CaCat001Signal::CreateCaCat001Signal(
        transaction_id, 1);

    //add certs to the signal
    for (int i=0; i< input_certs.size(); i++ )
    {
       EXPECT_TRUE(signal->AddCertificate(input_certs[i]));
    }

    signal->SetRecipients(users);

    //encode signal
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    //if filename specified, dump signal contents
    if (filename.size()>0)
    {
       DumpMessageFile(filename, (char*)(buffer->data()), buffer->size());
    }

    //decode the signal
    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestCaCat001Signal::oid, TestCaCat001Signal::CreateTestCaCat001Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
    {
        FAIL();
    }

    //check signal type
    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestCaCat001Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaCat001Signal);

    //check certificates
    std::shared_ptr<TestCaCat001Signal> typed_decoded_signal =
        std::static_pointer_cast<TestCaCat001Signal>(decoded_signal);

    //number of certificates
    ASSERT_EQ(input_certs.size(), typed_decoded_signal->received_certs_.size());

    //compare certificates one-by-one
    for (int i=0; i< input_certs.size(); i++ )
    {
       EXPECT_EQ(X509_cmp(input_certs[i], typed_decoded_signal->received_certs_[i]), 0 );
    }
}


/**
 * \brief Test encode/decode realistic valid Ca CAT 001
 */
TEST (CCMSignalsTest, EncodeDecodeCaCat001)
{

    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaCat001Signal> signal = CaCat001Signal::CreateCaCat001Signal(
        transaction_id, 1);

    signal->SetRecipients(users);

    std::vector<X509*> input_certs;


    input_certs.push_back(fsm::ut_GetCertFromPEM(fsm::ROOT_CERT_PEM.c_str()));
    input_certs.push_back(fsm::ut_GetCertFromPEM(fsm::CLIENT_ONE_CERT_PEM.c_str()));
    input_certs.push_back(fsm::ut_GetCertFromPEM(fsm::CLIENT_TWO_CERT_PEM.c_str()));

    TestCaCat001(input_certs, "ca_cat_001_signal.bin");
}

/**
 * \brief Test encode/decode Ca DEL 001
 */
TEST (CCMSignalsTest, EncodeDecodeCaDel001)
{

    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDel001Signal> signal = CaDel001Signal::CreateCaDel001Signal(
        transaction_id, 1);

    signal->SetRecipients(users);

    //encode signal
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    //decode the signal
    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(TestCaDel001Signal::oid, TestCaDel001Signal::CreateTestCaDel001Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
    {
        FAIL();
    }

    //check signal type
    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(TestCaDel001Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDel001Signal);
}


/**
 * \brief Test encoding incorrect Ca CAT 001, no certificates
 */
TEST (CCMSignalsTest, EncodeIncorrectCaCat001)
{

    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaCat001Signal> signal = CaCat001Signal::CreateCaCat001Signal(
        transaction_id, 1);

    signal->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);
    ASSERT_TRUE(buffer == nullptr);
}

/**
 * \brief Test adding too many certificates to Ca CAT 001
 */
TEST (CCMSignalsTest, AddToManyCertsToCaCat001)
{

    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaCat001Signal> signal = CaCat001Signal::CreateCaCat001Signal(
                transaction_id, 1);

    signal->SetRecipients(users);

    std::vector<X509*> input_certs;

    //we should be able to add MAX_NO_CERTIFICATES
    for (int i=0; i<CaCat001Signal::MAX_NO_CERTIFICATES; i++)
    {
        ASSERT_TRUE(signal->AddCertificate(fsm::ut_GetCertFromPEM(fsm::ROOT_CERT_PEM.c_str())));
    }

    //but not more
    ASSERT_FALSE(signal->AddCertificate(fsm::ut_GetCertFromPEM(fsm::ROOT_CERT_PEM.c_str())));
}



/**
 * \brief Test encode/decode max number of certificates
 */
TEST (CCMSignalsTest, EncodeDecodeMaxCertsCaCat001)
{

    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaCat001Signal> signal = CaCat001Signal::CreateCaCat001Signal(
        transaction_id, 1);

    signal->SetRecipients(users);

    std::vector<X509*> input_certs;

    //add 20 certificates
    for (int i = 0; i<10; i++)
    {
      input_certs.push_back(fsm::ut_GetCertFromPEM(fsm::ROOT_CERT_PEM.c_str()));
      input_certs.push_back(fsm::ut_GetCertFromPEM(fsm::CLIENT_ONE_CERT_PEM.c_str()));
    }

    TestCaCat001(input_certs);
}


/**
 * \brief Test encode/decode CA-DPA-020 containing a ECDHE Approval voc frame and UUID pairing id.
 */
TEST (CCMSignalsTest, TestEncodeDecodeCaDpa020EcdheApprovalUuid)
{
    // setup signal, voc frame and voc frame codec
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDpa020Signal> ca_dpa_020 = std::make_shared<CaDpa020Signal>(transaction_id, 1);

    ca_dpa_020->SetRecipients(users);

    VocFrameCodec codec;
    EcdheApprovalVocFrame frame;

    // set data
    CaDpa020Signal::DevicePairingIdType pairing_id_type =
        CaDpa020Signal::DevicePairingIdType::kDevicePairingIdTypeUuid;
    std::string pairing_id = "this is ! a UUID";

    ASSERT_TRUE(ca_dpa_020->SetDevicePairingId(pairing_id_type,
                                     pairing_id));
    ASSERT_TRUE(ca_dpa_020->SetFrame(codec,
                                     frame));

    // encode

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ca_dpa_020);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_dpa_020_ecdhe_approval_uuid.bin", (char*)(buffer->data()), buffer->size());

    // decode

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(CaDpa020Signal::oid_, CaDpa020Signal::CreateCaDpa020Signal);

    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(CaDpa020Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDpa020Signal);

    std::shared_ptr<CaDpa020Signal> typed_decoded_signal =
        std::static_pointer_cast<CaDpa020Signal>(decoded_signal);

    // compare payload

    CaDpa020Signal::DevicePairingIdType decoded_paring_id_type;
    std::string decoded_pairing_id;

    typed_decoded_signal->GetDevicePairingId(decoded_paring_id_type, decoded_pairing_id);

    EXPECT_EQ(decoded_paring_id_type, pairing_id_type);
    EXPECT_EQ(decoded_pairing_id, pairing_id);

    std::shared_ptr<fsm::Signal> decoded_frame = typed_decoded_signal->GetVocFrame(codec);

    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(EcdheApprovalVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kEcdheApprovalVocFrame);

}

/**
 * \brief Test encode/decode CA-DPA-020 containing a ECDHE Approval voc frame and VIN pairing id.
 */
TEST (CCMSignalsTest, TestEncodeDecodeCaDpa020EcdheApprovalVin)
{
    // setup signal, voc frame and voc frame codec
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDpa020Signal> ca_dpa_020 = std::make_shared<CaDpa020Signal>(transaction_id, 1);

    ca_dpa_020->SetRecipients(users);

    VocFrameCodec codec;
    EcdheApprovalVocFrame frame;

    // set data
    CaDpa020Signal::DevicePairingIdType pairing_id_type =
        CaDpa020Signal::DevicePairingIdType::kDevicePairingIdTypeVin;
    std::string pairing_id = "this is not a VIN";

    ASSERT_TRUE(ca_dpa_020->SetDevicePairingId(pairing_id_type,
                                     pairing_id));
    ASSERT_TRUE(ca_dpa_020->SetFrame(codec,
                                     frame));

    // encode

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ca_dpa_020);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_dpa_020_ecdhe_approval_vin.bin", (char*)(buffer->data()), buffer->size());

    // decode

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(CaDpa020Signal::oid_, CaDpa020Signal::CreateCaDpa020Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(CaDpa020Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDpa020Signal);

    std::shared_ptr<CaDpa020Signal> typed_decoded_signal =
        std::static_pointer_cast<CaDpa020Signal>(decoded_signal);

    // compare payload

    CaDpa020Signal::DevicePairingIdType decoded_paring_id_type;
    std::string decoded_pairing_id;

    typed_decoded_signal->GetDevicePairingId(decoded_paring_id_type, decoded_pairing_id);

    EXPECT_EQ(decoded_paring_id_type, pairing_id_type);
    EXPECT_EQ(decoded_pairing_id, pairing_id);

    std::shared_ptr<fsm::Signal> decoded_frame = typed_decoded_signal->GetVocFrame(codec);

    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(EcdheApprovalVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kEcdheApprovalVocFrame);

}

/**
 * \brief Test encode/decode CA-DPA-020 containing a AES BLE DA KEY voc frame and UUID pairing id.
 */
TEST (CCMSignalsTest, TestEncodeDecodeCaDpa020AesBleDaKeyUuid)
{
    // setup signal, voc frame and voc frame codec
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDpa020Signal> ca_dpa_020 = std::make_shared<CaDpa020Signal>(transaction_id, 1);

    ca_dpa_020->SetRecipients(users);

    AesBleDaKeyVocFrame::BleDaKey key =
        {'b', 'l', 'e', 'd', 'a', 't', 'a', 'a', 'c', 'c', 'e', 's', 's', 'k', 'e', 'y'};
    AesBleDaKeyVocFrame frame(key);

    VocFrameCodec codec;
    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;
    std::vector<unsigned char> secret;
    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    // set data
    CaDpa020Signal::DevicePairingIdType pairing_id_type =
        CaDpa020Signal::DevicePairingIdType::kDevicePairingIdTypeUuid;
    std::string pairing_id = "this is ! a UUID";

    ASSERT_TRUE(ca_dpa_020->SetDevicePairingId(pairing_id_type,
                                     pairing_id));
    ASSERT_TRUE(ca_dpa_020->SetFrame(codec,
                                     frame));

    // encode

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ca_dpa_020);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_dpa_020_aes_ble_da_key_uuid.bin", (char*)(buffer->data()), buffer->size());

    // decode

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(CaDpa020Signal::oid_, CaDpa020Signal::CreateCaDpa020Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(CaDpa020Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDpa020Signal);

    std::shared_ptr<CaDpa020Signal> typed_decoded_signal =
        std::static_pointer_cast<CaDpa020Signal>(decoded_signal);

    // compare payload

    CaDpa020Signal::DevicePairingIdType decoded_paring_id_type;
    std::string decoded_pairing_id;

    typed_decoded_signal->GetDevicePairingId(decoded_paring_id_type, decoded_pairing_id);

    EXPECT_EQ(decoded_paring_id_type, pairing_id_type);
    EXPECT_EQ(decoded_pairing_id, pairing_id);

    std::shared_ptr<fsm::Signal> decoded_frame = typed_decoded_signal->GetVocFrame(codec);

    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(AesBleDaKeyVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kAesBleDaKeyVocFrame);

    std::shared_ptr<AesBleDaKeyVocFrame> typed_decoded_frame =
        std::static_pointer_cast<AesBleDaKeyVocFrame>(decoded_frame);

    AesBleDaKeyVocFrame::BleDaKey decoded_key = typed_decoded_frame->GetKey();
    EXPECT_TRUE(decoded_key == key);

    EVP_PKEY_free(tcam_key);
    EVP_PKEY_free(peer_key);
}

/**
 * \brief Test encode/decode CA-DPA-020 containing a AES BLE DA KEY voc frame and VIN pairing id.
 */
TEST (CCMSignalsTest, TestEncodeDecodeCaDpa020AesBleDaKeyVin)
{
    // setup signal, voc frame and voc frame codec
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDpa020Signal> ca_dpa_020 = std::make_shared<CaDpa020Signal>(transaction_id, 1);

    ca_dpa_020->SetRecipients(users);

    AesBleDaKeyVocFrame::BleDaKey key =
        {'b', 'l', 'e', 'd', 'a', 't', 'a', 'a', 'c', 'c', 'e', 's', 's', 'k', 'e', 'y'};
    AesBleDaKeyVocFrame frame(key);

    VocFrameCodec codec;
    EVP_PKEY* tcam_key = nullptr;
    EVP_PKEY* peer_key = nullptr;
    std::vector<unsigned char> secret;
    ASSERT_TRUE(MakeKeysAndSecret(&tcam_key, &peer_key, secret));
    ASSERT_TRUE(codec.SetEncryptionKeys(tcam_key, peer_key, secret));

    // set data
    CaDpa020Signal::DevicePairingIdType pairing_id_type =
        CaDpa020Signal::DevicePairingIdType::kDevicePairingIdTypeVin;
    std::string pairing_id = "this is not a VIN";

    ASSERT_TRUE(ca_dpa_020->SetDevicePairingId(pairing_id_type,
                                     pairing_id));
    ASSERT_TRUE(ca_dpa_020->SetFrame(codec,
                                     frame));

    // encode

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ca_dpa_020);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_dpa_020_aes_ble_da_key_vin.bin", (char*)(buffer->data()), buffer->size());

    // decode

    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(CaDpa020Signal::oid_, CaDpa020Signal::CreateCaDpa020Signal);

    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(CaDpa020Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDpa020Signal);

    std::shared_ptr<CaDpa020Signal> typed_decoded_signal =
        std::static_pointer_cast<CaDpa020Signal>(decoded_signal);

    // compare payload

    CaDpa020Signal::DevicePairingIdType decoded_paring_id_type;
    std::string decoded_pairing_id;

    typed_decoded_signal->GetDevicePairingId(decoded_paring_id_type, decoded_pairing_id);

    EXPECT_EQ(decoded_paring_id_type, pairing_id_type);
    EXPECT_EQ(decoded_pairing_id, pairing_id);

    std::shared_ptr<fsm::Signal> decoded_frame = typed_decoded_signal->GetVocFrame(codec);

    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(AesBleDaKeyVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kAesBleDaKeyVocFrame);

    std::shared_ptr<AesBleDaKeyVocFrame> typed_decoded_frame =
        std::static_pointer_cast<AesBleDaKeyVocFrame>(decoded_frame);

    AesBleDaKeyVocFrame::BleDaKey decoded_key = typed_decoded_frame->GetKey();
    EXPECT_TRUE(decoded_key == key);

    EVP_PKEY_free(tcam_key);
    EVP_PKEY_free(peer_key);
}

/**
 * \brief Test encode/decode CA-DPA-021 containing a ECDHE Approval voc frame and VIN pairing id.
 *        As earlier tests have already verified both DevicePairingId and ResponseInfo and Voc Frames
 *        we do not have a variety of specific CA-DPA-021 tests testing ranges of input,
 */
TEST (CCMSignalsTest, TestEncodeDecodeCaDpa021)
{
    // setup signal, voc frame and voc frame codec
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDpa021Signal> ca_dpa_021 = std::make_shared<CaDpa021Signal>(transaction_id, 1);

    ca_dpa_021->SetRecipients(users);

    VocFrameCodec codec;
    EcdheApprovalVocFrame frame;

    // first try without VOC Frame (its optional)
    // set data
    volvo_on_call::DevicePairingId::DevicePairingIdType pairing_id_type =
        volvo_on_call::DevicePairingId::DevicePairingIdType::kDevicePairingIdTypeVin;
    std::string pairing_id = "this is not a VIN";

    ASSERT_TRUE(ca_dpa_021->SetDevicePairingId(pairing_id_type,
                                               pairing_id));
    ASSERT_TRUE(ca_dpa_021->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified));

    // encode
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ca_dpa_021);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_dpa_021_no_frame_vin_success.bin", (char*)(buffer->data()), buffer->size());

    // decode
    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(CaDpa021Signal::oid_, CaDpa021Signal::CreateCaDpa021Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(CaDpa021Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDpa021Signal);

    std::shared_ptr<CaDpa021Signal> typed_decoded_signal =
        std::static_pointer_cast<CaDpa021Signal>(decoded_signal);

    // compare payload
    volvo_on_call::DevicePairingId::DevicePairingIdType decoded_paring_id_type;
    fsm::ResponseInfoData::ResponseCodeSuccess decoded_response_code;
    std::string decoded_pairing_id;

    typed_decoded_signal->GetDevicePairingId(decoded_paring_id_type, decoded_pairing_id);

    EXPECT_EQ(decoded_paring_id_type, pairing_id_type);
    EXPECT_EQ(decoded_pairing_id, pairing_id);

    EXPECT_TRUE(typed_decoded_signal->IsSuccess());
    decoded_response_code = typed_decoded_signal->GetSuccessCode();
    EXPECT_EQ(decoded_response_code, fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);

    // Now also try to set a frame.
    ASSERT_TRUE(ca_dpa_021->SetFrame(codec, frame));

    // encode
    fsm::SetCertsForEncode();

    buffer = fsm::SignalFactory::Encode(ca_dpa_021);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_dpa_021_ecdhe_approval_vin_success.bin", (char*)(buffer->data()), buffer->size());

    // decode
    fsm::SetCertsForDecode();

    decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(CaDpa021Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDpa021Signal);

    typed_decoded_signal = std::static_pointer_cast<CaDpa021Signal>(decoded_signal);

    // compare payload
    typed_decoded_signal->GetDevicePairingId(decoded_paring_id_type, decoded_pairing_id);

    EXPECT_EQ(decoded_paring_id_type, pairing_id_type);
    EXPECT_EQ(decoded_pairing_id, pairing_id);

    EXPECT_TRUE(typed_decoded_signal->IsSuccess());
    decoded_response_code = typed_decoded_signal->GetSuccessCode();
    EXPECT_EQ(decoded_response_code, fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);

    std::shared_ptr<fsm::Signal> decoded_frame = typed_decoded_signal->GetVocFrame(codec);

    ASSERT_EQ(typeid(*decoded_frame).hash_code(), typeid(EcdheApprovalVocFrame).hash_code());
    ASSERT_EQ(decoded_frame->GetSignalType(), VocSignalTypes::kEcdheApprovalVocFrame);

}


/**
 * \brief Test encode/decode CA-DPA-001 mandatory fields missing
 */

TEST (CCMSignalsTest, TestEncodeDecodeCaDpa001MandatoryMissing)
{
    // setup signals
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDpa001Signal> ca_dpa_001_none = std::make_shared<CaDpa001Signal>(transaction_id, 1); //no field set
    std::shared_ptr<CaDpa001Signal> ca_dpa_001_devId = std::make_shared<CaDpa001Signal>(transaction_id, 1); //only pairing id set
    std::shared_ptr<CaDpa001Signal> ca_dpa_001_command = std::make_shared<CaDpa001Signal>(transaction_id, 1);//only command set

    ca_dpa_001_none->SetRecipients(users);
    ca_dpa_001_devId->SetRecipients(users);
    ca_dpa_001_command->SetRecipients(users);

    // set data, mandatory only
    volvo_on_call::DevicePairingId::DevicePairingIdType pairing_id_type =
        volvo_on_call::DevicePairingId::DevicePairingIdType::kDevicePairingIdTypeVin;
    std::string pairing_id = "this is not a VIN";

    ASSERT_TRUE(ca_dpa_001_devId->SetDevicePairingId(pairing_id_type,
                                               pairing_id));


    ca_dpa_001_command->SetCommandStart();
    // encode
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ca_dpa_001_none);
    ASSERT_TRUE(buffer == nullptr);

    buffer = fsm::SignalFactory::Encode(ca_dpa_001_devId);
    ASSERT_TRUE(buffer == nullptr);

    buffer = fsm::SignalFactory::Encode(ca_dpa_001_command);
    ASSERT_TRUE(buffer == nullptr);
}

TEST (CCMSignalsTest, TestEncodeDecodeCaDpa001Mandatory)
{
    // setup signal
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDpa001Signal> ca_dpa_001 = std::make_shared<CaDpa001Signal>(transaction_id, 1);

    ca_dpa_001->SetRecipients(users);

    // set data, mandatory only
    volvo_on_call::DevicePairingId::DevicePairingIdType pairing_id_type =
        volvo_on_call::DevicePairingId::DevicePairingIdType::kDevicePairingIdTypeVin;
    std::string pairing_id = "this is not a VIN";

    ASSERT_TRUE(ca_dpa_001->SetDevicePairingId(pairing_id_type,
                                               pairing_id));

    ca_dpa_001->SetCommandStart();

    // encode
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ca_dpa_001);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_dpa_001_mandatory_mandatory.bin", (char*)(buffer->data()), buffer->size());

    // decode
    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(CaDpa001Signal::oid_, CaDpa001Signal::CreateCaDpa001Signal);
    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(CaDpa001Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDpa001Signal);

    std::shared_ptr<CaDpa001Signal> typed_decoded_signal =
        std::static_pointer_cast<CaDpa001Signal>(decoded_signal);

    // compare payload
    volvo_on_call::DevicePairingId::DevicePairingIdType decoded_paring_id_type;
    std::string decoded_pairing_id;

    typed_decoded_signal->GetDevicePairingId(decoded_paring_id_type, decoded_pairing_id);

    EXPECT_EQ(typed_decoded_signal->command_ , CaDpa001Signal::kCommandStart);
    EXPECT_EQ(typed_decoded_signal->remote_connection_timeout_set_ , false);
    EXPECT_EQ(typed_decoded_signal->pairing_timeout_set_ , false);
    EXPECT_EQ(typed_decoded_signal->vehicle_info_set_ , false);

    EXPECT_EQ(decoded_paring_id_type, pairing_id_type);
    EXPECT_EQ(decoded_pairing_id, pairing_id);

}



/**
 * \brief Test encode/decode CA-DPA-001 with optionals
 */
TEST (CCMSignalsTest, TestEncodeDecodeCaDpa001Optionals)
{
    // setup signal
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDpa001Signal> ca_dpa_001 = std::make_shared<CaDpa001Signal>(transaction_id, 1);

    ca_dpa_001->SetRecipients(users);

    // set data, mandatory
    volvo_on_call::DevicePairingId::DevicePairingIdType pairing_id_type =
        volvo_on_call::DevicePairingId::DevicePairingIdType::kDevicePairingIdTypeVin;
    std::string pairing_id = "this is not a VIN";

    ASSERT_TRUE(ca_dpa_001->SetDevicePairingId(pairing_id_type,
                                               pairing_id));

    ca_dpa_001->SetCommandStop();

    //set optionals
    struct tm remote_con_timeout = {};
    remote_con_timeout.tm_year = 27;
    remote_con_timeout.tm_mon = 3;
    remote_con_timeout.tm_mday = 14;

    ca_dpa_001->SetRemoteConnectionTimeout(remote_con_timeout);

    struct tm paring_timeout = {};
    paring_timeout.tm_year = 26;
    paring_timeout.tm_mon = 2;
    paring_timeout.tm_mday = 15;

    ca_dpa_001->SetPairingTimeout(paring_timeout);

    std::string vin = "33334";
    long model = 2222;
    long model_year = 2022;

    ca_dpa_001->SetVehicleInfo(vin, model, model_year);

    // encode
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ca_dpa_001);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("ca_dpa_001_mandatory_optionals.bin", (char*)(buffer->data()), buffer->size());

    // decode
    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(CaDpa001Signal::oid_, CaDpa001Signal::CreateCaDpa001Signal);

    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(CaDpa001Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDpa001Signal);

    std::shared_ptr<CaDpa001Signal> typed_decoded_signal =
        std::static_pointer_cast<CaDpa001Signal>(decoded_signal);

    // compare payload
    volvo_on_call::DevicePairingId::DevicePairingIdType decoded_paring_id_type;
    std::string decoded_pairing_id;

    typed_decoded_signal->GetDevicePairingId(decoded_paring_id_type, decoded_pairing_id);

    EXPECT_EQ(typed_decoded_signal->command_ , CaDpa001Signal::kCommandStop);

    EXPECT_EQ(typed_decoded_signal->remote_connection_timeout_set_ , true);
    EXPECT_EQ(TimeEqual(&typed_decoded_signal->remote_connection_timeout_ , &remote_con_timeout), true);

    EXPECT_EQ(typed_decoded_signal->pairing_timeout_set_ , true);
    EXPECT_EQ(TimeEqual(&typed_decoded_signal->pairing_timeout_ , &paring_timeout), true);

    EXPECT_EQ(typed_decoded_signal->vehicle_info_set_ , true);
    EXPECT_TRUE(std::to_string(typed_decoded_signal->vin_) ==  vin);
    EXPECT_EQ(typed_decoded_signal->model_, model);
    EXPECT_EQ(typed_decoded_signal->model_year_, model_year);

    EXPECT_EQ(decoded_paring_id_type, pairing_id_type);
    EXPECT_EQ(decoded_pairing_id, pairing_id);

}


/**
 * \brief Test encode/decode CA-DPA-002 with optionals
 */
//tests ca dpa signal optionals
//setting set_status will set status
//initiated to false assumes finished and then FinishedStatus is used, otherwise it is ignored
//empty session_id is not used
void TestCaDpa002(bool set_status, bool initiated, CaDpa002Signal::FinishedStatus finish_status, std::string  sessionId, std::string filename)
{
    // setup signal
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<CaDpa002Signal> ca_dpa_002 = std::make_shared<CaDpa002Signal>(transaction_id, 1);

    ca_dpa_002->SetRecipients(users);

    // set data, mandatory only
    volvo_on_call::DevicePairingId::DevicePairingIdType pairing_id_type =
        volvo_on_call::DevicePairingId::DevicePairingIdType::kDevicePairingIdTypeVin;
    std::string pairing_id = "this is not a VIN";

    ASSERT_TRUE(ca_dpa_002->SetDevicePairingId(pairing_id_type,
                                               pairing_id));

    ASSERT_TRUE(ca_dpa_002->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified));

    //Encode optionals

    if (set_status)
    {
        if (initiated)
        {
            ca_dpa_002->SetInitiated();
        }
        else
        {
            ca_dpa_002->SetFinished(finish_status);
        }
    }

    if (sessionId.size() > 0)
    {
        ca_dpa_002->SetRemoteConnectionSessionId(sessionId);
    }


    // encode
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ca_dpa_002);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile(filename, (char*)(buffer->data()), buffer->size());

    // decode
    fsm::SetCertsForDecode();


    fsm::SignalFactory::RegisterCcmFactory(CaDpa002Signal::oid_, CaDpa002Signal::CreateCaDpa002Signal);

    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(CaDpa002Signal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , VocSignalTypes::kCaDpa002Signal);

    std::shared_ptr<CaDpa002Signal> typed_decoded_signal =
        std::static_pointer_cast<CaDpa002Signal>(decoded_signal);

    // compare payload
    volvo_on_call::DevicePairingId::DevicePairingIdType decoded_paring_id_type;
    std::string decoded_pairing_id;

    typed_decoded_signal->GetDevicePairingId(decoded_paring_id_type, decoded_pairing_id);

    fsm::ResponseInfoData::ResponseCodeSuccess decoded_response_code;


    EXPECT_EQ(decoded_paring_id_type, pairing_id_type);
    EXPECT_EQ(decoded_pairing_id, pairing_id);

    EXPECT_TRUE(typed_decoded_signal->IsSuccess());
    decoded_response_code = typed_decoded_signal->GetSuccessCode();
    EXPECT_EQ(decoded_response_code, fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);


    CaDpa002Signal::FinishedStatus received_finished_status;
    std::string received_session_id;


    EXPECT_EQ(typed_decoded_signal->IsFinished(received_finished_status), set_status && !initiated);

    if (set_status && !initiated)
    {
        EXPECT_EQ(finish_status, received_finished_status );
    }

    EXPECT_EQ(typed_decoded_signal->IsInitiated(), set_status && initiated );

    EXPECT_EQ(typed_decoded_signal->GetRemoteConnectionSessionId(received_session_id), sessionId.size() > 0 );

    if (sessionId.size() > 0)
    {
        EXPECT_EQ(sessionId, received_session_id);
    }
}


TEST (CCMSignalsTest, TestEncodeDecodeCaDpa002MandatoryParameters)
{
    TestCaDpa002(false, false, CaDpa002Signal::kStoppedByCommand, std::string(), "ca_dpa_002_mandatory.bin");
}

TEST (CCMSignalsTest, TestEncodeDecodeCaDpa002OptionalInitiated)
{
    TestCaDpa002(true, true, CaDpa002Signal::kStoppedByCommand, std::string(), "ca_dpa_002_initiated.bin");

}
TEST (CCMSignalsTest, TestEncodeDecodeCaDpa002OptionalFinished)
{
    TestCaDpa002(true, false, CaDpa002Signal::kStoppedByCommand, std::string(), "ca_dpa_002_stopped.bin");
    TestCaDpa002(true, false, CaDpa002Signal::kTerminatedRemoteConnectionTimeout, std::string(), "ca_dpa_002_contimeout.bin");
    TestCaDpa002(true, false, CaDpa002Signal::kTerminatedPairingTimeout, std::string(), "ca_dpa_002_pairtimeout.bin");
}

TEST (CCMSignalsTest, TestEncodeDecodeCaDpa002OptionalSession)
{
    std::string session_id = "thisissessionid1"; //must be 16 long
    TestCaDpa002(false, true, CaDpa002Signal::kStoppedByCommand, session_id, "ca_dpa_002_onlysessionid.bin");
}

TEST (CCMSignalsTest, TestEncodeDecodeCaDpa002OptionalMix)
{
    std::string session_id = "thisissessionid1"; //must be 16 long
    TestCaDpa002(true, true, CaDpa002Signal::kStoppedByCommand, session_id, "ca_dpa_002_init_sessionid.bin");
}

//Compares 2 feature infos
void CompareFeatureInfo(fsm::FeaturesSignal::FeatureInfo& feature1, fsm::FeaturesSignal::FeatureInfo& feature2)
{
    EXPECT_TRUE(feature1.name.compare(feature2.name)==0);
    EXPECT_TRUE(feature1.enabled == feature2.enabled);
    EXPECT_TRUE(feature1.visible == feature2.visible);
    EXPECT_TRUE(feature1.name.compare(feature2.name)==0);
    EXPECT_TRUE(feature1.uri.compare(feature2.uri)==0);
    EXPECT_TRUE(feature1.description.compare(feature2.description)==0);

    bool same_number_of_tags = (feature1.tags.size() == feature2.tags.size());
    EXPECT_TRUE(same_number_of_tags);

    if (same_number_of_tags)
    {
        for (int i=0; i<feature1.tags.size(); i++ )
        {
             EXPECT_TRUE(feature1.tags[i].compare(feature2.tags[i])==0);
        }
    }

}

//Tests FeaturesSignal, takes in thiz string, a list of features and a file name to store the signal. thiz and feature
//list are used to create a FeaturesSignal, encode and decode it and make sure the output matches the input
//All parameters are optional, inserting empty string or list means that given element should not be encoded
void TestFeaturesSignal(std::string thiz, std::vector<fsm::FeaturesSignal::FeatureInfo>& features_list, std::string filename)
{
    // setup signal
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<fsm::FeaturesSignal> features = std::make_shared<fsm::FeaturesSignal>(transaction_id, 1);

    features->SetRecipients(users);

    //if thiz uri defined, add it
    if (thiz.size() != 0)
    {
        features->SetUri(thiz);
    }

    //add all the features from the features list
    for (int i=0; i<features_list.size(); i++)
    {
        features->AddFeature(features_list[i]);
    }

    // encode
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(features);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile(filename, (char*)(buffer->data()), buffer->size());

    // decode
    fsm::SetCertsForDecode();


    fsm::SignalFactory::RegisterCcmFactory(fsm::FeaturesSignal::oid_, fsm::FeaturesSignal::CreateFeaturesSignal);

    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
    {
        FAIL();
    }

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(fsm::FeaturesSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , fsm::Signal::kFeaturesSignal);

    std::shared_ptr<fsm::FeaturesSignal> typed_decoded_signal =
            std::static_pointer_cast<fsm::FeaturesSignal>(decoded_signal);

    //check if thiz matches expectations
    EXPECT_TRUE(typed_decoded_signal->GetUri().compare(thiz) == 0); //works also for empty strings

    //retrieve list of features
    std::vector<fsm::FeaturesSignal::FeatureInfo> received_features = typed_decoded_signal->GetFeatures();
    bool same_number_of_features = received_features.size() == features_list.size();
    EXPECT_TRUE(same_number_of_features);

    if (same_number_of_features )
    {
        //loop through all the features, make sure they match expectations
        for (int i=0; i<features_list.size(); i++)
        {
            CompareFeatureInfo(features_list[i], received_features[i] );
        }
    }
}

//Empty signal is legal, all parameters are optional
TEST (CCMSignalsTest, TestFeaturesSignalEmpty)
{
    std::vector<fsm::FeaturesSignal::FeatureInfo> features_list;
    TestFeaturesSignal(std::string(""), features_list, std::string("features_signal_empty.bin"));
}

//test one feature case (mandatory fields only)
TEST (CCMSignalsTest, TestFeaturesSignalOneFeatureMandatoryFieldsF)
{

    std::vector<fsm::FeaturesSignal::FeatureInfo> features_list;

    //define and add feature, only mandatory fields
    fsm::FeaturesSignal::FeatureInfo feature_info;
    feature_info.name = "name";
    feature_info.enabled = true;
    feature_info.visible = true;
    feature_info.uri = "uri";
    features_list.push_back(feature_info);
    TestFeaturesSignal(std::string(""), features_list, std::string("features_signal_onefeature_mandatoryfields.bin"));

}

//test one feature case (all fields)
TEST (CCMSignalsTest, TestFeaturesSignalOneFeatureAllFieldsF)
{

    std::vector<fsm::FeaturesSignal::FeatureInfo> features_list;

    //define and add feature
    fsm::FeaturesSignal::FeatureInfo feature_info;
    feature_info.name = "name";
    feature_info.enabled = true;
    feature_info.visible = false;
    feature_info.uri = "uri";
    feature_info.icon = "icon";
    feature_info.description = "description";
    feature_info.tags.push_back("tag1");
    feature_info.tags.push_back("tag2");
    features_list.push_back(feature_info);

    TestFeaturesSignal(std::string("THIZ"), features_list, std::string("features_signal_onefeature_allfields.bin"));

}

//test multiple feature case with variable number of tags
TEST (CCMSignalsTest, TestFeaturesSignalMultipleFeatures)
{

    std::vector<fsm::FeaturesSignal::FeatureInfo> features_list;

    for (int i=0; i<20; i++)
    {
        //define and add feature, introduce some variability in values for different features
        fsm::FeaturesSignal::FeatureInfo feature_info;
        std::string postfix = std::to_string(i);
        feature_info.name = "name" + postfix;
        feature_info.enabled = (i % 3 == 0);
        feature_info.visible = (i % 2 == 0);
        if (i % 4 != 0)
        {
            feature_info.uri = "uri" + postfix;
            feature_info.icon = "icon";
            feature_info.description = "description" + postfix;
        }
        for (int j=0; j<i; j++)
        {
            feature_info.tags.push_back("tag_" + postfix + "_" + std::to_string(j));
        }
        features_list.push_back(feature_info);
    }

    TestFeaturesSignal(std::string("MULTIPLE"), features_list, std::string("features_signal_multiplefeatures.bin"));

}

/**
 * \brief Function for testing EntryPointSignal
 * \param[in] uri uri to set
 * \param[in] specification_version specification version to set
 * \param[in] implementation_version implementation version to set
 * \param[in] client_uri client_uri to set
 * \param[in] host host to set
 * \param[in] port port to set
 * \param[in] signaling_service_uri signaling service uri to set (empty string for undefined)
 * \param[in] valid_locations list of valid locations to set (empty vector for undefined)
 * \param[in] filename name of the file to be created
 */
void TestEntryPointSignal (std::string uri, long specification_version, long implementation_version,
                           std::string client_uri, std::string host, long port,
                           std::string signaling_service_uri, std::vector<std::string> valid_locations,
                           std::string filename)
{



    // setup signal
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<fsm::EntryPointSignal> entry_point = std::make_shared<fsm::EntryPointSignal>(transaction_id, 1);

    entry_point->SetRecipients(users);

    entry_point->SetParameters(uri, specification_version, implementation_version,
                               client_uri, host, port, signaling_service_uri, valid_locations);

    // encode
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(entry_point);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile(filename, (char*)(buffer->data()), buffer->size());

    // decode
    fsm::SetCertsForDecode();

    fsm::SignalFactory::RegisterCcmFactory(fsm::EntryPointSignal::oid_, fsm::EntryPointSignal::CreateEntryPointSignal);

    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(fsm::EntryPointSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , fsm::Signal::kEntryPointSignal);

    std::shared_ptr<fsm::EntryPointSignal> typed_decoded_signal =
            std::static_pointer_cast<fsm::EntryPointSignal>(decoded_signal);

    //check if decoded output matches expectations
    EXPECT_TRUE(typed_decoded_signal->GetUri().compare(uri) == 0);
    EXPECT_TRUE(typed_decoded_signal->GetSpecificationVersion() == specification_version);
    EXPECT_TRUE(typed_decoded_signal->GetImplementationVersion() == implementation_version);
    EXPECT_TRUE(typed_decoded_signal->GetClientUri().compare(client_uri) == 0);
    EXPECT_TRUE(typed_decoded_signal->GetHost().compare(host) == 0);
    EXPECT_TRUE(typed_decoded_signal->GetPort() == port);
    EXPECT_TRUE(typed_decoded_signal->GetSignalServiceUri().compare(signaling_service_uri) == 0);

    //retrieve the list of valid locations
    std::vector<std::string> received_locations = typed_decoded_signal->GetValidLocations();

    bool same_number_of_locations = received_locations.size() == valid_locations.size();
    EXPECT_TRUE(same_number_of_locations);

    if (same_number_of_locations )
    {
        //loop through all the features, make sure they match expectations
        for (int i=0; i<received_locations.size(); i++)
        {
            EXPECT_TRUE(received_locations[i].compare(valid_locations[i]) == 0);
        }
    }
}


//test Entry Point with only mandatory fields provided
TEST (CCMSignalsTest, TestEntryPointMandatory)
{
    std::string uri = "uri";
    long specification_version = 1212;
    long implementation_version = 231;
    std::string client_uri = "client_uri";
    std::string host = "host";
    long port = 8282;
    std::string signaling_service_uri;
    std::vector<std::string> valid_locations;

    TestEntryPointSignal(uri, specification_version, implementation_version,
                         client_uri, host, port, signaling_service_uri,
                         valid_locations, "entry_point_signal_mandatory.bin");
}

//test Entry Point with all fields provided
TEST (CCMSignalsTest, TestEntryPointOptional)
{
    std::string uri = "uri2";
    long specification_version = 4345;
    long implementation_version = 3453;
    std::string client_uri = "client_uri_2";
    std::string host = "host_2";
    long port = 5455;
    std::string signaling_service_uri = "signaling_service_uri_2";
    std::vector<std::string> valid_locations;
    valid_locations.push_back("SE");
    valid_locations.push_back("DE");
    valid_locations.push_back("RO");
    valid_locations.push_back("PK");
    valid_locations.push_back("CN");
    valid_locations.push_back("UA");
    valid_locations.push_back("PL");

    TestEntryPointSignal(uri, specification_version, implementation_version,
                         client_uri, host, port, signaling_service_uri,
                         valid_locations, "entry_point_signal_all.bin");
}

//tests comparison operator of FeatureInfo instances
TEST (CCMSignalsTest, TestFeatureInfoComparison)
{
    //define and add feature
    fsm::FeaturesSignal::FeatureInfo feature_info1;
    fsm::FeaturesSignal::FeatureInfo feature_info2;
    fsm::FeaturesSignal::FeatureInfo feature_info3;
    feature_info1.name = "name";
    feature_info1.enabled = true;
    feature_info1.visible = false;
    feature_info1.uri = "uri";
    feature_info1.icon = "icon";
    feature_info1.description = "description";
    feature_info1.tags.push_back("tag1");
    feature_info1.tags.push_back("tag2");

    //identical to 1
    feature_info2.name = "name";
    feature_info2.enabled = true;
    feature_info2.visible = false;
    feature_info2.uri = "uri";
    feature_info2.icon = "icon";
    feature_info2.description = "description";
    feature_info2.tags.push_back("tag1");
    feature_info2.tags.push_back("tag2");

    //different
    feature_info3.name = "name";
    feature_info3.enabled = true;
    feature_info3.visible = false;
    feature_info3.uri = "uri";
    feature_info3.icon = "icon";
    feature_info3.description = "description";
    feature_info3.tags.push_back("tag1");
    feature_info3.tags.push_back("tag2");

    EXPECT_TRUE(feature_info1 == feature_info2);

    ASSERT_TRUE(feature_info1 == feature_info3); //starting point for testing diff, initially identical

    //test name
    feature_info3.name = "name2";
    EXPECT_FALSE(feature_info1 == feature_info3);
    feature_info3.name = "name";

    //test enabled
    feature_info3.enabled = false;
    EXPECT_FALSE(feature_info1 == feature_info3);
    feature_info3.enabled = true;

    //test visible
    feature_info3.visible = true;
    EXPECT_FALSE(feature_info1 == feature_info3);
    feature_info3.visible = false;

    //test uri
    feature_info3.uri = "uri2";
    EXPECT_FALSE(feature_info1 == feature_info3);
    feature_info3.uri = "uri";

    //test icon
    feature_info3.icon = "";
    EXPECT_FALSE(feature_info1 == feature_info3);
    feature_info3.icon = "icon";

    //test description
    feature_info3.description = "";
    EXPECT_FALSE(feature_info1 == feature_info3);
    feature_info3.description = "description";

    //test tags
    std::string tag = feature_info3.tags[0];
    feature_info3.tags[0] = "NOTAG";
    EXPECT_FALSE(feature_info1 == feature_info3);
    feature_info3.tags[0] = tag;

    //test tags
    std::string tag2 = "SOMETAG";
    feature_info3.tags.push_back(tag2);
    EXPECT_FALSE(feature_info1 == feature_info3);
    feature_info3.tags.erase(std::remove(feature_info3.tags.begin(), feature_info3.tags.end(), tag2), feature_info3.tags.end());

    //confirm that we have not made a mistake on the way and we always cleaned right
    //otherwise we risk that diff was not triggered by the tested element
    ASSERT_TRUE(feature_info1 == feature_info3);

}

//Compares 2 resource infos (from Expiry Notification)
void CompareResourceInfo(fsm::ExpiryNotificationSignal::ResourceInfo& resource1, fsm::ExpiryNotificationSignal::ResourceInfo& resource2)
{
    EXPECT_TRUE(resource1.name.compare(resource2.name)==0);
    EXPECT_TRUE(resource1.reconnect == resource2.reconnect);
    EXPECT_TRUE(resource1.parameters.compare(resource2.parameters)==0);
}


//Tests ExpirySignal, takes in a list of resources and a file name to store the signal.
//Resource list is used to create a ExpiryNotificationSignal,
//encode and decode it and make sure the output matches the input
void TestExpirySignal(std::vector<fsm::ExpiryNotificationSignal::ResourceInfo>& resource_list,
                      std::string filename)
{
    // setup signal
    fsm::CCMTransactionId transaction_id;
    std::shared_ptr<fsm::ExpiryNotificationSignal> expiry = std::make_shared<fsm::ExpiryNotificationSignal>(transaction_id, 1);


    //add all the resources from the resource list
    for (int i=0; i<resource_list.size(); i++)
    {
        expiry->AddResource(resource_list[i]);
    }

    // encode
    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(expiry);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile(filename, (char*)buffer->data(), buffer->size());

    // decode
    fsm::SetCertsForDecode();

    //register factory for ExpiryNotification
    fsm::SignalFactory::RegisterCcmFactory(fsm::ExpiryNotificationSignal::oid_, fsm::ExpiryNotificationSignal::CreateExpiryNotificationSignal);

    std::shared_ptr<fsm::Signal> decoded_signal = fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
    {
        FAIL();
    }

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(fsm::ExpiryNotificationSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , fsm::kExpiryNotificationSignal);

    std::shared_ptr<fsm::ExpiryNotificationSignal> typed_decoded_signal =
            std::static_pointer_cast<fsm::ExpiryNotificationSignal>(decoded_signal);


    //retrieve list of resources
    std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> received_resources= typed_decoded_signal->GetResources();
    bool same_number_of_resources = received_resources.size() == resource_list.size();
    EXPECT_TRUE(same_number_of_resources);

    if (same_number_of_resources )
    {
        //loop through all the resources, make sure they match expectations
        for (int i=0; i<received_resources.size(); i++)
        {
            CompareResourceInfo(resource_list[i], received_resources[i] );
        }
    }
}


//test one resource case (mandatory fields only). Produces update on caraccess.
TEST (CCMSignalsTest, TestExpiryNotificationSignalOneResourceMandatoryFields)
{

    std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> resources_list;

    //define and add resource, only mandatory fields
    fsm::ExpiryNotificationSignal::ResourceInfo resource_info;
    resource_info.name = "caraccess";
    resource_info.reconnect = true;
    resources_list.push_back(resource_info);
    TestExpirySignal(resources_list, std::string("expiry_signal_oneresource_mandatoryfields_ca.bin"));
}

//like above, produces update on Features (for cloud mock testing)
TEST (CCMSignalsTest, TestExpiryNotificationSignalOneResourceMandatoryFieldsFeatures)
{

    std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> resources_list;

    //define and add resource, only mandatory fields
    fsm::ExpiryNotificationSignal::ResourceInfo resource_info;
    resource_info.name = "Features";
    resource_info.reconnect = true;
    resources_list.push_back(resource_info);
    TestExpirySignal(resources_list, std::string("expiry_signal_oneresource_mandatoryfields_features.bin"));
}

//test one resource case (all fields)
TEST (CCMSignalsTest, TestExpiryNotificationSignalOneResourceAllFields)
{
    std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> resources_list;

    //define and add resource, all fields
    fsm::ExpiryNotificationSignal::ResourceInfo resource_info;
    resource_info.name = "caraccess";
    resource_info.parameters = "parameters";
    resource_info.reconnect = false;
    resources_list.push_back(resource_info);
    TestExpirySignal(resources_list, std::string("expiry_signal_oneresource_allfields.bin"));
}


//test expiry for car access and feature list (useful for cloud mock testing)
TEST (CCMSignalsTest, TestExpiryNotificationsCaAndFeatures)
{

    std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> resources_list;

    fsm::ExpiryNotificationSignal::ResourceInfo features_ca_resource_info;
    features_ca_resource_info.name = "caraccess";
    features_ca_resource_info.reconnect = true;

    fsm::ExpiryNotificationSignal::ResourceInfo ca_resource_info;
    ca_resource_info.name = "Features";
    ca_resource_info.reconnect = true;

    resources_list.push_back(features_ca_resource_info);
    resources_list.push_back(ca_resource_info);


    TestExpirySignal(resources_list, std::string("expiry_signal_ca_features.bin"));

}


//test multiple resource case with variable number of parameters
TEST (CCMSignalsTest, TestExpiryNotificationsSignalMultipleResources)
{

    std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> resources_list;

    for (int i=0; i<10; i++)
    {
        //define and add feature, introduce some variability in values for different features
        fsm::ExpiryNotificationSignal::ResourceInfo resource_info;
        std::string postfix = std::to_string(i);
        resource_info.name = "name" + postfix;
        resource_info.reconnect = (i % 3 == 0);
        if (i % 4 != 0)
        {
            resource_info.parameters = "parameters" + postfix;

        }

        resources_list.push_back(resource_info);
    }

    TestExpirySignal(resources_list, std::string("expiry_signal_multipleresources.bin"));

}

/**
 * \brief Test encode/decode BasicCarControlPayload
 */
TEST (CCMSignalsTest, EncodeDecodeBasicCarControlPayload)
{
    std::string uri = "foo uri";

    std::shared_ptr<BasicCarControlPayload> basic_car_control =
        std::make_shared<BasicCarControlPayload>();

    basic_car_control->SetAddressType(BasicCarControlPayload::kUri);
    basic_car_control->SetAddressUri(uri);

    fsm::PayloadInterface::PayloadIdentifier basic_car_control_identifier =
        basic_car_control->GetIdentifier().payload_identifier;

    fsm::SignalFactory::RegisterPayloadFactory<BasicCarControlPayload>(basic_car_control_identifier);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(basic_car_control);

    if (!buffer)
    {
        FAIL();
    }

    fsm::SetCertsForDecode();

    std::shared_ptr<fsm::PayloadInterface> decoded_payload =
        fsm::SignalFactory::DecodePayload(buffer->data(),
                                          buffer->size(),
                                          &basic_car_control_identifier);

    if (!decoded_payload)
    {
        FAIL();
    }

    std::shared_ptr<BasicCarControlPayload> basic_car_control_2 =
        std::dynamic_pointer_cast<BasicCarControlPayload>(decoded_payload);

    if (!basic_car_control_2)
    {
        FAIL();
    }

    EXPECT_EQ(basic_car_control_2->GetAddressType(), BasicCarControlPayload::kUri);
    std::string decoded_uri;
    EXPECT_TRUE(basic_car_control_2->GetAddressUri(decoded_uri));
    EXPECT_EQ(decoded_uri, uri);
}

/**
 * \brief Test encode/decode BasicCarControlPayload
 */
TEST (CCMSignalsTest, EncodeDecodeBasicCarControlPayloadAsCcm)
{
    std::string uri = "foo uri";

    std::shared_ptr<BasicCarControlPayload> basic_car_control =
        std::make_shared<BasicCarControlPayload>();

    fsm::PayloadInterface::PayloadIdentifier basic_car_control_identifier =
        basic_car_control->GetIdentifier().payload_identifier;

    fsm::SignalFactory::RegisterPayloadFactory<BasicCarControlPayload>(basic_car_control_identifier.oid);


    basic_car_control->SetAddressType(BasicCarControlPayload::kUri);
    basic_car_control->SetAddressUri(uri);

    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<fsm::CcmSignal> ccm = std::make_shared<fsm::CcmSignal>(transaction_id);

    ccm->SetPayload(basic_car_control);
    ccm->SetRecipients(users);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(ccm);

    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    fsm::SetCertsForDecode();

    std::shared_ptr<fsm::Signal> decoded_signal =
        fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    std::shared_ptr<BasicCarControlPayload> basic_car_control_2 =
        fsm::SignalFactory::GetSignalPayload<BasicCarControlPayload>(decoded_signal);

    if (!basic_car_control_2)
        FAIL();

    EXPECT_EQ(basic_car_control_2->GetAddressType(), BasicCarControlPayload::kUri);
    std::string decoded_uri;
    EXPECT_TRUE(basic_car_control_2->GetAddressUri(decoded_uri));
    EXPECT_EQ(decoded_uri, uri);
}

/**
 * \brief Test encode/decode empty BasicCarControlPayload
 */
TEST (CCMSignalsTest, EncodeDecodeBasicCarControlPayloadEmpty)
{
    std::shared_ptr<BasicCarControlPayload> basic_car_control =
        std::make_shared<BasicCarControlPayload>();

    fsm::PayloadInterface::PayloadIdentifier basic_car_control_identifier =
        basic_car_control->GetIdentifier().payload_identifier;

    fsm::SignalFactory::RegisterPayloadFactory<BasicCarControlPayload>(basic_car_control_identifier);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(basic_car_control);
    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    fsm::SetCertsForDecode();

    std::shared_ptr<fsm::PayloadInterface> decoded_payload =
        fsm::SignalFactory::DecodePayload(buffer->data(),
                                          buffer->size(),
                                          &basic_car_control_identifier);
    ASSERT_NE(decoded_payload, nullptr);

    std::shared_ptr<BasicCarControlPayload> basic_car_control_2 =
        std::dynamic_pointer_cast<BasicCarControlPayload>(decoded_payload);
    ASSERT_NE(basic_car_control_2, nullptr);

    ASSERT_EQ(basic_car_control_2->GetAddressType(), BasicCarControlPayload::kNone);
    std::string decoded_uri;
    EXPECT_FALSE(basic_car_control_2->GetAddressUri(decoded_uri));
    EXPECT_TRUE(decoded_uri.empty());
}

/**
 * \brief Test encode/decode BasicCarControlPayload with specified encoding
 */
static void testEncodeDecodeBasicCarControlPayload(fs_Encoding encoding)
{
    std::string uri = "foo uri";

    std::shared_ptr<BasicCarControlPayload> basic_car_control =
        std::make_shared<BasicCarControlPayload>(encoding);

    basic_car_control->SetAddressType(BasicCarControlPayload::kUri);
    basic_car_control->SetAddressUri(uri);

    fsm::PayloadInterface::PayloadIdentifier basic_car_control_identifier =
        basic_car_control->GetIdentifier().payload_identifier;

    fsm::SignalFactory::RegisterPayloadFactory<BasicCarControlPayload>(basic_car_control_identifier);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(basic_car_control, &encoding);
    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    fsm::SetCertsForDecode();

    std::shared_ptr<fsm::PayloadInterface> decoded_payload =
        fsm::SignalFactory::DecodePayload(buffer->data(),
                                          buffer->size(),
                                          &basic_car_control_identifier,
                                          &encoding);
    ASSERT_NE(decoded_payload, nullptr);

    std::shared_ptr<BasicCarControlPayload> basic_car_control_2 =
        std::dynamic_pointer_cast<BasicCarControlPayload>(decoded_payload);
    ASSERT_NE(basic_car_control_2, nullptr);

    EXPECT_EQ(basic_car_control_2->GetAddressType(), BasicCarControlPayload::kUri);
    std::string decoded_uri;
    EXPECT_TRUE(basic_car_control_2->GetAddressUri(decoded_uri));
    EXPECT_EQ(decoded_uri, uri);
}

/**
 * \brief Test encode/decode BasicCarControlPayload with BER encoding
 */
TEST (CCMSignalsTest, EncodeDecodeBasicCarControlPayloadWithBER)
{
    testEncodeDecodeBasicCarControlPayload(FS_ENCODING_BER);
}

/**
 * \brief Test encode/decode BasicCarControlPayload with DER encoding
 */
TEST (CCMSignalsTest, EncodeDecodeBasicCarControlPayloadWithDER)
{
    testEncodeDecodeBasicCarControlPayload(FS_ENCODING_DER);
}

/**
 * \brief Test encode/decode BasicCarControlPayload with PER encoding
 */
TEST (CCMSignalsTest, EncodeDecodeBasicCarControlPayloadWithPER)
{
    testEncodeDecodeBasicCarControlPayload(FS_ENCODING_PER);
}

/**
 * \brief Test encode/decode BasicCarControlPayload with XER encoding
 */
TEST (CCMSignalsTest, EncodeDecodeBasicCarControlPayloadWithXER)
{
    testEncodeDecodeBasicCarControlPayload(FS_ENCODING_XER);
}

/**
 * \brief Test encode BasicCarControlPayload with unsupported encoding
 */
TEST (CCMSignalsTest, EncodeBasicCarControlPayloadUnsupportedEncoding)
{
    std::string uri = "foo uri";
    fs_Encoding encoding = FS_ENCODING_XML;

    std::shared_ptr<BasicCarControlPayload> basic_car_control =
        std::make_shared<BasicCarControlPayload>(encoding);

    basic_car_control->SetAddressType(BasicCarControlPayload::kUri);
    basic_car_control->SetAddressUri(uri);

    fsm::PayloadInterface::PayloadIdentifier basic_car_control_identifier =
        basic_car_control->GetIdentifier().payload_identifier;

    fsm::SignalFactory::RegisterPayloadFactory<BasicCarControlPayload>(basic_car_control_identifier);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(basic_car_control, &encoding);
    EXPECT_EQ(encoding, FS_ENCODING_UNDEFINED);
    EXPECT_EQ(buffer, nullptr);
}

/**
 * \brief Test decode BasicCarControlPayload with unsupported encoding
 */
TEST (CCMSignalsTest, DecodeBasicCarControlPayloadUnsupportedEncoding)
{
    std::string uri = "foo uri";
    fs_Encoding encoding = FS_ENCODING_UPER;

    std::shared_ptr<BasicCarControlPayload> basic_car_control =
        std::make_shared<BasicCarControlPayload>(encoding);

    basic_car_control->SetAddressType(BasicCarControlPayload::kUri);
    basic_car_control->SetAddressUri(uri);

    fsm::PayloadInterface::PayloadIdentifier basic_car_control_identifier =
        basic_car_control->GetIdentifier().payload_identifier;

    fsm::SignalFactory::RegisterPayloadFactory<BasicCarControlPayload>(basic_car_control_identifier);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(basic_car_control, &encoding);
    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    fsm::SetCertsForDecode();

    encoding = FS_ENCODING_XML;

    std::shared_ptr<fsm::PayloadInterface> decoded_payload =
        fsm::SignalFactory::DecodePayload(buffer->data(),
                                          buffer->size(),
                                          &basic_car_control_identifier,
                                          &encoding);
    EXPECT_EQ(decoded_payload, nullptr);
}

/**
 * \brief Test decode BasicCarControlPayload with payload version mismatch
 */
TEST (CCMSignalsTest, DecodeBasicCarControlPayloadVersionMismatch)
{
    std::string uri = "foo uri";
    fs_Encoding encoding = FS_ENCODING_UPER;

    std::shared_ptr<BasicCarControlPayload> basic_car_control =
        std::make_shared<BasicCarControlPayload>(encoding);

    basic_car_control->SetAddressType(BasicCarControlPayload::kUri);
    basic_car_control->SetAddressUri(uri);

    fsm::PayloadInterface::PayloadIdentifier basic_car_control_identifier =
        basic_car_control->GetIdentifier().payload_identifier;

    fsm::SignalFactory::RegisterPayloadFactory<BasicCarControlPayload>(basic_car_control_identifier);

    fsm::SetCertsForEncode();

    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(basic_car_control, &encoding);
    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    fsm::SetCertsForDecode();

    fs_VersionInfo versionInfo;
    versionInfo.signal_flow_version = 2;
    versionInfo.schema_version = 2;
    versionInfo.preliminary_version = 20;

    std::shared_ptr<fsm::PayloadInterface> decoded_payload =
        fsm::SignalFactory::DecodePayload(buffer->data(),
                                          buffer->size(),
                                          &basic_car_control_identifier,
                                          &encoding,
                                          &versionInfo);
    EXPECT_EQ(decoded_payload, nullptr);
}
} // namespace

/** \}    end of addtogroup */
