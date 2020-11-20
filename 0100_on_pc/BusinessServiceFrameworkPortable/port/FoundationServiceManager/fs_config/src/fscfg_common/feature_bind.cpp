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
 *  \file     feature_bind.cpp
 *  \brief    Foundation Services Config Feature bindings handler implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfg_common/feature_bind.h>

#include <fscfg_common/utilities.h>

namespace fsm
{

FeatureBind::FeatureBind()
 : ProvisionedResourceBind(),
   bind_base_()
{
    std::shared_ptr<IdGenerator<std::uint32_t>> id_generator = ProvisionedResourceBind::bind_base_.id_generator_;
    bind_base_.SetGenerator(id_generator);
}

fscfg_ReturnCode FeatureBind::BindStateChanged(StateChangedCb func, std::uint32_t& id)
{
    id = std::get<kStateChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureBind::BindDescriptionChanged(DescriptionChangedCb func, std::uint32_t& id)
{
    id = std::get<kDescriptionChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureBind::BindUriChanged(UriChangedCb func, std::uint32_t& id)
{
    id = std::get<kUriChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureBind::BindIconChanged(IconChangedCb func, std::uint32_t& id)
{
    id = std::get<kIconChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureBind::BindTagsChanged(TagsChangedCb func, std::uint32_t& id)
{
    id = std::get<kTagsChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureBind::Unbind(std::uint32_t id)
{
    fscfg_ReturnCode rc = ProvisionedResourceBind::Unbind(id);

    if (rc == fscfg_kRcNotFound)
    {
        rc = bind_base_.Unbind(id);
    }

    return rc;
}

fscfg_ReturnCode FeatureBind::OnStateChanged(State state)
{
    StateChangedEvent state_ev;

    state_ev.feature = std::dynamic_pointer_cast<FeatureBind>(shared_from_this());
    state_ev.state = state;

    BindingSet<StateChangedEvent>& binds = std::get<kStateChanged>(bind_base_.tup_);

    CallBindings(binds, state_ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureBind::OnDescriptionChanged(std::string description)
{
    DescriptionChangedEvent description_ev;

    description_ev.feature = std::dynamic_pointer_cast<FeatureBind>(shared_from_this());
    description_ev.description = description;

    BindingSet<DescriptionChangedEvent>& binds = std::get<kDescriptionChanged>(bind_base_.tup_);

    CallBindings(binds, description_ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureBind::OnUriChanged(std::string uri)
{
    UriChangedEvent uri_ev;

    uri_ev.feature = std::dynamic_pointer_cast<FeatureBind>(shared_from_this());
    uri_ev.uri = uri;

    BindingSet<UriChangedEvent>& binds = std::get<kUriChanged>(bind_base_.tup_);

    CallBindings(binds, uri_ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureBind::OnIconChanged(std::string icon)
{
    IconChangedEvent icon_ev;

    icon_ev.feature = std::dynamic_pointer_cast<FeatureBind>(shared_from_this());
    icon_ev.icon = icon;

    BindingSet<IconChangedEvent>& binds = std::get<kIconChanged>(bind_base_.tup_);

    CallBindings(binds, icon_ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureBind::OnTagsChanged(std::vector<std::string> tags)
{
    TagsChangedEvent tags_ev;

    tags_ev.feature = std::dynamic_pointer_cast<FeatureBind>(shared_from_this());
    tags_ev.tags = tags;

    BindingSet<TagsChangedEvent>& binds = std::get<kTagsChanged>(bind_base_.tup_);

    CallBindings(binds, tags_ev);

    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
