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
 *  \file     encoding_interface.h
 *  \brief    Foundation Services Encoding subject interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_ENCODING_INTERFACE_H_INC_
#define FSM_ENCODING_INTERFACE_H_INC_

#include <cstdint>
#include <vector>
#include <memory>

#include "fscfg_types.h"

namespace fsm
{

/**
 * \brief Foundation Services Encoding subject interface.
*/
class EncodingInterface
{
public:
    /**
     * \brief virtual Destructor.
     */
    virtual ~EncodingInterface() { }

    /**
     * \brief Retrieves the name of the encoding instance.
     *
     * \param[out] name name of the encoding instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetName(std::string& name) = 0;
};

} // namespace fsm

#endif // FSM_ENCODINGINTERFACE_H_INC_

/** \}    end of addtogroup */
