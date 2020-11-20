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
 *  \file     basic_car_control_signal.h
 *  \brief    Basic car control signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_BASIC_CAR_CONTROL_SIGNAL_H_
#define VOC_SIGNALS_BASIC_CAR_CONTROL_SIGNAL_H_

// fsm includes
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm_encodable.h"

// voc includes
#include "voc_framework/signals/signal.h"

// voc includes
#include "signals/signal_types.h"


// ans1c includes
#include "BasicCarControl.h"

#include <memory>
#include <vector>

namespace volvo_on_call
{

class BasicCarControlSignal: public fsm::Signal, public fsm::CCMDecodable, public fsm::CCMEncodable
{

 public:

    static const std::string oid;

    /**
    * \brief Types of addresses that can be present in BCC FUNC 002
    */
     enum  AddressType {None, Uri, InstanceId};

    ~BasicCarControlSignal ();

    /**
     * \brief Constructs empty signal from supplied data
     * \param[in] transaction_id  ccm header transactionId
     * \param[in] sequence_number ccm header sequenceNumber
     * \return shared pointer to created signal
     */
    static std::shared_ptr<BasicCarControlSignal> CreateBasicCarControlSignalFromData(fsm::CCMTransactionId& transaction_id,
                                                                            uint16_t sequence_number);
    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<fsm::Signal> CreateBasicCarControlSignal(ccm_Message* ccm,
                                                               fsm::TransactionId& transaction_id);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "BasicCarControlSignal"; };

    /**
    * \brief Provides information if CarLocator functionality is supported
    * \return True if supported, false otherwise.
    */
    bool CarLocatorSupported() {return clConfigSupported;};

    /**
    * \brief Returns information about the address type.
    * \return Returns the type of address specified. AddressType::None means
    *         that no address is specified
    */
    AddressType GetAddressType() {return addressType;};

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
    * \brief Sets car location support functionality
    */
    void SetCarLocatorSupport() {clConfigSupported=true;};

    /**
    * \brief Sets uri address
    * \param[in] address address in uri format
    */
    void SetAdressUri(std::string address);

    /**
    * \brief Sets InstanceId
    * \param[in] address address in UUID format
    */
    void SetAdressInstanceId(UUID address);

 protected:
    /**
     * \brief Provide the BCC_FUNC_002 oid.
     * \return the oid
     */
    const char* GetOid ();

    /**
     * \brief Pack BCC-FUNC-002 into ASN1C structure.
     * \return pointer to asn1c struct containing payload, caller must free.
     */
    void* GetPackedPayload ();

    /**
     * \brief Provides the ASN1C type descriptor for BCC-FUNC-002
     * \return pointer to ASN1C type decriptor, caller must not free
     */
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor ();

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    fs_VersionInfo GetContentVersion ();

 private:

    const long kSignalFlowVersion = 1;
    const long kSchemaVersion = 1;
    const long kPreliminaryVersion = 10;

    bool valid = false;

    /**
     * \brief OID message patters for different functions
     */
    //Car Locator
    static const std::string oid_function_CL;
    //Climate Calender
    static const std::string oid_function_CLIM;
    //Health Check
    static const std::string oid_function_HC;
    //Locking
    static const std::string oid_function_LOCK;
    //Remote Battery Management
    static const std::string oid_function_RBM;
    //Remote Dashboard
    static const std::string oid_function_RD;
    //Send Message To Car Central Stack Display
    static const std::string oid_function_SMES;
    //Send to Car Navigation
    static const std::string oid_function_STC;
    //Status
    static const std::string oid_function_STAT;

    /**
     * \brief Stores information if Car Locator is supported
     */
    bool clConfigSupported = false;

    /**
     * \brief Stores information if Car Locator is defined (true)
     *        or if only OID is set (false). Only meaningful when
     *        clConfigSupported set to true
     */
    bool clConfigDefined = false;

    /**
     * \brief Variables for storing different address variants
     */
    std::string address_uri;
    UUID address_instance;
    AddressType addressType = None;

    /**
     * \brief Populates the asn1c BasicCarControl struct with
     *        the Car Locator configuration
     * \param[in] asn1c_bcc_func_002 the struct to populate
     * \return true if operation succeeded, false otherwise
     */
    bool AddCarLocatorInfo(BasicCarControl* asn1c_bcc_func_002);

    /**
     * \brief Populates the asn1c BasicCarControl struct with
     *        the Climat Calendar configuration
     * \param[in] asn1c_bcc_func_002 nthe struct to unpack
     * \return true if operation succeeded, false otherwise
     */
    bool AddClimatCalenderInfo(BasicCarControl* asn1c_bcc_func_002);

    /**
     * \brief Constructs empty signal, must be populated via setters
     * \param[in] transaction_id  transaction id
     * \param[in] sequence_number sequence number
     */
    BasicCarControlSignal (fsm::CCMTransactionId& transaction_id,
                      uint16_t sequence_number);

    /**
     * \brief Contructs a signal populated from a ccm_Message
     * \param[in] ccm            encoded ccm message
     * \param[in] transaction_id transaction id
     *
     */
    BasicCarControlSignal (ccm_Message* ccm,
                      fsm::TransactionId& transaction_id);


    /**
     * \brief unpacks a asn1c BasicCarControl struct and populates this
     *        message with the contents
     * \param[in] asn1c_bcc_func_002 the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(BasicCarControl* asn1c_bcc_func_002);

    /**
     * \brief updates this signal with information obtained from
     *        a function defined by an OID
     * \param[in] asn1c_oid OID from Functions
     * \return true if handled properly, false if format unknown and message discarded
     */
    bool HandleOID(OBJECT_IDENTIFIER_t *asn1c_oid);

};

} // namespace

#endif //VOC_SIGNALS_BASIC_CAR_CONTROL_SIGNAL_H_

/** \}    end of addtogroup */
