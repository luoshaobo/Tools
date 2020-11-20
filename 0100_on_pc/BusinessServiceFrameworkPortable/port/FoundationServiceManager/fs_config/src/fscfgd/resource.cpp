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
 *  \file     resource.cpp
 *  \brief    Foundation Services Resource implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/resource.h>

#include <dlt/dlt.h>

#include <fscfg/bus_names.h>
#include <fscfg_common/utilities.h>

#include <fscfgd/config.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd);

namespace fsm
{

Resource::Resource(const std::string& name, std::shared_ptr<Config> config)
 : name_(name),
   config_(config),
   resource_dbus_obj_(nullptr)
{
    resource_dbus_obj_ = config_resource_skeleton_new();

    if (resource_dbus_obj_)
    {
        config_resource_set_name(resource_dbus_obj_,name_.c_str());

        fscfg_ReturnCode rc = ConnectSignals();

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Resource: Failed to connect D-Bus signals");
        }
    }
}

Resource::~Resource()
{
    if (resource_dbus_obj_)
    {
        g_object_unref(resource_dbus_obj_);
    }
}

fscfg_ReturnCode Resource::GetName(std::string& name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        name.append(name_);
    }

    return rc;
}

fscfg_ReturnCode Resource::GetSources(std::vector<std::shared_ptr<SourceInterface>>& sources)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!config_)
    {
        rc = fscfg_kRcBadState; // null config.
    }
    else
    {
        // Ask config to supply all registered sources and check each one if it describes a payload for this resource.
        std::vector<std::shared_ptr<SourceInterface>> all_sources;

        rc = config_->GetAll(all_sources);

        if (rc == fscfg_kRcSuccess)
        {
            std::vector<std::shared_ptr<SourceInterface>> owner_sources;

            for (auto& source : all_sources)
            {
                bool has_resource = false;

                rc = source->HasResource(std::dynamic_pointer_cast<Resource>(shared_from_this()), has_resource);

                if (rc != fscfg_kRcSuccess)
                {
                    break;
                }
                else if (has_resource)
                {
                    owner_sources.push_back(source);
                }
            }

            if (rc == fscfg_kRcSuccess)
            {
                for (auto& source : owner_sources)
                {
                    sources.push_back(source);
                }
            }
        }
    }

    return rc;
}

fscfg_ReturnCode Resource::GetEncodings(std::vector<std::shared_ptr<EncodingInterface>>& encodings)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    // Get all sources that describe this resource and for each one, grab the encodings.
    // After the above-mentioned iteration, there is the possibility that we have an encoding more than once -
    // - we will perform a reduction to the unique set.

    std::vector<std::shared_ptr<SourceInterface>> resource_srcs;

    rc = GetSources(resource_srcs);

    if (rc == fscfg_kRcSuccess)
    {
        std::vector<std::shared_ptr<EncodingInterface>> resource_encs;

        for (auto& source : resource_srcs)
        {

            rc = source->GetEncodings(std::dynamic_pointer_cast<Resource>(shared_from_this()), resource_encs);

            if (rc != fscfg_kRcSuccess)
            {
                break;
            }
        }

        if (rc == fscfg_kRcSuccess)
        {
            // As we want to supply the unique set of encodings, we will have to set and reduce to unique.
            // Sort by encoding name.
            std::sort(resource_encs.begin(), resource_encs.end(),
                      [](std::shared_ptr<EncodingInterface>& enc_a, std::shared_ptr<EncodingInterface>& enc_b)
            {
                std::string enc_a_name;
                std::string enc_b_name;

                enc_a->GetName(enc_a_name);
                enc_b->GetName(enc_b_name);

                return enc_b < enc_a;
            });

            // Reduce to unique set.
            std::vector<std::shared_ptr<EncodingInterface>>::iterator it_end
                    = std::unique(resource_encs.begin(),
                                  resource_encs.end(),
                                  [](std::shared_ptr<EncodingInterface>& enc_a,
                                  std::shared_ptr<EncodingInterface>& enc_b)
            {
                std::string enc_a_name;
                std::string enc_b_name;

                enc_a->GetName(enc_a_name);
                enc_b->GetName(enc_b_name);

                return enc_b == enc_a;
            });

            // Finally, populate the output parameter.
            std::vector<std::shared_ptr<EncodingInterface>>::iterator it = resource_encs.begin();

            for(; it != it_end; ++it)
            {
                encodings.push_back((*it));
            }
        }

    }

    return rc;
}

