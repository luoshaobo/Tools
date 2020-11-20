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
 *  \file     vsdpsource.cpp
 *  \brief    Foundation Services VsdpSource interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/vsdpsource.h>

#include <fscfg/bus_names.h>

#include <dlt/dlt.h>

#include <fscfgd/fscfgd.h>
#include <fscfgd/provisionedresource.h>


DLT_IMPORT_CONTEXT(dlt_fscfgd)

namespace fsm
{

VsdpSource::VsdpSource(std::shared_ptr<fsm::Provisioning> provisioning,
                       std::shared_ptr<LocalConfig> local_config,
                       std::shared_ptr<transfermanager::TransferManager> transfer_manager)
 : Source("VSDP"),
   transfer_manager_(transfer_manager),
   uri_map_(),
   provisioning_(provisioning),
   config_(provisioning->GetConfig()),
   local_config_(local_config)
{
}

VsdpSource::~VsdpSource()
{
}

fscfg_ReturnCode VsdpSource::HasResource(std::shared_ptr<ResourceInterface> resource, bool& has_resource)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!resource)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string resource_name;

        rc = resource->GetName(resource_name);

        // If there's an URI mapping available, we report that the resource is available;
        // this way, clients can request the resource payload because it is advertised as being available.
        if (uri_map_.count(resource_name))
        {
            has_resource = true;
        }
        else
        {
            rc = Source::HasResource(resource, has_resource);
        }
    }

    return rc;
}

fscfg_ReturnCode VsdpSource::GetPayload(std::shared_ptr<ResourceInterface> resource,
                                        std::shared_ptr<EncodingInterface> encoding,
                                        std::vector<std::uint8_t>& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!transfer_manager_)
    {
        rc = fscfg_kRcBadState;
    }
    else if (!resource || !encoding)
    {
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::string resource_name;
        resource->GetName(resource_name);

        UriMap::iterator it = uri_map_.find(resource_name);

        if (it == uri_map_.end())
        {
            // Resource was not found.
            rc = fscfg_kRcNotFound;
        }
        else
        {
            // we have the resource in the uri map;
            // We will check if we also have a potential payload in the requested encoding.

            // Fetch and dispatch any updates on parents before.
            fscfg_ReturnCode update_parents_rc = UpdateParentPayloads(resource_name);

            if (update_parents_rc != fscfg_kRcSuccess)
            {
                // This might be pretty bad, but trying to actually fetch the payload for "resource" will not crash
                // if updating parents failed.
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_ERROR,
                                "VsdpSource::GetPayload: failed to update parents of %s",
                                resource_name.c_str());
            }

            //updating the parents might have introduced changes on the URI mapping, check again
            it = uri_map_.find(resource_name);

            if (it == uri_map_.end())
            {
                // Resource was not found.
                DLT_LOG_STRINGF(dlt_fscfgd,DLT_LOG_INFO,"VsdpSource::GetPayload: %s removed during parent update", resource_name.c_str());
                rc = fscfg_kRcNotFound;
            }
            else
            {
                std::string encoding_name;

                encoding->GetName(encoding_name);

                const std::string& stored_encoding = it->second.second;
                const std::string& stored_uri = it->second.first;

                if (stored_encoding != encoding_name)
                {
                    // Encoding names do not match - whoopsie.
                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_WARN,
                                    "VsdpSource::GetPayload requested encoding(%s) does not match stored encoding(%s)",
                                    encoding_name.c_str(),
                                    stored_encoding.c_str());

                    rc = fscfg_kRcBadParam;
                }
                else
                {
                    // We have everything we need - pass the request through TSM.
                    bool updated;
                    rc = Fetch(resource_name, stored_uri, encoding_name, payload, updated);

                    // Set the payload entry for binary-difference tracking.
                    // Perform down-casting by using the name of the objects and the config container.

                    std::shared_ptr<Encoding> specialized_encoding;
                    std::shared_ptr<Resource> specialized_resource;

                    rc = rc == fscfg_kRcSuccess ? config_->Get(resource_name, specialized_resource) : rc;
                    rc = rc == fscfg_kRcSuccess ? config_->Get(encoding_name, specialized_encoding) : rc;

                    if (rc != fscfg_kRcSuccess)
                    {
                        DLT_LOG_STRING(dlt_fscfgd,
                                       DLT_LOG_ERROR,
                                       "VsdpSource::GetPayload: failed to retrieve payload or specialized objects");
                    }
                    else
                    {
                        DLT_LOG_STRINGF(dlt_fscfgd,
                                        DLT_LOG_INFO,
                                        "VsdpSource::GetPayload: resource %s updated=%s",
                                        resource_name.c_str(),
                                        (updated ? "true" : "false"));

                        if (updated)
                        {
                            rc = SetPayloadEntry(specialized_resource, specialized_encoding, payload);
                            rc = rc == fscfg_kRcSuccess ? Update() : rc;

                            if (rc != fscfg_kRcSuccess)
                            {
                                DLT_LOG_STRING(dlt_fscfgd,
                                               DLT_LOG_ERROR,
                                               "VsdpSource::GetPayload: failed to update payload entry");
                            }
                        }
                    }
                }
            }
        }
    }

    return rc;
}

