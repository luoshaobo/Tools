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
 *  \file     car_access_signal_source.h
 *  \brief    VOC Service CCM signal source base class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_CAR_ACCESS_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_CAR_ACCESS_SIGNAL_SOURCE_H_


#include "voc_framework/features/feature_config.h"
#include "voc_framework/signal_sources/ssm_signal_source.h"
#include "voc_framework/signals/car_access_signal.h"
#include "voc_framework/signals/cloud_resource.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/transactions/event_queue.h"

#include <string>
#include <thread>


namespace fsm
{

enum FeatureStatus
{
    kUnknown,
    kEnabled,
    kDisabled
};

enum OperationType
{
    kHandleSignal, //* handle incoming signal
    kCheckConfig   //* check configuration (mqtt subscriptions)
};

typedef struct
{
    OperationType operation_type;
    std::string topic;
    std::shared_ptr<Signal> signal;
    FeatureStatus status;
} Operation;

class CarAccessSignalSource: public SsmSignalSource, public EventQueue<Operation>
{


 public:

    //Only one Car Access signal source instance should exist in in the system
    static CarAccessSignalSource& GetInstance();

    /**
     * \brief Gets all paired users.
     *        Can be done directly with usermanager but using this utility ensures the
     *        same user manager instance gets reused.
     *        It's located in this class as it's likely that all entíties that wants to deal with
     *        users will also deal with CarAccessSignalSource.
     * \param[in/out] users The users will be added in this vector.
     * \return None.
     */
    void GetAllPairedUsers(std::vector<fsm::UserId>& users);

    /**
     * \brief Publishes signal over MQTT to a Car Access topic.
     *        The function will internally look up an appropriate
     *        topic for publishing in the Car Access configuration.
     * \param[in] signal signal to publish
     * \param[in] priority requested priority, if omitted a default priority will be used
     * \return true if publishing was successful, false otherwise     */
    bool PublishMessage(std::shared_ptr<PayloadInterface> signal, int priority = Signal::kPriorityUndefined);

    /**
     * \brief Types of special backend topics defined in car access
     */
    enum BackendTopic
    {
        kCatalogue,
        kDelegate,
        kDevicePairing
    };

    /**
     * \brief Publishes signal over MQTT to one of car access backend topics. The method looks up
     *        the actual topic in the car access configuration and publishes the signal on it
     * \param[in] signal signal to publish
     * \param[in] backend_topic requested topic
     * \return true if publishing was successful, false otherwise
     */
    bool PublishMessageToBackend(std::shared_ptr<PayloadInterface> signal,
                                 CarAccessSignalSource::BackendTopic backend_topic);


    // ***
    // Feature virtual functions implementation start.
    // ***

    /**
     * \brief Gets a topic required for signal with a given combination if oid and priority
     * \param[in] oid OID of the message to transfer
     * \param[in] priority required priority, usually the priority of the corresponding request
     * \return Name of the MQTT topic that should be used to publish the signal
     */
    std::string GetUplinkTopic(std::string oid, int priority);

    /**
     * \brief Gets a topic required for a signal with given oid
     * \param[in] oid OID of the message to transfer
     * \return Name of the MQTT topic that should be used to publish the signal
     */
    std::string GetUplinkTopic(std::string oid);

    ////////////////////////////////////////////////////////////
    // @brief :Returns the string literal for the passed enumerations.
    // @param[in]  signalType VDSSignalTypes enumerations
    // @return     uplink topic.
    // @author     linhaixia, 26-Sep-2018
    ////////////////////////////////////////////////////////////
    std::string GetUplinkTopic(Signal::SignalType signalType);

    /**
     * \brief Gets the priority of a topic with a given name
     * \param[in] topic topic name
     * \return the priority of the topic. Signal::kPriorityUndefined used if topic unknown
     */
    int GetDownlinkTopicPriority(std::string topic);

    /**
     * \brief Gets the name of a specific backend topic
     * \param[in] topic backend topic
     * \return topic name for a given backend topic
     */
    std::string GetBackendTopic(CarAccessSignalSource::BackendTopic topic);

 private:

    CarAccessSignalSource();
    ~CarAccessSignalSource();

    std::shared_ptr<CarAccessSignal> car_access_signal_;

    /**
     * \brief Cloud resource for car access config
     */
    CaFunc002CloudResource ca_func_002_resource_;

    /**
     * \brief Car access feature configuration
     */
    CarAccessFeatureConfig ca_feature_config_;

    /**
     * \brief Handler for resource change callback
     * \param[in] new_data new signal
     */
    void CloudResourceChangeHandlerCb(std::shared_ptr<fsm::CarAccessSignal> new_data);

    /**
     * \brief Handler for status change callback
     * \param[in] new_data new status
     */
    void FeatureStatusChangeHandlerCb(bool new_data);

    /**
     * \brief Checks if existing configuration (MQTT subscriptions) is still correct. If new configuration
     *        is available, it performs necessery adaptations
     * \param[in] signal optional input, set new configuration data if available,
     *                   if not, signal will be fetched
     * \param[in] status optional input, set new status if available,
     *                   if not, signal will be fetched
     */
    void CheckConfiguration(std::shared_ptr<Signal> signal = nullptr, FeatureStatus status = kUnknown);

    /**
     * \brief Processes signal received over MQTT
     * \param[in] operation object containing the signal details
     */
    void ProcessSignal(Operation& operation);

    //here comes implementation of base class methods

    //SsmSignalSource methods
    /**
     * \brief Handles signal received from the MQTT broker. It will first perform a check if
     *        the signal has come on a Car Access internal topic, in which case it will be
     *        forwarded to the Car Access feature. If the message has arrived on a generic
     *        Car Access topic, then it will be broadcasted to all subscribers.
     * \param[in] signal signal to handle, guaranteed not to be null
     * \param[in] signal topic on which the signal has been received
     */
    void HandleSignal(std::shared_ptr<Signal> signal, const std::string topic);

    //EventQueue methods
    /**
     * \brief Handles operations defined by operation (input parameter). It defines operation
     *        required by EventQueue interface.
     * \param[in] operation operation to handle
     * \return always true as Car Access Signal Source is always active
     */
    bool HandleEvent(Operation operation);



};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_CAR_ACCESS_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
