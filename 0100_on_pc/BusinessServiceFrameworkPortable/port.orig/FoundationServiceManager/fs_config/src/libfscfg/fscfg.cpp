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
 *  \file     fscfg.cpp
 *  \brief    Foundation Services Config top-level client factory implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fscfg
 *  \{
 */

#include <fscfg/fscfg.h>

#include <libfscfg/worker_proxy.h>
#include <libfscfg/config_proxy.h>
#include <libfscfg/provisioning_proxy.h>
#include <libfscfg/discovery_proxy.h>

namespace fsm
{

static std::shared_ptr<WorkerProxy> GetWorker()
{
    static std::shared_ptr<WorkerProxy> worker_proxy = std::make_shared<WorkerProxy>();
    return worker_proxy;
}

std::shared_ptr<ConfigInterface> GetConfigInterface()
{
    std::shared_ptr<ConfigInterface> config_iface(nullptr);
    std::shared_ptr<WorkerProxy> worker_proxy = GetWorker();

    if (worker_proxy)
    {
        config_iface = worker_proxy->GetConfig();
    }

    return config_iface;
}

std::shared_ptr<ProvisioningInterface> GetProvisioningInterface()
{
    std::shared_ptr<ProvisioningInterface> provisioning_iface(nullptr);
    std::shared_ptr<WorkerProxy> worker_proxy = GetWorker();

    if (worker_proxy)
    {
        provisioning_iface = worker_proxy->GetProvisioning();
    }

    return provisioning_iface;
}

std::shared_ptr<DiscoveryInterface> GetDiscoveryInterface()
{
    std::shared_ptr<DiscoveryInterface> discovery_iface(nullptr);
    std::shared_ptr<WorkerProxy> worker_proxy = GetWorker();

    if (worker_proxy)
    {
        discovery_iface = worker_proxy->GetDiscovery();
    }

    return discovery_iface;
}

}


/** \}    end of addtogroup */
