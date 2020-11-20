/**
 * Copyright (C) 2017-2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     device_pairing_transaction.h
 *  \brief    Device pairing transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_DEVICE_PAIRING_TRANSACTION_H_
#define VOC_TRANSACTIONS_DEVICE_PAIRING_TRANSACTION_H_

#include <math.h>
#include <chrono>

#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/sha.h>

#include "ble_manager_interface.hpp"
#include "voc_framework/transactions/smart_transaction.h"

#include "signals/aes_csr_actor_voc_frame.h"
#include "signals/aes_csr_nearfield_voc_frame.h"
#include "signals/ca_dpa_002_signal.h"
#include "signals/ca_dpa_021_signal.h"
#include "signals/ecdhe_request_voc_frame.h"
#include "signals/voc_frame_codec.h"

namespace volvo_on_call
{

typedef struct Derived_Secret {
    unsigned char* secret_buffer;
    size_t secret_len;
} Derived_Secret_t;

/**
* \brief Default timeouts definitions.
*/
const std::chrono::seconds kVisibility_timeout_default(120);
const std::chrono::seconds kRemote_timeout_default(10);

/**
 * \brief LocalConfig section used to keep configuration variables.
 */
const std::string kFSMLocalConfigSection = "FoundationServiceManager";

/**
 * \brief LocalConfig PairingVisibilityTimeout used in pairing transaction.
 */
const std::string kPairingVisibilityTimeout = "PairingVisibilityTimeout";

/**
 * \brief LocalConfig RemotePairingVisibilityTimeout used in pairing transaction.
 */
const std::string kRemotePairingVisibilityTimeout = "RemotePairingVisibilityTimeout";

/**
 * \brief DevicePairingTransaction implements the Device Pairing State Machine.
 *        It is started in response to a PairingVisibilityRequest from IHU and
 *        continues until pairing has finished or failed.
 */

class DevicePairingTransaction: public fsm::SmartTransaction
{
 public:

    /**
     * \brief Creates a device pairing transaction.
     * \param[in] initial_signal fsm::Signal which caused transaction to be created.
     */
    DevicePairingTransaction(std::shared_ptr<fsm::Signal> initial_signal);

    /**
     * \brief Destroy a device pairing transaction entity.
     */
    ~DevicePairingTransaction();

 private:

#ifdef VOC_TESTS
 //make privates available to tests
 public:
#endif
    // Internal signal created and enqueued on itself by the DevicePairingTransaction
    // when handling BLE manager connection callback
    class BleConnectionSignal : public fsm::Signal
    {
        public:
            BleConnectionSignal(BLEM::ConnectionID id, BLEM::ConnectionState state)
                : fsm::Signal(fsm::CCMTransactionId(), VocSignalTypes::kBleConnectionSignal)
            {
                connection_id_ = id;
                connection_state_ = state;
            }

            BLEM::ConnectionID GetConnectionId()
            {
                return connection_id_;
            }

            BLEM::ConnectionState GetConnectionState()
            {
                return connection_state_;
            }

            std::string ToString()
            {
                return "BleConnectionSignal";
            }

        private:
            BLEM::ConnectionID connection_id_;
            BLEM::ConnectionState connection_state_;
    };

    /**
     * \brief Enumerates the states this transaction can be in.
     */
    enum States
    {
        kNew = 0, ///< Newly created.
        kAdvertising, ///< TCAM advertising its availability for pairing.
        kEcdheApproval, ///< Received ECDHE request from client and seeking approval
        kCreateClientCertificates, ///< Received approval both from client and head unit
        kFinalizePairing, ///< Client certificates handled, finalize pairing
        kDone ///< Finished
    };

    /**
     * \brief Enumerates the possible pairing channels we may be pairing over.
     */
    enum PairingChannel
    {
        kUndefined, ///< Default undefined value.
        kVsdp, ///< Pairing over SingalSerivice via VSDP
        kBle ///<Pairing over BLE pairing service
        //kWifi
    };

    const int kOnScreenPinSize = 6;

    /**
     * \brief The EC curve in use.
     */
    const int kCurve_name_ = NID_X9_62_prime256v1;

    /**
     * \brief Calculated shared ECDHE secret.
     */
    std::vector<unsigned char> ecdhe_shared_secret_;

    /**
     * \brief Generated ECDHE ephemeral key.
     */
    EVP_PKEY *ecdhe_ephemeral_key_ = nullptr;

    /**
     * \brief VocFrameCodec used to encode/decode voc frames.
     */
    VocFrameCodec codec_;

    /**
     * \brief Interface to the pairing service.
     *        Initialization may fail in which case this will
     *        be null.
     *        BLEManager owns this pointer, we must not free it.
     */
    BLEM::IPairingService* ble_pairing_service_ = nullptr;

    /**
     * \brief Tracks the connection id of the last seen
     *        ble connection before moving to state kEcdheApproval.
     *        To be used when sending frames over ble if pairing via
     *        ble. Any disconnect during pairing via ble will abort
     *        the pairing attempt.
     */
    // There does not exist any "undefined connection id", we leave it uninitialized
    // and rely on our state to determine whether its valid.
    BLEM::ConnectionID ble_connection_id_;

