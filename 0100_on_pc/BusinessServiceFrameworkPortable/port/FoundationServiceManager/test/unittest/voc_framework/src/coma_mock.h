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
 *  \file     coma_mock.h
 *  \brief    CommunicationManager mock
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup unittest
 *  \{
 */

#include <gmock/gmock.h>

#include "icommunicationmanagerproxy.h"

#ifndef COMA_MOCK_H_
#define COMA_MOCK_H_

class CommunicationManagerProxyFake;
class CommunicatorProxyMock;
class CommunicatorDataMock;
class CommunicatorDataFake;

/**
 * \brief This class is a mock of ICommunicationManagerProxy
 */
class CommunicationManagerProxyMock : public ICommunicationManagerProxy
{
public:
    CommunicationManagerProxyMock();
    ~CommunicationManagerProxyMock();

    /**
     * \brief Returns coma_fake_
     */
    CommunicationManagerProxyFake* GetCommunicationManagerProxyFake();

    /**
     * \brief ICommunicationManagerProxy interface implementation
     */
    MOCK_CONST_METHOD0(getInitStatus, InitialisationStatus());
    MOCK_METHOD2(init,
                 bool(RequestStatus &requestStatus,
                      std::function<void(ResponseComaInitClient&)> memberFunction));
    MOCK_METHOD0(deinit, bool());
    MOCK_METHOD3(getCommunicator,
                 void(ResourceDescriptor& descriptor,
                      ICommunicatorProxy*& pCommunicator,
                      std::function<void(ICommunicatorProxy::EventCommunicatorActivationChanged&)> callback));
    MOCK_METHOD1(releaseCommunicator, void(CommunicatorId id));
    MOCK_METHOD0(getPSAPCallController, IPSAPCallController*());
    MOCK_METHOD0(getNadIfController, INadIfController*());
    MOCK_METHOD0(getInformationController, IInformationController*());
    MOCK_METHOD2(attachEvent,
                 void(ICommunicationManagerProxyEvents::EventType type,
                      ICommunicationManagerProxyEvents* callback));
    MOCK_METHOD2(detachEvent,
                 void(ICommunicationManagerProxyEvents::EventType type,
                      ICommunicationManagerProxyEvents* callback));
    MOCK_CONST_METHOD2(isAttached,
                 bool(ICommunicationManagerProxyEvents::EventType type,
                      ICommunicationManagerProxyEvents* callback));

    /**
     * \brief ICommunicationManager interface implementation
     */
    MOCK_METHOD2(getCommunicator, void(ResourceDescriptor& descriptor, ICommunicator*& pCommunicator));

private:
    CommunicationManagerProxyFake* coma_fake_;
};

/**
 * \brief This class is a fake of ICommunicationManagerProxy
 */
class CommunicationManagerProxyFake
{
public:
    CommunicationManagerProxyFake();
    ~CommunicationManagerProxyFake();

    /**
     * \brief Returns communicator_mock_
     */
    CommunicatorProxyMock* GetCommunicatorProxyMock();

    /**
     * \brief Sets coma_com_mgr_init_callback_response_
     */
    void SetFakeInitCallbackResponse(ResponseComaInitClient& response);

    /**
     * \brief Sets coma_get_comunicator_callback_event_
     */
    void SetFakeGetCommunicatorCallbackEvent(ICommunicatorProxy::EventCommunicatorActivationChanged& event);

    /**
     * \brief Sends event to ble_message_receiver_
     */
    void SendEvent(const NotificationBluetoothLEIncomingMessage* event);

    /**
     * \brief Execute memberFunction() in a new thread. Used for simulating asynchronous callback.
     * \param[in] memberFunction Callback to be executed when init is completed.
     */
    void init(RequestStatus &requestStatus,
              std::function<void(ResponseComaInitClient&)> memberFunction);

    /**
     * \brief Spawn a new thread that will sleep(seconds::kComaTimeout) before executing memberFunction().
     *        Used for simulating asynchronous callback that comes after client times-out.
     * \param[in] memberFunction Callback to be executed when init is completed.
     */
    void init_timeout(RequestStatus &requestStatus,
              std::function<void(ResponseComaInitClient&)> memberFunction);

    /**
     * \brief Set pCommunicator to CommunicatorProxyMock object and then create a new thread running
     *        callback.
     * \param[in] descriptor Not used.
     * \param[in/out] pCommunicator Will be set to ICommunicatorProxy object.
     * \param[in] callback Called when operation completes.
     */
    void getCommunicator(ResourceDescriptor& descriptor,
            ICommunicatorProxy*& pCommunicator,
            std::function<void(ICommunicatorProxy::EventCommunicatorActivationChanged&)> callback);

    /**
     * \brief Save registered callback interfaces to use them later for sending events.
     */
    void attachEvent(ICommunicationManagerProxyEvents::EventType type,
            ICommunicationManagerProxyEvents* callback);

private:

    /**
     * \brief This object is returned in getCommunicator() call
     */
    CommunicatorProxyMock* communicator_mock_;

    /**
     * \brief Fake response returned in callback resulting from init() call, set in SetFakeInitCallbackResponse()
     */
    ResponseComaInitClient coma_com_mgr_init_callback_response_;

    /**
     * \brief Fake response returned in callback resulting from getCommunicator() call,
     *        set in SetFakeGetCommunicatorCallbackEvent()
     */
    ICommunicatorProxy::EventCommunicatorActivationChanged coma_get_comunicator_callback_event_;

    /**
     * \brief Set in attachEvent() when the type is UNSOLICITEDEVENTTYPE_BTLE_MESSAGE_ARRIVED, used for sending
     *        BLE message events to registered listeners
     */
    ICommunicationManagerProxyEvents* ble_message_receiver_ = nullptr;

    /**
     * \brief Sleep kComaTimeout before executing memberFunction().
     * \param[in] memberFunction Callback.
     */
    void init_timeout_thread(std::function<void(ResponseComaInitClient&)> memberFunction);
};

/**
 * \brief Implements ICommunicatorProxy
 */
class CommunicatorProxyMock : public ICommunicatorProxy
{
public:
    CommunicatorProxyMock();
    ~CommunicatorProxyMock();

    /**
     * \brief Returns communicator_data_mock_
     */
    CommunicatorDataMock* GetCommunicatorDataMock();

    MOCK_CONST_METHOD0(isActivated, bool());
    MOCK_CONST_METHOD0(getCommunicatorId, CommunicatorId());
    MOCK_METHOD0(getICommunicatorCalls, ICommunicatorCalls*());
    MOCK_METHOD0(getICommunicatorData, ICommunicatorData*());
    MOCK_METHOD0(getICommunicatorPSAPCall, ICommunicatorPSAPCall*());
private:
    CommunicatorDataMock* communicator_data_mock_;
};

/**
 * \brief ICommunicatorProxy fake class
 */
class CommunicatorProxyFake
{
public:
    ICommunicatorData* getICommunicatorData();
};

/**
 * \brief ICommunicatorData mock class
 */
class CommunicatorDataMock : public ICommunicatorData
{
public:
    CommunicatorDataMock();
    ~CommunicatorDataMock();

    /**
     * \brief Returns communicator_data_fake_
     */
    CommunicatorDataFake* GetCommunicatorDataFake();

    MOCK_METHOD3(startMqttSession,
                 void(RequestStatus &requestStatus,
                      const std::string url,
                      std::function<void(ResponseCommunicatorDataStartMqttSession&)> callback));

    MOCK_METHOD3(stopMqttSession,
                 void(RequestStatus &requestStatus,
                      const MqttConnectionIdentifier& mqttConnectionIdentifier,
                      std::function<void(ResponseCommunicatorDataStopMqttSession&)> callback));

    MOCK_METHOD3(subscribeMqtt,
                 void(RequestStatus &requestStatus,
                      const std::vector<MqttTopic> topics,
                      std::function<void(ResponseCommunicatorDataSubscribeMqtt&)> callback));

    MOCK_METHOD3(unsubscribeMqtt,
                 void(RequestStatus &requestStatus,
                      const std::vector<MqttTopicIdentifier> topics,
                      std::function<void(ResponseCommunicatorDataUnsubscribeMqtt&)> callback));

    MOCK_METHOD5(sendMessage,
                 void(RequestStatus &requestStatus,
                      const std::vector<SendMethod> &methods,
                      const std::vector<std::string> &recipents,
                      std::function<void(CallbackCommunicatorDataSendMessage&)> packCallback,
                      std::function<void(ResponseCommunicatorDataSendMessage&)> callback));

    MOCK_METHOD4(sendMessage,
                 void(RequestStatus &requestStatus,
                      SendDescription &sendDescription,
                      std::function<void(CallbackCommunicatorDataSendMessageToUsers&)> packCallback,
                      std::function<void(ResponseCommunicatorDataSendMessageToUsers&)> callback));

    MOCK_METHOD4(httpsGet,
                 void(RequestStatus &requestStatus,
                      const std::string url,
                      const std::vector<std::string> &header,
                      std::function<void(ResponseCommunicatorDataHttpsGet&)> callback));

    MOCK_METHOD2(startDataSession,
                 void(RequestStatus &requestStatus,
                      std::function<void(ResponseCommunicatorStartDataSession&)> callback));

    MOCK_METHOD3(stopDataSession,
                 void(RequestStatus &requestStatus,
                      unsigned int sessionId,
                      std::function<void(ResponseCommunicatorStopDataSession&)> callback));

private:
    CommunicatorDataFake* communicator_data_fake_ = nullptr;
};

/**
 * \brief ICommunicatorData fake class
 */
class CommunicatorDataFake
{

public:

    /**
     * \brief Sets the callback_parameter_
     */
    void SetFakeCallbackParameter(CallbackCommunicatorDataSendMessageToUsers& parameter);

    /**
     * \brief Sets the response_parameter_
     */
    void SetFakeResponseParameter(ResponseCommunicatorDataSendMessageToUsers& parameter);
    /**
     * \brief Spawns threads executing callback and response functions
     */
    void sendMessage(RequestStatus& requestStatus, SendDescription& sendDescription,
                     std::function<void(CallbackCommunicatorDataSendMessageToUsers&)> callback,
                     std::function<void(ResponseCommunicatorDataSendMessageToUsers&)> response);

    /**
     * \brief This function sleeps before execting the callback, used to simulate asynchronous callback execution by COMA
     */
    void callback_delayed(std::function<void(CallbackCommunicatorDataSendMessageToUsers&)> callback);

    /**
     * \brief This function sleeps before execting the response, used to simulate asynchronous callback execution by COMA
     */
    void response_delayed(std::function<void(ResponseCommunicatorDataSendMessageToUsers&)> response);

private:

    /**
     * \brief Parameter passed to the callback from sendMessage()
     */
    CallbackCommunicatorDataSendMessageToUsers* callback_parameter_ = nullptr;

    /**
     * \brief Parameter passed to the response from sendMessage()
     */
    ResponseCommunicatorDataSendMessageToUsers* response_parameter_ = nullptr;
};


#endif  // #ifndef COMA_MOCK_H_

/** \}    end of addtogroup */
