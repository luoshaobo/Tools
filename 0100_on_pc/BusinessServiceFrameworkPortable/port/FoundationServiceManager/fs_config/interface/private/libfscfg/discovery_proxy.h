/**
 * Copyright (C) 2017, 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     discovery_proxy.h
 *  \brief    Foundation Services Discovery proxy interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_DISCOVERY_PROXY_H_INC_
#define FSM_DISCOVERY_PROXY_H_INC_

#include <fscfg_common/generated/foundation-services-config-generated.h>
#include <fscfg_common/discovery_bind.h>

#include "feature_proxy.h"
#include "provisioning_proxy.h"

namespace fsm
{

/**
 * \brief Foundation Services Discovery proxy
 */
class DiscoveryProxy : public DiscoveryBind, public std::enable_shared_from_this<DiscoveryProxy>
{
public:

    /**
     * \brief DiscoveryProxy Constructor.
     *
     * \param[in] provisioning_iface provisioning pointer.
     */
    DiscoveryProxy(std::shared_ptr<ProvisioningProxy> provisioning_proxy);

    /**
     * DiscoveryProxy Destructor.
     */
    ~DiscoveryProxy();

    /**
     * \brief Instantiate discovery feature ObjectManager object, updates containers and
     *        binds to the discoveryproxy properties changed.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if object manager not initalize properly.
     *         fscfg_kRcError on Error.
     */
    fscfg_ReturnCode Init();

    /**
     * \copydoc fsm::DiscoveryInterface::GetAll
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<FeatureInterface>>& features);

    /**
     * \copydoc fsm::DiscoveryInterface::Get
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<FeatureInterface>& feature);

    /**
     * \brief Retrieve a feature proxy by name.
     *
     * \param[in] name name of the feature.
     * @param[out] feature feature proxy instance.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadResource if feature was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<FeatureProxy>& feature);


    /**
     * \copydoc fsm::DiscoveryInterface::GetCloudConnectionStatus
     */
    fscfg_ReturnCode GetCloudConnectionStatus(char& certificate_validity,
                                              char& central_connect_status,
                                              std::string& central_server_uri,
                                              std::string& central_date_time,
                                              char& regional_connect_status,
                                              std::string& regional_server_uri,
                                              std::string& regional_date_time);

protected:
    /**
     * \copydoc fsm::DiscoveryInterface::OnResourcesChanged
     */
    fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<FeatureInterface>>& features);

private:
    /**
     * \brief Callback function when properties changed on Dbus.
     *
     * \param[in] proxy Dbus proxy object
     * \param[in] changed array of string variant pairs representing the changed properties and their value.
     * \param[in] invalidated array of invalidated properties
     * \param[in] discovery_instance container in which changed occured.
     */
    static void OnPropertiesChanged(GDBusProxy* proxy,
                                    GVariant* changed,
                                    GStrv invalidated,
                                    gpointer discovery_instance);
    /**
     * \brief Update all containers
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if failed to update container.
     */
    fscfg_ReturnCode Update();

    ///! Hierarchical Proxy-side Feature Object Manager.
    typedef ObjectManagerHierarchyProxy<fsm::ProvisioningProxy,
                                        fsm::DiscoveryProxy,
                                        fsm::ProvisionedResourceProxy,
                                        fsm::FeatureProxy,
                                        ConfigFeature> ObjMgrFeature;

    std::mutex sync_;                                     ///< Concurrent access synchronization primitive.

    std::shared_ptr<ObjMgrFeature> objmgr_feature_;       ///< Object manager feature object.
    std::shared_ptr<ProvisioningProxy> provisioning_proxy_; ///< provisioning pointer.
    _Discovery* discovery_dbus_obj_;                      ///< Proxy-side DBus discovery object.
};

} // namespace fsm

#endif

/** \}    end of addtogroup */
