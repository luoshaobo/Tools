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
 *  \file     unittest_config.cc
 *  \brief    Volvo On Call, unit tests, cloud configuration
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */


/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include "fs_config_mock.h"
#include "test_classes.h"


#include "signals/basic_car_control_signal.h"
#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/car_access_signal.h"
#include "voc_framework/signals/cloud_resource.h"
#include "voc_framework/features/feature_config.h"


#include <gtest/gtest.h>


namespace voc_feature_test
{
using namespace volvo_on_call;

/**
 * \brief Test CloudResource
 */
TEST (CloudConfigTests, CloudResourceSuccessCcm)
{

    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_, fsm::CarAccessSignal::CreateCarAccessSignal);

    //successful scenario - request car access resource
    fsm::CloudResource<fsm::CarAccessSignal> cloud_resource(fsm::kCarAccessResourceName);
    std::shared_ptr<fsm::CarAccessSignal> received_signal = cloud_resource.GetPayload();
    ASSERT_TRUE(cloud_resource.IsInitialized());
    ASSERT_TRUE(received_signal!=nullptr);
    ASSERT_EQ(received_signal->GetSignalType() , fsm::Signal::kCarAccessSignal);

}

TEST (CloudConfigTests, CloudResourceSuccessXml)
{
    std::shared_ptr<fsm::XmlPayload> received_signal =
        std::make_shared<fsm::XmlPayload>();
    fsm::PayloadInterface::PayloadIdentifier xml_identifier =
        received_signal->GetIdentifier().payload_identifier;

    fsm::SignalFactory::RegisterPayloadFactory<fsm::XmlPayload>(xml_identifier);
    //successful scenario - request diagnostics
    fsm::CloudResource<fsm::XmlPayload> cloud_resource(std::string(fsm::fscfg_kExternalDiagnosticsResource));
    received_signal = cloud_resource.GetPayload();
    ASSERT_TRUE(cloud_resource.IsInitialized());
    ASSERT_TRUE(received_signal!=nullptr);
    ASSERT_EQ(received_signal->GetXmlData() , fsm::external_diagnostics_test_xml);
}

TEST (CloudConfigTests, CloudResourceGetRawPayloadSuccess)
{
    //successful scenario - request car access resource
    fsm::CloudResource<void> cloud_resource(fsm::kCarAccessResourceName);
    std::vector<std::uint8_t> raw_payload = cloud_resource.GetRawPayload();
    ASSERT_TRUE(cloud_resource.IsInitialized());
    ASSERT_TRUE(raw_payload.size() != 0);
}


TEST (CloudConfigTests, CloudResourceGetRawPayloadNoPayload)
{
    //successful scenario - request car access resource
    fsm::CloudResource<void> cloud_resource("undefined");
    std::vector<std::uint8_t> raw_payload = cloud_resource.GetRawPayload();
    ASSERT_FALSE(cloud_resource.IsInitialized());
    ASSERT_TRUE(raw_payload.size() == 0);
}

TEST (CloudConfigTests, CloudResourceRequestWrongSignalType)
{
    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_, fsm::CarAccessSignal::CreateCarAccessSignal);

    //request car access but expect basic car control signal
    fsm::CloudResource<BasicCarControlSignal> cloud_resource_bcc(fsm::kCarAccessResourceName);
    std::shared_ptr<BasicCarControlSignal> received_signal_bcc = cloud_resource_bcc.GetPayload();
    ASSERT_TRUE(cloud_resource_bcc.IsInitialized());
    ASSERT_TRUE(received_signal_bcc==nullptr);

}

TEST (CloudConfigTests, CloudResourceRequestNonExistingResource)
{

    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_, fsm::CarAccessSignal::CreateCarAccessSignal);

    //request signal that does not exist
    fsm::CloudResource<BasicCarControlSignal> cloud_resource_bcc2("undefined");
    std::shared_ptr<BasicCarControlSignal> received_signal_bcc2 = cloud_resource_bcc2.GetPayload();
    ASSERT_FALSE(cloud_resource_bcc2.IsInitialized());

    ASSERT_TRUE(received_signal_bcc2==nullptr);

}

