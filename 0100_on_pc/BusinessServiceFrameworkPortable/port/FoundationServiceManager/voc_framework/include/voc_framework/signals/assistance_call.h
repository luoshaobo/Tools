/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     assistance_call.h
 *  \brief    Implementation of the AssistanceCallPayload functional discovery resource.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_ASSISTANCE_CALL_H_
#define VOC_FRAMEWORK_SIGNALS_ASSISTANCE_CALL_H_

//voc_framework
#include "voc_framework/signals/marben_payload.h"
#include "voc_framework/signals/signal.h"


#include "asttype.h"


namespace fsm
{

static std::string kAssistanceCallOid = "1.3.6.1.4.1.37916.3.5.2.0";

/**
 * \brief Implementation of the AssistanceCallPayload functional discovery resource.
 */
class AssistanceCallPayload : public fsm::MarbenPayload<ast_AssistanceCall>
{

 public:



    /**
     * \brief Types of addresses that can be present in AssistanceCall
     */
    enum  AddressType {kNone, kUri, kInstanceId};

    /**
     * \brief Constructs a AssistanceCallPayload payload.
     */
    AssistanceCallPayload() : MarbenPayload(kAssistanceCallOid,
                                      Signal::kAssistanceCall,
                                      kSchemaVersion,
                                      kPreliminaryVersion,
                                      kSignalFlowVersion)
    {}

#ifdef UNIT_TESTS
    AssistanceCallPayload(fs_Encoding encoding) : MarbenPayload(kAssistanceCallOid,
                                                          Signal::kAssistanceCall,
                                                          kSchemaVersion,
                                                          kPreliminaryVersion,
                                                          kSignalFlowVersion,
                                                          encoding)
        {}
#endif

    /**
     * \brief Returns information about the address type of CallCenterAddress.
     * \return Returns the type of address specified. AddressType::kNone means
     *         that no address is specified
     */
    AddressType GetCallCenterAddressType();

    /**
     * \brief Returns CallCenterAddress address, if specified
     * \param[out] address   address in type specific format
     * \return True if address available,in which case address will be populated.
     *         False otherwise, in which case address will be intact.
     */
    bool GetCallCenterAddress(std::string &address);

    /**
     * \brief Returns information about the address type of Thiz.
     * \return Returns the type of address specified. AddressType::kNone means
     *         that no address is specified
     */
    AddressType GetThizAddressType();

    /**
     * \brief Returns Thiz address, if specified
     * \param[out] address   address in type specific format
     * \return True if address available,in which case address will be populated.
     *         False otherwise, in which case address will be intact.
     */
    bool GetThizAddress(std::string &address);

    // ***
    // MarbenPAyload virtual functions implementation start.
    // ***

    /**
     * \brief Pack the contents of this AssistanceCallPayload into a asntype
     *        ready for encoding.
     *
     * \param[in/out] assistance_call object asn object to to populate.
     *
     * \return False on failure, true on success.
     */
    bool Pack(ast_AssistanceCall& assistance_call) const;

    /**
     * \brief Populate AssistanceCallPayload from asntype
     *
     * \param[in/out] assistance_call asn object to populate from.
     *
     * \return False on failure, true on success.
     */
    bool Populate(const ast_AssistanceCall& assistance_call);

    // ***
    // MarbenPAyload virtual functions implementation end.
    // ***

 private:

    /**
     * \brief Schema versioning
     */
    static const long kSchemaVersion = 1;
    static const long kPreliminaryVersion = 13;
    static const long kSignalFlowVersion = 1;

    /**
     * \brief Variable for storing CallCenterSettings address value
     */
    std::string call_center_address_;

    /**
     * \brief Variable for storing CallCenterSettings address type
     */
    AddressType call_center_address_type_ = kNone;


    /**
     * \brief Variable for storing Thiz address value
     */
    std::string thiz_address_;

    /**
     * \brief Variable for storing Thiz address type
     */
    AddressType thiz_address_type_ = kNone;


    /**
     * \brief Parse ast_Address__C and retrieve type and address from it
     *
     * \param[in] address address for parsing
     * \param[out] type type of address encoding
     * \param[out] value value of the address
     *
     * \return False on failure, true on success.
     */
    bool ParseAstAddress(ast_Address__C* address, AddressType& type, std::string& value);


#ifdef UNIT_TESTS

    // During unit test we want to run both encode and decode.
    // As this payload is only expected to be decoded in production
    // the encode support is under ifdef.


    /**
     * \brief Create an appropriate ast_Address__C object based on the
     *        input description
     *
     * \param[in] type type of address encoding
     * \param[in] address value of the address
     *
     * \return nullptr on failure, ast_Address__C object on success.
     */
    ast_Address__C* CreateAstAddress(const AddressType type, const std::string address ) const;


 public:

    /**
     * \brief Set CallCenterAddress field
     *
     * \param[in] address_type type of address encoding
     * \param[in] address value of the address
     *
     * \return False on failure, true on success.
     */
    void SetCallCenterAddress(AddressType address_type, std::string address);

    /**
     * \brief Set Thiz field
     *
     * \param[in] address_type type of address encoding
     * \param[in] address value of the address
     *
     * \return False on failure, true on success.
     */
    void SetThizAddress(AddressType address_type, std::string address);


#endif

 };

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_ASSISTANCE_CALL_H_

/** \}    end of addtogroup */
