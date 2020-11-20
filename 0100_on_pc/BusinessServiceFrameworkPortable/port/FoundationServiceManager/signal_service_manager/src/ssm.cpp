/*
 * Copyright (C) 2017, 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     ssm.cpp
 *  \brief    Signal Service Manager
 *  \author   Maksym Mozok & Gustav Evertsson
 *
 *  \addtogroup signalservicemanager
 *  \{
 */

#include "ssm.h"

#include "dlt/dlt.h"

DLT_DECLARE_CONTEXT(dlt_libssm);

namespace fsm
{

namespace signalservicemanager
{

SignalServiceManager::SignalServiceManager()
{
    DLT_REGISTER_CONTEXT(dlt_libssm, "LSSM","libssm logs");
    DLT_LOG_STRING(dlt_libssm, DLT_LOG_INFO, "Initializing libssm");
    return;
}

SignalServiceManager::~SignalServiceManager()
{
    DLT_LOG_STRING(dlt_libssm, DLT_LOG_INFO, "Terminating libssm");
    DLT_UNREGISTER_CONTEXT(dlt_libssm);
    return;
}


std::string SignalServiceManager::read_flag()
{
    std::string flag;
    flag.clear();
    FILE * pFile = fopen ("/data/mqtt_flag.txt","r");
    if (pFile == NULL)
    {
        DLT_LOG_STRINGF(dlt_libssm,
                         DLT_LOG_INFO,
            "%s(): get flag error, open file error", __FUNCTION__);
        return flag;
    }
    char mystring [100];
    if ( fgets (mystring , 100 , pFile) != NULL )
    {
        flag = mystring;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libssm,
                         DLT_LOG_INFO,
            "%s(): get flag error, open file error", __FUNCTION__);
        return flag;
    }

    
    fclose (pFile);
    return flag;
}

ErrorCode SignalServiceManager::SignalServiceManagerInit(std::function<void(MsgPkt_t&)> callback)
{
    ErrorCode err_code = ErrorCode::kNone;

    if (!fsm_persist_data_manager_.FsmPersistDataInit())
    {
        DLT_LOG_STRING(dlt_libssm, DLT_LOG_INFO, "SignalServiceManagerInit failed to initialize persist data manager");

        err_code = ErrorCode::kPersistResourceNotFound;
    }
    else
    {
    
        if (read_flag() == "TLS")
        {
          DLT_LOG_STRINGF(dlt_libssm,
                         DLT_LOG_INFO,
                         "SignalServiceManager::%s(), use SSL"
                         , __FUNCTION__);
          str_ssm_url_ = fsm_persist_data_manager_.FsmPersistDataGet("signal_service_uri_ssl");
        }
        else
        {
          DLT_LOG_STRINGF(dlt_libssm,
                         DLT_LOG_INFO,
                         "SignalServiceManager::%s(), not use SSL"
                         , __FUNCTION__);
          str_ssm_url_ = fsm_persist_data_manager_.FsmPersistDataGet(kDefaultSignalServiceURIKeyname);
        }

        if (CreateComaClient() != ErrorCode::kNone)
        {
            DLT_LOG_STRING(dlt_libssm,
                           DLT_LOG_INFO,
                           "SignalServiceManagerInit failed to create CoMa Manager Proxy");

            err_code = ErrorCode::kBadState;
        }
        else
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_INFO, "SignalServiceManagerInit success! setting callback.");
            on_message_callback_ = callback;
        }
    }
    DLT_LOG_STRINGF(dlt_libssm,
                   DLT_LOG_INFO,
                   "%s(), attach sms event", __FUNCTION__);
    coma_proxy_->attachEvent(ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_NADIF_SMS_DATA, this);
    DLT_LOG_STRINGF(dlt_libssm,
                   DLT_LOG_INFO,
                   "%s(), attach mqtt event", __FUNCTION__);
    coma_proxy_->attachEvent(ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_MQTT_MESSAGE_ARRIVED, this);

    return err_code;
}

int SignalServiceManager::set_sms_callback(std::function<void(const NADIF_SMS_DATA *)> callback)
{
    on_sms_message_callback_ = callback;
    return 0;
}

