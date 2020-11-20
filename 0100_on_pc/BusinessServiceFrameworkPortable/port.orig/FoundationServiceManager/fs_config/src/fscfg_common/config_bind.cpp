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
 *  \file     config_bind.cpp
 *  \brief    Foundation Services Config bindings handler implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfg_common/config_bind.h>

#include <fscfg_common/utilities.h>

namespace fsm
{

ConfigBind::ConfigBind()
    : bind_base_()
{
}

fscfg_ReturnCode ConfigBind::BindEncodingsChanged(EncodingsChangedCb func, std::uint32_t& id)
{
    id = std::get<kEncodingsChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ConfigBind::BindSourcesChanged(SourcesChangedCb func, std::uint32_t& id)
{
    id = std::get<kSourcesChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ConfigBind::BindResourcesChanged(ResourcesChangedCb func,std::uint32_t& id)
{
    id = std::get<kResourcesChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ConfigBind::Unbind(std::uint32_t id)
{
    return bind_base_.Unbind(id);
}

fscfg_ReturnCode ConfigBind::OnEncodingsChanged(std::vector<std::shared_ptr<EncodingInterface>> encodings)
{
    EncodingsChangedEvent ev;

    ev.encodings = encodings;

    BindingSet<EncodingsChangedEvent>& binds = std::get<kEncodingsChanged>(bind_base_.tup_);

    CallBindings(binds, ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ConfigBind::OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources)
{
    SourcesChangedEvent ev;

    ev.sources = sources;

    BindingSet<SourcesChangedEvent>& binds = std::get<kSourcesChanged>(bind_base_.tup_);

    CallBindings(binds, ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ConfigBind::OnResourcesChanged(std::vector<std::shared_ptr<ResourceInterface>> resources)
{
    ResourcesChangedEvent ev;

    ev.resources = resources;

    BindingSet<ResourcesChangedEvent>& binds = std::get<kResourcesChanged>(bind_base_.tup_);

    CallBindings(binds, ev);

    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
