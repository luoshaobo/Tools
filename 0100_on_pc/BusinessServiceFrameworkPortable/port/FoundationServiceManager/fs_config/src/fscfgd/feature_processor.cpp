/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     feature_processor.cpp
 *  \brief    Foundation Services Config additional per-feature processing logic implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/feature_processor.h>

#include <dlt/dlt.h>

#include <fscfg/bus_names.h>

#include <fscfgd/parsing_utilities.h>


#include <voc_framework/signals/assistance_call.h>
#include <voc_framework/signals/signal_factory.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd)

namespace  fsm
{

FeatureProcessor::FeatureProcessor(std::shared_ptr<VsdpSource> vsdp_source, std::shared_ptr<Config> config)
    : ResourceBindingHandler(config),
      tracked_features_(),
      feature_process_method_(),
      vsdp_source_(vsdp_source)
{
    PerformProcessingMethodMapping();
    //TODO: register factory for assistance call
}

void FeatureProcessor::PerformProcessingMethodMapping()
{
    // Assign each of the known features to a processing method.
    feature_process_method_[fscfg_kExternalDiagnosticsResource] = &FeatureProcessor::ProcessExternalDiagnostics;
    feature_process_method_[fscfg_kAssistanceCallResource] = &FeatureProcessor::ProcessAssistanceCall;

}

void FeatureProcessor::CallProcessor(const std::string& feature_name, std::vector<std::uint8_t>& payload)
{
     std::map<std::string, ProcessingMethod>::iterator it = feature_process_method_.find(feature_name);

     if (it == feature_process_method_.end())
     {
         DLT_LOG_STRINGF(dlt_fscfgd,
                         DLT_LOG_ERROR,
                         "FeatureProcessor::CallProcessor: No processing method for %s",
                         feature_name.c_str());
     }
     else
     {
         // So, now we know for sure that a processing method exists; retrieve it from the iterator and just call it.
         ProcessingMethod& processing_method = it->second;

         DLT_LOG_STRINGF(dlt_fscfgd,
                         DLT_LOG_INFO,
                         "FeatureProcessor::CallProcessor: Processing feature %s...",
                         feature_name.c_str());

         fscfg_ReturnCode rc = (this->*processing_method)(tracked_features_[feature_name], payload);

         if (rc != fscfg_kRcSuccess)
         {
             DLT_LOG_STRINGF(dlt_fscfgd,
                             DLT_LOG_ERROR,
                             "FeatureProcessor::CallProcessor: Processing method for %s has failed! rc=%u",
                             feature_name.c_str(),
                             static_cast<unsigned>(fscfg_kRcSuccess));
         }
     }
}

fscfg_ReturnCode FeatureProcessor::TrackFeature(std::shared_ptr<Feature> feature)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "FeatureProcessor::TrackFeature: null feature");

        rc = fscfg_kRcBadParam;
    }
    else
    {
        // Check if the feature is trackable by checking if there's a feature process method associated.
        std::string feature_name;

        feature->GetName(feature_name);

        if (!feature_process_method_.count(feature_name))
        {
            // Feature is not trackable as there is no process method available.
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_ERROR,
                            "FeatureProcessor::TrackFeature: Feature %s is not trackable",
                            feature_name.c_str());

            rc = fscfg_kRcNotPermitted;
        }
        else
        {
            // Feature is not previously tracked; We're going to add in the tracked feature set, bind for change
            // notifications, as well as calling the processing method in order to update the provisioning container.

            if (tracked_features_.count(feature_name) == 0)
            {
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_INFO,
                                "FeatureProcessor::TrackFeature: Will track feature %s...",
                                feature_name.c_str());

                // add to tracked feature set.
                tracked_features_[feature_name] = feature;
            }

            // Call the processing method.
            std::vector<std::uint8_t> no_payload; //payload will be fetched

            CallProcessor(feature_name, no_payload);

            // ResourceBindingHandler will print if any errors occurred. If it's already bound, we don't really need
            // to do anything, nothing will be printed.
            BindPayloadChange(feature_name);
        }
    }

    return rc;
}

