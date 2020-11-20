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
 *  \file     expiry_notification_signal.h
 *  \brief    Expiry notification signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_EXPIRY_NOTIFICATION_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_EXPIRY_NOTIFICATION_SIGNAL_H_

#include <memory>
#include <vector>

#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/signal.h"
#ifdef VOC_TESTS
#include "voc_framework/signals/ccm_encodable.h"
#endif

#include "ExpiryNotification.h"

namespace fsm
{

const int kExpiryNotificationSignal = 1;
/**
 * \brief ExpiryNotificationSignal class is used for processing Feature list received as CCM
 */
class ExpiryNotificationSignal: public Signal, public CCMDecodable
#ifdef VOC_TESTS
, public CCMEncodable
#endif
{

 public:
     static const std::string oid_;

     /**
      * \brief ResourceInfo structure, contains data about individual resource on the list of expired resources.
      */
     struct ResourceInfo
     {
         std::string name;          ///< Resource name.
         std::string parameters;    ///< Parameters
         bool reconnect;            ///< Eviction type, true for reconnect, false for invalidate
     };

     /**
      * \brief Gets list of resources
      * \return vector containing all resources in ResourceInfo format, empty vector if no resources defined
      */
     std::vector<ResourceInfo> GetResources();


     /**
      * \brief Destructor
      */
     ~ExpiryNotificationSignal ();

    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<Signal> CreateExpiryNotificationSignal(ccm_Message* ccm, TransactionId& transaction_id);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "ExpiryNotificationSignal"; };

 protected:


    /**
     * \brief Variable for the list of features
     */
    std::vector<ResourceInfo> resources_;

    const long kSignalFlowVersion = 1;
    const long kSchemaVersion = 1;
    const long kPreliminaryVersion = 8;

    bool valid_ = false;

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    fs_VersionInfo GetContentVersion ();

  private:

    /**
     * \brief Contructs a signal populated from a ccm_Message
     */
    ExpiryNotificationSignal (ccm_Message* ccm, TransactionId& transaction_id);

    /**
     * \brief unpacks a asn1c Features_t struct and populates this
     *        message with its contents
     * \param[in] asn1c_Features the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(ExpiryNotification_t* asn1c_ExpiryNotification);


#ifdef VOC_TESTS

    // During VOC unit test we want to run both encode and decode
    // as this signal is only expected to be decoded in production
    // the encode support is under ifdef.

 public:
    /**
     * \brief Constructs empty signal
     */
    ExpiryNotificationSignal(fsm::CCMTransactionId& transaction_id,
                   uint16_t message_id);

    /**
     * \brief Sets URI to the cloud resource
     * \param[in] uri uri to set
     */
    void SetUri(const std::string& uri);

    /**
     * \brief Adds a resource to the resource list
     * \param[in] resource_info resource to add
     */
    void AddResource(const ResourceInfo& resource_info);
    // ***
    // CCMEncodable virtual functions implementation start.
    // ***

    /**
     * \brief Gets the oid of the payload content.
     * \return the oid.
     */

    const char* GetOid();

 protected:

    /**
     * \brief Pack CCM payload into ASN1C structure, used while encoding payload
     * \return pointer to asn1c struct containing payload, caller must free.
     */
    void* GetPackedPayload();

    /**
     * \brief Provides the ASN1C type descriptor for the CCM payload
     * \return pointer to ASN1C type decriptor, caller must not free
     */
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor();

    // ***
    // CCMEncodable virtual functions implementation stop.
    // ***

#endif
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_EXPIRY_NOTIFICATION_SIGNAL_H_

/** \}    end of addtogroup */
