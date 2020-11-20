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
 *  \file     car_access_signal_source.cc
 *  \brief    VOC Service Car Access signal source.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signal_sources/car_access_signal_source.h"
#include "voc_framework/signals/car_access_signal.h"
#include "voc_framework/signals/signal_factory.h"

#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signals/internal_signal.h"

#include "usermanager_interface.h"

#include "dlt/dlt.h"

#include <algorithm>
#include <vector>

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

CarAccessSignalSource::CarAccessSignalSource(): SsmSignalSource()
{
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "Initializing CarAccessSignalSource\n ");

    SignalFactory::RegisterCcmFactory(CarAccessSignal::oid_, CarAccessSignal::CreateCarAccessSignal);

    std::function<void (bool)> feature_state_cb
            = std::bind(&CarAccessSignalSource::FeatureStatusChangeHandlerCb, this, std::placeholders::_1);

    ca_feature_config_.RegisterStateChangeHandler(feature_state_cb);


    std::function<void (std::shared_ptr<fsm::CarAccessSignal>) > ca_func_002_cb
            = std::bind(&CarAccessSignalSource::CloudResourceChangeHandlerCb, this, std::placeholders::_1);

    ca_func_002_resource_.RegisterChangeHandler(ca_func_002_cb);

    //trigger configuration check to update mqtt subscriptions. After that point
    //the subscription update will only happen in response to a change notification
    Operation operation;
    operation.operation_type = kCheckConfig;
    operation.signal = nullptr;
    operation.status = kUnknown;
    EnqueueEvent(operation);
}

void CarAccessSignalSource::GetAllPairedUsers(std::vector<fsm::UserId>& users)
{
    static std::shared_ptr<fsm::UsermanagerInterface> user_manager =
        fsm::UsermanagerInterface::Create();

    user_manager->GetUsers(users, fsm::UserRole::kAdmin);
    user_manager->GetUsers(users, fsm::UserRole::kUser);
    //TODO: should this include the delegate?
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "GetAllPairedUsers: %d ", users.size());


}

CarAccessSignalSource::~CarAccessSignalSource()
{
}


CarAccessSignalSource& CarAccessSignalSource::GetInstance()
{
    static CarAccessSignalSource instance;
    return instance;
}

bool CarAccessSignalSource::PublishMessage(std::shared_ptr<PayloadInterface> signal,
                                           int priority)
{
    bool successful_publish = false;

#ifndef VOC_TESTS
    if (ca_feature_config_.IsEnabled())
    {
        std::string oid = signal ? signal->GetIdentifier().payload_identifier.oid : "";

        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Publishing signal %s\n ", oid.c_str());

        //find out an appropriate topic to publish the message on
        std::string topic = GetUplinkTopic(oid, priority);

        if (topic.size() > 0)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "For publishing will use topic: %s\n ", topic.c_str());
            successful_publish = PublishMessageOnTopic(signal, topic);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "Cannot publish message, no matching topic for OID: %s",
                            oid.c_str());
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                       "Cannot publish message, not allowed by configuration");
    }

#else
    successful_publish = true;
#endif

    return successful_publish;
}

bool CarAccessSignalSource::PublishMessageToBackend(std::shared_ptr<PayloadInterface> signal,
                                                    CarAccessSignalSource::BackendTopic backend_topic)
{
    bool successful_publish = false;

#ifndef VOC_TESTS
    if (ca_feature_config_.IsEnabled())
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Publishing signal %s on topic: %d\n ",
                        signal ? signal->GetIdentifier().payload_identifier.oid.c_str() : "",
                        backend_topic);

        //find out an appropriate topic to publish the message on
        std::string topic = GetBackendTopic(backend_topic);

        if (topic.size() > 0)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "For publishing will use topic: %s\n ", topic.c_str());
            successful_publish = PublishMessageOnTopic(signal, topic);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "Cannot publish message, no matching topic for oid: %s",
                            signal ? signal->GetIdentifier().payload_identifier.oid.c_str() : "");

        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                       "Cannot publish message, not allowed by configuration");
    }

#else
    successful_publish = true;
#endif

    return successful_publish;
}

void CarAccessSignalSource::HandleSignal(std::shared_ptr<Signal> signal, const std::string topic)
{
    //pack signal in operation structure and enqueue it for processing

    Operation operation;
    operation.operation_type = kHandleSignal;
    operation.topic = topic;
    operation.signal = signal;

    EnqueueEvent(operation);
}


