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
 *  \file     unittest_signal.cc
 *  \brief    Foundation Services voc framework signals, Test Cases
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include "voc_framework/signals/assistance_call.h"
#include "voc_framework/signals/car_access_signal.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm_signal.h"
#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/xml_payload.h"
#include "voc_framework/transactions/transaction_id.h"

#include <stack>

#include <openssl/evp.h>

#include <gtest/gtest.h>

#include "certUtils.h"
#include "keystore_stub.h"
#include "usermanager_interface.h"
#include "utilities.h"

TEST(SignalsTest, GetCertificatesFromCCM)
{
    fsm::GenerateCerts();

    // First make and encode a signal

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

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    // verify that no certs are attached to CCM

    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_,
                                           fsm::CarAccessSignal::CreateCarAccessSignal);

    std::shared_ptr<fsm::Signal> decoded_signal =
        fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    ASSERT_NE(decoded_signal, nullptr);

    std::shared_ptr<fsm::CCMDecodable> ccm_decodable =
        std::dynamic_pointer_cast<fsm::CCMDecodable>(decoded_signal);

    ASSERT_NE(ccm_decodable, nullptr);

    STACK_OF(X509)* certificates = sk_X509_new_null();
    ASSERT_NE(certificates, nullptr);

    EXPECT_FALSE(ccm_decodable->GetCertificates(certificates));
    EXPECT_EQ(sk_X509_num(certificates), 0);

    sk_X509_pop_free(certificates, X509_free);
    certificates = nullptr;

    // Then add some certificates to the CCM

    unsigned char* cms_with_certs_buffer = nullptr;
    int num_bytes;

    BIO* data_bio = BIO_new_mem_buf(buffer->data(), buffer->size());

    ASSERT_NE(data_bio, nullptr);

    CMS_ContentInfo* signed_data = d2i_CMS_bio(data_bio, nullptr);

    ASSERT_NE(signed_data, nullptr);

    std::stack<std::pair<X509*, EVP_PKEY*>> certs;

    int kNumCerts = 10;

    for (int i = 0; i < kNumCerts; i++)
    {
        X509* cert = nullptr;
        EVP_PKEY* key = nullptr;
        GeneratePrivateKey(&key, fsm::kMinRSAKeySize);
        ASSERT_NE(key, nullptr);
        GenerateCertificate(&cert, key);
        ASSERT_NE(cert, nullptr);

        certs.push(std::make_pair(cert, key));

        ASSERT_EQ(CMS_add1_cert(signed_data, cert), 1);
    }

    BIO* signed_bio = BIO_new(BIO_s_mem());

    ASSERT_GT(i2d_CMS_bio(signed_bio, signed_data), 0);
    BIO_set_close(signed_bio, BIO_NOCLOSE);

    num_bytes = BIO_get_mem_data(signed_bio, &cms_with_certs_buffer);
    ASSERT_GT(num_bytes, 0);
    ASSERT_NE(cms_with_certs_buffer, nullptr);

    CMS_ContentInfo_free(signed_data);
    BIO_free(data_bio);
    BIO_free(signed_bio);


    // Finally, do the decode and verify certs are returned as expected

    decoded_signal = fsm::SignalFactory::DecodeCcm(cms_with_certs_buffer, num_bytes);

    ASSERT_NE(decoded_signal, nullptr);

    ccm_decodable = std::dynamic_pointer_cast<fsm::CCMDecodable>(decoded_signal);

    ASSERT_NE(ccm_decodable, nullptr);

    certificates = sk_X509_new_null();
    ASSERT_NE(certificates, nullptr);

    EXPECT_TRUE(ccm_decodable->GetCertificates(certificates));

    int num_certs = sk_X509_num(certificates);

    EXPECT_EQ(num_certs, kNumCerts);

    for (int i = 0; i < kNumCerts; i++)
    {
        std::pair<X509*, EVP_PKEY*> cert_key_pair = certs.top();

        bool found = false;

        for (int k = 0; k < num_certs; k++)
        {
            found = X509_cmp(cert_key_pair.first, sk_X509_value(certificates, k)) == 0;

            if (found)
            {
                break;
            }
        }

        EXPECT_TRUE(found);

        X509_free(cert_key_pair.first);
        EVP_PKEY_free(cert_key_pair.second);

        certs.pop();
    }

    sk_X509_pop_free(certificates, X509_free);
    certificates = nullptr;


    fsm::FreeCerts();
}

/**
 * \brief Test encode/decode empty (invalid) CAFunc002
 */
