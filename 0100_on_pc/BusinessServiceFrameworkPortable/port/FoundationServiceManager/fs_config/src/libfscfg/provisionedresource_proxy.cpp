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
 *  \file     provisionedresource_proxy.cpp
 *  \brief    Foundation Services Resource proxy implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <libfscfg/provisionedresource_proxy.h>

#include <libfscfg/provisioning_proxy.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

ProvisionedResourceProxy::ProvisionedResourceProxy(std::shared_ptr<fsm::ProvisioningProxy> provisioning_proxy,
                                                   std::shared_ptr<ResourceProxy> base,
                                                   ConfigProvisionedResource* provisioned_resource_dbus_obj)
    : ResourceProxy(*base),
      provisioning_proxy_(provisioning_proxy),
      provisioned_resource_dbus_obj_(provisioned_resource_dbus_obj),
      base_(base)
{
    fscfg_ReturnCode rc = ConnectSignals();

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to connect ProvisionedResourceProxy signals on Dbus");
    }
}

ProvisionedResourceProxy::~ProvisionedResourceProxy()
{
}

fscfg_ReturnCode ProvisionedResourceProxy::GetName(std::string& name)
{
    return base_->GetName(name);
}

fscfg_ReturnCode ProvisionedResourceProxy::GetPayload(std::vector<std::uint8_t>& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!provisioned_resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        std::shared_ptr<SourceInterface> source;
        std::shared_ptr<EncodingInterface> encoding;

        GetSource(source);
        GetEncoding(encoding);

        if (!source || !encoding)
        {
            // Logging.
            std::string resource_name;

            GetName(resource_name);

            DLT_LOG_STRINGF(dlt_libfscfg,
                            DLT_LOG_INFO,
                            "ProvisionedResourceProxy(%s)::GetPayload: bad source or encoding",
                            resource_name.c_str());

            rc = fscfg_kRcNotFound;
        }
        else
        {
            rc = ResourceProxy::GetPayload(encoding, source, payload);
        }
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResourceProxy::GetSource(std::shared_ptr<SourceInterface>& source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!provisioned_resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        std::string source_name;
        const char* source_obj_path = nullptr;

        source_obj_path = config_provisioned_resource_get_source(provisioned_resource_dbus_obj_);

        if (!source_obj_path)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to get provisioned resource's source from Dbus.");
            rc = fscfg_kRcError;
        }
        else
        {
            GetBasename(source_obj_path, source_name);

            std::shared_ptr<fsm::ConfigProxy> config_proxy = provisioning_proxy_->GetConfig();

            rc = config_proxy->Get(source_name, source);

            if (!source)
            {
                DLT_LOG_STRINGF(dlt_libfscfg,
                                DLT_LOG_ERROR,
                                "Failed to get provisioned resource's source by name: %s",
                                source_obj_path);
            }
        }
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResourceProxy::GetEncoding(std::shared_ptr<EncodingInterface>& encoding)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!provisioned_resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        std::string encoding_name;
        const char* encoding_obj_path = nullptr;

        encoding_obj_path = config_provisioned_resource_get_encoding(provisioned_resource_dbus_obj_);

        if (!encoding_obj_path)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to get provisioned resource's encoding from Dbus.");

            rc = fscfg_kRcError;
        }
        else
        {
            GetBasename(encoding_obj_path, encoding_name);

            std::shared_ptr<fsm::ConfigProxy> config_proxy = provisioning_proxy_->GetConfig();

            rc = config_proxy->Get(encoding_name, encoding);

            if (!encoding)
            {
                DLT_LOG_STRINGF(dlt_libfscfg,
                                DLT_LOG_ERROR,
                                "Failed to get provisioned resource's encoding by name: %s",
                                encoding_obj_path);
            }
        }
    }

    return rc;
}

fscfg_ReturnCode ProvisionedResourceProxy::GetObject(_ConfigProvisionedResource*& resource)
{
    resource = provisioned_resource_dbus_obj_;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisionedResourceProxy::OnPayloadChanged(std::vector<std::uint8_t> payload)
{
    return ProvisionedResourceBind::OnPayloadChanged(payload);
}

fscfg_ReturnCode ProvisionedResourceProxy::OnSourceChanged(std::shared_ptr<SourceInterface> source)
{
    return ProvisionedResourceBind::OnSourceChanged(source);
}

fscfg_ReturnCode ProvisionedResourceProxy::ConnectSignals()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!provisioned_resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        if (!g_signal_connect(provisioned_resource_dbus_obj_,
                              "g-properties-changed",
                              G_CALLBACK(ProvisionedResourceProxy::OnPropertiesChanged),
                              this))
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to register ProvisionedResourceProxy OnPropertiesChanged callback on Dbus.");
            rc = fscfg_kRcError;
        }
    }

    return rc;
}

void ProvisionedResourceProxy::OnPropertiesChanged(GDBusProxy* proxy,
                                                   GVariant* changed,
                                                   GStrv invalidated,
                                                   gpointer provisioned_resource_instance)
{
    if (!provisioned_resource_instance)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to execute ProvisionedResourceProxy OnPropertiesChanged, invalid input.");
    }
    else
    {
        ProvisionedResourceProxy* provisioned_resource_proxy =
                reinterpret_cast<ProvisionedResourceProxy*>(provisioned_resource_instance);

        if (g_variant_n_children(changed) > 0)
        {
            // Extract property names.
            GVariantIter* iter = nullptr;
            const gchar* key = nullptr;
            GVariant* value = nullptr;

            g_variant_get(changed, "a{sv}", &iter);

            // Check which property has changed, get the value and call the bindings.
            while (g_variant_iter_loop(iter, "{&sv}", &key, &value))
            {
                std::string key_str(key);

                if (key_str == "Source")
                {
                    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "ProvisionedResourceProxy:: Source prop updated");

                    std::shared_ptr<SourceInterface> source;
                    const char* source_path;
                    std::string source_name;

                    g_variant_get(value, "o", &source_path);
                    GetBasename(source_path, source_name);

                    fscfg_ReturnCode rc = provisioned_resource_proxy->config_proxy_->Get(source_name, source);

                    if (rc != fscfg_kRcSuccess)
                    {
                        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                                       "Failed to get provisioned resource source");
                    }
                    else
                    {
                        provisioned_resource_proxy->OnSourceChanged(source);
                    }
                }
                else if (key_str == "Payload")
                {
                        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "ProvisionedResourceProxy:: Payload prop updated");

                    if (!value)
                    {
                        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                                       "Failed to get retrieve payload as GVariant");
                    }
                    else
                    {
                        std::vector<std::uint8_t> payload;

                        ConvertPayload(value, payload);
                        provisioned_resource_proxy->OnPayloadChanged(payload);
                    }
                }
                else
                {
                    DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_WARN,
                                    "ProvisionedResourceProxy::OnPropertiesChanged invalid property received: %s",
                                    key_str.c_str());
                }
            }

            g_variant_iter_free(iter);
        }
    }
}

} // namespace fsm

/** \}    end of addtogroup */
