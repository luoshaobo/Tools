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
 *  \file     ca_del_002_signal.cc
 *  \brief    CA_DEL_002 signal
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/ca_del_002_signal.h"
#include "signals/signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string CaDel002Signal::oid_ = "1.3.6.1.4.1.37916.3.8.5.0.0.2";

std::shared_ptr<fsm::Signal> CaDel002Signal::CreateCaDel002Signal(ccm_Message* ccm,
                                                                  fsm::TransactionId& transaction_id)
{
    CaDel002Signal* signal = new CaDel002Signal(ccm, transaction_id);
    if (!signal->valid)
    {
        delete signal;
        return std::shared_ptr<CaDel002Signal>();
    }
    else
    {
        return std::shared_ptr<CaDel002Signal>(signal);
    }
}

CaDel002Signal::CaDel002Signal (ccm_Message* ccm,
                                fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                      fsm::Signal::Signal(transaction_id,
                                                                                          kCaDel002Signal)
{
    //asn1c struct to hold decoded data
    CA_DEL_002_t* asn1c_ca_del_002 = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_ca_del_002, &asn_DEF_CA_DEL_002, GetContentVersion ());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_ca_del_002);
    }

    // free decoded asn1c struct
    asn_DEF_CA_DEL_002.free_struct(&asn_DEF_CA_DEL_002, asn1c_ca_del_002, 0);
}

CaDel002Signal::~CaDel002Signal ()
{
    // free any instance data that needs freeing
}

/**************************************
 * virtual CCM function implementations
 **************************************/

fs_VersionInfo CaDel002Signal::GetContentVersion ()
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


bool CaDel002Signal::UnpackPayload(CA_DEL_002_t* asn1c_ca_del_002)
{
    bool return_value = true;

    // check if resources are available
    if (asn1c_ca_del_002)
    {
        // call super class to unpack teh asn1c repsonse info data
        return_value = UnpackData(&(asn1c_ca_del_002->responseInfo));
    }
    else
    {
        return_value = false;
    }

    return return_value;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
