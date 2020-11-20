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
 *  \file     features_signal.cc
 *  \brief    Features signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/features_signal.h"
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

const std::string FeaturesSignal::oid_ = "1.3.6.1.4.1.37916.3.0.2";


bool FeaturesSignal::FeatureInfo::operator==(const FeatureInfo& a) const
{
    bool identical = true;
    identical = identical && a.name.compare(name)==0;
    identical = identical && a.enabled == enabled;
    identical = identical && a.visible == visible;
    identical = identical && a.name.compare(name)==0;
    identical = identical && a.uri.compare(uri)==0;
    identical = identical && a.icon.compare(icon)==0;
    identical = identical && a.description.compare(description)==0;

    bool same_number_of_tags = (a.tags.size() == tags.size());
    identical = identical && same_number_of_tags;

    if (identical)
    {
        for (size_t i=0; i<a.tags.size() && identical; i++ )
        {
             identical = identical && a.tags[i].compare(tags[i])==0;
        }
    }

    return identical;
}


std::shared_ptr<Signal> FeaturesSignal::CreateFeaturesSignal(ccm_Message* ccm,
                                                             TransactionId& transaction_id)
{
    FeaturesSignal* signal = new(std::nothrow) FeaturesSignal(ccm, transaction_id);
    if (!signal || !signal->valid)
    {
        delete signal;
        return std::shared_ptr<FeaturesSignal>();
    }
    else
    {
        return std::shared_ptr<FeaturesSignal>(signal);
    }
}



FeaturesSignal::FeaturesSignal (ccm_Message* ccm,
                                TransactionId& transaction_id) : CCM::CCM(ccm),
                                                                 Signal::Signal(transaction_id,
                                                                                kFeaturesSignal)
{
    //asn1c struct to hold decoded data
    Features_t* asn1c_Features = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_Features, &asn_DEF_Features, GetContentVersion ());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_Features);
    }

    // free decoded asn1c struct
    asn_DEF_Features.free_struct(&asn_DEF_Features, asn1c_Features, 0);
}


#ifdef VOC_TESTS
FeaturesSignal::FeaturesSignal(fsm::CCMTransactionId& transaction_id,
                               uint16_t message_id): fsm::CCM::CCM(transaction_id, message_id),
                                                     fsm::Signal::Signal(transaction_id, kFeaturesSignal)
{}

#endif

FeaturesSignal::~FeaturesSignal ()
{
}


std::vector<FeaturesSignal::FeatureInfo> FeaturesSignal::GetFeatures()

{
    return features_;
}


std::string FeaturesSignal::GetUri()
{
    return uri_;
}

fs_VersionInfo FeaturesSignal::GetContentVersion ()
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


bool FeaturesSignal::UnpackPayload(Features_t* asn1c_features)
{
    bool success = false;

    //check if asn1c_features are available
    if (asn1c_features)
    {
        success = true;

        //unpack thiz if present
        if (asn1c_features->thiz)
        {
            ASN1String2StdString(*asn1c_features->thiz, uri_);
        }

        //check if features are defined
        if (asn1c_features->feature)
        {
            //loop through features and unpack them
            unsigned int num_features = asn1c_features->feature->list.count;

            features_.clear();
            for (unsigned int i = 0; i < num_features; i++)
            {
                struct Feature asn1_feature = *(asn1c_features->feature->list.array[i]);
                FeatureInfo feature_info;
                ASN1String2StdString(asn1_feature.name,feature_info.name);
                feature_info.enabled = asn1_feature.enabled;
                feature_info.visible = asn1_feature.visible;
                ASN1String2StdString(asn1_feature.uri,feature_info.uri);

                if (asn1_feature.icon)
                {
                    ASN1String2StdString(*asn1_feature.icon, feature_info.icon);
                }

                if (asn1_feature.description)
                {
                    ASN1String2StdString(*asn1_feature.description, feature_info.description);
                }

                //unpack tags
                if (asn1_feature.tags)
                {
                    unsigned int num_tags = asn1_feature.tags->list.count;
                    for (unsigned int j = 0; j < num_tags; j++)
                    {
                        UTF8String_t asn1_tag = *(asn1_feature.tags->list.array[j]);
                        std::string tag;
                        ASN1String2StdString(asn1_tag, tag);
                        feature_info.tags.push_back(tag);
                    }
                }

                features_.push_back(feature_info);
            }
        }

    }
    return success;
}


/**************************************
 * virtual CCM function implementations
 **************************************/

#ifdef VOC_TESTS

const char* FeaturesSignal::GetOid ()
{
    return oid_.c_str();
}

asn_TYPE_descriptor_t* FeaturesSignal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_Features;
}

void FeaturesSignal::SetUri(const std::string& uri)
{
    uri_ = uri;
}

void FeaturesSignal::AddFeature(const FeatureInfo& feature_info)
{
    features_.push_back(feature_info);
}

void* FeaturesSignal::GetPackedPayload()
{

    Features_t* asn1c_features = (Features_t*) calloc(1, sizeof(Features_t));
    bool success = false;

    if (asn1c_features)
    {
        success = true;

        //pack uri (optional)
        if (uri_.size() != 0)
        {
            asn1c_features->thiz =
                    OCTET_STRING_new_fromBuf(&asn_DEF_Uri, uri_.data(), uri_.length());

            if (asn1c_features->thiz == NULL)
            {
                success = false;
            }
        }

        //pack features (optional)
        if (success && (features_.size() > 0) )
        {
            asn1c_features->feature = (struct Features::feature*)calloc(1, sizeof(struct Features::feature));

            for (unsigned int i = 0; i < features_.size() && success; i++)
            {
                //we calloc to make sure everything is set to 0
                Feature_t* asn1c_feature = (Feature_t*) calloc (1, sizeof(Feature_t));

                //populate feature
                OCTET_STRING_fromBuf(&(asn1c_feature->name), features_[i].name.data(), features_[i].name.length());
                asn1c_feature->enabled = features_[i].enabled;
                asn1c_feature->visible = features_[i].visible;
                OCTET_STRING_fromBuf(&(asn1c_feature->uri), features_[i].uri.data(), features_[i].uri.length());

                asn1c_feature->description = OCTET_STRING_new_fromBuf(&asn_DEF_UTF8String, features_[i].description.data(), features_[i].description.length());

                //add tags
                if (features_[i].tags.size()>0)
                {
                    asn1c_feature->tags = (struct Feature::tags*)calloc(1, sizeof(struct Feature::tags));
                    for (int j=0; j<features_[i].tags.size() && success; j++)
                    {
                        Tag_t* asn1c_tag = (Tag_t*) calloc (1, sizeof(Tag_t));
                        OCTET_STRING_fromBuf(asn1c_tag, features_[i].tags[j].data(), features_[i].tags[j].length());
                        if (ASN_SEQUENCE_ADD(&(asn1c_feature->tags->list),
                                             asn1c_tag) != 0)
                        {
                            success = false;
                            ASN_STRUCT_FREE(asn_DEF_Tag, asn1c_tag);
                        }
                    }

                }

                if (!success || ASN_SEQUENCE_ADD(&(asn1c_features->feature->list), asn1c_feature) != 0)
                {
                    success = false;
                    ASN_STRUCT_FREE(asn_DEF_Feature, asn1c_feature);
                }
            }
        }

        if (!success)
        {
            ASN_STRUCT_FREE(*GetPayloadTypeDescriptor(), (void*)(asn1c_features));
            asn1c_features = nullptr;
        }
    }

    return (void*)asn1c_features;
}

#endif


} // namespace fsm

/** \}    end of addtogroup */
