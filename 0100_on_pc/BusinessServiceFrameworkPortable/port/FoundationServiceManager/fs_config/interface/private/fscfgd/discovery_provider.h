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
 *  \file     discovery_provider.h
 *  \brief    Foundation Services Service Discovery Provider interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_DISCOVERY_PROVIDER_H_INC_
#define FSM_DISCOVERY_PROVIDER_H_INC_

#include <memory>

#include <transfer_manager.h>

#include <fscfgd/discovery.h>
#include <fscfgd/feature.h>
#include <fscfgd/parsing_utilities.h>
#include <fscfgd/event_listener_interface.h>
#include <fscfgd/resource_binding_handler.h>
#include <fscfgd/local_config.h>
#include <fscfgd/feature_processor.h>
#include <fscfgd/expiry_notification.h>
#include <fscfgd/vsdpsource.h>

namespace fsm
{

class DaemonEventQueue;
struct ExpiryEventNotification;

/**
 * \brief Handles the Service Discovery process and populates the Foundation Services Config Discovery container.
 */
class DiscoveryProvider : public ResourceBindingHandler, public EventListenerInterface
{
public:
    /**
     * \brief Discovery states.
     */
    enum class State
    {
        kUndefined,                 ///< Uninitialized value.

        kInitial,                   ///< Initial state.
        kEntryPoint,                ///< Service Discovery in progress, processing entry point.
        kFeatureList,               ///< Service Discovery in progress, processing feature list.
        kReady,                     ///< Everything is ok. User services may be access.

        kMax                        ///< Maximum value type should hold. Keep this last.
    };

    static const std::array<std::string,
                            static_cast<std::size_t>(State::kMax)> kStateNames;    ///< State names look-up.

    /**
     * \brief DiscoveryProvider Constructor.
     *
     * \param[in] discovery discovery container reference.
     * \param[in] vsdp_source VSDP Source reference.
     * \param[in] local_config file-based Local Config reference.
     * \param[in] transfer_manager TransferManager reference.
     */
    DiscoveryProvider(std::shared_ptr<Discovery> discovery,
                      std::shared_ptr<VsdpSource> vsdp_source,
                      std::shared_ptr<LocalConfig> local_config,
                      std::shared_ptr<transfermanager::TransferManager> transfer_manager);

    /**
     * \brief DiscoveryProvider Destructor.
     */
    ~DiscoveryProvider();

    /**
     * \brief Perform discovery state-machine transition.
     *
     * \param[in/out] fetch_feature_leaves true if during processing of the feature list the resource leaves should be fetched,
     *            false otherwise.
     * \param[in] state state state-machine is to transition to.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode TransitionTo(bool& fetch_feature_leaves, State state);

    /**
     * \brief Getter for the current discovery state-machine state.
     *
     * \param[out] state current discovery state-machine state.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetState(State& state);

    /**
     * \copydoc fsm::EventListenerInterface::ExecuteEvent
     */
    fscfg_ReturnCode ExecuteEvent(Event event);

private:
    ///! State handler method pointer.
    typedef fscfg_ReturnCode (DiscoveryProvider::*StateMethod)(bool& fetch_feature_leaves, State& next_state);

    ///! Vector of states.
    typedef std::vector<State> StatesVector;

    ///! Association between a vector of possible transitions and a state handler.
    typedef std::pair<StatesVector, StateMethod> StateConfiguration;

