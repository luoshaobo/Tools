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
 *  \file     config.cpp
 *  \brief    Foundation Services Config implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/config.h>

#include <glib.h>
#include <gio/gio.h>

#include <dlt/dlt.h>

#include <fscfg/bus_names.h>
#include <fscfg_common/utilities.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd);

namespace fsm
{

Config::Config()
 : objmgr_encoding_(),
   objmgr_source_(),
   objmgr_resource_(),
   bind_ex_(bind_base_.id_generator_),
   config_container_path_(),
   own_id_(0),
   local_state_(State::kUndefined),
   config_dbus_obj_(nullptr),
   connection_(nullptr)
{
    config_dbus_obj_ = config_skeleton_new();

    if (!config_dbus_obj_)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Config: Failed to create skeleton");
        local_state_ = State::kError;
    }
    else
    {
        // Retrieve the bus paths from the common library, initialize the object managers
        // and start owning the bus name. Once the main-loop gets activated, the bus-acquired callback should come.
        // At that point, pass the notification to higher-level containers (via binds) to initialize them.
        fscfg_ReturnCode rc;

        const char* container_path;
        const char* encodings_path;
        const char* sources_path;
        const char* resources_path;

        // Prepare the paths.
        rc = fscfg_GetObjectPath(&container_path, fscfg_kBotConfig);
        rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&encodings_path, fscfg_kBotConfigEncoding) : rc;
        rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&sources_path, fscfg_kBotConfigSource) : rc;
        rc = rc == fscfg_kRcSuccess ? fscfg_GetObjectPath(&resources_path, fscfg_kBotConfigResource) : rc;

        if (rc != fscfg_kRcSuccess)
        {
            local_state_ = State::kError; // Pretty bad. We could not retrieve the bus names.
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Config: Failed to retrieve bus and container names");
        }
        else
        {
            config_container_path_.assign(container_path);

            // Create the object managers.
            // Encoding, Source and Resource Object Managers.
            objmgr_encoding_ = std::make_shared<ObjMgrEncoding>(config_dbus_obj_,
                                                                config_set_encodings,
                                                                encodings_path);

            objmgr_source_ = std::make_shared<ObjMgrSource>(config_dbus_obj_,
                                                            config_set_sources,
                                                            sources_path);

            objmgr_resource_ = std::make_shared<ObjMgrResource>(config_dbus_obj_,
                                                                config_set_resources,
                                                                resources_path);

            if (!(objmgr_encoding_
                 && objmgr_source_
                 && objmgr_resource_))
            {
                DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Config: Failed to initialize object managers");
                local_state_ = State::kError;
            }
            else
            {
                local_state_ = State::kInitialized;
            }
        }
    }
}

Config::~Config()
{
    if (config_dbus_obj_)
    {
        g_object_unref(config_dbus_obj_);
    }

    if (own_id_)
    {
        g_bus_unown_name(own_id_);
    }
}

fscfg_ReturnCode Config::Unbind(std::uint32_t id)
{
    fscfg_ReturnCode rc = ConfigBind::Unbind(id);

    rc = rc != fscfg_kRcSuccess ? bind_ex_.Unbind(id) : rc;

    return rc;
}

fscfg_ReturnCode Config::OnEncodingsChanged(std::vector<std::shared_ptr<EncodingInterface>> encodings)
{
    std::vector<std::string> names;
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!objmgr_encoding_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        for (auto& encoding : encodings)
        {
            std::string name;

            rc = encoding->GetName(name);
            names.push_back(name);
        }

        objmgr_encoding_->UpdateNames(names);

        // log all encodings.
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Config: Start exporting encodings...");

        for (auto& encoding : encodings)
        {
            std::string encoding_name;

            encoding->GetName(encoding_name);
            DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Config: Encoding->%s", encoding_name.c_str());
        }

        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Config: Done exporting encodings.");

        // call bindings.
        rc = ConfigBind::OnEncodingsChanged(encodings);
    }

    return rc;
}

fscfg_ReturnCode Config::OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources)
{
    std::vector<std::string> names;
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!objmgr_source_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        for (auto& source : sources)
        {
            std::string name;

            rc = source->GetName(name);
            names.push_back(name);
        }

        objmgr_source_->UpdateNames(names);

        // log all sources.
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Config: Start exporting sources...");

        for (auto& source : sources)
        {
            std::string source_name;

            source->GetName(source_name);
            DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Config: Source->%s", source_name.c_str());
        }

        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Config: Done exporting sources.");

        // call bindings.
        rc = ConfigBind::OnSourcesChanged(sources);
    }

    return rc;
}