fscfg_ReturnCode Resource::GetPayload(std::shared_ptr<EncodingInterface> encoding,
                                      std::shared_ptr<SourceInterface> source,
                                      std::vector<std::uint8_t>& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!encoding || !source)
    {
        rc = fscfg_kRcBadParam; // null encoding or source.
    }
    else
    {
        rc = source->GetPayload(std::dynamic_pointer_cast<Resource>(shared_from_this()), encoding, payload);
    }

    return rc;
}

fscfg_ReturnCode Resource::GetObject(_ConfigResource*& resource)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        resource = resource_dbus_obj_;
    }

    return rc;
}

fscfg_ReturnCode Resource::AddPayloadEntry(std::shared_ptr<Encoding> encoding, std::shared_ptr<Source> source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!encoding || !source)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string encoding_name;
        std::string source_name;

        encoding->GetName(encoding_name);
        source->GetName(source_name);

        PayloadMap& front = payloads_.Get();

        // Payload is new, as such it is marked as changed.
        front[source_name][encoding_name] = true;
        sources_[source_name] = source;
        encodings_[encoding_name] = encoding;
    }

    return rc;
}

fscfg_ReturnCode Resource::RemovePayloadEntry(std::shared_ptr<Encoding> encoding, std::shared_ptr<Source> source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!encoding || !source)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string encoding_name;
        std::string source_name;

        encoding->GetName(encoding_name);
        source->GetName(source_name);

        PayloadMap& front = payloads_.Get();

        front[source_name].erase(encoding_name);

        if (front[source_name].size() == 0)
        {
            // as there are no more payloads for source, we remove the source also.
            front.erase(source_name);
            sources_.erase(source_name);
        }

        // if the encoding can't be found in any of the sources anymore, we remove the mapping.
        bool found = false;

        for (auto& pair_src_enc_payload : front)
        {
            for (auto& pair_enc_payload : pair_src_enc_payload.second)
            {
                if (pair_enc_payload.first == encoding_name)
                {
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            encodings_.erase(encoding_name);
        }
    }

    return rc;
}

fscfg_ReturnCode Resource::SetPayloadEntryChanged(std::shared_ptr<Encoding> encoding, std::shared_ptr<Source> source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!encoding || !source)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string encoding_name;
        std::string source_name;

        encoding->GetName(encoding_name);
        source->GetName(source_name);

        PayloadMap& front = payloads_.Get();

        front[source_name][encoding_name] = true; // mark the encoding as being changed.
    }

    return rc;
}


fscfg_ReturnCode Resource::Update(std::shared_ptr<Source> source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!source)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        // TODO: We should have OnEncodingsChanged - all interfaces need to be adapted.
        // Because we've decided not to handle the multiple encodings thingie, the above is not present in the interface.

        // compute OnSourcesChanged.
        {
            std::vector<std::string> new_sources;
            std::vector<std::string> deleted_sources;

            BufferedDataSetDiff(payloads_, new_sources, deleted_sources);

            if (new_sources.size() != 0 || deleted_sources.size() != 0)
            {

                std::vector<std::shared_ptr<SourceInterface>> sources;

                for (auto& pair_name_src : sources_)
                {
                    sources.push_back(pair_name_src.second);
                }

                OnSourcesChanged(sources);
            }
        }

        // compute OnPayloadsChanged.
        {
            // gather all payloads marked as changed in order to signal "PayloadsChanged".
            std::vector<std::tuple<std::vector<std::uint8_t>,
                        std::shared_ptr<SourceInterface>,
                        std::shared_ptr<EncodingInterface>>> changed_payloads;

            PayloadMap& front = payloads_.Get();

            for (auto& pair_src_enc_payload : front)
            {
                const std::string& source_name = pair_src_enc_payload.first;
                std::shared_ptr<Source> source = sources_[pair_src_enc_payload.first];

                for (auto& enc_payload : pair_src_enc_payload.second)
                {
                    const std::string& encoding_name = enc_payload.first;

                    std::shared_ptr<Encoding> encoding = encodings_[encoding_name];

                    // marked as true if changed - just collect it.
                    if (front[source_name][encoding_name])
                    {
                        std::vector<std::uint8_t> payload;

                        source->GetStoredPayload(std::dynamic_pointer_cast<Resource>(shared_from_this()),
                                                 encoding,
                                                 payload);

                        // collect it 'cause it's marked as changed.
                        changed_payloads.push_back(std::make_tuple(payload, source, encoding));
                    }
                }
            }

            // Just iterate over what we collected, signal on D-bus and call our bindings.
            for (auto& changed_payload : changed_payloads)
            {
                std::shared_ptr<SourceInterface> source;
                std::shared_ptr<EncodingInterface> encoding;

                std::tie(std::ignore, source, encoding) = changed_payload;

                std::string source_name;
                std::string encoding_name;

                if(source)//modify klocwork warning,yangjun add 20181107
                {
                    source->GetName(source_name);
                }
                
                if(encoding)//modify klocwork warning,yangjun add 20181107
                {
                    encoding->GetName(encoding_name);
                }
                
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_INFO,
                                "Resource(%s)::Update: payload from %s in %s changed",
                                name_.c_str(),
                                source_name.c_str(),
                                encoding_name.c_str());

                OnPayloadsChanged(std::get<0>(changed_payload),
                                  std::get<1>(changed_payload),
                                  std::get<2>(changed_payload));
            }
        }


        payloads_.Update(); // equalize front and back.

        OnUpdated(source);

        // Logging.
        std::string source_name;
        source->GetName(source_name);

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "Resource(%s): Updated by src: %s",
                        name_.c_str(),
                        source_name.c_str());
    }

    return rc;
}

