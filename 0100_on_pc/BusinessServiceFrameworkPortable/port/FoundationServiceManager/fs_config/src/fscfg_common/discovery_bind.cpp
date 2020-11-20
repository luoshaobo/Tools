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
 *  \file     discovery_bind.cpp
 *  \brief    Foundation Services Discovery bindings handler implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfg_common/discovery_bind.h>

#include <fscfg_common/utilities.h>

namespace fsm
{

DiscoveryBind::DiscoveryBind()
 : bind_base_()
{
}

fscfg_ReturnCode DiscoveryBind::BindResourcesChanged(ResourcesChangedCb func,std::uint32_t& id)
{
    id = std::get<kResourcesChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode DiscoveryBind::Unbind(std::uint32_t id)
{
    return bind_base_.Unbind(id);
}

fscfg_ReturnCode DiscoveryBind::OnResourcesChanged(std::vector<std::shared_ptr<FeatureInterface>>& features)
{
    ResourcesChangedEvent resources_ev;

    resources_ev.features = features;

    BindingSet<ResourcesChangedEvent>& binds = std::get<kResourcesChanged>(bind_base_.tup_);

    CallBindings(binds, resources_ev);

    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
