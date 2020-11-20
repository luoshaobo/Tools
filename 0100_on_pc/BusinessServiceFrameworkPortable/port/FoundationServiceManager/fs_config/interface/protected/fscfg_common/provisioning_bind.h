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
 *  \file     provisioning_bind.h
 *  \brief    Foundation Services provisioning bindings handler interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_PROVISIONING_BIND_H_INC_
#define FSM_PROVISIONING_BIND_H_INC_

#include <fscfg/provisioning_interface.h>

#include "multi_binding_set.h"

namespace fsm
{

/**
 * \brief Foundation Services Provisioning Bind
*/
class ProvisioningBind : public ProvisioningInterface
{
public:
    /**
     * \brief Constructor.
     */
    ProvisioningBind();

    /**
     * \copydoc fsm::ProvisioningInterface::BindResourcesChanged
     */
     fscfg_ReturnCode BindResourcesChanged(ResourcesChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::ProvisioningInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);

protected:
    /**
     * \brief Provisioning base binds
     */
   enum BindBase
    {
        kResourcesChanged,  ///< ResourcesChanged binding set tuple member position.

        kBindBaseMax        ///< Maximum value in this enum. Keep this last.
    };

    /**
     * \brief Resources set changed D-Bus callback.
     *
     * \param[in] resources set of resources present on the bus after change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources);

    MultiBindingSet<ResourcesChangedEvent> bind_base_; ///< base bindings.

};

} // namespace fsm

#endif // FSM_PROVISIONING_BIND_H_INC_

/** \}    end of addtogroup */
