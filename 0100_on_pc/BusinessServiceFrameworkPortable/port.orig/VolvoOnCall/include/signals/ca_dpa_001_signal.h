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
 *  \file     ca_dpa_001_signal.h
 *  \brief    ca_dpa_001 signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_CA_DPA_001_SIGNAL_H_
#define VOC_SIGNALS_CA_DPA_001_SIGNAL_H_

#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/signal.h"

#include "signals/device_pairing_id.h"

// ans1c includes
#include "CA-DPA-001.h"


namespace volvo_on_call
{
class CaDpa001Signal: public fsm::Signal, public DevicePairingId, public fsm::CCMEncodable
#ifdef VOC_TESTS
, public fsm::CCMDecodable
#endif
{

 public:

    /**
     * \brief The oid of this CCM.
     */
    static const std::string oid_;

    ~CaDpa001Signal ();

    /**
     * \brief Constructs empty signal
     */
    CaDpa001Signal(fsm::CCMTransactionId& transaction_id,
                   uint16_t message_id);


    // ***
    // Signal implementation start.
    // ***

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "CaDpa001Signal"; }

    // ***
    // Signal implementation end.
    // ***

    /**
     * \brief Set command to start
     */
    void SetCommandStart();

    /**
     * \brief Set command to stop
     */
    void SetCommandStop();

    /**
     * \brief Set the remote connection timeout (optional)
     *
     * \param[in] timeout remote connection timeout
     */
    void SetRemoteConnectionTimeout (struct tm timeout);

    /**
     * \brief Set the pairing timeout (optional)
     *
     * \param[in] timeout pairing timeout
     */
    void SetPairingTimeout (struct tm timeout);


    /**
     * \brief Set the vehicle info (optional)
     *
     * \param[in] vin   VIN of the car
     * \param[in] model model of the car.
     * \param[in] model_year model year of the car
     */
    void SetVehicleInfo (std::string vin, long model, long model_year);


    // ***
    // CCMEncodable virtual functions implementation start.
    // ***

    /**
     * \brief Gets the oid of the payload content.
     * \return the oid.
     */
    const char* GetOid() {return oid_.c_str();};


 protected:

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    fs_VersionInfo GetContentVersion()
    {
        fs_VersionInfo version_info;
        version_info.signal_flow_version = kSignalFlowVersion;
        version_info.schema_version = kSchemaVersion;
        version_info.preliminary_version = kPreliminaryVersion;
        return version_info;
    };

    /**
     * \brief Pack CCM payload into ASN1C structure, used while encoding payload
     * \return pointer to asn1c struct containing payload, caller must free.
     */
    void* GetPackedPayload();

    /**
     * \brief Provides the ASN1C type descriptor for the CCM payload
     * \return pointer to ASN1C type decriptor, caller must not free
     */
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor() {return &asn_DEF_CA_DPA_001;};

    // ***
    // CCMEncodable virtual functions implementation stop.
    // ***

 private:

    static const long kSignalFlowVersion = 1;
    static const long kSchemaVersion = 1;
    static const long kPreliminaryVersion = 8;

    /**
     * \brief Tracks if the signal is valid.
     */
    bool valid_ = false;

#ifdef VOC_TESTS
    //public for decodable test class
 public:
#endif

    /**
     * \brief Tracks the command that has been set.
     */
    enum Command
    {
        kCommandUndefined,
        kCommandStart,
        kCommandStop
    } command_ = kCommandUndefined;

    /**
     * \brief Remote connection timeout
     */
    struct tm remote_connection_timeout_ = {};

    /**
     * \brief Tracks if the remote connection timeout has been set
     */
    bool remote_connection_timeout_set_ = false;

    /**
     * \brief  Pairing connection timeout
     */
    struct tm pairing_timeout_ = {};

    /**
     * \brief Tracks if the pairing timeout has been set
     */
    bool pairing_timeout_set_ = false;

    /**
     * \brief vin, part of the vehicle info
     */
    long vin_ = 0;

    /**
     * \brief model, part of the vehicle info
     */
    long model_ = 0;

    /**
     * \brief model year, part of the vehicle info
     */
    long model_year_ = 0;

    /**
     * \brief Tracks if the vehicle info has been set
     */
    bool vehicle_info_set_ = false;

#ifdef VOC_TESTS

    // During VOC unit test we want to run both encode and decode.
    // As this signal is only expected to be encoded in production
    // the decode support is under ifdef.

 public:

    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<CaDpa001Signal> CreateCaDpa001Signal(ccm_Message* ccm,
                                                                fsm::TransactionId& transaction_id);

 private:

    /**
     * \brief Contructs a signal populated from a ccm_Message
     */
    CaDpa001Signal(ccm_Message* ccm,
                   fsm::TransactionId& transaction_id);

    /**
     * \brief unpacks a asn1c CA_DPA_001_t struct and populates this
     *        message with its contents
     * \param[in] asn1c_ca_dpa_001 the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(CA_DPA_001_t* asn1c_ca_dpa_001);


#endif

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_CA_DPA_001_SIGNAL_H_

/** \}    end of addtogroup */
