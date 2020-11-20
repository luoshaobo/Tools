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
 *  \file     car_access_signal.cc
 *  \brief    car access signal
 *  \author   Gustav Evertsson
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/car_access_signal.h"
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

const std::string CarAccessSignal::oid_ = "1.3.6.1.4.1.37916.3.8.2.0"; //oid introduced in version 1.8

std::shared_ptr<Signal> CarAccessSignal::CreateCarAccessSignal(ccm_Message* ccm,
                                                               TransactionId& transaction_id)
{
    CarAccessSignal* signal = new CarAccessSignal(ccm, transaction_id);
    if (!signal || !signal->valid)
    {
        delete signal;
        return std::shared_ptr<CarAccessSignal>();
    }
    else
    {
        return std::shared_ptr<CarAccessSignal>(signal);
    }
}


CarAccessSignal::CarAccessSignal (ccm_Message* ccm,
                                  TransactionId& transaction_id) : CCM::CCM(ccm),
                                  Signal::Signal(transaction_id, kCarAccessSignal),
                                  uplink_topics_(new std::vector<CarAccessSignal::MqttTopicInfo>()),
                                  downlink_topics_(new std::vector<CarAccessSignal::MqttTopicInfo>())
{
    //asn1c struct to hold decoded data
    CarAccess_t* asn1c_car_access = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_car_access, &asn_DEF_CarAccess, GetContentVersion ());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_car_access);
    }

    // free decoded asn1c struct
    asn_DEF_CarAccess.free_struct(&asn_DEF_CarAccess, asn1c_car_access, 0);
}

CarAccessSignal::~CarAccessSignal ()
{
    // free any instance data that needs freeing
}

bool CarAccessSignal::GetAddressUri(std::string &uri)
{
    if (address_type_ != AddressType::Uri)
    {
        return false;
    }
    else
    {
        uri = address_uri_;
        return true;
    }
}

bool CarAccessSignal::GetAddressInstanceId(UUID &instanceId)
{
    if (address_type_ != AddressType::InstanceId)
    {
        return false;
    }
    else
    {
        memcpy(instanceId, address_instance_, FS_UUID_LEN);
        return true;
    }
}

std::string CarAccessSignal::GetCatalogueUplink()
{
    return catalogue_uplink_topic_;
}

std::string CarAccessSignal::GetCatalogueDownlink()
{
    return catalogue_downlink_topic_;
}

std::string CarAccessSignal::GetDelegateUplink()
{
    return delegate_uplink_topic_;
}

std::string CarAccessSignal::GetDelegateDownlink()
{
    return delegate_downlink_topic_;
}

std::string CarAccessSignal::GetDevicePairingUplink()
{
    return device_pairing_uplink_topic_;
}

std::string CarAccessSignal::GetDevicePairingDownlink()
{
    return device_pairing_downlink_topic_;
}

std::shared_ptr<std::vector<CarAccessSignal::MqttTopicInfo> > CarAccessSignal::GetUplinkTopics()
{
    return uplink_topics_;
}

std::shared_ptr<std::vector<CarAccessSignal::MqttTopicInfo> > CarAccessSignal::GetDownlinkTopics()

{
    return downlink_topics_;
}

fs_VersionInfo CarAccessSignal::GetContentVersion ()
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
void CarAccessSignal::Asn1TopicsInfo2MqttTopicsInfo(struct TopicInfo &asn1_topic_info, CarAccessSignal::MqttTopicInfo &topic_info)
{
    ASN1String2StdString(asn1_topic_info.topic, topic_info.topic);

    topic_info.priority = asn1_topic_info.priority;

    if(asn1_topic_info.services)
    {
        unsigned int num_services = asn1_topic_info.services->list.count;
        for (unsigned int i = 0; i < num_services; i++)
        {
            struct ServiceInfo asn1_service_info = *(asn1_topic_info.services->list.array[i]);
            MqttServiceInfo service_info;
            fs_Oid fs_oid;
            if (! ASN1C2FSOid(&(asn1_service_info.oid), &fs_oid) )
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                                "Received unsupported service oid type in CarAccessSignal:\n ");
                break;
            }
            service_info.oid.assign((const char*)(&fs_oid.oid), strlen((const char*)(&fs_oid.oid)));
            topic_info.services.push_back(service_info);
        }
    }
}

