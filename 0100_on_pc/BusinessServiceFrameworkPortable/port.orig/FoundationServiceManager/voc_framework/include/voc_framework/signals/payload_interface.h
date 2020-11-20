/**
 * Copyright (C) 2016, 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     payload_interface.h
 *  \brief    Interface to be realized by payload implementations.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_PAYLOAD_INTERFACE_H_
#define VOC_FRAMEWORK_SIGNALS_PAYLOAD_INTERFACE_H_

#include <memory>
#include <vector>

//asn1c
#include "asn_application.h"

//marben
#include "aipuptyp.h"

//fsm_ccm
#include "fsm_ccm.h"

//voc_framework
#include "voc_framework/signals/signal.h"


namespace fsm
{

/**
 * \brief PayloadInterface provides functions necessery for SignalFactory to
 *        encode/decode a payload using a CodecInterface impelementations. Typically
 *        to be used as content in a CCM.
 *
 *        A typical realization will support either encode or decode for one codec,
 *        in such cases either GetPayload or SetPayload should always return false.
 *
 *        Content which is received from the outside should only be decodable
 *        and content which is sent should only be encodable, this to avoid dead code
 *        in the prodcution system.
 *
 *        Cases may exist where both encode and decodae shall be supported, or several
 *        codecs.
 *
 *        Payload realizations may be as complex as they like and contain deeply nested
 *        structures, as long as they are able to pack/unpack everything into/from a
 *        format the codec understands. The intent however is to use nested PayloadInterface
 *        realizations in such cases, see for instance CcmSignal.
 *
 *        For normal cases a abstract utility base class realizing PayloadInterface and
 *        exposing a reduced interface will exist for each codec supported by the system.
 *        Deriving from these is the recommended way to implement a payload.
 */
class PayloadInterface
{

 public:

    /**
     * \brief Describes possible codecs
     *        a payload realization can support.
     */
    enum CodecType
    {
        kCodecTypeUndefined = 0,    //*< Undefined Codec type
        kCodecTypeLegacyCcm,        //*< Legacy CCMDecodable/Encodable
        kCodecTypeCcm,              //*< Legacy CCMDecodable/Encodable
        kCodecTypeMarben,           //*< Marben asn.1 codec
        kCodecTypeAsn1c,            //*< ASN.1C asn.1 codec
        kCodecTypeXml,              //*< Xml codec
        kCodecTypeApplicationDefined = 1000, //*< Applications may provide custom codecs with ids > this
    };

    /**
     * \brief VoC-specific EncodeFlags
     */
    enum EncodeFlag
    {
        kNone = 0x0, /**< Used to indicate no encode flags should be applied */
        kEncodeAsPayload = 0x01, /**< will be used to pass CCM_ENCODE_AS_PAYLOAD to FSM ccm encoder */
        kDoNotEncrypt = 0x02, /**< will be used to pass CCM_DO_NOT_ENCRYPT to FSM ccm encoder */
        kEncodeWithClientCredentials = 0x04 /**< Will use the TLS Client credentials for ccm encoding */
    };

    /**
     * \brief a bit field where each bit correspond to a EncodeFlag.
     */
    typedef int EncodeFlags;

    /**
     * \brief Holds the payload in a codec specific representation during decode/encode.
     */
    typedef struct CodecPayload
    {
        CodecType codec_type; //*< Codec type the CodecPayload is formatted for

        struct CodecData
        {
            struct CcmData
            {
                std::vector<fsm::UserId> recipients;    //*< Holds the recipients of the message as fsm users
                fsm::UserId sender;                     //*< Holds the senseder of the message as a fsm user
                STACK_OF(X509)* certificates = nullptr; //*< List of certificates extracted from CCM durign decode, if any
                ccm_Message* ccm; //*< The decoded CCM, or the CCM to encode

                /**
                 * \brief Constructs a CcmData.
                 */
                CcmData()
                {
                    ccm = nullptr;
                    sender = fsm::kUndefinedUserId;
                }

                /**
                 * \brief Destructs a CcmData, deöleting the contained message,
                 */
                ~CcmData()
                {
                    if (ccm)
                    {
                        delete ccm;
                    }
                }

            } ccm_data; //*< Holds a CCM and associated data needed for decode/encode

            std::shared_ptr<asntype> marben_data; //*< Marben base type

            struct Asn1cData
            {
                void** data_struct; //*< Double pointer to ASN.1C "type struct"
                asn_TYPE_descriptor_t* type_descriptor; //*< ASN.1C type descriptor
            } asn1c_data; //*< Container for ASN1C data

            struct XmlData
            {
                std::string data; //*< Xml data
            } xml_data;

        } codec_data; //*< Holds data in format according to codec_type

        struct EncodeData
        {
            fs_Encoding prefered_encoding; //*< The prefered encoding to use
            int encode_flags; //*< flags controlling encode operation
        } encode_data; //*< Holds encoding configuration data

        fs_VersionInfo version; //*< The verion of the payload which the PayloadInterface implements

    } CodecPayload;

    /**
     * \brief Identifies a payload.
     */
    typedef struct PayloadIdentifier
    {
        std::string oid; //*< Object Identifier for the payload, empty if none exist.
        //xml element?

        /**
         * \brief Compare this PayloadIdentifier to another for equality.
         *
         * \param[in] other Other PayloadIdentifeir to compare to.
         *
         * \return True if identifiers are equal, false otherwise.
         */
        bool operator==(const PayloadIdentifier& other) const
        {
            return oid == other.oid;
        }

        /**
         * \brief Compare this PayloadIdentifier to another for inequality.
         *
         * \param[in] other Other PayloadIdentifeir to compare to.
         *
         * \return True if identifiers are not equal, false otherwise.
         */
        bool operator<(const PayloadIdentifier& other) const
        {
            return oid < other.oid;
        }

    } PayloadIdentifier; //*< Unique content identifier, typically used during decode.

    /**
     * \brief Identifies a payload.
     */
    typedef struct ExtendedPayloadIdentifier
    {
        PayloadIdentifier payload_identifier; //*< identifier
        Signal::SignalType signal_type; //*< VOC framework signal type
    } ExtendedPayloadIdentifier; //*< Unique content identifier, typically used during decode.

    /**
     * \brief Returns a PayloadIdentifier for this payload.
     *
     * \return A PayloadIdentifier.
     */
    virtual ExtendedPayloadIdentifier GetIdentifier() const = 0;

    /**
     * \brief Returns a list of codecs supported by this payload.
     *
     * \return A list of codecs supported by this payload.
     */
    virtual std::vector<CodecType> GetSupportedCodecs() const = 0;

    /**
     * \brief Gets this payloads contents in format consumable by codec.
     *
     * \param[out] codec_payload  Will be populated with payload contents.
     *
     * \return False on failure, true on success.
     */
    virtual bool GetPayload(CodecPayload& codec_payload) const = 0;

    /**
     * \brief Intialize a CodecPayload for this payload so that
     *        is is ready to be populated by a codec and passed to
     *        SetPayload.
     *
     * \param[in/out] codec_payload CodecPayload to intialize.
     *
     * \return False on failure, otherwise true.
     */
    virtual bool GetEmptyPayload(CodecPayload& codec_payload) const = 0;

    /**
     * \brief Populate this payload with data from codec.
     *
     * \param[in] codec_payload Data to populate from.
     *
     * \return False on failure, true on success.
     */
    virtual bool SetPayload(const CodecPayload& codec_payload) = 0;

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_PAYLOAD_INTERFACE_H_

/** \}    end of addtogroup */