fscfg_ReturnCode Config::OnResourcesChanged(std::vector<std::shared_ptr<ResourceInterface>> resources)
{
    std::vector<std::string> names;
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!objmgr_source_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        for (auto& resource : resources)
        {
            std::string name;

            rc = resource->GetName(name);
            names.push_back(name);
        }

        objmgr_resource_->UpdateNames(names);

        // log all resources.
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Config: Start exporting resources...");

        for (auto& resource : resources)
        {
            std::string resource_name;

            resource->GetName(resource_name);
            DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Config: Resource->%s", resource_name.c_str());
        }

        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Config: Done exporting resources.");

        // call bindings.
        rc = ConfigBind::OnResourcesChanged(resources);
    }

    return rc;
}

fscfg_ReturnCode Config::OnBusAcquired(GDBusConnection* connection, const gchar* name, gpointer config_instance)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    BusAcquiredEvent bus_acquired;

    bus_acquired.connection = connection;
    bus_acquired.name = name;
    bus_acquired.parent_instance = config_instance;

    BindingSet<BusAcquiredEvent>& binds = std::get<kBusAcquired>(bind_ex_.tup_);

    CallBindings(binds, bus_acquired);

    return rc;
}


fscfg_ReturnCode Config::GetAll(std::vector<std::shared_ptr<EncodingInterface>>& encodings)
{
    return GetAllFromManager(objmgr_encoding_, encodings);
}

fscfg_ReturnCode Config::GetAll(std::vector<std::shared_ptr<SourceInterface>>& sources)
{
    return GetAllFromManager(objmgr_source_, sources);
}

fscfg_ReturnCode Config::GetAll(std::vector<std::shared_ptr<ResourceInterface>>& resources)
{
    return GetAllFromManager(objmgr_resource_, resources);
}

fscfg_ReturnCode Config::Get(const std::string& name, std::shared_ptr<EncodingInterface>& encoding)
{
    return GetFromManager(objmgr_encoding_, name, encoding);
}

fscfg_ReturnCode Config::Get(const std::string& name, std::shared_ptr<SourceInterface>& source)
{
    return GetFromManager(objmgr_source_, name, source);
}

fscfg_ReturnCode Config::Get(const std::string& name, std::shared_ptr<ResourceInterface>& resource)
{
    return GetFromManager(objmgr_resource_, name, resource);
}

fscfg_ReturnCode Config::GetAll(std::vector<std::shared_ptr<Encoding>>& encodings)
{
    return GetAllFromManager(objmgr_encoding_, encodings);
}

fscfg_ReturnCode Config::GetAll(std::vector<std::shared_ptr<Source>>& sources)
{
    return GetAllFromManager(objmgr_source_, sources);
}

fscfg_ReturnCode Config::GetAll(std::vector<std::shared_ptr<Resource>>& resources)
{
    return GetAllFromManager(objmgr_resource_, resources);
}

fscfg_ReturnCode Config::Get(const std::string& name, std::shared_ptr<Encoding>& encoding)
{
    return GetFromManager(objmgr_encoding_, name, encoding);
}

fscfg_ReturnCode Config::Get(const std::string& name, std::shared_ptr<Source>& source)
{
    return GetFromManager(objmgr_source_, name, source);
}

fscfg_ReturnCode Config::Get(const std::string& name, std::shared_ptr<Resource>& resource)
{
    return GetFromManager(objmgr_resource_, name, resource);
}

fscfg_ReturnCode Config::Register(std::shared_ptr<Encoding> encoding)
{
    return RegisterInManager(objmgr_encoding_, encoding);
}

fscfg_ReturnCode Config::Register(std::shared_ptr<Source> source)
{
    return RegisterInManager(objmgr_source_, source);
}

fscfg_ReturnCode Config::Register(std::shared_ptr<Resource> resource)
{
    return RegisterInManager(objmgr_resource_, resource);
}

fscfg_ReturnCode Config::RemoveEncoding(const std::string& encoding_name)
{
    return RemoveFromManager(objmgr_encoding_, encoding_name);
}

fscfg_ReturnCode Config::RemoveSource(const std::string& source_name)
{
    return RemoveFromManager(objmgr_source_, source_name);
}

fscfg_ReturnCode Config::RemoveResource(const std::string& resource_name)
{
    return RemoveFromManager(objmgr_source_, resource_name);
}

