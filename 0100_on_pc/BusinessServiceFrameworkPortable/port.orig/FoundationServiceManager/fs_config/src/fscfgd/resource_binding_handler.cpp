/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     resource_binding_handler.cpp
 *  \brief    Foundation Services Config resource binding handler abstract class implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/resource_binding_handler.h>

namespace fsm
{

ResourceBindingHandler::ResourceBindingHandler(std::shared_ptr<Config> config)
    : update_bindings_(),
      update_payload_bindings_(),
      config_(config)
{
}

ResourceBindingHandler::~ResourceBindingHandler()
{
    // Unbind everything that was previously bound but not unbound.

    for (auto pair_resource_bind_id : update_bindings_)
    {
        UnbindUpdate(pair_resource_bind_id.first);
    }

    for (auto pair_payload_bind_id : update_payload_bindings_)
    {
        UnbindUpdate(pair_payload_bind_id.first);
    }
}

fscfg_ReturnCode ResourceBindingHandler::BindUpdate(const std::string& resource_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!config_)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "ResourceBindingHandlder::BindUpdate: null config");

        rc = fscfg_kRcBadState;
    }
    else
    {
        std::map<std::string, std::uint32_t>::iterator it = update_bindings_.find(resource_name);

        if (it == update_bindings_.end())
        {
            // resource was not previously bound, perform the binding operation.
            std::shared_ptr<Resource> resource;

            config_->Get(resource_name, resource);

            if (!resource)
            {
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_ERROR,
                                "ResourceBindingHandler::BindUpdate: No such resource %s",
                                resource_name.c_str());

                rc = fscfg_kRcBadResource;
            }
            else
            {
                std::uint32_t bind_id;

                resource->BindUpdated(std::bind(&ResourceBindingHandler::OnBoundResourceUpdated, this, std::placeholders::_1),
                                      bind_id);

                update_bindings_[resource_name] = bind_id;
            }
        }
    }

    return rc;
}

fscfg_ReturnCode ResourceBindingHandler::BindPayloadChange(const std::string& resource_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!config_)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "ResourceBindingHandlder::BindPayloadChange: null config");

        rc = fscfg_kRcBadState;
    }
    else
    {
        std::map<std::string, std::uint32_t>::iterator it = update_payload_bindings_.find(resource_name);

        if (it == update_payload_bindings_.end())
        {
            // resource was not previously bound, perform the binding operation.
            std::shared_ptr<Resource> resource;

            config_->Get(resource_name, resource);

            if (!resource)
            {
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_ERROR,
                                "ResourceBindingHandler::BindPayloadChange: No such resource %s",
                                resource_name.c_str());

                rc = fscfg_kRcBadResource;
            }
            else
            {
                std::uint32_t bind_id;

                resource->BindPayloadsChanged(std::bind(&ResourceBindingHandler::OnBoundResourcePayloadChanged, this, std::placeholders::_1),
                                              bind_id);

                update_payload_bindings_[resource_name] = bind_id;
            }
        }
    }

    return rc;
}


fscfg_ReturnCode ResourceBindingHandler::UnbindUpdate(const std::string& resource_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!config_)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "ResourceBindingHandlder::UnbindUpdate: null config");

        rc = fscfg_kRcBadState;
    }
    else
    {
        std::shared_ptr<Resource> resource;

        config_->Get(resource_name, resource);

        if (!resource)
        {
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_ERROR,
                            "ResourceBindingHandlder::UnbindUpdate: No such resource %s",
                            resource_name.c_str());
        }
        else
        {
            if (update_bindings_.count(resource_name) > 0)
            {
                resource->Unbind(update_bindings_[resource_name]);

                update_bindings_.erase(resource_name);
            }
            if (update_payload_bindings_.count(resource_name) > 0)
            {
                resource->Unbind(update_payload_bindings_[resource_name]);

                update_payload_bindings_.erase(resource_name);
            }
        }
    }

    return rc;
}



fscfg_ReturnCode ResourceBindingHandler::OnBoundResourceUpdated(ResourceInterface::UpdatedEvent updated_event)
{
    return fscfg_kRcNotImplemented;
}

fscfg_ReturnCode ResourceBindingHandler::OnBoundResourcePayloadChanged(ResourceInterface::PayloadsChangedEvent payloads_changed_event)
{
    return fscfg_kRcNotImplemented;
}

} // namespace fsm

/** \}    end of addtogroup */