//this method should return the name of the topic
//that should be used for sending out the message with
//given oid and given requested priority
std::string CarAccessSignalSource::GetUplinkTopic(std::string oid, int priority)
{
    std::shared_ptr<CarAccessSignal> signal = ca_func_002_resource_.GetPayload();

    std::string return_value;

    if (signal) {
        std::string topic_oid, topic_priority, topic_normal;
        long int highest_oid_priority = LONG_MIN;
        long int closest_priority = LONG_MAX;

        std::vector<CarAccessSignal::MqttTopicInfo> &uplink_topics = *(signal->GetUplinkTopics());

        for (CarAccessSignal::MqttTopicInfo topic_info : uplink_topics) {

            // Try OID match, if multiple matches select one with highest priority
            for (CarAccessSignal::MqttServiceInfo service : topic_info.services) {
                if (service.oid == oid) {
                    if (topic_info.priority > highest_oid_priority) {
                        highest_oid_priority = topic_info.priority;
                        topic_oid = topic_info.topic;
                    }
                }
            }

            // Topic with priority closest but >= than priority
            if (priority != Signal::kPriorityUndefined
                    && topic_info.priority >= priority
                    && topic_info.priority < closest_priority) {
                closest_priority = topic_info.priority;
                topic_priority = topic_info.topic;
            }

            // Topic with normal priority
            if (topic_info.priority == e_Priority::Priority_normal) {
                topic_normal = topic_info.topic;
            }
        }  // for(CarAccessSignal...

        if (!topic_oid.empty()) {
            return_value = topic_oid;
        } else if (!topic_priority.empty()) {
            return_value = topic_priority;
        } else if (!topic_normal.empty()) {
            return_value = topic_normal;
        }

    } else {  // if (signal)
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "No car_access_signal!");
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "Selected outgoing topic %s", return_value.c_str());

    return return_value;
}

//this method should return the name of the topic
//that should be used for sending out the message with
//given oid and without any explicitely requested prio
std::string CarAccessSignalSource::GetUplinkTopic(std::string oid)
{
    return GetUplinkTopic(oid, Signal::kPriorityUndefined);
}

std::string CarAccessSignalSource::GetUplinkTopic(Signal::SignalType signalType)
{
    return "VDI/TEM/" + VdsSignalType::toString(signalType);
}

int CarAccessSignalSource::GetDownlinkTopicPriority(std::string topic)
{
    int priority = Signal::kPriorityUndefined;

    std::shared_ptr<CarAccessSignal> signal = ca_func_002_resource_.GetPayload();

    if (signal) {
        std::vector<CarAccessSignal::MqttTopicInfo> &downlink_topics = *(signal->GetDownlinkTopics());

        // Select matching topic with highest priority (in case of multiple hits)
        for (CarAccessSignal::MqttTopicInfo topic_info : downlink_topics) {
            if (topic_info.topic == topic) {
                if (topic_info.priority > priority) {
                    priority = topic_info.priority;
                }
            }
        }

    } else {  // if (signal)
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "No car_access_signal!");
    }

    return priority;
}


std::string CarAccessSignalSource::GetBackendTopic(CarAccessSignalSource::BackendTopic topic)
{
    std::shared_ptr<CarAccessSignal> car_access_signal = ca_func_002_resource_.GetPayload();
    std::string topic_name;
    if (car_access_signal)
    {
        switch (topic)
        {
        case CarAccessSignalSource::kCatalogue:
            topic_name = car_access_signal->GetCatalogueUplink();
            break;
        case CarAccessSignalSource::kDelegate:
            topic_name = car_access_signal->GetDelegateUplink();
            break;
        case CarAccessSignalSource::kDevicePairing:
            topic_name = car_access_signal->GetDevicePairingUplink();
            break;
        default:
            DLT_LOG_STRINGF(dlt_libfsm,
                            DLT_LOG_ERROR,
                            "CarAccessSignalSource, special topic with id %d undefined",
                            topic);
        }
    }
    else {  // if (signal)
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "No car_access_signal!");
    }
    return topic_name;
}

void CarAccessSignalSource::CloudResourceChangeHandlerCb(std::shared_ptr<fsm::CarAccessSignal> new_data)
{
    //configuration might have changed, trigger config check
    Operation operation;
    operation.operation_type = kCheckConfig;
    operation.signal = new_data;
    operation.status = kUnknown;
    EnqueueEvent(operation);
}