    /**
     * \brief Resource payload updated callback.
     *
     * \param[in] updated_event resource payload updated event parameter.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode OnBoundResourceUpdated(ResourceInterface::UpdatedEvent updated_event) override;

    /**
     * \brief Retrieves the state configuration for a given state.
     * Configuration involves a set of possible transitions and a state-handler method.
     *
     * \param[in] state state to retrieve configuration for.
     * \param[in] state_config state configuration.
     *
     * \return fscfg_kRcBadParam if state is out of bounds,\n
     *         fscfg_kRcError if state doesn't have method handler,\n
     *         fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetStateConfiguration(State state, StateConfiguration& state_config);

    /**
     * \brief Retrieves the resource name for a given feature name.
     *
     * \param[in] feature_name name of the feature to retrieve the resource name for.
     * \param[out] resource_name resorce name for given feature.
     *
     * \return fscfg_kRcNotFound if there is no mapping for given feature,\n
     *         fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode MapFeatureToResource(const std::string& feature_name, std::string& resource_name);

    /**
     * \brief kInitial State handler method.
     * Performs state-machine initialization and immediately instructs a transition to kEntryPoint.
     *
     * \param[in/out] fetch_feature_leaves true if during processing of the feature list the resource leaves should be fetched,
     *            false otherwise.
     * \param[out] next_state next state to transition into. Same state if transitions should stop and wait for external
     * stimulus.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode OnInitial(bool& fetch_feature_leaves, State& next_state);

    /**
     * \brief kEntryPoint State handler method.
     * Retrives the EntryPoint URL from local config, fetches and parses the data at URL, initializes default host
     * and MQTT URI and sets up the FeatureList URIs.
     *
     * \param[in/out] fetch_feature_leaves true if during processing of the feature list the resource leaves should be fetched,
     *            false otherwise.
     * \param[out] next_state next state to transition into. Same state if transitions should stop and wait for external
     * stimulus.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode OnEntryPoint(bool& fetch_feature_leaves, State& next_state);

    /**
     * \brief kFeatureList State handler method.
     * Fetches the data at FeatureList URI and parses each feature. After parsing it updates the current set against
     * the previously discovered feature set. Additionally, if a feature is deduced as being trackable by the feature processor,
     * additional processing is performed in the context of the feature precessor. The main concern the feature processor handles
     * is to deduce if there are any other child resources referenced by the feature in it's payload.
     *
     * \param[in/out] fetch_feature_leaves true if during processing of the feature list the resource leaves should be fetched,
     *            false otherwise.
     * \param[out] next_state next state to transition into. Same state if transitions should stop and wait for external
     * stimulus.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode OnFeatureList(bool& fetch_feature_leaves, State& next_state);

    /**
     * \brief kReady State handler method.
     * Publishes the state of the discovered resources to Foundation Services Config clients.
     *
     * \param[in/out] fetch_feature_leaves true if during processing of the feature list the resource leaves should be fetched,
     *            false otherwise.
     * \param[out] next_state next state to transition into. Same state if transitions should stop and wait for external
     * stimulus.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode OnReady(bool& fetch_feature_leaves, State& next_state);

    /**
     * \brief Updates the expiry notification feature as well as the handler implementation.
     * If the handler or feature is not yet instantiated, they will be instatiated, else they will only be updated.
     *
     * \param[in] feature_data FeatureList parsing data.
     */
    void UpdateExpiryNotification(const FeatureData& feature_data);

    /**
     * \brief Handles the expiry notification event.
     * Handling in a two-step manner; all resources are initially cache-invalidated followed by a refetch on those resources
     * that are marked with immediate true flag.
     *
     * \param[in] expired_events set of expired events.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode HandleExpiry(const std::vector<ExpiryEventNotification>& expired_events);

    /**
     * \brief Launches an additional thread that will post a "kExecuteTransition" event inside the event queue to transition
     * the state machine to the parameter-provided state.
     *
     * \param[in] fetch_feature_leaves true if during processing of the feature list the resource leaves should be fetched,
     *            false otherwise.
     * \param[in] state state to transition to.
     * \param[in] sleep_seconds seconds to wait before pushing the "kExecuteTransition" event.
     *
     */
    void LaunchRetryHandler(bool fetch_feature_leaves,
                            DiscoveryProvider::State state,
                            std::uint32_t sleep_seconds = 60);

    /**
     * \brief Adds the parameter provided feature to the feature processor internal house-keeping structures in order
     * to deduce additional child resources that the feature might reference, all if the feature is deemed as trackable,
     * if not, it will be silently ignored.
     *
     * \param[in] feature feature to be tracked.
     */
    void TrackFeature(std::shared_ptr<Feature> feature);

    /**
     * \brief Checks if feature is defined as a trackable feature on feture processor.
     *
     * \param[in] feature feature to be tracked.
     *
     * \return True if trackable, false otherwise
     */
    bool IsFeatureTrackable(std::shared_ptr<Feature> feature);

    /**
     *
     * \brief Removed the parameter provided feature if the feature has been previously tracked by the tracking mechanism.
     * If no such tracking exists, no operation is performed.
     *
     * \param[in] feature feature to be un-tracked.
     */
    void UntrackFeature(std::shared_ptr<Feature> feature);

    std::shared_ptr<Discovery> discovery_;                              ///< Discovery Config container.
    std::shared_ptr<VsdpSource> vsdp_source_;                           ///< VSDP Source.
    std::shared_ptr<LocalConfig> local_config_;                         ///< Local config.
    std::shared_ptr<Feature> expiry_notification_feature_;              ///< Expiry notification feature.
    std::shared_ptr<ExpiryNotification> expiry_notification_;           ///< ExpiryNotification instance.
    FeatureProcessor feature_processor_;                                ///< Feature processor.
    std::shared_ptr<transfermanager::TransferManager> transfer_manager_;///< TransferManager reference.
    State state_;                                                       ///< Current active state in discovery state-machine.
    EntryPointData entry_point_data_;                                   ///< Stored EntryPoint parsed data.
    DaemonEventQueue& event_queue_;                                     ///< Daemon's event queue.
    std::map<std::string, std::uint32_t> update_bindings_;              ///< Resource name to update bind id.
    bool entry_updated_;                                                ///< True if during servide discovery, an entry update occured.
    const char* kExpiryFeatureName = "expiry";                          ///< Expiry feature name.
};

}

#endif // FSM_DISCOVERY_PROVIDER_INC_

/** \}    end of addtogroup */
