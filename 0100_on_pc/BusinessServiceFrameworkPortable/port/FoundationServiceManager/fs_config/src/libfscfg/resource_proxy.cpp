/**
 * Copyright (C) 2017, 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     resource_proxy.cpp
 *  \brief    Foundation Services Resource proxy implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <libfscfg/resource_proxy.h>

#include <fscfg/fscfg.h>

#include <fscfg/bus_names.h>
#include <fscfg_common/utilities.h>

#include <libfscfg/config_proxy.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

ResourceProxy::ResourceProxy(std::shared_ptr<fsm::ConfigProxy> config_proxy, _ConfigResource* resource_dbus_obj)
 : config_proxy_(config_proxy),
   resource_dbus_obj_(resource_dbus_obj)
{
    fscfg_ReturnCode rc = ConnectSignals();

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to connect ResourceProxy signals on Dbus");
    }
}

ResourceProxy::~ResourceProxy()
{
}

fscfg_ReturnCode ResourceProxy::GetName(std::string& name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        name.append(config_resource_get_name(resource_dbus_obj_));
    }

    return rc;
}

fscfg_ReturnCode ResourceProxy::GetSources(std::vector<std::shared_ptr<SourceInterface>>& sources)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        const char* const *source_obj_paths = nullptr;

        source_obj_paths = config_resource_get_sources(resource_dbus_obj_);

        if (!source_obj_paths)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to get resource's sources from Dbus.");
            rc = fscfg_kRcError;
        }
        else
        {
            while (source_obj_paths && *source_obj_paths)
            {
                std::string source_name;
                std::shared_ptr<SourceInterface> source;

                GetBasename(*source_obj_paths, source_name);
                rc = config_proxy_->Get(source_name, source);

                if (rc != fscfg_kRcSuccess)
                {
                    DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                    "Failed to get source object for source name: %s",
                                    source_name.c_str());
                    break;
                }

                sources.push_back(source);
                ++source_obj_paths;
            }
        }
    } // if (!resource_dbus_obj_)

    return rc;
}

fscfg_ReturnCode ResourceProxy::GetEncodings(std::vector<std::shared_ptr<EncodingInterface>>& encodings)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        const char* const* encoding_obj_paths = nullptr;

        encoding_obj_paths = config_resource_get_encodings(resource_dbus_obj_);

        if (!encoding_obj_paths)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to get resource's encodings from Dbus.");
            rc = fscfg_kRcError;
        }
        else
        {
            while (encoding_obj_paths && *encoding_obj_paths)
            {
                std::string encoding_name;
                std::shared_ptr<EncodingInterface> encoding;

                GetBasename(*encoding_obj_paths, encoding_name);
                rc = config_proxy_->Get(encoding_name, encoding);

                if (rc != fscfg_kRcSuccess)
                {
                    DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                    "Failed to get encoding object for encoding name: %s",
                                    encoding_name.c_str());
                    break;
                }

                encodings.push_back(encoding);
                ++encoding_obj_paths;
            }
        }
    }

    return rc;
}

fscfg_ReturnCode ResourceProxy::GetPayload(std::shared_ptr<EncodingInterface> encoding,
                                           std::shared_ptr<SourceInterface> source,
                                           std::vector<std::uint8_t>& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    std::string resource_name;
    GetName(resource_name);


    if (!encoding || !source)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string encoding_name;
        std::string source_name;

        const char* config_encoding_base_path = nullptr;
        const char* config_source_base_path = nullptr;

        // Get the encoding and source name
        rc = encoding->GetName(encoding_name);
        rc = rc == fscfg_kRcSuccess ? source->GetName(source_name) : rc;

        // Get the encoding and source object base path
        rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&config_encoding_base_path, fscfg_kBotConfigEncoding) : rc;
        rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&config_source_base_path, fscfg_kBotConfigSource) : rc;

        if (rc == fscfg_kRcSuccess)
        {
            //Construct the path with object name
            std::string encoding_obj_path = std::string(config_encoding_base_path) + "/" + encoding_name;
            std::string source_obj_path = std::string(config_source_base_path) + "/" + source_name;

            GVariant* g_payload = nullptr;
            GError* error = nullptr;
            gboolean result = false;

            // Log attempt.
            DLT_LOG_STRINGF(dlt_libfscfg,
                            DLT_LOG_INFO,
                            "ResourceProxy(%s)::GetPayload: DBus call to get_payload... encoding path=%s source path=%s",
                            resource_name.c_str(),
                            encoding_obj_path.c_str(),
                            source_obj_path.c_str());

            // Get the payload from D-Bus
            result = config_resource_call_get_payload_sync(resource_dbus_obj_,
                                                           encoding_obj_path.c_str(),
                                                           source_obj_path.c_str(),
                                                           &g_payload,
                                                           nullptr,
                                                           &error);

            if (!g_payload || !result)
            {
                if (error)
                {
                    DLT_LOG_STRINGF(dlt_libfscfg,
                                    DLT_LOG_ERROR,
                                    "ResourceProxy(%s)::GetPayload: Failed to get resource payload from Dbus, error: %s",
                                    resource_name.c_str(),
                                    error->message);

                    g_clear_error (&error);
                }
                else
                {
                    DLT_LOG_STRINGF(dlt_libfscfg,
                                   DLT_LOG_ERROR,
                                   "ResourceProxy(%s)::GetPayload: Failed to get resource proxy payload from Dbus.",
                                    resource_name.c_str());
                }

                rc = fscfg_kRcError;
            }
            else
            {
                // Convert G_Variant payload to bytes.
                ConvertPayload(g_payload, payload);
            }

            if (g_payload)
            {
                g_variant_unref(g_payload);
            }
        }
    } // else {if (!encoding || !source)}

    DLT_LOG_STRINGF(dlt_libfscfg,
                    DLT_LOG_INFO,
                    "ResourceProxy(%s)::GetPayload: Returning with size=%u rc=%u",
                    resource_name.c_str(),
                    payload.size(),
                    static_cast<unsigned>(rc));

    return rc;
}


fscfg_ReturnCode ResourceProxy::OnPayloadsChanged(std::vector<std::uint8_t> payload,
                                                  std::shared_ptr<SourceInterface> source,
                                                  std::shared_ptr<EncodingInterface> encoding)
{
    return ResourceBind::OnPayloadsChanged(payload, source, encoding);
}

fscfg_ReturnCode ResourceProxy::OnExpired(bool immediate)
{
    return ResourceBind::OnExpired(immediate);
}

fscfg_ReturnCode ResourceProxy::OnUpdated(std::shared_ptr<SourceInterface> source)
{
    return ResourceBind::OnUpdated(source);
}

fscfg_ReturnCode ResourceProxy::OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources)
{
    return ResourceBind::OnSourcesChanged(sources);
}

fscfg_ReturnCode ResourceProxy::ConnectSignals()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        if (!g_signal_connect(resource_dbus_obj_, "updated", G_CALLBACK(ResourceProxy::OnUpdatedDbus), this))
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to register ResourceProxy OnUpdatedDbus callback on Dbus.");
            rc = fscfg_kRcError;
        }

        if (!g_signal_connect(resource_dbus_obj_, "expired", G_CALLBACK(ResourceProxy::OnExpiredDbus), this))
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to register ResourceProxy OnExpiredDbus callback on Dbus.");
            rc = fscfg_kRcError;
        }

        if (!g_signal_connect(resource_dbus_obj_,
                              "payloads-changed",
                              G_CALLBACK(ResourceProxy::OnPayloadsChangedDbus),
                              this))
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to register ResourceProxy OnPayloadsChangedDbus callback on Dbus.");
            rc = fscfg_kRcError;
        }

        if (!g_signal_connect(resource_dbus_obj_,
                              "g-properties-changed",
                              G_CALLBACK(ResourceProxy::OnPropertiesChanged),
                              this))
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to register ResourceProxy OnPropertiesChanged callback on Dbus.");
            rc = fscfg_kRcError;
        }
    }

    return rc;
}

gboolean ResourceProxy::OnUpdatedDbus(_ConfigResource* object, const gchar* arg_source, ResourceProxy* resource_proxy)
{
    gboolean result = TRUE;

    if (!object || !resource_proxy)
    {
        DLT_LOG_STRING(dlt_libfscfg,
                       DLT_LOG_ERROR,
                       "ResourceProxy::OnUpdatedDbus:Failed, invalid input.");

        result = FALSE;
    }
    else
    {
        std::string resource_name;
        resource_proxy->GetName(resource_name);

        DLT_LOG_STRINGF(dlt_libfscfg,
                       DLT_LOG_INFO,
                       "ResourceProxy(%s)::OnUpdatedDbus: enter callback",
                        resource_name.c_str());

        std::string source_name;
        std::shared_ptr<SourceInterface> current_source;

        GetBasename(arg_source, source_name);

        fscfg_ReturnCode rc = resource_proxy->config_proxy_->Get(source_name, current_source);

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRINGF(dlt_libfscfg,
                            DLT_LOG_ERROR,
                            "ResourceProxy(%s)::OnUpdatedDbus:Failed to get source proxy by name: %s",
                            resource_name.c_str(),
                            source_name.c_str());

            result = FALSE;
        }
        else
        {
            fscfg_ReturnCode rc = resource_proxy->OnUpdated(current_source);

            if (rc != fscfg_kRcSuccess)
            {
                DLT_LOG_STRINGF(dlt_libfscfg,
                                DLT_LOG_ERROR,
                                "ResourceProxy(%s)::OnUpdatedDbus: Failed with rc=%u",
                                resource_name.c_str(),
                                static_cast<unsigned>(rc));

                result = FALSE;
            }
        }
    }

    return result;
}

gboolean ResourceProxy::OnExpiredDbus(_ConfigResource* object, gboolean arg_immediate, ResourceProxy* resource_proxy)
{
    gboolean result = TRUE;

    if (!object || !resource_proxy)
    {
        DLT_LOG_STRING(dlt_libfscfg,
                       DLT_LOG_ERROR,
                       "ResourceProxy::OnExpiredDbus:Failed, invalid input.");
        result = FALSE;
    }
    else
    {
        std::string resource_name;
        resource_proxy->GetName(resource_name);

        DLT_LOG_STRINGF(dlt_libfscfg,
                        DLT_LOG_INFO,
                        "ResourceProxy(%s)::OnExpiredDbus: enter callback",
                        resource_name.c_str());

        fscfg_ReturnCode rc = resource_proxy->OnExpired(arg_immediate);

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRINGF(dlt_libfscfg,
                            DLT_LOG_ERROR,
                            "ResourceProxy(%s)::OnExpiredDbus: OnExpired failed.",
                            resource_name.c_str());

            result = FALSE;
        }
    }

    return result;
}

gboolean ResourceProxy::OnPayloadsChangedDbus(_ConfigResource* object,
                                              const gchar *encoding_path,
                                              const gchar *source_path,
                                              GVariant *g_payload,
                                              ResourceProxy* resource_proxy)
{


    gboolean result = TRUE;

    if (!object || !resource_proxy || !encoding_path || !source_path || !g_payload)
    {
        DLT_LOG_STRING(dlt_libfscfg,
                       DLT_LOG_ERROR,
                       "ResourceProxy::OnPayloadsChangedDbus: Failed, invalid input.");
        result = FALSE;
    }
    else
    {
        std::shared_ptr<EncodingInterface> encoding;
        std::shared_ptr<SourceInterface> source;
        std::vector<std::uint8_t> payload;

        std::string resource_name;
        std::string encoding_name;
        std::string source_name;

        resource_proxy->GetName(resource_name);

        DLT_LOG_STRINGF(dlt_libfscfg,
                        DLT_LOG_INFO,
                        "ResourceProxy(%s)::OnPayloadsChangedDbus: enter callback",
                        resource_name.c_str());

        // Get the base name
        GetBasename(encoding_path, encoding_name);
        GetBasename(source_path, source_name);

        // Get instances by name
        resource_proxy->config_proxy_->Get(encoding_name, encoding);
        resource_proxy->config_proxy_->Get(source_name, source);

        if (!encoding || !source)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to execute callback OnPayloadsChangedDbus, bad objects on path.");
        }
        else
        {
            // Convert G_Variant payload to bytes.
            ConvertPayload(g_payload, payload);

            // Call the bindings
            resource_proxy->OnPayloadsChanged(payload, source, encoding);
        }
    }

    if (g_payload)
    {
        g_variant_unref(g_payload);
    }

    return result;
}

void ResourceProxy::OnPropertiesChanged(GDBusProxy* proxy,
                                        GVariant* changed,
                                        GStrv invalidated,
                                        gpointer resource_instance)
{
    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Received ResourceProxy OnPropertiesChanged callback");

    if (!resource_instance)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to execute ResourceProxy OnPropertiesChanged, invalid input.");
    }
    else
    {
        ResourceProxy* resource_proxy = reinterpret_cast<ResourceProxy*>(resource_instance);

        if (resource_proxy)
        {
            if (g_variant_n_children(changed) > 0)
            {
                // Extract property names.
                GVariantIter* iter = nullptr;
                const gchar* key = nullptr;
                GVariant* value = nullptr;

                g_variant_get(changed, "a{sv}", &iter);

                // Check which property has changed and call the bindings.
                while (g_variant_iter_loop(iter, "{&sv}", &key, &value))
                {
                    std::string key_str(key);

                    if (key_str == "Sources")
                    {
                       std::vector<std::shared_ptr<SourceInterface>> sources;

                       resource_proxy->config_proxy_->GetAll(sources);
                       resource_proxy->OnSourcesChanged(sources);
                    }
                    else
                    {
                        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_WARN,
                                        "ResourceProxy::OnPropertiesChanged invalid property received: %s",
                                        key_str.c_str());
                    }
                }

                g_variant_iter_free(iter);
            }
        }
    }
}

} // namespace fsm

/** \}    end of addtogroup */
