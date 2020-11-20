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
 *  \file     source_proxy.cpp
 *  \brief    Foundation Services Source proxy implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <libfscfg/source_proxy.h>

#include <fscfg/resource_interface.h>
#include <fscfg/fscfg.h>
#include <fscfg/bus_names.h>

#include <fscfg_common/utilities.h>

#include <libfscfg/config_proxy.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

SourceProxy::SourceProxy(std::shared_ptr<fsm::ConfigProxy> config_proxy, _ConfigSource* source_dbus_obj)
 : config_proxy_(config_proxy),
   source_dbus_obj_(source_dbus_obj)
{
    fscfg_ReturnCode rc = ConnectSignals();

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to connect SourceProxy signals on Dbus");
    }
    else
    {
        // Logging.
        std::string source_name;

        GetName(source_name);

        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO, "Source::Source(%s)", source_name.c_str());
    }
}

SourceProxy::~SourceProxy()
{
}

fscfg_ReturnCode SourceProxy::OnUpdated(std::vector<std::shared_ptr<ResourceInterface>> resources)
{
    return SourceBind::OnUpdated(resources);
}

fscfg_ReturnCode SourceProxy::GetName(std::string& name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!source_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        name.append(config_source_get_name(source_dbus_obj_));
    }

    return rc;
}

fscfg_ReturnCode SourceProxy::GetEncodings(std::shared_ptr<ResourceInterface> resource,
                                           std::vector<std::shared_ptr<EncodingInterface>>& encodings)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string resource_name;
        const char* config_container_base_path = nullptr;

        rc = resource->GetName(resource_name);
        rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&config_container_base_path, fscfg_kBotConfigResource) : rc;

        if (rc == fscfg_kRcSuccess)
        {
            std::string resource_obj_path = std::string(config_container_base_path) + "/" + resource_name;

            char** encoding_obj_paths = nullptr;
            GError *error = nullptr;
            gboolean result = false;

            result = config_source_call_get_encodings_sync(source_dbus_obj_,
                                                           resource_obj_path.c_str(),
                                                           &encoding_obj_paths,
                                                           nullptr,
                                                           &error);

            if (!encoding_obj_paths || !result)
            {
                if (error)
                {
                    DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                    "Failed to get source proxy encodings from Dbus, error: %s",
                                    error->message);
                    g_clear_error (&error);
                }
                else
                {
                    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get source proxy encodings from Dbus.");
                }

                rc = fscfg_kRcError;
            }
            else
            {
                char** encoding_names = encoding_obj_paths;

                while (encoding_names && *encoding_names)
                {
                    std::string encoding_name;
                    std::shared_ptr<EncodingInterface> encoding;

                    GetBasename(*encoding_names, encoding_name);
                    rc = config_proxy_->Get(encoding_name, encoding);

                    if (rc != fscfg_kRcSuccess)
                    {
                        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                        "Failed to get encoding object for encoding name: %s",
                                        encoding_name.c_str());
                        break;
                    }

                    encodings.push_back(encoding);
                    ++encoding_names;
                }
            }

            if (encoding_obj_paths)
            {
                g_free(encoding_obj_paths);
            }
        }
    } // else {if (!resource)}

    return rc;
}

