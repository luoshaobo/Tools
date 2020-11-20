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
 *  \file     generate_cloudmock_content.cc
 *  \brief    Generator for cloud mock ccm content
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#include <gtest/gtest.h>
#include <string>

#include "test_classes.h"

#define VOC_CCM_TEST 1

#include "signals/basic_car_control_signal.h"
#include "voc_framework/signals/car_access_signal.h"
#include "signals/signal_types.h"

#include "fscfg/bus_names.h"
#include "fscfgd/expiry_notification_signal.h"
#include "voc_framework/signals/assistance_call.h"
#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm.h"


namespace voc_ccm_test
{

using namespace volvo_on_call;
using namespace voc_test_classes;

TEST (GenerateCloudmockContent, GenerateBccFunc002)
{
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<BasicCarControlSignal> signal = BasicCarControlSignal::CreateBasicCarControlSignalFromData
        (transaction_id, 1);

    std::string uri = "yet another random text";
    signal->SetCarLocatorSupport();
    signal->SetAdressUri(uri);

    fsm::SetCertsForEncode();

    signal->SetRecipients(users);
    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);

    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("basic_car_control_signal.bin",
                    (char*)buffer->data(),
                    buffer->size());
}

TEST (GenerateCloudmockContent, GenerateCaFunc002)
{
    const std::string bcc_func_001_oid = "1.3.6.1.4.1.37916.3.6.2.0.0.1";
    const std::string bcc_func_002_oid = "1.3.6.1.4.1.37916.3.6.2.0.0.2";
    const std::string bcc_cl_001_oid = "1.3.6.1.4.1.37916.3.6.3.0.0.1";
    const std::string bcc_cl_002_oid = "1.3.6.1.4.1.37916.3.6.3.0.0.2";
    const std::string bcc_cl_010_oid = "1.3.6.1.4.1.37916.3.6.3.0.1.0";
    const std::string bcc_cl_011_oid = "1.3.6.1.4.1.37916.3.6.3.0.1.1";

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

    fsm::SetCertsForEncode();

    signal->SetRecipients(users);
    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("car_access_signal_all_topics.bin", (char*)buffer->data(), buffer->size());
}

TEST (GenerateCloudmockContent, GenerateCaFunc002_2)
{
    const std::string bcc_func_001_oid = "1.3.6.1.4.1.37916.3.6.2.0.0.1";
    const std::string bcc_func_002_oid = "1.3.6.1.4.1.37916.3.6.2.0.0.2";
    const std::string bcc_cl_001_oid = "1.3.6.1.4.1.37916.3.6.3.0.0.1";
    const std::string bcc_cl_002_oid = "1.3.6.1.4.1.37916.3.6.3.0.0.2";
    const std::string bcc_cl_010_oid = "1.3.6.1.4.1.37916.3.6.3.0.1.0";
    const std::string bcc_cl_011_oid = "1.3.6.1.4.1.37916.3.6.3.0.1.1";


    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_,
                                           fsm::CarAccessSignal::CreateCarAccessSignal);

    fsm::CCMTransactionId transaction_id;

    fsm::CarAccessSignal* tmp = new fsm::CarAccessSignal(transaction_id, 1);
    std::shared_ptr<fsm::CarAccessSignal> signal = std::shared_ptr<fsm::CarAccessSignal>(tmp);

    signal->SetAdressUri("/this_is_a_test_uri");
    signal->SetCatalogueUplink("catalogue_uplink_2");
    signal->SetCatalogueDownlink("catalogue_downlink_2");
    signal->SetDelegateUplink("delegate_uplink_2");
    signal->SetDelegateDownlink("delegate_downlink_2");
    signal->SetDevicePairingUplink("device_pairing_uplink_2");
    signal->SetDevicePairingDownlink("device_pairing_downlink_2");

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_1;
    uplink_topic_1.topic = "uplink_topic_high_2";
    uplink_topic_1.priority = 80;
    fsm::CarAccessSignal::MqttServiceInfo service_info;
    service_info.oid = bcc_cl_002_oid;
    uplink_topic_1.services.push_back(service_info);
    service_info.oid = bcc_cl_011_oid;
    uplink_topic_1.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_1);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_2;
    uplink_topic_2.topic = "uplink_topic_normal_2";
    uplink_topic_2.priority = 50;
    // services are optional, dont set any for normal
    signal->AddUplinkTopic(uplink_topic_2);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_3;
    uplink_topic_3.topic = "uplink_topic_low_2";
    uplink_topic_3.priority = 20;
    service_info.oid = bcc_func_002_oid;
    uplink_topic_3.services.push_back(service_info);
    service_info.oid = bcc_cl_002_oid;
    uplink_topic_3.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_3);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_4;
    uplink_topic_4.topic = "uplink_topic_high_2";
    uplink_topic_4.priority = 90;
    service_info.oid = bcc_cl_002_oid;
    uplink_topic_4.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_4);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_1;
    downlink_topic_1.topic = "downlink_topic_high_2";
    downlink_topic_1.priority = 80;
    service_info.oid = bcc_cl_001_oid;
    downlink_topic_1.services.push_back(service_info);
    service_info.oid = bcc_cl_010_oid;
    downlink_topic_1.services.push_back(service_info);
    signal->AddDownlinkTopic(downlink_topic_1);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_2;
    downlink_topic_2.topic = "downlink_topic_normal_2";
    downlink_topic_2.priority = 50;
    // services are optional, dont set any for normal
    signal->AddDownlinkTopic(downlink_topic_2);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_3;
    downlink_topic_3.topic = "downlink_topic_low_2";
    downlink_topic_3.priority = 20;
    service_info.oid = bcc_func_001_oid;
    downlink_topic_3.services.push_back(service_info);
    signal->AddDownlinkTopic(downlink_topic_3);

    fsm::SetCertsForEncode();

    signal->SetRecipients(users);
    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(signal);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("car_access_signal_all_topics_2.bin", (char*)buffer->data(), buffer->size());
}