void SignalServiceManager::HandleComaInited(ResponseComaInitClient &response,
                                            std::shared_ptr<std::condition_variable> cv,
                                            std::shared_ptr<std::mutex> cv_m,
                                            std::shared_ptr<bool> cb_received,
                                            std::shared_ptr<ResponseComaInitClient> init_response)
{
    std::unique_lock<std::mutex> lk(*cv_m);
    init_response->CopyFrom(response);
    *cb_received = true;
    lk.unlock();
    cv->notify_one();
}

void SignalServiceManager::SignalServiceManagerDone()
{
    if(coma_proxy_ != nullptr)
    {
        coma_proxy_ = nullptr;
        destroyCommunicationManagerProxy();
    }
}

ErrorCode SignalServiceManager::SignalServiceManagerConnect()
{
    if(coma_proxy_ == nullptr || coma_proxy_->getInitStatus() != ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS)
    {
        DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR, "SignalServiceManager connect() Coma Proxy has not been setup.");
        return ErrorCode::kBadState;
    }

    std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
    std::shared_ptr<std::mutex> cv_m = std::make_shared<std::mutex>();

    RequestStatus status;
    ResourceDescriptor rd;
    rd.m_type = ResourceDescriptor::RES_TYPE_CALL;

    {
        std::unique_lock<std::mutex> lk(*cv_m);
        std::shared_ptr<bool> cb_received = std::make_shared<bool>();
        *cb_received = false;

        coma_proxy_->getCommunicator(rd,
                                    coma_communicator_,
                                    std::bind(&SignalServiceManager::HandleCommunicatorActivationChanged, this, _1, cv, cv_m, cb_received));
        if(coma_communicator_ == nullptr)
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                        "SignalServiceManager could not get communicator from coma");
            return ErrorCode::kBadResource;
        }
        else if (!cv->wait_for(lk, std::chrono::seconds(kComaTimeout), [&]{return *cb_received;}))
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                           "SignalServiceManager Timeout on coma communicator activation");
            return ErrorCode::kTimeout;
        }
        else if(!coma_communicator_->isActivated())
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                           "SignalServiceManager coma communicator is not activated");
            return ErrorCode::kBadState;
        }
    }


    {
        std::unique_lock<std::mutex> lk(*cv_m);
        std::shared_ptr<ResponseCommunicatorDataStartMqttSession> mqtt_response = std::make_shared<ResponseCommunicatorDataStartMqttSession>();

        std::string url = str_ssm_url_;

        if(0 == url.size())
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                           "SignalServiceManager MQTT broker url is not found");
            return ErrorCode::kBadResource;
        }

        std::string divider("://");
        std::string::size_type pos = url.find(divider);

        if (pos!= std::string::npos)
        {
           const std::string mqtt("mqtt");
           const std::string mqtts("mqtts");

           DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_INFO,
                           "SignalServiceManager Prefixed url: %s\n", url.c_str());

           std::string prefix = url.substr(0, pos);

           DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_INFO,
                           "SignalServiceManager Prefix: %s\n", prefix.c_str());

           std::string new_prefix("");

           //to lower case
           for (std::string::size_type i = 0; i<pos; i++)
           {
               prefix[i] = tolower(prefix[i]);
           }

           //replace mqtt with tcp and mqtts with ssl as such prefixes are required by coma/paho
           if (prefix == mqtt)
           {
               url.erase(0, pos);
               url.insert(0,"tcp");
           }
           else if ((prefix == mqtts))
           {
               url.erase(0, pos);
               url.insert(0,"ssl");
           }

           DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_INFO,
                           "SignalServiceManager Url to be used: %s\n", url.c_str());
        }
        std::shared_ptr<bool> cb_received = std::make_shared<bool>();
        *cb_received = false;

        coma_communicator_->getICommunicatorData()->startMqttSession(status,
                                                                     url,
                                                                     std::bind(&SignalServiceManager::HandleStartMqttSession, this, _1, cv, cv_m, cb_received, mqtt_response));
        if(status.getValue() != RequestStatus::SUCCESS)
        {
            DLT_LOG_STRING_INT(dlt_libssm, DLT_LOG_ERROR,
                               "SignalServiceManager Coma communicator start mqtt failed with status ", status.getValue());
            return ErrorCode::kBadResource;
        }
        else if(!cv->wait_for(lk, std::chrono::seconds(kComaTimeout), [&]{return *cb_received;}))
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                           "SignalServiceManager Timeout on coma communicator mqtt start");
            return ErrorCode::kTimeout;
        }
        /*
        else if(mqtt_response->status() != Status::STATUS_SUCCESS)
        {
            DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                            "SignalServiceManager Coma communicator mqtt start failed with callback status %s",
                             Status_Name(mqtt_response->status()).c_str());
            return ErrorCode::kBadState;
        }
        mqttConnectionIdentifier_ = mqtt_response->mqttconnectionidentifier();
        DLT_LOG_STRING(dlt_libssm, DLT_LOG_INFO, "SignalServiceManager is now connected to COMA.");
        */
        //modify klocwork warning,yangjun add 20181107
        if(mqtt_response)
        {
            if(mqtt_response->status() != Status::STATUS_SUCCESS)
            {
                DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                                "SignalServiceManager Coma communicator mqtt start failed with callback status %s",
                                 Status_Name(mqtt_response->status()).c_str());
                return ErrorCode::kBadState;
            }
            mqttConnectionIdentifier_ = mqtt_response->mqttconnectionidentifier();
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_INFO, "SignalServiceManager is now connected to COMA.");
        }
    }

    return ErrorCode::kNone;
}

