/**
 * Copyright (C) 2017, 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     test_classes.cc
 *  \brief    Volvo On Test Classes
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#include "fs_config_mock.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <cstring>
#include <vector>


#include <fscfg/provisioning_interface.h>
#include <fscfg/encoding_interface.h>
#include <fscfg/bus_names.h>




#include "signals/basic_car_control_signal.h"

#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm.h"

#include "test_classes.h"


//test implementation of the fs_config interfaces
namespace fsm
{


std::shared_ptr<ProvisioningInterface> GetProvisioningInterface()
{
    static std::shared_ptr<ProvisioningTest> provisioning_test =
            std::make_shared<ProvisioningTest>();

    return provisioning_test;
}


std::shared_ptr<DiscoveryInterface> GetDiscoveryInterface()
{
    static std::shared_ptr<DiscoveryTest> discovery_test =
            std::make_shared<DiscoveryTest>();

    return discovery_test;
}


TestEncodingInterface::TestEncodingInterface( std::string name)
{
    name_ = name;
}

fscfg_ReturnCode TestEncodingInterface::GetName(std::string& name)
{
    name = name_;
    return fscfg_kRcSuccess;
}


ProvisioningTest::ProvisioningTest()
{
    delayed_resources_available_ = false;
    //create common ca provisioned object
    ca_res_ = std::make_shared<ProvisionedResourceTest>(fsm::kCarAccessResourceName);
    ca_res_delayed_ = std::make_shared<ProvisionedResourceTest>("caraccess-delayed");
}

ProvisioningTest::~ProvisioningTest()
{
}


fscfg_ReturnCode ProvisioningTest::OnResourcesChanged(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources)
{
    return fscfg_kRcNotImplemented;
}


fscfg_ReturnCode ProvisioningTest::BindResourcesChanged(std::function<fscfg_ReturnCode (ResourcesChangedEvent)> func,
                                                        std::uint32_t& id)
{
    id = 1234;
    cb_resources_ = func;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisioningTest::Unbind(std::uint32_t id)
{
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisioningTest::GetSource(std::shared_ptr<ProvisionedResourceInterface> resource,
                                             std::shared_ptr<SourceInterface>& source)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisioningTest::GetEncoding(std::shared_ptr<ProvisionedResourceInterface> resource,
                                               std::shared_ptr<EncodingInterface>& source)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisioningTest::GetAll(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resource)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisioningTest::Get(const std::string& path,
                                       std::shared_ptr<ProvisionedResourceInterface>& resource)
{
    fscfg_ReturnCode ret_code = fscfg_kRcSuccess;

    if (path.compare(fsm::kCarAccessResourceName) == 0)
    {
        resource = ca_res_;
    }
    else if (path.compare("undefined") == 0)
    {
        ret_code = fscfg_kRcUnavailable;
    }
    else if (path.compare("caraccess-delayed") == 0)
    {
        //return ca resource only if delayed_resources_available_ is true
        if (delayed_resources_available_)
        {
            resource = ca_res_delayed_;
        }
        else
        {
            ret_code = fscfg_kRcUnavailable;
        }
    }
    else
    {
        resource = std::make_shared<ProvisionedResourceTest>(path);
    }

    return ret_code;
}

void ProvisioningTest::SetDelayedResourceAvailability(bool resource_present)
{
    delayed_resources_available_ = resource_present;
}


void ProvisioningTest::TriggerResourcesChanged(bool resource_present)
{
    if (cb_resources_)
    {
        ResourcesChangedEvent event;
        if (resource_present)
        {
            event.resources.push_back(ca_res_delayed_); //add car access
        }
        cb_resources_(event);
    }
}


static void add_topics(std::shared_ptr<fsm::CarAccessSignal> signal)
{
    // Uplink topics
    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_1;
    uplink_topic_1.topic = fsm::uplink_topic_high;
    uplink_topic_1.priority = fsm::high_prio;
    fsm::CarAccessSignal::MqttServiceInfo service_info;
    service_info.oid = volvo_on_call::BccCl002Signal::oid;
    uplink_topic_1.services.push_back(service_info);
    service_info.oid = volvo_on_call::BccCl011Signal::oid;
    uplink_topic_1.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_1);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_2;
    uplink_topic_2.topic = fsm::uplink_topic_normal;
    uplink_topic_2.priority = fsm::normal_prio;
    // services are optional, dont set any for normal
    signal->AddUplinkTopic(uplink_topic_2);

    fsm::CarAccessSignal::MqttTopicInfo uplink_topic_3;
    uplink_topic_3.topic = fsm::uplink_topic_low;
    uplink_topic_3.priority = fsm::low_prio;
    service_info.oid = volvo_on_call::BasicCarControlSignal::oid;
    uplink_topic_3.services.push_back(service_info);
    service_info.oid = volvo_on_call::BccCl002Signal::oid;
    uplink_topic_3.services.push_back(service_info);
    signal->AddUplinkTopic(uplink_topic_3);


    // Downlink topics
    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_1;
    downlink_topic_1.topic = fsm::downlink_topic_high;
    downlink_topic_1.priority = fsm::high_prio;
    signal->AddDownlinkTopic(downlink_topic_1);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_2;
    downlink_topic_2.topic = fsm::downlink_topic_normal;
    downlink_topic_2.priority = fsm::normal_prio;
    signal->AddDownlinkTopic(downlink_topic_2);

    fsm::CarAccessSignal::MqttTopicInfo downlink_topic_3;
    downlink_topic_3.topic = fsm::downlink_topic_low;
    downlink_topic_3.priority = fsm::low_prio;
    signal->AddDownlinkTopic(downlink_topic_3);
}

ProvisionedResourceTest::ProvisionedResourceTest(const std::string& path)
{
    name_ = path;

    //create test payload
    fsm::CCMTransactionId transaction_id;

    std::shared_ptr<fsm::CarAccessSignal> signal
            = std::make_shared<fsm::CarAccessSignal>(transaction_id, 1);

    signal->SetAdressUri("/this_is_a_test_uri");
    signal->SetCatalogueUplink("catalogue_uplink");
    signal->SetCatalogueDownlink("catalogue_downlink");
    signal->SetDelegateUplink("delegate_uplink");
    signal->SetDelegateDownlink("delegate_downlink");
    signal->SetDevicePairingUplink("device_pairing_uplink");
    signal->SetDevicePairingDownlink("device_pairing_downlink");

    add_topics(signal);
    char* buffer = NULL;
    size_t num_bytes = 0;

    fsm::SetCertsForEncode();

    signal->SetRecipients(voc_test_classes::users);
    test_payload_ca_ = *(SignalFactory::Encode(signal));

    std::shared_ptr<fsm::XmlPayload> xml_payload =
        std::make_shared<fsm::XmlPayload>();

    fsm::PayloadInterface::PayloadIdentifier xml_identifier =
        xml_payload->GetIdentifier().payload_identifier;

    xml_payload->SetXmlData(external_diagnostics_test_xml);

    fsm::SignalFactory::RegisterPayloadFactory<fsm::XmlPayload>(xml_identifier);

    test_payload_ed_ = *fsm::SignalFactory::Encode(xml_payload);

    fsm::SetCertsForDecode();

}

ProvisionedResourceTest::~ProvisionedResourceTest()
{
}

fscfg_ReturnCode ProvisionedResourceTest::OnPayloadChanged(std::vector<std::uint8_t> payload)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisionedResourceTest::OnSourceChanged(std::shared_ptr<SourceInterface> source)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode
ProvisionedResourceTest::BindPayloadChanged(std::function<fscfg_ReturnCode (PayloadChangedEvent)> func,
                                            std::uint32_t& id)
{
    payload_cb_ = func;
    id = 123;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode
ProvisionedResourceTest::BindSourceChanged(std::function<fscfg_ReturnCode (SourceChangedEvent)> func,
                                           std::uint32_t& id)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisionedResourceTest::Unbind(std::uint32_t id)
{
    payload_cb_ = nullptr;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisionedResourceTest::GetPayload(std::vector<std::uint8_t>& payload)
{
    fscfg_ReturnCode result = fscfg_kRcNotImplemented;

    std::string external_diagnostcs_name = std::string(fscfg_kExternalDiagnosticsResource);

    //if car access was requested, return a car access signal
    if (name_.compare(fsm::kCarAccessResourceName)==0 || name_.compare("caraccess-delayed")==0)
    {
        payload = test_payload_ca_;
        //trigger payload change if cb registered;
        TriggerPayloadChange();
        result = fscfg_kRcSuccess;
    } else if (name_.compare(external_diagnostcs_name) == 0)
    {
        payload = test_payload_ed_;
        //trigger payload change if cb registered;
        TriggerPayloadChange();
        result = fscfg_kRcSuccess;
    }

    return result;
}

fscfg_ReturnCode ProvisionedResourceTest::GetSource(std::shared_ptr<SourceInterface>& payload)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisionedResourceTest::GetEncoding(std::shared_ptr<EncodingInterface>& payload)
{
    fscfg_ReturnCode result = fscfg_kRcNotImplemented;

    if (name_.compare(fsm::kCarAccessResourceName)==0 || name_.compare("caraccess-delayed")==0)
    {
        payload = std::make_shared<TestEncodingInterface>(std::string(fsm::fscfg_kCcmEncoding));
        result = fscfg_kRcSuccess;

    } else if (name_.compare(std::string(fsm::fscfg_kExternalDiagnosticsResource)) == 0)
    {
        payload = std::make_shared<TestEncodingInterface>(std::string(fsm::fscfg_kXmlEncoding));
        result = fscfg_kRcSuccess;
    }

    return result;
}

fscfg_ReturnCode ProvisionedResourceTest::GetName(std::string& name)
{
    name = name_;
    return fscfg_kRcSuccess;
}

void ProvisionedResourceTest::TriggerPayloadChange()
{
    if (payload_cb_)
    {
        ProvisionedResourceInterface::PayloadChangedEvent event;
        event.payload = test_payload_ca_;
        payload_cb_(event);
    }
}



//DiscoveryTest
DiscoveryTest::DiscoveryTest()
{
    delayed_resources_available_ = false;
    //create common ca feature object
    ca_feature_ = std::make_shared<FeatureTest>(fsm::kCarAccessResourceName);
    ca_feature_delayed_ = std::make_shared<FeatureTest>("caraccess-delayed");
}

DiscoveryTest::~DiscoveryTest()
{
}

fscfg_ReturnCode DiscoveryTest::OnResourcesChanged(std::vector<std::shared_ptr<FeatureInterface>>& new_resources)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode DiscoveryTest::BindResourcesChanged(std::function<fscfg_ReturnCode (ResourcesChangedEvent)> func,
                                                     std::uint32_t& id)
{
    id = 12334;
    cb_resources_ = func;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode DiscoveryTest::Unbind(std::uint32_t id)
{
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode DiscoveryTest::GetAll(std::vector<std::shared_ptr<FeatureInterface>>& resources)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode DiscoveryTest::Get(const std::string& path, std::shared_ptr<FeatureInterface>& resource)
{
    static std::shared_ptr<FeatureTest> ca_feature_ = std::make_shared<FeatureTest>(path);

    fscfg_ReturnCode ret = fscfg_kRcSuccess;
    if (path.compare(fsm::kCarAccessResourceName) == 0)
    {
        resource = ca_feature_;
    }
    else if (path.compare("undefined") == 0)
    {
        ret = fscfg_kRcUnavailable;
    }
    else if (path.compare("caraccess-delayed") == 0)
    {
        //return ca feature only if delayed_resources_available_ is true
        if (delayed_resources_available_)
        {
            resource = ca_feature_delayed_;
        }
        else
        {
            ret = fscfg_kRcUnavailable;
        }
    }
    else
    {
        resource = std::make_shared<FeatureTest>(path);
    }

    return ret;
}

fscfg_ReturnCode DiscoveryTest::GetCloudConnectionStatus(char& certificate_validity,
                                                         char& central_connect_status,
                                                         std::string& central_server_uri,
                                                         std::string& central_date_time,
                                                         char& regional_connect_status,
                                                         std::string& regional_server_uri,
                                                         std::string& regional_date_time)
{
    return fscfg_kRcNotImplemented;
}

void DiscoveryTest::SetDelayedResourceAvailability(bool resource_present)
{
    delayed_resources_available_ = resource_present;
}


void DiscoveryTest::TriggerResourcesChanged(bool resource_present)
{
    if (cb_resources_)
    {
        ResourcesChangedEvent event;
        if (resource_present)
        {
            event.features.push_back(ca_feature_delayed_); //add car access
        }
        cb_resources_(event);
    }
}


FeatureTest::FeatureTest(std::string name)
{
    feature_name_ = name;
}

fscfg_ReturnCode FeatureTest::OnStateChanged(State state)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::OnDescriptionChanged(std::string description)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::OnUriChanged(std::string uri)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::OnIconChanged(std::string icon)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::OnTagsChanged(std::vector<std::string> tags)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::BindStateChanged(std::function<fscfg_ReturnCode (StateChangedEvent)> func, std::uint32_t& id)
{
    state_cb_ = func;
    id = 123;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureTest::BindDescriptionChanged(std::function<fscfg_ReturnCode (DescriptionChangedEvent)> func,
                                                     std::uint32_t& id)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::BindUriChanged(std::function<fscfg_ReturnCode (UriChangedEvent)> func,
                                             std::uint32_t& id)
{
    uri_cb_ = func;
    id = 123;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureTest::BindIconChanged(std::function<fscfg_ReturnCode (IconChangedEvent)> func,
                                              std::uint32_t& id)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::BindTagsChanged(std::function<fscfg_ReturnCode (TagsChangedEvent)> func,
                                              std::uint32_t& id)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::Unbind(std::uint32_t id)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::GetState(State& state)
{
   if (feature_name_.compare(fsm::kCarAccessResourceName) == 0)
   {
       state = FeatureInterface::State::kEnabledVisible;
   }
   else if (feature_name_.compare("caraccess-delayed") == 0)
   {
       state = FeatureInterface::State::kEnabledInvisible;
   }
   else
   {
       state = FeatureInterface::State::kDisabledInvisible;

   }

   return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureTest::GetDescription(std::string& description)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::GetUri(std::string& uri)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::GetIcon(std::string& icon)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode FeatureTest::GetTags(std::vector<std::string>& tags)
{
    return fscfg_kRcNotImplemented;
}


fscfg_ReturnCode FeatureTest::GetName(std::string& name)
{
    name = feature_name_;
    return fscfg_kRcSuccess;

}

void FeatureTest::TriggerUriChange()
{
    if (uri_cb_)
    {
        FeatureInterface::UriChangedEvent event;
        event.uri = "TEST_URI";
        uri_cb_(event);
    }
}

void FeatureTest::TriggerStateChange()
{
    if (state_cb_)
    {
        FeatureInterface::StateChangedEvent event;
        event.state = FeatureInterface::State::kEnabledVisible;
        state_cb_(event);
    }
}

}

/** \}    end of addtogroup */
