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
 *  \file     discovery_provider.cpp
 *  \brief    Foundation Services Service Discovery Provider implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/discovery_provider.h>

#include <algorithm>
#include <chrono>
#include <thread>

#include <dlt/dlt.h>
#include <xml.hpp>
#include <fsm_persist_data_mgr.h>

#include <fscfg/bus_names.h>

#include <fscfgd/fscfgd.h>
#include <fscfgd/events.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd)

namespace fsm
{

const std::array<std::string, static_cast<std::size_t>(DiscoveryProvider::State::kMax)>
    DiscoveryProvider::kStateNames = {
    "Undefined",
    "Initial",
    "EntryPoint",
    "FeatureList",
    "Ready"
};

void DelayedRetryStateHandler(bool fetch_feature_leaves, DiscoveryProvider::State state, std::uint32_t sleep_seconds)
{
    Daemon* daemon_instance = GetDaemon();

    if (!daemon_instance)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "DelayedRetryStateHandler: Failed to retrieve daemon instance");
    }
    else
    {
        // Will post state-machine execution event after 'sleep_seconds' elapsed.
        DiscoveryEvent discovery_event(DiscoveryEventType::kExecuteTransition,
                                       state,
                                       fetch_feature_leaves);

        DaemonEventQueue& event_queue = daemon_instance->GetQueue();

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "DelayedRetryStateHandler: Will retry state %s in %u seconds...",
                        DiscoveryProvider::kStateNames[static_cast<unsigned>(state)].c_str(),
                        sleep_seconds);

        std::this_thread::sleep_for(std::chrono::seconds(sleep_seconds));

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "DelayedRetryStateHandler: Retrying state %s...",
                        DiscoveryProvider::kStateNames[static_cast<unsigned>(state)].c_str());

        event_queue.PostEvent(discovery_event);
    }
}

fscfg_ReturnCode UpdateFeatureWithData(std::shared_ptr<Feature> feature, const FeatureData& feature_data)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    static std::map<std::pair<bool, bool>, FeatureInterface::State> state_map =
    {
        std::make_pair(std::make_pair(false, false), FeatureInterface::State::kDisabledInvisible),
        std::make_pair(std::make_pair(false, true), FeatureInterface::State::kDisabledVisible),
        std::make_pair(std::make_pair(true, false), FeatureInterface::State::kEnabledInvisible),
        std::make_pair(std::make_pair(true, true), FeatureInterface::State::kEnabledVisible)
    };

    std::pair<bool, bool> state_pair(feature_data.enabled, feature_data.visible);

    if (!feature)
    {
        DLT_LOG_STRING(dlt_fscfgd,
                       DLT_LOG_ERROR,
                       "UpdateFeatureWithData: null feature");

        rc = fscfg_kRcBadParam;
    }
    else
    {
        rc = feature->SetDescription(feature_data.description);
        rc = rc == fscfg_kRcSuccess ? feature->SetIcon(feature_data.icon) : rc;
        rc = rc == fscfg_kRcSuccess ? feature->SetState(state_map[state_pair]) : rc;
        rc = rc == fscfg_kRcSuccess ? feature->SetUri(feature_data.uri) : rc;
        rc = rc == fscfg_kRcSuccess ? feature->SetIcon(feature_data.icon) : rc;
        rc = rc == fscfg_kRcSuccess ? feature->SetTags(feature_data.tags) : rc;

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "UpdateFeatureWithData: failed to update feature");
        }
    }

    return rc;
}

DiscoveryProvider::DiscoveryProvider(std::shared_ptr<Discovery> discovery,
                                     std::shared_ptr<VsdpSource> vsdp_source,
                                     std::shared_ptr<LocalConfig> local_config,
                                     std::shared_ptr<transfermanager::TransferManager> transfer_manager)
 : ResourceBindingHandler(discovery && discovery->GetProvisioning()
                          ? discovery->GetProvisioning()->GetConfig() : nullptr),
   discovery_(discovery),
   vsdp_source_(vsdp_source),
   local_config_(local_config),
   expiry_notification_(nullptr),
   feature_processor_(vsdp_source, discovery && discovery->GetProvisioning()
                      ? discovery->GetProvisioning()->GetConfig() : nullptr),
   transfer_manager_(transfer_manager),
   state_(State::kInitial),
   entry_point_data_(),
   event_queue_(GetDaemon()->GetQueue()),
   update_bindings_(),
   entry_updated_(false)
{
    event_queue_.RegisterListener(this, EventClass::kDiscovery);
    event_queue_.RegisterListener(this, EventClass::kExpiry);
}