fscfg_ReturnCode VsdpSource::AddUriMapping(const std::string& uri, const std::string& resource_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    std::string encoding_name = fscfg_kXmlEncoding; // Default encoding if we can't map to it.
    rc = GetEncodingName(resource_name, encoding_name);
    bool run_unsecure;

    std::string request_uri = uri;

    DLT_LOG_STRINGF(dlt_fscfgd,
                    DLT_LOG_INFO,
                    "Maping resource %s to uri %s",
                    resource_name.c_str(),
                    uri.c_str());

    // Adapt protocol string if we're using ssl tunnel.
    if (!local_config_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        rc = local_config_->GetRunUnsecure(run_unsecure);

        if (rc == fscfg_kRcSuccess && !run_unsecure)
        {
            if (request_uri.compare(0, 5, "http:") == 0)
            {
                request_uri.insert(4, "s");
            }
        }
    }

    if (rc == fscfg_kRcSuccess)
    {
        std::shared_ptr<Encoding> encoding = nullptr;

        // check if such an encoding exists.
        rc = config_->Get(encoding_name, encoding);

        if (rc != fscfg_kRcSuccess)
        {
            encoding = std::make_shared<Encoding>(encoding_name);

            rc = config_->Register(encoding);
        }

        // Let's check if we actually have a resource.
        std::shared_ptr<ProvisionedResource> resource;

        rc = provisioning_->Get(resource_name, resource);

        if (rc != fscfg_kRcSuccess)
        {
            resource = std::make_shared<ProvisionedResource>(resource_name, provisioning_);
            rc = provisioning_->Register(resource);
        }

        uri_map_[resource_name] = std::pair<std::string, std::string>(request_uri, encoding_name);

        // Mark the resource as being available in the deduced encoding.
        encodings_[encoding_name] = encoding;

        PayloadMap& front = payloads_.Get();
        std::map<std::string, Source::Payload>::iterator it = front[resource_name].find(encoding_name);

        if (it == front[resource_name].end())
        {
            // A payload does not exist, we mark that it exists by passing an empty payload.
            SetPayloadEntry(resource, encoding, std::vector<std::uint8_t>());
        }
    }

    return rc;
}

fscfg_ReturnCode VsdpSource::RemoveUriMapping(const std::string& resource_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    UriMap::iterator it = uri_map_.find(resource_name);

    if (it == uri_map_.end())
    {
        rc = fscfg_kRcNotFound;
    }
    else
    {
        std::shared_ptr<Resource> resource = resources_[resource_name];

        uri_map_.erase(it);

        PayloadMap& front = payloads_.Get();

        // Remove all potential payload entries for the given resource.
        for (auto encoding_name_payload_pair : front[resource_name])
        {
            RemovePayloadEntry(resource, encodings_[encoding_name_payload_pair.first]);
        }
    }

    return rc;
}

fscfg_ReturnCode VsdpSource::GetUriForResource(const std::string& resource_name, std::string& uri)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    UriMap::iterator it = uri_map_.find(resource_name);

    if (it == uri_map_.end())
    {
        rc = fscfg_kRcNotFound;
    }
    else
    {
        uri = it->second.first;
    }

    return rc;
}

