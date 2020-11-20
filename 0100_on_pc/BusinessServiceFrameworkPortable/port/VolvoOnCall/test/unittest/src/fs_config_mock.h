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
 *  \file     fs_config_mock.h
 *  \brief    Volvo On Call fs config classes
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#ifndef VOC_FSCONFIG_MOCK_HH_
#define VOC_FSCONFIG_MOCK_HH_



#include "fscfg/discovery_interface.h"
#include "fscfg/feature_interface.h"
#include "fscfg/provisionedresource_interface.h"
#include "fscfg/provisioning_interface.h"
#include "fscfg/resource_interface.h"
#include "signals/basic_car_control_signal.h"
#include "voc_framework/signals/car_access_signal.h"
#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/cloud_resource.h"


namespace fsm
{
    
const char* fscfg_kCarAccessResource = "CarAccess";

const std::string external_diagnostics_test_xml =
        "<external_diagnostics>"
        "<this>/external_diagnostics-1</this>"
        "<remote_session>/external_diagnostics-1/03924akfjldkzjcv</remote_session>"
        "</external_diagnostics>";


/**
 * \brief Foundation Services Encoding Interface
 */

class TestEncodingInterface : public EncodingInterface
{
public:
    TestEncodingInterface( std::string name);
    /**
     * \brief Retrieves the name of the encoding instance.
     *
     * \param[out] name name of the encoding instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
     fscfg_ReturnCode GetName(std::string& name);

private:

     std::string name_;


};

/**
 * \brief Foundation Services ProvisionedResource proxy
 */
class ProvisionedResourceTest : public ProvisionedResourceInterface
{
public:        

    /**
     * \brief ProvisionedResourceTest Constructor.
     *
     * \param[in] name of the resource.
     */
    ProvisionedResourceTest(const std::string& name);

    /**
     * ProvisionedResourceTest Destructor.
     */
    ~ProvisionedResourceTest();

    /**
     * \copydoc fsm::ProvisionedResourceInterface::BindPayloadChanged
     */
    fscfg_ReturnCode BindPayloadChanged(std::function<fscfg_ReturnCode (PayloadChangedEvent)> func,
                                        std::uint32_t& id);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::BindSourceChanged
     */
    fscfg_ReturnCode BindSourceChanged(std::function<fscfg_ReturnCode (SourceChangedEvent)> func,
                                       std::uint32_t& id);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetSource
     */
    fscfg_ReturnCode GetSource(std::shared_ptr<SourceInterface>& source);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetEncoding
     */
    fscfg_ReturnCode GetEncoding(std::shared_ptr<EncodingInterface>& encoding);

    fscfg_ReturnCode Unbind(std::uint32_t id);


    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetPayload
     */
    fscfg_ReturnCode GetPayload(std::vector<std::uint8_t>& payload);

    fscfg_ReturnCode GetName(std::string& name);


protected:
    /**
     * \copydoc fsm::ProvisionedResourceInterface::OnPayloadChanged
     */
    fscfg_ReturnCode OnPayloadChanged(std::vector<std::uint8_t> payload);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::OnSourceChanged
     */
    fscfg_ReturnCode OnSourceChanged(std::shared_ptr<SourceInterface> source);

    std::string name_;

    std::vector<std::uint8_t> test_payload_ca_;  //test payload for car access (ccm)
    std::vector<std::uint8_t> test_payload_ed_;  //test payload for external diagnostics (xml)


    std::function<fscfg_ReturnCode (PayloadChangedEvent)> payload_cb_; //registered callback


public:
    //test functions
    void TriggerPayloadChange();
};



class ProvisioningTest : public ProvisioningInterface
{
public:
    /**
     * \brief ProvisioningTest Constructor.

     */
    ProvisioningTest();

    /**
     * ProvisioningTest Destructor.
     */
    ~ProvisioningTest();