DiscoveryProvider::~DiscoveryProvider()
{
    event_queue_.UnregisterListener(this);
}

fscfg_ReturnCode DiscoveryProvider::TransitionTo(bool& fetch_feature_leaves, State state)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    // based on the current state, we allow transitions only to other known states.
    if (!(state > State::kUndefined && state < State::kMax))
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "DiscoveryProvider::TransitionTo: out-of-bounds state");
        rc = fscfg_kRcBadParam;
    }
    else
    {
        // What our state machine driver basically does is get the state configuration and call the state method.
        StateConfiguration current_state_config;
        StateConfiguration next_state_config;

        rc = GetStateConfiguration(state_, current_state_config);
        rc = rc == fscfg_kRcSuccess ? GetStateConfiguration(state, next_state_config) : rc;

        if (rc == fscfg_kRcSuccess)
        {
            // Grab the state configuration and check if we are allowed to make a transition to the
            // given state in our parameter list.

            StatesVector::const_iterator state_it =
                    std::find(current_state_config.first.begin(), current_state_config.first.end(), state);

            if (state_it == current_state_config.first.end())
            {
                // We are not allowed to transition to the given state from the current state.
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_ERROR,
                                "DiscoveryProvider::TransitionTo: %s -> %s is not allowed!",
                                kStateNames[static_cast<unsigned>(state_)].c_str(),
                                kStateNames[static_cast<unsigned>(state)].c_str());

                rc = fscfg_kRcNotPermitted;
            }
            else
            {
                // Execute the state transition method.
                State next_state = state;

                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_INFO,
                                "DiscoveryProvider::TransitionTo: Executing state %s...",
                                kStateNames[static_cast<unsigned>(state)].c_str());

                StateMethod& state_method = next_state_config.second;
                rc = (this->*state_method)(fetch_feature_leaves, next_state); // Call the method through a pointer.

                if (rc != fscfg_kRcSuccess)
                {
                    // there was some really bad state transition problem.
                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_ERROR,
                                    "DiscoveryProvider::TransitionTo: %s -> %s failed",
                                    kStateNames[static_cast<unsigned>(state_)].c_str(),
                                    kStateNames[static_cast<unsigned>(state)].c_str());
                }
                else
                {
                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_INFO,
                                    "DiscoveryProvider::TransitionTo: %s -> %s, next_state %s",
                                    kStateNames[static_cast<unsigned>(state_)].c_str(),
                                    kStateNames[static_cast<unsigned>(state)].c_str(),
                                    kStateNames[static_cast<unsigned>(next_state)].c_str());

                    // Report the new state through the event system.
                    event_queue_.ExecuteEvent(DiscoveryEvent(DiscoveryEventType::kStateTransition,
                                                             state,
                                                             fetch_feature_leaves));

                    // the current state becomes the given state.
                    state_ = state;

                    // if the same state is reported by the state-method, we should just wait in that
                    // state for external stimulus.
                    if (next_state != state)
                    {
                        rc = TransitionTo(fetch_feature_leaves, next_state);
                    }
                }
            }
        }
    }

    return rc;
}

fscfg_ReturnCode DiscoveryProvider::GetState(State& state)
{
    state = state_;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode DiscoveryProvider::ExecuteEvent(Event event)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!vsdp_source_)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        switch (event.event_class)
        {
            case EventClass::kDiscovery:
            {
                if (event.subsystem_event.discovery_event.type == DiscoveryEventType::kExecuteTransition)
                {
                    rc = TransitionTo(event.subsystem_event.discovery_event.parameter.fetch_feature_leaves,
                                      event.subsystem_event.discovery_event.parameter.transition);
                }

                break;
            }

            case EventClass::kExpiry:
            {
                if (event.subsystem_event.expiry_event.type == ExpiryEventType::kExpired)
                {
                    const std::vector<ExpiryEventNotification>& expired_resources
                            = event.subsystem_event.expiry_event.parameter.expired_events;

                    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "DiscoveryProvider::ExecuteEvent: handling kExpiry...");

                    rc = HandleExpiry(expired_resources);

                    if (rc != fscfg_kRcSuccess)
                    {
                        DLT_LOG_STRING(dlt_fscfgd,
                                       DLT_LOG_INFO,
                                       "DiscoveryProvder::ExecuteEvent: failed to handle kExpiry");
                    }
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }

    return rc;
}

