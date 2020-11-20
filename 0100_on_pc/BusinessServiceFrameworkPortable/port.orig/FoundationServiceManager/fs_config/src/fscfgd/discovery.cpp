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
 *  \file     discovery.cpp
 *  \brief    Foundation Services Discovery implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/discovery.h>

#include <glib.h>
#include <gio/gio.h>

#include <dlt/dlt.h>

#include <fscfg/bus_names.h>
#include <fscfg_common/utilities.h>

#include <fscfgd/feature.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd);

namespace fsm
{

Discovery::Discovery(std::shared_ptr<Provisioning> provisioning)
 : provisioning_(provisioning),
   discovery_dbus_obj_(nullptr),
   config_bus_bind_id_(0)
{
    fscfg_ReturnCode rc;
    const char* resources_path;

    discovery_dbus_obj_ = discovery_skeleton_new();

    // Prepare the paths.
    rc = fscfg_GetObjectPath(&resources_path, fscfg_kBotDiscoveryFeature);

    if (discovery_dbus_obj_ && rc == fscfg_kRcSuccess)
    {
        // Create the object managers.
        objmgr_feature_ = std::make_shared<ObjMgrFeature>(discovery_dbus_obj_,
                                                          discovery_set_resources,
                                                          resources_path);

        std::shared_ptr<Config> config = provisioning->GetConfig();

        // bind for incomming bus acquired event in order to publish skeleton.
        rc = config->BindBusAcquired(std::bind(&Discovery::EventBusAcquired, this, std::placeholders::_1),
                                      config_bus_bind_id_);

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Discovery::Failed to initialize");
        }
    }
}

Discovery::~Discovery()
{
    if (discovery_dbus_obj_)
    {
        g_object_unref(discovery_dbus_obj_);
    }
}

fscfg_ReturnCode Discovery::GetAll(std::vector<std::shared_ptr<FeatureInterface>>& resources)
{
    return GetAllFromManager(objmgr_feature_, resources);
}

fscfg_ReturnCode Discovery::Get(const std::string& name, std::shared_ptr<FeatureInterface>& resource)
{
    return GetFromManager(objmgr_feature_, name, resource);
}

fscfg_ReturnCode Discovery::GetAll(std::vector<std::shared_ptr<Feature>>& resources)
{
    return GetAllFromManager(objmgr_feature_, resources);
}

fscfg_ReturnCode Discovery::Get(const std::string& name, std::shared_ptr<Feature>& resource)
{
    return GetFromManager(objmgr_feature_, name, resource);
}

fscfg_ReturnCode Discovery::GetCloudConnectionStatus(char& certificate_validity,
                                                     char& central_connect_status,
                                                     std::string& central_server_uri,
                                                     std::string& central_date_time,
                                                     char& regional_connect_status,
                                                     std::string& regional_server_uri,
                                                     std::string& regional_date_time)
{
    // We do not need a daemon-local getter. Just report an error.

    return fscfg_kRcError;
}

fscfg_ReturnCode Discovery::Register(std::shared_ptr<Feature> resource)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    rc = provisioning_->Register(resource);

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_WARN, "Discovery::Register: Failed to register in provisioning");
    }
    else
    {
        rc = RegisterInManager(objmgr_feature_, resource);
    }

    return rc;
}

fscfg_ReturnCode Discovery::Update(std::shared_ptr<Feature> resource, bool create)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    // We attempt to update existing feature. If it does not exist,
    // we'll just register the given feature.
    std::string feature_name;
    std::shared_ptr<Feature> existing_feature;

    if (!resource)
    {
        rc = fscfg_kRcBadParam;
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_WARN, "Discovery::Update: Bad feature pointer");
    }
    else
    {
        resource->GetName(feature_name);

        rc = Get(feature_name, existing_feature);

        if (rc == fscfg_kRcBadResource)
        {
            // a feature with feature_name does not exist.
            if (!create)
            {
                // Ok, so the intention is NOT to create it, but to update it.
                // As such, it should've existed prior.

                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_WARN,
                                "Discovery::Update: Feature %s does not exist",
                                feature_name.c_str());
            }
            else
            {
                // Feature is allowed to be created if it does not exist.
                rc = Register(resource);
            }
        }
        else
        {
            // As feature exists, we will just update it with the properties of the given feature.
            Feature::State state = Feature::State::kUndefined;
            std::string description;
            std::string uri;
            std::string icon;
            std::vector<std::string> tags;

            // Get the props from the given feature.
            rc = rc == fscfg_kRcSuccess ? resource->GetState(state) : rc;
            rc = rc == fscfg_kRcSuccess ? resource->GetDescription(description) : rc;
            rc = rc == fscfg_kRcSuccess ? resource->GetUri(uri) : rc;
            rc = rc == fscfg_kRcSuccess ? resource->GetIcon(icon) : rc;
            rc = rc == fscfg_kRcSuccess ? resource->GetTags(tags) : rc;

            // Update props of existing feature.
            rc = rc == fscfg_kRcSuccess ? existing_feature->SetState(state) : rc;
            rc = rc == fscfg_kRcSuccess ? existing_feature->SetDescription(description) : rc;
            rc = rc == fscfg_kRcSuccess ? existing_feature->SetUri(uri) : rc;
            rc = rc == fscfg_kRcSuccess ? existing_feature->SetIcon(icon) : rc;
            rc = rc == fscfg_kRcSuccess ? existing_feature->SetTags(tags) : rc;
        }
    }

    return rc;
}

