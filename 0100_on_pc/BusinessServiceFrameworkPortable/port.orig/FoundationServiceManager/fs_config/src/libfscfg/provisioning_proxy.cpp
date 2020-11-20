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
 *  \file     provisioning_proxy.cpp
 *  \brief    Foundation Services Provisioning proxy implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <libfscfg/provisioning_proxy.h>

#include <fscfg_common/utilities.h>
#include <fscfg/bus_names.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

ProvisioningProxy::ProvisioningProxy(std::shared_ptr<ConfigProxy> config_proxy)
 : sync_(),
   config_proxy_(config_proxy),
   objmgr_provisioned_resource_(),
   provisioning_dbus_obj__(nullptr)
{
    const char* bus_name = nullptr;
    const char* container_path = nullptr;

    fscfg_ReturnCode rc = fscfg_GetBusName(&bus_name);
    rc = rc ==fscfg_kRcSuccess ? fscfg_GetObjectPath(&container_path, fscfg_kBotProvisioning) : rc;

    if (rc == fscfg_kRcSuccess)
    {
        GError *error = nullptr;

        provisioning_dbus_obj__ = provisioning_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                                     G_DBUS_PROXY_FLAGS_NONE,
                                                                     bus_name,
                                                                     container_path,
                                                                     NULL,
                                                                     &error);
        if (!provisioning_dbus_obj__)
        {
            if (error)
            {
                DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                "Failed to create provisioning proxy dbus object, error: %s",
                                error->message);

                g_clear_error(&error);
            }
            else
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to create provisioning proxy dbus object");
            }
        }
    }
}

ProvisioningProxy::~ProvisioningProxy()
{
}

fscfg_ReturnCode ProvisioningProxy::Init()
{
    const char* bus_name = nullptr;
    const char* resource_path = nullptr;

    fscfg_ReturnCode rc = fscfg_GetBusName(&bus_name);

    rc = fscfg_GetObjectPath(&resource_path, fscfg_kBotProvisioningResource);

    if (rc == fscfg_kRcSuccess)
    {
        objmgr_provisioned_resource_ = std::make_shared<ObjMgrResource>(shared_from_this(),
                                                                        sync_,
                                                                        config_proxy_,
                                                                        object_get_config_provisioned_resource,
                                                                        bus_name,
                                                                        resource_path);
        if (!objmgr_provisioned_resource_)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to create object manager provisioned resource object.");
            rc = fscfg_kRcError;
        }

        // Update the provisioning proxy resource
        rc = rc == fscfg_kRcSuccess ? Update() : rc;
        if (rc == fscfg_kRcSuccess)
        {
            if (!g_signal_connect(provisioning_dbus_obj__,
                                  "g-properties-changed",
                                  G_CALLBACK(ProvisioningProxy::OnPropertiesChanged),
                                  this))
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                               "Failed to register ProvisioningProxy OnPropertiesChanged callback on Dbus.");
                rc = fscfg_kRcError;
            }
        }
    }

    return rc;
}

fscfg_ReturnCode ProvisioningProxy::OnResourcesChanged(
                                    std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources)
{
    return ProvisioningBind::OnResourcesChanged(resources);
}

fscfg_ReturnCode ProvisioningProxy::GetSource(std::shared_ptr<ProvisionedResourceInterface> resource,
                                              std::shared_ptr<SourceInterface>& source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string resource_name;
        std::shared_ptr<ProvisionedResourceProxy> provisioned_resource_proxy;

        rc = resource->GetName(resource_name);
        rc = rc == fscfg_kRcSuccess ? Get(resource_name, provisioned_resource_proxy) : rc;

        if (rc == fscfg_kRcSuccess && provisioned_resource_proxy)
        {
            _ConfigProvisionedResource* config_provisioned_resource = nullptr;
            provisioned_resource_proxy->GetObject(config_provisioned_resource);

            if (!config_provisioned_resource)
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                               "Failed to register config provisioned resource object.");
                rc = fscfg_kRcBadState;
            }
            else
            {
                const char* source_path;
                std::string source_name;

                source_path = config_provisioned_resource_get_source(config_provisioned_resource);

                if (!source_path)
                {
                    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                                   "Failed to get provisioned resource's source from Dbus.");
                }
                else
                {
                    GetBasename(source_path, source_name);
                    rc = rc == fscfg_kRcSuccess ? config_proxy_->Get(source_name, source) : rc;
                }
            }
        }
    }

    return rc;
}

