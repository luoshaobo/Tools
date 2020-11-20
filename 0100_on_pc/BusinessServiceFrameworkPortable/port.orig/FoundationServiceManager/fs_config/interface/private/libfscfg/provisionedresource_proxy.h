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
 *  \file     provisionedresource_proxy.h
 *  \brief    Foundation Services ProvisionedResource proxy
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_PROVISIONEDRESOURCE_PROXY_H_INC_
#define FSM_PROVISIONEDRESOURCE_PROXY_H_INC_

#include <fscfg_common/provisionedresource_bind.h>

#include "resource_proxy.h"

namespace fsm
{

class ProvisioningProxy;

/**
 * \brief Foundation Services ProvisionedResource proxy
 */
class ProvisionedResourceProxy : public virtual ProvisionedResourceBind,
                                 public ResourceProxy
{
public:
    /**
     * \brief ProvisionedResourceProxy Constructor.
     *
     * \param[in] provisioning_proxy   provisioning proxy object
     * \param[in] base   base class ResourceProxy object
     * \param[in] feature_dbus_obj   Dbus provisioned resource object.
     */
    ProvisionedResourceProxy(std::shared_ptr<fsm::ProvisioningProxy> provisioning_proxy,
                             std::shared_ptr<ResourceProxy> base,
                             ConfigProvisionedResource* provisioned_resource_obj);

    /**
     * ProvisionedResourceProxy Destructor.
     */
    ~ProvisionedResourceProxy();

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetName
     */
    fscfg_ReturnCode GetName(std::string& name);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetSource
     */
    fscfg_ReturnCode GetSource(std::shared_ptr<SourceInterface>& source);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetEncoding
     */
    fscfg_ReturnCode GetEncoding(std::shared_ptr<EncodingInterface>& encoding);

    /**
     * \copydoc fsm::ProvisionedResourceInterface::GetPayload
     */
    fscfg_ReturnCode GetPayload(std::vector<std::uint8_t>& payload);

    /**
     * \brief Retrieves the D-Bus provisioned resource object.
     *
     * \param[out] resource reference to the D-Bus provisioned resource object.
     *                      Call must do the NULL check.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetObject(_ConfigProvisionedResource*& resource);

protected:
    /**
     * \copydoc fsm::ProvisionedResourceBind::OnPayloadChanged
     */
    fscfg_ReturnCode OnPayloadChanged(std::vector<std::uint8_t> payload);

    /**
     * \copydoc fsm::ProvisionedResourceBind::OnSourceChanged
     */
    fscfg_ReturnCode OnSourceChanged(std::shared_ptr<SourceInterface> source);

private:
    /**
     * \brief Register callback functions to Dbus signals.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if Dbus object is null.
     *         fscfg_kRcError if failed to register callback.
     */
    fscfg_ReturnCode ConnectSignals();

    /**
     * \brief Callback function when properties changed on Dbus.
     *
     * \param[in] proxy   Dbus proxy object
     * \param[in] changed   properites that are changed
     * \param[in] invalidated   Array of properties that was invalidated
     * \param[in] config_instance   Object in which changed occured.
     */
    static void OnPropertiesChanged(GDBusProxy* proxy,
                                    GVariant* changed,
                                    GStrv invalidated,
                                    gpointer config_instance);

    std::shared_ptr<ProvisioningProxy> provisioning_proxy_;      ///< Provisioning proxy.
    ConfigProvisionedResource* provisioned_resource_dbus_obj_; ////< Proxy-side DBus provisioned resource object.
    std::shared_ptr<ResourceProxy> base_;                      ///< ResourceProxy object.

};

} // namespace fsm

#endif // FSM_PROVISIONEDRESOURCE_PROXY_H_

/** \}    end of addtogroup */