fscfg_ReturnCode VsdpSource::GetEncodingName(const std::string& resource_name, std::string& encoding_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    static std::map<std::string, std::string> resource_to_encoding = {
        std::make_pair(fscfg_kEntryPointResource, fscfg_kXmlEncoding),
        std::make_pair(fscfg_kFeaturesResource, fscfg_kXmlEncoding),
        std::make_pair(fscfg_kBasicCarControlResource, fscfg_kCcmEncoding),
        std::make_pair(fscfg_kAssistanceCallResource, fscfg_kCcmEncoding),
        std::make_pair(fscfg_kAssistanceCallCallCenterSettingsResource, fscfg_kCcmEncoding),
        std::make_pair(fscfg_kCarAccessResource, fscfg_kCcmEncoding),
        std::make_pair(fscfg_kExternalDiagnosticsResource, fscfg_kXmlEncoding),
        std::make_pair(fscfg_kExternalDiagnosticsRemoteSessionResource, fscfg_kXmlEncoding),
        std::make_pair(fscfg_kExternalDiagnosticsClientCapabilitiesResource, fscfg_kXmlEncoding)
    };

    std::map<std::string, std::string>::iterator it = resource_to_encoding.find(resource_name);

    if (it == resource_to_encoding.end())
    {
        // No known encoding in which the resource is to be fetched.
        rc = fscfg_kRcNotFound;
    }
    else
    {
        encoding_name = it->second;
    }

    return rc;
}

fscfg_ReturnCode VsdpSource::Fetch(std::string resource_name,
                                   std::string resource_uri,
                                   std::string encoding_name,
                                   std::vector<std::uint8_t>& payload,
                                   bool& updated)
{
    // Adjust the URI based on the resource-to-parameter list mapping.
    // TODO:Iulian: This should use TSM HTTP parameters.
    std::string parameterized_uri;
    fscfg_ReturnCode rc = ComputeUriWithParameters(resource_name, resource_uri, parameterized_uri);

    if (rc == fscfg_kRcSuccess)
    {
        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "VsdpSource::Fetch: Fetching... resource %s in encoding %s from uri %s",
                        resource_name.c_str(),
                        encoding_name.c_str(),
                        parameterized_uri.c_str());

        transfermanager::Request request(parameterized_uri);
        std::shared_ptr<transfermanager::Response> response;

        request.AddHeaderField(transfermanager::Message::FieldKey::kAcceptLanguage, "se");
        request.AddHeaderField(transfermanager::Message::FieldKey::kAcceptLanguage, "en-gb;q=0.8");
        request.AddHeaderField(transfermanager::Message::FieldKey::kAcceptLanguage, "en;q=0.7");

        std::string content_type;

        if (GetContentType(resource_name, content_type) == fscfg_kRcNotFound)
        {
            // fall-back to using the resource name for the content type, if we didn't find it.
            content_type = std::string("application/volvo.cloud.") + resource_name;

            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_WARN,
                            "VspdpSource::GetPayload: Using content type as resource name for %s",
                            resource_name.c_str());
        }

        // CCM+ASN.1 is not a valid bus name, as such the name of the encoding is just "CCM";
        // the content_type needs to be adapted to "CCM+ASN.1" if it's "CCM", otherwise server might not understand
        // the request.
        content_type += "+" + encoding_name;
        content_type += encoding_name == "CCM" ? "+ASN.1" : "";

        request.AddHeaderField(transfermanager::Message::FieldKey::kAccept,
                               content_type);

        transfermanager::ErrorCode tsm_ec = transfer_manager_->Read(request, response);

        if (tsm_ec != transfermanager::ErrorCode::kNone)
        {
            // transfermanager read failed.

            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_ERROR,
                            "VsdpSource::GetPayload:TransferManager::Read failed %u",
                            static_cast<unsigned>(tsm_ec));

            rc = fscfg_kRcError;
        }
        else
        {
            // Perform the actual read from the TransferManager response.
            std::stringstream payload_stream;
            std::size_t payload_size;

            tsm_ec = response->Read(payload_stream, payload_size);

            if (tsm_ec != transfermanager::ErrorCode::kNone)
            {
                // failed to read from response.

                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_ERROR,
                                "VsdpSource::GetPayload:Response::Read failed %u",
                                 static_cast<unsigned>(tsm_ec));

                rc = fscfg_kRcError;
            }
            else
            {
                // process the status code.
                transfermanager::Response::StatusCode status_code;
                tsm_ec = response->GetStatusCode(status_code);

                switch (status_code)
                {
                    case transfermanager::Response::StatusCode::kOk:
                    {
                        // The fetch caused a cache update. Inform caller that this is a fresh payload
                        // in comparison to previous cache entry. Listeners interested in given resource should be updated.
                        updated = true;
                        break;
                    }

                    case transfermanager::Response::StatusCode::kStaleCache:
                    {
                        // TransferManager was unable to validate the cache entry against the backend.
                        // This means that we're getting a cache entry that might not contain the latest payload.
                        // This might also mean that we have some communcation issues with the backend.
                        DLT_LOG_STRINGF(dlt_fscfgd,
                                        DLT_LOG_WARN,
                                        "VsdpSource::Fetch: stale cache for resource name %s at uri %s",
                                        resource_name.c_str(),
                                        resource_uri.c_str());

                        updated = false;
                        break;
                    }

                    case transfermanager::Response::StatusCode::kNotModified:
                    {
                        // Entry is from cache and up-to-date.
                        DLT_LOG_STRINGF(dlt_fscfgd,
                                        DLT_LOG_WARN,
                                        "VsdpSource::Fetch: stale cache for resource name %s at uri %s",
                                        resource_name.c_str(),
                                        resource_uri.c_str());

                        updated = false;
                        break;
                    }

                    default:
                    {
                        // Suprisingly, we were unable to retrieve the payload. We'll just log the status code.
                        DLT_LOG_STRINGF(dlt_fscfgd,
                                        DLT_LOG_ERROR,
                                        "VsdpSource::Fetch:FAILED! status_code %u for resource name %s at %s",
                                        static_cast<unsigned>(status_code),
                                        resource_name.c_str(),
                                        resource_uri.c_str());

                        rc = fscfg_kRcError;
                        break;
                    }
                }
            }

            if (rc == fscfg_kRcSuccess)
            {
                // Transform the payload stream into a vector of bytes.
                uint8_t payload_byte;

                while (payload_stream >> std::noskipws >> payload_byte)
                {
                    payload.push_back(payload_byte);
                }
            }
        }
    }

    return rc;
}

