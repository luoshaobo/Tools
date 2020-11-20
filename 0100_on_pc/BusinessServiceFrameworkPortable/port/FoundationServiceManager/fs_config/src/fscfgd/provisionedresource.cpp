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
 *  \file     provisionedresource.cpp
 *  \brief    Foundation Services ProvisionedResource implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/provisionedresource.h>

#include <dlt/dlt.h>

#include <fscfg/bus_names.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd)

namespace fsm
{

ProvisionedResource::ProvisionedResource(const std::string& name, std::shared_ptr<Provisioning> provisioning)
 : Resource(name, provisioning->GetConfig()),
   provisioning_(provisioning),
   resource_dbus_obj_(nullptr),
   src_bind_id_(0),
   payload_bind_id_(0),
   last_top_source_(nullptr)
{
    resource_dbus_obj_ = config_provisioned_resource_skeleton_new();

    // Perform binds.
    BindSourcesChanged(std::bind(&ProvisionedResource::EventSourcesChanged, this, std::placeholders::_1),
                       src_bind_id_);

    BindPayloadsChanged(std::bind(&ProvisionedResource::EventPayloadsChanged, this, std::placeholders::_1),
                        payload_bind_id_);
}

ProvisionedResource::~ProvisionedResource()
{
    if (resource_dbus_obj_)
    {
        g_object_unref(resource_dbus_obj_);
    }

    Unbind(src_bind_id_);
    Unbind(payload_bind_id_);
}

fscfg_ReturnCode ProvisionedResource::GetName(std::string& name)
{
    return Resource::GetName(name);
}

fscfg_ReturnCode ProvisionedResource::GetPayload(std::vector<std::uint8_t>& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    std::shared_ptr<SourceInterface> source;
    std::shared_ptr<EncodingInterface> encoding;

    rc = GetSource(source);
    rc = rc == fscfg_kRcSuccess ? GetEncoding(encoding) : rc;

    if (rc == fscfg_kRcSuccess)
    {
        rc = source->GetPayload(std::dynamic_pointer_cast<ProvisionedResource>(shared_from_this()),
                                encoding,
                                payload);
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResource::GetSource(std::shared_ptr<SourceInterface>& source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!provisioning_)
    {
        rc = fscfg_kRcBadState; // provisioning is null.
    }
    else
    {
        rc = provisioning_->GetSource(std::dynamic_pointer_cast<ProvisionedResource>(shared_from_this()), source);
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResource::GetEncoding(std::shared_ptr<EncodingInterface>& encoding)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!provisioning_)
    {
        rc = fscfg_kRcBadState; // provisioning is null.
    }
    else
    {
        rc = provisioning_->GetEncoding(std::dynamic_pointer_cast<ProvisionedResource>(shared_from_this()),
                                        encoding);
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResource::GetObject(_ConfigProvisionedResource*& resource)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // resource_dbus_obj is null.
    }
    else
    {
        resource = resource_dbus_obj_;
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResource::OnPayloadChanged(std::vector<std::uint8_t> payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // resource_dbus_obj_ is null.
    }
    else
    {
        // Set payload on D-bus.
        GVariant* g_payload =
        g_variant_new_from_data(G_VARIANT_TYPE("ay"), &payload[0], payload.size(), TRUE, nullptr, nullptr);

        config_provisioned_resource_set_payload(resource_dbus_obj_, g_payload);
        g_dbus_interface_skeleton_flush(G_DBUS_INTERFACE_SKELETON(resource_dbus_obj_));

        // call bindings.
        ProvisionedResourceBind::OnPayloadChanged(payload);

        // Logging.
        std::shared_ptr<SourceInterface> src_if;
        std::shared_ptr<EncodingInterface> enc_if;

        std::string src_name;
        std::string enc_name;

        GetSource(src_if);
        GetEncoding(enc_if);

        src_if->GetName(src_name);
        enc_if->GetName(enc_name);

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "ProvisionedResource(%s): new payload from src :%s in enc: %s",
                        name_.c_str(),
                        src_name.c_str(),
                        enc_name.c_str());
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResource::OnSourceChanged(std::shared_ptr<SourceInterface> source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // resource_dbus_obj_ is null.
    }
    else
    {
        std::string src_full_path;      // full source path.
        std::string enc_full_path;      // full encoding path.
        std::string source_name;
        std::string encoding_name;
        const char* src_base_path;
        const char* enc_base_path;
        std::shared_ptr<EncodingInterface> encoding;

        fscfg_GetObjectPath(&src_base_path, fscfg_kBotConfigSource);
        fscfg_GetObjectPath(&enc_base_path, fscfg_kBotConfigEncoding);

        GetEncoding(encoding);

        source->GetName(source_name);
        encoding->GetName(encoding_name);

        src_full_path = std::string(src_base_path) + "/" + source_name;
        enc_full_path = std::string(enc_base_path) + "/" + encoding_name;

        // Set source on D-bus.
        config_provisioned_resource_set_source(resource_dbus_obj_, src_full_path.c_str());

        // Set encoding on D-bus.
        config_provisioned_resource_set_encoding(resource_dbus_obj_, enc_full_path.c_str());

        g_dbus_interface_skeleton_flush(G_DBUS_INTERFACE_SKELETON(resource_dbus_obj_));

        // call bindings.
        rc = ProvisionedResourceBind::OnSourceChanged(source);

        // Logging.
        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "ProvisionedResource(%s): new src: %s",
                        name_.c_str(),
                        source_name.c_str());
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResource::EventSourcesChanged(Resource::SourcesChangedEvent ev)
{
    std::shared_ptr<SourceInterface> top_source;

    std::string top_source_name;
    std::string last_top_source_name;

    GetSource(top_source);

    top_source->GetName(top_source_name);

    if (last_top_source_)
    {
        last_top_source_->GetName(last_top_source_name);
    }

    if (top_source_name != last_top_source_name)
    {
        // Only now can we say we have a new top source;
        // Notify all listeners.

        OnSourceChanged(top_source);
        last_top_source_ = top_source;
    }

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisionedResource::EventPayloadsChanged(Resource::PayloadsChangedEvent ev)
{
    std::shared_ptr<EncodingInterface> top_encoding;

    std::string top_encoding_name;
    std::string encoding_name;

    GetEncoding(top_encoding);

    ev.encoding->GetName(encoding_name);
    top_encoding->GetName(top_encoding_name);

    if (top_encoding_name == encoding_name)
    {
        // top encoding payload changed - notify listeners.
        OnPayloadChanged(ev.payload);

        // Logging.
        std::string source_name;
        std::string encoding_name;

        ev.source->GetName(source_name);
        ev.encoding->GetName(encoding_name);

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "ProvisionedResource(%s): new top payload from src:%s in enc:%s",
                        name_.c_str(),
                        source_name.c_str(),
                        encoding_name.c_str());
    }

    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
