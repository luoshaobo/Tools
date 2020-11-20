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
 *  \file     entry_point_signal.cc
 *  \brief    EntryPoint signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/entry_point_signal.h"
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

const std::string EntryPointSignal::oid_ = "1.3.6.1.4.1.37916.3.0.4";

std::shared_ptr<Signal> EntryPointSignal::CreateEntryPointSignal(ccm_Message* ccm,
                                                                 TransactionId& transaction_id)
{
    EntryPointSignal* signal = new EntryPointSignal(ccm, transaction_id);
    if (!signal || !signal->valid)
    {
        delete signal;
        return std::shared_ptr<EntryPointSignal>();
    }
    else
    {
        return std::shared_ptr<EntryPointSignal>(signal);
    }
}



EntryPointSignal::EntryPointSignal (ccm_Message* ccm,
                                    TransactionId& transaction_id) : CCM::CCM(ccm),
                                                                     Signal::Signal(transaction_id,
                                                                                    kEntryPointSignal)
{
    //asn1c struct to hold decoded data
    EntryPoint_t* asn1c_EntryPoint = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_EntryPoint, &asn_DEF_EntryPoint, GetContentVersion ());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_EntryPoint);
    }

    // free decoded asn1c struct
    asn_DEF_EntryPoint.free_struct(&asn_DEF_EntryPoint, asn1c_EntryPoint, 0);
}


#ifdef VOC_TESTS
EntryPointSignal::EntryPointSignal(fsm::CCMTransactionId& transaction_id,
                                   uint16_t message_id): fsm::CCM::CCM(transaction_id, message_id),
                                                         fsm::Signal::Signal(transaction_id, kEntryPointSignal)
{}
#endif

EntryPointSignal::~EntryPointSignal ()
{
}


std::string EntryPointSignal::GetUri()
{
    return uri_;
}

long EntryPointSignal::GetSpecificationVersion()
{
    return specification_version_;
}

long EntryPointSignal::GetImplementationVersion()
{
    return implementation_version_;
}

/**
 * \brief Gets URI to the feature list
 * \return URI of the feature list
 */
std::string EntryPointSignal::GetClientUri()
{
    return client_uri_;
}

/**
 * \brief Gets the host part of the feature list URI
 * \return host of the feature list
 */
std::string EntryPointSignal::GetHost()
{
    return host_;
}

/**
 * \brief Gets the port part of the feature list URI
 * \return port of the feature list URI
 */
long EntryPointSignal::GetPort()
{
    return port_;
}

/**
 * \brief Gets the signal service URI
 * \return signal service URI, empty string if undefined
 */
std::string EntryPointSignal::GetSignalServiceUri()
{
    return signal_service_uri_;
}

/**
 * \brief Gets the list of country codes entry point is valid for
 * \return list of country code, empty vector if undefined
 */
std::vector<std::string> EntryPointSignal::GetValidLocations()
{
    return valid_locations_;
}

fs_VersionInfo EntryPointSignal::GetContentVersion ()
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


bool EntryPointSignal::UnpackPayload(EntryPoint_t* asn1c_EntryPoint)
{
    bool success = false;

    //check if asn1c_EntryPoint are available
    if (asn1c_EntryPoint)
    {
        success = true;
        //Get mandatory data
        ASN1String2StdString(asn1c_EntryPoint->thiz, uri_);
        specification_version_ = asn1c_EntryPoint->specificationVersion;
        implementation_version_ = asn1c_EntryPoint->implementationVersion;
        ASN1String2StdString(asn1c_EntryPoint->clientUri, client_uri_);
        ASN1String2StdString(asn1c_EntryPoint->host, host_);
        port_ = asn1c_EntryPoint->port;

        //Get optional data
        //unpack signal service uri if present
        if (asn1c_EntryPoint->signalServiceUri)
        {
            ASN1String2StdString(*asn1c_EntryPoint->signalServiceUri, signal_service_uri_);
        }
        //check if valid locations are defined
        if (asn1c_EntryPoint->validLocations)
        {
            //loop through valid location list and unpack them
            unsigned int num_locations = asn1c_EntryPoint->validLocations->list.count;
            for (unsigned int i = 0; i < num_locations; i++)
            {
                UTF8String_t asn1_country = *(asn1c_EntryPoint->validLocations->list.array[i]);
                std::string country;
                ASN1String2StdString(asn1_country, country);
                valid_locations_.push_back(country);
            }
        }

    }
    return success;
}

#ifdef VOC_TESTS
/**************************************
 * virtual CCM function implementations
 **************************************/

const char* EntryPointSignal::GetOid ()
{
    return oid_.c_str();
}

asn_TYPE_descriptor_t* EntryPointSignal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_EntryPoint;
}

void EntryPointSignal::SetParameters(std::string uri, long spec_version, long impl_version,
                                     std::string client_uri, std::string host, long port,
                                     std::string signaling_service_uri, std::vector<std::string> valid_locations )
{
    uri_ = uri;
    specification_version_ = spec_version;
    implementation_version_ = impl_version;
    client_uri_ = client_uri;
    host_ = host;
    port_ = port;
    signal_service_uri_ = signaling_service_uri;
    valid_locations_ = valid_locations;

}

void* EntryPointSignal::GetPackedPayload()
{

    EntryPoint_t* asn1c_EntryPoint = (EntryPoint_t*) calloc(1, sizeof(EntryPoint_t));
    bool success = false;

    if (asn1c_EntryPoint)
    {
        success = true;

        OCTET_STRING_fromBuf(&(asn1c_EntryPoint->thiz), uri_.data(), uri_.length());
        asn1c_EntryPoint->specificationVersion = specification_version_;
        asn1c_EntryPoint->implementationVersion = implementation_version_;
        OCTET_STRING_fromBuf(&(asn1c_EntryPoint->clientUri), client_uri_.data(), client_uri_.length());
        OCTET_STRING_fromBuf(&(asn1c_EntryPoint->host), host_.data(), host_.length());
        asn1c_EntryPoint->port = port_;

        //pack signal service uri if present (optional)
        if (signal_service_uri_.size() != 0)
        {
            asn1c_EntryPoint->signalServiceUri =
                    OCTET_STRING_new_fromBuf(&asn_DEF_Uri, signal_service_uri_.data(), signal_service_uri_.length());

            if (asn1c_EntryPoint->signalServiceUri == NULL)
            {
                success = false;
            }
        }

        if (success && valid_locations_.size() > 0)
        {
            asn1c_EntryPoint->validLocations = (struct EntryPoint::validLocations*)calloc(1, sizeof(struct EntryPoint::validLocations));
            if (asn1c_EntryPoint->validLocations)
            {
                for (int j=0; j<valid_locations_.size() && success; j++)
                {
                    Country_t* asn1c_country = (Country_t*) calloc (1, sizeof(Country_t));
                    OCTET_STRING_fromBuf(asn1c_country, valid_locations_[j].data(), valid_locations_[j].length());
                    if (ASN_SEQUENCE_ADD(&(asn1c_EntryPoint->validLocations->list),
                                         asn1c_country) != 0)
                    {
                        success = false;
                        ASN_STRUCT_FREE(asn_DEF_Country, asn1c_country);
                    }
                }
            }
            else
            {
                success = false;
            }
        }


        if (!success)
        {
            ASN_STRUCT_FREE(*GetPayloadTypeDescriptor(), (void*)(asn1c_EntryPoint));
            asn1c_EntryPoint = nullptr;
        }
    }

    return (void*)asn1c_EntryPoint;
}

#endif


} // namespace fsm

/** \}    end of addtogroup */
