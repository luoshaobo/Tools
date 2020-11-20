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
 *  \file     car_access_signal.h
 *  \brief    car access signal
 *  \author   Gustav Evertsson
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_CA_FUNC_002_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_CA_FUNC_002_SIGNAL_H_

#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/signal.h"

// ans1c includes
#include "CarAccess.h"

#include <memory>
#include <vector>

namespace fsm
{

class CarAccessSignal: public Signal, public CCMDecodable
#ifdef UNIT_TESTS
, public CCMEncodable
#endif
{

 public:
    static const std::string oid_;

    /**
     * \brief ServiceInfo. SubId is currently not included.
     */
    typedef struct {
        std::string oid;

    } MqttServiceInfo;

    /**
     * \brief Topics with priority and list of services used for this topic.
     */
    typedef struct {
        std::string topic;
        long int priority;
        std::vector<MqttServiceInfo> services;
    } MqttTopicInfo;

    /**
     * \brief Types of addresses that can be present in CA FUNC 002
     */
    enum AddressType {None, Uri, InstanceId};


    ~CarAccessSignal ();

    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<Signal> CreateCarAccessSignal(ccm_Message* ccm,
                                                         TransactionId& transaction_id);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "CarAccessSignal"; };


    /**
     * \brief Returns information about the address type.
     * \return Returns the type of address specified. AddressType::None means
     *         that no address is specified
     */
    AddressType GetAddressType() {return address_type_;};

    /**
     * \brief Returns uri address, if specified
     * \param[out] uri   address in uri format
     * \return True if uri address available,in which case uri will be populated.
     *         False otherwise, in which case uri will be intact.
     */
    bool GetAddressUri(std::string &uri);

    /**
     * \brief Returns instance id address, if specified
     * \param[out] instanceId   address in UUID format
     * \return True if UUID address available,in which case instanceId will be populated.
     *         False otherwise, in which case instanceId will be intact.
     */
    bool GetAddressInstanceId(UUID &instanceId);

    /**
     * \brief Returns the catalogue uplink topic string.
     * \return The Topic string
     */
    std::string GetCatalogueUplink();

    /**
     * \brief Returns the catalogue downlink topic string.
     * \return The Topic string
     */
    std::string GetCatalogueDownlink();

    /**
     * \brief Returns the delegate uplink topic string.
     * \return The Topic string
     */
    std::string GetDelegateUplink();

    /**
     * \brief Returns the delegate downlink topic string.
     * \return The Topic string
     */
    std::string GetDelegateDownlink();

    /**
     * \brief Returns the device pairing uplink topic string.
     * \return The Topic string
     */
    std::string GetDevicePairingUplink();

    /**
     * \brief Returns the device pairing downlink topic string.
     * \return The Topic string
     */
    std::string GetDevicePairingDownlink();

    /**
     * \brief Returns a list of topics information to be used to lookup OID<->Topic for
     *        uplink messages.
     * \return The list of Topics
     */
    std::shared_ptr<std::vector<CarAccessSignal::MqttTopicInfo> > GetUplinkTopics();

    /**
     * \brief Returns a list of topics information to be used to lookup OID<->Topic for
     *        downlink messages.
     * \return The list of Topics
     */
    std::shared_ptr<std::vector<CarAccessSignal::MqttTopicInfo> > GetDownlinkTopics();

 protected:
     /**
      * \brief Variables for storing different address variants
      */
     std::string address_uri_;
     UUID address_instance_;
     AddressType address_type_ = None;

     /**
      * \brief Variables for storing special topic strings.
      */
     std::string catalogue_uplink_topic_;
     std::string catalogue_downlink_topic_;
     std::string delegate_uplink_topic_;
     std::string delegate_downlink_topic_;
     std::string device_pairing_uplink_topic_;
     std::string device_pairing_downlink_topic_;

     /**
      * \brief Variables for storing topics information with topics to oid lookup
      *        information.
      */
     std::shared_ptr<std::vector<CarAccessSignal::MqttTopicInfo> > uplink_topics_;
     std::shared_ptr<std::vector<CarAccessSignal::MqttTopicInfo> > downlink_topics_;

