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
 *  \file     config_proxy.cpp
 *  \brief    Foundation Services Config proxy implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <libfscfg/config_proxy.h>

#include <dlt/dlt.h>

#include <fscfg_common/utilities.h>
#include <fscfg/bus_names.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

#include <libfscfg/encoding_proxy.h>
#include <libfscfg/source_proxy.h>
#include <libfscfg/resource_proxy.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

ConfigProxy::ConfigProxy()
    : sync_(),
      objmgr_encoding_(),
      objmgr_source_(),
      objmgr_resource_(),
      config_dbus_obj__(nullptr)
{
    const char* bus_name = nullptr;
    const char* container_path = nullptr;

    fscfg_ReturnCode rc = fscfg_GetBusName(&bus_name);
    rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&container_path, fscfg_kBotConfig) : rc;

    if (rc == fscfg_kRcSuccess)
    {
        GError *error = nullptr;

        config_dbus_obj__ = config_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                         G_DBUS_PROXY_FLAGS_NONE,
                                                         bus_name,
                                                         container_path,
                                                         NULL,
                                                         &error);

        if (!config_dbus_obj__)
        {
            if (error)
            {
                DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                "Failed to create config proxy dbus object, error: %s",
                                error->message);
                g_clear_error (&error);
            }
            else
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to create config proxy dbus object");
            }
        }
    }
}

ConfigProxy::~ConfigProxy()
{
}

fscfg_ReturnCode ConfigProxy::Init()
{
    const char* bus_name = nullptr;

    const char* encoding_path = nullptr;
    const char* source_path = nullptr;
    const char* resource_path = nullptr;

    fscfg_ReturnCode rc = fscfg_GetBusName(&bus_name);

    rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&encoding_path, fscfg_kBotConfigEncoding) : rc;
    rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&source_path, fscfg_kBotConfigSource) : rc;
    rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&resource_path, fscfg_kBotConfigResource) : rc;

    if (rc == fscfg_kRcSuccess)
    {
        objmgr_encoding_ = std::make_shared<ObjMgrEncoding>(shared_from_this(),
                                                            sync_,
                                                            object_get_config_encoding,
                                                            bus_name,
                                                            encoding_path);

        objmgr_source_ = std::make_shared<ObjMgrSource>(shared_from_this(),
                                                        sync_,
                                                        object_get_config_source,
                                                        bus_name,
                                                        source_path);

        objmgr_resource_ = std::make_shared<ObjMgrResource>(shared_from_this(),
                                                            sync_,
                                                            object_get_config_resource,
                                                            bus_name,
                                                            resource_path);

        if (!objmgr_encoding_ || !objmgr_source_ || !objmgr_resource_)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to create object manager objects.");
            rc = fscfg_kRcBadState;
        }
        else
        {
            // Update all config proxy resources
            rc = rc == fscfg_kRcSuccess ? Update() : rc;
            if (rc == fscfg_kRcSuccess)
            {
                if (!g_signal_connect(config_dbus_obj__,
                                      "g-properties-changed",
                                      G_CALLBACK(ConfigProxy::OnPropertiesChanged),
                                      this))
                {
                    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                                   "Failed to register ConfigProxy OnPropertiesChanged callback on Dbus.");
                    rc = fscfg_kRcError;
                }
            }
        }
    }

    return rc;
}

fscfg_ReturnCode ConfigProxy::OnEncodingsChanged(std::vector<std::shared_ptr<EncodingInterface>> encodings)
{
    return ConfigBind::OnEncodingsChanged(encodings);
}

fscfg_ReturnCode ConfigProxy::OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources)
{
    return ConfigBind::OnSourcesChanged(sources);
}

fscfg_ReturnCode ConfigProxy::OnResourcesChanged(std::vector<std::shared_ptr<ResourceInterface>> resources)
{
    return ConfigBind::OnResourcesChanged(resources);
}

fscfg_ReturnCode ConfigProxy::GetAll(std::vector<std::shared_ptr<EncodingInterface>>& encodings)
{
    return GetAllFromManager(objmgr_encoding_, sync_, encodings);
}

fscfg_ReturnCode ConfigProxy::GetAll(std::vector<std::shared_ptr<SourceInterface>>& sources)
{
    return GetAllFromManager(objmgr_source_, sync_, sources);
}

fscfg_ReturnCode ConfigProxy::GetAll(std::vector<std::shared_ptr<ResourceInterface>>& resources)
{
    return GetAllFromManager(objmgr_resource_, sync_, resources);
}

fscfg_ReturnCode ConfigProxy::Get(const std::string& name, std::shared_ptr<EncodingInterface>& encoding)
{
    return GetFromManager(objmgr_encoding_, name, sync_, encoding);
}

fscfg_ReturnCode ConfigProxy::Get(const std::string& name, std::shared_ptr<SourceInterface>& source)
{
    return GetFromManager(objmgr_source_, name, sync_, source);
}

fscfg_ReturnCode ConfigProxy::Get(const std::string& name, std::shared_ptr<ResourceInterface>& resource)
{
    return GetFromManager(objmgr_resource_, name, sync_, resource);
}