void SignalServiceManager::HandleCommunicatorActivationChanged(ICommunicatorProxy::EventCommunicatorActivationChanged &event,
                                                               std::shared_ptr<std::condition_variable> cv,
                                                               std::shared_ptr<std::mutex> cv_m,
                                                               std::shared_ptr<bool> cb_received)
{
    std::unique_lock<std::mutex> lk(*cv_m);
    *cb_received = true;
    lk.unlock();
    cv->notify_one();
}

void SignalServiceManager::HandleStartMqttSession(ResponseCommunicatorDataStartMqttSession &response,
                                                  std::shared_ptr<std::condition_variable> cv,
                                                  std::shared_ptr<std::mutex> cv_m,
                                                  std::shared_ptr<bool> cb_received,
                                                  std::shared_ptr<ResponseCommunicatorDataStartMqttSession> mqtt_response)
{
    std::unique_lock<std::mutex> lk(*cv_m);
    mqtt_response->CopyFrom(response);
    *cb_received = true;
    lk.unlock();
    cv->notify_one();
}


ErrorCode SignalServiceManager::SignalServiceManagerSubscribe(const std::string topic_name, const int qos)
{

    std::vector<MqttTopic> topics;
    MqttTopic topic;
    topic.set_topic(topic_name);
    topic.set_qos(qos);
    topics.push_back(topic);

    std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
    std::shared_ptr<std::mutex> cv_m = std::make_shared<std::mutex>();
    std::shared_ptr<bool> cb_received = std::make_shared<bool>();
    *cb_received = false;

    if (coma_proxy_ == nullptr
        || coma_proxy_->getInitStatus() != ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS
        || coma_communicator_ == nullptr)
    {
        DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR, "SignalServiceManager subscribe() Coma proxy and/or communicator has not been setup.");
        return ErrorCode::kBadState;
    }

    {
        std::unique_lock<std::mutex> lk(*cv_m);
        RequestStatus status;
        std::shared_ptr<ResponseCommunicatorDataSubscribeMqtt> mqtt_response = std::make_shared<ResponseCommunicatorDataSubscribeMqtt>();
        coma_communicator_->getICommunicatorData()->subscribeMqtt(status,
                                                                  topics,
                                                                  std::bind(&SignalServiceManager::HandleSubscribeMqtt, this, _1, cv, cv_m, cb_received, mqtt_response));

        if(status.getValue() != RequestStatus::SUCCESS)
        {
            DLT_LOG_STRING_INT(dlt_libssm, DLT_LOG_ERROR,
                               "SignalServiceManager Coma communicator subscribe mqtt failed with status ", status.getValue());
            return ErrorCode::kBadResource;
        }
        else if(!cv->wait_for(lk, std::chrono::seconds(kComaTimeout), [&]{return *cb_received;}))
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                           "SignalServiceManager Timeout on coma communicator mqtt subscribe");
            return ErrorCode::kTimeout;
        }
        /*
        else if(mqtt_response->status() != Status::STATUS_SUCCESS)
        {
            DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                            "SignalServiceManager Coma communicator mqtt subscribe failed with callback status %s",
                            Status_Name(mqtt_response->status()).c_str());
            return ErrorCode::kBadState;
        }
        */
        if(mqtt_response)
        {
            if(mqtt_response->status() != Status::STATUS_SUCCESS)
            {
                DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                                "SignalServiceManager Coma communicator mqtt subscribe failed with callback status %s",
                                Status_Name(mqtt_response->status()).c_str());
                return ErrorCode::kBadState;
            }
        }

        subscribed_topics_[topic_name] = mqtt_response->mqtttopicidentifier(0);

        DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_INFO, "SignalServiceManager is subscribed to MQTT topic %s.", topic_name.c_str());
    }

    return ErrorCode::kNone;
}