fscfg_ReturnCode VsdpSource::GetContentType(const std::string& resource_name, std::string& content_type)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    static std::map<std::string, std::string> resource_to_content = {
        std::make_pair(fscfg_kEntryPointResource, "application/volvo.cloud.EntryPoint"),
        std::make_pair(fscfg_kFeaturesResource, "application/volvo.cloud.Features"),
        std::make_pair(fscfg_kBasicCarControlResource, "application/volvo.cloud.BasicCarControl"),
        std::make_pair(fscfg_kAssistanceCallResource, "application/volvo.cloud.AssistanceCall"),
        std::make_pair(fscfg_kAssistanceCallCallCenterSettingsResource, "application/volvo.cloud.AssistanceCall.CallCenterSettings"),
        std::make_pair(fscfg_kCarAccessResource, "application/volvo.cloud.caraccess"),
        std::make_pair(fscfg_kExternalDiagnosticsResource, "application/volvo.cloud.ExternalDiagnostics"),
        std::make_pair(fscfg_kExternalDiagnosticsRemoteSessionResource, "application/volvo.cloud.RemoteSession"),
        std::make_pair(fscfg_kExternalDiagnosticsClientCapabilitiesResource, "application/volvo.cloud.ClientCapabilities")
    };

    std::map<std::string, std::string>::iterator it = resource_to_content.find(resource_name);

    if (it == resource_to_content.end())
    {
        rc = fscfg_kRcNotFound;
    }
    else
    {
        content_type.append(it->second);
    }

    return rc;
}