void CarAccessSignalSource::FeatureStatusChangeHandlerCb(bool new_data)
{
    //feature status may have changed, trigger config check
    Operation operation;
    operation.operation_type = kCheckConfig;
    operation.signal = nullptr;

    if (new_data)
    {
        operation.status = kEnabled;
    }
    else
    {
        operation.status = kDisabled;
    }
    EnqueueEvent(operation);
}

bool CarAccessSignalSource::HandleEvent(Operation operation)
{
    switch (operation.operation_type)
    {
    case kHandleSignal: ProcessSignal(operation); break;
    case kCheckConfig:  CheckConfiguration(operation.signal, operation.status); break;
    default:
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource: Received unknown operation\n ");
    }
    }
    return true; //we never stop on a signal
}

void CarAccessSignalSource::ProcessSignal(Operation& operation)
{
    //if message received on a generic topic, set the topic priority
    //and broadcast the signal to all subscribers
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource: Received signal, broadcasting\n ");

    std::shared_ptr<Signal> signal = operation.signal;

    int priority = GetDownlinkTopicPriority(operation.topic);
    if (priority != Signal::kPriorityUndefined)
    {
        signal->SetTopicPriority(priority);
    }
    BroadcastSignal(signal);
}

void CarAccessSignalSource::CheckConfiguration(std::shared_ptr<Signal> signal, FeatureStatus status)
{

    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource configuration re-check");

    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource new subscriptions only");

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s", "VDI/+/TEM");
    bool result = SubscribeToTopic("VDI/+/TEM");

    if(result)
    {
        // send kSubscribeTopicSignal to MTATransaction
        // and payload tag that show if force upload mta info to tsp
        // current: forceUploadMta = false;
        // this flag should be true while MQTT Server url changed
        // or devices Five Code changed ?
        fsm::InternalSignalTransactionId transaction_id;
        std::shared_ptr<bool> forceUploadMta = std::make_shared<bool>();
        *forceUploadMta = false;
        std::shared_ptr<fsm::InternalSignal<bool>> configChange =
        std::make_shared<fsm::InternalSignal<bool>>(transaction_id,
                                                    fsm::Signal::kSubscribeTopicSignal,
                                                    forceUploadMta );
        BroadcastSignal(configChange);
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource internal_signal_source");
    }
/*
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s","downlink topic_high");
    SubscribeToTopic("downlink topic_high");

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                     "downlink topic_low");
    SubscribeToTopic("downlink topic_low");

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                    "catalogue_downlink");
    SubscribeToTopic( "catalogue_downlink");

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                    "delegate_downlink");
    SubscribeToTopic( "delegate_downlink");

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                    "device_pairing_downlink");
    SubscribeToTopic("device_pairing_downlink");
*/

}



