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
 *  \file     provisionedresource.h
 *  \brief    Foundation Services ProvisionedResource interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_PROVISIONEDRESOURCE_H_INC_
#define FSM_PROVISIONEDRESOURCE_H_INC_

#include <fscfg_common/provisionedresource_bind.h>
#include <fscfg_common/generated/foundation-services-config-generated.h>

#include "provisioning.h"
#include "resource.h"

namespace fsm
{

/**
 * \brief Foundation Services ProvisionedResource interface.
*/
class ProvisionedResource : public virtual ProvisionedResourceBind, public Resource
{
public:
    /**
     * \brief ProvisionedResource Constructor.
     *
     * \param[in] name of the resource.
     * \param[in] provisioning reference to the provisioning container.
     *
     * \return fscfg_kRcSuccess on success.
     */
    ProvisionedResource(const std::string& name, std::shared_ptr<Provisioning> provisioning);

    /**
     * \brief ProvisionedResource Destructor.
     */
    ~ProvisionedResource();

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetName
     */
    fscfg_ReturnCode GetName(std::string& name);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetPayload
     */
    fscfg_ReturnCode GetPayload(std::vector<std::uint8_t>& payload);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetSource
     */
    fscfg_ReturnCode GetSource(std::shared_ptr<SourceInterface>& source);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetEncoding
     */
    fscfg_ReturnCode GetEncoding(std::shared_ptr<EncodingInterface>& encoding);

    /**
     * \brief Retrieves the D-Bus resource object.
     *
     * \param[out] resource reference to the D-Bus resource object.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetObject(_ConfigProvisionedResource*& resource);

protected:
    /**
     * \copydoc fsm::ProvisionedResourceInterface::OnPayloadChanged
     */
    fscfg_ReturnCode OnPayloadChanged(std::vector<std::uint8_t> payload);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::OnSourceChanged
     */
    fscfg_ReturnCode OnSourceChanged(std::shared_ptr<SourceInterface> source);

    /**
     * \brief Bound function for Resource::OnSourcesChanged.
     * Prioritizes the sources with the help of Provisioning and dispatches callbacks in the situation the changed
     * source is the top-prio one.
     *
     * \param[in] event event parameters.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode EventSourcesChanged(Resource::SourcesChangedEvent ev);

    /**
     * \brief Bound function for Resource::OnPayloadsChanged.
     * Prioritizes the payloads and the encodings with the help of Provisioning and dispatches callbacks in the
     * situation the changed payload is the top-prio one.
     *
     * \param[in] event event parameters.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode EventPayloadsChanged(Resource::PayloadsChangedEvent ev);

    std::shared_ptr<Provisioning> provisioning_;                ///< Provisioning container reference.
    _ConfigProvisionedResource* resource_dbus_obj_;   ///< D-Bus resource object.

    std::uint32_t src_bind_id_;                                 ///< SourcesChanged bind identifier.
    std::uint32_t payload_bind_id_;                             ///< PayloadsChanged bind identifier.

    std::shared_ptr<SourceInterface> last_top_source_;          ///< Latest update top source.
};

} // namespace fsm

#endif // FSM_PROVISIONEDRESOURCE_H_INC_

/** \}    end of addtogroup */