fscfg_ReturnCode SourceProxy::GetPayload(std::shared_ptr<ResourceInterface> resource,
                                         std::shared_ptr<EncodingInterface> encoding,
                                         std::vector<std::uint8_t>& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource || !encoding)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string resource_name;
        std::string encoding_name;

        const char* config_resource_base_path = nullptr;
        const char* config_encoding_base_path = nullptr;

        // Get the encoding and resource name
        rc = resource->GetName(resource_name);
        rc = rc == fscfg_kRcSuccess ? encoding->GetName(encoding_name) : rc;

        // Get the encoding and resource object base path
        rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&config_resource_base_path, fscfg_kBotConfigResource) : rc;
        rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&config_encoding_base_path, fscfg_kBotConfigEncoding) : rc;

        if (rc == fscfg_kRcSuccess)
        {
            //Construct the path with object name
            std::string resource_obj_path = std::string(config_resource_base_path) + "/" + resource_name;
            std::string encoding_obj_path = std::string(config_encoding_base_path) + "/" + encoding_name;

            GVariant* g_payload = nullptr;
            GError *error = nullptr;
            gboolean result = false;

            // Get the payload from D-Bus
            result = config_source_call_get_payload_sync(source_dbus_obj_,
                                                         resource_obj_path.c_str(),
                                                         encoding_obj_path.c_str(),
                                                         &g_payload,
                                                         nullptr,
                                                         &error);

            if (!g_payload || !result)
            {
                if (error)
                {
                    DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                    "Failed to get source proxy payload from Dbus, error: %s",
                                    error->message);
                    g_clear_error (&error);
                }
                else
                {
                    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get source proxy payload from Dbus.");
                }

                rc = fscfg_kRcError;
            }
            else
            {
                // Convert G_Variant payload type to vector of uint8_t.
                ConvertPayload(g_payload, payload);
            }

            if (g_payload)
            {
                g_variant_unref(g_payload);
            }
        }
    } // else {if (!resource || !encoding)}

    return rc;
}

fscfg_ReturnCode SourceProxy::HasResource(std::shared_ptr<ResourceInterface> resource, bool& has_resource)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!source_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        std::string resource_name;
        const char* config_resource_base_path = nullptr;

        rc = resource->GetName(resource_name);

        // Get the resource object base path
        rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&config_resource_base_path, fscfg_kBotConfigResource) : rc;

        if (rc == fscfg_kRcSuccess)
        {
            // Construct resource object path with object name
            std::string resource_obj_path = std::string(config_resource_base_path) + "/" + resource_name;

            GError* error = nullptr;
            gboolean result = false;
            gboolean has_resource_dbus;

            // Call Dbus to get has_resource for the given resource path
            result = config_source_call_has_resource_sync(source_dbus_obj_,
                                                          resource_obj_path.c_str(),
                                                          &has_resource_dbus,
                                                          nullptr,
                                                          &error);

            // Check Dbus return value
            if (!result)
            {
                // Check if error is set
                if (error)
                {
                    DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                    "Failed to get source has_resource from Dbus, error: %s",
                                    error->message);
                    g_clear_error (&error);
                }
                else
                {
                    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to get source has_resource from Dbus.");
                }

                rc = fscfg_kRcError;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO, "Resource(%s): has the resource", resource_name.c_str());
                has_resource = has_resource_dbus;
            }
        }
    }

    return rc;
}

fscfg_ReturnCode SourceProxy::ConnectSignals()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!source_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        if (!g_signal_connect(source_dbus_obj_, "updated", G_CALLBACK(SourceProxy::OnUpdatedDbus), this))
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to register SourceProxy OnUpdatedDbus callback on Dbus.");
            rc = fscfg_kRcError;
        }
    }

    return rc;
}

gboolean SourceProxy::OnUpdatedDbus(_ConfigSource* object, const gchar *const *arg_resources, SourceProxy* source_proxy)
{
    gboolean result = TRUE;

    if (!object || !source_proxy)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "SourceProxy::OnUpdatedDbus: bad params");
        result = FALSE;
    }
    else
    {
        std::vector<std::shared_ptr<ResourceInterface>> updated_resources;

        while (arg_resources && *arg_resources)
        {
            std::string name;
            std::shared_ptr<ResourceInterface> current_resource;

            GetBasename(*arg_resources, name);

            fscfg_ReturnCode rc = source_proxy->config_proxy_->Get(name, current_resource);

            if (rc != fscfg_kRcSuccess)
            {
                DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                "Failed to get resource proxy by name: %s",
                                name.c_str());
                result = FALSE;
                break;
            }

            updated_resources.push_back(current_resource);
            ++arg_resources;
        }

        if (result)
        {
            // Call the bindings
            source_proxy->OnUpdated(updated_resources);
        }
    }

    return result;
}

}

/** \}    end of addtogroup */
