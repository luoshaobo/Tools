/*
* Copyright (C) 2018 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/
/**
 * @file vpom_IClient.hpp
 * @brief IClient for VPOM functionality (Volvo Positioning Manager)
 *
 *  \addtogroup vpom_IClient
 *  \{
 */

#ifndef VPOM_CLIENT_HPP
#define VPOM_CLIENT_HPP

#include "vpom_IPositioningService.hpp"

class VpomGdbusClientProxy;  // forward declaration

/**
 * @brief    Interface to be used by client apps to access the IPCommand functionality
 *
 */
class VpomIClient
{
private:
    static VpomGdbusClientProxy m_VpomGdbusClientProxy;  // instance of actual implementation (GDbus proxy)

    VpomIClient(); // Prevent construction
    VpomIClient(const VpomIClient&) = delete; // Prevent construction by copying
    VpomIClient& operator=(const VpomIClient&) = delete; // Prevent assignment

public:
    /**
     * @brief  Function called by client; needed mainly to check the pre-conditions and initialize underlying IPC initialization
     *
     * @return True if initialization was ok - the client can be used
     *         False otherwise - the client should NOT be used
     */
    static bool Init();

    /**
     * @brief   Function called by client to de-initialize the underlying IPC initialization
     *
     * @return  True if de-initialization was ok.
     *          False otherwise.
     */
    static bool Deinit();

    /**
     * @brief   Provides access to Positioning functions offerred by VPOM
     *
     * @return  Reference to PositioningSerivce
     */
    static VpomIPositioningService & GetPositioningService();
};

#endif //VPOM_CLIENT_HPP

/** \}    end of addtogroup */
