/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     unittest_libfscfg.h
 *  \brief    Foundation Services config Test Cases header file for common
 *            data types, variables and functions
 *  \author   Imran Siddique
 *
 *  \addtogroup unittest
 *  \{
 */

#ifndef UNITTEST_LIBFSCFG_H_
#define UNITTEST_LIBFSCFG_H_

#include <fscfg/fscfg.h>
#include <fscfg/config_interface.h>
#include <fscfg/provisioning_interface.h>
#include <fscfg/provisionedresource_interface.h>
#include <fscfg/discovery_interface.h>

#include <dbus_interface_mock.h>

// Config callback functions
/**
 * \brief A callback function that is binds for config encodings changes
 *
 * \param [in] param EncodingsChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Config_EncodingsChangedCb(fsm::ConfigInterface::EncodingsChangedEvent param);

/**
 * \brief A callback function that is binds for config sources changes
 *
 * \param [in] param SourcesChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Config_SourcesChangedCb(fsm::ConfigInterface::SourcesChangedEvent param);

/**
 * \brief A callback function that is binds for config resources changes
 *
 * \param [in] param ResourcesChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Config_ResourcesChangedCb(fsm::ConfigInterface::ResourcesChangedEvent param);

// Resource callback functions
/**
 * \brief A callback function that is binds for resource expired notification
 *
 * \param [in] param ExpiredEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Resource_ExpiredDbusCb(fsm::ResourceInterface::ExpiredEvent param);

/**
 * \brief A callback function that is binds for resource payload changes
 *
 * \param [in] param PayloadsChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Resource_PayloadsChangedCb(fsm::ResourceInterface::PayloadsChangedEvent param);

/**
 * \brief A callback function that is binds for resource update event
 *
 * \param [in] param UpdatedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Resource_UpdatedCb(fsm::ResourceInterface::UpdatedEvent param);

/**
 * \brief A callback function that is binds for resource's source changes
 *
 * \param [in] param SourcesChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Resource_SourcesChangedCb(fsm::ResourceInterface::SourcesChangedEvent param);

// Provisioning callback functions
/**
 * \brief A callback function that is binds for provisioning resources changes
 *
 * \param [in] param ResourcesChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Provisioning_ResourcesChangedCb(fsm::ProvisioningInterface::ResourcesChangedEvent param);

// ProvisionedResource callback functions
/**
 * \brief A callback function that is binds for provisioned resource changes
 *
 * \param [in] param PayloadChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode ProvisionedResource_PayloadChangedCb(fsm::ProvisionedResourceInterface::PayloadChangedEvent param);

/**
 * \brief A callback function that is binds for provisioned resource's source changes
 *
 * \param [in] param SourceChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode ProvisionedResource_SourceChangedCb(fsm::ProvisionedResourceInterface::SourceChangedEvent param);

// Discovery callback functions
/**
 * \brief A callback function that is binds for discovery resource changes
 *
 * \param [in] param ResourcesChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Discovery_ResourcesChangedCb(fsm::DiscoveryInterface::ResourcesChangedEvent param);

// Feature callback functions
/**
 * \brief A callback function that is binds for feature's state changes
 *
 * \param [in] param StateChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Feature_StateChangedCb(fsm::FeatureInterface::StateChangedEvent param);

/**
 * \brief A callback function that is binds for feature's description changes
 *
 * \param [in] param DescriptionChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Feature_DescriptionChangedCb(fsm::FeatureInterface::DescriptionChangedEvent param);

/**
 * \brief A callback function that is binds for feature's URI changes
 *
 * \param [in] param UriChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Feature_UriChangedCb(fsm::FeatureInterface::UriChangedEvent param);

/**
 * \brief A callback function that is binds for feature's icon changes
 *
 * \param [in] param IconChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Feature_IconChangedCb(fsm::FeatureInterface::IconChangedEvent param);

/**
 * \brief A callback function that is binds for feature's tag changes
 *
 * \param [in] param TagsChangedEvent event parameters.
 *
 * \return fscfg_kRcSuccess on success
 */
fscfg_ReturnCode Feature_TagsChangedCb(fsm::FeatureInterface::TagsChangedEvent param);

/**
 * \brief Payload contents
 */
const std::string kTestPayloadResource = "<Payload>PayloadResource</Payload>";
const std::string kTestPayloadEncoding = "PayloadEncoding";
const std::string kTestPayloadSource = "<Payload>PayloadSource</Payload>";
const std::string kTestPayloadProv = "<Payload>PayloadProv</Payload>";

// Config callbacks check
bool is_encodings_changed_cb_ = false;
bool is_sources_changed_cb_ = false;
bool is_resources_changed_cb_ = false;
bool is_expired_dbus_cb_ = false;
bool is_payloads_changed_cb_ = false;
bool is_updated_cb_ = false;
bool is_resource_sources_changed_cb_ = false;

// Provisioning callbacks check
bool is_provResources_changed_cb_ = false;
bool is_prov_payload_changed_cb_ = false;
bool is_prov_source_changed_cb_ = false;

// Discovery callbacks check
bool is_discovery_resources_changed_cb_ = false;

// Feature callbacks check
bool is_state_changed_cb_ = false;
bool is_description_changed_cb_ = false;
bool is_uri_changed_cb_ = false;
bool is_icon_changed_cb_ = false;
bool is_tags_changed_cb_ = false;

// Interface Objects for config, provisioning and discovery
std::shared_ptr<fsm::ConfigInterface> config_interface_ = fsm::GetConfigInterface();
std::shared_ptr<fsm::ProvisioningInterface> provisioning_interface_ = fsm::GetProvisioningInterface();
std::shared_ptr<fsm::DiscoveryInterface> discovery_interface_ = fsm::GetDiscoveryInterface();

// DBus mock interface
std::shared_ptr<fsm::DBusInterfaceMock> dbus_interface_ = std::make_shared<fsm::DBusInterfaceMock>();


#endif /* UNITTEST_LIBFSCFG_H_ */
/** \}    end of addtogroup */
