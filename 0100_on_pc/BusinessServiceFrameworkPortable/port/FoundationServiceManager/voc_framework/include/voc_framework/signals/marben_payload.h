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
 *  \file     marben_payload.h
 *  \brief    Signal subclass representing a CCM.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_MARBEN_PAYLOAD_H_
#define VOC_FRAMEWORK_SIGNALS_MARBEN_PAYLOAD_H_

#include "dlt/dlt.h"

//voc_framework
#include "voc_framework/signals/payload_interface.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

/**
 * \brief MarbenPayload is a uility base class for payloads using the marben codec.
 */
template<typename T>
class MarbenPayload : public PayloadInterface
{

 public:

    /**
     * \brief Pack the contents of this payload into a asntype
     *        ready for encoding.
     *
     * \param[in/out] asn_type asntype to populate.
     *
     * \return False on failure, true on success.
     */
    virtual bool Pack(T& asn_type) const = 0;

    /**
     * \brief Populate this payload from asn_type
     *
     * \param asn_type Marben asn_type instance to populate from.
     *
     * \retrun False on failure, otherwise true.
     */
    virtual bool Populate(const T& asn_type) = 0;

    // ***
    // PayloadInterface realization start.
    // ***

    /**
     * \brief Returns the contained payloads identifier,
     *        or a empty identifier if there is no payload.
     *
     * \return A PaylaodIdentifier.
     */
    ExtendedPayloadIdentifier GetIdentifier() const
    {
        ExtendedPayloadIdentifier identifier = ExtendedPayloadIdentifier();

        identifier.signal_type = signal_type_;
        identifier.payload_identifier.oid = oid_;

        return identifier;
    }

    /**
     * \brief Returns a list of codecs supported by this payload.
     *
     * \return A list of codecs supported by this payload.
     */
    std::vector<CodecType> GetSupportedCodecs() const
    {
        return {kCodecTypeMarben};
    }

    /**
     * \brief Populate this payload with data from codec.
     *
     * \param[in] codec_payload Data to populate from.
     *
     * \return False on failure, true on success.
     */
    bool SetPayload(const CodecPayload& codec_payload)
    {
        bool return_value = true;

        if (codec_payload.codec_type == kCodecTypeMarben &&
            codec_payload.version == supported_version_ &&
            codec_payload.codec_data.marben_data)
        {
            std::shared_ptr<T> typed_asn_type = nullptr;
            typed_asn_type = std::dynamic_pointer_cast<T>(codec_payload.codec_data.marben_data);

            if (typed_asn_type)
            {
                return_value = Populate(*typed_asn_type);
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "Failed to populate payload for payload: %s.",
                                oid_.c_str());
                return_value = false;
            }
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "SetPayload on payload: %s with bad input.",
                            oid_.c_str());
            return_value = false;
        }

        return return_value;
    }

    /**
     * \brief Gets this payloads contents in format consumable by codec.
     *
     * \param[out] codec_payload  Will be populated with payload contents.
     *
     * \return False on failure, true on success.
     */
    bool GetPayload(CodecPayload& codec_payload) const
    {
        bool return_value = true;

        std::shared_ptr<T> typed_asntype = std::make_shared<T>();

        if (typed_asntype)
        {
            return_value = Pack(*typed_asntype);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "GetPayload on payload: %s, failed to allocate codec_data.",
                            oid_.c_str());
            return_value = false;
        }

        if (return_value)
        {
            codec_payload.codec_data.marben_data = typed_asntype;
            codec_payload.codec_type = kCodecTypeMarben;
            codec_payload.version = supported_version_;
            codec_payload.encode_data.prefered_encoding = prefered_encoding_;
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "GetPayload on payload: %s, failed to pack payload.",
                            oid_.c_str());

            codec_payload.codec_data.marben_data = nullptr;
        }

        return return_value;
    }

    /**
     * \brief Intialize a CodecPayload for this payload so that
     *        is is ready to be populated by a codec and passed to
     *        SetPayload.
     *
     * \param[in/out] codec_payload CodecPayload to intialize.
     *
     * \return False on failure, otherwise true.
     */
    bool GetEmptyPayload(CodecPayload& codec_payload) const
    {
        bool return_value = true;

        codec_payload.codec_data.marben_data = std::make_shared<T>();

        if (codec_payload.codec_data.marben_data)
        {
            codec_payload.codec_type = kCodecTypeMarben;
            codec_payload.version = supported_version_;
            codec_payload.encode_data.prefered_encoding = prefered_encoding_;
        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "GetEmptyPayload on payload: %s, failed to allocate codec_data.",
                            oid_.c_str());
            return_value = false;
        }

        return return_value;
    }

    // ***
    // PayloadInterface realization end.
    // ***

 protected:

    /**
     * \brief Constructs a Marben Payload.
     *
     * \param[in] oid               Oid identifer for this payload.
     * \param[in] signal_type       SignalType identifier for this payload.
     * \param[in] supported_version Version of thie payload which is supported.
     */
    MarbenPayload(std::string oid,
                  Signal::SignalType signal_type,
                  long schema_version,
                  long preliminary_version,
                  long signal_flow_version,
                  fs_Encoding prefered_encoding = FS_ENCODING_UPER) :
        signal_type_(signal_type),
        oid_(oid),
        prefered_encoding_(prefered_encoding)
    {
                supported_version_.schema_version = schema_version;
                supported_version_.preliminary_version = preliminary_version;
                supported_version_.signal_flow_version = signal_flow_version;
    }

 private:

    /**
     * \brief The signaltype for this payload.
     */
    Signal::SignalType signal_type_;

    /**
     * \brief The oid for this payload.
     */
    std::string oid_;

    /**
     * \brief The version of this payload supported.
     */
    fs_VersionInfo supported_version_;

    /**
     * \brief The prefered asn.1 encoding rule for
     *        this payload.
     */
    fs_Encoding prefered_encoding_;

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_MARBEN_PAYLOAD_H_

/** \}    end of addtogroup */
