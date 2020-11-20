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
 *  \file     encoding.h
 *  \brief    Foundation Services Encoding interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_ENCODING_H_INC_
#define FSM_ENCODING_H_INC_

#include <fscfg/encoding_interface.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

namespace fsm
{

/**
 * \brief Foundation Services Encoding
*/
class Encoding : public EncodingInterface
{
public:

    /**
     * \brief Encoding Constructor.
     *
     * \param[in] name of the encoding.
     */
    Encoding(const std::string& name);

    /**
     * \brief Encoding Destructor.
     */
    ~Encoding();

    /**
     * \brief Retrieves the name of the encoding.
     *
     * \param[out] name name of the encoding.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetName(std::string& name);

    /**
     * \brief Retrieves the D-Bus encoding object.
     *
     * \param[out] encoding reference to the D-Bus encoding object.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetObject(_ConfigEncoding*& encoding);

    /**
     * \brief Updates the properties of the D-Bus object and makes them visible on the bus.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode Update();

private:
    std::string name_;                   ///< encoding name.
    _ConfigEncoding* encoding_dbus_obj_; ///< D-Bus encoding object.
};

} // namespace fsm

#endif // FSM_ENCODING_H_INC_

/** \}    end of addtogroup */
