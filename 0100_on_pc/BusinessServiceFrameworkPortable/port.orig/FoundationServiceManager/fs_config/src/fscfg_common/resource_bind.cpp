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
 *  \file     resource_bind.cpp
 *  \brief    Foundation Services Config Resource bindings handler implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfg_common/resource_bind.h>

#include <fscfg_common/utilities.h>

namespace fsm
{

ResourceBind::ResourceBind()
    : bind_base_()
{
}

fscfg_ReturnCode ResourceBind::BindExpired(ExpiredCb func, std::uint32_t& id)
{
    id = std::get<kExpired>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ResourceBind::BindUpdated(UpdatedCb func, std::uint32_t& id)
{
    id = std::get<kUpdated>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ResourceBind::BindSourcesChanged(SourcesChangedCb func, std::uint32_t& id)
{
    id = std::get<kSourcesChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ResourceBind::BindPayloadsChanged(PayloadsChangedCb func, std::uint32_t& id)
{
    id = std::get<kPayloadsChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ResourceBind::Unbind(std::uint32_t id)
{
    return bind_base_.Unbind(id);
}

fscfg_ReturnCode ResourceBind::OnExpired(bool immediate)
{
    ExpiredEvent expired_ev;

    expired_ev.resource = shared_from_this();
    expired_ev.immediate = immediate;

    BindingSet<ExpiredEvent>& binds = std::get<kExpired>(bind_base_.tup_);

    CallBindings(binds, expired_ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ResourceBind::OnUpdated(std::shared_ptr<SourceInterface> source)
{
    UpdatedEvent updated_ev;

    updated_ev.resource = shared_from_this();
    updated_ev.source = source;

    BindingSet<UpdatedEvent>& binds = std::get<kUpdated>(bind_base_.tup_);

    CallBindings(binds, updated_ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ResourceBind::OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources)
{
    SourcesChangedEvent source_ev;

    source_ev.resource = shared_from_this();
    source_ev.sources = sources;

    BindingSet<SourcesChangedEvent>& binds = std::get<kSourcesChanged>(bind_base_.tup_);

    CallBindings(binds, source_ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ResourceBind::OnPayloadsChanged(std::vector<std::uint8_t> payload,
                                                 std::shared_ptr<SourceInterface> source,
                                                 std::shared_ptr<EncodingInterface> encoding)
{
    PayloadsChangedEvent payloads_ev;

    payloads_ev.resource = shared_from_this();
    payloads_ev.payload = payload;
    payloads_ev.source = source;
    payloads_ev.encoding = encoding;

    BindingSet<PayloadsChangedEvent>& binds = std::get<kPayloadsChanged>(bind_base_.tup_);

    CallBindings(binds, payloads_ev);

    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
