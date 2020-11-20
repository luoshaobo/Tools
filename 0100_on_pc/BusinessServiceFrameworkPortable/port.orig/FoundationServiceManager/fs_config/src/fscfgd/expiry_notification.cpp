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
 *  \file     expiry_notification.cpp
 *  \brief    Foundation Services ExpiryNotification Feature handler implementation.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */


#include <fscfgd/expiry_notification.h>

#include <dlt/dlt.h>

#include <fscfg/fscfg_types.h>

#include <fscfgd/daemon.h>
#include <fscfgd/expiry_notification_signal.h>
#include <fscfgd/fscfgd.h>
#include <fscfgd/resource.h>

#include <voc_framework/signals/signal_factory.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd);

namespace fsm
{

ExpiryNotification::ExpiryNotification(std::shared_ptr<Feature> expiry_feat_res)
 : expiry_feature_resource_(expiry_feat_res)
{
    //register to SSM
    ssm_signal_source_.RegisterSignalReceiver(*this);

    //register signal factory
    fsm::SignalFactory::RegisterCcmFactory(fsm::ExpiryNotificationSignal::oid_, fsm::ExpiryNotificationSignal::CreateExpiryNotificationSignal);

    DLT_LOG_STRINGF(dlt_fscfgd,
                    DLT_LOG_INFO,
                    "Registered factory for ExpiryNotification: %s",
                    fsm::ExpiryNotificationSignal::oid_.c_str());

    //register callbacks for feature availability and URI changes
    if (expiry_feature_resource_)
    {
        //create and register state change callback
        Callback<FeatureInterface::StateChangedEvent> state_callback
                = std::bind(&ExpiryNotification::StateChangedCallback, this, std::placeholders::_1);

        if (expiry_feature_resource_->BindStateChanged(state_callback, state_changed_id_) != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "Failed to bind state change callback for ExpiryNotification.");
            //TODO: this should never happen, consider shutdown
        }

        //create and register URI change callback
        std::function<fscfg_ReturnCode (FeatureInterface::UriChangedEvent)> uri_callback
                = std::bind(&ExpiryNotification::UriChangedCallback, this, std::placeholders::_1);

        if (expiry_feature_resource_->BindUriChanged(uri_callback, uri_changed_id_) != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "Failed to bind Uri change callback for ExpiryNotification.");
            //TODO: this should never happen, consider shutdown
        }

    }
    else
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "No ExpiryNotification object");
        //TODO: this should never happen, consider shutdown
    }

    //subscribe to SSM
    UpdateSubscription();
}

ExpiryNotification::~ExpiryNotification()
{
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "ExpiryNotification unbindnig");

    //unbind bindings towards expiry feature
    if (expiry_feature_resource_)
    {
        if (state_changed_id_ != 0)
        {
            expiry_feature_resource_->Unbind(state_changed_id_);
        }

        if (uri_changed_id_ != 0)
        {
            expiry_feature_resource_->Unbind(uri_changed_id_);
        }
    }

    //unregister from the MQTT topic
    if (topic_.size() > 0)
    {
        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO, "ExpiryNotification unsubscribing from topic: %s", topic_.c_str());
        ssm_signal_source_.UnSubscribeFromTopic(topic_);
    }

    //remove itself as a signal receiver
    ssm_signal_source_.UnregisterSignalReceiver(*this);
}