static bool CloudResourceChangeHandlerCb_called = false;

static void CloudResourceChangeHandlerCb(std::shared_ptr<fsm::CarAccessSignal> new_data)
{
    CloudResourceChangeHandlerCb_called = true;
}

TEST (CloudConfigTests, CloudResourceTriggerPayloadChange)
{

    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_, fsm::CarAccessSignal::CreateCarAccessSignal);

    //successful scenario - request car access resource
    fsm::CloudResource<fsm::CarAccessSignal> cloud_resource(fsm::kCarAccessResourceName);
    std::function<void(std::shared_ptr<fsm::CarAccessSignal> new_data)> func_cb = CloudResourceChangeHandlerCb;
    cloud_resource.RegisterChangeHandler(func_cb);
    ASSERT_TRUE(cloud_resource.IsInitialized());

    //set CB flag to false
    CloudResourceChangeHandlerCb_called = false;

    std::shared_ptr<fsm::ProvisionedResourceInterface> resource;

    //get interface to trigger read, ASSERT is to assure proper test execution
    ASSERT_EQ(fsm::GetProvisioningInterface()->Get(fsm::kCarAccessResourceName, resource), fscfg_kRcSuccess);

    std::shared_ptr<fsm::ProvisionedResourceTest> resource_test
            = std::dynamic_pointer_cast<fsm::ProvisionedResourceTest>(resource);
    ASSERT_TRUE(resource_test != nullptr);

    resource_test->TriggerPayloadChange();
    ASSERT_TRUE(CloudResourceChangeHandlerCb_called);
    CloudResourceChangeHandlerCb_called = false;
}

TEST (CloudConfigTests, CloudResourcePayloadChangeDelayedRegisteration)
{

    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_, fsm::CarAccessSignal::CreateCarAccessSignal);

    //car access delayed is a resource that will not be avialable on the first call
    fsm::CloudResource<fsm::CarAccessSignal> cloud_resource("caraccess-delayed");
    std::function<void(std::shared_ptr<fsm::CarAccessSignal> new_data)> func_cb = CloudResourceChangeHandlerCb;
    cloud_resource.RegisterChangeHandler(func_cb);
    ASSERT_FALSE(cloud_resource.IsInitialized()); //should first fail in this test scenario

    //set CB flag to false
    CloudResourceChangeHandlerCb_called = false;

    //get test provisioning interface
    std::shared_ptr<fsm::ProvisioningInterface> provisioning_iface = fsm::GetProvisioningInterface();
    std::shared_ptr<fsm::ProvisioningTest> provisioning_test
            = std::dynamic_pointer_cast<fsm::ProvisioningTest>(provisioning_iface);
    ASSERT_TRUE(provisioning_test != nullptr);

    //make delayed resource available
    provisioning_test->SetDelayedResourceAvailability(true);

    std::shared_ptr<fsm::ProvisionedResourceInterface> resource;
    //get interface to trigger read, ASSERT is to assure proper test execution
    ASSERT_EQ(fsm::GetProvisioningInterface()->Get("caraccess-delayed", resource), fscfg_kRcSuccess);

    std::shared_ptr<fsm::ProvisionedResourceTest> resource_test
            = std::dynamic_pointer_cast<fsm::ProvisionedResourceTest>(resource);
    ASSERT_TRUE(resource_test != nullptr);

    //trigger payload change, should not be propagated to clour resource as we have not
    //informed cloud resource that the resource is now available
    resource_test->TriggerPayloadChange();
    ASSERT_FALSE(CloudResourceChangeHandlerCb_called);

    //inform cloud resource  config that resource is available
    provisioning_test->TriggerResourcesChanged(true);

    sleep(1); //give thread time to perform initalization

    ASSERT_TRUE(cloud_resource.IsInitialized()); //CR should be now initialized
    ASSERT_TRUE(CloudResourceChangeHandlerCb_called); //payload change should now reach CR
    ASSERT_TRUE(cloud_resource.GetPayload() != nullptr); // data returned

    //cleanup
    CloudResourceChangeHandlerCb_called = false;
    provisioning_test->SetDelayedResourceAvailability(false);

}