void SignalServiceManager::HandleSubscribeMqtt(ResponseCommunicatorDataSubscribeMqtt &response,
                                               std::shared_ptr<std::condition_variable> cv,
                                               std::shared_ptr<std::mutex> cv_m,
                                               std::shared_ptr<bool> cb_received,
                                               std::shared_ptr<ResponseCommunicatorDataSubscribeMqtt> mqtt_response)
{
    std::unique_lock<std::mutex> lk(*cv_m);
    mqtt_response->CopyFrom(response);
    *cb_received = true;
    lk.unlock();
    cv->notify_one();
}

ErrorCode SignalServiceManager::SignalServiceManagerPublish(const std::string topic_name, const_fs_Data payload, const int qos)
{
    std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
    std::shared_ptr<std::mutex> cv_m = std::make_shared<std::mutex>();
    std::shared_ptr<bool> cb_received = std::make_shared<bool>();
    *cb_received = false;

    std::vector<SendMethod> send_methods;
    SendMethod method;
    method.set_sink(SendMessageSink::SENDMESSAGERECIPIENT_DATABROKER);
    method.set_sinkaddress(topic_name);
    send_methods.push_back(method);

    if(coma_proxy_ == nullptr ||
        coma_proxy_->getInitStatus() != ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS ||
        coma_communicator_ == nullptr)
    {
        DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR, "SignalServiceManager publish() Coma proxy and/or communicator has not been setup.");
        return ErrorCode::kBadState;
    }

    std::vector<std::string> recipients;

    {
        std::unique_lock<std::mutex> lk(*cv_m);
        RequestStatus status;
        std::shared_ptr<ResponseCommunicatorDataSendMessage> mqtt_response = std::make_shared<ResponseCommunicatorDataSendMessage>();
        coma_communicator_->getICommunicatorData()->sendMessage(status,
                                                                send_methods,
                                                                recipients,
                                                                std::bind(&SignalServiceManager::HandleMqttEncodeMessage, this, _1, payload),
                                                                std::bind(&SignalServiceManager::HandleMqttSendMessage, this, _1, cv, cv_m, cb_received, mqtt_response));
        if(status.getValue() != RequestStatus::SUCCESS)
        {
            DLT_LOG_STRING_INT(dlt_libssm, DLT_LOG_ERROR,
                                "SignalServiceManager Coma communicator send message over mqtt failed with status ", status.getValue());
            return ErrorCode::kBadResource;
        }
        else if(!cv->wait_for(lk, std::chrono::seconds(kComaTimeout), [&]{return *cb_received;}))
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                           "SignalServiceManager Timeout on coma communicator mqtt send message");
            return ErrorCode::kTimeout;
        }
        /*
        else if(mqtt_response->status() != Status::STATUS_SUCCESS)
        {
            DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                            "SignalServiceManager Coma communicator send message over mqtt failed with callback status %s",
                            Status_Name(mqtt_response->status()).c_str());
            return ErrorCode::kBadState;
        }
        */
        if(mqtt_response)
        {
            if(mqtt_response->status() != Status::STATUS_SUCCESS)
            {
                DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                                "SignalServiceManager Coma communicator send message over mqtt failed with callback status %s",
                                Status_Name(mqtt_response->status()).c_str());
                return ErrorCode::kBadState;
            }
        }
    }
    return ErrorCode::kNone;
}

