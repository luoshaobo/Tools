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
 *  \file     fs_config_stub.h
 *  \brief    Foundation Service Manager Config stubs
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_FS_CONFIG_STUB_H_INC_
#define FSM_FS_CONFIG_STUB_H_INC_

#include "fscfg/discovery_interface.h"
#include "fscfg/provisioning_interface.h"

namespace fsm
{

/**
 * \brief Foundation Services Provisioning subject stub.
*/
class ProvisioningStub : public ProvisioningInterface
{
public:
    /**
     * \brief ProvisioningTest Constructor.

     */
    ProvisioningStub();

    /**
     * ProvisioningTest Destructor.
     */
    ~ProvisioningStub();

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
    fscfg_ReturnCode GetSource(std::shared_ptr<ProvisionedResourceInterface> resource,
                               std::shared_ptr<SourceInterface>& source);

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

protected:
    /**
     * \copydoc fsm::ProvisioningInterface::OnResourcesChanged
     */
    fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources);
};

/**
 * \brief Foundation Services Discovery subject stub.
*/
class DiscoveryStub: public DiscoveryInterface
{
public:

    DiscoveryStub();

    ~DiscoveryStub();

    /**
     * \copydoc fsm::DiscoveryInterface::BindResourcesChanged
     */
    fscfg_ReturnCode BindResourcesChanged(std::function<fscfg_ReturnCode (ResourcesChangedEvent)> func,
                                          std::uint32_t& id);

    /**
     * \copydoc fsm::DiscoveryInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);

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
};

}

/** \}    end of addtogroup */
#endif
