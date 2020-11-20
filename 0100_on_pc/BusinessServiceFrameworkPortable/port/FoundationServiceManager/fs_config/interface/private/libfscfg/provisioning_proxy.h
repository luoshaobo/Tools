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
 *  \file     provisioning_proxy.h
 *  \brief    Foundation Services Provisioning proxy interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_PROVISIONING_PROXY_H_INC_
#define FSM_PROVISIONING_PROXY_H_INC_

#include <fscfg/provisioning_interface.h>

#include<fscfg_common/provisioning_bind.h>
#include <fscfg_common/multi_binding_set.h>

#include "config_proxy.h"
#include "resource_proxy.h"
#include "provisionedresource_proxy.h"

namespace fsm
{

class ProvisionedResourceProxy;

/**
 * \brief Foundation Services Provisioning proxy
 */
class ProvisioningProxy : public ProvisioningBind, public std::enable_shared_from_this<ProvisioningProxy>
{
public:
    /**
     * \brief ProvisioningProxy Constructor.
     *
     * \param[in] config_proxy ConfigProxy object.
     */
    ProvisioningProxy(std::shared_ptr<ConfigProxy> config_proxy);

    /**
     * ProvisioningProxy Destructor.
     */
    ~ProvisioningProxy();

    /**
     * \brief Instantiate provisionined resource ObjectManager object,
     *        updates contaniner and binds to the provisioningproxy properties changed.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if object manager not initalize properly.
     *         fscfg_kRcError on Error.
     */
    fscfg_ReturnCode Init();

    /**
     * \copydoc fsm::ProvisioningInterface::GetSource
     */
    fscfg_ReturnCode GetSource(std::shared_ptr<ProvisionedResourceInterface> resource,
                               std::shared_ptr<SourceInterface>& source);

    /**
     * \copydoc fsm::ProvisioningInterface::GetEncoding
     */
    fscfg_ReturnCode GetEncoding(std::shared_ptr<ProvisionedResourceInterface> resource,
                                 std::shared_ptr<EncodingInterface>& encoding);

    /**
     * \copydoc fsm::ProvisioningInterface::GetAll
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources);

    /**
     * \copydoc fsm::ProvisioningInterface::Get
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<ProvisionedResourceInterface>& resource);

    /**
     * \brief Retrieve a provisioned resource proxy by name.
     *
     * \param[in] name name of the provisioned resource.
     * @param[out] resource provisioned resource proxy instance.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadResource if resource was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<ProvisionedResourceProxy>& resource);

    /**
     * \brief ConfigProxy container getter.
     *
     * \return ConfigProxy container.
     */
    std::shared_ptr<ConfigProxy> GetConfig() const;

protected:
    /**
     * \copydoc fsm::ProvisioningInterface::OnResourcesChanged
     */
    fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources);

private:
    /**
     * \brief Callback function when properties changed on Dbus.
     *
     * \param[in] proxy Dbus proxy object
     * \param[in] changed properites that are changed
     * \param[in] invalidated array of properties that was invalidated
     * \param[in] provisioning_instance container in which changed occured.
     */
    static void OnPropertiesChanged(GDBusProxy* proxy,
                                    GVariant* changed,
                                    GStrv invalidated,
                                    gpointer provisioning_instance);

    /**
     * \brief Update all containers
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if failed to update container.
     */
    fscfg_ReturnCode Update();

    ///! Hierarchical Proxy-side ProvisionedResource Object Manager.
    typedef ObjectManagerHierarchyProxy<fsm::ConfigProxy,
                                        fsm::ProvisioningProxy,
                                        fsm::ResourceProxy,
                                        fsm::ProvisionedResourceProxy,
                                        ConfigProvisionedResource> ObjMgrResource;

    std::mutex sync_;                                             ///< Concurrent access synchronization primitive.

    std::shared_ptr<ConfigProxy> config_proxy_;                     ///< ConfigProxy container.
    std::shared_ptr<ObjMgrResource> objmgr_provisioned_resource_; ///< provisioned resources object manager.
    _Provisioning* provisioning_dbus_obj__;                        ///< DBus provisioning object.
};

} // namespace fsm

#endif // FSM_PROVISIONING_PROXY_H_INC_

/** \}    end of addtogroup */