void SignalServiceManager::HandleMqttEncodeMessage(CallbackCommunicatorDataSendMessage &message,
                                                   const_fs_Data payload)
{
    message.set_payload(payload.data, payload.num_bytes);
}

void SignalServiceManager::HandleMqttSendMessage(ResponseCommunicatorDataSendMessage &response,
                                                  std::shared_ptr<std::condition_variable> cv,
                                                  std::shared_ptr<std::mutex> cv_m,
                                                 std::shared_ptr<bool> cb_received,
                                                  std::shared_ptr<ResponseCommunicatorDataSendMessage> mqtt_response)
{
    std::unique_lock<std::mutex> lk(*cv_m);
    mqtt_response->CopyFrom(response);
    *cb_received = true;
    lk.unlock();
    cv->notify_one();

}

ErrorCode SignalServiceManager::SignalServiceManagerUnSubscribe(const std::string topic_name)
{
    std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
    std::shared_ptr<std::mutex> cv_m = std::make_shared<std::mutex>();
    std::shared_ptr<bool> cb_received = std::make_shared<bool>();
    *cb_received = false;

    if(subscribed_topics_.find(topic_name) == subscribed_topics_.end())
    {
        DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                        "SignalServiceManager Could not lookup topic identifier for topic %s", topic_name.c_str());
        return ErrorCode::kBadKey;
    }

    if(coma_proxy_ == nullptr ||
        coma_proxy_->getInitStatus() != ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS ||
        coma_communicator_ == nullptr)
    {
        DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR, "SignalServiceManager unsubscribe() Coma proxy and/or communicator has not been setup.");
        return ErrorCode::kBadState;
    }

    std::vector<MqttTopicIdentifier> topics;
    topics.push_back(subscribed_topics_[topic_name]);

    {
        std::unique_lock<std::mutex> lk(*cv_m);
        RequestStatus status;
        std::shared_ptr<ResponseCommunicatorDataUnsubscribeMqtt> mqtt_response = std::make_shared<ResponseCommunicatorDataUnsubscribeMqtt>();
        coma_communicator_->getICommunicatorData()->unsubscribeMqtt(status,
                                                                    topics,
                                                                    std::bind(&SignalServiceManager::HandleUnSubscribeMqtt, this, _1, cv, cv_m, cb_received, mqtt_response));
        if(status.getValue() != RequestStatus::SUCCESS)
        {
            DLT_LOG_STRING_INT(dlt_libssm, DLT_LOG_ERROR,
                               "SignalServiceManager Coma communicator unsubscribe mqtt failed with status ", status.getValue());
            return ErrorCode::kBadResource;
        }
        else if(!cv->wait_for(lk, std::chrono::seconds(kComaTimeout), [&]{return *cb_received;}))
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                           "SignalServiceManager Timeout on coma communicator mqtt unsubscribe");
            return ErrorCode::kTimeout;
        }
        /*
        else if(mqtt_response->status() != Status::STATUS_SUCCESS)
        {
            DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                            "SignalServiceManager Coma communicator unsubscribe mqtt failed with callback status %s",
                            Status_Name(mqtt_response->status()).c_str());
            return ErrorCode::kBadState;
        }
        */
        if(mqtt_response)
        {
            if(mqtt_response->status() != Status::STATUS_SUCCESS)
            {
                DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                                "SignalServiceManager Coma communicator unsubscribe mqtt failed with callback status %s",
                                Status_Name(mqtt_response->status()).c_str());
                return ErrorCode::kBadState;
            }
        }
    }

    DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_INFO, "SignalServiceManager has unsubscribed from MQTT topic %s.", topic_name.c_str());

    subscribed_topics_.erase(topic_name);

    return ErrorCode::kNone;
}

