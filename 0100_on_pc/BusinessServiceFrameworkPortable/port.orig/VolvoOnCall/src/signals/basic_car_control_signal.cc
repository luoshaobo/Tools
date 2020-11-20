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
 *  \file     basic_car_control_signal.cc
 *  \brief    BCC_FUNC_002 signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */
#include "signals/basic_car_control_signal.h"
#include "signals/signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

// hack to make c++ compiler understand this asn1c type
// better solutions are welcome
typedef struct BasicCarControl::functions functions_t;

const std::string BasicCarControlSignal::oid = "1.3.6.1.4.1.37916.3.6.2.0.0.2";


/*
 * OID format for function specification:
 * 1.3.6.1.4.1.37916.3.6.y.m.m.m, where y is specific function and m.m.m is a message number.
o y = 1 for COM – Common ASN.1 types and generic messages
o y = 2 for FUNC – Functions
o y = 3 for CL – Car Locator
o y = 4 for CLIM – Climate Calender
o y = 5 for HC – Health Check
o y = 6 for LOCK – Locking
o y = 7 for RBM – Remote Battery Management
o y = 8 for RD – Remote Dashboard
o y = 9 for SMES – Send Message To Car Central Stack Display
o y = 10 for STC – Send to Car Navigation (TBC)
o y = 11 for STAT – Status (TBC)
*/
//Car Locator
const std::string BasicCarControlSignal::oid_function_CL   = "1.3.6.1.4.1.37916.3.6.3";
//Climate Calender
const std::string BasicCarControlSignal::oid_function_CLIM = "1.3.6.1.4.1.37916.3.6.4";
//Health Check
const std::string BasicCarControlSignal::oid_function_HC   = "1.3.6.1.4.1.37916.3.6.5";
//Locking
const std::string BasicCarControlSignal::oid_function_LOCK = "1.3.6.1.4.1.37916.3.6.6";
//Remote Battery Management
const std::string BasicCarControlSignal::oid_function_RBM  = "1.3.6.1.4.1.37916.3.6.7";
//Remote Dashboard
const std::string BasicCarControlSignal::oid_function_RD   = "1.3.6.1.4.1.37916.3.6.8";
//Send Message To Car Central Stack Display
const std::string BasicCarControlSignal::oid_function_SMES = "1.3.6.1.4.1.37916.3.6.9";
//Send to Car Navigation
const std::string BasicCarControlSignal::oid_function_STC  = "1.3.6.1.4.1.37916.3.6.10";
//Status
const std::string BasicCarControlSignal::oid_function_STAT = "1.3.6.1.4.1.37916.3.6.11";


std::shared_ptr<BasicCarControlSignal> BasicCarControlSignal::CreateBasicCarControlSignalFromData(
                                                          fsm::CCMTransactionId& transaction_id,
                                                          uint16_t sequence_number)
{
    BasicCarControlSignal* signal = new BasicCarControlSignal(transaction_id,
                                                    sequence_number);


    if (!signal->valid)
    {
        delete signal;
        return std::shared_ptr<BasicCarControlSignal>();
    }
    else
    {
        return std::shared_ptr<BasicCarControlSignal>(signal);
    }
}

std::shared_ptr<fsm::Signal> BasicCarControlSignal::CreateBasicCarControlSignal(
                                                          ccm_Message* ccm,
                                                          fsm::TransactionId& transaction_id)
{
    BasicCarControlSignal* signal = new BasicCarControlSignal(ccm, transaction_id);
    if (!signal->valid)
    {
        delete signal;
        return std::shared_ptr<BasicCarControlSignal>();
    }
    else
    {
        return std::shared_ptr<BasicCarControlSignal>(signal);
    }
}

BasicCarControlSignal::BasicCarControlSignal (fsm::CCMTransactionId& transaction_id,
                                    uint16_t sequence_number) : fsm::CCM::CCM(transaction_id,
                                                                              sequence_number),
                                                                fsm::Signal::Signal(transaction_id,
                                                                                    kBasicCarControlSignal)
{
    valid = true;
}



BasicCarControlSignal::BasicCarControlSignal (ccm_Message* ccm,
                                    fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                          fsm::Signal::Signal(transaction_id,
                                                                                              kBasicCarControlSignal)
{
    //asn1c struct to hold decoded data
    BasicCarControl_t* asn1c_basic_car_control = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_basic_car_control, &asn_DEF_BasicCarControl, GetContentVersion ());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_basic_car_control);
    }

    // free decoded asn1c struct
    asn_DEF_BasicCarControl.free_struct(&asn_DEF_BasicCarControl, asn1c_basic_car_control, 0);
}

BasicCarControlSignal::~BasicCarControlSignal ()
{
    // free any instance data that needs freeing
}