fscfg_ReturnCode DiscoveryProvider::GetStateConfiguration(State state, StateConfiguration& state_config)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    // the configuration of the state-machine is a mapping between the current state,
    // a vector of all possible states to transition into, and a method pointer that allows
    // custom state execution on entry.

    static StatesVector initial_state_transitions = {State::kInitial, State::kEntryPoint};
    static StateConfiguration initial_state = std::make_pair(initial_state_transitions,
                                                             &DiscoveryProvider::OnInitial);

    static StatesVector entry_point_transitions = {State::kInitial, State::kEntryPoint, State::kFeatureList};
    static StateConfiguration entry_point_state = std::make_pair(entry_point_transitions,
                                                                 &DiscoveryProvider::OnEntryPoint);

    static StatesVector feature_list_transitions = {State::kInitial, State::kEntryPoint, State::kReady};
    static StateConfiguration feature_list_state = std::make_pair(feature_list_transitions,
                                                                  &DiscoveryProvider::OnFeatureList);

    static StatesVector ready_transitions = {State::kInitial, State::kEntryPoint, State::kFeatureList};
    static StateConfiguration ready_state = std::make_pair(ready_transitions, &DiscoveryProvider::OnReady);

    // Static state-machine configuration. Each state decribes what are the other possible transitions and what is
    // the handler method for that particular state.
    static std::map<State, StateConfiguration> state_machine = {
            std::make_pair(State::kInitial, initial_state),
            std::make_pair(State::kEntryPoint, entry_point_state),
            std::make_pair(State::kFeatureList, feature_list_state),
            std::make_pair(State::kReady, ready_state)
    };

    if (!(state > State::kUndefined && state < State::kMax))
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "DiscoveryProvider::GetStateConfiguration: out-of-bounds state");
        rc = fscfg_kRcBadParam;
    }
    else
    {
        std::map<State, StateConfiguration>::iterator it = state_machine.find(state);

        if (it == state_machine.end())
        {
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_ERROR,
                            "DiscoveryProvider::GetStateConfiguration: State %s does not have configuration",
                            kStateNames[static_cast<unsigned>(state)].c_str());

            rc = fscfg_kRcError; // No state configuration - new state was introduced, but without a configuration.
        }
        else
        {
            state_config = it->second;
        }
    }

    return rc;
}

fscfg_ReturnCode DiscoveryProvider::MapFeatureToResource(const std::string& feature_name, std::string& resource_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    static std::map<std::string, std::string> feature_to_resource_name =
    {
        std::make_pair("fdbcc", fscfg_kBasicCarControlResource),
        std::make_pair("fdast", fscfg_kAssistanceCallResource),
        std::make_pair("caraccess", fscfg_kCarAccessResource),
        std::make_pair("ExternalDiagnostics", fscfg_kExternalDiagnosticsResource)
    };

    std::map<std::string, std::string>::iterator it = feature_to_resource_name.find(feature_name);

    if (it == feature_to_resource_name.end())
    {
        rc = fscfg_kRcNotFound;
    }
    else
    {
        resource_name = it->second;
    }

    return rc;
}

fscfg_ReturnCode DiscoveryProvider::OnInitial(bool& fetch_feature_leaves, State& next_state)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    // One could argue that we should perform Clean-up at this point, but that doesn't really stand - we will in anyway
    // re-perform service discovery which will detect features that don't exist anymore and will remove them.

    // While the DiscoveryProvider state-machine is active, the parent payload validation mechanisms from VSDP source
    // are disabled in order to minimize re-fetch of data that is not cached.
    vsdp_source_->SetUpdateParentPayloads(false);

    // Trigger the entry point.
    next_state = State::kEntryPoint;

    return rc;
}