fscfg_ReturnCode VsdpSource::GetParameterList(const std::string& resource_name,
                                              std::vector<HttpParameter>& http_parameters)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    static std::vector<HttpParameter> car_access_parameters = {
        std::make_pair("schema_version", "1"),
        std::make_pair("preliminary_version", "8"),
        std::make_pair("signal_flow_versions", "1"),
        std::make_pair("protocol_capabilities", "1")
    };

    static std::vector<HttpParameter> basic_car_control_parameters = {
        std::make_pair("schema_version", "1"),
        std::make_pair("preliminary_version", "10"),
        std::make_pair("signal_flow_versions", "1")
    };

    static std::vector<HttpParameter> assistance_call_parameters = {
        std::make_pair("schema_version", "1"),
        std::make_pair("preliminary_version", "12"),
        std::make_pair("signal_flow_versions", "1")
    };

    static std::vector<HttpParameter> assistance_call_call_center_settings_parameters = {
        std::make_pair("schema_version", "1"),
        std::make_pair("preliminary_version", "12"),
        std::make_pair("signal_flow_versions", "1")
    };

    static std::map<std::string, std::vector<HttpParameter>> resource_parameter_mapping = {
        std::make_pair(fscfg_kEntryPointResource, std::vector<HttpParameter>()), // TODO:Iulian: We need the correct parameters if resource would ever be encoded in CCM.
        std::make_pair(fscfg_kFeaturesResource, std::vector<HttpParameter>()), // TODO:Iulian: We need the correct parameters if resource would ever be encoded in CCM.
        std::make_pair(fscfg_kCarAccessResource, car_access_parameters),
        std::make_pair(fscfg_kBasicCarControlResource, basic_car_control_parameters),
        std::make_pair(fscfg_kAssistanceCallResource, assistance_call_parameters),
        std::make_pair(fscfg_kAssistanceCallCallCenterSettingsResource, assistance_call_call_center_settings_parameters),
        std::make_pair(fscfg_kExternalDiagnosticsResource, std::vector<HttpParameter>()), // TODO:Iulian: We need the correct parameters if resource would ever be encoded in CCM.
        std::make_pair(fscfg_kExternalDiagnosticsRemoteSessionResource, std::vector<HttpParameter>()), // TODO:Iulian: We need the correct parameters if resource would ever be encoded in CCM.
        std::make_pair(fscfg_kExternalDiagnosticsClientCapabilitiesResource, std::vector<HttpParameter>()) // TODO:Iulian: We need the correct parameters if resource would ever be encoded in CCM.
    };

    std::map<std::string, std::vector<HttpParameter>>::iterator it = resource_parameter_mapping.find(resource_name);

    if (it == resource_parameter_mapping.end())
    {
        rc = fscfg_kRcNotFound;
    }
    else
    {
        const std::vector<HttpParameter>& stored_parameters = it->second;
        http_parameters.insert(http_parameters.end(), stored_parameters.begin(), stored_parameters.end());
    }

    return rc;
}

fscfg_ReturnCode VsdpSource::ComputeUriWithParameters(const std::string& resource_name,
                                                      const std::string& uri,
                                                      std::string& computed_uri)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    std::vector<HttpParameter> http_parameters;

    // Retrieve the additional HTTP parameters for the given resource.
    // Intention is to adjust the given resource uri to describe the HTTP parameter list as well.
    rc = GetParameterList(resource_name, http_parameters);

    if (rc != fscfg_kRcSuccess)
    {
        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_ERROR,
                        "VsdpSource::ComputeUriWithParameters: failed to retrieve parameters for resource %s",
                        resource_name.c_str());
    }
    else
    {
        computed_uri.append(uri);

        // If we have additional HTTP parameters for the given resource, we're going to create a parameter stream
        // that holds the parameter part of the request uri.
        // After the above is computed, append it to the original given uri.

        if (http_parameters.size() > 0)
        {
            std::stringstream parameter_stream;

            std::vector<HttpParameter>::iterator it = http_parameters.begin();

            for(; it != http_parameters.end(); ++it)
            {
                std::string parameter_separator = it == http_parameters.begin() ? "?" : "&";

                parameter_stream << parameter_separator << it->first << "=" << it->second;
            }

            computed_uri.append(parameter_stream.str());
        }
    }

    return rc;
}

}

/** \}    end of addtogroup */
