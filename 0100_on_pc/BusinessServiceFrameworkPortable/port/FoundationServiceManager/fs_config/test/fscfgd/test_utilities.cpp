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
 *  \file     test_utilities.h
 *  \brief    Foundation Services Daemon testing utilities interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <cstdint>

#include <vector>
#include <string>

#include <fscfgd/resource.h>

#include <test_utilities.h>

namespace
{
    std::shared_ptr<fsm::Config> g_config;
    std::shared_ptr<fsm::Provisioning> g_provisioning;
    std::shared_ptr<fsm::Discovery> g_discovery;
}

namespace fsm
{
    void SetConfig(std::shared_ptr<Config> config)
    {
        g_config = config;
    }

    void SetProvisioning(std::shared_ptr<Provisioning> provisioning)
    {
        g_provisioning = provisioning;
    }

    void SetDiscovery(std::shared_ptr<Discovery> discovery)
    {
        g_discovery = discovery;
    }

    std::shared_ptr<Config> GetConfig()
    {
        return g_config;
    }

    std::shared_ptr<Provisioning> GetProvisioning()
    {
        return g_provisioning;
    }

    std::shared_ptr<Discovery> GetDiscovery()
    {
        return g_discovery;
    }

    std::vector<std::uint8_t> CreatePayload(const std::string& content)
    {
        return std::vector<std::uint8_t>(content.begin(), content.end());
    }

    std::shared_ptr<Resource> CreateResource(const std::string& name)
    {
        return std::make_shared<Resource>(name, GetConfig());
    }

    std::shared_ptr<ProvisionedResource> CreateProvisionedResource(const std::string& name)
    {
        return std::make_shared<ProvisionedResource>(name, GetProvisioning());
    }

    std::shared_ptr<Feature> CreateFeature(const std::string& name)
    {
        return std::make_shared<Feature>(name, GetDiscovery());
    }

    std::shared_ptr<Source> CreateSource(const std::string& name)
    {
        return std::make_shared<Source>(name);
    }

    std::shared_ptr<Encoding> CreateEncoding(const std::string& name)
    {
        return std::make_shared<Encoding>(name);
    }
}

/** \}    end of addtogroup */