/*******************************************************************************
 *  Test a scenario in which CR is created before it is avaibale in fs config,
 *  then it apears in the fs config and then it gets removed, and appears again.
 *  Make sure we get expected input in every step
 *******************************************************************************/
TEST (CloudConfigTests, CloudResourcePayloadChangeDelayedRegisterationAndRemoval)
{

    fsm::SignalFactory::RegisterCcmFactory(fsm::CarAccessSignal::oid_, fsm::CarAccessSignal::CreateCarAccessSignal);

    //car access delayed is a resource that will not be avialable on the first call
    fsm::CloudResource<fsm::CarAccessSignal> cloud_resource("caraccess-delayed");
    std::function<void(std::shared_ptr<fsm::CarAccessSignal> new_data)> func_cb = CloudResourceChangeHandlerCb;
    cloud_resource.RegisterChangeHandler(func_cb);
    ASSERT_FALSE(cloud_resource.IsInitialized()); //should first fail in this test scenario

    //set CB flag to false
    CloudResourceChangeHandlerCb_called = false;

    //get test provisioning interface
    std::shared_ptr<fsm::ProvisioningInterface> provisioning_iface = fsm::GetProvisioningInterface();
    std::shared_ptr<fsm::ProvisioningTest> provisioning_test
            = std::dynamic_pointer_cast<fsm::ProvisioningTest>(provisioning_iface);
    ASSERT_TRUE(provisioning_test != nullptr);

    //make delayed resource available
    provisioning_test->SetDelayedResourceAvailability(true);

    std::shared_ptr<fsm::ProvisionedResourceInterface> resource;
    //get interface to trigger read, ASSERT is to assure proper test execution
    ASSERT_EQ(fsm::GetProvisioningInterface()->Get("caraccess-delayed", resource), fscfg_kRcSuccess);

    std::shared_ptr<fsm::ProvisionedResourceTest> resource_test
            = std::dynamic_pointer_cast<fsm::ProvisionedResourceTest>(resource);
    ASSERT_TRUE(resource_test != nullptr);

    //inform cloud resource  config that resource is now available
    provisioning_test->TriggerResourcesChanged(true);

    sleep(1); //give thread time to perform initalization

    ASSERT_TRUE(cloud_resource.IsInitialized()); //CR should be now initialized
    ASSERT_TRUE(CloudResourceChangeHandlerCb_called); //callback should have been executed
                                                       //as a part of initialization
    ASSERT_TRUE(cloud_resource.GetPayload() != nullptr); // data returned

    //Now remove the resource in the cloud
    CloudResourceChangeHandlerCb_called = false;
    provisioning_test->SetDelayedResourceAvailability(false);
    ASSERT_TRUE(cloud_resource.IsInitialized()); //CR should still be initialized until we call callback

    provisioning_test->TriggerResourcesChanged(false);
    ASSERT_TRUE(CloudResourceChangeHandlerCb_called); //callback should have been executed
    ASSERT_FALSE(cloud_resource.IsInitialized()); //CR should be uninitialized
    ASSERT_TRUE(cloud_resource.GetPayload() == nullptr); //no data returned

    //make resource re-appear
    CloudResourceChangeHandlerCb_called = false;
    provisioning_test->SetDelayedResourceAvailability(true);
    ASSERT_FALSE(CloudResourceChangeHandlerCb_called);
    provisioning_test->TriggerResourcesChanged(true);
    sleep(1); //give thread time to perform initalization
    ASSERT_TRUE(cloud_resource.IsInitialized());
    ASSERT_TRUE(CloudResourceChangeHandlerCb_called);
    ASSERT_TRUE(cloud_resource.GetPayload() != nullptr);

    //cleanup
    CloudResourceChangeHandlerCb_called = false;
    provisioning_test->SetDelayedResourceAvailability(false);

}


