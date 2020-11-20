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
 *  \file     discovery.h
 *  \brief    Foundation Services Discovery interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_DISCOVERY_H_INC_
#define FSM_DISCOVERY_H_INC_


#include <fscfg_common/discovery_bind.h>

#include "object_manager_server.h"
#include "provisioning.h"

namespace fsm
{

class Feature;

/**
 * \brief Discovery Config container.
 */
class Discovery : public DiscoveryBind
{
public:

    /**
     * \brief Discovery Constructor.
     *
     * \param[in] provisioning provisioning reference
     */
    Discovery(std::shared_ptr<Provisioning> provisioning);

    /**
     * \brief Discovery Destructor.
     */
    ~Discovery();

    /**
     * \copydoc fsm::DiscoveryInterface::GetAll
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<FeatureInterface>>& features);

    /**
     * \copydoc fsm::DiscoveryInterface::Get
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<FeatureInterface>& feature);

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

    /**
     * \brief retrieves all registered feature instances.
     *
     * \param[out] features set of all registered features.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<Feature>>& features);

    /**
     * \brief Retrieve a feature by name.
     *
     * \param[in] name name of the feature.
     * \param[out] feature feature instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if feature was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<Feature>& feature);

    /**
     * \brief Registers a new feature.
     *
     * \param[in] resource feature instance to register.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if encoding feature is already marked as registered.
     */
    fscfg_ReturnCode Register(std::shared_ptr<Feature> resource);

    /**
     * \brief Updates an existing feature with a given feature data and optionally create it if it does not exist.
     *
     * \param[in] resource feature instance to register.
     * \param[in] create true if feature should be registered if it does not exist, false if it should only be updated.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if encoding name is already marked as registered.
     */
    fscfg_ReturnCode Update(std::shared_ptr<Feature> resource, bool create = false);

    /**
     * \brief Remove existing feature by name.
     *
     * \param[in] feature_name name of the feature to be removed.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if name does not exist.
     */
    fscfg_ReturnCode RemoveFeature(const std::string& feature_name);

    /**
     * \brief Publish all ongoing modifications since last update.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode Update();

    /**
    * \brief Getter for the internal provisioning reference.
    *
    * \return provisioning reference.
    */
   std::shared_ptr<Provisioning> GetProvisioning();

protected:
   /**
    * \copydoc fsm::DiscoveryBind::OnResourcesChanged
    */
   fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<FeatureInterface>>& features);

   /**
    * \brief Bound function for Config::OnBusAcuired.
    * Exports discovery interface on bus name.
    *
    * \param[in] event event parameters.
    *
    * \return fscfg_kRcSuccess on success.
    */
   fscfg_ReturnCode EventBusAcquired(Config::BusAcquiredEvent event);

private:
   /**
    * \brief Populates the cloud connection status property with dummy values.
    */
   void SetCloudConnectionStatus();

   ///! Features Object Manager.
   typedef ObjectManagerServer<_Discovery, Feature, _ConfigFeature> ObjMgrFeature;

    std::shared_ptr<Provisioning> provisioning_;         ///< provisioning reference.
    std::shared_ptr<ObjMgrFeature> objmgr_feature_;      ///< features object manager.

    _Discovery* discovery_dbus_obj_;                     ///< DBus discovery object.
    std::uint32_t config_bus_bind_id_;                   ///< BusAcquired bind identifier.
};

} // namespace fsm

#endif // FSM_DISCOVERY_H_INC_

/** \}    end of addtogroup */
