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
 *  \file     ca_del_001_signal.cc
 *  \brief    CA_DEL_001 signal
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/signal_types.h"
#include "signals/ca_del_001_signal.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string CaDel001Signal::oid = "1.3.6.1.4.1.37916.3.8.5.0.0.1";


std::shared_ptr<CaDel001Signal> CaDel001Signal::CreateCaDel001Signal(fsm::CCMTransactionId& transaction_id,
                                                                     uint16_t sequence_number)
{
    CaDel001Signal* signal = new CaDel001Signal(transaction_id, sequence_number);

    return std::shared_ptr<CaDel001Signal>(signal);
}


CaDel001Signal::CaDel001Signal (fsm::CCMTransactionId& transaction_id,
                                uint16_t sequence_number) : fsm::CCM::CCM(transaction_id, sequence_number),
                                                            fsm::Signal::Signal(transaction_id, kCaDel001Signal)
{
}

CaDel001Signal::~CaDel001Signal ()
{
    // free any instance data that needs freeing
}

/**************************************
 * virtual CCM function implementations
 **************************************/

const char* CaDel001Signal::GetOid ()
{
    return oid.c_str();
}

asn_TYPE_descriptor_t* CaDel001Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_CA_DEL_001;
}

fs_VersionInfo CaDel001Signal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
}

void* CaDel001Signal::GetPackedPayload()
{
    CA_DEL_001_t* asn1c_ca_del_001 = NULL;

    // allocate the asn1c struct, caller shall free
    asn1c_ca_del_001 = (CA_DEL_001_t*) calloc(1, sizeof(CA_DEL_001_t));

    if (asn1c_ca_del_001 == NULL)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CA-DEL-001 encoder: failed to allocate memory for CA_DEL_001");
    }

    // This signal carries no data, just tells the cloud it's time to send delegate CSR
    return (void*) asn1c_ca_del_001;
}

int CaDel001Signal::GetEncodeFlags() const
{
    return EncodeFlag::kNone;
}

fs_Encoding CaDel001Signal::GetPreferedEncoding ()
{
    return FS_ENCODING_DER;
}


} // namespace volvo_on_call
/** \}    end of addtogroup */