bool CarAccessSignal::UnpackPayload(CarAccess_t* asn1c_ca_FUNC_002)
{
    //check if resources are available
    if (asn1c_ca_FUNC_002)
    {
        bool success = false;

        switch (asn1c_ca_FUNC_002->resources.thiz.present)
        {
            case Address_PR::Address_PR_uri:
                address_type_ = Uri;
                ASN1String2StdString(asn1c_ca_FUNC_002->resources.thiz.choice.uri, address_uri_);
                break;

            case Address_PR::Address_PR_instanceId:
                address_type_ = InstanceId;
                success  = ASN1C2OctetString(&(asn1c_ca_FUNC_002->resources.thiz.choice.instanceId),
                                             (char*)&address_instance_, FS_UUID_LEN);
                //if failed to decode the string (e.g. incorrect length) then exit
                if (!success)
                {
                    return false;
                }
                break;
            default:
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "Received unsupported address type in CarAccessSignal: %d\n ",asn1c_ca_FUNC_002->resources.thiz.present);
                return false;
        }


        ASN1String2StdString(asn1c_ca_FUNC_002->resources.catalogueUl, catalogue_uplink_topic_);
        ASN1String2StdString(asn1c_ca_FUNC_002->resources.catalogueDl, catalogue_downlink_topic_);

        ASN1String2StdString(asn1c_ca_FUNC_002->resources.delegateUl, delegate_uplink_topic_);
        ASN1String2StdString(asn1c_ca_FUNC_002->resources.delegateDl, delegate_downlink_topic_);

        ASN1String2StdString(asn1c_ca_FUNC_002->resources.devicePairingUl, device_pairing_uplink_topic_);
        ASN1String2StdString(asn1c_ca_FUNC_002->resources.devicePairingDl, device_pairing_downlink_topic_);

        unsigned int num_topics = asn1c_ca_FUNC_002->uplinkTopics.list.count;
        for (unsigned int i = 0; i < num_topics; i++)
        {
            struct TopicInfo asn1_topic_info = *(asn1c_ca_FUNC_002->uplinkTopics.list.array[i]);
            MqttTopicInfo topic_info;
            Asn1TopicsInfo2MqttTopicsInfo(asn1_topic_info, topic_info);
            uplink_topics_->push_back(topic_info);

        }

        num_topics = asn1c_ca_FUNC_002->downlinkTopics.list.count;
        for (unsigned int i = 0; i < num_topics; i++)
        {
            struct TopicInfo asn1_topic_info = *(asn1c_ca_FUNC_002->downlinkTopics.list.array[i]);
            MqttTopicInfo topic_info;
            Asn1TopicsInfo2MqttTopicsInfo(asn1_topic_info, topic_info);
            downlink_topics_->push_back(topic_info);
        }
    }

    return true;
}

#ifdef UNIT_TESTS

CarAccessSignal::CarAccessSignal (CCMTransactionId& transaction_id,
                                  uint16_t sequence_number)
                 : CCM::CCM(transaction_id, sequence_number),
                   Signal::Signal(transaction_id, kCarAccessSignal),
                   uplink_topics_(new std::vector<CarAccessSignal::MqttTopicInfo>()),
                   downlink_topics_(new std::vector<CarAccessSignal::MqttTopicInfo>())
{}

void CarAccessSignal::SetAdressUri(std::string address)
{
    address_uri_ = address;
    address_type_ = Uri;
}

void CarAccessSignal::SetAddressInstanceId(UUID address)
{
    memcpy(&address_instance_, address, FS_UUID_LEN);// address_instance = address;
    address_type_ = InstanceId;
}

void CarAccessSignal::SetCatalogueUplink(std::string topic)
{
    catalogue_uplink_topic_ = topic;
}

void CarAccessSignal::SetCatalogueDownlink(std::string topic)
{
    catalogue_downlink_topic_ = topic;
}

void CarAccessSignal::SetDelegateUplink(std::string topic)
{
    delegate_uplink_topic_ = topic;
}

void CarAccessSignal::SetDelegateDownlink(std::string topic)
{
    delegate_downlink_topic_ = topic;
}

void CarAccessSignal::SetDevicePairingUplink(std::string topic)
{
    device_pairing_uplink_topic_ = topic;
}

void CarAccessSignal::SetDevicePairingDownlink(std::string topic)
{
    device_pairing_downlink_topic_ = topic;
}

void CarAccessSignal::AddUplinkTopic(MqttTopicInfo topic)
{
    uplink_topics_->push_back(topic);
}

void CarAccessSignal::AddDownlinkTopic(MqttTopicInfo topic)
{
    downlink_topics_->push_back(topic);
}

void CarAccessSignal::MqttTopicsInfo2Asn1TopicsInfo(fsm::CarAccessSignal::MqttTopicInfo &topic_info,
                                                        TopicInfo_t &asn1c_topic_info)
{
    if ( 0 != OCTET_STRING_fromBuf(&(asn1c_topic_info.topic),
        topic_info.topic.c_str(), topic_info.topic.length()) )
    {
        return;
    }

    asn1c_topic_info.priority = topic_info.priority;
    asn1c_topic_info.mqttProtocol = MqttProtocol_ccm;

    if (!topic_info.services.empty()) {
        asn1c_topic_info.services = (struct TopicInfo::services*)calloc(1, sizeof(struct TopicInfo::services));
    }

    for (fsm::CarAccessSignal::MqttServiceInfo service_info : topic_info.services)
    {
        ServiceInfo_t* asn1c_service_info = (ServiceInfo_t*) calloc (1, sizeof(ServiceInfo_t));

        if (asn1c_service_info == NULL)
        {
            return;
        }

        fs_Oid fs_oid;
        ccm_SetOid(&fs_oid, service_info.oid.c_str());
        FSOid2ASN1C (&fs_oid, &asn1c_service_info->oid);


        if (ASN_SEQUENCE_ADD(&(asn1c_topic_info.services->list),
            asn1c_service_info) != 0)
        {
            return;
        }
    }
}

