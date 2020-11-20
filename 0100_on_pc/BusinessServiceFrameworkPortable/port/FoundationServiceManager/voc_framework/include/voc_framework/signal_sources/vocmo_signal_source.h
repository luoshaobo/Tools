/**
 * Copyright (C) 2018 Continental AG and subsidiaries
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     vocmo_signal_source.h
 *  \brief    Vocmo signal source.
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_VOCMO_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_VOCMO_SIGNAL_SOURCE_H_

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <condition_variable>
#include <memory>

#include <icommunicationmanagerproxy.h>
#include <icommunicatorproxy.h>

#include "signal_source.h"
#include "voc_framework/signal_sources/ble_signal_source.h"
#include "voc_framework/signal_sources/car_access_signal_source.h"
#include "voc_framework/signal_sources/signal_receiver_interface.h"
#include "voc_framework/signals/payload_interface.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/gly_pack.h"
#include "voc_framework/signal_adapt/app_data.h"

extern "C"
{
 #include "nadif_sim.h"
}

namespace fsm
{

/**
 * \brief VocmoSignalSource class. Registers to all available VOCMO communication channels to send/receive
 *        CCM messages.
 */
class VocmoSignalSource : public SignalSource, public SignalReceiverInterface
{

 public:

    /**
     * \brief Constructor
     */
    VocmoSignalSource();

    /**
     * \brief Destructor
     */
    ~VocmoSignalSource();

    /**
     * \brief Gets all paired users.
     * \param[in/out] users The users will be added in this vector.
     * \return None.
     */
    void GetAllPairedUsers(std::vector<UserId>& users);

    /**
     * \brief This method inherited from SignalSource needs to be extended to, besides what the original method
     *        does, when registering first signal_receiver register VocmoSignalSource to all signal sources "below"
     *        (e.g. BleSignalSource, CarAccessSignalSource).
     * \param[in] signal_receiver - The signal receiver to add.
     */
    virtual void RegisterSignalReceiver(SignalReceiverInterface& signal_receiver);

    /**
     * \brief This method inherited from SignalSource needs to be extended to, besides what the original method
     *        does, when unregistering last signal_receiver unregister VocmoSignalSource from all signal sources
     *        "below" (e.g. BleSignalSource, CarAccessSignalSource).
     * \param[in] signal_receiver - The signal receiver to add.
     */
    virtual void UnregisterSignalReceiver(SignalReceiverInterface& signal_receiver);

    /**
     * \brief Broadcasts the signal to registered signal receivers.
     *  \param[in] signal - Signal for processing.
     */
    void ProcessSignal(std::shared_ptr<Signal> signal) override;
    //do not call this
    //bool GeelySendMessage(std::shared_ptr<fsm::Signal> signal, int priority = Signal::kPriorityUndefined);
    //but call this
    bool GeelySendMessage(std::shared_ptr<fsm::SignalPack> signal, bool sms = false, int priority = Signal::kPriorityUndefined);

    /**
     * \brief Sends the CCM to users.
     *
     * \param[in] signal - CCM message to be sent.
     * \param[in] priority - Optional MQTT topic priority.
     *
     * \return True/false for success/error.
     */
    bool SendMessage(std::shared_ptr<PayloadInterface> signal, int priority = Signal::kPriorityUndefined);

    /**
     * \brief Get Sim Current Profile.
     *
     * \param[in/out] mta - MTA data.
     *
     * \return True/false for success/error.
     */
    bool GetSimCurrentProfile(fsm::MTAData_t& mta);

    /**
     * \brief Handle get sim current Profile.
     *
     * \param[in/out] mta - MTA data.
     * \param[in] cv - condition variable.
     *
     * \return True/false for success/error.
     */
    void HandleGetSimCurrentProfile(NADIF_GET_SIM_CURRENT_PROFILE_INFO_RESP& response,
                                    fsm::MTAData_t& mta,
                                    std::shared_ptr<std::condition_variable> cv);

    /**
     * \brief Get nadIf network signal quality
     *
     * \param[in/out] mta - MTA data.
     *
     * \return True/false for success/error.
     */
    bool GetNadIfNetworkSignalQuality(MTAData_t& mta);

    /**
     * \brief Handle callback network signal quality.
     *
     * \param[in/out] mta - MTA data.
     * \param[in] cv - condition variable.
     *
     * \return True/false for success/error.
     */
    void HandleCallbackNetworkSignalQuality(NADIF_GET_SIG_QUAL_RESP& callback,
                                            MTAData_t& mta,
                                            std::shared_ptr<std::condition_variable> cv);

 private:

