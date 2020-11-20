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
 *  \file     entry_point_signal.h
 *  \brief    EntryPoint signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_ENTRYPOINT_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_ENTRYPOINT_SIGNAL_H_

#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/signal.h"

// ans1c includes
#include "EntryPoint.h"

#include <memory>
#include <vector>

#ifdef VOC_TESTS
#include "voc_framework/signals/ccm_encodable.h"
#endif


namespace fsm
{

/**
 * \brief EntryPointSignal class handles access to EntryPoint resource encoded as CCM
 * */
class EntryPointSignal: public Signal, public CCMDecodable
#ifdef VOC_TESTS
, public CCMEncodable
#endif
{

 public:
     static const std::string oid_;


     /**
      * \brief Gets URI to the cloud resource, can be used for refreshing the resource. Called thiz in
      *        the corresponding ASN1 specification
      * \return uri of the entry point
      */
     std::string GetUri();

     /**
      * \brief Gets the version of the specification, which the service implements
      * \return specification version
      */
     long GetSpecificationVersion();

     /**
      * \brief Gets the version of the implementation
      * \return implementation version
      */
     long GetImplementationVersion();

     /**
      * \brief Gets URI to the feature list
      * \return URI of the feature list
      */
     std::string GetClientUri();

     /**
      * \brief Gets the host part of the feature list URI
      * \return host of the feature list
      */
     std::string GetHost();

     /**
      * \brief Gets the port part of the feature list URI
      * \return port of the feature list URI
      */
     long GetPort();

     /**
      * \brief Gets the signal service URI
      * \return signal service URI, empty string if undefined
      */
     std::string GetSignalServiceUri();

     /**
      * \brief Gets the list of country codes entry point is valid for
      * \return list of country code, empty vector if undefined
      */
     std::vector<std::string> GetValidLocations();

     /**
      * \brief Destructor
      */
     ~EntryPointSignal ();

    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<Signal> CreateEntryPointSignal(ccm_Message* ccm, TransactionId& transaction_id);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "EntryPointSignal"; };

 protected:

     /**
      * \brief Variable for storing uri to the corresponding cloud resource
      */
    std::string uri_;

    /**
     * \brief Variable for storing client uri
     */
   std::string client_uri_;

   /**
    * \brief Variable for storing specification version
    */
   long specification_version_;

   /**
    * \brief Variable for storing implementation version
    */
   long implementation_version_;

   /**
    * \brief Variable for storing host
    */
   std::string host_;

   /**
    * \brief Variable for storing port
    */
   long port_;

   /**
    * \brief Variable for storing signal service URI
    */
   std::string signal_service_uri_;

    /**
     * \brief Variable for the list of valid locations
     */
    std::vector<std::string> valid_locations_;

    const long kSignalFlowVersion = 1;
    const long kSchemaVersion = 1;
    const long kPreliminaryVersion = 8;

    bool valid = false;

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    fs_VersionInfo GetContentVersion ();

  private:

    /**
     * \brief Contructs a signal populated from a ccm_Message
     */
    EntryPointSignal (ccm_Message* ccm, TransactionId& transaction_id);

    /**
     * \brief unpacks a asn1c EntryPoint_t struct and populates this
     *        message with its contents
     * \param[in] asn1c_EntryPoint the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(EntryPoint_t* asn1c_EntryPoint);

#ifdef VOC_TESTS

    // During VOC unit test we want to run both encode and decode
    // as this signal is only expected to be decoded in production
    // the encode support is under ifdef.

 public:

    /**
     * \brief Constructs empty signal
     * \param[in] transaction_id ccm header transactionId
     * \param[in] message_id ccm header message_id
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    EntryPointSignal(fsm::CCMTransactionId& transaction_id,
                   uint16_t message_id);

    /**
     * \brief Sets parameters
     * \param[in] uri uri to set
     * \param[in] spec_version specification version to set
     * \param[in] impl_version implementation version to set
     * \param[in] client_uri client_uri to set
     * \param[in] host host to set
     * \param[in] port port to set
     * \param[in] signaling_service_uri signaling service uri to set (empty string for undefined)
     * \param[in] valid_locations list of valid locations to set (empty vector for undefined)
     */
    void SetParameters(std::string uri, long spec_version, long impl_version,
                       std::string client_uri, std::string host, long port,
                       std::string signaling_service_uri, std::vector<std::string> valid_locations );


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

#endif //VOC_FRAMEWORK_SIGNALS_ENTRYPOINT_SIGNAL_H_

/** \}    end of addtogroup */