void BasicCarControlSignal::SetAdressUri(std::string address)
{
    address_uri = address;
    addressType = Uri;
}

void BasicCarControlSignal::SetAdressInstanceId(UUID address)
{
    memcpy(&address_instance, address, FS_UUID_LEN);// address_instance = address;
    addressType = InstanceId;
}


bool BasicCarControlSignal::GetAddressUri(std::string &uri)
{
    if (addressType != AddressType::Uri)
    {
        return false;
    }
    else
    {
        uri = address_uri;
        return true;
    }
}

bool BasicCarControlSignal::GetAddressInstanceId(UUID &instanceId)
{
    if (addressType != AddressType::InstanceId)
    {
        return false;
    }
    else
    {
        memcpy(instanceId, address_instance, FS_UUID_LEN);
        return true;
    }
}

/**************************************
 * virtual CCM function implementations
 **************************************/

const char* BasicCarControlSignal::GetOid ()
{
    return oid.c_str();
}

//This function will need to get refactored when the message is defined
bool BasicCarControlSignal::AddClimatCalenderInfo(BasicCarControl_t* asn1c_basic_car_control)
{
    return true;
}

//This function will need to get extended when the message numbers
//will be defined
bool BasicCarControlSignal::AddCarLocatorInfo(BasicCarControl_t* asn1c_basic_car_control)
{
    bool return_value = true;

    if (clConfigSupported)
    {
       Functions_t* function_ptr = reinterpret_cast<Functions_t*>(calloc (1, sizeof(Functions_t)));
       if (function_ptr)
       {
           if (!clConfigDefined)
           {
               fs_Oid fs_oid;
               if (!ccm_SetOid(&fs_oid, BasicCarControlSignal::oid_function_CL.c_str()))
               {
                   return_value = false;
               }

               function_ptr->present = Functions_PR::Functions_PR_oid;
               if (return_value && !FSOid2ASN1C(&fs_oid, &(function_ptr->choice.oid)))
               {
                   return_value = false;
               }
           }
           else
           {
               //TODO:specs not yet finalized
               //change to Functions_PR::Address_PR_instanceId when implementation
               //is provided
               function_ptr->present = Functions_PR::Functions_PR_NOTHING;
           }

           if (return_value)
           {
               if (0 != ASN_SEQUENCE_ADD(&(asn1c_basic_car_control->functions->list), function_ptr))
               {
                   return_value = false;
               }
           }

           if (!return_value)
           {
               free(function_ptr);
           }
       }
       else
       {
           return_value = false;
       }

    }

    return return_value;
}

void* BasicCarControlSignal::GetPackedPayload()
{

    // allocate the asn1c struct, caller shall free
    BasicCarControl_t* asn1c_basic_car_control = (BasicCarControl_t*) calloc(1, sizeof(BasicCarControl_t));
    if (asn1c_basic_car_control == NULL)
    {
        return NULL;
    }

    bool addressDecoded = true;
    asn_TYPE_descriptor_t* type_descriptor = GetPayloadTypeDescriptor();
    //Handle resources, currently only address type
    if (addressType != None)
    {
        asn1c_basic_car_control->resources = (Resources_t*) calloc(1, sizeof(Resources_t));
        if (asn1c_basic_car_control->resources == NULL)
        {
            addressDecoded = false;
        }
        else
        {
            switch (addressType)
            {
            case Uri:
                asn1c_basic_car_control->resources->thiz.present
                        = Address_PR::Address_PR_uri;
                if ( 0 != OCTET_STRING_fromBuf(&(asn1c_basic_car_control->resources->thiz.choice.uri),
                                               address_uri.c_str(), address_uri.length()) )
                {
                    addressDecoded = false;
                }

                break;

            case InstanceId:
                asn1c_basic_car_control->resources->thiz.present
                        = Address_PR::Address_PR_instanceId;
                if ( 0 != OCTET_STRING_fromBuf(&(asn1c_basic_car_control->resources->thiz.choice.instanceId),
                                               (char*)&address_instance, FS_UUID_LEN ) )
                {
                    addressDecoded = false;
                }

                break;

            default:
                //unsupported format, error
                addressDecoded = false;;
            }
        }
    }

    if (addressDecoded == false)
    {
        ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_basic_car_control));
        return NULL;
    }

    //Handle functions
    //allocate memory
    asn1c_basic_car_control->functions = (functions_t*) calloc(1, sizeof(functions_t));

    if (asn1c_basic_car_control->functions == NULL)
    {
        ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_basic_car_control));
        return NULL;
    }
    else
    {
        bool succeeded = true;
        //add functions one-by-one
        succeeded = succeeded && AddCarLocatorInfo(asn1c_basic_car_control);
        succeeded = succeeded && AddClimatCalenderInfo(asn1c_basic_car_control);
        if (!succeeded)
        {
            ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_basic_car_control));
            return NULL;
        }
    }

    return (void*) asn1c_basic_car_control;
}

