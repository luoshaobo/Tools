/*
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
 *  \file     ssm.h
 *  \brief    Signal Service Manager
 *  \author   Maksym Mozok & Gustav Evertsson
 *
 *  \addtogroup signalservicemanager
 *  \{
*/

#ifndef FSM_SSM_H
#define FSM_SSM_H

#include "fsm_ccm.h"
#include "fsm_persist_data_mgr.h"
#include "ssm_global.h"

#include <chrono>
#include <cstdint>
#include <condition_variable>
#include <map>
#include <mutex>

#include "coma/icommunicationmanagerproxy.h"
#include "coma/icommunicatorproxy.h"


namespace fsm
{

namespace signalservicemanager
{

/**
 * \brief Signal service manager class that provides interface to MQTT services.
 *
 */
class SignalServiceManager : public ICommunicationManagerProxyEvents
{
    private:
        const int kComaTimeout = 10; // In seconds
        std::string str_ssm_url_;
        fsm::fsmpersistdatamgr::FsmPersistDataMgr fsm_persist_data_manager_;
        RunningClients cl_connected_clients_;
        ICommunicationManagerProxy* coma_proxy_ = nullptr;
        ICommunicatorProxy* coma_communicator_ = nullptr;
        MqttConnectionIdentifier mqttConnectionIdentifier_;
        std::map<std::string, MqttTopicIdentifier> subscribed_topics_;
        std::function<void(MsgPkt_t&)> on_message_callback_;
        std::function<void(const NADIF_SMS_DATA *)> on_sms_message_callback_;

        void HandleComaInited(ResponseComaInitClient &response,
                              std::shared_ptr<std::condition_variable> cv,
                              std::shared_ptr<std::mutex> cv_m,
                              std::shared_ptr<bool> cb_received,
                              std::shared_ptr<ResponseComaInitClient> init_response);
        void HandleCommunicatorActivationChanged(ICommunicatorProxy::EventCommunicatorActivationChanged &event,
                                                 std::shared_ptr<std::condition_variable> cv,
                                                 std::shared_ptr<std::mutex> cv_m,
                                                 std::shared_ptr<bool> cb_received);
        void HandleStartMqttSession(ResponseCommunicatorDataStartMqttSession &response,
                                    std::shared_ptr<std::condition_variable> cv,
                                    std::shared_ptr<std::mutex> cv_m,
                                    std::shared_ptr<bool> cb_received,
                                    std::shared_ptr<ResponseCommunicatorDataStartMqttSession> mqtt_response);
        void HandleSubscribeMqtt(ResponseCommunicatorDataSubscribeMqtt &response,
                                 std::shared_ptr<std::condition_variable> cv,
                                 std::shared_ptr<std::mutex> cv_m,
                                 std::shared_ptr<bool> cb_received,
                                 std::shared_ptr<ResponseCommunicatorDataSubscribeMqtt> mqtt_response);
        void HandleMqttEncodeMessage(CallbackCommunicatorDataSendMessage &message,
                                     const_fs_Data payload);
        void HandleMqttSendMessage(ResponseCommunicatorDataSendMessage &response,
                                   std::shared_ptr<std::condition_variable> cv,
                                   std::shared_ptr<std::mutex> cv_m,
                                   std::shared_ptr<bool> cb_received,
                                   std::shared_ptr<ResponseCommunicatorDataSendMessage> mqtt_response);
        void HandleUnSubscribeMqtt(ResponseCommunicatorDataUnsubscribeMqtt &response,
                                   std::shared_ptr<std::condition_variable> cv,
                                   std::shared_ptr<std::mutex> cv_m,
                                   std::shared_ptr<bool> cb_received,
                                   std::shared_ptr<ResponseCommunicatorDataUnsubscribeMqtt> mqtt_response);
        void HandleStopMqttSession(ResponseCommunicatorDataStopMqttSession &response,
                                   std::shared_ptr<std::condition_variable> cv,
                                   std::shared_ptr<std::mutex> cv_m,
                                   std::shared_ptr<bool> cb_received,
                                   std::shared_ptr<ResponseCommunicatorDataStopMqttSession> mqtt_response);
    public:
        /**
         * \brief Default constructor.
         */
        SignalServiceManager();

        /**
         * \brief Default deconstructor.
         */
        ~SignalServiceManager();

        /**
         * \brief Creates the connection to Coma proxy and register the callback for incoming messages.
         * \param[in] callback Callback function for incoming messages.
         * \return ErrorCode.kNone on success
         */
        ErrorCode SignalServiceManagerInit(std::function<void(MsgPkt_t&)> callback);
        int set_sms_callback(std::function<void(const NADIF_SMS_DATA *)> callback);

        /**
         * \brief Takes down the connection to Coma proxy.
         */
        void SignalServiceManagerDone();


        std::string read_flag();
        
        /**
         * \brief Connect to mqtt server.
         * \return ErrorCode.kNone on success
         */
        ErrorCode SignalServiceManagerConnect();

        /**
         * \brief Subscribe to a mqtt topic.
         * \param[in] topic_name Mqtt topic
         * \param[in] qos QoS for this subscription.
         * \return ErrorCode.kNone on success
         */
        ErrorCode SignalServiceManagerSubscribe(const std::string topic_name, const int qos);

        /**
         * \brief Publish a new message on a mqtt topic.
         * \param[in] topic_name Mqtt topic
         * \param[in] payload Message payload
         * \param[in] qos QoS for this message, defaults to 1
         * \return ErrorCode.kNone on success
         */
        ErrorCode SignalServiceManagerPublish(const std::string topic_name, const_fs_Data payload, const int qos = 1);

        /**
         * \brief Unsubscribe to a mqtt topic.
         * \param[in] topic_name Mqtt topic
         * \return ErrorCode.kNone on success
         */
        ErrorCode SignalServiceManagerUnSubscribe(const std::string topic_name);

        /**
         * \brief Disconnect from the mqtt server.
         * \return ErrorCode.kNone on success
         */
        ErrorCode SignalServiceManagerDisconnect();

        /**
         * \brief Callback interface from Coma for incoming mqtt messages.
         * \param[in] event Data about the event.
         */
        void handleEvent(const NotificationMqttIncomingMessage* event);
        virtual void handleEvent(const NADIF_SMS_DATA* pEvent);

private:
        /**
         * \brief Handles the details of creating and initializing the CoMa Manager proxy.
         * The instance is retrieved and initialized if the initialization request was never passed inside the process
         * boundary.
         *
         * \return ErrorCode.kNone on success.
         */
        ErrorCode CreateComaClient();
};

} // namespace signalservicemanager
} // namespace fsm

#endif // FSM_SSM_H

/** \}    end of addtogroup */
