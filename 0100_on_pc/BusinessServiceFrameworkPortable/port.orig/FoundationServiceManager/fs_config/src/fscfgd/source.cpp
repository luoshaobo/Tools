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
 *  \file     source.cpp
 *  \brief    Foundation Services Config implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/source.h>

#include <algorithm>

#include <dlt/dlt.h>

#include <fscfg/bus_names.h>

#include <fscfgd/resource.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd);

namespace fsm
{

Source::Source(const std::string& name)
 : name_(name),
   source_dbus_obj_(nullptr),
   payloads_(),
   resources_(),
   encodings_(),
   update_parent_payloads_(true)
{
    source_dbus_obj_ = config_source_skeleton_new();
    config_source_set_name(source_dbus_obj_, name_.c_str());

    fscfg_ReturnCode rc = ConnectSignals();

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Source: Failed to connect D-Bus signals");
    }
}

Source::~Source()
{
    if (source_dbus_obj_)
    {
        g_object_unref(source_dbus_obj_);
    }
}

fscfg_ReturnCode Source::GetName(std::string& name)
{
    name.append(name_);

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Source::GetEncodings(std::shared_ptr<ResourceInterface> resource,
                                      std::vector<std::shared_ptr<EncodingInterface>>& encodings)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;
    std::string resource_name;

    PayloadMap& front = payloads_.Get();

    resource->GetName(resource_name);

    if (!front.count(resource_name))
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        for (auto& pair_name_payload : front[resource_name])
        {
            encodings.push_back(encodings_[pair_name_payload.first]);
        }
    }

    return rc;
}

fscfg_ReturnCode Source::GetPayload(std::shared_ptr<ResourceInterface> resource,
                                    std::shared_ptr<EncodingInterface> encoding,
                                    std::vector<uint8_t>& payload)
{
    return GetStoredPayload(resource, encoding, payload);
}

fscfg_ReturnCode Source::GetStoredPayload(std::shared_ptr<ResourceInterface> resource,
                                          std::shared_ptr<EncodingInterface> encoding,
                                          std::vector<uint8_t>& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource || !encoding)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        PayloadMap& front = payloads_.Get();

        std::string resource_name;
        std::string encoding_name;

        resource->GetName(resource_name);
        encoding->GetName(encoding_name);

        if (!front.count(resource_name))
        {
            // the given resource doesn't have a payload described by this source.
            rc = fscfg_kRcBadParam;
        }
        else if (!front[resource_name].count(encoding_name))
        {
            // this source doesn't have a payload in mentioned encoding.
            rc = fscfg_kRcBadParam;
        }
        else
        {
            const Payload& stored_payload = front[resource_name][encoding_name];
            payload.insert(payload.end(), stored_payload.begin(), stored_payload.end());
        }
    }

    return rc;
}

fscfg_ReturnCode Source::HasResource(std::shared_ptr<ResourceInterface> resource, bool& has_resource)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string resource_name;

        resource->GetName(resource_name);
        has_resource = HasResource(resource_name);
    }

    return rc;
}

bool Source::HasResource(const std::string& name)
{
    PayloadMap& front = payloads_.Get();
    bool found = false;

    if (front.count(name))
    {
        found = true;
    }

    return found;
}

fscfg_ReturnCode Source::GetObject(_ConfigSource*& source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!source_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        source = source_dbus_obj_;
    }

    return rc;
}