fscfg_ReturnCode DiscoveryProvider::OnEntryPoint(bool& fetch_feature_leaves, State& next_state)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!local_config_ || !vsdp_source_ || !discovery_)
    {
        // We depend on instances that were not properly allocated.
        rc = fscfg_kRcBadState;
    }
    else
    {
        std::string entry_point_url;

        rc = local_config_->GetEntryPointUrl(entry_point_url);

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "DiscoveryProvider::OnEntryPoint: Failed to retrieve Entry Point URL");
        }
        else
        {
            // perform VSDP set-up.
            rc = vsdp_source_->AddUriMapping(entry_point_url, fscfg_kEntryPointResource);

            if (rc != fscfg_kRcSuccess)
            {
                DLT_LOG_STRING(dlt_fscfgd,
                               DLT_LOG_ERROR,
                               "DiscoveryProvider::OnEntryPoint: Failed to add EntryPoint URI to VSDP");

                rc = fscfg_kRcError;
            }
            else
            {
                // Retrieve the payload from provisioning.
                std::shared_ptr<Provisioning> provisioning = discovery_->GetProvisioning();

                if (!provisioning)
                {
                    DLT_LOG_STRING(dlt_fscfgd,
                                   DLT_LOG_ERROR,
                                   "DiscoveryProvider::OnEntryPoint: Failed to retrieve provisioning from discovery");

                    rc = fscfg_kRcError;
                }
                else
                {
                    // Retrieve the EntryPoint payload and parse it.
                    std::shared_ptr<ProvisionedResource> entry_point_resource;
                    std::vector<std::uint8_t> entry_point_payload;

                    rc = provisioning->Get(fscfg_kEntryPointResource, entry_point_resource);

                    rc = rc == fscfg_kRcSuccess && entry_point_resource
                              ? entry_point_resource->GetPayload(entry_point_payload)
                              : fscfg_kRcError;

                    if (rc != fscfg_kRcSuccess)
                    {
                        DLT_LOG_STRING(dlt_fscfgd,
                                       DLT_LOG_WARN,
                                       "DiscoveryProvider::OnEntryPoint: Failed to retrieve EntryPoint payload. Retry...");

                        LaunchRetryHandler(fetch_feature_leaves, State::kInitial);
                    }
                    else
                    {
                        BindUpdate(fscfg_kEntryPointResource);
                        bool parse_success = ParseEntryPoint(entry_point_payload, entry_point_data_);

                        if (!parse_success)
                        {
                            DLT_LOG_STRING(dlt_fscfgd,
                                           DLT_LOG_ERROR,
                                           "DiscoveryProvider::OnEntryPoint: Failed parsing entry point data");

                            rc = fscfg_kRcError;
                        }
                        else
                        {
                            // Data was successfully parsed. Setup EntryPoint related data.
                            bool run_unsecure;

                            rc = local_config_->GetRunUnsecure(run_unsecure);

                            if (rc != fscfg_kRcSuccess)
                            {
                                DLT_LOG_STRING(dlt_fscfgd,
                                                DLT_LOG_ERROR,
                                                "DiscoveryProvider::OnEntryPoint: Failed retrieving SSL tunnel "
                                                "configuration from LocalConfig");
                            }
                            else
                            {
                                // Process the EntryPoint fields.
                                bool setup_success = true;
                                std::string default_host = "http";
                                std::string protocol_hint = "";
                                fsmpersistdatamgr::FsmPersistDataMgr persistence_api;

                                protocol_hint = !run_unsecure ? "s" : protocol_hint;

                                default_host.append(protocol_hint);
                                default_host.append("://");
                                default_host.append(entry_point_data_.host);
                                default_host.append(":");
                                default_host.append(entry_point_data_.port);

                                // Setup TransferManager default host and
                                // MQTT broker uri.
                                DLT_LOG_STRINGF(dlt_fscfgd,
                                                DLT_LOG_INFO,
                                                "Saving default host: %s",
                                                default_host.c_str());

                                transfermanager::ErrorCode ec =
                                        transfermanager::TransferManager::SetDefaultHost(default_host);

                                setup_success = ec != transfermanager::ErrorCode::kNone ? false : setup_success;

                                setup_success = setup_success ? persistence_api.FsmPersistDataInit() : setup_success;

                                setup_success = setup_success ?
                                    persistence_api.FsmPersistDataSet(kDefaultSignalServiceURIKeyname,
                                                                      entry_point_data_.signal_service_uri)
                                    : setup_success;

                                setup_success = setup_success ? vsdp_source_->AddUriMapping(entry_point_data_.client_uri,
                                                                                            fscfg_kFeaturesResource) == fscfg_kRcSuccess
                                                              : setup_success;

                                if (!setup_success)
                                {
                                    DLT_LOG_STRING(dlt_fscfgd,
                                                   DLT_LOG_ERROR,
                                                   "DiscoveryProvider::OnEntryPoint:Failed to process EntryPoint");

                                            rc = fscfg_kRcError;
                                }
                                else
                                {

                                    // What happens if there was an entry_updated_ ?
                                    // Well, the only reason why entry_updated_ occured was because we fetched EntryPoint.. but the payload
                                    // that was returned is actually the latest one, so we don't need to re-trigger the EntryPoint state.
                                    entry_updated_ = false; // Update not needed anymore.
                                    next_state = State::kFeatureList;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return rc;
}

fscfg_ReturnCode DiscoveryProvider::OnFeatureList(bool& fetch_feature_leaves, State& next_state)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    std::shared_ptr<Provisioning> provisioning = discovery_ ? discovery_->GetProvisioning() : nullptr;

    if (!discovery_ || ! provisioning || ! vsdp_source_)
    {
        // We depend on instances that were not properly allocated.
        rc = fscfg_kRcBadState;
    }
    else
    {
        // Retrieve the feature list and parse the payload.
        std::vector<std::uint8_t> features_payload;
        std::shared_ptr<ProvisionedResource> features_resource;

        rc = provisioning->Get(fscfg_kFeaturesResource, features_resource);
        rc = rc == fscfg_kRcSuccess && features_resource ? features_resource->GetPayload(features_payload) : rc;

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "DiscoveryProvider::OnFeatureList: Failed to retrieve Features payload. Retry...");

            // We just lost the connection during the discovery process. What we really want to do is to re-trigger
            // the full process, not just the FeatureList step.
            LaunchRetryHandler(fetch_feature_leaves, State::kInitial);
        }
        else
        {
            std::vector<FeatureData> features_data;

            BindUpdate(fscfg_kFeaturesResource);

            bool parse_success = ParseFeatures(features_payload, features_data);

            if (!parse_success)
            {
                DLT_LOG_STRING(dlt_fscfgd,
                               DLT_LOG_ERROR,
                               "DiscoveryProvider::OnFeatureList: Failed to parse Features");

                rc = fscfg_kRcError;
            }
            else
            {
                // Process each feature data and update all feature resources.
                // The feature name is not the same as the resource name, as such we need to map to the right resource
                // name. If there is no mapping avaiable we will use the feature name as the actual resource name.
                std::set<std::string> fresh_resource_names; // resources discovered during current run.
                std::set<std::string> previous_resource_names; // resources discovered during previous run.
                std::vector<std::shared_ptr<Feature>> previous_features;

                discovery_->GetAll(previous_features);

                for (auto feature : previous_features)
                {
                    std::string resource_name;

                    feature->GetName(resource_name);
                    previous_resource_names.insert(resource_name);
                }

                for (auto feature_data : features_data)
                {
                    std::string resource_name = feature_data.name;

                    if (MapFeatureToResource(feature_data.name, resource_name) == fscfg_kRcNotFound)
                    {
                        // Special handling for "expiry". Expiry is not a feature as all the other "features" so it
                        // needs a little-bit of added care when dealt with. ExpiryNotification is not on the bus
                        // because we don't have a resource name for it - as such, we instantiate it, but never publish it.
                        if (feature_data.name == kExpiryFeatureName)
                        {
                            UpdateExpiryNotification(feature_data);
                        }
                        else
                        {
                            // If we can't find the resource name for a feature name in the Features list,
                            // we do not publish details on that particular feature - we do not have proper information
                            // on how to request the payload for it. The idea is that we just ignore it and provide information
                            // in the logs that we did so.
                            DLT_LOG_STRINGF(dlt_fscfgd,
                                            DLT_LOG_WARN,
                                            "DiscoveryProvider::OnFeatureList: Ignoring feature %s...",
                                            feature_data.name.c_str());
                        }
                    }
                    else
                    {
                        std::shared_ptr<Feature> feature = std::make_shared<Feature>(resource_name, discovery_);

                        if (!feature)
                        {
                            DLT_LOG_STRING(dlt_fscfgd,
                                            DLT_LOG_ERROR,
                                            "DiscoveryProvider::OnFeatureList: Failed to allocate feature");

                            rc = fscfg_kRcError;
                            break;
                        }
                        else
                        {
                            fresh_resource_names.insert(resource_name);

                            rc = UpdateFeatureWithData(feature, feature_data);
                            rc = rc == fscfg_kRcSuccess ? discovery_->Update(feature, true) : rc;
                            rc = rc == fscfg_kRcSuccess ? vsdp_source_->AddUriMapping(feature_data.uri, resource_name) : rc;

                            if (rc != fscfg_kRcSuccess)
                            {
                                DLT_LOG_STRINGF(dlt_fscfgd,
                                               DLT_LOG_ERROR,
                                               "DiscoveryProvider::OnFeatureList: failed to update feature %s",
                                                resource_name.c_str());

                                break;
                            }

                            // Track the feature, if trackable and we're not tracking it already.
                            TrackFeature(feature);

                            // Fetch the payload at feature.uri just to perform an update in VSDP.
                            if (fetch_feature_leaves && !IsFeatureTrackable(feature))
                            {
                                std::vector<std::uint8_t> payload;
                                std::shared_ptr<ProvisionedResource> provisioned_feature;

                                provisioning->Get(resource_name, provisioned_feature);

                                if (provisioned_feature)
                                {
                                    provisioned_feature->GetPayload(payload);
                                }
                            }

                        }
                    }
                }


                // We've updated all feature resources. There's still the chance that a feature does not exist in the
                // feature list anymore - those features need to be removed!
                std::vector<std::string> deleted_resource_names;

                std::set_difference(previous_resource_names.begin(),
                                    previous_resource_names.end(),
                                    fresh_resource_names.begin(),
                                    fresh_resource_names.end(),
                                    std::inserter(deleted_resource_names, deleted_resource_names.end()));

                for (const auto& deleted_resource_name : deleted_resource_names)
                {
                    std::shared_ptr<Feature> feature;

                    discovery_->Get(deleted_resource_name, feature);
                    UntrackFeature(feature);

                    // Untrack the feature and remove it from the container.
                    discovery_->RemoveFeature(deleted_resource_name);
                }

                // remove URI mapping for the deleted features if the source was VSDP; Otherwise VSDP source will still
                // say that it still has it, which isn't the case if this is not the first time service discovery is performed.
                std::string vsdp_source_name;
                std::string features_source_name;
                std::shared_ptr<SourceInterface> features_source;
                vsdp_source_->GetName(vsdp_source_name);

                if (!features_resource)
                {
                    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "DiscoveryProvider::OnFeatureList: null features_resource");
                    rc = fscfg_kRcError;
                }
                else
                {
                    features_resource->GetSource(features_source);

                    if (features_source_name == vsdp_source_name)
                    {
                        for (const auto& deleted_resource_name : deleted_resource_names)
                        {
                            vsdp_source_->RemoveUriMapping(deleted_resource_name);
                        }
                    }
                }

                next_state = State::kReady;
                // there is the possibility that we already did fetch the feature leaves,
                // as such, we set this to false.
                fetch_feature_leaves = false;
            }
        }
    }

    return rc;
}

fscfg_ReturnCode DiscoveryProvider::OnReady(bool& fetch_feature_leaves, State& next_state)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    // Make all resources public.
    discovery_->Update();

    // Enable the parent payload update mechanism once service discovery is done; When clients request a resource,
    // VSDP will know to check (with the help of HTTP Cache) the parent payloads for validity and re-fetch them if necessary,
    // all prior to retrieving the mentioned resource.
    vsdp_source_->SetUpdateParentPayloads(true);

    // Service is ready, we remain in this state until we encounter an external stimulus.
    next_state = State::kReady;

    DLT_LOG_STRING(dlt_fscfgd,
                   DLT_LOG_INFO,
                   "DiscoveryProvider::OnReady: Service Discovery Done!");

    return rc;
}


fscfg_ReturnCode DiscoveryProvider::OnBoundResourceUpdated(ResourceInterface::UpdatedEvent updated_event)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!updated_event.resource)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "DiscoveryProvider::OnBoundResourceUpdated: null resource");
    }
    else
    {
        std::string resource_name;

        updated_event.resource->GetName(resource_name);

        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "DiscoveryProvider: resource %s updated", resource_name.c_str());

        // The payload for either EntryPoint or Features has changed.

        // While service discovery process is running, we can't call execute as that'll cause recursive behaviour.
        // We'll just mark that there was an entry update while performing the service, we won't publish
        // the resources and re-trigger the discovery process.
        if (state_ != State::kReady)
        {
            // this is self-triggered by the discovery state-machine.
            if (resource_name == fscfg_kEntryPointResource)
            {
                entry_updated_ = true;
            }
        }
        else
        {
            DiscoveryProvider::State state_to_transition_to = DiscoveryProvider::State::kEntryPoint;

            if (resource_name == fscfg_kFeaturesResource)
            {
                // No point in triggering EntryPoint if we're just reconnecting Features resource.
                // If any updates on entry during kFeatureList, we'll know about it with entry_updated_ flag.
                state_to_transition_to = DiscoveryProvider::State::kFeatureList;
            }

            event_queue_.ExecuteEvent(DiscoveryEvent(DiscoveryEventType::kExecuteTransition,
                                                     state_to_transition_to,
                                                     false));
        }
    }

    return rc;
}

