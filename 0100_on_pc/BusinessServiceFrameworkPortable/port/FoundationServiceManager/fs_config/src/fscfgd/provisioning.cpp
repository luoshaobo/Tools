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
 *  \file     provisioning.cpp
 *  \brief    Foundation Services Provisioning implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/provisioning.h>

#include <glib.h>
#include <gio/gio.h>

#include <dlt/dlt.h>

#include <fscfg/bus_names.h>
#include <fscfg_common/utilities.h>

#include <fscfgd/provisionedresource.h>

#include <fscfgd/config.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd);

namespace fsm
{

Provisioning::Provisioning(std::shared_ptr<Config> config)
 : config_(config),
   config_bus_bind_id(0),
   provisioning_dbus_obj_(nullptr)
{
    fscfg_ReturnCode rc;
    const char* resources_path;

    provisioning_dbus_obj_ = provisioning_skeleton_new();

    if (!provisioning_dbus_obj_)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Provisioning: Failed to create skeleton");
    }
    else
    {
        // Prepare the paths.
        rc = fscfg_GetObjectPath(&resources_path, fscfg_kBotProvisioningResource);

        if (!(rc == fscfg_kRcSuccess && provisioning_dbus_obj_ && config_))
        {
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Provisioning: Failed to initialize");
        }
        else
        {
            // Create the object managers.
            objmgr_resource_ = std::make_shared<ObjMgrResource>(provisioning_dbus_obj_,
                                                                provisioning_set_resources,
                                                                resources_path);

            // bind for incomming bus acquired event in order to publish skeleton.
            rc = config_->BindBusAcquired(std::bind(&Provisioning::EventBusAcquired, this, std::placeholders::_1),
                                          config_bus_bind_id);

            if (rc != fscfg_kRcSuccess)
            {
                DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Provisioning: Failed to bind on bus acquired");
            }
            else
            {
                DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Provisioning: Bound on bus acquired");
            }
        }
    }
}

Provisioning::~Provisioning()
{
    config_->Unbind(config_bus_bind_id);

    if (provisioning_dbus_obj_)
    {
        g_object_unref(provisioning_dbus_obj_);
    }
}

fscfg_ReturnCode Provisioning::GetAll(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources)
{
    return GetAllFromManager(objmgr_resource_, resources);
}

fscfg_ReturnCode Provisioning::Get(const std::string& name, std::shared_ptr<ProvisionedResourceInterface>& resource)
{
    return GetFromManager(objmgr_resource_, name, resource);
}

fscfg_ReturnCode Provisioning::GetAll(std::vector<std::shared_ptr<ProvisionedResource>>& resources)
{
    return GetAllFromManager(objmgr_resource_, resources);
}

fscfg_ReturnCode Provisioning::Get(const std::string& name, std::shared_ptr<ProvisionedResource>& resource)
{
    return GetFromManager(objmgr_resource_, name, resource);
}

fscfg_ReturnCode Provisioning::Register(std::shared_ptr<ProvisionedResource> resource)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    rc = config_->Register(resource);

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_WARN, "Provisioning::Register: Failed to register in provisioning");
    }
    else
    {
        rc = RegisterInManager(objmgr_resource_, resource);
    }

    return rc;
}

fscfg_ReturnCode Provisioning::RemoveResource(const std::string& resource_name)
{
    return RemoveFromManager(objmgr_resource_, resource_name);
}

fscfg_ReturnCode Provisioning::Update()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    rc = config_->Update();

    if (rc == fscfg_kRcSuccess)
    {
        // update all provisioned resources.
        {
            std::vector<std::string> new_names;
            std::vector<std::string> deleted_names;
            std::vector<std::shared_ptr<ProvisionedResource>> resources;

            MapToValuesVector(objmgr_resource_->set.Get(), resources);

            BufferedDataSetDiff(objmgr_resource_->set, new_names, deleted_names);
            std::pair<const std::vector<std::string>&, const std::vector<std::string>&> names(new_names, deleted_names);

            if (new_names.size() || deleted_names.size())
            {
                objmgr_resource_->UpdateContainer(names);

                std::vector<std::shared_ptr<ProvisionedResourceInterface>>
                        resources_bases(resources.begin(), resources.end());

                OnResourcesChanged(resources_bases);
            }
        }

        // Flush properties update.
        g_dbus_interface_skeleton_flush(G_DBUS_INTERFACE_SKELETON(provisioning_dbus_obj_));
    }

    return rc;
}

