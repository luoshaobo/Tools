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
 *  \file     bcc_cl_010_signal.cc
 *  \brief    BCC_CL_010 signal
 *  \author   Iulian Sirghi
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/bcc_cl_010_signal.h"
#include "signals/signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string BccCl010Signal::oid = "1.3.6.1.4.1.37916.3.6.3.0.1.0";

std::shared_ptr<fsm::Signal> BccCl010Signal::CreateBccCl010Signal(ccm_Message* ccm, fsm::TransactionId& transaction_id)
{
    std::shared_ptr<BccCl010Signal> signal = std::shared_ptr<BccCl010Signal>(new BccCl010Signal(ccm, transaction_id));

    if (!signal->valid)
    {
        signal = std::shared_ptr<BccCl010Signal>();
    }

    return signal;
}

BccCl010Signal::BccCl010Signal(ccm_Message *ccm,
                               fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                     fsm::Signal::Signal(transaction_id,
                                                                                         kBccCl010Signal)
{
    // BCC-CL-010 does not have any payload content,
    // normally we would try to decode paylaod and
    // unpack data here but lets not for bcc-cl-010.
    valid = true;
}

BccCl010Signal::~BccCl010Signal ()
{
    // free any instance data that needs freeing
}

/**************************************
 * virtual CCM function implementations
 **************************************/

const char* BccCl010Signal::GetOid()
{
    return oid.c_str();
}

asn_TYPE_descriptor_t* BccCl010Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_BCC_CL_010;
}

fs_VersionInfo BccCl010Signal::GetContentVersion ()
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

bool BccCl010Signal::UnpackPayload(BCC_CL_010_t* asn1c_bcc_CL_010)
{
    // The data members for BCC-CL-010 from BasicCarControl-PA15 are marked as TDB,
    // at this point. We will revisit the unpacking method if or when the members are determined.
    return true;
}


} // namespace volvo_on_call

/** \}    end of addtogroup */