void DiscoveryProvider::UpdateExpiryNotification(const FeatureData& feature_data)
{
    expiry_notification_feature_ = !expiry_notification_feature_ ?
                                   std::make_shared<Feature>(kExpiryFeatureName, discovery_)
                                   : expiry_notification_feature_;

    if (expiry_notification_feature_)
    {
        // Update the data at the feature level.
        UpdateFeatureWithData(expiry_notification_feature_, feature_data);

        // Dispatch the changed to all listeners. (In this situation, the ExpiryNotification handler itself).
        expiry_notification_feature_->Update();
    }

    expiry_notification_ = !expiry_notification_ ?
                           std::make_shared<ExpiryNotification>(expiry_notification_feature_)
                           : expiry_notification_;

    if (!expiry_notification_ || !expiry_notification_)
    {
        DLT_LOG_STRING(dlt_fscfgd,
                       DLT_LOG_ERROR,
                       "DiscoveryProvider::UpdateExpiryNotification: Failed to instantiate ExpiryNotification");
    }
}

fscfg_ReturnCode DiscoveryProvider::HandleExpiry(const std::vector<ExpiryEventNotification>& expired_events)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    // no matter resource and the eviction type (immediate true or false) we will invalidate the cache.
    // If immediate is true for a resource, we shall re-fetch the payload in a blocking fashion, while
    // executing the event.
    std::vector<std::shared_ptr<Resource>> resources_to_refetch;
    std::shared_ptr<Provisioning> provisioning = discovery_->GetProvisioning();
    std::shared_ptr<Config> config = provisioning ? provisioning->GetConfig() : nullptr;

    if (!config)
    {
        DLT_LOG_STRING(dlt_fscfgd,
                       DLT_LOG_ERROR,
                       "DiscoveryProvider::HandleExpiry: Failed to access Config");

        rc = fscfg_kRcBadState;
    }
    else
    {
        for (auto& expired_resource : expired_events)
        {
            const std::string& resource_name = expired_resource.resource_name;
            std::string resource_uri;
            fscfg_ReturnCode lookup_rc = vsdp_source_->GetUriForResource(resource_name, resource_uri);

            if (lookup_rc != fscfg_kRcSuccess)
            {
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_ERROR,
                                "DiscoveryProvider::HandleExpiry: no uri for %s",
                                resource_name.c_str());
            }
            else
            {
                // invalidate cache no matter what.
                transfermanager::ErrorCode ec = transfer_manager_->SetValidCacheFlag(resource_uri, false);

                if (ec != transfermanager::ErrorCode::kNone)
                {
                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_WARN,
                                    "DiscoveryProvider::HandleExpiry: Failed to set cache flag for uri %s",
                                    resource_uri.c_str());
                }

                std::shared_ptr<Resource> resource;
                fscfg_ReturnCode get_resource_rc = config->Get(expired_resource.resource_name, resource);

                if (get_resource_rc != fscfg_kRcSuccess)
                {
                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_WARN,
                                    "DiscoveryProvider::HandleExpiry: Failed to retrieve resource %s",
                                    expired_resource.resource_name.c_str());
                }
                else
                {
                    // dispatch the notification to individual resoures.
                    fscfg_ReturnCode process_expiry_rc = resource->ProcessExpiry(expired_resource.immediate);

                    if (process_expiry_rc != fscfg_kRcSuccess)
                    {
                        DLT_LOG_STRINGF(dlt_fscfgd,
                                        DLT_LOG_WARN,
                                        "DiscoveryProvider::HandleExpiry: Failed to process expiry for resource %s",
                                        expired_resource.resource_name.c_str());
                    }

                    if (expired_resource.immediate)
                    {
                        // Update payloads only after all have been notified.
                        resources_to_refetch.push_back(resource);
                    }
                }
            }
        }

        // Refetch all resources marked with immediate true.
        for (const auto& resource : resources_to_refetch)
        {
            std::vector<std::uint8_t> payload;
            std::vector<std::shared_ptr<EncodingInterface>> encodings;
            std::string resource_name;

            resource->GetName(resource_name);

            fscfg_ReturnCode get_encodings_rc = vsdp_source_->GetEncodings(resource, encodings);

            if (get_encodings_rc != fscfg_kRcSuccess)
            {
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_WARN,
                                "DiscoveryProvider::HandleExpiry: Failed to retreive encodings for resource %s",
                                resource_name.c_str());
            }
            else
            {
                // call GetPayload for all available encodings.
                for (auto encoding : encodings)
                {
                    fscfg_ReturnCode payload_rc = vsdp_source_->GetPayload(resource, encoding, payload);

                    // If any update, this will re-trigger service discovery.
                    if (payload_rc != fscfg_kRcSuccess)
                    {
                        DLT_LOG_STRINGF(dlt_fscfgd,
                                        DLT_LOG_ERROR,
                                        "DiscoveryProvider::HandleExpiry: Failed to retrieve payload for %s",
                                        resource_name.c_str());
                    }
                }
            }
        }
    }

    return rc;
}