fscfg_ReturnCode ProvisioningProxy::GetEncoding(std::shared_ptr<ProvisionedResourceInterface> resource,
                                                std::shared_ptr<EncodingInterface>& encoding)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string resource_name;
        std::shared_ptr<ProvisionedResourceProxy> provisioned_resource_proxy;

        rc = resource->GetName(resource_name);
        rc = rc == fscfg_kRcSuccess ? Get(resource_name, provisioned_resource_proxy) : rc;

        if (rc == fscfg_kRcSuccess && provisioned_resource_proxy)
        {
            _ConfigProvisionedResource* config_provisioned_resource = nullptr;
            provisioned_resource_proxy->GetObject(config_provisioned_resource);

            if (!config_provisioned_resource)
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                               "Failed to register config provisioned resource object.");
                rc = fscfg_kRcBadState;
            }
            else
            {
                const char* encoding_path;
                std::string encoding_name;

                encoding_path = config_provisioned_resource_get_encoding(config_provisioned_resource);

                if (!encoding_path)
                {
                    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                                   "Failed to get provisioned resource's encoding from Dbus.");
                }
                else
                {
                    GetBasename(encoding_path, encoding_name);
                    rc = rc == fscfg_kRcSuccess ? config_proxy_->Get(encoding_name, encoding) : rc;
                }
            }
        }
    }

    return rc;
}

fscfg_ReturnCode ProvisioningProxy::GetAll(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources)
{
    return GetAllFromManager(objmgr_provisioned_resource_, sync_, resources);
}

fscfg_ReturnCode ProvisioningProxy::Get(const std::string& name,
                                        std::shared_ptr<ProvisionedResourceInterface>& resource)
{
    return GetFromManager(objmgr_provisioned_resource_, name, sync_, resource);
}

fscfg_ReturnCode ProvisioningProxy::Get(const std::string& name,
                                        std::shared_ptr<ProvisionedResourceProxy>& resource)
{
    return GetFromManager(objmgr_provisioned_resource_, name, sync_, resource);
}

std::shared_ptr<ConfigProxy> ProvisioningProxy::GetConfig() const
{
    return config_proxy_;
}

void ProvisioningProxy::OnPropertiesChanged(GDBusProxy* proxy,
                                            GVariant* changed,
                                            GStrv invalidated,
                                            gpointer provisioning_instance)
{
    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Received ProvisioningProxy OnPropertiesChanged callback");

    if (!provisioning_instance)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to execute ProvisioningProxy OnPropertiesChanged, invalid input.");
    }
    else
    {
        ProvisioningProxy* provisioning_proxy = reinterpret_cast<ProvisioningProxy*>(provisioning_instance);

        if (provisioning_proxy)
        {
            if (g_variant_n_children(changed) > 0)
            {
                // Extract property names.
                GVariantIter* iter = nullptr;
                const gchar* key = nullptr;
                GVariant* value = nullptr;

                g_variant_get(changed, "a{sv}", &iter);

                // Check which property has changed, update resources and call the bindings.
                while (g_variant_iter_loop(iter, "{&sv}", &key, &value))
                {
                    std::string key_str(key);

                    if (key_str == "Resources")
                    {
                        provisioning_proxy->Update();
                    }
                    else
                    {
                        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_WARN,
                                        "ProvisioningProxy::OnPropertiesChanged invalid property received: %s",
                                        key_str.c_str());
                    }
                }

                g_variant_iter_free(iter);
            }
        }
    }
}

fscfg_ReturnCode ProvisioningProxy::Update()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!objmgr_provisioned_resource_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Updating ProvisioningProxy resource");

        std::vector<std::string> new_names;
        std::vector<std::string> deleted_names;

        rc = objmgr_provisioned_resource_->UpdateContainer(new_names, deleted_names);

        if (rc  == fscfg_kRcSuccess && (new_names.size() != 0 || deleted_names.size() != 0))
        {
            std::vector<std::shared_ptr<ProvisionedResourceProxy>> resources;

            fsm::MapToValuesVector(objmgr_provisioned_resource_->set, resources);

            std::vector<std::shared_ptr<ProvisionedResourceInterface>> resources_bases(resources.begin(),
                                                                                       resources.end());

            rc = OnResourcesChanged(resources_bases);
        }
    }

    return rc;
}

} // namespace fsm

/** \}    end of addtogroup */
