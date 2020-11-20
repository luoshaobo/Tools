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
 *  \file     events.cpp
 *  \brief    Foundation Services Daemon-side events implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/events.h>

namespace fsm
{

///! Event
Event::Event(EventClass event_class)
    : event_class(event_class),
      subsystem_event()
{
}

Event::Event(const DiscoveryEvent& discovery_event)
    : event_class(EventClass::kDiscovery),
      subsystem_event(discovery_event)
{
}

Event::Event(const ExpiryEvent& expiry_event)
    : event_class(EventClass::kExpiry),
      subsystem_event(expiry_event)
{
}

///! Subsystem Event
Event::SubsystemEvent::SubsystemEvent(const DiscoveryEvent& discovery_event)
 : discovery_event(discovery_event)
{
}

Event::SubsystemEvent::SubsystemEvent(const ExpiryEvent& expiry_event)
{
    this->expiry_event = expiry_event;
}


///! Discovery Event
DiscoveryEvent::DiscoveryEvent(DiscoveryEventType type, DiscoveryProvider::State state, bool fetch_feature_leaves)
    : type(type),
      parameter(state, fetch_feature_leaves)
{
}

DiscoveryEvent::Parameter::Parameter(DiscoveryProvider::State state, bool fetch_feature_leaves)
      : transition(state),
        fetch_feature_leaves(fetch_feature_leaves)
{
}

///! Expiry Event
ExpiryEvent::ExpiryEvent(std::vector<ExpiryEventNotification> expired_resources)
    : type(ExpiryEventType::kExpired),
      parameter(expired_resources)
{
}

ExpiryEvent::Parameter::Parameter()
{
}

ExpiryEvent::Parameter::Parameter(std::vector<ExpiryEventNotification> expired_resources)
 : expired_events(expired_resources)
{
}

} // namespace fsm


/** \}    end of addtogroup */
