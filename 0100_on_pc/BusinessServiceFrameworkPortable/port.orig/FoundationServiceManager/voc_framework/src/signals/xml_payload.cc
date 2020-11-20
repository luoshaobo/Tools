/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     xml_payload.cc
 *  \brief    XML payload representation.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/xml_payload.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{


bool XmlPayload::Pack(std::string& data) const
{
    //TODO: in future we can consider some validity checks
    data = xml_data_;
    return true;
}


bool XmlPayload::Populate(const std::string& data)
{
    //TODO: in future we can consider some validity checks
    xml_data_ = data;
    return true;
}

PayloadInterface::ExtendedPayloadIdentifier XmlPayload::GetIdentifier() const
{
    ExtendedPayloadIdentifier identifier = ExtendedPayloadIdentifier();

    identifier.signal_type = Signal::BasicSignalTypes::kXmlSignal;
    identifier.payload_identifier.oid = oid_;

    return identifier;
}

std::vector<PayloadInterface::CodecType> XmlPayload::GetSupportedCodecs() const
{
    return {kCodecTypeXml};
}

bool XmlPayload::SetPayload(const CodecPayload& codec_payload)
{
    bool return_value = true;

    if (codec_payload.codec_type == kCodecTypeXml &&
            codec_payload.codec_data.xml_data.data.size() > 0)
    {
        return_value = Populate(codec_payload.codec_data.xml_data.data);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "XmlPayload::SetPayload on payload: %s with bad input. Codec type: %d, data size: %d",
                        oid_.c_str(), codec_payload.codec_type, codec_payload.codec_data.xml_data.data.size());
        return_value = false;
    }

    return return_value;
}

bool XmlPayload::GetPayload(CodecPayload& codec_payload) const
{
    bool return_value = true;


    if (xml_data_.size() > 0)
    {
        return_value = Pack(codec_payload.codec_data.xml_data.data);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "GetPayload on payload: %s, no data to pack.",
                        oid_.c_str());
        return_value = false;
    }

    if (return_value)
    {
        codec_payload.codec_data.xml_data.data = xml_data_;
        codec_payload.codec_type = kCodecTypeXml;

    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "GetPayload on payload: %s, failed to pack payload.",
                        oid_.c_str());

        //codec_payload.codec_data.xml_data.data = nullptr;
        codec_payload.codec_data.xml_data.data = "";//modify klocwork warning,yangjun add 20181107
    }

    return return_value;
}

bool XmlPayload::GetEmptyPayload(CodecPayload& codec_payload) const
{
    codec_payload.codec_type = kCodecTypeXml;
    codec_payload.codec_data.xml_data.data = std::string();
    return true;
}


XmlPayload::XmlPayload() : signal_type_(Signal::kXmlSignal),
                           oid_("XML"),
                           xml_data_(std::string())
{
}


XmlPayload::XmlPayload(const Signal::SignalType signal_type, const std::string identifier) :
    signal_type_(signal_type), oid_(identifier), xml_data_(std::string())
{
}

std::string XmlPayload::GetXmlData() const
{
    return xml_data_;
}

bool XmlPayload::SetXmlData(const std::string data)
{
    /*
     * TODO: In the future we may consider including some level of XML validation
     */

    xml_data_ = data;

    return true;
}

} // namespace fsm


/** \}    end of addtogroup */
