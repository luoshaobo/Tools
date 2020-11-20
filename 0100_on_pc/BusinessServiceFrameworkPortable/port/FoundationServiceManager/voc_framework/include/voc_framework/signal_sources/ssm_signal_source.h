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
 *  \file     ssm_signal_source.h
 *  \brief    VOC Service SSM signal source base class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_SSM_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_SSM_SIGNAL_SOURCE_H_

#include "voc_framework/signal_sources/signal_source.h"
#include "voc_framework/signals/payload_interface.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signal_sources/sms_certificate.h"

#include <string>
#include "s_ack.h"

#ifndef VOC_TESTS
    #include "ssm.h"
#endif

namespace fsm
{

class SsmSignalSource: public SignalSource
{

 public:

    /**
     * \brief Constructor of the SsmSignalSource
     */
    SsmSignalSource();

    /**
     * \brief Destructor of the SsmSignalSource
     */
    ~SsmSignalSource();

    /**
     * \brief Subscribes to an MQTT topic.
     * \param[in] topic topic to subscribe to
     * \return true if subscribtion was successful, false otherwise
     */
    bool SubscribeToTopic(std::string topic);

    /**
     * \brief Unsubscribes from an MQTT topic.
     * \param[in] topic topic to unsubscribe from
     * \return true if unsubscribtion was successful, false otherwise
     */
    bool UnSubscribeFromTopic(std::string topic);

    /**
     * \brief Publishes signal over MQTT on a specified topic.
     * \param[in] signal signal to publish
     * \param[in] topic topic to publish on
     * \param[in] recipients list of users to send the message to
     * \return true if publishing was successful, false otherwise
     */
    bool PublishMessageOnTopic(std::shared_ptr<PayloadInterface> signal,
                               std::string topic);

 protected:

    /**
     * \brief Handles signal received from the MQTT broker. This method is called when a signal is received
     *        from the MQTT broker. The signal is broadcasted to all subscribers. For signal source
     *        that inherit from SsmSignalSource this behavior can be overloaded.
     *        then it is broadcasted
     * \param[in] signal signal to handle, guaranteed not to be null
     * \param[in] topic topic on which the signal has been received
     */
    virtual void HandleSignal(std::shared_ptr<Signal> signal, const std::string topic);


 private:
#ifndef VOC_TESTS
    fsm::signalservicemanager::SignalServiceManager ssm_;
    void SSMCallBack(fsm::signalservicemanager::MsgPkt_t &msg);
    void ssm_sms_callback(const NADIF_SMS_DATA *sms_data);
    t_sms_certificate m_sms_certificate;
#endif
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_SSM_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
