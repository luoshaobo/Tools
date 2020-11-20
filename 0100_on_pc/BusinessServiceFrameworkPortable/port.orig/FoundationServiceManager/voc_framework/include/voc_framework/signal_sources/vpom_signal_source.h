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
 *  \file     vpom_signal_source.h
 *  \brief    VOC Service vpom signal source.
 *  \author   Axel Fagerstedt, Florian Schindler
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_VPOM_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_VPOM_SIGNAL_SOURCE_H_

#include "voc_framework/features/feature.h"
#include "voc_framework/signal_sources/signal_source.h"
#include "voc_framework/signals/vpom_signal.h"

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "vpom_IClient.hpp"


namespace fsm
{

class VpomSignalSource : public SignalSource
{

 public:

    /**
     * \brief Get VpomSignalSource singelton instance.
     * \return VpomSignalSource singelton instance.
     */
    static VpomSignalSource& GetInstance();

    /**
     * \brief Get VpomIPositioningService object
     *
     * \return VpomIPositioningService object, which can be used for further calls
     */
    VpomIPositioningService& GetPositioningServiceObject();
    /**
     * \brief Starts a subscription for a signal in VolvoPositioningManager.
     *        Any recieved signals will be propagated to all features
     *        which have been added to the signal source.
     *
     * \param[in] signal_type The signal to subscribe for, should be
     *                        a VpomSignal type signal.
     * \return True if subscription was successfully started, otherwise false.
     */
    bool SubscribeForSignal(Signal::SignalType signal_type);

    /**
     * \brief Registers a list of signals at VolvoPositioningManager
     * See \ref RegisterSignal for details
     *
     * \param[in] requested_subscriptions  list of signals to be registered
     *
     * \return True if all registrations were successfull, otherwise false.
     */
    bool RegisterSignals(std::vector<Signal::SignalType> requested_subscriptions);

    /**
     * \brief Registers a single signal at VolvoPositioningManager
     * Callback will be registered at VolvoPositioningManager
     * Uses an internal reference counter, which is incremented with each call of this method
     *
     * \param[in] signal_type  signal to be registered
     *
     * \return True if registration was successfull, otherwise false.
     */
    bool RegisterSignal(Signal::SignalType signal_type);

    /**
     * \brief Unregisters a list of signals at VolvoPositioningManager
     * See \ref UnregisterSignal for details
     *
     * \param[in] requested_unsubscribptions  list of signals to be unregistered
     *
     * \return True if all unregistrations were successfull, otherwise false.
     */
    bool UnregisterSignals(std::vector<Signal::SignalType> requested_unsubscriptions);

    /**
     * \brief Unregister a single signal at VolvoPositioningManager
     * For the signal the reference counter will be decremented and
     * in case it is zero the callback will be deregistered at VolvoPositioningManager
     *
     * \param[in] signal_type  signal to be unregistered
     *
     * \return True if unregistration was successfull, otherwise false.
     */
    bool UnregisterSignal(Signal::SignalType signal_type, bool forced = false);

    /**
     * \brief Callback for GNSS position updates from tcam
     *
     * \param[in] data        The payload \ref GNSSPositionDataResponse in vpom_IPositioningService.hpp
     * \param[in] request_id  The request id.
     */
    void GNSSPositionDataResponseCb(vpom::GNSSData *data, uint64_t request_id);

 private:

    VpomSignalSource();
    ~VpomSignalSource();

    typedef std::map<Signal::SignalType, uint32_t>  SignalMapType;  //*< the signal and its reference counter

    /**
     * \brief list for subscribed/registered signal types with reference counter
     */
    SignalMapType active_subscriptions_;

    /**
     * \brief Tracks whether VolvoPositioningManager has been successfully initialized.
     */
    bool vpom_initialized_ = false;

    /**
     * \brief Mutex used to guard operations on subscriptions.
     */
    std::mutex subscriptions_mutex_;

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_VPOM_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