//Encodes ExpirySignal, takes in a list of resources and a file name to store the signal.
//Resource list is used to create a ExpiryNotificationSignal,
void EncodeExpirySignal(std::vector<fsm::ExpiryNotificationSignal::ResourceInfo>& resource_list,
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

    expiry->SetRecipients(users);
    std::shared_ptr<std::vector<unsigned char>> buffer = fsm::SignalFactory::Encode(expiry);
    ASSERT_FALSE(buffer == nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile(filename, (char*)buffer->data(), buffer->size());
}



//Generate expiry notification for CA
TEST (GenerateCloudmockContent, GenerateExpiryOnCarAccess)
{
    std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> resources_list;

    //define and add resource, only mandatory fields
    fsm::ExpiryNotificationSignal::ResourceInfo resource_info;
    resource_info.name = std::string(fsm::fscfg_kCarAccessResource);
    resource_info.reconnect = true;
    resources_list.push_back(resource_info);
    EncodeExpirySignal(resources_list, std::string("expiry_signal_CA_reconnect.bin"));
}

//Generate expiry notification for Features
TEST (GenerateCloudmockContent, GenerateExpiryOnFeatures)
{
    std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> resources_list;

    //define and add resource, only mandatory fields
    fsm::ExpiryNotificationSignal::ResourceInfo resource_info;
    resource_info.name = std::string(fsm::fscfg_kFeaturesResource);
    resource_info.reconnect = true;
    resources_list.push_back(resource_info);
    EncodeExpirySignal(resources_list, std::string("expiry_signal_Features_reconnect.bin"));
}

//Generate expiry notification for Features and CA
TEST (GenerateCloudmockContent, GenerateExpiryOnFeaturesAndCA)
{
    std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> resources_list;

    //define and add resource, only mandatory fields
    fsm::ExpiryNotificationSignal::ResourceInfo resource_info;
    resource_info.name = std::string(fsm::fscfg_kFeaturesResource);
    resource_info.reconnect = true;
    resources_list.push_back(resource_info);

    fsm::ExpiryNotificationSignal::ResourceInfo resource_info2;
    resource_info2.name = std::string(fsm::fscfg_kCarAccessResource);
    resource_info2.reconnect = true;
    resources_list.push_back(resource_info2);

    EncodeExpirySignal(resources_list, std::string("expiry_signal_Features_CA.bin"));
}

TEST (GenerateCloudmockContent, GenerateAssistanceCall)
{
    fsm::SetCertsForEncode();

    //create assistance call object
    std::shared_ptr<fsm::AssistanceCallPayload> assistance_call =
        std::make_shared<fsm::AssistanceCallPayload>();

    fsm::PayloadInterface::PayloadIdentifier assistance_call_identifier =
        assistance_call->GetIdentifier().payload_identifier;

    //set requested values for call center and thiz
    assistance_call->SetCallCenterAddress(fsm::AssistanceCallPayload::kUri,
                                          "http://192.168.1.113:8088/RN/assistancecall-1/callcentersettings");
    assistance_call->SetThizAddress(fsm::AssistanceCallPayload::kUri, "http://192.168.1.113:8088/RN/assistancecall-1");


    //Create a ccm signal
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<fsm::CcmSignal> ccm = std::make_shared<fsm::CcmSignal>(transaction_id);

    ccm->SetPayload(assistance_call);
    ccm->SetRecipients(users);


    //Encode the signal
    std::shared_ptr<std::vector<unsigned char>> buffer =
        fsm::SignalFactory::Encode(ccm);

    //make sure something got encoded
    ASSERT_NE(buffer, nullptr);
    ASSERT_GT(buffer->size(), 0);

    DumpMessageFile("assistance_call.bin", (char*)buffer->data(), buffer->size());
}

}

/** \}    end of addtogroup */
