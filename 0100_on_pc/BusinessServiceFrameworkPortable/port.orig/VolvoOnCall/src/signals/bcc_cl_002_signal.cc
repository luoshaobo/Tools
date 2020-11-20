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
 *  \file     bcc_cl_001_signal.cc
 *  \brief    BCC_CL_001 signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/bcc_cl_002_signal.h"
#include "signals/signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string BccCl002Signal::oid = "1.3.6.1.4.1.37916.3.6.3.0.0.2";

std::shared_ptr<BccCl002Signal> BccCl002Signal::CreateBccCl002Signal(fsm::CCMTransactionId& transaction_id,
                                                                     uint16_t sequence_number)
{
    BccCl002Signal* signal = new BccCl002Signal(transaction_id, sequence_number);

    return std::shared_ptr<BccCl002Signal>(signal);
}

BccCl002Signal::BccCl002Signal (ccm_Message* ccm,
                                    fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                          fsm::Signal::Signal(transaction_id,
                                                                                              kBccCl002Signal)
{
}

BccCl002Signal::BccCl002Signal (fsm::CCMTransactionId& transaction_id,
                                uint16_t sequence_number) : fsm::CCM::CCM(transaction_id, sequence_number),
                                                            fsm::Signal::Signal(transaction_id, kBccCl002Signal)
{
}

BccCl002Signal::~BccCl002Signal ()
{
    // free any instance data that needs freeing
}


/**************************************
 * virtual CCM function implementations
 **************************************/

const char* BccCl002Signal::GetOid ()
{
    return oid.c_str();
}

asn_TYPE_descriptor_t* BccCl002Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_BCC_CL_002;
}

fs_VersionInfo BccCl002Signal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
}


/**************************
 * protected member functions
 **************************/


void* BccCl002Signal::GetPackedPayload()
{
    // allocate the asn1c struct, caller shall free
    BCC_CL_002_t* asn1c_bcc_cl_002 = (BCC_CL_002_t*) calloc(1, sizeof(BCC_CL_002_t));
    if (asn1c_bcc_cl_002 == NULL)
    {
        return NULL;
    }

    // Call ResponseInfo super class to pack response info data
    if (PackData(&asn1c_bcc_cl_002->responseInfo))
    {
        return (void*) asn1c_bcc_cl_002;
    }
    else
    {
        //free the structure and return NULL upon error
        ASN_STRUCT_FREE(*GetPayloadTypeDescriptor(), (void*)(asn1c_bcc_cl_002));
        return NULL;
    }
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
