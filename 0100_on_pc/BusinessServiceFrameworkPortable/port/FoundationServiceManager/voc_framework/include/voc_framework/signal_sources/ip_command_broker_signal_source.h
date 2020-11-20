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
 *  \file     ip_command_broker_signal_source.h
 *  \brief    VOC Service IPCommandBroker signal source.
 *  \author   Axel Fagerstedt, Florian Schindler
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_IP_COMMAND_BROKER_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_IP_COMMAND_BROKER_SIGNAL_SOURCE_H_

#include "voc_framework/features/feature.h"
#include "voc_framework/signal_sources/signal_source.h"
#include "voc_framework/signals/ip_command_broker_signal.h"

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "ipcb_IClient.hpp"
#include "ipcb_ITelematicsService.hpp"


namespace fsm
{

class IpCommandBrokerSignalSource : public SignalSource
{

 public:

    /**
     * \brief Get IpCommandBrokerSignalSource singelton instance.
     * \return IpCommandBrokerSignalSource singelton instance.
     */
    static IpCommandBrokerSignalSource& GetInstance();

    /**
     * \brief Get IpcbIGNSSService object
     *
     * \return IpcbIGNSSService object, which can be used for further calls
     */
    IpcbIGNSSService& GetIpcbGNSSServiceObject();

    ////////////////////////////////////////////////////////////
    // @brief : Get IpcbIOTAService object
    // @param[in]
    // @return     IpcbIGNSSService object, which can be used for further calls
    // @author     Tian Yong, Jan 24, 2019
    //////////////////////////////////////////////////////
    IpcbIOTAService& GetIpcbIOTAServiceObject();

    ////////////////////////////////////////////////////////////
    // @brief : Get IpcbIRVDCService object
    // @param[in]
    // @return     IpcbIRVDCService object, which can be used for further calls
    // @author     Nie Yujin, Feb 25, 2019
    //////////////////////////////////////////////////////    
    IpcbIRVDCService& GetIpcbRvdcServiceObject();

    /**
     * \brief Starts a subscription for a signal in IPCommandBroker.
     *        Any recieved signals will be propagated to all features
     *        which have been added to the signal source.
     *
     * \param[in] signal_type The signal to subscribe for, should be
     *                        a IpCommandBrokerSignal type signal.
     * \return True if subscription was successfully started, otherwise false.
     */
    bool SubscribeForSignal(Signal::SignalType signal_type);

    /**
     * \brief Registers a list of signals at IP Command Broker
     * See \ref RegisterSignal for details
     *
     * \param[in] requested_subscriptions  list of signals to be registered
     *
     * \return True if all registrations were successfull, otherwise false.
     */
    bool RegisterSignals(std::vector<Signal::SignalType> requested_subscriptions);

    /**
     * \brief Registers a single signal at IP Command Broker
     * Callback will be registered at IP Command Broker
     * Uses an internal reference counter, which is incremented with each call of this method
     *
     * \param[in] signal_type  signal to be registered
     *
     * \return True if registration was successfull, otherwise false.
     */
    bool RegisterSignal(Signal::SignalType signal_type);

    /**
     * \brief Unregisters a list of signals at IP Command Broker
     * See \ref UnregisterSignal for details
     *
     * \param[in] requested_unsubscribptions  list of signals to be unregistered
     *
     * \return True if all unregistrations were successfull, otherwise false.
     */
    bool UnregisterSignals(std::vector<Signal::SignalType> requested_unsubscriptions);

    /**
     * \brief Unregister a single signal at IP Command Broker
     * For the signal the reference counter will be decremented and
     * in case it is zero the callback will be deregistered at IP Command Broker
     *
     * \param[in] signal_type  signal to be unregistered
     *
     * \return True if unregistration was successfull, otherwise false.
     */
    bool UnregisterSignal(Signal::SignalType signal_type, bool forced = false);

    /**
     * \brief Callback for privacy notifications.
     *
     * \param[in] notification_data The privacy notification data.
     * \param[in] request_id        The request id.
     */
    void PrivacyNotificationCb(PrivacyNotificationType *notification_data, uint64_t request_id);

    /**
     * \brief Callback for dead reckoning (position) updates from Head Unit
     *
     * \param[in] data        The payload \ref DeadReckonedPositionResponse in ipcb_IGNSSService.hpp
     * \param[in] request_id  The request id.
     */
    void DeadReckonedPositionResponseCb(DeadReckonedPositionResponse *data, uint64_t request_id);

    /**
     * \brief Callback for GNSS position updates from Head Unit
     *
     * \param[in] data        The payload \ref GNSSPositionDataResponse in ipcb_IGNSSService.hpp
     * \param[in] request_id  The request id.
     */
    void GNSSPositionDataResponseCb(GNSSPositionDataResponse *data, uint64_t request_id);
    void THEFTNoticeNotificationCb(TheftNoticeData theft);
    void TelmDshbNotificationCb(OpTelmDshb_NotificationCyclic_Data TelmDshb_data);
    void RMSNotificationCb(RemoteMonitoringInfo_t  *rms_data);
    void HvBattVoltageCb(HvBattVoltageData hbv);
    void HvBattTempCb(HvBattTempData hbt);
    void VehSoHCheckResponseCb(VehSoHCheckData data);
    void VehSoHCheckNotifyCb(VehSoHCheckData data);
    bool VehicleSoHCheckRequest(uint64_t request_id);

 private:

    IpCommandBrokerSignalSource();
    ~IpCommandBrokerSignalSource();

    typedef std::map<Signal::SignalType, uint32_t>  SignalMapType;  //*< the signal and its reference counter

    /**
     * \brief list for subscribed/registered signal types with reference counter
     */
    SignalMapType active_subscriptions_;

    /**
     * \brief Tracks whether IPCommandBroker has been successfully initialized.
     */
    bool broker_initialized_ = false;

    /**
     * \brief Mutex used to guard operations on subscriptions.
     */
    std::mutex subscriptions_mutex_;

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_IP_COMMAND_BROKER_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