fscfg_ReturnCode Discovery::RemoveFeature(const std::string& feature_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    rc = provisioning_->RemoveResource(feature_name);

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_ERROR,
                        "Discovery::RemoveFeature: Failed to remove feature:%s from provisioning",
                        feature_name.c_str());
    }
    else
    {
        ObjMgrFeature::Map& front = objmgr_feature_->set.Get();
        ObjMgrFeature::Map::const_iterator it = front.find(feature_name);

        if (it == front.end())
        {
            rc = fscfg_kRcBadResource;
        }
        else
        {
            front.erase(it);
        }
    }

    return rc;
}

fscfg_ReturnCode Discovery::Update()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    rc = provisioning_->Update();

    if (rc == fscfg_kRcSuccess)
    {
        std::vector<std::string> new_names;
        std::vector<std::string> deleted_names;
        std::vector<std::shared_ptr<Feature>> features;

        MapToValuesVector(objmgr_feature_->set.Get(), features);

        BufferedDataSetDiff(objmgr_feature_->set, new_names, deleted_names);
        std::pair<const std::vector<std::string>&, const std::vector<std::string>&> names(new_names, deleted_names);

        if (new_names.size() || deleted_names.size())
        {
            objmgr_feature_->UpdateContainer(names);

            std::vector<std::shared_ptr<FeatureInterface>>
                    resources_bases(features.begin(), features.end());

            OnResourcesChanged(resources_bases);
        }

        // For each feature, call update.
        for (auto feature : features)
        {
            fscfg_ReturnCode update_feature_rc = feature->Update();

            if (update_feature_rc != fscfg_kRcSuccess)
            {
                std::string feature_name;

                feature->GetName(feature_name);

                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_ERROR,
                                "Discovery::Update: Feature %s failed to update",
                                feature_name.c_str());
            }
        }

        // Flush properties update.
        g_dbus_interface_skeleton_flush(G_DBUS_INTERFACE_SKELETON(discovery_dbus_obj_));
    }

    return rc;
}

std::shared_ptr<Provisioning> Discovery::GetProvisioning()
{
    return provisioning_;
}

fscfg_ReturnCode Discovery::OnResourcesChanged(std::vector<std::shared_ptr<FeatureInterface>>& resources)
{
    std::vector<std::string> names;
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    for (auto& resource : resources)
    {
        std::string name;

        rc = resource->GetName(name);
        names.push_back(name);
    }

    objmgr_feature_->UpdateNames(names);

    // log all encodings.
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Discovery: Start exporting Features...");

    for (auto& resource : resources)
    {
        std::string feature_name;

        resource->GetName(feature_name);
        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Discovery: Feature->%s", feature_name.c_str());
    }

    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Discovery: Done exporting Features.");

    rc = DiscoveryBind::OnResourcesChanged(resources);

    return rc;
}

fscfg_ReturnCode Discovery::EventBusAcquired(Config::BusAcquiredEvent event)
{
    fscfg_ReturnCode rc;

    const char* container_path;

    // Prepare the paths.
    rc = fscfg_GetObjectPath(&container_path, fscfg_kBotDiscovery);

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Discovery: Failed to retrieve container name");
    }
    else
    {
        // Export Discovery interface on the bus.
        gboolean iface_exported = g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(discovery_dbus_obj_),
                                                                   event.connection,
                                                                   container_path,
                                                                   NULL);

        if (!iface_exported)
        {
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Discovery: Failed to export interface");
            rc = fscfg_kRcError;
        }
        else
        {
            // Set default values for the cloud connection status.
            SetCloudConnectionStatus();
            objmgr_feature_->SetConnection(event.connection);
        }
    }

    return rc;
}

void Discovery::SetCloudConnectionStatus()
{

    // Some initial default values.
    guint8 certificate_validity_status = 0;

    guint8 central_connection_status = 0;
    std::string central_server_uri  = "http://192.168.1.113:8088/CN";
    std::string central_date_time = "2017-05-31 13:50:00";


    guint8 regional_connection_status = 0;
    std::string regional_server_uri = "http://192.168.1.113:8088/RN";
    std::string regional_date_time = "2017-05-31 13:50:00";

    GVariant* connection_status = g_variant_new("(yyssyss)",
                                                certificate_validity_status,
                                                central_connection_status,
                                                central_server_uri.c_str(),
                                                central_date_time.c_str(),
                                                regional_connection_status,
                                                regional_server_uri.c_str(),
                                                regional_date_time.c_str());

    discovery_set_cloud_connection_status(discovery_dbus_obj_, connection_status);
}


} // namespace fsm

/** \}    end of addtogroup */