    /**
     * \copydoc fsm::ProvisioningInterface::BindResourcesChanged
     */
    fscfg_ReturnCode BindResourcesChanged(std::function<fscfg_ReturnCode (ResourcesChangedEvent)> func,
                                          std::uint32_t& id);

    /**
     * \copydoc fsm::ProvisioningInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);

    /**
     * \copydoc fsm::ProvisioningInterface::GetSource
     */
    fscfg_ReturnCode GetSource(std::shared_ptr<ProvisionedResourceInterface> resource, std::shared_ptr<SourceInterface>& source);

    /**
     * \copydoc fsm::ProvisioningInterface::GetEncoding
     */
    fscfg_ReturnCode GetEncoding(std::shared_ptr<ProvisionedResourceInterface> resource,
                                 std::shared_ptr<EncodingInterface>& encoding);

    /**
     * \copydoc fsm::ProvisioningInterface::GetAll
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources);

    /**
     * \copydoc fsm::ProvisioningInterface::Get
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<ProvisionedResourceInterface>& resource);


    /**
     * \brief Triggers signalling that resources have changed
     * \param[in] resource_present true if resource visible, false otherwise.
     */
    void SetDelayedResourceAvailability(bool resource_present);

    /**
     * \brief Sets flag that delayed resource should be now made available
     * \param[in] resource_present true if resource should be on the list of resources, false otherwise.
     */
    void TriggerResourcesChanged(bool resource_present);

protected:
    /**
     * \copydoc fsm::ProvisioningInterface::OnResourcesChanged
     */
    fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources);

    std::function<fscfg_ReturnCode (ResourcesChangedEvent)> cb_resources_; //callback

    bool delayed_resources_available_;

    std::shared_ptr<ProvisionedResourceTest> ca_res_; //common ca provisioned object
    std::shared_ptr<ProvisionedResourceTest> ca_res_delayed_; //common delayed ca provisioned object
};


class FeatureTest: public FeatureInterface
{
public:

    /**
     * \brief virtual Destructor.
     */
    ~FeatureTest() { };

    FeatureTest(std::string name);


    /**
     * \copydoc fsm::FeatureInterface::BindStateChanged
     */
    fscfg_ReturnCode BindStateChanged(std::function<fscfg_ReturnCode (StateChangedEvent)> func,
                                      std::uint32_t& id) ;


    /**
     * \copydoc fsm::FeatureInterface::BindDescriptionChanged
     */
    fscfg_ReturnCode BindDescriptionChanged(std::function<fscfg_ReturnCode (DescriptionChangedEvent)> func,
                                            std::uint32_t& id);

    /**
     * \copydoc fsm::FeatureInterface::BindUriChanged
     */
    fscfg_ReturnCode BindUriChanged(std::function<fscfg_ReturnCode (UriChangedEvent)> func,
                                    std::uint32_t& id);

    /**
     * \copydoc fsm::FeatureInterface::BindIconChanged
     */
    fscfg_ReturnCode BindIconChanged(std::function<fscfg_ReturnCode (IconChangedEvent)> func,
                                     std::uint32_t& id);

    /**
     * \copydoc fsm::FeatureInterface::BindTagsChanged
     */
    fscfg_ReturnCode BindTagsChanged(std::function<fscfg_ReturnCode (TagsChangedEvent)> func,
                                     std::uint32_t& id);
    /**
     * \copydoc fsm::FeatureInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);


    /**
     * \copydoc fsm::FeatureInterface::GetName
     */
    fscfg_ReturnCode GetName(std::string& name);

    /**
     * \copydoc fsm::FeatureInterface::GetState
     */
    fscfg_ReturnCode GetState(State& state);

    /**
     * \copydoc fsm::FeatureInterface::GetDescription
     */
    fscfg_ReturnCode GetDescription(std::string& description);

    /**
     * \copydoc fsm::FeatureInterface::GetUri
     */
    fscfg_ReturnCode GetUri(std::string& uri);

    /**
     * \copydoc fsm::FeatureInterface::GetIcon
     */
    fscfg_ReturnCode GetIcon(std::string& icon);

