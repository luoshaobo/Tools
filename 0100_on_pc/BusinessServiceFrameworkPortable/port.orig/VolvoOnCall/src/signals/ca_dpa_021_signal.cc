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
 *  \file     ca_dpa_021_signal.cc
 *  \brief    CA_DPA_021 signal
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/ca_dpa_021_signal.h"
#include "signals/signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string CaDpa021Signal::oid_ = "1.3.6.1.4.1.37916.3.8.7.0.2.1";

/**************************
 * public member functions
 **************************/

CaDpa021Signal::CaDpa021Signal (fsm::CCMTransactionId& transaction_id,
                                uint16_t message_id) : fsm::CCM::CCM(transaction_id, message_id),
                                                       fsm::Signal::Signal(transaction_id, VocSignalTypes::kCaDpa021Signal)
{}

bool CaDpa021Signal::SetFrame(VocFrameCodec codec, const VocFrameEncodableInterface& frame)
{
    return codec.Encode(frame_, frame);
}

CaDpa021Signal::~CaDpa021Signal ()
{
    // free any instance data that needs freeing
}

/**************************
 * protected member functions
 **************************/

void* CaDpa021Signal::GetPackedPayload()
{
    CA_DPA_021_t* asn1c_ca_dpa_021 = (CA_DPA_021_t*) calloc(1, sizeof(CA_DPA_021_t));

    // call super class to pack device_pairing_id
    if (asn1c_ca_dpa_021 && !DevicePairingId::PackData(&(asn1c_ca_dpa_021->id)))
    {
        asn_DEF_CA_DPA_021.free_struct(&asn_DEF_CA_DPA_021, asn1c_ca_dpa_021, 0);
        asn1c_ca_dpa_021 = nullptr;
    }

    // call super class to pack response_info
    if (asn1c_ca_dpa_021 && !ResponseInfoData::PackData(&(asn1c_ca_dpa_021->responseInfo)))
    {
        asn_DEF_CA_DPA_021.free_struct(&asn_DEF_CA_DPA_021, asn1c_ca_dpa_021, 0);
        asn1c_ca_dpa_021 = nullptr;
    }

    if (asn1c_ca_dpa_021 && !frame_.empty()) //no problem if frame is empty, its optional
    {
        asn1c_ca_dpa_021->data = (BIT_STRING_t*) calloc(1, sizeof(BIT_STRING_t));
        if (asn1c_ca_dpa_021->data)
        {
            asn1c_ca_dpa_021->data->buf = (uint8_t*) calloc(1, frame_.size());
            if (asn1c_ca_dpa_021->data->buf)
            {
                memcpy(asn1c_ca_dpa_021->data->buf, frame_.data(), frame_.size());
                asn1c_ca_dpa_021->data->size = frame_.size();
            }
            else
            {
                asn1c_ca_dpa_021->data->size = 0;
            }

            asn1c_ca_dpa_021->data->bits_unused = 0;
        }
        else
        {
            asn_DEF_CA_DPA_021.free_struct(&asn_DEF_CA_DPA_021, asn1c_ca_dpa_021, 0);
            asn1c_ca_dpa_021 = nullptr;
        }
    }

    return (void*)asn1c_ca_dpa_021;
}


/**************************
 * private member functions
 **************************/

#ifdef VOC_TESTS

bool CaDpa021Signal::UnpackPayload(CA_DPA_021_t* asn1c_ca_dpa_021)
{
    bool return_value = true;

    //check if resources are available
    if (asn1c_ca_dpa_021)
    {
        // call super class to unpack device pairing id
        return_value = DevicePairingId::UnpackData(&(asn1c_ca_dpa_021->id));

        if (return_value)
        {
            // call super class to unpack response info
            return_value = ResponseInfoData::UnpackData(&(asn1c_ca_dpa_021->responseInfo));
        }

        if (return_value &&
            asn1c_ca_dpa_021->data &&
            asn1c_ca_dpa_021->data->buf &&
            (asn1c_ca_dpa_021->data->size > 0))
        {
            frame_.assign(asn1c_ca_dpa_021->data->buf, asn1c_ca_dpa_021->data->buf + asn1c_ca_dpa_021->data->size);
        }
    }

    return return_value;
}

CaDpa021Signal::CaDpa021Signal (ccm_Message* ccm,
                                fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                      fsm::Signal::Signal(transaction_id,
                                                                                          VocSignalTypes::kCaDpa021Signal)
{
    //asn1c struct to hold decoded data
    CA_DPA_021_t* asn1c_ca_dpa_021 = NULL;

    fs_VersionInfo version_info;
    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    //call super decode with input data from here
    valid_ = DecodePayload((void**)&asn1c_ca_dpa_021, &asn_DEF_CA_DPA_021, version_info);

    if (valid_)
    {
        // extract the data and store in class members
        valid_ = UnpackPayload(asn1c_ca_dpa_021);
    }

    // free decoded asn1c struct
    asn_DEF_CA_DPA_021.free_struct(&asn_DEF_CA_DPA_021, asn1c_ca_dpa_021, 0);
}

std::shared_ptr<fsm::Signal> CaDpa021Signal::GetVocFrame(VocFrameCodec& codec)
{
    return codec.Decode(frame_);
}

std::shared_ptr<CaDpa021Signal> CaDpa021Signal::CreateCaDpa021Signal(ccm_Message* ccm,
                                                                     fsm::TransactionId& transaction_id)
{
    CaDpa021Signal* signal = new CaDpa021Signal(ccm, transaction_id);
    if (!signal->valid_)
    {
        delete signal;
        return std::shared_ptr<CaDpa021Signal>();
    }
    else
    {
        return std::shared_ptr<CaDpa021Signal>(signal);
    }
}

#endif

} // namespace volvo_on_call

/** \}    end of addtogroup */