    /**
     * \brief tracks which channel we are pairing over.
     */
    PairingChannel pairing_channel_ = PairingChannel::kUndefined;

    /**
     * \brief Tracks whether ECDHE approval has been received
     *        from remote client.
     */
    bool is_peer_approval_received_ = false;

    /**
     * \brief Tracks whether user approval has been received
     *        from IHU.
     */
    bool is_user_approval_received_ = false;

    /**
     * \brief Transaction ID generated specifically for
     *        voc frames received from the BLE pairing service.
     *        Can be used to check if a frame was received over BLE.
     *        There is some risk of collision but in theory they are unique.
     */
    //TODO: this is kinda hacky, once we do signal refactoring
    //      put the Source of the Signal into Signal which
    //      will at that point be a decoupled header.
    const fsm::CCMTransactionId kBlePairingServiceTransactionId_;

    /**
     * \brief Used to store the transction id used when responding to
     *        ECDHE Approval.
     */
    fsm::CCMTransactionId ecdhe_approval_transaction_id_;

    /**
     * \brief Used to store the transction id used when responding to
     *        final AES CSR.
     */
    fsm::CCMTransactionId finalize_pairing_transaction_id_;

    /**
      * Timeout for the entire pairing.
      */
    std::chrono::seconds visibility_timeout_ = kVisibility_timeout_default;

    /**
      * Timeout for the VSDP pairing.
      */
    std::chrono::seconds remote_timeout_ = kRemote_timeout_default;

    /**
     * \brief Tracks the type of the pairing id in use for this pairing.
     */
    DevicePairingId::DevicePairingIdType pairing_id_type_ =
        DevicePairingId::DevicePairingIdType::kDevicePairingIdTypeVin;

    /**
     * \brief Tracks the pairing id in use for this pairing.
     */
    // TODO: read from VC
    std::string pairing_id_ = "this is not a VIN";

    /**
     * \brief holds the session id for this pairing.
     */
    std::string pairing_session_id_;

    /**
     * \brief Holds the actor certificate we will create for client in
     *        response to AES CSR ACTOR.
     */
    X509* client_actor_certificate_ = nullptr;

    /**
     * \brief Holds the nearfield certificate we will create for client in
     *        response to AES CSR NEARFIELD.
     */
    X509* client_nearfield_certificate_ = nullptr;

    /**
      * Transaction id of the upload request. Stored for future use in commands like stop in case of timeout
      */
    fsm::CCMTransactionId device_pairing_transaction_id_;

    /**
      * IHU should be periodically updated about the progress of pairing. This value is
      * used as an interval for notifications
      */
    const std::chrono::seconds kPeriodic_tick_ = std::chrono::seconds(5);

    /**
      * The id of a timeout for periodic tick towards IHU
      */
    fsm::TimeoutTransactionId kPeriodic_tick_id_;

    /**
      * The id of a timeout for remote visibility
      */
    fsm::TimeoutTransactionId remote_visibility_id_;

    /**
     * \brief Callback for BLE Manager pairing service.
     *        Any disconnect after a pairing via BLE has started
     *        will cause the pairing to be aborted.
     *
     * \param[in] connection_id Id of the connection for which the state has changed.
     * \param[in] connection_state New state of the connection.
     */
    void OnBleManagerConnectionCallback(BLEM::ConnectionID connection_id,
                                        BLEM::ConnectionState connection_state);

    /**
     * \brief Callback for BLE manager pairing service.
     *        Will convert the recevied frame to a
     *        Signal and put it on this transactions
     *        queue.
     *
     * \param[in] connection_id Id of the connection the frame was received in.
     * \param[in] frame the recevied frame.
     */
    void OnBleManagerFrame(BLEM::ConnectionID connection_id,
                           BLEM::IVoCBaseFrame* frame);

