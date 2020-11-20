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
 *  \file     events.h
 *  \brief    Foundation Services Daemon-side events
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_FSCFGD_EVENTS_H_INC_
#define FSM_FSCFGD_EVENTS_H_INC_

#include <fscfgd/discovery_provider.h>
#include <fscfgd/resource.h>

namespace fsm
{

    /**
     * \brief Identifier for the event generator's subsystem.
     */
    enum class EventClass : int
    {
        kUndefined,     ///< Initial undefined state.

        kDiscovery,     ///< Discovery subsystem identifier.
        kExpiry,        ///< Expiry subsystem identifier.

        kMax            ///< Last literal in enum. Keep this last.
    };

    /**
     * \brief  Discovery subsystem event identifier.
     */
    enum class DiscoveryEventType : int
    {
        kUndefined,             ///< Initial undefined state.

        kExecuteTransition,     ///< Execute transition event.
        kStateTransition,       ///< State transition report event.

        kMax                    ///< Last literal in enum. Keep this last.
    };

    /**
     * \brief Discovery subsystem event.
     */
    struct DiscoveryEvent
    {
        DiscoveryEvent() = default;

        /**
         * \brief DiscoveryEvent Constructor.
         *
         * \param[in] type discovery subsystem event identifier.
         * \param[in] state state-machine parameter.
         * \param[in] fetch_feature_leaves true if during feature list processing, the features should be independent fetched.
         */
        DiscoveryEvent(DiscoveryEventType type, DiscoveryProvider::State state, bool fetch_feature_leaves = true);

        DiscoveryEventType type; ///< discovery subsystem event identifier.

        /**
         * \brief event parameter.
         */
        struct Parameter
        {
           Parameter() = default;
           Parameter(DiscoveryProvider::State state, bool fetch_feature_leaves = true);

           DiscoveryProvider::State transition;     ///< state-machine paramter.
           bool fetch_feature_leaves;               ///< true if during feature list processing, the features should be independenty
                                                    /// fetched.
        } parameter;
    };

    /**
     * \brief  Expiry subsystem event identifier.
     */
    enum class ExpiryEventType : int
    {
        kUndefined,     ///< Initial undefined state.

        kExpired,       ///< Resource expired event.

        kMax            ///< Last literal in enum. Keep this last.
    };

    /**
     * \brief Expiry Event notification.
     */
    struct ExpiryEventNotification
    {
        std::string resource_name;  ///< resource that the expiry notification concerns.
        bool immediate;             ///< true if resource should be fetched immediately, false if just caches need to be updated.
    };


    /**
     * \brief Expiry subsystem event.
     */
    struct ExpiryEvent
    {
        ExpiryEvent() = default;

        /**
         * \brief ExpiryEvent constructor with list of expired events.
         *
         * \param[in] expired_event list of expired events.
         */
        ExpiryEvent(std::vector<ExpiryEventNotification> expired_events);

        ExpiryEventType type; ///< Expiry subsystem event identifier.

        /**
         * \brief event parameter.
         */
        struct Parameter
        {
            Parameter();
            Parameter(std::vector<ExpiryEventNotification> expired_events);

            std::vector<ExpiryEventNotification> expired_events;   ///< list of expired events.

        } parameter;
    };

    /**
     * \brief Describes event subsystem, event identifier and event parameters.
     */
    struct Event
    {
        EventClass event_class;                 ///< Event's subsystem.

        /**
         * \brief Subsystem event type and data.
         */
        struct SubsystemEvent
        {
            SubsystemEvent() = default;
            SubsystemEvent(const DiscoveryEvent& discovery_event);
            SubsystemEvent(const ExpiryEvent& expiry_event);

            DiscoveryEvent discovery_event;     ///< Discovery event.
            ExpiryEvent expiry_event;           ///< Expiry event.
        } subsystem_event;

        Event() = default;
        Event(EventClass event_class);
        Event(const DiscoveryEvent& discovery_event);
        Event(const ExpiryEvent& expiry_event);

    };

} // namespace fsm

#endif  // FSM_FSCFGD_EVENTS_H_INC_

/** \}    end of addtogroup */