fscfg_ReturnCode Resource::ProcessExpiry(bool immediate)
{
    return OnExpired(immediate);
}

fscfg_ReturnCode Resource::ConnectSignals()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
         rc = fscfg_kRcBadState; // resource_dbus_obj_ is null.
    }
    else
    {
        if (!g_signal_connect(resource_dbus_obj_, "handle-get-payload", G_CALLBACK(Resource::OnGetPayloadDbus), this))
        {
            rc = fscfg_kRcError;
        }
    }

    return rc;
}

fscfg_ReturnCode Resource::OnExpired(bool immediate)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // resource_dbus_obj_ is null.
    }
    else
    {
        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "Resource(%s)::OnExpired: Emitting expired, immediate: %s",
                        name_.c_str(),
                        immediate ? "true" : "false");

        config_resource_emit_expired(resource_dbus_obj_, immediate);

        // call bindings.
        ResourceBind::OnExpired(immediate);
    }

    return rc;
}

fscfg_ReturnCode Resource::OnUpdated(std::shared_ptr<SourceInterface> source)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // resource_dbus_obj_ is null.
    }
    else if (!source)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        // compute the full path of the given source and emit the signal.
        const char* base_path;
        std::string source_name;

        source->GetName(source_name);
        fscfg_GetObjectPath(&base_path, fscfg_kBotConfigSource);

        std::string full_path = std::string(base_path) + "/" + source_name;

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "Resource(%s)::OnUpdated: Emitting updated, source: %s",
                        name_.c_str(),
                        source_name.c_str());

        config_resource_emit_updated(resource_dbus_obj_, full_path.c_str());

        // call bindings.
        ResourceBind::OnUpdated(source);
    }

    return rc;
}

fscfg_ReturnCode Resource::OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // resource_dbus_obj_ is null.
    }
    else
    {
        // Create vector of source names.
        std::vector<std::string> source_names;

        for (auto& src : sources)
        {
            std::string src_name;

            src->GetName(src_name);
            source_names.push_back(src_name);
        }

        // compute the full paths of the given sources and emit the signal.
        const char* base_path;
        std::vector<std::string> full_paths_storage;
        std::vector<const char*> full_paths;

        fscfg_GetObjectPath(&base_path, fscfg_kBotConfigSource);

        ConvertNames(source_names, base_path, full_paths_storage, full_paths);
        config_resource_set_sources(resource_dbus_obj_, &full_paths[0]);

        // call bindings.
        ResourceBind::OnSourcesChanged(sources);

        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Resource(%s)::OnSourcesChanged: Start Sources...", name_.c_str());

        for (auto& src_name : source_names)
        {
            DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Resource(%s): Source->%s", name_.c_str(), src_name.c_str());
        }

        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "Resource(%s)::OnSourcesChanged: End Sources", name_.c_str());
    }

    return rc;
}