void ExpiryNotification::UpdateSubscription()
{
    //make sure subscription handling is not done in parallel
    std::lock_guard<std::mutex> lock(subscription_mutex_);
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "ExpiryNotification updating subscription");

    if (FeatureEnabled())
    {
        std::string uri;
        if (expiry_feature_resource_->GetUri(uri) == fscfg_kRcSuccess)
        {
            //proceed only if we are not already subscribed to this topic
            if (uri.compare(topic_)!=0)
            {
                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_INFO,
                                "ExpiryNotification new topic found: %s.",
                                uri.c_str());

                // Use the cloud-resource workaround as there isn't any CloudSignEncrypt certificate.
                if (ssm_signal_source_.SubscribeToTopic(uri))
                {
                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_INFO,
                                    "ExpiryNotification subscibing to topic: %s.",
                                    uri.c_str());
                    //unsubscibe from previous topic, if there is any
                    if (topic_.size() > 0)
                    {
                        ssm_signal_source_.UnSubscribeFromTopic(topic_);
                    }
                    //assign new topic as current;
                    topic_ = uri;
                }
                else
                {
                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_ERROR,
                                    "ExpiryNotification failed to subscibe to topic: %s.",
                                    uri.c_str());
                }

            }
            else
            {
                DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "ExpiryNotification subscriptions up to date");
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "ExpiryNotification failed to get Uri.");
        }
    }
    else
    {
        //unsubscribe from previous topic, if there is any, and clean topic_
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "ExpiryNotification disabled");

        if (topic_.size() > 0)
        {
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_INFO,
                            "ExpiryNotification unsubscribing from topic: %s.",
                            topic_.c_str());
            ssm_signal_source_.UnSubscribeFromTopic(topic_);
            topic_ = std::string();
        }
    }
}

bool ExpiryNotification::FeatureEnabled()
{
    bool enabled = false;
    Feature::State state;
    if (expiry_feature_resource_)
    {
        if (expiry_feature_resource_->GetState(state) == fscfg_kRcSuccess)
        {
            enabled = (state == FeatureInterface::State::kEnabledVisible)
                      || (state == FeatureInterface::State::kEnabledInvisible);
        }
        else
        {
            DLT_LOG_STRING(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "ExpiryNotification failed to get state.");
        }
    }

    return enabled;
}

void ExpiryNotification::ProcessSignal (std::shared_ptr<Signal> signal)
{
    if (signal->GetSignalType() == fsm::kExpiryNotificationSignal)
    {
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "ExpiryNotification received signal");

        //cast to ExpiryNotificationSignal
        std::shared_ptr<ExpiryNotificationSignal> expiry_signal =
                std::static_pointer_cast<ExpiryNotificationSignal>(signal);

        //get daemon and event queue
        Daemon* daemon = GetDaemon();
        DaemonEventQueue& event_queue = daemon->GetQueue();
        std::shared_ptr<Config> config = daemon->GetConfig();

        if (daemon && config)
        {
            std::vector<fsm::ExpiryNotificationSignal::ResourceInfo> resources;
            resources = expiry_signal->GetResources();

            std::vector<ExpiryEventNotification> expired_events;

            //add resources from the notification
            for(auto const& resource: resources)
            {
                ExpiryEventNotification expired_event;
                expired_event.resource_name = resource.name;
                expired_event.immediate = resource.reconnect;
                expired_events.push_back(expired_event);
            }

            if (expired_events.size() > 0)
            {
                DLT_LOG_STRING(dlt_fscfgd,
                               DLT_LOG_INFO,
                               "Posting expiry notifications\n");
                event_queue.PostEvent(Event(ExpiryEvent(expired_events)));
            }
            else
            {
                DLT_LOG_STRING(dlt_fscfgd,
                               DLT_LOG_INFO,
                               "Found no expiry notifications for posting");
            }


        }
        else
        {
            //this should not happen
            DLT_LOG_STRING(dlt_fscfgd,
                           DLT_LOG_ERROR,
                           "ExpiryNotification failed to get daemon, event queue or config");
        }

    }
    else
    {
        //this should not happen, we should not receive any other signal over this topic
        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_ERROR,
                        "ExpiryNotification received unexpected signal: %d.",
                        signal->GetSignalType());
    }
}

fscfg_ReturnCode ExpiryNotification::StateChangedCallback(FeatureInterface::StateChangedEvent event)
{
    //no matter the change, just call UpdateSubscription, the function will figure out what to do
    UpdateSubscription();

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ExpiryNotification::UriChangedCallback(FeatureInterface::UriChangedEvent event)
{
    //no matter the change, just call UpdateSubscription, the function will figure out what to do
    UpdateSubscription();

    return fscfg_kRcSuccess;
}

}   // namespace fsm

/** \}    end of addtogroup */
