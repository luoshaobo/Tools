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
 *  \file     discovery_proxy.cpp
 *  \brief    Foundation Services Discovery proxy implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <libfscfg/discovery_proxy.h>

#include <dlt/dlt.h>

#include <fscfg_common/utilities.h>
#include <fscfg/bus_names.h>

#include <fscfg/provisioning_interface.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

DiscoveryProxy::DiscoveryProxy(std::shared_ptr<ProvisioningProxy> provisioning_proxy)
 : sync_(),
   objmgr_feature_(),
   provisioning_proxy_(provisioning_proxy),
   discovery_dbus_obj_(nullptr)
{
    const char* bus_name = nullptr;
    const char* container_path = nullptr;

    fscfg_ReturnCode rc = fscfg_GetBusName(&bus_name);
    rc = rc ==fscfg_kRcSuccess ? fscfg_GetObjectPath(&container_path, fscfg_kBotDiscovery) : rc;

    if (rc == fscfg_kRcSuccess)
    {
        GError *error = nullptr;

        discovery_dbus_obj_ = discovery_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                               G_DBUS_PROXY_FLAGS_NONE,
                                                               bus_name,
                                                               container_path,
                                                               NULL,
                                                               &error);

        if (!discovery_dbus_obj_)
        {
            if (error)
            {
                DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                "Failed to create discovery proxy dbus object, error: %s",
                                error->message);
                g_clear_error (&error);
            }
            else
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to create discovery proxy dbus object");
            }
        }
    }
}

DiscoveryProxy::~DiscoveryProxy()
{
}

fscfg_ReturnCode DiscoveryProxy::Init()
{
    const char* bus_name = nullptr;
    const char* feature_path = nullptr;

    fscfg_ReturnCode rc = fscfg_GetBusName(&bus_name);

    rc = fscfg_GetObjectPath(&feature_path, fscfg_kBotDiscoveryFeature);

    if (rc == fscfg_kRcSuccess)
    {
        objmgr_feature_ = std::make_shared<ObjMgrFeature>(shared_from_this(),
                                                          sync_,
                                                          provisioning_proxy_,
                                                          object_get_config_feature,
                                                          bus_name,
                                                          feature_path);

        if (!objmgr_feature_)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                           "Failed to create object manager feature object.");
            rc = fscfg_kRcError;
        }

        // Update the discovery proxy resource
        rc = rc == fscfg_kRcSuccess ? Update() : rc;
        if (rc == fscfg_kRcSuccess)
        {

            if (!g_signal_connect(discovery_dbus_obj_,
                                 "g-properties-changed",
                                 G_CALLBACK(DiscoveryProxy::OnPropertiesChanged),
                                 this))
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                               "Failed to register DiscoveryProxy OnPropertiesChanged callback on Dbus.");
                rc = fscfg_kRcError;
            }
        }
    }

    return rc;
}

fscfg_ReturnCode DiscoveryProxy::GetAll(std::vector<std::shared_ptr<FeatureInterface>>& resources)
{
    return GetAllFromManager(objmgr_feature_, sync_, resources);
}

fscfg_ReturnCode DiscoveryProxy::Get(const std::string& name, std::shared_ptr<FeatureInterface>& resource)
{
    return GetFromManager(objmgr_feature_, name, sync_, resource);
}

fscfg_ReturnCode DiscoveryProxy::Get(const std::string& name, std::shared_ptr<FeatureProxy>& resource)
{
    return GetFromManager(objmgr_feature_, name, sync_, resource);
}

fscfg_ReturnCode DiscoveryProxy::GetCloudConnectionStatus(char& certificate_validity,
                                                          char& central_connect_status,
                                                          std::string& central_server_uri,
                                                          std::string& central_date_time,
                                                          char& regional_connect_status,
                                                          std::string& regional_server_uri,
                                                          std::string& regional_date_time)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!discovery_dbus_obj_)
    {
        DLT_LOG_STRING(dlt_libfscfg,
                       DLT_LOG_ERROR,
                       "DiscoveryProxy::GetCloudConnectionStatus: Bad dbus object");

        rc = fscfg_kRcBadState;
    }
    else
    {
        GVariant* connection_details = discovery_get_cloud_connection_status(discovery_dbus_obj_);

        if (!connection_details)
        {
            DLT_LOG_STRING(dlt_libfscfg,
                           DLT_LOG_ERROR,
                           "DiscoveryProxy::GetCloudConnectionStatus: failed to retrieve connection details from bus");
        }
        else
        {
            char* bus_central_server_uri;
            char* bus_central_date_time;
            char* bus_regional_server_uri;
            char* bus_regional_date_time;

            g_variant_get(connection_details,
                          "(yyssyss)",
                          &certificate_validity,
                          &central_connect_status,
                          &bus_central_server_uri,
                          &bus_central_date_time,
                          &regional_connect_status,
                          &bus_regional_server_uri,
                          &bus_regional_date_time);

            central_server_uri.append(bus_central_server_uri);
            central_date_time.append(bus_central_date_time);
            regional_server_uri.append(bus_regional_server_uri);
            regional_date_time.append(bus_regional_date_time);
        }
    }

    return rc;
}


fscfg_ReturnCode DiscoveryProxy::OnResourcesChanged(std::vector<std::shared_ptr<FeatureInterface>>& new_resources)
{
    return DiscoveryBind::OnResourcesChanged(new_resources);
}

void DiscoveryProxy::OnPropertiesChanged(GDBusProxy* proxy,
                                         GVariant* changed,
                                         GStrv invalidated,
                                         gpointer discovery_instance)
{
    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Received DiscoveryProxy OnPropertiesChanged callback");

    if (!discovery_instance)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                       "Failed to execute DiscoveryProxy OnPropertiesChanged, invalid input.");
    }
    else
    {
        DiscoveryProxy* discovery_proxy = reinterpret_cast<DiscoveryProxy*>(discovery_instance);

        if (discovery_proxy)
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
                        discovery_proxy->Update();
                    }
                    else
                    {
                        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_WARN,
                                        "DiscoveryProxy::OnPropertiesChanged unhandled property received: %s",
                                        key_str.c_str());
                    }
                }

                g_variant_iter_free(iter);
            }
        }
    }
}

fscfg_ReturnCode DiscoveryProxy::Update()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!objmgr_feature_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Updating DiscoveryProxy resources");

        std::vector<std::string> new_names;
        std::vector<std::string> deleted_names;

        rc = objmgr_feature_->UpdateContainer(new_names, deleted_names);

        if (rc  == fscfg_kRcSuccess && (new_names.size() != 0 || deleted_names.size() != 0))
        {
            std::vector<std::shared_ptr<FeatureProxy>> features;

            fsm::MapToValuesVector(objmgr_feature_->set, features);

            std::vector<std::shared_ptr<FeatureInterface>> features_bases(features.begin(),
                                                                          features.end());

            rc = OnResourcesChanged(features_bases);
        }
    }

    return rc;
}

} // namespace fsm

/** \}    end of addtogroup */
