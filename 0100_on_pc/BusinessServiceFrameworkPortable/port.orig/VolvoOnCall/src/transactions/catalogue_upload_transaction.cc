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
 *  \file     catalogue_upload_transaction.cc
 *  \brief    VOC Service catalogue upload transaction.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */
#include "keystore.h"

#include "features/car_access_feature.h"
#include "signals/ca_cat_001_signal.h"
#include "signals/ca_cat_002_signal.h"
#include "signals/signal_types.h"
#include "voc_framework/signal_sources/car_access_signal_source.h"
#include "transactions/catalogue_upload_transaction.h"


#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

CatalogueUploadTransaction::CatalogueUploadTransaction () : Transaction()
{
}

bool CatalogueUploadTransaction::WantsSignal(std::shared_ptr<fsm::Signal> signal)
{

    bool expected_signal = false;  //track if the signal is expected in this transaction

    //check if signal is expected in this transaction
    switch (signal->GetSignalType())
    {
    case VocSignalTypes::kCatalogueSignal :
    {
        if (state_ == kNew)
        {
            //special case, accept regardless of transaction id mapping
            expected_signal = true;
        }

        break;
    }
    case VocSignalTypes::kCaCat002Signal :
    {
        // First check if it matches our mapped transaction ids.
        //TODO: reinstate transaction id check

        if (signal->GetTransactionId() != upload_transaction_id_)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                           "CatalogueUploadTransaction, accepting ca_cat_002 with incorrect transaction id");
        }

        expected_signal = true;

        break;
    }
    default:
        expected_signal = false;
    }

    //the signal must be of expected type and must have a matching transaction id
    if (expected_signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CatalogueUploadTransaction, accepting signal: %s", signal->ToString().c_str());

    }

    return expected_signal;
}

bool CatalogueUploadTransaction::UploadCertificates()
{
    bool publishing_successful = false;

    std::shared_ptr<CaCat001Signal> upload_signal
            = CaCat001Signal::CreateCaCat001Signal(upload_transaction_id_,
                                                   1); // we do not send a sequence of messages

    fsm::CarAccessSignalSource& car_access_signal_source = fsm::CarAccessSignalSource::GetInstance();


    fsm::Keystore keystore;

    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CatalogueUploadTransaction, uploading certificates");


    //Process vehicle actor certificate
    X509* vehicle_actor_certificate = nullptr;

    if (fsm::ReturnCode::kSuccess == keystore.GetCertificate(&vehicle_actor_certificate,
                                                                                 fsm::Keystore::CertificateRole::kVehicleActor))
    {
        publishing_successful = upload_signal->AddCertificate(vehicle_actor_certificate);

        if (!publishing_successful )
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CatalogueUploadTransaction, failed to add vehicle actor certificate");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CatalogueUploadTransaction, added vehicle actor certificate");
        }

    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CatalogueUploadTransaction, failed to get the vehicle actor certificate");
    }

    //Process vehicle ca certificates
    STACK_OF(X509)* vehicle_ca_certificates = sk_X509_new_null();
    if (publishing_successful &&
        vehicle_ca_certificates != NULL &&
        fsm::ReturnCode::kSuccess == keystore.GetCertificates(vehicle_ca_certificates,
                                                                                  fsm::Keystore::CertificateRole::kVehicleCa))
    {
        for (int i = 0; (i < sk_X509_num(vehicle_ca_certificates)) && publishing_successful; i++) {

            X509 *vehicle_ca_certificate = nullptr;
            vehicle_ca_certificate = sk_X509_value(vehicle_ca_certificates, i);
            if (vehicle_ca_certificate != NULL &&
                upload_signal->AddCertificate(vehicle_ca_certificate))
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CatalogueUploadTransaction, added vehicle ca certificate number: %d\n", i);
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "CatalogueUploadTransaction, failed to add vehicle ca certificate number: %d\n",i);
                publishing_successful = false;
            }
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CatalogueUploadTransaction, failed to get vehicle CA certificate");
        publishing_successful = false;
    }
    /*
    //Process vehicle ca certificates
    STACK_OF(X509)* user_actor_certificates = sk_X509_new_null();
    if (publishing_successful &&
        user_actor_certificates != NULL &&
        fsm::ReturnCode::kSuccess == keystore.GetCertificates(user_actor_certificates,
                                                                                  fsm::Keystore::CertificateRole::kUserActor))
    {
        for (int i = 0; (i < sk_X509_num(user_actor_certificates)) && publishing_successful; i++) {

            X509 *user_actor_certificate = nullptr;
            user_actor_certificate = sk_X509_value(user_actor_certificates, i);

            if (user_actor_certificate!= NULL &&
                upload_signal->AddCertificate(user_actor_certificate))
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CatalogueUploadTransaction, added user actor certificate number: %d\n", i);
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "CatalogueUploadTransaction, failed to add user actor certificate number: %d\n",i);
                publishing_successful = false;
            }
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CatalogueUploadTransaction, failed to get user actor certificate\n");
        publishing_successful = false;
    }
    */
    // TODO: get CarAccess user? depends on decisions around backend app certificates
    if (publishing_successful && car_access_signal_source.PublishMessageToBackend(upload_signal, fsm::CarAccessSignalSource::kCatalogue))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CatalogueUploadTransaction, CA-CAT-001 published");
    }
    else
    {
        publishing_successful = false;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CatalogueUploadTransaction, "
                                               "Failed to publish CA-CAT-001.");
    }

    //free the certificates
    if (vehicle_actor_certificate != nullptr)
    {
        X509_free(vehicle_actor_certificate);
    }

    if (vehicle_ca_certificates != nullptr)
    {
        sk_X509_pop_free(vehicle_ca_certificates, X509_free);
    }
    /*
    if (user_actor_certificates != nullptr)
    {
        sk_X509_pop_free(user_actor_certificates, X509_free);
    }
    */
    return publishing_successful;
}

bool CatalogueUploadTransaction::HandleSignal(std::shared_ptr<fsm::Signal> signal)
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CatalogueUploadTransaction, incoming signal: %s", signal->ToString().c_str());
    switch (signal->GetSignalType())
    {
    case VocSignalTypes::kCatalogueSignal :
    {
        if (state_ == kNew)
        {
            state_ = kCertificatesUpload;
            if (!UploadCertificates())
            {
                state_ = kDone;
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                               "CarLocatorMapRequestTransaction, failed to upload certificates");
            }
        }
        break;
    }
    case (VocSignalTypes::kCaCat002Signal) :
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, received CA-CAT-002");
        //TODO: print out the response code
        std::shared_ptr<CaCat002Signal> ca_cat_002 = std::static_pointer_cast<CaCat002Signal>(signal);

        bool success = ca_cat_002->IsSuccess();
        long code = success ? (long) ca_cat_002->GetSuccessCode() : (long) ca_cat_002->GetErrorCode();
        std::string message = ca_cat_002->GetInfoMessage();

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                        "CatalogueUploadTransaction, received ca-cat-002: result %s code: %d message %s\n",
                        success ? "SUCCESS":"FAILURE",
                        code,
                        message.c_str());

        state_ = kDone;
        break;
    }
    default :
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "CarLocatorMapRequestTransaction, received "
                                              "unexpected signal %s, ignoring.", signal->ToString().c_str());
    }

    //if done then terminate the transaction
    return (state_ != kDone);

}


} // namespace volvo_on_call
/** \}    end of addtogroup */