fscfg_ReturnCode Source::UpdateParentPayloads(const std::string& resource_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!update_parent_payloads_)
    {
        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "Source(%s)::UpdateParentPayloads: Skip updating parents for %s",
                        name_.c_str(),
                        resource_name.c_str());
    }
    else
    {
        std::vector<std::string> resource_names;
        rc = fscfg_GetParentResourceNames(resource_name.c_str(), resource_names);

        if (rc == fscfg_kRcSuccess && resource_names.size())
        {
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_INFO,
                            "Source(%s)::UpdateParentPayloads: Updating parents for %s....",
                            name_.c_str(),
                            resource_name.c_str());

            const std::string& parent_resource_name = resource_names[resource_names.size() - 1];

            PayloadMap& front = payloads_.Get();
            std::vector<std::uint8_t> payload;
            std::map<std::string, Payload>::iterator first_encoding = front[parent_resource_name].begin();

            if (first_encoding == front[parent_resource_name].end())
            {
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_ERROR,
                                "Source(%s)::UpdateParentPayloads: no available encodings for resource %s",
                                name_.c_str(),
                                parent_resource_name.c_str());
            }
            else
            {
                std::shared_ptr<Encoding> available_encoding = encodings_[first_encoding->first];
                std::shared_ptr<Resource> parent_resource = resources_[parent_resource_name];

                std::string available_encoding_name;
                available_encoding->GetName(available_encoding_name);

                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_INFO,
                                "Source(%s)::UpdateParentPayloads: Updating payload %s in encoding %s as parent of %s",
                                name_.c_str(),
                                parent_resource_name.c_str(),
                                available_encoding_name.c_str(),
                                resource_name.c_str());

                fscfg_ReturnCode resource_payload_rc =
                        parent_resource->GetPayload(available_encoding,
                                                    std::dynamic_pointer_cast<Source>(shared_from_this()),
                                                    payload);

                if (resource_payload_rc != fscfg_kRcSuccess)
                {
                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_ERROR,
                                    "Source(%s)::UpdateParentPayloads: failed to update parent %s",
                                    name_.c_str(),
                                    parent_resource_name.c_str());

                    rc = fscfg_kRcError;
                }
            }
        }
    }

    return rc;
}


fscfg_ReturnCode Source::Update()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!source_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        std::set<std::string> resource_names;   ///< current resources.
        PayloadMap back(payloads_.GetBack());   ///< Used to compare payloads.

        payloads_.Update();

        MapToKeySet(payloads_.Get(), resource_names);
        PayloadMap& front = payloads_.Get();

        std::vector<std::shared_ptr<ResourceInterface>> resources;

        // for all current resources, compare current payload with past payload and check if any diff;
        // if there's a diff, report that resource as being changed.
        for (const std::string& resource_name : resource_names)
        {
            // Check that all payloads are the same. If they are not, add it to the reported set.
            // Check each current encoding payload with the old one.

            for (auto& pair_enc_payload : front[resource_name])
            {
                bool payload_changed = false;

                // check if the back has a payload in the front encoding.
                const std::string& enc_name = pair_enc_payload.first;

                // check if back has resource at all.
                PayloadMap::iterator it = back.find(resource_name);

                if (it == back.end())
                {
                   // resource is newly introduced in front (in last update) - didn't exist prior.
                   // we mark it as changed in order to report it.
                   payload_changed = true;
                }
                else
                {
                   // check if the encoding existed before.
                   // if it didn't exist, it's a new payload for a new encoding, otherwise it can only be marked
                   // as changed if there's a binary-diff between the payloads (old and new).
                   std::map<std::string, Payload>& enc_map = it->second;
                   std::map<std::string, Payload>::iterator it = enc_map.find(enc_name);

                   if (it == enc_map.end())
                   {
                       // encoding is newly introduced - we mark payload as changed in order to report it.
                       payload_changed = true;
                   }
                   else
                   {
                       // encoding existed in the past and it still exists - we perform binary-diff
                       // to identify if the payload contents have changed.
                       if (pair_enc_payload.second != it->second)
                       {
                           // binary-diff, payload has changed.
                           payload_changed = true;
                       }
                   }
                }

                if (payload_changed)
                {
                   // Look-up names to actual objects.
                   std::shared_ptr<Resource> resource = resources_[resource_name];
                   std::shared_ptr<Encoding> encoding = encodings_[enc_name];

                   resource->SetPayloadEntryChanged(encoding, std::dynamic_pointer_cast<Source>(shared_from_this()));
                   resource->Update(std::dynamic_pointer_cast<Source>(shared_from_this()));

                   resources.push_back(resource);
                }
            }
        }

        std::vector<std::shared_ptr<ResourceInterface>> resource_bases(resources.begin(), resources.end());

        if (resource_bases.size())
        {
            OnUpdated(resource_bases);
        }

        payloads_.Update();
    }

    return rc;
}