TEST (SignalsTest, EncodeDecodeCaFunc002Empty)
{

    fsm::GenerateCerts();
    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_,
                                           fsm::CarAccessSignal::CreateCarAccessSignal);

    fsm::CCMTransactionId transaction_id;

    fsm::CarAccessSignal* tmp = new fsm::CarAccessSignal(transaction_id, 1);
    std::shared_ptr<fsm::CarAccessSignal> signal = std::shared_ptr<fsm::CarAccessSignal>(tmp);

    signal->SetRecipients(fsm::users);

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_EQ(buffer, nullptr);

    fsm::FreeCerts();
}

/**
 * \brief Test encode/decode minimum valid CAFunc002
 */
TEST (SignalsTest, EncodeDecodeCaFunc002Minimum)
{
    fsm::GenerateCerts();
    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_,
                                           fsm::CarAccessSignal::CreateCarAccessSignal);

    fsm::CCMTransactionId transaction_id;

    fsm::CarAccessSignal* tmp = new fsm::CarAccessSignal(transaction_id, 1);
    std::shared_ptr<fsm::CarAccessSignal> signal = std::shared_ptr<fsm::CarAccessSignal>(tmp);

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

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    std::shared_ptr<fsm::Signal> decoded_signal =
        fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(fsm::CarAccessSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , fsm::Signal::kCarAccessSignal);

    std::shared_ptr<fsm::CarAccessSignal> typed_decoded_signal =
        std::static_pointer_cast<fsm::CarAccessSignal>(decoded_signal);

    std::string decoded_address_uri;

    EXPECT_TRUE(typed_decoded_signal->GetAddressUri(decoded_address_uri));
    EXPECT_EQ(decoded_address_uri.compare("/this_is_a_test_uri"), 0);
    EXPECT_EQ(typed_decoded_signal->GetCatalogueUplink().compare("catalogue_uplink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetCatalogueDownlink().compare("catalogue_downlink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetDelegateUplink().compare("delegate_uplink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetDelegateDownlink().compare("delegate_downlink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetDevicePairingUplink().compare("device_pairing_uplink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetDevicePairingDownlink().compare("device_pairing_downlink"), 0);

    std::shared_ptr<std::vector<fsm::CarAccessSignal::MqttTopicInfo>> topic_infos;

    topic_infos = typed_decoded_signal->GetUplinkTopics();
    if (!topic_infos)
        FAIL();
    ASSERT_EQ(topic_infos->size(), 1);
    EXPECT_EQ((*topic_infos)[0].topic.compare("uplink topic"), 0);
    EXPECT_EQ((*topic_infos)[0].priority, 1);
    EXPECT_EQ((*topic_infos)[0].services.size(), 0);

    topic_infos = typed_decoded_signal->GetDownlinkTopics();
    if (!topic_infos)
        FAIL();
    ASSERT_EQ(topic_infos->size(), 1);
    EXPECT_EQ((*topic_infos)[0].topic.compare("downlink topic"), 0);
    EXPECT_EQ((*topic_infos)[0].priority, 1);
    EXPECT_EQ((*topic_infos)[0].services.size(), 0);


    fsm::FreeCerts();
}

/**
 * \brief Test encode/decode realistic valid CAFunc002
 */
TEST (SignalsTest, EncodeDecodeCaFunc002Realistic)
{
    const std::string bcc_func_001_oid = "1.3.6.1.4.1.37916.3.6.2.0.0.1";
    const std::string bcc_func_002_oid = "1.3.6.1.4.1.37916.3.6.2.0.0.2";
    const std::string bcc_cl_001_oid = "1.3.6.1.4.1.37916.3.6.3.0.0.1";
    const std::string bcc_cl_002_oid = "1.3.6.1.4.1.37916.3.6.3.0.0.2";
    const std::string bcc_cl_010_oid = "1.3.6.1.4.1.37916.3.6.3.0.1.0";
    const std::string bcc_cl_011_oid = "1.3.6.1.4.1.37916.3.6.3.0.1.1";
    fsm::GenerateCerts();
    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_,
                                           fsm::CarAccessSignal::CreateCarAccessSignal);

    fsm::CCMTransactionId transaction_id;

    fsm::CarAccessSignal* tmp = new fsm::CarAccessSignal(transaction_id, 1);
    std::shared_ptr<fsm::CarAccessSignal> signal = std::shared_ptr<fsm::CarAccessSignal>(tmp);

    signal->SetAdressUri("/this_is_a_test_uri");
    signal->SetCatalogueUplink("catalogue_uplink");
    signal->SetCatalogueDownlink("catalogue_downlink");
    signal->SetDelegateUplink("delegate_uplink");
    signal->SetDelegateDownlink("delegate_downlink");
    signal->SetDevicePairingUplink("device_pairing_uplink");
    signal->SetDevicePairingDownlink("device_pairing_downlink");

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_1;
    uplink_topic_1.topic = "uplink topic_high";
    uplink_topic_1.priority = 80;
    fsm::CarAccessSignal::MqttServiceInfo service_info;
    service_info.oid = bcc_cl_002_oid;
    uplink_topic_1.services.push_back(service_info);
    service_info.oid = bcc_cl_011_oid;
    uplink_topic_1.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_1);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_2;
    uplink_topic_2.topic = "uplink topic_normal";
    uplink_topic_2.priority = 50;
    // services are optional, dont set any for normal
    signal->AddUplinkTopic(uplink_topic_2);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_3;
    uplink_topic_3.topic = "uplink topic_low";
    uplink_topic_3.priority = 20;
    service_info.oid = bcc_func_002_oid;
    uplink_topic_3.services.push_back(service_info);
    service_info.oid = bcc_cl_002_oid;
    uplink_topic_3.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_3);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_4;
    uplink_topic_4.topic = "uplink topic_high";
    uplink_topic_4.priority = 90;
    service_info.oid = bcc_cl_002_oid;
    uplink_topic_4.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_4);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_1;
    downlink_topic_1.topic = "downlink topic_high";
    downlink_topic_1.priority = 80;
    service_info.oid = bcc_cl_001_oid;
    downlink_topic_1.services.push_back(service_info);
    service_info.oid = bcc_cl_010_oid;
    downlink_topic_1.services.push_back(service_info);
    signal->AddDownlinkTopic(downlink_topic_1);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_2;
    downlink_topic_2.topic = "downlink topic_normal";
    downlink_topic_2.priority = 50;
    // services are optional, dont set any for normal
    signal->AddDownlinkTopic(downlink_topic_2);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_3;
    downlink_topic_3.topic = "downlink topic_low";
    downlink_topic_3.priority = 20;
    service_info.oid = bcc_func_001_oid;
    downlink_topic_3.services.push_back(service_info);
    signal->AddDownlinkTopic(downlink_topic_3);

    signal->SetRecipients(fsm::users);

    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    std::shared_ptr<fsm::Signal> decoded_signal =
        fsm::SignalFactory::DecodeCcm(buffer->data(), buffer->size());

    if (!decoded_signal)
        FAIL();

    ASSERT_EQ(typeid(*decoded_signal).hash_code(), typeid(fsm::CarAccessSignal).hash_code());
    ASSERT_EQ(decoded_signal->GetSignalType() , fsm::Signal::kCarAccessSignal);

    std::shared_ptr<fsm::CarAccessSignal> typed_decoded_signal =
        std::static_pointer_cast<fsm::CarAccessSignal>(decoded_signal);

    std::string decoded_address_uri;

    EXPECT_TRUE(typed_decoded_signal->GetAddressUri(decoded_address_uri));
    EXPECT_EQ(decoded_address_uri.compare("/this_is_a_test_uri"), 0);
    EXPECT_EQ(typed_decoded_signal->GetCatalogueUplink().compare("catalogue_uplink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetCatalogueDownlink().compare("catalogue_downlink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetDelegateUplink().compare("delegate_uplink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetDelegateDownlink().compare("delegate_downlink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetDevicePairingUplink().compare("device_pairing_uplink"), 0);
    EXPECT_EQ(typed_decoded_signal->GetDevicePairingDownlink().compare("device_pairing_downlink"), 0);

    std::shared_ptr<std::vector<fsm::CarAccessSignal::MqttTopicInfo>> topic_infos;

    topic_infos = typed_decoded_signal->GetUplinkTopics();
    if (!topic_infos)
        FAIL();
    ASSERT_EQ(topic_infos->size(), 4);
    EXPECT_EQ((*topic_infos)[0].topic.compare("uplink topic_high"), 0);
    EXPECT_EQ((*topic_infos)[0].priority, 80);
    EXPECT_EQ((*topic_infos)[0].services.size(), 2);
    EXPECT_EQ((*topic_infos)[1].topic.compare("uplink topic_normal"), 0);
    EXPECT_EQ((*topic_infos)[1].priority, 50);
    EXPECT_EQ((*topic_infos)[1].services.size(), 0);
    EXPECT_EQ((*topic_infos)[2].topic.compare("uplink topic_low"), 0);
    EXPECT_EQ((*topic_infos)[2].priority, 20);
    EXPECT_EQ((*topic_infos)[2].services.size(), 2);

    topic_infos = typed_decoded_signal->GetDownlinkTopics();
    if (!topic_infos)
        FAIL();
    ASSERT_EQ(topic_infos->size(), 3);
    EXPECT_EQ((*topic_infos)[0].topic.compare("downlink topic_high"), 0);
    EXPECT_EQ((*topic_infos)[0].priority, 80);
    EXPECT_EQ((*topic_infos)[0].services.size(), 2);
    EXPECT_EQ((*topic_infos)[1].topic.compare("downlink topic_normal"), 0);
    EXPECT_EQ((*topic_infos)[1].priority, 50);
    EXPECT_EQ((*topic_infos)[1].services.size(), 0);
    EXPECT_EQ((*topic_infos)[2].topic.compare("downlink topic_low"), 0);
    EXPECT_EQ((*topic_infos)[2].priority, 20);
    EXPECT_EQ((*topic_infos)[2].services.size(), 1);

    //TODO: verify the rest

    fsm::FreeCerts();
}


/**
 * \brief Test encode/decode EncodeDecodeXML
 *        This tests attempts to push a string of data through the XML
 *        encoding pipeline (put into XMLPaylod, encode using
 *        the codec, then decode, extract the data and compare with
 *        the original string. Plain XML just gets copied in all steps
 *        so input and output should be identical.
 */
TEST (SignalsTest, EncodeDecodeXML)
{
    std::string xml_content = "<some_tag> sdsddss <some_tag>";

    std::shared_ptr<fsm::XmlPayload> xml_payload =
        std::make_shared<fsm::XmlPayload>();

    fsm::PayloadInterface::PayloadIdentifier xml_identifier =
        xml_payload->GetIdentifier().payload_identifier;

    xml_payload->SetXmlData(xml_content);

    fsm::SignalFactory::RegisterPayloadFactory<fsm::XmlPayload>(xml_identifier);

    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(xml_payload);

    if (!buffer)
    {
        FAIL();
    }

    std::shared_ptr<fsm::PayloadInterface> decoded_payload =
        fsm::SignalFactory::DecodePayload(buffer->data(),
                                          buffer->size(),
                                          &xml_identifier);

    if (!decoded_payload)
    {
        FAIL();
    }

    std::shared_ptr<fsm::XmlPayload> xml_payload_2 =
        std::dynamic_pointer_cast<fsm::XmlPayload>(decoded_payload);

    if (!xml_payload_2)
    {
        FAIL();
    }

    EXPECT_EQ(xml_payload_2->GetXmlData(), xml_content);
}


/**
 * \brief Test encode/decode empty AssistanceCallPayload
 */
TEST (CCMSignalsTest, EncodeDecodAssistanceCallPayloadEmpty)
{

    fsm::GenerateCerts();

    //create assistance call payload object
    std::shared_ptr<fsm::AssistanceCallPayload> assistance_call =
        std::make_shared<fsm::AssistanceCallPayload>();

    fsm::PayloadInterface::PayloadIdentifier assistance_call_identifier =
        assistance_call->GetIdentifier().payload_identifier;

    //register payload factory, will be needed for decoding
    fsm::SignalFactory::RegisterPayloadFactory<fsm::AssistanceCallPayload>(assistance_call_identifier);

    //encode the payload without setting any values. Empty payload is legitimate according to the spec
    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(assistance_call);

    //Make sure we got something encoded, so buffer not nullptr or empty
    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    //decode the payload
    std::shared_ptr<fsm::PayloadInterface> decoded_payload =
        fsm::SignalFactory::DecodePayload(buffer->data(),
                                          buffer->size(),
                                          &assistance_call_identifier);
    //make sure we got something
    ASSERT_NE(decoded_payload, nullptr);

    //cast the payload to AssistanceCallPayload class
    std::shared_ptr<fsm::AssistanceCallPayload> assistance_call_2 =
        std::dynamic_pointer_cast<fsm::AssistanceCallPayload>(decoded_payload);

    //make sure cast succeeded
    ASSERT_NE(assistance_call_2, nullptr);

    //make sure we did not get any CallCenterAddress
    std::string decoded_address;
    EXPECT_FALSE(assistance_call_2->GetCallCenterAddress(decoded_address));
    EXPECT_EQ(assistance_call_2->GetCallCenterAddressType(), fsm::AssistanceCallPayload::kNone);
    EXPECT_TRUE(decoded_address.empty());

    //make sure we did not get any Thiz
    std::string decoded_thiz;
    EXPECT_FALSE(assistance_call_2->GetThizAddress(decoded_address));
    EXPECT_EQ(assistance_call_2->GetThizAddressType(), fsm::AssistanceCallPayload::kNone);
    EXPECT_TRUE(decoded_thiz.empty());

    fsm::FreeCerts();
}

/**
 * \brief Function for testing different correct variants of Resources.
 *
 * \param[in] cc_type CallCenterAddress type
 * \param[in] cc_address CallCenterAddress value
 * \param[in] thiz_type Thiz type
 * \param[in] thiz_address Thiz value
 */
void TestAssistanceCallResources(fsm::AssistanceCallPayload::AddressType cc_type, std::string cc_address,
                                 fsm::AssistanceCallPayload::AddressType thiz_type, std::string thiz_address)
{
    fsm::GenerateCerts();

    //create assistance call object
    std::shared_ptr<fsm::AssistanceCallPayload> assistance_call =
        std::make_shared<fsm::AssistanceCallPayload>();

    fsm::PayloadInterface::PayloadIdentifier assistance_call_identifier =
        assistance_call->GetIdentifier().payload_identifier;

    //register factory for decoding
    fsm::SignalFactory::RegisterPayloadFactory<fsm::AssistanceCallPayload>(assistance_call_identifier);

    //set requested values for call center and thiz
    assistance_call->SetCallCenterAddress(cc_type, cc_address);
    assistance_call->SetThizAddress(thiz_type, thiz_address);

    //Encode the signal
    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(assistance_call);

    //make sure something got encoded
    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    //decode the signal
    std::shared_ptr<fsm::PayloadInterface> decoded_payload =
        fsm::SignalFactory::DecodePayload(buffer->data(),
                                          buffer->size(),
                                          &assistance_call_identifier);
    //make sure something got decoded
    ASSERT_NE(decoded_payload, nullptr);

    //cast the payload to assistance call
    std::shared_ptr<fsm::AssistanceCallPayload> assistance_call_2 =
        std::dynamic_pointer_cast<fsm::AssistanceCallPayload>(decoded_payload);
    ASSERT_NE(assistance_call_2, nullptr);

    //make sure we got expected CallCenterAddress
    std::string decoded_address;
    EXPECT_TRUE(assistance_call_2->GetCallCenterAddress(decoded_address));
    EXPECT_EQ(assistance_call_2->GetCallCenterAddressType(), cc_type);
    EXPECT_EQ(decoded_address, cc_address);

    //make cure we got expected Thiz address
    std::string decoded_thiz;
    EXPECT_TRUE(assistance_call_2->GetThizAddress(decoded_thiz));
    EXPECT_EQ(assistance_call_2->GetThizAddressType(), thiz_type);
    EXPECT_EQ(decoded_thiz, thiz_address);

    fsm::FreeCerts();
}

/**
 * \brief Test encode/decode AssistanceCallPayload with resources as uri
 */
TEST (CCMSignalsTest, EncodeDecodAssistanceCallResourceURI)
{
    //test both fields as uri
    TestAssistanceCallResources(fsm::AssistanceCallPayload::kUri, "Some uri",
                                fsm::AssistanceCallPayload::kUri, "Other uri");
}


/**
 * \brief Test encode/decode AssistanceCallPayload with resources as instance id
 */
TEST (CCMSignalsTest, EncodeDecodAssistanceCallResourceInstanceId)
{
    //test both fields as instanceId
    TestAssistanceCallResources(fsm::AssistanceCallPayload::kInstanceId, "1234567890123456",
                                fsm::AssistanceCallPayload::kInstanceId, "1234567890123457");
}


/**
 * \brief Test encode/decode AssistanceCallPayload with mix resource encodings
 *        one as instance id and one as URI
 */
TEST (CCMSignalsTest, EncodeDecodAssistanceCallResourceUriInstanceId)
{
    //test a mix of uri and instanceId
    TestAssistanceCallResources(fsm::AssistanceCallPayload::kUri, "someuri",
                                fsm::AssistanceCallPayload::kInstanceId, "1234567890123457");
    TestAssistanceCallResources(fsm::AssistanceCallPayload::kInstanceId, "1234567890123456",
                                fsm::AssistanceCallPayload::kUri, "otheruri");
}

/** \}    end of addtogroup */