    /**
     * \brief VocmoSignalSource result codes.
     */
    typedef enum
    {
        kUndefined,                 ///< Uninitialized value.

        kSuccess,                   ///< Operation was executed with success.
        kFailed,                    ///< Operation failed without specific reason.
        kBadState,                  ///< Object is in bad state and cannot satifsy operation.
        kBadValue,                  ///< Unexpected or disallowed value.
        kTimeout,                   ///< Operation wait-time expired.

        kMax                        ///< Maximum value type should hold. Keep this last.
    } ResultCode;

    /**
     * \brief Number of seconds to wait until COMA operation time-out.
     */
    const uint32_t kComaTimeout = 10;
    
    const uint32_t kComaTimeout60 = 60;
    
    /**
     * \brief Get the ICommunicationMgrProxy instance.
     */
    ResultCode ComaGetCommunicationMgrProxy();

    /**
     * \brief Initialize the ICommunicationMgrProxy instance.
     */
    ResultCode ComaInitCommunicationMgrProxy();

    /**
     * \brief Callback with ICommunicationMgrProxy instance initialization status.
     */
    void ComaInitCommunicationMgrProxyCallback(ResponseComaInitClient& response,
                                               std::shared_ptr<std::condition_variable> cv);

    /**
     * \brief Get the ICommunicator instance, it's not usable until the callback ComaGetCommunicatorCallback()
     *        is received.
     */
    ResultCode ComaGetCommunicator();

    /**
     * \brief Callback informing if the ICommunicator instance was properly initialized.
     */
    void ComaGetCommunicatorCallback(ICommunicatorProxy::EventCommunicatorActivationChanged& event,
                                     std::shared_ptr<std::condition_variable> cv);

    /**
     * \brief Callback from ICommunicatorData interface informing it's ready to send the message.
     */
    void GeelyHandleSendMessageCallback(CallbackCommunicatorDataSendMessageToUsers& callback_data,
                                   std::shared_ptr<fsm::SignalPack> signal,
                                   std::shared_ptr<std::condition_variable> cv);
    /**
     * \brief Callback from ICommunicatorData interface informing about the sending operation has completed.
     */
    void GeelyHandleSendMessageResponse(ResponseCommunicatorDataSendMessageToUsers& response_data,
                                   std::shared_ptr<std::condition_variable> cv);

    /**
     * \brief Callback from ICommunicatorData interface informing it's ready to send the message.
     */
    void HandleSendMessageCallback(CallbackCommunicatorDataSendMessageToUsers& callback_data,
                                   std::shared_ptr<PayloadInterface> signal,
                                   std::shared_ptr<std::condition_variable> cv);

    /**
     * \brief Callback from ICommunicatorData interface informing about the sending operation has completed.
     */
    void HandleSendMessageResponse(ResponseCommunicatorDataSendMessageToUsers& response_data,
                                   std::shared_ptr<std::condition_variable> cv);

    std::string  read_isdn();
    /**
     * \brief Mutex used for synchronizing critical sections.
     */
    std::mutex mutex_;

    /**
     * \brief Number of registered receivers.
     *        Based on its value VocmoSignalSource registers/deregisters to/from signal sources it need,
     *        e.g. BleSignalSource
     */
    unsigned int receivers_count_ = 0;

    /**
     * \brief A pointer to ICommunicationManagerProxy instance.
     */
    ICommunicationManagerProxy* communication_mgr_ = nullptr;

    /**
     * \brief Protects against spurious wake-ups when waiting for ICommunicationManagerProxy::init() result.
     */
    bool coma_init_callback_received_ = false;

    /**
     * \brief A pointer to ICommunicator instance.
     */
    ICommunicatorProxy* communicator_ = nullptr;

    /**
     * \brief Protects against spurious wake-ups when waiting for ICommunicationManagerProxy::getCommunicator() result.
     */
    bool get_communicator_callback_received_ = false;

    /**
     * \brief Carries the ICommunicationManagerProxy::getCommunicator() result, true means the operation was successful.
     */
    bool communicator_activated_ = false;

    /**
     * \brief A pointer to BleSignalSource instance.
     */
    BleSignalSource* ble_signal_source_ = nullptr;

    /**
     * \brief Carries the ICommunicatorData::sendMessage() result.
     */
    com::contiautomotive::communicationmanager::messaging::internal::Status send_status_ =
        com::contiautomotive::communicationmanager::messaging::internal::Status::STATUS_UNINITED;

    /**
     * \brief Protects against spurious wake-ups when waiting for ICommunicatorData::sendMessage() result.
     */
    bool send_response_received_ = false;

    /**
     * \brief Set to true if signal encoding fails in the COMA pack callback. If it happens SendMessage will return
     *        with error immediately without waiting for COMA response callback.
     */
    bool signal_encoding_failed_ = false;
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_VOCMO_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
