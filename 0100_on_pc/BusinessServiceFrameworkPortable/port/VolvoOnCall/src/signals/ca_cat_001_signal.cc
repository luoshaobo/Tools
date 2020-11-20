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
 *  \file     ca_cat_001_signal.cc
 *  \brief    CA_CAT_001 signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "Certificate.h"
#include "signals/signal_types.h"
#include "signals/ca_cat_001_signal.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

const std::string CaCat001Signal::oid = "1.3.6.1.4.1.37916.3.8.4.0.0.1";


std::shared_ptr<CaCat001Signal> CaCat001Signal::CreateCaCat001Signal(fsm::CCMTransactionId& transaction_id,
                                                                     uint16_t sequence_number)
{
    CaCat001Signal* signal = new CaCat001Signal(transaction_id, sequence_number);

    return std::shared_ptr<CaCat001Signal>(signal);
}


CaCat001Signal::CaCat001Signal (fsm::CCMTransactionId& transaction_id,
                                uint16_t sequence_number) : fsm::CCM::CCM(transaction_id, sequence_number),
                                                            fsm::Signal::Signal(transaction_id, kCaCat001Signal)
{
}

CaCat001Signal::~CaCat001Signal ()
{
    // free any instance data that needs freeing
}


bool CaCat001Signal::AddCertificate(X509* certificate)
{
    bool certificate_added = false;

    //only MAX_NO_CERTIFICATES can be encoded, so add certificate only
    //if there is room for it
    if (certificates_.size() < MAX_NO_CERTIFICATES)
    {
        if (certificate)
        {
           certificates_.push_back(certificate);
           certificate_added = true;
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                            "CA-CAT-001 encoder: attempt to add NULL certificate");
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                        "CA-CAT-001 encoder: attempt to add too many certificate. Supported max certificates: %d",
                        MAX_NO_CERTIFICATES);
    }

    return certificate_added;
}

/**************************************
 * virtual CCM function implementations
 **************************************/

const char* CaCat001Signal::GetOid ()
{
    return oid.c_str();
}

asn_TYPE_descriptor_t* CaCat001Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_CA_CAT_001;
}

fs_VersionInfo CaCat001Signal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
}

void* CaCat001Signal::GetPackedPayload()
{
    //tracks the correctness of the operation
    bool success = true;

    //stores result. In case of unsuccessful operation will be freed if necessary, no need to free before
    CA_CAT_001_t* asn1c_ca_cat_001 = NULL;

    //Assure that there are certificates to encode
    if ((certificates_.size() == 0) || (certificates_.size() > MAX_NO_CERTIFICATES) )
    {
        DLT_LOG_STRINGF(dlt_voc,
                        DLT_LOG_WARN,
                        "CA-CAT-001 encoder: attempt to encode with %d certificates, expects 1..%d certificates",
                        certificates_.size(),
                        MAX_NO_CERTIFICATES);
        success = false;
    }
    else
    {
        // allocate the asn1c struct, caller shall free
        asn1c_ca_cat_001 = (CA_CAT_001_t*) calloc(1, sizeof(CA_CAT_001_t));
        if (asn1c_ca_cat_001 == NULL)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CA-CAT-001 encoder: failed to allocate memory for CA_CAT_001");
            success = false;
        }

        //loop through certificates and add them
        for(X509* certificate_source : certificates_)
        {
            //If we have already failed, break
            if (!success)
            {
                break;
            }

            //In order to populate the list of certificates we make use of the fact that
            //OpenSSL supports encoding certificates to DER and ASN1C supports decoding from DER.
            //Therefore we first encode the cerificate to DER using OpenSSL and then decode it directly
            //to the ASN1c's Certificate_t structure using ASN1c decoding.
            //This is probably not the fastest way of doing this, but this code is not considered
            //performance critical. The biggest advantage of this solution is the simplicity,
            //as the alternative would be to manually transfer all fields one-by-one from one representation
            //to the other. This would require significant effort and would likely be error prone
            //due to lacking/insufficient documentation. In the current solution we rely on a standardized
            //representation when transcoding, so it should be reliable.

            //buffer to store the DER representation of the certificate
            unsigned char *buf = NULL;

            //encode the certificate to DER using openssl
            int len = i2d_X509(certificate_source, &buf);

            //check the operation status
            if (len <= 0)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CA-CAT-001 encoder: failed to encode der certificate");
                success = false;

                //in case buf has been allocated, release the memory
                if (buf != NULL)
                {
                    OPENSSL_free(buf);
                }
            }
            else
            {

                //create an instance of Certificate_t from the DER encoded content
                Certificate_t *certificate = 0;
                asn_dec_rval_t rval;

                //decode to the ASN1c structure
                rval = ber_decode(0, &asn_DEF_Certificate, (void**)&certificate, buf, len);

                //buf is not needed anymore, as the data is now in certificate
                OPENSSL_free(buf);

                //check operation status
                if (rval.code != RC_OK)
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CA-CAT-001 encoder: failed to decode ber certificate");
                    success = false;
                }
                else
                {
                    //add the certificate to the list of certificates
                    if (ASN_SEQUENCE_ADD(&(asn1c_ca_cat_001->certificates.list), certificate) != 0)
                    {
                        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CA-CAT-001 encoder: failed to add certificate");

                        //free the certificate structure
                        ASN_STRUCT_FREE(asn_DEF_Certificate, (void*)(certificate));
                        success = false;
                    }
                }
            }
        }

    }

    //in case the operation has failed but the asn1c_ca_cat_001 has beed successfully allocated, release it
    if (!success && (asn1c_ca_cat_001 != NULL))
    {
        ASN_STRUCT_FREE(*GetPayloadTypeDescriptor(), (void*)(asn1c_ca_cat_001));
        asn1c_ca_cat_001 = NULL;
    }

    return (void*) asn1c_ca_cat_001;
}

int CaCat001Signal::GetEncodeFlags() const
{
    return EncodeFlag::kNone;
}

fs_Encoding CaCat001Signal::GetPreferedEncoding ()
{
    return FS_ENCODING_DER;
}


} // namespace volvo_on_call
/** \}    end of addtogroup */