void SignalServiceManager::HandleUnSubscribeMqtt(ResponseCommunicatorDataUnsubscribeMqtt &response,
                                                 std::shared_ptr<std::condition_variable> cv,
                                                 std::shared_ptr<std::mutex> cv_m,
                                                 std::shared_ptr<bool> cb_received,
                                                 std::shared_ptr<ResponseCommunicatorDataUnsubscribeMqtt> mqtt_response)
{
    std::unique_lock<std::mutex> lk(*cv_m);
    mqtt_response->CopyFrom(response);
    *cb_received = true;
    lk.unlock();
    cv->notify_one();
}

void SignalServiceManager::handleEvent(const NotificationMqttIncomingMessage* event)
{
    DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_VERBOSE,
                    "SignalServiceManager Incoming Message topic:%s qos:%d len:%d retained:%d\n",
                    event->topic().c_str(), event->qos(), event->payload().size(), event->retained());


    DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_INFO,
                    "DLT_LOG_INFO SignalServiceManager Incoming Message topic:%s qos:%d len:%d retained:%d\n",
                    event->topic().c_str(), event->qos(), event->payload().size(), event->retained());
    
    MsgPkt_t msg(event->topic(), event->payload());
    if(on_message_callback_)
    {
        on_message_callback_(msg);
    }
}

void SignalServiceManager::handleEvent(const NADIF_SMS_DATA* pEvent)
{
    DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_INFO,
                    "SignalServiceManager receive sms, addr=%s, index=%d, len=%d, data=%s, format=%d",
                    pEvent->addr, pEvent->index, pEvent->data_len, pEvent->data, pEvent->data_format);
    on_sms_message_callback_(pEvent);
}

ErrorCode SignalServiceManager::SignalServiceManagerDisconnect()
{
    std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
    std::shared_ptr<std::mutex> cv_m = std::make_shared<std::mutex>();
    std::shared_ptr<bool> cb_received = std::make_shared<bool>();

    if(coma_proxy_ == nullptr ||
        coma_proxy_->getInitStatus() != ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS ||
        coma_communicator_ == nullptr)
    {
        DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR, "SignalServiceManager disconnect() Coma proxy and/or communicator has not been setup.");
        return ErrorCode::kBadState;
    }

    {
        std::unique_lock<std::mutex> lk(*cv_m);
        RequestStatus status;
        std::shared_ptr<ResponseCommunicatorDataStopMqttSession> mqtt_response = std::make_shared<ResponseCommunicatorDataStopMqttSession>();
        coma_communicator_->getICommunicatorData()->stopMqttSession(status,
                                                                    mqttConnectionIdentifier_,
                                                                    std::bind(&SignalServiceManager::HandleStopMqttSession, this, _1, cv, cv_m, cb_received, mqtt_response));

        if(status.getValue() != RequestStatus::SUCCESS)
        {
            DLT_LOG_STRING_INT(dlt_libssm, DLT_LOG_ERROR,
                               "SignalServiceManager Coma communicator disconnect mqtt failed with status ", status.getValue());
            return ErrorCode::kBadResource;
        }
        else if(!cv->wait_for(lk, std::chrono::seconds(kComaTimeout), [&]{return *cb_received;}))
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                           "SignalServiceManager Timeout on coma communicator mqtt disconnect");
            return ErrorCode::kTimeout;
        }
        /*
        else if(mqtt_response->status() != Status::STATUS_SUCCESS)
        {
            DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                            "SignalServiceManager Coma communicator disconnect mqtt failed with callback status %s",
                            Status_Name(mqtt_response->status()).c_str());
            return ErrorCode::kBadState;
        }
        */
        if(mqtt_response)
        {
            if(mqtt_response->status() != Status::STATUS_SUCCESS)
            {
                DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                                "SignalServiceManager Coma communicator disconnect mqtt failed with callback status %s",
                                Status_Name(mqtt_response->status()).c_str());
                return ErrorCode::kBadState;
            }
        }
    }

    DLT_LOG_STRING(dlt_libssm, DLT_LOG_INFO, "SignalServiceManager is now disconnected from COMA.");

    return ErrorCode::kNone;
}

