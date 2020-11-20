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
 *  \file     fscfg.h
 *  \brief    Foundation Services Config top-level client factory interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fscfg
 *  \{
 */

#ifndef FSM_FSCFG_H_INC_
#define FSM_FSCFG_H_INC_


#include <fscfg/config_interface.h>
#include <fscfg/provisioning_interface.h>
#include <fscfg/discovery_interface.h>

namespace fsm
{

/**
 * \brief Getter for the ConfigInterface.
 * If the Proxy-side Config was not yet initialized, it gets initialized first.
 *
 * \return pointer to ConfigInterface.
 */
std::shared_ptr<ConfigInterface> GetConfigInterface();

/**
 * \brief Getter for the ProvisioningInterface.
 * If the Proxy-side Provisioning was not yet initialized, it gets initialized first.
 *
 * \return pointer to ProvisioningInterface.
 */
std::shared_ptr<ProvisioningInterface> GetProvisioningInterface();

/**
 * \brief Getter for the DiscoveryInterface.
 * If the Proxy-side Discovery was not yet initialized, it gets initialized first.
 *
 * \return pointer to DiscoveryInterface.
 */
std::shared_ptr<DiscoveryInterface> GetDiscoveryInterface();

} // namespace fsm

#endif // FSM_FSCFG_H_INC_

/** \}    end of addtogroup */
