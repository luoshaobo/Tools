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
 *  \file     basic_car_control.cc
 *  \brief    Implementation of the BasicCarControlPayload functional discovery resource.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/basic_car_control.h"

//TODO: finish signal implementation, currently only partial for test purposes

namespace volvo_on_call
{

BasicCarControlPayload::AddressType BasicCarControlPayload::GetAddressType()
{
    return address_type_;
}

bool BasicCarControlPayload::GetAddressUri(std::string &uri)
{
    bool return_value = true;

    if (address_type_ == kUri)
    {
        uri = address_uri_;
    }
    else
    {
        return_value = false;
    }

    return return_value;
}

bool BasicCarControlPayload::Pack(bcc_BasicCarControl& basic_car_control) const
{
    bool return_value = true;

#ifdef UNIT_TESTS

    // During unit test we want to run both encode and decode.
    // As this payload is only expected to be decoded in production
    // the encode support is under ifdef.

    if (address_type_ != kNone)
    {
        asncbcc_uri* address = new asncbcc_uri();
        bcc_Resources* resources = new bcc_Resources();

        if (address && resources)
        {
            if (address_type_ == kUri)
            {
                address->uri.copyOctetString(7, (unsigned char*)address_uri_.c_str());
            }

            resources->thiz = address;

            basic_car_control.resources = resources;
            basic_car_control.optional.setPresence(asn_bcc_resources);
        }
        else
        {
            if (address)
            {
                delete address;
            }
            if (resources)
            {
                delete resources;
            }

            return_value = false;
        }
    }

#else
    return_value = false;
#endif
    return return_value;
}

bool BasicCarControlPayload::Populate(const bcc_BasicCarControl& basic_car_control)
{
    bool return_value = true;

    // marben not onst correct
    if (const_cast<bcc_BasicCarControl&>(basic_car_control).optional.getPresence(asn_bcc_resources))
    {
        asncbcc_uri* address_uri = dynamic_cast<asncbcc_uri*>(basic_car_control.resources->thiz);

        if (address_uri)
        {
            asnMAXUINT uri_length = 0;
            asnbyte* uri = nullptr;

            address_uri->uri.getOctetString(&uri_length, &uri);

            if (uri && uri_length > 0)
            {
                address_uri_.assign(reinterpret_cast<char*>(uri), uri_length);
                address_type_ = kUri;
            }
            else
            {
                return_value = false;
            }
        }
        else
        {
            return_value = false;
        }
    }

    return return_value;
}


#ifdef UNIT_TESTS

// During unit test we want to run both encode and decode.
// As this payload is only expected to be decoded in production
// the encode support is under ifdef.

void BasicCarControlPayload::SetAddressType(AddressType address_type)
{
    address_type_ = address_type;
}

void BasicCarControlPayload::SetAddressUri(std::string uri)
{
    address_uri_ = uri;
}

#endif
} // namespace volvo_on_call

/** \}    end of addtogroup */