void* CarAccessSignal::GetPackedPayload()
{
    CarAccess_t* asn1c_ca_func_002 = (CarAccess_t*) calloc(1, sizeof(CarAccess_t));

    // address
    bool addressDecoded = true;
    asn_TYPE_descriptor_t* type_descriptor = GetPayloadTypeDescriptor();
    //Handle resources, currently only address type
    if (address_type_ != None)
    {

        switch (address_type_)
        {
            case Uri:
                asn1c_ca_func_002->resources.thiz.present = Address_PR::Address_PR_uri;
                if ( 0 != OCTET_STRING_fromBuf(&(asn1c_ca_func_002->resources.thiz.choice.uri),
                    address_uri_.c_str(), address_uri_.length()) )
                {
                    addressDecoded = false;
                }

                break;

            case InstanceId:
                asn1c_ca_func_002->resources.thiz.present = Address_PR::Address_PR_instanceId;
                if ( 0 != OCTET_STRING_fromBuf(&(asn1c_ca_func_002->resources.thiz.choice.instanceId),
                    (char*)&address_instance_, FS_UUID_LEN ) )
                {
                    addressDecoded = false;
                }

                break;

            default:
                //unsupported format, error
                addressDecoded = false;
        }

    }

    if (addressDecoded == false)
    {
        ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
        return NULL;
    }



    // topics in the resources struct
    if ( 0 != OCTET_STRING_fromBuf(&(asn1c_ca_func_002->resources.catalogueUl),
        catalogue_uplink_topic_.c_str(), catalogue_uplink_topic_.length()) )
    {
        ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
        return NULL;
    }
    if ( 0 != OCTET_STRING_fromBuf(&(asn1c_ca_func_002->resources.catalogueDl),
        catalogue_downlink_topic_.c_str(), catalogue_downlink_topic_.length()) )
    {
        ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
        return NULL;
    }

    if ( 0 != OCTET_STRING_fromBuf(&(asn1c_ca_func_002->resources.delegateUl),
        delegate_uplink_topic_.c_str(), delegate_uplink_topic_.length()) )
    {
        ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
        return NULL;
    }
    if ( 0 != OCTET_STRING_fromBuf(&(asn1c_ca_func_002->resources.delegateDl),
        delegate_downlink_topic_.c_str(), delegate_downlink_topic_.length()) )
    {
        ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
        return NULL;
    }

    if ( 0 != OCTET_STRING_fromBuf(&(asn1c_ca_func_002->resources.devicePairingUl),
        device_pairing_uplink_topic_.c_str(), device_pairing_uplink_topic_.length()) )
    {
        ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
        return NULL;
    }
    if ( 0 != OCTET_STRING_fromBuf(&(asn1c_ca_func_002->resources.devicePairingDl),
        device_pairing_downlink_topic_.c_str(), device_pairing_downlink_topic_.length()) )
    {
        ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
        return NULL;
    }

    // uplink
    for (fsm::CarAccessSignal::MqttTopicInfo topic_info : *uplink_topics_)
    {
        TopicInfo_t* asn1c_topic_info = (TopicInfo_t*) calloc (1, sizeof(TopicInfo_t));

        if (asn1c_topic_info == NULL)
        {
            ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
            return NULL;
        }

        MqttTopicsInfo2Asn1TopicsInfo(topic_info, *asn1c_topic_info);

        if (ASN_SEQUENCE_ADD(&(asn1c_ca_func_002->uplinkTopics.list),
            asn1c_topic_info) != 0)
        {
            ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
            return NULL;
        }
    }

    // downlink
    for (fsm::CarAccessSignal::MqttTopicInfo topic_info : *downlink_topics_)
    {
        TopicInfo_t* asn1c_topic_info = (TopicInfo_t*) calloc (1, sizeof(TopicInfo_t));

        if (asn1c_topic_info == NULL)
        {
            ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
            return NULL;
        }

        MqttTopicsInfo2Asn1TopicsInfo(topic_info, *asn1c_topic_info);

        if (ASN_SEQUENCE_ADD(&(asn1c_ca_func_002->downlinkTopics.list),
            asn1c_topic_info) != 0)
        {
            ASN_STRUCT_FREE(*type_descriptor, (void*)(asn1c_ca_func_002));
            return NULL;
        }
    }

    return (void*) asn1c_ca_func_002;
}

#endif //UNIT_TESTS

} // namespace fsm

/** \}    end of addtogroup */