fscfg_ReturnCode ConfigProxy::Get(const std::string& name, std::shared_ptr<EncodingProxy>& encoding)
{
    return GetFromManager(objmgr_encoding_, name, sync_, encoding);
}

fscfg_ReturnCode ConfigProxy::Get(const std::string& name, std::shared_ptr<SourceProxy>& source)
{
    return GetFromManager(objmgr_source_, name, sync_, source);
}

fscfg_ReturnCode ConfigProxy::Get(const std::string& name, std::shared_ptr<ResourceProxy>& resource)
{
    return GetFromManager(objmgr_resource_, name, sync_, resource);
}

void ConfigProxy::OnPropertiesChanged(GDBusProxy* proxy,
                                      GVariant* changed,
                                      GStrv invalidated,
                                      gpointer config_instance)
{
    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Received ConfigProxy OnPropertiesChanged callback");

    if (!config_instance)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to execute ConfigProxy OnPropertiesChanged, invalid input.");
    }
    else
    {
        ConfigProxy* config_proxy = reinterpret_cast<ConfigProxy*>(config_instance);

        if (config_proxy)
        {
            if (g_variant_n_children(changed) > 0)
            {
                fscfg_ReturnCode rc;

                // Extract property names.
                GVariantIter* iter = nullptr;
                const gchar* key = nullptr;
                GVariant* value = nullptr;

                g_variant_get(changed, "a{sv}", &iter);

                // Check which property has changed, update it and call the bindings.
                while (g_variant_iter_loop(iter, "{&sv}", &key, &value))
                {
                    std::string key_str(key);

                    if (key_str == "Encodings")
                    {
                        // Update Encodings
                        rc = config_proxy->UpdateEncodings();

                        if (rc != fscfg_kRcSuccess)
                        {
                            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                                           "ConfigProxy::OnPropertiesChanged Failed to update encodings");
                        }
                    }
                    else if (key_str == "Sources")
                    {
                        // Update Sources
                        rc = config_proxy->UpdateSources();

                        if (rc != fscfg_kRcSuccess)
                        {
                            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                                           "ConfigProxy::OnPropertiesChanged Failed to update sources");
                        }
                    }
                    else if (key_str == "Resources")
                    {
                        // Update Resources
                        rc = config_proxy->UpdateResources();

                        if (rc != fscfg_kRcSuccess)
                        {
                            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                                           "ConfigProxy::OnPropertiesChanged Failed to update resources");
                        }
                    }
                    else
                    {
                        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_WARN,
                                        "ConfigProxy::OnPropertiesChanged invalid property received: %s",
                                        key_str.c_str());
                    }
                }

                g_variant_iter_free(iter);
            }
        }
    }
}

fscfg_ReturnCode ConfigProxy::Update()
{
    // Update all config proxy resources
    fscfg_ReturnCode rc = UpdateEncodings();
    rc = rc == fscfg_kRcSuccess ? UpdateSources() : rc;
    rc = rc == fscfg_kRcSuccess ? UpdateResources() : rc;

    return rc;
}

fscfg_ReturnCode ConfigProxy::UpdateEncodings()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!objmgr_encoding_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Updating ConfigProxy encodings");

        std::vector<std::string> new_names;
        std::vector<std::string> deleted_names;

        objmgr_encoding_->UpdateContainer(new_names, deleted_names);

        if (new_names.size() != 0
            || deleted_names.size() !=0)
        {
            std::vector<std::shared_ptr<EncodingProxy>> encodings;

            fsm::MapToValuesVector(objmgr_encoding_->set, encodings);
            std::vector<std::shared_ptr<EncodingInterface>> encodings_bases(encodings.begin(), encodings.end());

            OnEncodingsChanged(encodings_bases);
        }
    }

    return rc;
}

fscfg_ReturnCode ConfigProxy::UpdateSources()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!objmgr_source_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Updating ConfigProxy sources");

        std::vector<std::string> new_names;
        std::vector<std::string> deleted_names;

        objmgr_source_->UpdateContainer(new_names, deleted_names);

        if (new_names.size() != 0
            || deleted_names.size() !=0)
        {
            std::vector<std::shared_ptr<SourceProxy>> sources;

            fsm::MapToValuesVector(objmgr_source_->set, sources);
            std::vector<std::shared_ptr<SourceInterface>> sources_bases(sources.begin(), sources.end());

            OnSourcesChanged(sources_bases);
        }
    }

    return rc;
}

fscfg_ReturnCode ConfigProxy::UpdateResources()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!objmgr_resource_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Updating ConfigProxy resources");

        std::vector<std::string> new_names;
        std::vector<std::string> deleted_names;

        objmgr_resource_->UpdateContainer(new_names, deleted_names);

        if (new_names.size() != 0
            || deleted_names.size() !=0)
        {
            std::vector<std::shared_ptr<ResourceProxy>> resources;

            fsm::MapToValuesVector(objmgr_resource_->set, resources);
            std::vector<std::shared_ptr<ResourceInterface>> resources_bases(resources.begin(), resources.end());

            OnResourcesChanged(resources_bases);
        }
    }

    return rc;
}

} // namespace fsm

/** \}    end of addtogroup */
