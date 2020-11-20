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
 *  \file     bcc_cl_020_signal.cc
 *  \brief    BCC_CL_020 signal
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/bcc_cl_020_signal.h"
#include "signals/signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);


namespace volvo_on_call
{

const std::string BccCl020Signal::oid = "1.3.6.1.4.1.37916.3.6.3.0.2.0";


std::shared_ptr<BccCl020Signal> BccCl020Signal::CreateBccCl020Signal(
                                                      fsm::CCMTransactionId& transaction_id,
                                                      uint16_t sequence_number)
{
    BccCl020Signal* signal = new BccCl020Signal(transaction_id,
                                                sequence_number);

    return std::shared_ptr<BccCl020Signal>(signal);
}

BccCl020Signal::BccCl020Signal (fsm::CCMTransactionId& transaction_id,
                                uint16_t sequence_number) : fsm::CCM::CCM(transaction_id, sequence_number),
                                                            fsm::Signal::Signal(transaction_id, kBccCl020Signal)
{
    SetStoreAndForwardOptions(CCM_STORE_AND_FORWARD_LATEST);
}

BccCl020Signal::~BccCl020Signal ()
{
    // free any instance data that needs freeing
}

/**************************************
 * virtual CCM function implementations
 **************************************/

const char* BccCl020Signal::GetOid ()
{
    return oid.c_str();
}

void* BccCl020Signal::GetPackedPayload()
{
    bool result = true;

    // allocate the asn1c struct, caller shall free
    BCC_CL_020_t* asn1c_bcc_cl_020
            = reinterpret_cast<BCC_CL_020_t*>(calloc(1, sizeof(BCC_CL_020_t)));

    if (nullptr == asn1c_bcc_cl_020)
    {
        result = false;
    }

    fsm::LocationData::LocationPr location_pr;

    if (result)
    {
        result = GetPackedLocationData(&(asn1c_bcc_cl_020->location.choice.extendedLocation),
                                       &(asn1c_bcc_cl_020->location.choice.minimumLocation),
                                       location_pr);
    }

    if (result)
    {
        switch (location_pr)
        {
            case fsm::LocationData::kLocationNothing:
            {
                asn1c_bcc_cl_020->location.present = location_PR_NOTHING;
                break;
            }

            case fsm::LocationData::kLocationMinimum:
            {
                asn1c_bcc_cl_020->location.present = location_PR_minimumLocation;
                break;
            }

            case fsm::LocationData::kLocationExtended:
            {
                asn1c_bcc_cl_020->location.present = location_PR_extendedLocation;
                break;
            }

            default:
            {
                asn1c_bcc_cl_020->location.present = location_PR_NOTHING;
                break;
            }
        }
    }
    else
    {
        if (asn1c_bcc_cl_020)
        {
            ASN_STRUCT_FREE(asn_DEF_BCC_CL_020, asn1c_bcc_cl_020);
        }
        asn1c_bcc_cl_020 = nullptr;
    }

    return reinterpret_cast<void*>(asn1c_bcc_cl_020);
}

asn_TYPE_descriptor_t* BccCl020Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_BCC_CL_020;
}

fs_VersionInfo BccCl020Signal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
}

} //namespace
/** \}    end of addtogroup */
