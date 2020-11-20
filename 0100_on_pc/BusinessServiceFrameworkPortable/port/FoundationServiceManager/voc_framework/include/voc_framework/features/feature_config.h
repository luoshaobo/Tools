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
 *  \file     feature_config.h
 *  \brief    Feature config
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_FEATURES_FEATURE_CONFIG_H_
#define VOC_FRAMEWORK_FEATURES_FEATURE_CONFIG_H_

#include <algorithm>
#include <functional>
#include <string>

#include "voc_framework/signals/cloud_resource.h" //for feature name


namespace fsm
{

/**
 * \brief FeatureConfig class provides means of accessing
 *        the feature list level cloud configuration of a feature.
 * */
class FeatureConfig
{
private:
    /**
     * \brief Used to indicate that callback is undefined
     */
    const std::uint32_t kUndefinedCallbackId = 0;

public:
    /**
     * \brief Constructs FeatureConfig instance
     * \param[in] feature_name the name of the feature
     */
     FeatureConfig(const std::string& feature_name);

     /**
      * \brief Destructs FeatureConfig instance
      */
     ~FeatureConfig();

     /**
      * \brief Provides the state of the feature
      * \return true when enabled, false if disabled or if for any reason
      *         the configuration is not available
      */
     bool IsEnabled();

     /**
      * \brief Registers state change handler (enabled/disabled). Only one handler is supported, registing
      *        a new handler invalidates the previous one. Do not explicitely call IsEnabled inside the handler
      *        routine as it may lead to a deadlock. Current state is provided as input to the callback routine.
      * \param[in] state_changed_cb callback function that will be called when a state change has been detected.
      *            Events triggering re-check of the state are ExpiryNotification or re-read of the feature
      *            list. No active monitoring of the cache expiry is performed.
      */
     void RegisterStateChangeHandler(std::function<void(bool)> state_changed_cb);

     /**
      * \brief Unregisters existing callback. Make sure to call this prior to destroying
      *        either the FeatureConfig object or anything holding client callbacks.
      */
     void UnregisterCallback();

     /**
      * \brief Can be used to check if initialization has been successful. Note that initialization will be re-tried
      *        so the state may change with time
      *  \return true if successful, false otherwise.
      */
     bool IsInitialized();

private:
     /**
      * \brief Feature name
      */
     std::string feature_name_;

     /**
      * \brief Feature interface
      */
     std::shared_ptr<FeatureInterface> feature_;

     /**
      * \brief Discovery interface
      */
     std::shared_ptr<DiscoveryInterface> discovery_iface_;

     /**
      * \brief state change callback registered by a client
      */
     std::function<void(bool enabled)> state_change_callback_;

     /**
      * \brief state change callback id, for unbinding
      */
     std::uint32_t state_changed_id_;

     /**
      * \brief features changed, for unbinding
      */
     std::uint32_t features_changed_id_;


     /** \brief Mutex synchronizing access to the initialization procedure */
     std::mutex initialization_mutex_;

     /** \brief Mutex synchronizing creation of the binding thread */
     std::mutex binding_thread_mutex_;

     /** \brief Flag to tell binding is ongoing */
     bool binding_thread_active_;

     /** \brief Handle to a thread handling deferred binding. Only one such thread is created. The reason
      *         for having it in the scope of a class is not to block return from callback that indicates that
      *         the requested resource has become available (as this may lead to deadlock)
      */
     std::future<bool> async_handle_;

     /**
      * \brief keeps track if init has been successful
      */
     bool init_succeeded_;

     /**
      * \brief Initializes the FeatureConfig interface
      * \return true when initialized successfully, false otherwise
      */
     bool Init();

     /**
      * \brief Calls Init to perform initialization. If initialization is successful, and there is a callback
      *        registered, then the callback will be executed to notify the client about a change
      */
     bool PerformInitializationAndNotifyOnChange();

     /**
      * \brief Internal callback for feature state change, to be registered on Feature interface
      * \param[in] event new state
      */
     fscfg_ReturnCode StateChangedCb(FeatureInterface::StateChangedEvent event);

     /**
      * \brief Internal callback for features  change, to be registered on Discovery interface
      * \param[in] event new state
      */
     fscfg_ReturnCode FeaturesChangedCb(DiscoveryInterface::ResourcesChangedEvent event);
};


/**
 * \brief CarAccessFeatureConfig class is a specialized helper class for accessing
 *        the feature list level of the car access feature configuration.
 * */
class CarAccessFeatureConfig: public FeatureConfig
{
public:
    /**
     * \brief Constructs CarAccessFeatureConfig instance.
     */
    CarAccessFeatureConfig() : FeatureConfig(kCarAccessResourceName) {};
};


} // namespace fsm

#endif //VOC_FRAMEWORK_FEATURES_FEATURE_CONFIG_H_

/** \}    end of addtogroup */
