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
 *  \file     expiry_notification_signal.cc
 *  \brief    Expiry notification signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include <fscfgd/expiry_notification_signal.h>

#include <dlt/dlt.h>

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

const std::string ExpiryNotificationSignal::oid_ = "1.3.6.1.4.1.37916.3.0.3";

std::shared_ptr<Signal> ExpiryNotificationSignal::CreateExpiryNotificationSignal(ccm_Message* ccm,
                                                             TransactionId& transaction_id)
{
    ExpiryNotificationSignal* signal = new(std::nothrow) ExpiryNotificationSignal(ccm, transaction_id);
    if (!signal || !signal->valid_)
    {
        delete signal;
        return std::shared_ptr<ExpiryNotificationSignal>();
    }
    else
    {
        return std::shared_ptr<ExpiryNotificationSignal>(signal);
    }
}



ExpiryNotificationSignal::ExpiryNotificationSignal (ccm_Message* ccm,
                                TransactionId& transaction_id) : CCM::CCM(ccm),
                                                                 Signal::Signal(transaction_id,
                                                                                kExpiryNotificationSignal)
{
    //asn1c struct to hold decoded data
    ExpiryNotification_t* asn1c_ExpiryNotification = NULL;

    //call super decode with input data from here
    valid_ = DecodePayload(reinterpret_cast<void**>(&asn1c_ExpiryNotification), &asn_DEF_ExpiryNotification, GetContentVersion ());

    if (valid_)
    {
        // extract the data and store in class members
        valid_ = UnpackPayload(asn1c_ExpiryNotification);
    }

    // free decoded asn1c struct
    asn_DEF_ExpiryNotification.free_struct(&asn_DEF_ExpiryNotification, asn1c_ExpiryNotification, 0);
}


#ifdef VOC_TESTS
ExpiryNotificationSignal::ExpiryNotificationSignal(fsm::CCMTransactionId& transaction_id,
                               uint16_t message_id): fsm::CCM::CCM(transaction_id, message_id),
                                                     fsm::Signal::Signal(transaction_id, kExpiryNotificationSignal)
{}

#endif

ExpiryNotificationSignal::~ExpiryNotificationSignal ()
{
}


std::vector<ExpiryNotificationSignal::ResourceInfo> ExpiryNotificationSignal::GetResources()

{
    return resources_;
}

fs_VersionInfo ExpiryNotificationSignal::GetContentVersion ()
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


bool ExpiryNotificationSignal::UnpackPayload(ExpiryNotification_t* asn1c_ExpiryNotification)
{
    bool success = false;

    //check if asn1c_ExpiryNotification are available
    if (asn1c_ExpiryNotification)
    {
        success = true;

        //check if resources are defined

        //loop through features and unpack them
        unsigned int num_resources = asn1c_ExpiryNotification->resources.list.count;

        resources_.clear();
        for (unsigned int i = 0; i < num_resources; i++)
        {
            struct Resource asn_Resource = *(asn1c_ExpiryNotification->resources.list.array[i]);

            ResourceInfo resource_info;

            //name
            ASN1String2StdString(asn_Resource.name,resource_info.name);
            //parameters (optional)
            if (asn_Resource.parameters)
            {
                ASN1String2StdString(*asn_Resource.parameters, resource_info.parameters);
            }
            //reconnect
            resource_info.reconnect = (asn_Resource.evictionType == EvictionType_reconnect);

            resources_.push_back(resource_info);
        }

    }
    return success;
}


/**************************************
 * virtual CCM function implementations
 **************************************/

#ifdef VOC_TESTS

const char* ExpiryNotificationSignal::GetOid ()
{
    return oid_.c_str();
}

asn_TYPE_descriptor_t* ExpiryNotificationSignal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_ExpiryNotification;
}

void ExpiryNotificationSignal::AddResource(const ResourceInfo& resource_info)
{
    resources_.push_back(resource_info);
}

void* ExpiryNotificationSignal::GetPackedPayload()
{

    ExpiryNotification_t* asn1c_ExpiryNotification = static_cast<ExpiryNotification_t*> (calloc(1, sizeof(ExpiryNotification_t)));
    bool success = false;

    if (asn1c_ExpiryNotification)
    {
        success = true;

        //pack features (optional)
        if (resources_.size() > 0)
        {
            for (unsigned int i = 0; i < resources_.size() && success; i++)
            {
                //we calloc to make sure everything is set to 0
                Resource_t* asn1c_resource = static_cast<Resource_t*> (calloc (1, sizeof(ExpiryNotification_t)));

                OCTET_STRING_fromBuf(&(asn1c_resource->name), resources_[i].name.data(), resources_[i].name.length());

                if (resources_[i].parameters.length()!=0)
                {
                asn1c_resource->parameters
                        = OCTET_STRING_new_fromBuf(&asn_DEF_UTF8String,
                                                   resources_[i].parameters.data(),
                                                   resources_[i].parameters.length());
                }

                if (resources_[i].reconnect)
                {
                    asn1c_resource->evictionType = EvictionType_reconnect;
                }
                else
                {
                    asn1c_resource->evictionType = EvictionType_invalidate;

                }

                if (!success || ASN_SEQUENCE_ADD(&(asn1c_ExpiryNotification->resources.list), asn1c_resource) != 0)
                {
                    success = false;
                    ASN_STRUCT_FREE(asn_DEF_Resource, asn1c_resource);
                }
            }
        }

        if (!success)
        {
            ASN_STRUCT_FREE(*GetPayloadTypeDescriptor(), reinterpret_cast<void*>(asn1c_ExpiryNotification));
            asn1c_ExpiryNotification = nullptr;
        }
    }

    return reinterpret_cast<void*>(asn1c_ExpiryNotification);
}

#endif


} // namespace fsm

/** \}    end of addtogroup */
