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
 *  \file     ca_dpa_002_signal.cc
 *  \brief    CA_DPA_002 signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/ca_dpa_002_signal.h"
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string CaDpa002Signal::oid_ = "1.3.6.1.4.1.37916.3.8.7.0.0.2";

typedef struct CA_DPA_002::status status_t;

/**************************
 * public member functions
 **************************/


std::shared_ptr<fsm::Signal> CaDpa002Signal::CreateCaDpa002Signal(
                                                          ccm_Message* ccm,
                                                          fsm::TransactionId& transaction_id)
{
    CaDpa002Signal* signal = new CaDpa002Signal(ccm, transaction_id);
    if (!signal->valid_)
    {
        delete signal;
        return std::shared_ptr<CaDpa002Signal>();
    }
    else
    {
        return std::shared_ptr<CaDpa002Signal>(signal);
    }
}

CaDpa002Signal::~CaDpa002Signal ()
{
    // free any instance data that needs freeing
}

bool CaDpa002Signal::IsFinished(FinishedStatus& finished_status)
{
    bool finished = (status_ == kStatusFinished);

    if (finished)
    {
        finished_status = status_finished_;
    }

    return finished;
}


bool CaDpa002Signal::IsInitiated()
{
    return (status_ == kStatusInitiated);
}


bool CaDpa002Signal::GetRemoteConnectionSessionId(std::string& session_id)
{
    if (session_id_set_)
    {
        session_id = session_id_;
    }

    return session_id_set_;
}

/**************************
 * private member functions
 **************************/