fscfg_ReturnCode Source::OnUpdated(std::vector<std::shared_ptr<ResourceInterface>> resources)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!source_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        const char* base_path;
        std::vector<std::string> names;
        std::vector<std::string> full_paths_storage;
        std::vector<const char*> full_paths;

        // convert resources to vector of names.
        // We will use our utility function to convert this vector of names to something that d-bus generated code
        // understands.
        for (auto& resource : resources)
        {
            std::string name;
            resource->GetName(name);
            names.push_back(name);
        }

        fscfg_GetObjectPath(&base_path, fscfg_kBotConfigSource);

        // convert to glib array of strings and signal D-bus.
        ConvertNames(names, base_path, full_paths_storage, full_paths);


        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "Source(%s)::OnUpdated: Emitting updated",
                        name_.c_str());

        config_source_emit_updated(source_dbus_obj_, &full_paths[0]);

        // call bindings.
        rc = SourceBind::OnUpdated(resources);

        // Logging.
         DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Source(%s): Updated resources...", name_.c_str());

         for (auto& resource : resources)
         {
             std::string resource_name;

             resource->GetName(resource_name);

              DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO,
                              "Source(%s): Resource->%s",
                              name_.c_str(),
                              resource_name.c_str());
         }

         DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Source(%s): End updated resources!", name_.c_str());
    }

    return rc;
}

fscfg_ReturnCode Source::GetResources(std::vector<std::shared_ptr<ResourceInterface>>& resources)
{
    for (auto& pair_name_resource : resources_)
    {
        resources.push_back(pair_name_resource.second);
    }

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Source::GetEncodings(std::vector<std::shared_ptr<EncodingInterface>>& encodings)
{
    for (auto& pair_name_encoding : encodings_)
    {
        encodings.push_back(pair_name_encoding.second);
    }

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Source::ConnectSignals()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!source_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // source_dbus_obj is null.
    }
    else
    {
        if (!g_signal_connect(source_dbus_obj_, "handle-update", G_CALLBACK(Source::OnUpdateDbus), this))
        {
            rc = fscfg_kRcError;
        }
        else if (!g_signal_connect(source_dbus_obj_, "handle-get-encodings", G_CALLBACK(Source::OnGetEncodingsDbus), this))
        {
            rc = fscfg_kRcError;
        }
        else if (!g_signal_connect(source_dbus_obj_, "handle-get-payload", G_CALLBACK(Source::OnGetPayloadDbus), this))
        {
            rc = fscfg_kRcError;
        }
        else if (!g_signal_connect(source_dbus_obj_, "handle-has-resource", G_CALLBACK(Source::OnHasResourceDbus), this))
        {
            rc = fscfg_kRcError;
        }
    }

    return rc;
}

fscfg_ReturnCode Source::SetPayloadEntry(std::shared_ptr<Resource> resource,
                                         std::shared_ptr<Encoding> encoding,
                                         const Payload& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource || !encoding)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        PayloadMap& front = payloads_.Get();
        std::string resource_name;
        std::string encoding_name;

        resource->GetName(resource_name);
        encoding->GetName(encoding_name);

        front[resource_name][encoding_name] = payload;

        resources_[resource_name] = resource;
        encodings_[encoding_name] = encoding;

        resource->AddPayloadEntry(encoding, std::dynamic_pointer_cast<Source>(shared_from_this()));
    }

    return rc;
}

fscfg_ReturnCode Source::RemovePayloadEntry(std::shared_ptr<Resource> resource,
                                            std::shared_ptr<Encoding> encoding)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource || !encoding)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        PayloadMap& front = payloads_.Get();
        std::string resource_name;
        std::string encoding_name;

        resource->GetName(resource_name);
        encoding->GetName(encoding_name);

        front[resource_name].erase(encoding_name);

        // If the resource no longer contains any payloads, remove it also.
        if (front[resource_name].size() == 0)
        {
            front.erase(resource_name);
            resources_.erase(resource_name);
        }

        resource->RemovePayloadEntry(encoding, std::dynamic_pointer_cast<Source>(shared_from_this()));
    }

    return rc;
}

fscfg_ReturnCode Source::SetUpdateParentPayloads(bool update_parent_payloads)
{
    update_parent_payloads_ = update_parent_payloads;

    DLT_LOG_STRINGF(dlt_fscfgd,
                    DLT_LOG_INFO,
                    "Source(%s)::SetUpdateParentPayloads: update_parent_payloads=%s",
                    name_.c_str(),
                    update_parent_payloads ? "true" : "false");

    return fscfg_kRcSuccess;
}

gboolean Source::OnUpdateDbus(_ConfigSource* source_dbus_obj, GDBusMethodInvocation* invoc, Source* source)
{
    gboolean result = TRUE;

    if (!source_dbus_obj || !invoc || !source)
    {
        result = FALSE;
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Source: Bad state on D-bus Update call");
    }
    else
    {
        source->Update();

        config_source_complete_update(source_dbus_obj, invoc);
    }

    return result;
}