     /**
      * \brief Signal flow version
      */
     const long kSignalFlowVersion = 1;

     /**
      * \brief Schema package version - major
      */
     const long kSchemaVersion = 1;

     /**
      * \brief Schema package version - minor
      */
     const long kPreliminaryVersion = 8;

     bool valid = false;

     /**
     * \brief Contructs a signal populated from a ccm_Message
     */
    CarAccessSignal (ccm_Message* ccm,
                      TransactionId& transaction_id);

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
      */
    fs_VersionInfo GetContentVersion ();

 private:

    /**
     * \brief Convert asn1c struct to MqttTopicInfo type.
     * \param[in] asn1_topic_info the struct to unpack
     * \param[in/out] topic_info the struct to populate
     */
    void Asn1TopicsInfo2MqttTopicsInfo(struct TopicInfo &asn1_topic_info, CarAccessSignal::MqttTopicInfo &topic_info);

    /**
     * \brief unpacks a asn1c CarAccess_t struct and populates this
     *        message with its contents
     * \param[in] asn1c_ca_func_002 the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(CarAccess_t* asn1c_ca_func_002);

#ifdef UNIT_TESTS

// During unit test we want to run both encode and decode.
// As this signal is only expected to be decoded in production
// the encode support is under ifdef.

 public:

    /**
     * \brief Constructs empty signal
     */
    CarAccessSignal (CCMTransactionId& transaction_id,
                     uint16_t sequence_number);

    /**
     * \brief Sets uri address
     * \param[in] uri   address in uri format
     */
    void SetAdressUri(std::string address);

    /**
     * \brief Sets InstanceId
     * \param[in] address   address in UUID format
     */
    void SetAddressInstanceId(UUID address);

    /**
     * \brief Set the catalogue uplink topic string.
     * \param[in] topic   The new topic string
     */
    void SetCatalogueUplink(std::string topic);

    /**
     * \brief Set the catalogue downlink topic string.
     * \param[in] topic   The new topic string
     */
    void SetCatalogueDownlink(std::string topic);

    /**
     * \brief Set the delegate uplink topic string.
     * \param[in] topic   The new topic string
     */
    void SetDelegateUplink(std::string topic);

    /**
     * \brief Set the delegate downlink topic string.
     * \param[in] topic   The new topic string
     */
    void SetDelegateDownlink(std::string topic);

    /**
     * \brief Set the device paring uplink topic string.
     * \param[in] topic   The new topic string
     */
    void SetDevicePairingUplink(std::string topic);

    /**
     * \brief Add a topic to the list of uplink topics.
     * \param[in] topic The topic to add.
     */
    void AddUplinkTopic(MqttTopicInfo topic);

    /**
     * \brief Add a topic to the list of downlink topics.
     * \param[in] topic The topic to add.
     */
    void AddDownlinkTopic(MqttTopicInfo topic);

    /**
     * \brief Set the device paring downlink topic string.
     * \param[in] topic   The new topic string
     */
    void SetDevicePairingDownlink(std::string topic);

    // ***
    // CCMEncodable virtual functions implementation start.
    // ***

    /**
     * \brief Provide the ca_func_002 oid.
     * \return the oid
     */
    const char* GetOid () {return oid_.c_str();};

 protected:

    /**
     * \brief Pack CA-FUNC-002 into ASN1C structure.
     * \return pointer to asn1c struct containing payload, caller must free.
     */
    void* GetPackedPayload ();

    /**
     * \brief Provides the ASN1C type descriptor for CA-FUNC-002
     * \return pointer to ASN1C type decriptor, caller must not free
     */
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor () {return &asn_DEF_CarAccess;};

    // ***
    // CCMEncodable virtual functions implementation stop.
    // ***

 private:
    void MqttTopicsInfo2Asn1TopicsInfo(fsm::CarAccessSignal::MqttTopicInfo &topic_info,
                                       struct TopicInfo &asn1_topic_info);

#endif //UNIT_TESTS
};
} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_CA_FUNC_002_SIGNAL_H_

/** \}    end of addtogroup */