fscfg_ReturnCode Provisioning::SetPriority(std::shared_ptr<Source> source, Priority prio)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!source)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        // We can only prioritize sources that are already registered in config.
        std::string source_name;
        std::shared_ptr<Source> config_source;

        rc = source->GetName(source_name);
        rc = rc == fscfg_kRcSuccess ? config_->Get(source_name, config_source) : rc;

        // at this point, we guarantee that source is registered. (if rc is success).
        // We need to make sure that we can set prio if not taken by some other source.
        if (rc == fscfg_kRcSuccess)
        {
            SourcePrioMap::iterator it = source_prio_.find(prio);
            SourcePrioMap::iterator source_found_it = source_prio_.end();

            if (it != source_prio_.end())
            {
                // Priority is already taken by some other source.
                rc = fscfg_kRcKeyExists;
            }
            else
            {
                // Priority slot is free. The source itself could still exist.
                // If source exists in priority map, we erase it and add it at the free and desired priority slot.
                // Linear search by value.
                SourcePrioMap::iterator current_it = source_prio_.begin();

                for (; current_it != source_prio_.end(); ++current_it)
                {
                    std::string current_source_name;

                    rc = current_it->second->GetName(current_source_name);

                    if (rc != fscfg_kRcSuccess)
                    {
                        break;
                    }
                    else
                    {
                        if (source_name == current_source_name)
                        {
                            // if source names are equal, source already exists (we need to re-prioritize).
                            source_found_it = current_it;
                            break;
                        }
                    }
                }
            }

            if (source_found_it != source_prio_.end())
            {
                // source exists, remove it.
                source_prio_.erase(source_found_it);
            }

            source_prio_[prio] = source;

            // Logging.
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Provisioning: Source prio...");

            for (auto& pair_prio_src : source_prio_)
            {
                std::string current_source_name;
                pair_prio_src.second->GetName(current_source_name);

                 DLT_LOG_STRINGF(dlt_fscfgd,
                                 DLT_LOG_INFO,
                                 "Provisioning: SourcePrio->%s",
                                 current_source_name.c_str());
            }

            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Provisioning: Source prio done");
        }
    }

    return rc;
}

fscfg_ReturnCode Provisioning::SetPriority(std::shared_ptr<Encoding> encoding, Priority prio)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!encoding)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        // We can only prioritize encodings that are already registered in config.
        std::string encoding_name;
        std::shared_ptr<Encoding> config_encoding;

        rc = encoding->GetName(encoding_name);
        rc = rc == fscfg_kRcSuccess ? config_->Get(encoding_name, config_encoding) : rc;

        // at this point, we guarantee that source is registered. (if rc is success).
        // We need to make sure that we can set prio if not taken by some other source.
        if (rc == fscfg_kRcSuccess)
        {
            EncodingPrioMap::iterator it = encoding_prio_.find(prio);
            EncodingPrioMap::iterator encoding_found_it = encoding_prio_.end();

            if (it != encoding_prio_.end())
            {
                // Priority is already taken by some other source.
                rc = fscfg_kRcKeyExists;
            }
            else
            {
                // Priority slot is free. The source itself could still exist.
                // If source exists in priority map, we erase it and add it at the free and desired priority slot.
                // Linear search by value.
                EncodingPrioMap::iterator current_it = encoding_prio_.begin();

                for (; current_it != encoding_prio_.end(); ++current_it)
                {
                    std::string current_encoding_name;

                    rc = current_it->second->GetName(current_encoding_name);

                    if (rc != fscfg_kRcSuccess)
                    {
                        break;
                    }
                    else
                    {
                        if (encoding_name == current_encoding_name)
                        {
                            // if source names are equal, source already exists (we need to re-prioritize).
                            encoding_found_it = current_it;
                            break;
                        }
                    }
                }
            }

            if (encoding_found_it != encoding_prio_.end())
            {
                // source exists, remove it.
                encoding_prio_.erase(encoding_found_it);
            }

            encoding_prio_[prio] = encoding;

            // Logging.
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Provisioning: Encoding prio...");

            for (auto& pair_prio_enc : encoding_prio_)
            {
                std::string current_encoding_name;
                pair_prio_enc.second->GetName(current_encoding_name);

                 DLT_LOG_STRINGF(dlt_fscfgd,
                                 DLT_LOG_INFO,
                                 "Provisioning: EncodingPrio->%s",
                                 current_encoding_name.c_str());
            }

            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Provisioning: Encoding prio done");
        }
    }

    return rc;
}