CaDpa002Signal::CaDpa002Signal (ccm_Message* ccm,
                                fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                      fsm::Signal::Signal(transaction_id,
                                                                                          VocSignalTypes::kCaDpa002Signal)
{
    //asn1c struct to hold decoded data
    CA_DPA_002_t* asn1c_ca_dpa_002 = NULL;

    fs_VersionInfo version_info;
    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    //call super decode with input data from here
    valid_ = DecodePayload((void**)&asn1c_ca_dpa_002, &asn_DEF_CA_DPA_002, version_info);

    if (valid_)
    {
        // extract the data and store in class members
        valid_ = UnpackPayload(asn1c_ca_dpa_002);
    }

    // free decoded asn1c struct
    asn_DEF_CA_DPA_002.free_struct(&asn_DEF_CA_DPA_002, asn1c_ca_dpa_002, 0);
}

bool CaDpa002Signal::UnpackPayload(CA_DPA_002_t* asn1c_ca_dpa_002)
{
    bool return_value = true;

    //check if resources are available
    if (asn1c_ca_dpa_002)
    {
        // call DevicePairingId super class to unpack device pairing id
        return_value = DevicePairingId::UnpackData(&(asn1c_ca_dpa_002->id));

        if (return_value)
        {
            // call ResponseInfoData super class to unpack response info
            return_value = ResponseInfoData::UnpackData(&(asn1c_ca_dpa_002->responseInfo));
        }

        // populate optionals

        //status
        if (return_value && asn1c_ca_dpa_002->status)
        {
            switch (asn1c_ca_dpa_002->status->present)
            {
            case status_PR::status_PR_initiated:
                status_ = kStatusInitiated;
                break;
            case status_PR::status_PR_finished:
                status_ = kStatusFinished;

                switch (asn1c_ca_dpa_002->status->choice.finished)
                {
                case FinishedStatus_stoppedByCommand:
                    status_finished_ = kStoppedByCommand;
                    break;
                case FinishedStatus_terminatedRemoteConnectionTimeout:
                    status_finished_ = kTerminatedRemoteConnectionTimeout;
                    break;
                case FinishedStatus_terminatedPairingTimeout:
                    status_finished_ = kTerminatedPairingTimeout;
                    break;
                default:
                    DLT_LOG_STRINGF(dlt_voc,
                                    DLT_LOG_WARN,
                                    "Got unknown finished status in ca-dpa-002 %d\n",
                                    asn1c_ca_dpa_002->status->choice.finished);
                    return_value = false;
                    break;
                }
                break;
            default:
                DLT_LOG_STRINGF(dlt_voc,
                                DLT_LOG_WARN,
                                "Got unknown status in ca-dpa-002 %d\n",
                                asn1c_ca_dpa_002->status->present);
                return_value = false;
                break;
            }
        }

        //remote connection session id
        if (return_value && asn1c_ca_dpa_002->remoteConnectionSessionId != NULL)
        {
            session_id_set_ = true;
            session_id_.assign(asn1c_ca_dpa_002->remoteConnectionSessionId->buf,
                               asn1c_ca_dpa_002->remoteConnectionSessionId->buf + asn1c_ca_dpa_002->remoteConnectionSessionId->size);
        }


    }

    return return_value;
}

#ifdef VOC_TESTS

CaDpa002Signal::CaDpa002Signal (fsm::CCMTransactionId& transaction_id,
                                uint16_t message_id) : fsm::CCM::CCM(transaction_id, message_id),
                                                       fsm::Signal::Signal(transaction_id, VocSignalTypes::kCaDpa002Signal)
{}


void* CaDpa002Signal::GetPackedPayload()
{

    CA_DPA_002_t* asn1c_ca_dpa_002 = (CA_DPA_002_t*) calloc(1, sizeof(CA_DPA_002_t));

    if (asn1c_ca_dpa_002)
    {
        bool success;

        //pack mandatory

        // call super class to pack device_pairing_id
        success = DevicePairingId::PackData(&(asn1c_ca_dpa_002->id));

        if (success)
        {
            success = ResponseInfoData::PackData(&(asn1c_ca_dpa_002->responseInfo));
        }

        //pack optionals

        //status
        if (success && status_ != kStatusNothing)
        {
            asn1c_ca_dpa_002->status = (status_t*) calloc(1, sizeof(status_t));

            if (status_ == kStatusInitiated)
            {
               asn1c_ca_dpa_002->status->present = status_PR::status_PR_initiated;
               asn1c_ca_dpa_002->status->choice.initiated = 0;
            }
            else if (status_ == kStatusFinished)
            {
               asn1c_ca_dpa_002->status->present = status_PR::status_PR_finished;
               asn1c_ca_dpa_002->status->choice.finished = status_finished_; //use the fact that enums match (this is test code)

            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc,
                                DLT_LOG_WARN,
                                "Got unknown status in ca-dpa-002:  %d\n",
                                status_);
                success = false;
            }
        }

        if (success && session_id_set_)
        {
           asn1c_ca_dpa_002->remoteConnectionSessionId =
                      OCTET_STRING_new_fromBuf(&asn_DEF_SessionId, session_id_.data(), session_id_.length());

           if (asn1c_ca_dpa_002->remoteConnectionSessionId == NULL)
           {
               DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to encode session id in ca-dpa-002\n");
               success = false;
           }
        }

        if (!success)
        {
            asn_DEF_CA_DPA_002.free_struct(&asn_DEF_CA_DPA_002, asn1c_ca_dpa_002, 0);
            asn1c_ca_dpa_002 = nullptr;
        }
    }

    return (void*)asn1c_ca_dpa_002;
}

void CaDpa002Signal::SetInitiated()
{
    status_ = kStatusInitiated;
}

/**
 * \brief Sets status to finished and assigns corresponding finished status
 * \param[in] status status to set
 */
void CaDpa002Signal::SetFinished(FinishedStatus status)
{
    status_ = kStatusFinished;
    status_finished_ = status;
}

/**
 * \brief Sets remote connection session id
 * \param[in] session_id session id
 */
void CaDpa002Signal::SetRemoteConnectionSessionId(std::string session_id)
{
    session_id_set_ = true;
    session_id_ = session_id;
}

#endif

} // namespace volvo_on_call

/** \}    end of addtogroup */
