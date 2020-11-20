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
 *  \file     provisioning_bind.cpp
 *  \brief    Foundation Services Provisioning bindings handler implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfg_common/provisioning_bind.h>

#include <fscfg_common/utilities.h>

namespace fsm
{

ProvisioningBind::ProvisioningBind()
 : bind_base_()
{
}

fscfg_ReturnCode ProvisioningBind::BindResourcesChanged(ResourcesChangedCb func,std::uint32_t& id)
{
    id = std::get<kResourcesChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisioningBind::Unbind(std::uint32_t id)
{
    return bind_base_.Unbind(id);
}

fscfg_ReturnCode ProvisioningBind::OnResourcesChanged(
                                   std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources)
{
    ResourcesChangedEvent resources_ev;

    resources_ev.resources = resources;

    BindingSet<ResourcesChangedEvent>& binds = std::get<kResourcesChanged>(bind_base_.tup_);

    CallBindings(binds, resources_ev);


    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
