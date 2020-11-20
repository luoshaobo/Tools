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

#ifndef FSM_RESOURCE_BIND_H_INC_
#define FSM_RESOURCE_BIND_H_INC_

#include <fscfg/resource_interface.h>

#include "multi_binding_set.h"

namespace fsm
{

/**
 * \brief Foundation Services Bind handler for Resource.
 * Provides the house-keeping mechanism (manipulates all function callbacks) for binding sets related to all
 * Resource types for both proxy daemon-side.
*/
class ResourceBind : public virtual ResourceInterface,
                     public std::enable_shared_from_this<ResourceBind>
{
public:
    ResourceBind();

    /**
     * \copydoc fsm::ResourceInterface::BindExpired
     */
    fscfg_ReturnCode BindExpired(ExpiredCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::ResourceInterface::BindUpdated
     */
    fscfg_ReturnCode BindUpdated(UpdatedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::ResourceInterface::BindSourcesChanged
     */
    fscfg_ReturnCode BindSourcesChanged(SourcesChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::ResourceInterface::BindPayloadsChanged
     */
    fscfg_ReturnCode BindPayloadsChanged(PayloadsChangedCb func, std::uint32_t& id);

    /**
     * \copydoc fsm::ResourceInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);

protected:
    /**
     * \brief Resource base binds
     */
    enum BindBase
    {
        kExpired,           ///< Expired binding set tuple member position.
        kUpdated,           ///< Updated binding set tuple member position.
        kSourcesChanged,    ///< Sources binding set tuple member position.
        kPayloadsChanged,   ///< PayloadsChanged binding set tuple member position.

        kBindBaseMax        ///< Maximum value in this enum. Keep this last.
    };

    /**
     * \brief Expiry notification D-Bus callback.
     *
     * \param[in] immediate true if resource payload should be fetched immediately, false if just caches need to be invalidated.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnExpired(bool immediate) = 0;

    /**
     * \brief Resource updated D-Bus callback.
     *
     * \param[in] source that updated the resource instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnUpdated(std::shared_ptr<SourceInterface> source);

    /**
     * \brief Source set changed D-Bus callback.
     *
     * \param[in] sources set of sources present on the bus after change occured.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources);

    /**
     * \brief Payloads changed D-Bus callback.
     *
     * \param[in] payload resource payload in the moment change occurred.
     * \param[in] source source that triggered the payload change.
     * \param[in] encoding encoding in which the changed payload is described in.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode OnPayloadsChanged(std::vector<std::uint8_t> payload,
                                              std::shared_ptr<SourceInterface> source,
                                              std::shared_ptr<EncodingInterface> encoding);

    MultiBindingSet<ExpiredEvent,
                    UpdatedEvent,
                    SourcesChangedEvent,
                    PayloadsChangedEvent> bind_base_; ///< base bindings.

};

} // namespace fsm

#endif // FSM_RESOURCE_BIND_H_INC_

/** \}    end of addtogroup */