#if 0
void CarAccessSignalSource::CheckConfiguration(std::shared_ptr<Signal> signal, FeatureStatus status)
{

    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource configuration re-check");

    bool unsubscribe = false; //tracks if unsubscribing should be attempted

    std::string signal_desc = "undefined";
    if (signal)
    {
        signal_desc = signal->ToString();
    }


    DLT_LOG_STRINGF(dlt_libfsm,
                    DLT_LOG_INFO,
                    "CarAccessSignalSource configuration re-check, signal %s, status %d ",
                    signal_desc.c_str(),
                    status);


    //if parent feature is enabled
    if (status == kEnabled || (status == kUnknown && ca_feature_config_.IsEnabled()))
    {

        std::shared_ptr<CarAccessSignal> ca_func_002 = nullptr;

        if (signal)
        {
            ca_func_002 = std::dynamic_pointer_cast<CarAccessSignal>(signal);

        }

        if (ca_func_002 == nullptr)
        {
            DLT_LOG_STRING(dlt_libfsm,
                           DLT_LOG_INFO,
                           "CarAccessSignalSource fetching signal ");

            ca_func_002 = ca_func_002_resource_.GetPayload();
        }

        if(ca_func_002 != nullptr)
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource Ca func 002 available");

            std::shared_ptr<std::vector<CarAccessSignal::MqttTopicInfo>> new_topics =
                    ca_func_002->GetDownlinkTopics();

            // if we are already subscribed then subscribe only to new topics
            if (car_access_signal_ != nullptr)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource previous subscriptions exist");

                std::shared_ptr<std::vector<CarAccessSignal::MqttTopicInfo>> current_topics =
                        car_access_signal_->GetDownlinkTopics();

                // Subscribe to added topics
                for (auto new_topic : *new_topics)
                {
                    auto iterator = find_if(current_topics->begin(), current_topics->end(),
                                            [&new_topic](const CarAccessSignal::MqttTopicInfo &current_topic)
                    {return new_topic.topic == current_topic.topic;});
                    if (iterator == current_topics->end()) {  // new_topic not found in current_topics
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s", new_topic.topic.c_str());
                        SubscribeToTopic(new_topic.topic);
                    }
                }

                // Subscribe to the catalogue topic (if changed)
                if (car_access_signal_->GetCatalogueDownlink() != ca_func_002->GetCatalogueDownlink()) {

                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                                    ca_func_002->GetCatalogueDownlink().c_str());
                    SubscribeToTopic(ca_func_002->GetCatalogueDownlink());

                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource unsubscribing from %s",
                                    car_access_signal_->GetCatalogueDownlink().c_str());
                    UnSubscribeFromTopic(car_access_signal_->GetCatalogueDownlink());
                }

                // Subscribe to the delegate topic (if changed)
                if (car_access_signal_->GetDelegateDownlink() != ca_func_002->GetDelegateDownlink()) {
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                                    ca_func_002->GetDelegateDownlink().c_str());
                    SubscribeToTopic(ca_func_002->GetDelegateDownlink());
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource unsubscribing from %s",
                                    car_access_signal_->GetDelegateDownlink().c_str());
                    UnSubscribeFromTopic(car_access_signal_->GetDelegateDownlink());
                }

                // Subscribe to the device pairing topic (if changed)
                if (car_access_signal_->GetDevicePairingDownlink() != ca_func_002->GetDevicePairingDownlink()) {

                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                                    ca_func_002->GetDevicePairingDownlink().c_str());
                    SubscribeToTopic(ca_func_002->GetDevicePairingDownlink());
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource unsubscribing from %s",
                                    car_access_signal_->GetDevicePairingDownlink().c_str());
                    UnSubscribeFromTopic(car_access_signal_->GetDevicePairingDownlink());

                }

                // If it's first time we get the signal subscribe to all downlink topics
            } else {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource new subscriptions only");

                for (auto new_topic : *new_topics) {
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s", new_topic.topic.c_str());
                    SubscribeToTopic(new_topic.topic);
                }

                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                                ca_func_002->GetCatalogueDownlink().c_str());
                SubscribeToTopic(ca_func_002->GetCatalogueDownlink());

                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                                ca_func_002->GetDelegateDownlink().c_str());
                SubscribeToTopic(ca_func_002->GetDelegateDownlink());

                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource subscribing to %s",
                                ca_func_002->GetDevicePairingDownlink().c_str());
                SubscribeToTopic(ca_func_002->GetDevicePairingDownlink());
            }

            car_access_signal_ = ca_func_002;
        }
        else  //cafunc002 not available
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource no configuration available");
            unsubscribe = true;  //we unsubscribe if there is no configuration

        }
    }
    else  //Car Access feature is disabled
    {
        unsubscribe = true;
    }

    //if we should unsubscribe and there is a signal then we have subscribed to its topics.
    //Unsubscribe and remove the signal
    if (unsubscribe && (car_access_signal_ != nullptr))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource disabled, unsubscribing from topics");

        std::shared_ptr<std::vector<CarAccessSignal::MqttTopicInfo>> old_topics =
                car_access_signal_->GetDownlinkTopics();
        for (auto old_topic : *old_topics) {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource unsubscribing from %s", old_topic.topic.c_str());
            UnSubscribeFromTopic(old_topic.topic);
        }
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource unsubscribing from %s",
                        car_access_signal_->GetCatalogueDownlink().c_str());
        UnSubscribeFromTopic(car_access_signal_->GetCatalogueDownlink());

        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource unsubscribing from %s",
                        car_access_signal_->GetDelegateDownlink().c_str());
        UnSubscribeFromTopic(car_access_signal_->GetDelegateDownlink());

        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CarAccessSignalSource unsubscribing from %s",
                        car_access_signal_->GetDevicePairingDownlink().c_str());
        UnSubscribeFromTopic(car_access_signal_->GetDevicePairingDownlink());

        //remove the old signal
        car_access_signal_ = nullptr;

    }

}

#endif


} // namespace fsm
/** \}    end of addtogroup */