fscfg_ReturnCode Provisioning::GetEncoding(std::shared_ptr<ProvisionedResourceInterface> resource,
                                           std::shared_ptr<EncodingInterface>& encoding)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource)
    {
        rc = fscfg_kRcBadParam;
    }
    else if (!config_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        // find top prio source.
        std::shared_ptr<SourceInterface> source;
        std::vector<std::shared_ptr<EncodingInterface>> encodings;
        std::shared_ptr<ResourceInterface> base_resource;
        std::string base_resource_name;

        resource->GetName(base_resource_name);

        rc = GetSource(resource, source);
        rc = rc == fscfg_kRcSuccess ? config_->Get(base_resource_name, base_resource) : rc;
        rc = rc == fscfg_kRcSuccess ? source->GetEncodings(base_resource, encodings) : rc;

        if (rc == fscfg_kRcSuccess)
        {
            EncodingPrioMap::iterator it = encoding_prio_.begin();

            // Interate through the encoding priority map and find the highest prio encoding
            // actually present in the source.
            bool encoding_found = false;

            for (; it != encoding_prio_.end() && !encoding_found && rc == fscfg_kRcSuccess; ++it)
            {
                std::string name_to_find;
                std::vector<std::shared_ptr<EncodingInterface>>::iterator it_encs = encodings.begin();
                rc = it->second->GetName(name_to_find);

                for (; it_encs != encodings.end() && !encoding_found && rc == fscfg_kRcSuccess; ++it_encs)
                {
                    std::string current_name;
                    rc = (*it_encs)->GetName(current_name);

                    if (current_name == name_to_find)
                    {
                        encoding = (*it_encs);
                        encoding_found = true;
                    }
                }
            }

            if (!encoding_found)
            {
                // If there isn't any encoding, it's not prioritized. As such we will return the first encoding mentioned
                // by the source instance.
                if (encodings.size() == 0)
                {
                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_ERROR,
                                    "Provisioning::GetEncoding: no encodings for %s",
                                    base_resource_name.c_str());
                }
                else
                {
                    encoding = encodings[0];
                }
            }
        }
    }

    return rc;
}

fscfg_ReturnCode Provisioning::GetSource(std::shared_ptr<ProvisionedResourceInterface> resource,
                                         std::shared_ptr<SourceInterface>& source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource)
    {
        rc = fscfg_kRcBadParam;
    }
    else if (!config_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {

        std::string base_resource_name;
        std::shared_ptr<ResourceInterface> base_resource;
        resource->GetName(base_resource_name);

        rc = config_->Get(base_resource_name, base_resource);

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRINGF(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "Provisioning::GetSource: Failed to retrieve base resource for %s",
                           base_resource_name.c_str());
        }
        else
        {
            SourcePrioMap::iterator it = source_prio_.begin();

            // Iterate in the priority map and find the first source
            // that has the resource.

            bool resource_found = false;
            rc = fscfg_kRcNotFound;

            for (; it != source_prio_.end() && !resource_found; ++it)
            {
                it->second->HasResource(base_resource, resource_found);

                if (resource_found)
                {
                    source = it->second;
                    rc = fscfg_kRcSuccess;
                }
            }
        }
    }

    return rc;
}

std::shared_ptr<Config> Provisioning::GetConfig()
{
    return config_;
}

fscfg_ReturnCode Provisioning::OnResourcesChanged(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources)
{
    std::vector<std::string> names;
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    for (auto& resource : resources)
    {
        std::string name;

        rc = resource->GetName(name);
        names.push_back(name);
    }

    objmgr_resource_->UpdateNames(names);

    // log all encodings.
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Provisioning: Start exporting resources...");

    for (auto& resource : resources)
    {
        std::string resource_name;

        resource->GetName(resource_name);
        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Provisioning: Resource->%s", resource_name.c_str());
    }

    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Provisioning: Done exporting resources.");

    // call bindings.
    rc = ProvisioningBind::OnResourcesChanged(resources);

    return rc;
}

fscfg_ReturnCode Provisioning::EventBusAcquired(Config::BusAcquiredEvent event)
{
    fscfg_ReturnCode rc;

    const char* container_path;

    // Prepare the paths.
    rc = fscfg_GetObjectPath(&container_path, fscfg_kBotProvisioning);

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Provisioning: Failed to retrieve container name");
    }
    else
    {
        // Export provisioning interface on the bus.
        gboolean iface_exported = g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(provisioning_dbus_obj_),
                                                                   event.connection,
                                                                   container_path,
                                                                   NULL);

        if (!iface_exported)
        {
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Provisioning: Failed to export interface");
        }
        else
        {
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO , "Provisioning: interface exported");

            objmgr_resource_->SetConnection(event.connection);
        }
    }

    return rc;
}


} // namespace fsm

/** \}    end of addtogroup */