TEST (CloudConfigTests, FeatureConfigStatusCheck)
{
    //successful scenario - request car access resource
    fsm::FeatureConfig ca_feature(fsm::kCarAccessResourceName);
    ASSERT_TRUE(ca_feature.IsEnabled());
    ASSERT_TRUE(ca_feature.IsInitialized());

    fsm::FeatureConfig other_feature("undefined");
    ASSERT_FALSE(other_feature.IsInitialized());
    ASSERT_FALSE(other_feature.IsEnabled());
}

static bool FeatureStatusChangeHandlerCb_called = false;

static void FeatureStatusChangeHandlerCb(bool new_data)
{
    FeatureStatusChangeHandlerCb_called = true;
}

TEST (CloudConfigTests, FeatureConfigStatusChange)
{
    //successful scenario - request car access resource
    fsm::FeatureConfig ca_feature(fsm::kCarAccessResourceName);

    std::function<void(bool)> func_cb
            = FeatureStatusChangeHandlerCb;
    ca_feature.RegisterStateChangeHandler(func_cb);

    ASSERT_TRUE(ca_feature.IsInitialized());

    std::shared_ptr<fsm::FeatureInterface> feature;

    //get interface to trigger read, ASSERT is to assure proper test execution
    ASSERT_EQ(fsm::GetDiscoveryInterface()->Get(fsm::kCarAccessResourceName, feature), fscfg_kRcSuccess);

    std::shared_ptr<fsm::FeatureTest> feature_test
            = std::dynamic_pointer_cast<fsm::FeatureTest>(feature);
    ASSERT_TRUE(feature_test != nullptr);

    FeatureStatusChangeHandlerCb_called = false;

    feature_test->TriggerStateChange();

    ASSERT_TRUE(FeatureStatusChangeHandlerCb_called);

    FeatureStatusChangeHandlerCb_called = false;

}


TEST (CloudConfigTests, FeatureConfigStatusChangeDelayedRegistration)
{
    //car access delayed is a resource that will not be avialable on the first call
    fsm::FeatureConfig ca_feature("caraccess-delayed");

    std::function<void(bool)> func_cb
            = FeatureStatusChangeHandlerCb;
    ca_feature.RegisterStateChangeHandler(func_cb);

    ASSERT_FALSE(ca_feature.IsInitialized()); //should fail here

    std::shared_ptr<fsm::FeatureInterface> feature;

    //get discovery test interface
    std::shared_ptr<fsm::DiscoveryInterface> discovery_iface = fsm::GetDiscoveryInterface();
    std::shared_ptr<fsm::DiscoveryTest> discovery_test
            = std::dynamic_pointer_cast<fsm::DiscoveryTest>(discovery_iface);
    ASSERT_TRUE(discovery_test != nullptr);

    //make delayed resource available
    discovery_test->SetDelayedResourceAvailability(true);

    //get interface to trigger read, ASSERT is to assure proper test execution
    ASSERT_EQ(fsm::GetDiscoveryInterface()->Get("caraccess-delayed", feature), fscfg_kRcSuccess);

    std::shared_ptr<fsm::FeatureTest> feature_test
            = std::dynamic_pointer_cast<fsm::FeatureTest>(feature);
    ASSERT_TRUE(feature_test != nullptr);

    FeatureStatusChangeHandlerCb_called = false;

    //Trigger state change before informing feature config that feature is availble
    feature_test->TriggerStateChange();

    //as we have not yet informed feature that CA-delayed is available the check should fail
    ASSERT_FALSE(FeatureStatusChangeHandlerCb_called);

    //trigger resource change callback to inform feature config that feature is now available
    discovery_test->TriggerResourcesChanged(true);

    sleep(2); //give thread time to perform initalization

    ASSERT_TRUE(ca_feature.IsInitialized()); //should be initialized now
    feature_test->TriggerStateChange();  //trigger state change
    ASSERT_TRUE(FeatureStatusChangeHandlerCb_called); //now callback should propagate
    ASSERT_TRUE(ca_feature.IsEnabled()); //should be enabled now

    FeatureStatusChangeHandlerCb_called = false;
    discovery_test->SetDelayedResourceAvailability(false);

}

