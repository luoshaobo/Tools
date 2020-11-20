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
 *  \file     ccm_decodable.h
 *  \brief    ccm decode util class
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_CCM_DECODABLE_H_
#define VOC_FRAMEWORK_SIGNALS_CCM_DECODABLE_H_

#include "voc_framework/signals/ccm.h"

namespace fsm
{

class CCMDecodable : public virtual CCM
{

 public:

    /**
     * \brief Get the session id of the wrapped ccm message.
     * \param[out] session_id The session id will be appended if set.
     * \return True if the session id is set, otherwise false.
     */
    bool GetSessionId(std::string& session_id);

    /**
     * \brief Get certificates attached in CCM (CMS), if any.
     *
     * \param[out] certificates Certificates attached to the CCM will be added to
     *                          this stack if any.
     *                          Caller must free these using X509_free,
     *                          sk_X509_pop_free or simillar once done with them.
     *
     * \return True if certificates available, false if not.
     */
    bool GetCertificates(STACK_OF(X509)* certificates);

 protected:

    /**
     * \brief Decodes the payload contained in the wrapped ccm.
     * \param[out] asn1c_packed_payload asn1c struct into which to decode the apyload
     * \param[in]  type_descriptor asn1c type descriptor for the struct
     * \param[in]  accepted_version the expected content version of the payload
     * \return true on success, false on failure
     */
    bool DecodePayload (void** asn1c_packed_payload,
                        asn_TYPE_descriptor_t* type_descriptor,
                        fs_VersionInfo accepted_version);

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_CCM_DECODABLE_H_

/** \}    end of addtogroup */
