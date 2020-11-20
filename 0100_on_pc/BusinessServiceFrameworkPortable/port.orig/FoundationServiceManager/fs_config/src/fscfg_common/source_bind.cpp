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
 *  \file     source_bind.cpp
 *  \brief    Foundation Services Source bindings handler implementation.
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfg_common/source_bind.h>

namespace fsm
{

SourceBind::SourceBind()
 : bind_base_()
{
}

fscfg_ReturnCode SourceBind::BindUpdated(UpdatedCb func,std::uint32_t& id)
{
    id = std::get<kUpdated>(bind_base_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode SourceBind::Unbind(std::uint32_t id)
{
    return bind_base_.Unbind(id);
}

fscfg_ReturnCode SourceBind::OnUpdated(std::vector<std::shared_ptr<ResourceInterface>> resources)
{
    UpdatedEvent ev;

    ev.source = shared_from_this();
    ev.resources = resources;

    for (auto& pair_id_func : std::get<kUpdated>(bind_base_.tup_).GetBindings())
    {
        pair_id_func.second(ev);
    }

    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
