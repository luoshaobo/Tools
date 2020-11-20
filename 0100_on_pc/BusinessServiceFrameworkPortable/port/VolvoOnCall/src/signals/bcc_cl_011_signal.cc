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
 *  \file     bcc_cl_011_signal.cc
 *  \brief    BCC_CL_011 signal
 *  \author   Iulian Sirghi
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/bcc_cl_011_signal.h"
#include "signals/signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

// Hack for compiler to resolve this as a type
typedef struct BCC_CL_011::location location_t;

const std::string BccCl011Signal::oid = "1.3.6.1.4.1.37916.3.6.3.0.1.1";


std::shared_ptr<BccCl011Signal> BccCl011Signal::CreateBccCl011Signal(fsm::CCMTransactionId& transaction_id,
                                                                     uint16_t sequence_number)
{
    BccCl011Signal* signal = new BccCl011Signal(transaction_id, sequence_number);
    return std::shared_ptr<BccCl011Signal>(signal);
}

BccCl011Signal::BccCl011Signal(fsm::CCMTransactionId& transaction_id,
                               uint16_t sequence_number)
    : fsm::CCM::CCM(transaction_id, sequence_number),
      fsm::Signal::Signal(transaction_id, kBccCl011Signal)
{
}

const char* BccCl011Signal::GetOid ()
{
    return oid.c_str();
}

void* BccCl011Signal::GetPackedPayload()
{
    bool result = true;

    // allocate the asn1c struct, caller shall free
    BCC_CL_011_t* asn1c_bcc_cl_011 = reinterpret_cast<BCC_CL_011_t*>(calloc(1, sizeof(BCC_CL_011_t)));

    if (!asn1c_bcc_cl_011)
    {
        result = false;
    }

    if (result)
    {
        // Call ResponseInfo superclass to pack data
        result = PackData(&asn1c_bcc_cl_011->responseInfo);
    }

    if (result)
    {
        fsm::LocationData::LocationPr location_pr;

        asn1c_bcc_cl_011->location = reinterpret_cast<location_t*>(calloc(1, sizeof(location_t)));

        if (asn1c_bcc_cl_011->location &&
            GetPackedLocationData(&(asn1c_bcc_cl_011->location->choice.extendedLocation),
                                  &(asn1c_bcc_cl_011->location->choice.minimumLocation),
                                  location_pr))
        {
            switch (location_pr)
            {
            case fsm::LocationData::kLocationNothing:
            {
                asn1c_bcc_cl_011->location->present = location_PR_NOTHING;
                break;
            }

            case fsm::LocationData::kLocationMinimum:
            {
                asn1c_bcc_cl_011->location->present = location_PR_minimumLocation;
                break;
            }

            case fsm::LocationData::kLocationExtended:
            {
                asn1c_bcc_cl_011->location->present = location_PR_extendedLocation;
                break;
            }

            default:
            {
                asn1c_bcc_cl_011->location->present = location_PR_NOTHING;
                break;
            }
            }
        }
        else
        {
            // Location data is optional in BCC-CL-011 so this is fine.
            if (asn1c_bcc_cl_011->location)
            {
                free(asn1c_bcc_cl_011->location);
            }
            asn1c_bcc_cl_011->location = nullptr;
        }
    }
    else
    {
        if (asn1c_bcc_cl_011)
        {
            ASN_STRUCT_FREE(asn_DEF_BCC_CL_011, asn1c_bcc_cl_011);
        }
        asn1c_bcc_cl_011 = nullptr;
    }

    return reinterpret_cast<void*>(asn1c_bcc_cl_011);
}

asn_TYPE_descriptor_t* BccCl011Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_BCC_CL_011;
}

fs_VersionInfo BccCl011Signal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
}

BccCl011Signal::~BccCl011Signal()
{
    // free any instance data that needs freeing
}


} //namespace
/** \}    end of addtogroup */