asn_TYPE_descriptor_t* BasicCarControlSignal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_BasicCarControl;
}

fs_VersionInfo BasicCarControlSignal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
}


/**************************
 * private member functions
 **************************/

bool BasicCarControlSignal::HandleOID(OBJECT_IDENTIFIER_t *asn1c_oid)
{
    //retrieve the OID in the text format
    std::string oid;
    fs_Oid fs_oid;
    if (! ASN1C2FSOid(asn1c_oid, &fs_oid) )
    {
        //Can't retrieve OID, return false
        return false;
    }

    //Find matching function by matching the function OID base patterns

    //Car Locator
    if (0 == strncmp(oid_function_CL.c_str(), (const char*)(&fs_oid.oid), oid_function_CL.size()))
    {
        clConfigSupported = true;
        clConfigDefined = false;
    }
    //Climate Calender
    else if (0 == strncmp(oid_function_CLIM.c_str(), (const char*)(&fs_oid.oid), oid_function_CLIM.size()))
    {
        //provide logic for Climate Calender
    }
    //Health Check
    else if (0 == strncmp(oid_function_HC.c_str(), (const char*)(&fs_oid.oid), oid_function_HC.size()))
    {
        //provide logic for Health Check
    }
    //Locking
    else if (0 == strncmp(oid_function_LOCK.c_str(), (const char*)(&fs_oid.oid), oid_function_LOCK.size()))
    {
        //provide logic for Locking
    }
    //Remote Battery Management
    else if (0 == strncmp(oid_function_RBM.c_str(), (const char*)(&fs_oid.oid), oid_function_RBM.size()))
    {
        //provide logic for Remote Battery Management
    }
    //Remote Dashboard
    else if (0 == strncmp(oid_function_RD.c_str(), (const char*)(&fs_oid.oid), oid_function_RD.size()))
    {
        //provide logic for Remote Dashboard
    }
    //Send Message To Car Central Stack Display
    else if (0 == strncmp(oid_function_SMES.c_str(), (const char*)(&fs_oid.oid), oid_function_SMES.size()))
    {
        //provide logic for Send Message To Car Central Stack Display
    }
    //Send to Car Navigation
    else if (0 == strncmp(oid_function_STC.c_str(), (const char*)(&fs_oid.oid), oid_function_STC.size()))
    {
        //provide logic for Send to Car Navigation
    }
    //Status
    else if (0 == strncmp(oid_function_STAT.c_str(), (const char*)(&fs_oid.oid), oid_function_STAT.size()))
    {
        //provide logic for Status
    }
    else {
        //unknown OID, return false
        return false;
    }

    return true;
}

bool BasicCarControlSignal::UnpackPayload(BasicCarControl_t* asn1c_basic_car_control)
{
    //check if resources are available
    if (asn1c_basic_car_control->resources)
    {
        char* data = NULL;
        int length = 0;
        bool success = false;
        switch (asn1c_basic_car_control->resources->thiz.present)
        {
         case Address_PR::Address_PR_uri:
                           addressType = Uri;
                           data = (char*)asn1c_basic_car_control->resources->thiz.choice.uri.buf;
                           length = asn1c_basic_car_control->resources->thiz.choice.uri.size;
                           address_uri.assign(data, length);
                           break;

         case Address_PR::Address_PR_instanceId:
                          addressType = InstanceId;
                          success  = ASN1C2OctetString
                                          (&(asn1c_basic_car_control->resources->thiz.choice.instanceId),
                                          (char*)&address_instance, FS_UUID_LEN);
                          //if failed to decode the string (e.g. incorrect length) then exit
                          if (!success)
                          {
                              return false;
                          }
                          break;
         default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                 "Received unsupported address type in BasicCarControlSignal: %d\n ",asn1c_basic_car_control->resources->thiz.present);
            return false;
        }
    }

    //check if functions are available
    if (asn1c_basic_car_control->functions)
    {
        unsigned int num_functions = asn1c_basic_car_control->functions->list.count;

        for (unsigned int i = 0; i < num_functions; i++)
        {
            struct Functions function = *(asn1c_basic_car_control->functions->list.array[i]);

            switch (function.present)
            {
            case Functions_PR::Functions_PR_oid:
                 HandleOID(&(function.choice.oid));
                 break;
            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                     "Received unsupported function type in BasicCarControlSignal: %d\n ",function.present);
                return false;
            }
        }
    }
    return true;
}

} // namespace

/** \}    end of addtogroup */
