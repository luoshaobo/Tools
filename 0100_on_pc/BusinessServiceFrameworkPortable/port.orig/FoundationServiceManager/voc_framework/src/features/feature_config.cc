/**
 * Copyright (C) 2016 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     feature_config.cc
 *  \brief    Feature config
 *  \author   Piotr Tomaszewski
 *
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/features/feature_config.h"


#include "fscfg/fscfg.h"
#include "fscfg/discovery_interface.h"
#include "fscfg/feature_interface.h"

#include <algorithm>
#include <string>
#include <functional>
#include <future>
#include <memory>


#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

FeatureConfig::FeatureConfig(const std::string& feature_name): feature_name_(feature_name),
                                                               feature_(nullptr),
                                                               discovery_iface_(nullptr),
                                                               state_changed_id_(kUndefinedCallbackId),
                                                               features_changed_id_(kUndefinedCallbackId),
                                                               binding_thread_active_(false),
                                                               init_succeeded_(false)
{
    Init();
}

bool FeatureConfig::Init()
{
    //make sure there is no risk init happens in parallel
    std::lock_guard<std::mutex> lock(initialization_mutex_);

    //get discovery interface
    if (!discovery_iface_)
    {
        discovery_iface_ = fsm::GetDiscoveryInterface();
        if (!discovery_iface_)
        {
            DLT_LOG_STRINGF(dlt_libfsm,
                            DLT_LOG_ERROR,
                            "Failed to fetch discovery interface for: %s.",
                            feature_name_.c_str());
            //TODO: consider shutdown, this should not happen
        }
    }

    //if not yet initialized, perform initialization
    if (!init_succeeded_)
    {
        if (discovery_iface_)
        {
            //first sign up for features changes callback before attempting to connect.
            //This is to make sure that callback is not missed if resource change happens
            //while executing this init method
            if (features_changed_id_ == kUndefinedCallbackId)
            {
                std::function<fscfg_ReturnCode (DiscoveryInterface::ResourcesChangedEvent)> features_cb
                        = std::bind(&FeatureConfig::FeaturesChangedCb, this, std::placeholders::_1);

                fscfg_ReturnCode result = discovery_iface_->BindResourcesChanged(features_cb, features_changed_id_);

                if (result != fscfg_kRcSuccess)
                {
                    DLT_LOG_STRINGF(dlt_libfsm,
                                    DLT_LOG_ERROR,
                                    "Failed to bind features change callback for feature: %s. Change callback not available.",
                                    feature_name_.c_str());
                    //TODO: this should not happen, consider shutdown
                }
            }

            //initialize feature
            fscfg_ReturnCode result = discovery_iface_->Get(feature_name_, feature_);
            if (result != fscfg_kRcSuccess)
            {
                DLT_LOG_STRINGF(dlt_libfsm,
                                DLT_LOG_ERROR,
                                "Failed to retrieve feature: %s,"
                                " error: %d\n. Feature will be disabled.",
                                feature_name_.c_str(),
                                result);
            }
            else
            {
                init_succeeded_ = true;
            }
        }
    }

    //if we have state change callback request but we have not bound yet to the config_fs, bind.
    //this should succeed if init has succeeded
    if (init_succeeded_ && state_change_callback_ && (state_changed_id_ == kUndefinedCallbackId))
    {
        std::function<fscfg_ReturnCode (FeatureInterface::StateChangedEvent)> state_cb
                = std::bind(&FeatureConfig::StateChangedCb, this, std::placeholders::_1);
        if (feature_->BindStateChanged(state_cb, state_changed_id_) != fscfg_kRcSuccess)
        {
            DLT_LOG_STRINGF(dlt_libfsm,
                            DLT_LOG_ERROR,
                            "Failed to bind state change callback for feature: %s.",
                            feature_name_.c_str());
            //TODO: this should never happen, consider shutdown
        }

    }

    //if init succeeded and we have feature changed callback registered we may unbind it. This is because
    //at this point, if we still have not succeeded with registering state changed callback, it is not due to
    //resource not available (as init has succeeded).
    if (init_succeeded_ && (features_changed_id_ != kUndefinedCallbackId))
    {
        discovery_iface_->Unbind(features_changed_id_);
        features_changed_id_ = kUndefinedCallbackId;
    }

    return init_succeeded_;
}

/**
 * \brief Calls Init to perform initialization. If initialization is successful, and there is a callback
 *        registered, then the callback will be executed to notify the client about a change
 */
