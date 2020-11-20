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
 *  \file     source_bind.h
 *  \brief    Foundation Services Source bindings handler interface.
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_SOURCE_BIND_H_INC_
#define FSM_SOURCE_BIND_H_INC_

#include <fscfg/config_interface.h>

#include "multi_binding_set.h"

namespace fsm
{

/**
 * \brief Foundation Services Bind handler for the Source type.
 * Provides the house-keeping mechanism (manipulates all function callbacks) for binding sets related to Source types
 * for both proxy daemon-side.
*/
class SourceBind : public SourceInterface,
                   public std::enable_shared_from_this<SourceBind>
{
public:
    /**
     * \brief Constructor.
     */
    SourceBind();

    /**
     * \copydoc fsm::SourceInterface::BindUpdated
     */
    fscfg_ReturnCode BindUpdated(UpdatedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::SourceInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);

protected:
    /**
     * \brief Owned resources updated D-Bus callback.
     *
     * \param[in] resources full set of resources that were updated by source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnUpdated(std::vector<std::shared_ptr<ResourceInterface>> resources);

    /**
     * \brief Source base binds
     */
    enum BindBase
    {
        kUpdated,       ///< Updated binding set tuple member position.

        kBindBaseMax    ///< Maximum value in this enum. Keep this last.
    };

    MultiBindingSet<UpdatedEvent> bind_base_; ///< base bindings.
};

} // namespace fsm

#endif // FSM_SOURCE_BIND_H_INC_

/** \}    end of addtogroup */