fscfg_ReturnCode Resource::OnPayloadsChanged(std::vector<std::uint8_t> payload,
                                              std::shared_ptr<SourceInterface> source,
                                              std::shared_ptr<EncodingInterface> encoding)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource_dbus_obj_)
    {
        rc = fscfg_kRcBadState; // resource_dbus_obj_ is null.
    }
    else if (!source || !encoding)
    {
        rc = fscfg_kRcBadParam; // null source or null encoding.
    }
    else
    {
        // Prepare the full paths and put the payload on the bus.
        const char* source_base;
        const char* encoding_base;

        fscfg_GetObjectPath(&source_base, fscfg_kBotConfigSource);
        fscfg_GetObjectPath(&encoding_base, fscfg_kBotConfigEncoding);

        std::string source_name;
        std::string encoding_name;

        source->GetName(source_name);
        encoding->GetName(encoding_name);

        const std::string kSourcePath = std::string(source_base) + "/" + source_name;
        const std::string kEncodingPath = std::string(encoding_base) + "/" + encoding_name;

        GVariant* g_payload =
        g_variant_new_from_data(G_VARIANT_TYPE("ay"), &payload[0], payload.size(), TRUE, nullptr, nullptr);

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "Resource(%s)::OnPayloadsChanged: Emitting payloads changed, source: %s, encoding: %s",
                        name_.c_str(),
                        source_name.c_str(),
                        encoding_name.c_str());

        config_resource_emit_payloads_changed(resource_dbus_obj_, kEncodingPath.c_str(), kSourcePath.c_str(), g_payload);

        // call bindings.
        rc = ResourceBind::OnPayloadsChanged(payload, source, encoding);

        // Logging.
        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "Resource(%s): new payload for src:%s in enc:%s",
                        name_.c_str(),
                        source_name.c_str(),
                        encoding_name.c_str());
    }

    return rc;
}

void null_deleter(Resource*) {}

gboolean Resource::OnGetPayloadDbus(_ConfigResource* resource_dbus_obj_,
                                    GDBusMethodInvocation* invoc,
                                    const char* encoding_path,
                                    const char* source_path,
                                    Resource* resource)
{

    gboolean result = TRUE;

    if (!resource)
    {
        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_ERROR,
                        "Resource::OnGetPayloadDbus: bad resource for request. Parameters encoding: %s source: %s",
                        encoding_path,
                        source_path);
    }
    else
    {

        std::string source_name;
        std::string encoding_name;

        GetBasename(source_path, source_name);
        GetBasename(encoding_path, encoding_name);

        std::shared_ptr<SourceInterface> source;
        std::shared_ptr<EncodingInterface> encoding;

        resource->config_->Get(source_name, source);
        resource->config_->Get(encoding_name, encoding);

        const char* resource_name  = config_resource_get_name(resource_dbus_obj_);

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "Resource::OnGetPayloadDbus(%s): source: %s encoding: %s",
                        resource_name,
                        source_name.c_str(),
                        encoding_name.c_str());

        if (!source || !encoding)
        {
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_ERROR,
                            "Resource: encoding %s or source %s has invalid name. Config didn't find the object!",
                            source_name.c_str(),
                            encoding_name.c_str());

            result = FALSE;
        }
        else
        {
            std::vector<std::uint8_t> payload;

            std::string source_name;
            source->GetName(source_name);

            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_INFO,
                            "Resource(%s)::OnGetPayloadDbus: Retrieving payload from source %s...",
                            resource_name,
                            source_name.c_str());

            fscfg_ReturnCode rc = source->GetPayload(std::shared_ptr<ResourceInterface>(resource, &null_deleter),
                                                     encoding,
                                                     payload);

            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_INFO,
                            "Resource(%s)::OnGetPayloadDbus: Retrieved payload from source %s!",
                            resource_name,
                            source_name.c_str());

            if (rc != fscfg_kRcSuccess)
            {
                DLT_LOG_STRING(dlt_fscfgd,
                               DLT_LOG_ERROR,
                               "Resource::OnGetPayloadDbus: Failed to retrieve payload from source on"
                               " GetPayload D-bus call");
                result = FALSE;
            }
            else
            {
                // convert payload to GVariant.
                GVariant* g_payload =
                g_variant_new_from_data(G_VARIANT_TYPE("ay"), &payload[0], payload.size(), TRUE, nullptr, nullptr);

                config_resource_complete_get_payload(resource_dbus_obj_, invoc, g_payload);
            }
        }
    }

    return result;
}

} // namespace fsm

/** \}    end of addtogroup */