bool FeatureConfig::PerformInitializationAndNotifyOnChange()
{
    //Call Init to attempt initialization
    bool init_successful = Init();

    //If initialization has been successful and we have a callback registered, it should be called
    if (init_successful &&  state_change_callback_)
    {
        state_change_callback_(IsEnabled());
    }
    //to notify that we are done
    binding_thread_active_ = false;

    return init_successful;

}

FeatureConfig::~FeatureConfig()
{
    //Unbind all callbacks in case someone has forgotten to call this method properly.
    //This minimizes risk for undesired behavior but it is not an equivalent
    //to calling this method properly!
    UnregisterCallback();
}


bool FeatureConfig::IsEnabled()
{
    bool enabled = false;

    if (IsInitialized())
    {
        FeatureInterface::State state;
        if (fscfg_kRcSuccess == feature_->GetState(state))
        {
            enabled = (state == FeatureInterface::State::kEnabledVisible)
                      || (state == FeatureInterface::State::kEnabledInvisible);
        }
    }
    return enabled;
}

void FeatureConfig::RegisterStateChangeHandler(std::function<void(bool)> state_changed_cb)
{
    state_change_callback_ = state_changed_cb;
    Init();

}

fscfg_ReturnCode FeatureConfig::StateChangedCb(FeatureInterface::StateChangedEvent event)
{
    if (state_change_callback_)
    {
        state_change_callback_(event.state == FeatureInterface::State::kEnabledVisible
                                              || event.state == FeatureInterface::State::kEnabledInvisible);
    }
    return fscfg_kRcSuccess;
};


fscfg_ReturnCode FeatureConfig::FeaturesChangedCb(DiscoveryInterface::ResourcesChangedEvent event)
{

    auto iterator = find_if(event.features.begin(), event.features.end(),
                            [&](std::shared_ptr<FeatureInterface> resource)
                                {
                                    std::string name;
                                    bool result = false;
                                    if (resource->GetName(name) == fscfg_kRcSuccess)
                                    {
                                        result = (name.compare(feature_name_) == 0);
                                    }
                                    return result;
                                });

    bool feature_available_in_cloud = (iterator != event.features.end());

    //if feature found among available features and init has not been successful yet
    if (feature_available_in_cloud && !init_succeeded_)
    {
        //make sure only one binding thread can be created
        std::lock_guard<std::mutex> lock(binding_thread_mutex_);

        //when the resource becomes available, only one binding attempt is sufficient.
        //If it fails, we have some bigger issue and should consider controlled shutdown
        if (!binding_thread_active_)
        {
            //launch binding procedure in a separate thread
            //since async_handle is in scope of the class we are not blocking
            //callback return and async thread will execute in parallel
            //this is to avoid deadlock when calling other feature operation
            //from a registered callback
            binding_thread_active_ = true;
            async_handle_ = std::async(std::launch::async, &FeatureConfig::PerformInitializationAndNotifyOnChange, this);
        }
    }
    //check if feature has disappeared
    else if ((!feature_available_in_cloud) && init_succeeded_)
    {
        //unbind
        if ((state_changed_id_ != kUndefinedCallbackId) && feature_)
        {
            feature_->Unbind(state_changed_id_);

            state_changed_id_ = kUndefinedCallbackId;
        }

        feature_ = nullptr;
        init_succeeded_ = false;

        //notify the user if callback registered
        if (state_change_callback_)
        {
            state_change_callback_(false);
        }
    }

    return fscfg_kRcSuccess;
};


void FeatureConfig::UnregisterCallback()
{
    //unbind callbacks

    if ((state_changed_id_ != kUndefinedCallbackId) && feature_)
    {
        feature_->Unbind(state_changed_id_);

        state_changed_id_ = kUndefinedCallbackId;
    }

    if (features_changed_id_ && discovery_iface_)
    {
        discovery_iface_->Unbind(features_changed_id_);

        features_changed_id_ = kUndefinedCallbackId;
    }

    //remove client registered callbacks
    state_change_callback_ = nullptr;
}

bool FeatureConfig::IsInitialized()
{
    return init_succeeded_;
}

} // namespace fsm
/** \}    end of addtogroup */
