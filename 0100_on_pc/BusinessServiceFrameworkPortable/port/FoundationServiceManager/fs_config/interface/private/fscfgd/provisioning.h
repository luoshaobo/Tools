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
 *  \file     provisioning.h
 *  \brief    Foundation Services Provisioning interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_PROVISIONING_H_INC_
#define FSM_PROVISIONING_H_INC_

#include <fscfg_common/provisioning_bind.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

#include "config.h"

namespace fsm
{

class ProvisionedResource;

/**
 * \brief Discovery Config container.
 */
class Provisioning : public ProvisioningBind
{
public:

    /**
     * \brief Provisioning priorities.
     */
    enum class Priority
    {
        kUndefined,     ///< Priority is undefined.

        kHighest,       ///< Highest priority.
        kHigh,          ///< High priority.
        kMedium,        ///< Medium priority.
        kLow,           ///< Low priority.
        kLast,

        kMax            ///< Maximum value type should hold. Keep this last.
    };

    /**
     * \brief Provisioning Constructor.
     *
     * \param[in] config config reference
     */
    Provisioning(std::shared_ptr<Config> config);

    /**
     * \brief Provisioning Destructor.
     */
    ~Provisioning();

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
     * \brief retrieves all registered provisioned resource instances.
     *
     * \param[out] resource set of all registered provisioned resources.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<ProvisionedResource>>& resources);

    /**
     * \brief Retrieve a provisioned resource by name.
     *
     * \param[in] name name of the source.
     * \param[out] resource provisioned resource instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if provisioned resource was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<ProvisionedResource>& resource);

    /**
     * \brief Registers a new resource.
     *
     * \param[in] resource resource instance to register.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if resource name is already marked as registered.
     */
    fscfg_ReturnCode Register(std::shared_ptr<ProvisionedResource> resource);

    /**
     * \brief Remove existing resource by name.
     *
     * \param[in] resource_name name of the provisioned resource to be removed.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if name does not exist.
     */
    fscfg_ReturnCode RemoveResource(const std::string& resource_name);

    /**
     * \brief Publish all ongoing modifications since last update.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode Update();

    /**
     * \brief Sets the priority for a given encoding.
     *
     * \param[in] encoding encoding to set the priority for.
     * \param[in] prio encoding priority.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetPriority(std::shared_ptr<Encoding> encoding, Priority prio);

    /**
     * \brief Sets the priority for a given source.
     *
     * \param[in] source source to set the priority for.
     * \param[in] prio source priority.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode SetPriority(std::shared_ptr<Source> source, Priority prio);

    /**
    * \brief Getter for the internal config reference.
    *
    * \return config reference.
    */
    std::shared_ptr<Config> GetConfig();

protected:
    /**
     * \copydoc fsm::ProvisioningInterface::OnResourcesChanged
     */
    fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<ProvisionedResourceInterface>>& resources);

    /**
     * \brief Bound function for Config::OnBusAcuired.
     * Exports provisioning interface on bus name.
     *
     * \param[in] event event parameters.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode EventBusAcquired(Config::BusAcquiredEvent event);

private:
    ///! Encoding priority map type.
    typedef std::map<Priority, std::shared_ptr<Encoding>> EncodingPrioMap;

    ///! Source priority map type.
    typedef std::map<Priority, std::shared_ptr<Source>> SourcePrioMap;

    ///! ProvisionedResources Object Manager.
    typedef ObjectManagerServer<_Provisioning,
                                ProvisionedResource,
                                _ConfigProvisionedResource> ObjMgrResource;


    std::shared_ptr<Config> config_;                                                ///< config container reference.
    EncodingPrioMap encoding_prio_;                                                 ///< Encoding priority map.
    SourcePrioMap source_prio_;                                                     ///< Source priority map.
    std::uint32_t config_bus_bind_id;                                               ///< BusAcquired bind identifier.

    std::shared_ptr<ObjMgrResource> objmgr_resource_;                               ///< ProvisionedResources Object Manager.

    _Provisioning* provisioning_dbus_obj_;                                           ///< DBus provisioning object.
};

} // namespace fsm

#endif // FSM_PROVISIONING_H_INC_

/** \}    end of addtogroup */
