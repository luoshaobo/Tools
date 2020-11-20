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
 *  \file     expiry_notification.h
 *  \brief    Foundation Services ExpiryNotification Feature handler interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_EXPIRY_NOTIFICATION_H_INC_
#define FSM_EXPIRY_NOTIFICATION_H_INC_

#include <memory>
#include <string>

#include "voc_framework/signals/signal.h"
#include "voc_framework/signal_sources/signal_receiver_interface.h"
#include "voc_framework/signal_sources/ssm_signal_source.h"

#include <fscfgd/feature.h>

namespace fsm
{

/**
 * \brief ExpiryNotification Feature handler.
 *
 * Uses SignalServiceManager (SSM) to register on feature topics and trigger events related to the above-mentioned
 * feature throughout the system.
 */
class ExpiryNotification: public SignalReceiverInterface
{
public:
    /**
     * \brief ExpiryNotification Constructor.
     */
    ExpiryNotification(std::shared_ptr<Feature> expiry_feat_res);

    /**
     * \brief ExpiryNotification Destructor.
     */
    ~ExpiryNotification();

    /**
     * \brief Receiver function required by SignalReceiverInterface. Will accept ExpirySignal
     *        and distribute it to all affected parties.
     *  \param[in]  signal - signal for processing.
     */
     void ProcessSignal (std::shared_ptr<Signal> signal);

private:
    std::shared_ptr<Feature> expiry_feature_resource_;  ///< ExpiryNotification Feature.

    SsmSignalSource ssm_signal_source_;  ///< Signal Source for Expiry Notifications

    std::string topic_; ///< the topic SSM listens to, empty if not subscribed

    std::uint32_t state_changed_id_; ///< id for state change subscription towards feature
    std::uint32_t uri_changed_id_;   ///< id for uri change subscription towards feature

    std::mutex subscription_mutex_; ///< Mutex synchronizing SSM subscription operations

    /**
     * \brief Checks if ExpiryNotification feature is enabled
     */
    bool FeatureEnabled();

    /**
     * \brief Internal callback for feature state change, to be registered on Feature interface
     * \param[in] event new state
     */
    fscfg_ReturnCode StateChangedCallback(FeatureInterface::StateChangedEvent event);

    /**
     * \brief Internal callback for feature uri change, to be registered on Feature interface
     * \param[in] event new state
     */
    fscfg_ReturnCode UriChangedCallback(FeatureInterface::UriChangedEvent event);

    /**
     * \brief Checks if configuration has changed and then updatesthe SSM subscription accordingly
     */
    void UpdateSubscription();

};

}   // namespace fsm

#endif // FSM_EXPIRY_NOTIFICATION_H_INC_

/** \}    end of addtogroup */