    /**
     * \brief Handle PairingVisibilityRequest signal.
     *        Expected in state kNew.
     *        Will request local configuration for timeouts
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandlePairingVisibilityRequest(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle HandlePairingVisibilityTimeouts configs.
     *        Will start advertising over BLE, start the pairing
     *        timers and go to state kAdvertising.
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandlePairingVisibilityTimeouts(void);

    /**
     * \brief Handle CA-DPA-002 signal.
     *        Expected in all states expect kNew and kDone.
     *        If signal indicates that VSDP found a match
     *        the pairing session id will be stored.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleCaDpa002(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle CA-DPA-020 signal.
     *        Expected in states kAdvertising, kEcdheApproval and
     *        kCreateClientCertificates.
     *        Simply unwraps frame and forwards to #HandleVocFrame
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleCaDpa020(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle a received frame.
     *        Expected in states kAdvertising, kEcdheApproval and
     *        kCreateClientCertificates.
     *        Behavior depends on state and Frame in signal,
     *        ECDHE and certificate exchange actions will be triggered.
     *
     * \param[in] frame The frame to  handle.
     * \param[in] transaction_id Transaction ID to use when replying if
     *                           pairing over VSDP.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleVocFrame(std::shared_ptr<fsm::Signal> frame, fsm::CCMTransactionId transaction_id);

    /**
     * \brief Handle a BLE connection callback signal.
     *        Expected in states kAdvertising, kEcdheApproval and
     *        kCreateClientCertificates.
     *        Behavior depends on state,
     *        TODO (Dariusz) - complete description.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleBleConnection(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle CA-DPA-002 signal.
     *        Expected in state kEcdheApproval.
     *        Will trigger sending of ECDHE Approval to mapp.
     *        If approval received from mapp will trigger move
     *        state kCreateClientCertificates.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleCodeConfirmationByUser(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle timeout signal.
     *        Expected in all states expect kNew and kDone.
     *        If remote visibility timer and state kAdvertising
     *        will trigger sending of CA-DPA-001.
     *        If pairing timeout will abort pairing attempt.
     *        Otherwise will simply trigger tick notification
     *        to IHU.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleTimeout(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle CA-CAT-002 signal.
     *        Expected in state kFinalizePairing.
     *        If success will trigger state change to kDone.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleCaCat002(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle entering the state kFinalizePairing.
     *        Will trigger:
     *        - sending of vehicle certificates and BDAK to device
     *        - notification to IHU
     *        - storing of device certificates
     *        - sending of CA-CAT-002 to VSDP
     *
     * \param[in] old_state The old state.
     * \param[in] new_state The new state.
     *
     * \return None.
     */
    void FinalizePairing(State old_state, State new_state);

    /**
     * \brief Handle entering the state kDone.
     *        Will stop the transaction.
     *
     * \param[in] old_state The old state.
     * \param[in] new_state The new state.
     *
     * \return None.
     */
    void Done(State old_state, State new_state);

    /**
     * \brief Handle ECDHE request.
     *
     * \param[in] frame The Ecdhe request voc frame.
     * \param[in] transaction_id The transaction id to use when responding.
     *
     * \return True on success, false on failure.
     */
    bool HandleEcdheRequest(std::shared_ptr<EcdheRequestVocFrame> frame,
                            fsm::CCMTransactionId transaction_id);

    /**
     * \brief ECDHEGeneration.
     *        Generate ECDHE Keys and shared secret.
     *
     * \param[in] peer_key The remote peer key.
     *
     * \return True on success, false on failure.
     */
     bool EcdheCalculateSecret(EVP_PKEY* peer_key);

    /**
     * \brief Get_OnScreen_Secret.
     *        Compute and save OnScreen 6 digits secret.
     *
     * \param[out] secret OnScreen 6 digits secret will be written hete on success.
     *
     * \return True on success, false on failure.
     */
     bool GetOnScreenSecret(std::string& secret);

     /**
      * \brief Publish a CA-DPA-021 to the car access
      * device pairing topic.
      *
      * \param[in] message Message to publish.
      *
      * \return True on success, false on failure.
      */
     bool PublishToDevicePairingTopic(std::shared_ptr<CaDpa021Signal> message);

     /**
      * \brief Sends a voc frame over the BLE pairing service.
      *
      * \param[in] frame The frame to send.
      *
      * \return True on success, false on failure.
      */
     bool SendToBlePairingService(const VocFrameEncodableInterface& frame);

     /**
      * \brief Sends a voc frame to the remote client, either over BLE or MQTT.
      *        The method used will depend on which channel pairing was initiated
      *        over.
      *
      * \param[in] frame The frame to send.
      * \param[in] ca_dpa_021 CCM wrapper to send frame in.
      *                       Used when sending over VSDP.
      *                       Header values are expected to be populated (frame will be set).
      *
      * \return True on success, false on failure.
      */
     bool SendToRemoteClient(const VocFrameEncodableInterface& frame,
                             std::shared_ptr<CaDpa021Signal> ca_dpa_021);

     /**
      * \brief Handle the voc frame AES CSR ACTOR.
      *
      * \param[in] frame The frame
      * \param[in] transaction_id Id to use when responding.
      *
      * \return True on success, false on failure.
      */
     bool HandleAesCsrActor(std::shared_ptr<AesCsrActorVocFrame> frame,
                            fsm::CCMTransactionId transaction_id);

     /**
      * \brief Handle the voc frame AES CSR NEARFIELD.
      *
      * \param[in] frame The frame
      * \param[in] transaction_id Id to use when responding.
      *
      * \return True on success, false on failure.
      */
     bool HandleAesCsrNearfield(std::shared_ptr<AesCsrNearfieldVocFrame> frame,
                                fsm::CCMTransactionId transaction_id);
     /**
      * \brief Sends CA-DPA-001
      *
      * \param[in] stop If true a stop signal will be sent.
      *
      * \return true if sending succeeded, false otherwise.
      */
     bool SendCaDpa001(bool stop = false);

     /**
      * Handles transaction abortion
      *
      * \param[in] reason Textual description of abort reason.
      * \param[in] abnormal indicates whether we are terminating
      *                     due to a abnormal reason.
      */
     void Terminate(std::string reason, bool abnormal);
};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_DEVICE_PAIRING_TRANSACTION_H_

/** \}    end of addtogroup */
