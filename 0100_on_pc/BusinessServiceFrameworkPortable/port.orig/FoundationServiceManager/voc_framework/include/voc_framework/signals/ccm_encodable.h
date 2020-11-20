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
 *  \file     ccm_encodable.h
 *  \brief    ccm encoding util class
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_CCM_ENCODABLE_H_
#define VOC_FRAMEWORK_SIGNALS_CCM_ENCODABLE_H_

#include "voc_framework/signals/ccm.h"

#include "usermanager_types.h"

namespace fsm
{

class CCMEncodable : public virtual CCM
{

 public:

    /**
     * \brief Encode the wrapped ccm message.
     *
     * \return return_shared_buffer_ptr shared ponter to buffer, or null on failure.
     */
    std::shared_ptr<std::vector<unsigned char>> Encode();


    /**
     * \brief Encode this CCMs payload as asn1c and store it in message.
     *
     * \param[in/out] ccm On success encoded payload will be stored in ccm.
     *
     * \return true if sucessfull, false if operation failed
     */
    bool EncodePayload(ccm_Message& ccm);

    /**
     * \brief Gets the oid of the payload content.
     * \return the oid to set
     */
    virtual const char* GetOid () = 0;

    /**
     * \brief Set the session id of the wrapped ccm message.
     *
     * \param[in] session_id The seesion id to set.
     *            Should be CCM_SESSION_ID_LEN long.
     */
    void SetSessionId (std::string session_id);

    /**
     * \brief Set the store and forward option. This option is used by cloud to decide how to deliver the message.
     *
     * \param[in] store_and_forward_options store and forward option to be used by cloud
     */
    void SetStoreAndForwardOptions(const ccm_storeAndForwardOptions store_and_forward_options);

    /**
     * \brief Set the recipients of the CCM.
     *
     * \param[in] recipients The recipients of the CCM.
     */
    void SetRecipients(const std::vector<fsm::UserId>& recipients);

    /**
     * \brief Get the recipients of the CCM.
     *
     * \return The recipients of the CCM.
     */
    std::vector<fsm::UserId> GetRecipients();

    /**
     * \brief Provide the fsm/ccm encode flags to use when encoding, if any.
     *        Default impelementation will return kEncodeAsPayload,
     *        derived class should override if needed.
     * \return The flags to use.
     */
    virtual int GetEncodeFlags() const;

protected:

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    virtual fs_VersionInfo GetContentVersion () = 0;

    /**
     * \brief Pack CCM payload into ASN1C structure, used while encoding payload
     * \return pointer to asn1c struct containing payload, caller must free.
     */
    virtual void* GetPackedPayload () = 0;

    /**
     * \brief Provides the ASN1C type descriptor for the CCM payload
     * \return pointer to ASN1C type decriptor, caller must not free
     */
    virtual asn_TYPE_descriptor_t* GetPayloadTypeDescriptor () = 0;

    /**
     * \brief Provides the prefered encoding for the CCM payload
     *        Default impelementation will return FS_ENCODING_UPER,
     *        derived class should override if needed.
     *        Currently only FS_ENCODING_DER and FS_ENCODING_UPER
     *        are supported.
     * \return prefered fs_Encoding, or FS_ENCODING_UNDEFINED for default
     */
    virtual fs_Encoding GetPreferedEncoding ();

 private:
    /**
     * \brief Recipients of this CCM.
     */
    std::vector<fsm::UserId> recipients_;
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_BCC_FUNC_001_SIGNAL_ENCODABLE_H_

/** \}    end of addtogroup */