    /**
     * \copydoc fsm::FeatureInterface::GetTags
     */
    fscfg_ReturnCode GetTags(std::vector<std::string>& tags);

protected:
    /**
     * \copydoc fsm::FeatureInterface::OnStateChanged
     */
    fscfg_ReturnCode OnStateChanged(State state);

    /**
     * \copydoc fsm::FeatureInterface::OnDescriptionChanged
     */
    fscfg_ReturnCode OnDescriptionChanged(std::string description);

    /**
     * \copydoc fsm::FeatureInterface::OnUriChanged
     */
    fscfg_ReturnCode OnUriChanged(std::string uri);

    /**
     * \copydoc fsm::FeatureInterface::OnIconChanged
     */
    fscfg_ReturnCode OnIconChanged(std::string icon);

    /**
     * \copydoc fsm::FeatureInterface::OnTagsChanged
     */
    fscfg_ReturnCode OnTagsChanged(std::vector<std::string> tags);

public:
    //test extensions
    std::string feature_name_;
    std::function<fscfg_ReturnCode (StateChangedEvent)> state_cb_;
    std::function<fscfg_ReturnCode (UriChangedEvent)> uri_cb_;
    void TriggerUriChange();
    void TriggerStateChange();
};


/**
 * \brief Foundation Services Discovery subject interface.
*/
class DiscoveryTest: public DiscoveryInterface
{
public:

    DiscoveryTest();

    ~DiscoveryTest();

    /**
     * \copydoc fsm::DiscoveryInterface::BindResourcesChanged
     */
    fscfg_ReturnCode BindResourcesChanged(std::function<fscfg_ReturnCode (ResourcesChangedEvent)> func,
                                          std::uint32_t& id);

    /**
     * \copydoc fsm::DiscoveryInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id) ;

    /**
     * \copydoc fsm::DiscoveryInterface::GetAll
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<FeatureInterface>>& features);

    /**
     * \copydoc fsm::DiscoveryInterface::Get
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<FeatureInterface>& feature);

    /**
     * \copydoc fsm::DiscoveryInterface::GetCloudConnectionStatus
     */
    fscfg_ReturnCode GetCloudConnectionStatus(char& certificate_validity,
                                              char& central_connect_status,
                                              std::string& central_server_uri,
                                              std::string& central_date_time,
                                              char& regional_connect_status,
                                              std::string& regional_server_uri,
                                              std::string& regional_date_time);

protected:
    /**
     * \copydoc fsm::DiscoveryInterface::OnResourcesChanged
     */
    fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<FeatureInterface>>& features);

    bool delayed_resources_available_; //keeps track if delayed resource should be made available
    std::shared_ptr<FeatureTest> ca_feature_; //common ca feature object
    std::shared_ptr<FeatureTest> ca_feature_delayed_; //common delayed ca feature object

    std::function<fscfg_ReturnCode (DiscoveryInterface::ResourcesChangedEvent)> cb_resources_;


public: //test interface


    /**
     * \brief Triggers signalling that resources have changed
     * \param[in] resource_present true if resource visible, false otherwise.
     */
    void TriggerResourcesChanged(bool resource_present);

    /**
     * \brief Sets flag that delayed resource should be now mde available
     * \param[in] resource_present true if resource should be on the list of resources, false otherwise.
     */
    void SetDelayedResourceAvailability(bool resource_present);

};

static const std::string uplink_topic_low = "uplink topic_low";
static const std::string uplink_topic_normal = "uplink topic_normal";
static const std::string uplink_topic_high = "uplink topic_high";
static const std::string downlink_topic_low = "downlink topic_low";
static const std::string downlink_topic_normal = "downlink topic_normal";
static const std::string downlink_topic_high = "downlink topic_high";
static const int high_prio = 80;
static const int normal_prio = 50;
static const int low_prio = 20;

}

/** \}    end of addtogroup */
#endif
