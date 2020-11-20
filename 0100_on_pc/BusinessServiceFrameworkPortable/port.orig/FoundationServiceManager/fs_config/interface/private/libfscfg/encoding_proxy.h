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
 *  \file     encoding_proxy.h
 *  \brief    Foundation Services Encoding proxy
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_ENCODING_PROXY_H_INC_
#define FSM_ENCODING_PROXY_H_INC_

#include <fscfg_common/generated/foundation-services-config-generated.h>

#include <fscfg/encoding_interface.h>

namespace fsm
{

class ConfigProxy;

/**
 * \brief Foundation Services Encoding proxy
 */
class EncodingProxy : public EncodingInterface
{
public:

    /**
     * \brief EncodingProxy Constructor.
     *
     * \param[in] config_proxy config proxy pointer.
     * \param[in] encoding_obj Dbus encoding object.
     */
    EncodingProxy(std::shared_ptr<fsm::ConfigProxy> config_proxy, _ConfigEncoding* encoding_obj);

    /**
     * \brief EncodingProxy Destructor.
     */
    ~EncodingProxy();

    /**
     * \copydoc fsm::EncodingInterface::GetName
     */
    fscfg_ReturnCode GetName(std::string& name);

private:
    _ConfigEncoding* encoding_dbus_obj__; ///< D-Bus encoding object.
};

} // namespace fsm

#endif // FSM_ENCODING_PROXY_H_INC_

/** \}    end of addtogroup */
