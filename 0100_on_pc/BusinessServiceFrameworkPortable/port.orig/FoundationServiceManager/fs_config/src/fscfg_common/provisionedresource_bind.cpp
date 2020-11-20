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
 *  \file     provisionedresource_bind.cpp
 *  \brief    Foundation Services Config Resource bindings handler implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfg_common/provisionedresource_bind.h>

#include <fscfg_common/utilities.h>

namespace fsm
{

ProvisionedResourceBind::ProvisionedResourceBind()
 : ResourceBind(),
   bind_base_()
{
    std::shared_ptr<IdGenerator<std::uint32_t>> id_generator = ResourceBind::bind_base_.id_generator_;
    bind_base_.SetGenerator(id_generator);
}

fscfg_ReturnCode ProvisionedResourceBind::BindPayloadChanged(PayloadChangedCb func, std::uint32_t& id)
{
    id = std::get<kPayloadChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisionedResourceBind::BindSourceChanged(SourceChangedCb func, std::uint32_t& id)
{
    id = std::get<kSourceChanged>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisionedResourceBind::Unbind(std::uint32_t id)
{
    fscfg_ReturnCode rc = ResourceBind::Unbind(id);

    if (rc == fscfg_kRcNotFound)
    {
        rc = bind_base_.Unbind(id);
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResourceBind::OnPayloadChanged(std::vector<std::uint8_t> payload)
{
    PayloadChangedEvent payload_ev;

    payload_ev.resource = std::dynamic_pointer_cast<ProvisionedResourceBind>(shared_from_this());
    payload_ev.payload = payload;

    BindingSet<PayloadChangedEvent>& binds = std::get<kPayloadChanged>(bind_base_.tup_);


    CallBindings(binds, payload_ev);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisionedResourceBind::OnSourceChanged(std::shared_ptr<SourceInterface> source)
{
    SourceChangedEvent source_ev;

    source_ev.resource = std::dynamic_pointer_cast<ProvisionedResourceBind>(shared_from_this());
    source_ev.source = source;

    BindingSet<SourceChangedEvent>& binds = std::get<kSourceChanged>(bind_base_.tup_);

    CallBindings(binds, source_ev);

    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
