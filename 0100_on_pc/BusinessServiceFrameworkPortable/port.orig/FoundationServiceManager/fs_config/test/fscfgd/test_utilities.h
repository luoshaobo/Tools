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
#include <fscfgd/provisionedresource.h>
#include <fscfgd/feature.h>

#include <fscfgd/discovery.h>
#include <fscfgd/provisioning.h>
#include <fscfgd/config.h>

namespace fsm
{
    void SetDiscovery(std::shared_ptr<Discovery> discovery);
    void SetProvisioning(std::shared_ptr<Provisioning> provisioning);
    void SetConfig(std::shared_ptr<Config> config);

    std::shared_ptr<Discovery> GetDiscovery();
    std::shared_ptr<Provisioning> GetProvisioning();
    std::shared_ptr<Config> GetConfig();

    std::vector<std::uint8_t> CreatePayload(const std::string& content);
    std::shared_ptr<Resource> CreateResource(const std::string& name);
    std::shared_ptr<ProvisionedResource> CreateProvisionedResource(const std::string& name);
    std::shared_ptr<Feature> CreateFeature(const std::string& name);
    std::shared_ptr<Source> CreateSource(const std::string& name);
    std::shared_ptr<Encoding> CreateEncoding(const std::string& name);

} // namespace fsm

/** \}    end of addtogroup */
