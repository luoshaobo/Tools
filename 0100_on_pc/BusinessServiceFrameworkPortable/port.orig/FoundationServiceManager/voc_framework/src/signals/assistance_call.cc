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
 *  \file     assistance_call.cc
 *  \brief    Implementation of the AssistanceCallPayload functional discovery resource.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/assistance_call.h"

//TODO: finish signal implementation, currently only partial for test purposes

namespace fsm
{


AssistanceCallPayload::AddressType AssistanceCallPayload::GetCallCenterAddressType()
{
    return call_center_address_type_;
}

bool AssistanceCallPayload::GetCallCenterAddress(std::string &address)
{
    bool return_value = true;

    if (call_center_address_type_ != kNone)
    {
        address = call_center_address_;
    }
    else
    {
        return_value = false;
    }

    return return_value;
}

AssistanceCallPayload::AddressType AssistanceCallPayload::GetThizAddressType()
{
    return thiz_address_type_;
}

bool AssistanceCallPayload::GetThizAddress(std::string &address)
{
    bool return_value = true;

    if (thiz_address_type_ != kNone)
    {
        address = thiz_address_;
    }
    else
    {
        return_value = false;
    }

    return return_value;
}


bool AssistanceCallPayload::Pack(ast_AssistanceCall& assistance_call) const
{
    bool return_value = true;

#ifdef UNIT_TESTS

    // During unit test we want to run both encode and decode.
    // As this payload is only expected to be decoded in production
    // the encode support is under ifdef.

    //Set call center address
    if (call_center_address_type_ != kNone)
    {
        ast_Resources* resources = new ast_Resources();

        if (resources)
        {
            ast_Address__C* call_center_address = CreateAstAddress(call_center_address_type_, call_center_address_);
            ast_Address__C* thiz_address = CreateAstAddress(thiz_address_type_, thiz_address_);

            // set resources
            if (call_center_address && thiz_address)
            {
                resources->thiz = thiz_address;
                resources->callCenterSettings = call_center_address;
                assistance_call.resources = resources;
                assistance_call.optional.setPresence(asn_ast_resources);
            }
            else
            {
                //if we failed to create addresses, release resources
                if (thiz_address)
                {
                    delete thiz_address;
                }

                if (call_center_address)
                {
                    delete call_center_address;
                }

                delete resources;

                return_value = false;
            }
        }
        else
        {
            return_value = false;
        }

    }
#else
    return_value = false;
#endif
    return return_value;
}


bool AssistanceCallPayload::ParseAstAddress(ast_Address__C* address, AddressType& type, std::string& value)
{
    bool result = false;

    if (address)
    {
        asnMAXUINT address_length = 0;
        asnbyte* address_content = nullptr;

        if (address->alternative == asn_ast_instanceId)
        {
            asncast_instanceId* instance_id = dynamic_cast<asncast_instanceId*>(address);
            if (instance_id)
            {
                type = kInstanceId;
                instance_id->instanceId.getOctetString(&address_length, &address_content);
                result = true;
            }
        }
        else if (address->alternative == asn_ast_Address__uri)
        {
            asncast_Address__uri* uri = dynamic_cast<asncast_Address__uri*>(address);
            if (uri)
            {
                type = kUri;
                uri->uri.getOctetString(&address_length, &address_content);
                result = true;
            }
        }

        //if we have retrieved values correctly
        result = result && address_content && (address_length > 0);

        //assign address
        if (result)
        {
            value.assign(reinterpret_cast<char*>(address_content), address_length);
        }

    }

    return result;
}

bool AssistanceCallPayload::Populate(const ast_AssistanceCall& assistance_call)
{
    bool return_value = true;

    // marben not const correct
    if (const_cast<ast_AssistanceCall&>(assistance_call).optional.getPresence(asn_ast_resources))
    {
        //asn_ast_Address__uri
         ast_Address__C* thiz_address = assistance_call.resources->thiz;
         ast_Address__C* call_center_address = assistance_call.resources->callCenterSettings;

         if (thiz_address && call_center_address)
         {
             return_value = return_value && ParseAstAddress(thiz_address, thiz_address_type_, thiz_address_);
             return_value = return_value && ParseAstAddress(call_center_address, call_center_address_type_, call_center_address_);

         }
    }

    return return_value;
}




#ifdef UNIT_TESTS

// During unit test we want to run both encode and decode.
// As this payload is only expected to be decoded in production
// the encode support is under ifdef.

void AssistanceCallPayload::SetCallCenterAddress(AddressType address_type, std::string address)
{
    call_center_address_type_ = address_type;
    call_center_address_ = address;
}

void AssistanceCallPayload::SetThizAddress(AddressType address_type, std::string address)
{
    thiz_address_type_ = address_type;
    thiz_address_ = address;
}

ast_Address__C* AssistanceCallPayload::CreateAstAddress(const AddressType type, const std::string address) const
{
    ast_Address__C* return_address = nullptr;

    if (type == kUri)
    {
        asncast_Address__uri *return_val = new asncast_Address__uri();
        if (return_val)
        {
            return_val->uri.copyOctetString(address.size(), (unsigned char*)address.c_str());
            return_address = return_val;
        }
    }
    else if (type == kInstanceId)
    {
        asncast_instanceId* return_val = new asncast_instanceId();
        if (return_val)
        {
            return_val->instanceId.copyOctetString(16, (unsigned char*)address.c_str());
            return_address = return_val;
        }
    }

    return return_address;
}

#endif
} // namespace fsm

/** \}    end of addtogroup */