fscfg_ReturnCode FeatureProcessor:: OnBoundResourcePayloadChanged(ResourceInterface::PayloadsChangedEvent payloads_changed_event)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    // Certainly, one of the tracked features has had it's payload updated (We're only bound on the ones being tracked).
    // Call the processing method for given resource so that it will update it's child resources.

    std::shared_ptr<ResourceInterface> tracked_resource = payloads_changed_event.resource;

    if (!tracked_resource)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "FeatureProcessor::OnBoundResourceUpdated: null resource");

        rc = fscfg_kRcBadParam;
    }
    else
    {
        // Great! Just call the processing method to deduce additional child resources.
        std::string resource_name;

        tracked_resource->GetName(resource_name);
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "FeatureProcessor::OnBoundResourceUpdated: CallProcessor called");

        CallProcessor(resource_name, payloads_changed_event.payload);
    }

    return rc;
}

fscfg_ReturnCode FeatureProcessor::UntrackFeature(std::shared_ptr<Feature> feature)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "FeatureProcessor::UntrackFeature: null feature");

        rc = fscfg_kRcBadParam;
    }
    else
    {
        // Check if feature is actually being tracked.

        std::string feature_name;

        feature->GetName(feature_name);

        if (!tracked_features_.count(feature_name))
        {
            // Feature is not tracked.
            rc = fscfg_kRcBadResource;
        }
        else
        {
            UnbindUpdate(feature_name);
            tracked_features_.erase(feature_name);
        }
    }

    return rc;
}

fscfg_ReturnCode FeatureProcessor::GetTrackableFeatures(std::vector<std::string>& trackable_features)
{
    for (auto name_feature_pair : feature_process_method_)
    {
        trackable_features.push_back(name_feature_pair.first);
    }

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode FeatureProcessor::ProcessExternalDiagnostics(std::shared_ptr<Feature> feature, std::vector<std::uint8_t>& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature)
    {
        DLT_LOG_STRING(dlt_fscfgd,
                       DLT_LOG_ERROR,
                       "FeatureProcessor::"
                       ": null feature");

        rc = fscfg_kRcBadParam;
    }
    else if (!vsdp_source_)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "FeatureProcessor::ProcessExternalDiagnostics: null vsdp_source_");
    }
    else
    {
        // process the ExternalDiagnostics payload in order to find where the RemoteSession is stored, if any.
        std::vector<std::uint8_t> feature_payload;
        std::string feature_name;


        // Potentially, the child resources as they might already exist from an older parsing
        // Attempt to remove them.
        vsdp_source_->RemoveUriMapping(fscfg_kExternalDiagnosticsRemoteSessionResource);
        vsdp_source_->RemoveUriMapping(fscfg_kExternalDiagnosticsClientCapabilitiesResource);

        feature->GetName(feature_name);

        if ( payload.size() == 0 )
        {
            rc = feature->ProvisionedResource::GetPayload(feature_payload);
        }
        else
        {
            feature_payload = payload;
        }

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd,
                            DLT_LOG_ERROR,
                            "FeatureProcessor::ProcessExternalDiagnostics: failed to retrieve payload");
        }
        else
        {
            ExternalDiagnosticsData external_diagnostics_data;

            bool parse_success = ParseExternalDiagnostics(feature_payload, external_diagnostics_data);

            if (!parse_success)
            {
                DLT_LOG_STRING(dlt_fscfgd,
                               DLT_LOG_ERROR,
                               "FeatureProcessor::ProcessExternalDiagnostics: failed to parse payload");

                rc = fscfg_kRcError;
            }
            else
            {
                // Add URI mapping for the resources that we are interested in.
                // This operation will also populate the resource in the provisioning container.
                // This effectively creates the resource objects inside VSDP,
                // even if we didn't yet retrieve the payload for it (nor will we, let client interested in resource
                // request it).
                // Below data structure maps a pair containing uri name and uri existence flag to a resource name.
                std::map<std::pair<std::string, bool>, std::string> uri_resource_pairs = {
                    std::make_pair(std::make_pair(external_diagnostics_data.remote_session_uri,
                                                  external_diagnostics_data.remote_session_exists),
                                                  fscfg_kExternalDiagnosticsRemoteSessionResource),
                    std::make_pair(std::make_pair(external_diagnostics_data.client_capabilities_uri, true), // this field is not optional, so it always exists.
                                                  fscfg_kExternalDiagnosticsClientCapabilitiesResource)
                };

                // Uri-map on all resources - effectively adds the resources in vsdp source; The bus will be updated
                // once service discovery will be finished.
                for (std::pair<std::pair<std::string, bool>, std::string> uri_resource_pair : uri_resource_pairs)
                {

                    if (uri_resource_pair.first.second)
                    {
                        DLT_LOG_STRINGF(dlt_fscfgd,
                                        DLT_LOG_INFO,
                                        "FeatureProcessor::ProcessExternalDiagnostics:Map %s to resource \'%s\'",
                                        uri_resource_pair.first.first.c_str(), uri_resource_pair.second.c_str());

                        fscfg_ReturnCode map_rc = vsdp_source_->AddUriMapping(uri_resource_pair.first.first,
                                                                              uri_resource_pair.second);

                        if (map_rc != fscfg_kRcSuccess)
                        {
                            // Log, but still attempt to continue; we will update the rc and keep it so in order
                            // to propagate the issue to caller.
                            DLT_LOG_STRINGF(dlt_fscfgd,
                                            DLT_LOG_ERROR,
                                            "FeatureProcessor::ProcessExternalDiagnostics: Failed to add uri mapping for"
                                            "%s to uri %s",
                                            uri_resource_pair.second, uri_resource_pair.first);

                            rc = fscfg_kRcError; // Propagate the error as a generic, internal error.
                        }
                    }
                }
            }
        }
    }

    return rc;
}



