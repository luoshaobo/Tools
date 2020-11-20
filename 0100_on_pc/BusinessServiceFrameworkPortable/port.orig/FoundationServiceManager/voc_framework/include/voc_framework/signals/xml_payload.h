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
 *  \file     xml_payload.h
 *  \brief    XML payload representation.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_XML_PAYLOAD_H_
#define VOC_FRAMEWORK_SIGNALS_XML_PAYLOAD_H_


//voc_framework
#include "voc_framework/signals/payload_interface.h"
#include "voc_framework/signals/signal.h"


namespace fsm
{

/**
 * \brief XmlPayload is a uility base class for payloads using the xml codec.
 */

class XmlPayload : public PayloadInterface
{

 public:

    /**
     * \brief Pack the contents of this payload into a type
     *        ready for encoding.
     *
     * \param[out] data string to populate.
     *
     * \return False on failure, true on success.
     */
    virtual bool Pack(std::string& data) const;

    /**
     * \brief Populate this payload from a string
     *
     * \param data xml string to populate from.
     *
     * \retrun False on failure, otherwise true.
     */
    virtual bool Populate(const std::string& data);

    // ***
    // PayloadInterface realization start.
    // ***

    /**
     * \brief Returns the contained payloads identifier,
     *        or a empty identifier if there is no payload.
     *
     * \return A PaylaodIdentifier.
     */
    ExtendedPayloadIdentifier GetIdentifier() const;

    /**
     * \brief Returns a list of codecs supported by this payload.
     *
     * \return A list of codecs supported by this payload.
     */
    std::vector<CodecType> GetSupportedCodecs() const;

    /**
     * \brief Populate this payload with data from codec.
     *
     * \param[in] codec_payload Data to populate from.
     *
     * \return False on failure, true on success.
     */
    bool SetPayload(const CodecPayload& codec_payload);

    /**
     * \brief Gets this payloads contents in format consumable by codec.
     *
     * \param[out] codec_payload  Will be populated with payload contents.
     *
     * \return False on failure, true on success.
     */
    bool GetPayload(CodecPayload& codec_payload) const;

    /**
     * \brief Intialize a CodecPayload for this payload so that
     *        is is ready to be populated by a codec and passed to
     *        SetPayload.
     *
     * \param[in,out] codec_payload CodecPayload to intialize.
     *
     * \return False on failure, otherwise true.
     */
    bool GetEmptyPayload(CodecPayload& codec_payload) const;


    /**
     * \brief Constructs an Xml Payload.
     *
     */
    XmlPayload();

    // ***
    // PayloadInterface realization end.
    // ***


    /**
     * \brief Returns the XML data as a string
     *
     * \return string with the entire xml content
     */
    std::string GetXmlData() const;

    bool SetXmlData(const std::string data);


 protected:

    /**
     * \brief Constructs an xml Payload, useful when XmlPayload is used as a base class
     *
     * \param[in] signal_type      SignalType identifier for this payload.
     * \param[in] identifier       Xml identifier (root element name) for this payload.
     */
    XmlPayload(const Signal::SignalType signal_type, const std::string identifier);

 private:

    /**
     * \brief The signaltype for this payload.
     */
    Signal::SignalType signal_type_;

    /**
     * \brief Identifier for this payload.
     */
    std::string oid_;

    /**
     * \brief data.
     */
    std::string xml_data_;

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_XML_PAYLOAD_H_

/** \}    end of addtogroup */
