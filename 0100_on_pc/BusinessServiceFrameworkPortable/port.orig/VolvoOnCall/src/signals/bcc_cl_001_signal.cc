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

#include "signals/bcc_cl_001_signal.h"
#include "signals/signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string BccCl001Signal::oid = "1.3.6.1.4.1.37916.3.6.3.0.0.1";

std::shared_ptr<fsm::Signal> BccCl001Signal::CreateBccCl001Signal(ccm_Message* ccm,
                                                                  fsm::TransactionId& transaction_id)
{
    BccCl001Signal* signal = new BccCl001Signal(ccm, transaction_id);
    if (!signal->valid)
    {
        delete signal;
        return std::shared_ptr<BccCl001Signal>();
    }
    else
    {
        return std::shared_ptr<BccCl001Signal>(signal);
    }
}


BccCl001Signal::BccCl001Signal (ccm_Message* ccm,
                                fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                      fsm::Signal::Signal(transaction_id,
                                                                                          kBccCl001Signal)
{
    //asn1c struct to hold decoded data
    BCC_CL_001_t* asn1c_bcc_CL_001 = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_bcc_CL_001, &asn_DEF_BCC_CL_001, GetContentVersion ());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_bcc_CL_001);
    }

    // free decoded asn1c struct
    asn_DEF_BCC_CL_001.free_struct(&asn_DEF_BCC_CL_001, asn1c_bcc_CL_001, 0);
}

BccCl001Signal::BccCl001Signal (fsm::CCMTransactionId& transaction_id,
                                uint16_t sequence_number)
                 : fsm::CCM::CCM(transaction_id, sequence_number),
                   fsm::Signal::Signal(transaction_id, kBccCl001Signal)
{
}

BccCl001Signal::~BccCl001Signal ()
{
    // free any instance data that needs freeing
}

/**************************************
 * virtual CCM function implementations
 **************************************/

const char* BccCl001Signal::GetOid ()
{
    return oid.c_str();
}

asn_TYPE_descriptor_t* BccCl001Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_BCC_CL_001;
}

fs_VersionInfo BccCl001Signal::GetContentVersion ()
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


bool BccCl001Signal::UnpackPayload(BCC_CL_001_t* asn1c_bcc_CL_001)
{
   //check if resources are available
  if (asn1c_bcc_CL_001)
  {
      //it the type gets extended switch may be more appropriate here
      if (asn1c_bcc_CL_001->operation.present
                  == operation_PR::operation_PR_locationAdvert)
      {
          switch (asn1c_bcc_CL_001->operation.choice.locationAdvert.advert)
          {
          case Adverts::Adverts_honk:
               supportedAdvert = Advert::Honk;
               break;
          case Adverts::Adverts_flash:
               supportedAdvert = Advert::Flash;
               break;
          case Adverts::Adverts_honkAndFlash:
               supportedAdvert = Advert::HonkAndFlash;
               break;
          default:
              //unsupported advert type received
              DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
               "Received unsupported advert type in BccCl001Signal: %d\n ",
                               asn1c_bcc_CL_001->operation.choice.locationAdvert.advert);
              supportedAdvert = Advert::None;
              return false;
          }
      }
  }
  return true;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
