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
 *  \file     discovery_bind.h
 *  \brief    Foundation Services Discovery bindings handler interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_DISCOVERY_BIND_H_INC_
#define FSM_DISCOVERY_BIND_H_INC_

#include <fscfg/discovery_interface.h>

#include "multi_binding_set.h"

namespace fsm
{

/**
 * \brief Foundation Services Bind handler for the Discovery container.
 * Provides the house-keeping mechanism (manipulates all function callbacks) for binding sets related to the
 * Discovery container for both proxy daemon-side.
*/
class DiscoveryBind : public DiscoveryInterface
{
public:
    /**
     * \brief Constructor.
     */
    DiscoveryBind();

    /**
     * \copydoc fsm::DiscvoeryInterface::BindResourcesChanged
     */
    fscfg_ReturnCode BindResourcesChanged(ResourcesChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::DiscvoeryInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);

protected:
    /**
     * \brief Discovery base binds
     */
    enum BindBase
    {
        kResourcesChanged,  ///< ResourcesChanged binding set tuple member position.
        kBindBaseMax        ///< Maximum value in this enum. Keep this last.
    };

    /**
     * \brief Feature set changed D-Bus callback.
     *
     * \param[in] features set of features present on the bus after change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<FeatureInterface>>& features) = 0;

    MultiBindingSet<ResourcesChangedEvent> bind_base_; ///< base bindings.
};

} // namespace fsm

#endif // FSM_DISCOVERY_BIND_H_INC_

/** \}    end of addtogroup */