fscfg_ReturnCode Config::Update()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (local_state_ != State::kBusConnectionActive)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        // update all containers.
        // encodings
        {
            std::vector<std::string> new_names;
            std::vector<std::string> deleted_names;
            std::vector<std::shared_ptr<Encoding>> encodings;

            MapToValuesVector(objmgr_encoding_->set.Get(), encodings);

            BufferedDataSetDiff(objmgr_encoding_->set, new_names, deleted_names);
            std::pair<const std::vector<std::string>&, const std::vector<std::string>&> names(new_names, deleted_names);

            if (new_names.size() || deleted_names.size())
            {
                objmgr_encoding_->UpdateContainer(names);

                std::vector<std::shared_ptr<EncodingInterface>> encoding_bases(encodings.begin(), encodings.end());
                OnEncodingsChanged(encoding_bases);
            }
        }

        // sources
        {
            std::vector<std::string> new_names;
            std::vector<std::string> deleted_names;
            std::vector<std::shared_ptr<Source>> sources;

            MapToValuesVector(objmgr_source_->set.Get(), sources);

            BufferedDataSetDiff(objmgr_source_->set, new_names, deleted_names);
            std::pair<const std::vector<std::string>&, const std::vector<std::string>&> names(new_names, deleted_names);

            if (new_names.size() || deleted_names.size())
            {
                objmgr_source_->UpdateContainer(names);

                std::vector<std::shared_ptr<SourceInterface>> source_bases(sources.begin(), sources.end());
                OnSourcesChanged(source_bases);
            }
        }

        // resources
        {
            std::vector<std::string> new_names;
            std::vector<std::string> deleted_names;
            std::vector<std::shared_ptr<Resource>> sources;

            MapToValuesVector(objmgr_resource_->set.Get(), sources);

            BufferedDataSetDiff(objmgr_resource_->set, new_names, deleted_names);
            std::pair<const std::vector<std::string>&, const std::vector<std::string>&> names(new_names, deleted_names);

            if (new_names.size() || deleted_names.size())
            {
                objmgr_resource_->UpdateContainer(names);

                std::vector<std::shared_ptr<ResourceInterface>> resource_bases(sources.begin(), sources.end());
                OnResourcesChanged(resource_bases);
            }
        }

        // Flush properties update.
        g_dbus_interface_skeleton_flush(G_DBUS_INTERFACE_SKELETON(config_dbus_obj_));
    }

    return rc;
}

fscfg_ReturnCode Config::OwnBusName()
{
    const char* bus_name;

    fscfg_ReturnCode rc = fscfg_GetBusName(&bus_name);

    if (rc != fscfg_kRcSuccess || local_state_ != State::kInitialized)
    {
        // We have failed to initialize, we will not proceed with owning the bus when the state is inconsistent.
        rc = fscfg_kRcBadState;

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_ERROR,
                        "Config:OwnBusName: Failed to own bus name due to bad state",
                        static_cast<unsigned>(local_state_));
    }
    else
    {
        local_state_ = State::kAquiringBusConnection;

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "Config::OwnBusName: Request for owning bus name %s...",
                        bus_name);

        own_id_ = g_bus_own_name(G_BUS_TYPE_SYSTEM,
                                 bus_name,
                                 G_BUS_NAME_OWNER_FLAGS_REPLACE,
                                 Config::OnBusAcquiredDBus,
                                 Config::OnNameAcquiredDBus,
                                 Config::OnNameLostDBus,
                                 this,
                                 NULL);

        if (own_id_ == 0)
        {
            // Failed to acquire the bus.
            local_state_ = State::kError;

            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Config: Failed to aqcuire bus name");
        }
        else
        {
            DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Config: Will acquire bus %s", bus_name);
        }
    }

    return rc;
}

fscfg_ReturnCode Config::BindBusAcquired(std::function<fscfg_ReturnCode (BusAcquiredEvent)> func, std::uint32_t& id)
{
    id = std::get<kBusAcquired>(bind_ex_.tup_).Register(func);

    return fscfg_kRcSuccess;
}

void Config::OnBusAcquiredDBus(GDBusConnection* connection, const gchar* name, gpointer config_instance)
{
    Config* config = reinterpret_cast<Config*>(config_instance);
    config->connection_ = connection;

    // Export config interface on the bus.
    gboolean iface_exported = g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(config->config_dbus_obj_),
                                                               connection,
                                                               config->config_container_path_.c_str(),
                                                               NULL);

    if (!iface_exported)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Config: Failed to export interface");
        config->local_state_ = State::kError;
    }
    else
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO , "Config: interface exported");

        config->objmgr_encoding_->SetConnection(connection);
        config->objmgr_source_->SetConnection(connection);
        config->objmgr_resource_->SetConnection(connection);

        config->local_state_ = State::kBusConnectionActive;

        config->OnBusAcquired(connection, name, config_instance);
    }
}

void Config::OnNameAcquiredDBus(GDBusConnection* connection, const gchar* name, gpointer config_instance)
{
    DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Config: Name acquired %s", name);
}

void Config::OnNameLostDBus(GDBusConnection* connection, const gchar* name, gpointer config_instance)
{
    // this isn't expected. If the name is lost that means that another process (daemon) acquired the
    // DBus name.
    DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_ERROR, "Config: Name lost %s", name);
}

} // namespace fsm

/** \}    end of addtogroup */