gboolean Source::OnGetEncodingsDbus(_ConfigSource* source_dbus_obj,
                                    GDBusMethodInvocation* invoc,
                                    const char* resource_path,
                                    Source* source)
{
    gboolean result = TRUE;

    if (!source_dbus_obj || !invoc  || !resource_path || !source)
    {
        result = FALSE;
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Source: Bad state on D-bus GetEncodings call");
    }
    else
    {
        std::string resource_name;

        GetBasename(resource_path, resource_name);

        if (!source->resources_.count(resource_name))
        {
            result = FALSE;
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Source: resource not found on D-bus GetEncodings call");
        }
        else
        {
            // Create vector of encoding names.
            std::vector<std::string> encoding_names;
            std::vector<std::shared_ptr<EncodingInterface>> encodings;

            std::shared_ptr<ResourceInterface> resource = source->resources_[resource_name];

            source->GetEncodings(resource, encodings);

            for (auto& enc : encodings)
            {
                std::string enc_name;
                enc->GetName(enc_name);

                encoding_names.push_back(enc_name);
            }

            // Create the glib array of paths and complete method invocation.
            const char* base_path;
            std::vector<std::string> full_paths_storage;
            std::vector<const char*> full_paths;

            // Get the base_path for encodings.
            fscfg_GetObjectPath(&base_path, fscfg_kBotConfigEncoding);
            ConvertNames(encoding_names, base_path, full_paths_storage, full_paths);

            config_source_complete_get_encodings(source_dbus_obj, invoc, &full_paths[0]);
        }
    }

    return result;
}

gboolean Source::OnGetPayloadDbus(_ConfigSource* source_dbus_obj,
                                 GDBusMethodInvocation* invoc,
                                 const char* resource_path,
                                 const char* encoding_path,
                                 Source* source)
{
    gboolean result = TRUE;

    if (!source_dbus_obj || !invoc || !resource_path || !encoding_path || !source)
    {
        result = FALSE;
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Source: Bad state on D-bus GetEncodings call");
    }
    else
    {
        // Find interface instance for both encoding and resource (by name).
        // Extract the name from the path.

        std::string resource_name;
        std::string encoding_name;

        GetBasename(resource_path, resource_name);
        GetBasename(encoding_path, encoding_name);

        std::shared_ptr<EncodingInterface> encoding(nullptr);
        std::shared_ptr<ResourceInterface> resource(nullptr);

        std::vector<std::shared_ptr<EncodingInterface>> encodings;
        std::vector<std::shared_ptr<ResourceInterface>> resources;

        source->GetEncodings(encodings);
        source->GetResources(resources);

        // find encoding by name.
        for (auto& enc : encodings)
        {
            std::string name;
            enc->GetName(name);

            if (name == encoding_name)
            {
                encoding = enc;
                break;
            }
        }

        // find resource by name.
        for (auto& res : resources)
        {
            std::string name;
            res->GetName(name);

            if (name == resource_name)
            {
                resource = res;
                break;
            }
        }

        if (!resource || !encoding)
        {
            // resource or encoding was not found inside source.
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Source: source or encoding not found on D-bus GetPayload call");
            result = FALSE;
        }
        else
        {
            std::vector<std::uint8_t> payload;
            fscfg_ReturnCode rc = source->GetPayload(resource, encoding, payload);

            if (rc != fscfg_kRcSuccess)
            {
                result = FALSE;
                DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Source: Failed to retrieve paylod from source on D-bus"
                                                           "GetPayload call");
            }
            else
            {
                // convert payload to GVariant.
                GVariant* g_payload =
                g_variant_new_from_data(G_VARIANT_TYPE("ay"), &payload[0], payload.size(), TRUE, nullptr, nullptr);
                config_source_complete_get_payload(source_dbus_obj, invoc, g_payload);
            }
        }
    }

    return result;
}

gboolean Source::OnHasResourceDbus(_ConfigSource* source_dbus_obj,
                                   GDBusMethodInvocation* invoc,
                                   const char* resource_path,
                                   Source* source)
{
    gboolean result = TRUE;


    if (!source_dbus_obj || !invoc || !resource_path || !source)
    {
        result = FALSE;
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Source: Bad state on D-bus HasResource call");
    }
    else
    {
        std::string base_name;
        bool has_resource;

        GetBasename(resource_path, base_name);

        has_resource = source->HasResource(base_name);

        config_source_complete_has_resource(source_dbus_obj, invoc, has_resource);
    }

    return result;
}

} // namespace fsm

/** \}    end of addtogroup */