fscfg_ReturnCode FeatureProcessor::ProcessAssistanceCall(std::shared_ptr<Feature> feature, std::vector<std::uint8_t>& payload)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!feature)
    {
        DLT_LOG_STRING(dlt_fscfgd,
                       DLT_LOG_ERROR,
                       "FeatureProcessor::ProcessAssistanceCall: null feature");

        rc = fscfg_kRcBadParam;
    }
    else if (!vsdp_source_)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "FeatureProcessor::ProcessAssistanceCall: null vsdp_source_");
    }
    else
    {
        // process the AssistanceCall payload in order to find where the CallCenterSettings are stored, if any.
        std::vector<std::uint8_t> feature_payload;
        std::string feature_name;

        //remove assistancecall call center settings (no matter what, will be added later)
        //no need to check the result, may fail if entry not added yet
        vsdp_source_->RemoveUriMapping(fscfg_kAssistanceCallCallCenterSettingsResource);

        feature->GetName(feature_name);

        if (payload.size() == 0)
        {
            rc = feature->ProvisionedResource::GetPayload(feature_payload);
        }
        else
        {
            feature_payload = payload;
        }

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd,
                            DLT_LOG_ERROR,
                            "FeatureProcessor::ProcessAssistanceCall: failed to retrieve payload");
        }
        else
        {

            //register factory for decoding
            fsm::SignalFactory::RegisterPayloadFactory<fsm::AssistanceCallPayload>(fsm::kAssistanceCallOid);

            //make sure something got decoded
            std::shared_ptr<fsm::Signal> decoded_signal =
                fsm::SignalFactory::DecodeCcm(feature_payload.data(), feature_payload.size());

            //cast the payload to assistance call
            std::shared_ptr<fsm::AssistanceCallPayload> assistance_call =
                fsm::SignalFactory::GetSignalPayload<fsm::AssistanceCallPayload>(decoded_signal);


            //if we managed to decode
            if (!assistance_call)
            {
                DLT_LOG_STRING(dlt_fscfgd,
                               DLT_LOG_ERROR,
                               "FeatureProcessor::ProcessAssistanceCall: failed to decode the payload");

                rc = fscfg_kRcError;
            }
            else
            {
                // Add URI mapping for the resources that we are interested in.
                // call center uri is optional, so we have to check the existance of the field.

                std::string call_center_uri;


                if ((assistance_call->GetCallCenterAddressType() == AssistanceCallPayload::kUri)
                        && assistance_call->GetCallCenterAddress (call_center_uri) )
                {
                    rc = vsdp_source_->AddUriMapping(call_center_uri,
                                                     fscfg_kAssistanceCallCallCenterSettingsResource);

                    if (rc != fscfg_kRcSuccess)
                    {
                        // Failed to add uri mapping
                        DLT_LOG_STRINGF(dlt_fscfgd,
                                        DLT_LOG_ERROR,
                                        "FeatureProcessor::ProcessAssistanceCall: Failed to add uri mapping for"
                                        "%s to uri %s",
                                        fscfg_kAssistanceCallCallCenterSettingsResource, call_center_uri.c_str());

                        rc = fscfg_kRcError; // Propagate the error as a generic, internal error.
                    }
                }
                else
                {
                    DLT_LOG_STRING(dlt_fscfgd,
                                   DLT_LOG_INFO,
                                   "FeatureProcessor::ProcessAssistanceCall: No uri for call center settings");
                }
            }
        }
    }

    return rc;
}


} // namespace fsm

/** \}    end of addtogroup */
