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
 *  \file     fs_config_stub.cc
 *  \brief    Foundation Service Manager Config stubs
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include "fs_config_stub.h"

namespace fsm
{

const char* fscfg_kCarAccessResource = "CarAccess";
const char* fscfg_kXmlEncoding = "xml";
const char* fscfg_kCcmEncoding = "CCM";

std::shared_ptr<ProvisioningInterface> GetProvisioningInterface()
{
    static std::shared_ptr<ProvisioningStub> provisioning_test =
            std::make_shared<ProvisioningStub>();

    return provisioning_test;
}


std::shared_ptr<DiscoveryInterface> GetDiscoveryInterface()
{
    static std::shared_ptr<DiscoveryStub> discovery_test =
            std::make_shared<DiscoveryStub>();

    return discovery_test;
}


ProvisioningStub::ProvisioningStub()
{
}

ProvisioningStub::~ProvisioningStub()
{
}

fscfg_ReturnCode ProvisioningStub::OnResourcesChanged(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources)
{
    return fscfg_kRcNotImplemented;
}


fscfg_ReturnCode ProvisioningStub::BindResourcesChanged(std::function<fscfg_ReturnCode (ResourcesChangedEvent)> func,
                                                        std::uint32_t& id)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisioningStub::Unbind(std::uint32_t id)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisioningStub::GetSource(std::shared_ptr<ProvisionedResourceInterface> resource,
                                             std::shared_ptr<SourceInterface>& source)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisioningStub::GetEncoding(std::shared_ptr<ProvisionedResourceInterface> resource,
                                               std::shared_ptr<EncodingInterface>& source)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisioningStub::GetAll(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resource)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ProvisioningStub::Get(const std::string& path,
                                       std::shared_ptr<ProvisionedResourceInterface>& resource)
{
    return fscfg_kRcNotImplemented;
}

//DiscoveryTest
DiscoveryStub::DiscoveryStub()
{
}

DiscoveryStub::~DiscoveryStub()
{
}

fscfg_ReturnCode DiscoveryStub::OnResourcesChanged(std::vector<std::shared_ptr<FeatureInterface>>& new_resources)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode DiscoveryStub::BindResourcesChanged(std::function<fscfg_ReturnCode (ResourcesChangedEvent)> func,
                                                     std::uint32_t& id)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode DiscoveryStub::Unbind(std::uint32_t id)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode DiscoveryStub::GetAll(std::vector<std::shared_ptr<FeatureInterface>>& resources)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode DiscoveryStub::Get(const std::string& path, std::shared_ptr<FeatureInterface>& resource)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode DiscoveryStub::GetCloudConnectionStatus(char& certificate_validity,
                                                         char& central_connect_status,
                                                         std::string& central_server_uri,
                                                         std::string& central_date_time,
                                                         char& regional_connect_status,
                                                         std::string& regional_server_uri,
                                                         std::string& regional_date_time)
{
    return fscfg_kRcNotImplemented;
}

}

/** \}    end of addtogroup */
