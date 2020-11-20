/**
 * Copyright (C) 2016, 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     basic_car_control.h
 *  \brief    Implementation of the BasicCarControlPayload functional discovery resource.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_BASIC_CAR_CONTROL_H_
#define VOC_SIGNALS_BASIC_CAR_CONTROL_H_

//voc_framework
#include "voc_framework/signals/marben_payload.h"

//TODO: git path etc etc
#include "bcctype.h"

#include "signal_types.h"

namespace volvo_on_call
{

/**
 * \brief Implementation of the BasicCarControlPayload functional discovery resource.
 */
class BasicCarControlPayload : public fsm::MarbenPayload<bcc_BasicCarControl>
{

 public:

    /**
     * \brief Types of addresses that can be present in BCC FUNC 002
     */
    enum  AddressType {kNone, kUri, kInstanceId};

    /**
     * \brief Constructs a BasicCarControlPayload paylaod.
     */
    BasicCarControlPayload() : MarbenPayload("1.3.6.1.4.1.37916.3.6.2.0",
                                      kBasicCarControl,
                                      kSchemaVersion,
                                      kPreliminaryVersion,
                                      kSignalFlowVersion)
    {}

#ifdef UNIT_TESTS
    BasicCarControlPayload(fs_Encoding encoding) : MarbenPayload("1.3.6.1.4.1.37916.3.6.2.0",
                                                          kBasicCarControl,
                                                          kSchemaVersion,
                                                          kPreliminaryVersion,
                                                          kSignalFlowVersion,
                                                          encoding)
        {}
#endif

    /**
     * \brief Returns information about the address type.
     * \return Returns the type of address specified. AddressType::None means
     *         that no address is specified
     */
    AddressType GetAddressType();

    /**
     * \brief Returns uri address, if specified
     * \param[out] uri   address in uri format
     * \return True if uri address available,in which case uri will be populated.
     *         False otherwise, in which case uri will be intact.
     */
    bool GetAddressUri(std::string &uri);

    // ***
    // MarbenPAyload virtual functions implementation start.
    // ***

    /**
     * \brief Pack the contents of this BasicCarControlPayload into a asntype
     *        ready for encoding.
     *
     * \param[in/out] asn_type asntype to populate.
     *
     * \return False on failure, true on success.
     */
    bool Pack(bcc_BasicCarControl& basic_car_control) const;

    /**
     * \brief Populate this BasicCarControlPayload from asn_type
     */
    bool Populate(const bcc_BasicCarControl& basic_car_control);

    // ***
    // MarbenPAyload virtual functions implementation end.
    // ***

 private:


    static const long kSchemaVersion = 1;
    static const long kPreliminaryVersion = 10;
    static const long kSignalFlowVersion = 1;

    /**
     * \brief Variables for storing different address variants
     */
    std::string address_uri_;
    AddressType address_type_ = kNone;


#ifdef UNIT_TESTS

    // During unit test we want to run both encode and decode.
    // As this payload is only expected to be decoded in production
    // the encode support is under ifdef.

 public:
    void SetAddressType(AddressType address_type);
    void SetAddressUri(std::string uri);

#endif

 };

} // namespace volvo_on_call

#endif //VOC_SIGNALS_BASIC_CAR_CONTROL_H_

/** \}    end of addtogroup */