/*******************************************************************************
 *  Test a scenario in which FC is created before it is available in fs config,
 *  then it apears in the fs config and then it gets removed, and appears again.
 *  Make sure we get expected input in every step
 *******************************************************************************/
TEST (CloudConfigTests, FeatureConfigStatusChangeDelayedRegistrationAndRemoval)
{
    //car access delayed is a resource that will not be avialable on the first call
    fsm::FeatureConfig ca_feature("caraccess-delayed");

    std::function<void(bool)> func_cb
            = FeatureStatusChangeHandlerCb;
    ca_feature.RegisterStateChangeHandler(func_cb);

    ASSERT_FALSE(ca_feature.IsInitialized()); //should fail here

    std::shared_ptr<fsm::FeatureInterface> feature;

    //get discovery test interface
    std::shared_ptr<fsm::DiscoveryInterface> discovery_iface = fsm::GetDiscoveryInterface();
    std::shared_ptr<fsm::DiscoveryTest> discovery_test
            = std::dynamic_pointer_cast<fsm::DiscoveryTest>(discovery_iface);
    ASSERT_TRUE(discovery_test != nullptr);

    FeatureStatusChangeHandlerCb_called = false;

    //make delayed resource available
    discovery_test->SetDelayedResourceAvailability(true);

    //get interface to trigger read, ASSERT is to assure proper test execution
    ASSERT_EQ(fsm::GetDiscoveryInterface()->Get("caraccess-delayed", feature), fscfg_kRcSuccess);

    std::shared_ptr<fsm::FeatureTest> feature_test
            = std::dynamic_pointer_cast<fsm::FeatureTest>(feature);
    ASSERT_TRUE(feature_test != nullptr);

    //as we have not yet informed feature that CA-delayed is available the check should fail
    ASSERT_FALSE(FeatureStatusChangeHandlerCb_called);

    //trigger resource change callback to inform feature config that feature is now available
    discovery_test->TriggerResourcesChanged(true);

    sleep(2); //give thread time to perform initalization

    ASSERT_TRUE(ca_feature.IsInitialized()); //should be initialized now
    feature_test->TriggerStateChange();  //trigger state change
    ASSERT_TRUE(FeatureStatusChangeHandlerCb_called); //now callback should propagate
    FeatureStatusChangeHandlerCb_called = false; //reset


    //make feature disappear
    discovery_test->SetDelayedResourceAvailability(false);
    discovery_test->TriggerResourcesChanged(false);
    //make sure that we got informed about the change
    ASSERT_TRUE(FeatureStatusChangeHandlerCb_called);
    //make sure we have initialized
    sleep(1); //give thread time to perform initalization
    ASSERT_FALSE(ca_feature.IsInitialized());
    FeatureStatusChangeHandlerCb_called = false; //reset


    //make feature appear again
    discovery_test->SetDelayedResourceAvailability(true);
    discovery_test->TriggerResourcesChanged(true);
    sleep(1); //give thread time to perform initalization
    //make sure that we got informed about the change
    ASSERT_TRUE(FeatureStatusChangeHandlerCb_called);
    //make sure we have initialized
    ASSERT_TRUE(ca_feature.IsInitialized());


    //reset
    FeatureStatusChangeHandlerCb_called = false;
    discovery_test->SetDelayedResourceAvailability(false);
}

/** \}    end of addtogroup */

} // namespace
