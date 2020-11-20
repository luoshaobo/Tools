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
 *  \file     resource_bind.h
 *  \brief    Foundation Services Config Resource bindings handler interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_PROVISIONEDRESOURCE_BIND_H_INC_
#define FSM_PROVISIONEDRESOURCE_BIND_H_INC_

#include <fscfg/provisionedresource_interface.h>

#include <fscfg_common/resource_bind.h>

#include "multi_binding_set.h"

namespace fsm
{

/**
 * \brief Foundation Services Bind handler for the Provisioned resource.
 * Provides the house-keeping mechanism (manipulates all function callbacks) for binding sets related to the
 * Provisioned resource for both proxy daemon-side.
*/
class ProvisionedResourceBind : public virtual ProvisionedResourceInterface,
                                public virtual ResourceBind
{
public:
    /**
     * \brief Constructor.
     */
    ProvisionedResourceBind();

    /**
     * \copydoc fsm::ProvisionedResourceInterface::BindPayloadChanged
     */
    fscfg_ReturnCode BindPayloadChanged(PayloadChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::BindSourceChanged
     */
    fscfg_ReturnCode BindSourceChanged(SourceChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);

protected:
    /**
     * \brief ProvisionedResource base binds
     */
    enum BindBase
    {
        kPayloadChanged,    ///< PayloadChanged binding set tuple member position.
        kSourceChanged,     ///< SourceChanged binding set tuple member position.

        kBindBaseMax        ///< Maximum value in this enum. Keep this last.
    };

    /**
     * \brief Payload changed D-Bus callback.
     *
     * \param[in] payload resource payload in the moment change occurred.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnPayloadChanged(std::vector<std::uint8_t> payload) = 0;

    /**
     * \brief Source changed D-Bus callback.
     *
     * \param[in] source resource highest priority source in the moment change occurred.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnSourceChanged(std::shared_ptr<SourceInterface> source) = 0;

    MultiBindingSet<PayloadChangedEvent,
                    SourceChangedEvent> bind_base_; ///< base bindings.
};

} // namespace fsm

#endif // FSM_PROVISIONEDRESOURCE_BIND_H_INC_

/** \}    end of addtogroup */