void SignalServiceManager::HandleStopMqttSession(ResponseCommunicatorDataStopMqttSession &response,
                                                 std::shared_ptr<std::condition_variable> cv,
                                                 std::shared_ptr<std::mutex> cv_m,
                                                 std::shared_ptr<bool> cb_received,
                                                 std::shared_ptr<ResponseCommunicatorDataStopMqttSession> mqtt_response)
{
    std::unique_lock<std::mutex> lk(*cv_m);
    mqtt_response->CopyFrom(response);
    *cb_received = true;
    lk.unlock();
    cv->notify_one();
}

ErrorCode SignalServiceManager::CreateComaClient()
{
    ErrorCode err_code = ErrorCode::kNone;

    // The technique in handling the initialization is to retrieve the instance and check for initialization status.
    // If it was not initialized prior, we call the initialization method and wait for the initialization result.
    if (!coma_proxy_)
    {
        coma_proxy_ = ::createCommunicationManagerProxy("SSMClientComaProxy", "libcomaclient.so");
    }

    if (!coma_proxy_)
    {
        DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR, "Failed to create CoMa Manager Proxy instance");
        err_code = ErrorCode::kBadResource;
    }
    else
    {
        // Check the initialization status to determine if initialization method should be called of not.
        // If InitStatus is success, nothing else to do - otherwise perform the initialization with blocking wait for
        // response.
        RequestStatus request_status;
        std::shared_ptr<ResponseComaInitClient> init_response = std::make_shared<ResponseComaInitClient>();
        std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
        std::shared_ptr<std::mutex> cv_m = std::make_shared<std::mutex>();
        std::shared_ptr<bool> cb_received = std::make_shared<bool>();
        std::unique_lock<std::mutex> lk(*cv_m);
        *cb_received = false;

        if (!init_response || !cv || !cv_m || !cb_received)
        {
            DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR, "SignalServiceManager failed to allocate dynamic objects");
            err_code = ErrorCode::kBadState;
        }
        else
        {
            ICommunicationManagerProxy::InitialisationStatus init_status = coma_proxy_->getInitStatus();

            switch (init_status)
            {
                case ICommunicationManagerProxy::INITIALISATIONSTATUS_NOTSTARTED:
                case ICommunicationManagerProxy::INITIALISATIONSTATUS_FAILED:
                {
                    // call init method and wait for response.
                    bool init_request_success = coma_proxy_->init(request_status,
                                                                  std::bind(&SignalServiceManager::HandleComaInited, this,
                                                                            _1, cv, cv_m, cb_received, init_response));

                    if (!init_request_success || request_status.getValue() != RequestStatus::SUCCESS)
                    {
                        DLT_LOG_STRING_INT(dlt_libssm, DLT_LOG_ERROR,
                                           "SignalServiceManager CoMa Manager Proxy init failed with status ",
                                           request_status.getValue());

                        err_code = ErrorCode::kBadResource;
                    }
                    else if (!cv->wait_for(lk, std::chrono::seconds(kComaTimeout), [&]{return *cb_received;}))
                    {
                        DLT_LOG_STRING(dlt_libssm, DLT_LOG_ERROR,
                                       "SignalServiceManager Timeout on coma communication manager proxy init");

                        err_code = ErrorCode::kTimeout;
                    }
                    else if(init_response->status() != Status::STATUS_SUCCESS)
                    {
                        DLT_LOG_STRINGF(dlt_libssm, DLT_LOG_ERROR,
                                        "SignalServiceManager CoMa Manager Proxy init failed with callback status %s",
                                        Status_Name(init_response->status()).c_str());

                        err_code = ErrorCode::kBadState;
                    }

                    break;
                }

                case ICommunicationManagerProxy::INITIALISATIONSTATUS_SUCCESS:
                {
                    DLT_LOG_STRING(dlt_libssm, DLT_LOG_INFO, "CoMa Manager Proxy already initialized");
                    break;
                }

                default:
                {
                    // Status code is not expected.
                    DLT_LOG_STRINGF(dlt_libssm,
                                    DLT_LOG_INFO,
                                    "Unexpected CoMa Manager Proxy init status code=%u",
                                    static_cast<unsigned>(init_status));

                    err_code = ErrorCode::kBadState;
                    break;
                }
            }
        }
    }

    return err_code;
}

} // namespace signalservicemanager
} // namespace fsm

/** \}    end of addtogroup */