void DiscoveryProvider::LaunchRetryHandler(bool fetch_feature_leaves,
                                           DiscoveryProvider::State state,
                                           uint32_t sleep_seconds)
{
    std::thread([fetch_feature_leaves, state, sleep_seconds]()
                {
                    DelayedRetryStateHandler(fetch_feature_leaves, state, sleep_seconds);
                }).detach();
}

bool DiscoveryProvider::IsFeatureTrackable(std::shared_ptr<Feature> feature)
{
    std::vector<std::string> trackable_feature_names;
    std::string feature_name;


    feature_processor_.GetTrackableFeatures(trackable_feature_names);
    feature->GetName(feature_name);

    bool tracked =  (std::find(trackable_feature_names.begin(), trackable_feature_names.end(), feature_name)
                     != trackable_feature_names.end());

    return tracked;
}

void DiscoveryProvider::TrackFeature(std::shared_ptr<Feature> feature)
{
    // Get trackable features and check that given feature is trackable - if so, tell feature processor to track it,
    // otherwise just silently ignore the feature (this is why we have this private variant of the tracking functionality).

    if (!feature)
    {
        DLT_LOG_STRING(dlt_fscfgd,
                       DLT_LOG_ERROR,
                       "DiscoveryProvider::TrackFeature: null feature");
    }
    else
    {

        if (IsFeatureTrackable(feature))
        {
            // As feature is trackable, proceed with the tracking inside feature processor. This will effectively register
            // the tracking functionality, if not already tracked, bind on changes, and then always perform the call to
            // the processing method.

            feature_processor_.TrackFeature(feature);
        }
    }
}

void DiscoveryProvider::UntrackFeature(std::shared_ptr<Feature> feature)
{
    feature_processor_.UntrackFeature(feature);
}

} // namespace fsm

/** \}    end of addtogroup */
