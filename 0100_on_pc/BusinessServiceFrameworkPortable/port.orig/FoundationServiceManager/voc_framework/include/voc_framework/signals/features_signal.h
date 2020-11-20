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
 *  \file     features_signal.h
 *  \brief    features signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_FEATURES_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_FEATURES_SIGNAL_H_

#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/signal.h"

// ans1c includes
#include "Features.h"

#include <memory>
#include <vector>

#ifdef VOC_TESTS
#include "voc_framework/signals/ccm_encodable.h"
#endif


namespace fsm
{

/**
 * \brief FeaturesSignal class is used for processing Feature list received as CCM
 */
class FeaturesSignal: public Signal, public CCMDecodable
#ifdef VOC_TESTS
, public CCMEncodable
#endif
{

 public:
     static const std::string oid_;

     /**
      * \brief FeatureInfo structure, contains data of individual features.
      */
     struct FeatureInfo
     {
         std::string name;
         bool enabled;
         bool visible;
         std::string uri;
         std::string icon;
         std::string description;
         std::vector<std::string> tags;

         bool operator==(const FeatureInfo& a) const;
     };

     /**
      * \brief Gets list of features
      * \return vector containing all features in FeatureInfo format, empty vector if no features defined
      */
     std::vector<FeatureInfo> GetFeatures();

     /**
      * \brief Gets uri to the cloud resource, can be used for refreshing the resource
      * \return uri of the feature list, empty string if undefined
      */
     std::string GetUri();

     /**
      * \brief Destructor
      */
     ~FeaturesSignal ();

    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<Signal> CreateFeaturesSignal(ccm_Message* ccm, TransactionId& transaction_id);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "FeaturesSignal"; };

 protected:

     /**
      * \brief Variable for storing uri to the corresponding cloud resource
      */
    std::string uri_;

    /**
     * \brief Variable for the list of features
     */
    std::vector<FeatureInfo> features_;

    const long kSignalFlowVersion = 1;
    const long kSchemaVersion = 1;
    const long kPreliminaryVersion = 8;

    bool valid = false;

    /**
     * \brief Contructs a signal populated from a ccm_Message
     */
    FeaturesSignal (ccm_Message* ccm, TransactionId& transaction_id);

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    fs_VersionInfo GetContentVersion ();

  private:

    /**
     * \brief unpacks a asn1c Features_t struct and populates this
     *        message with its contents
     * \param[in] asn1c_Features the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(Features_t* asn1c_Features);


#ifdef VOC_TESTS

    // During VOC unit test we want to run both encode and decode
    // as this signal is only expected to be decoded in production
    // the encode support is under ifdef.

 public:
    /**
     * \brief Constructs empty signal
     */
    FeaturesSignal(fsm::CCMTransactionId& transaction_id,
                   uint16_t message_id);

    /**
     * \brief Sets URI to the cloud resource
     * \param[in] uri uri to set
     */
    void SetUri(const std::string& uri);

    /**
     * \brief Adds a feature to the feature list
     * \param[in] feature_info feature to add
     */
    void AddFeature(const FeatureInfo& feature_info);

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

#endif //VOC_FRAMEWORK_SIGNALS_FEATURES_SIGNAL_H_

/** \}    end of addtogroup */
