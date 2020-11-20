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
 *  \file     ca_dpa_020_signal.cc
 *  \brief    CA_DPA_020 signal
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/ca_dpa_020_signal.h"
#include "dlt/dlt.h"

#include <iostream>

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string CaDpa020Signal::oid_ = "1.3.6.1.4.1.37916.3.8.7.0.2.0";

/**************************
 * public member functions
 **************************/

std::shared_ptr<fsm::Signal> CaDpa020Signal::GetVocFrame(VocFrameCodec& codec)
{
    return codec.Decode(frame_);
}

std::shared_ptr<fsm::Signal> CaDpa020Signal::CreateCaDpa020Signal(
                                                          ccm_Message* ccm,
                                                          fsm::TransactionId& transaction_id)
{
    CaDpa020Signal* signal = new CaDpa020Signal(ccm, transaction_id);
    if (!signal->valid_)
    {
        delete signal;
        return std::shared_ptr<CaDpa020Signal>();
    }
    else
    {
        return std::shared_ptr<CaDpa020Signal>(signal);
    }
}

CaDpa020Signal::~CaDpa020Signal ()
{
    // free any instance data that needs freeing
}

/**************************
 * private member functions
 **************************/

CaDpa020Signal::CaDpa020Signal (ccm_Message* ccm,
                                fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                      fsm::Signal::Signal(transaction_id,
                                                                                          VocSignalTypes::kCaDpa020Signal)
{
    //asn1c struct to hold decoded data
    CA_DPA_020_t* asn1c_ca_dpa_020 = NULL;

    fs_VersionInfo version_info;
    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    //call super decode with input data from here
    valid_ = DecodePayload((void**)&asn1c_ca_dpa_020, &asn_DEF_CA_DPA_020, version_info);

    if (valid_)
    {
        // extract the data and store in class members
        valid_ = UnpackPayload(asn1c_ca_dpa_020);
    }

    // free decoded asn1c struct
    asn_DEF_CA_DPA_020.free_struct(&asn_DEF_CA_DPA_020, asn1c_ca_dpa_020, 0);
}

bool CaDpa020Signal::UnpackPayload(CA_DPA_020_t* asn1c_ca_dpa_020)
{
    bool return_value = true;

    //check if resources are available
    if (asn1c_ca_dpa_020)
    {
        // call super class to unpack device pairing id
        return_value = UnpackData(&(asn1c_ca_dpa_020->id));

        if (return_value)
        {
            if (asn1c_ca_dpa_020->data.buf && asn1c_ca_dpa_020->data.size > 0) //not optional
            {
                frame_.assign(asn1c_ca_dpa_020->data.buf, asn1c_ca_dpa_020->data.buf + asn1c_ca_dpa_020->data.size);
            }
            else
            {
                return_value = false;
            }
        }
    }

    return return_value;
}

#ifdef VOC_TESTS

CaDpa020Signal::CaDpa020Signal (fsm::CCMTransactionId& transaction_id,
                                uint16_t message_id) : fsm::CCM::CCM(transaction_id, message_id),
                                                       fsm::Signal::Signal(transaction_id, VocSignalTypes::kCaDpa020Signal)
{}

bool CaDpa020Signal::SetFrame(VocFrameCodec codec, VocFrameEncodableInterface& frame)
{
    return codec.Encode(frame_, frame);
}

void* CaDpa020Signal::GetPackedPayload()
{
    CA_DPA_020_t* asn1c_ca_dpa_020 = (CA_DPA_020_t*) calloc(1, sizeof(CA_DPA_020_t));

    if (asn1c_ca_dpa_020)
    {
        // call super class tp pack device_pairing_id
        if (!PackData(&(asn1c_ca_dpa_020->id)))
        {
            free(asn1c_ca_dpa_020);
            asn1c_ca_dpa_020 = nullptr;
        }
    }

    if (asn1c_ca_dpa_020)
    {
        if (!frame_.empty()) //not optional
        {
            asn1c_ca_dpa_020->data.buf = (uint8_t*) calloc(1, frame_.size());
            memcpy(asn1c_ca_dpa_020->data.buf, frame_.data(), frame_.size());
            asn1c_ca_dpa_020->data.size = frame_.size();
            asn1c_ca_dpa_020->data.bits_unused = 0;
        }
        else
        {
            asn_DEF_CA_DPA_020.free_struct(&asn_DEF_CA_DPA_020, asn1c_ca_dpa_020, 0);
            asn1c_ca_dpa_020 = nullptr;
        }
    }

    return (void*)asn1c_ca_dpa_020;
}

#endif

} // namespace volvo_on_call

/** \}    end of addtogroup */
