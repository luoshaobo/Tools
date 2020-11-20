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
 *  \file     config_proxy.h
 *  \brief    Foundation Services Config proxy interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_CONFIG_PROXY_H_INC_
#define FSM_CONFIG_PROXY_H_INC_

#include <memory>
#include <vector>

#include <gio/gio.h>
#include <glib.h>

#include <fscfg/config_interface.h>

#include<fscfg_common/config_bind.h>
#include <fscfg_common/multi_binding_set.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

#include <libfscfg/object_manager_proxy.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

class EncodingProxy;
class SourceProxy;
class ResourceProxy;

/**
 * \brief Retrieves all objects inside a given object manager.
 *
 * \tparam R Foundation Services Resource type.
 * \tparam M ObjectManagerServer type
 *
 * \param[in] manager object manager
 * \param[in] sync synchronization primitive
 * \param[out] resources all resources in object manager
 *
 * \return fscfg_kRcSuccess on success.
 */
template <typename R, typename M>
fscfg_ReturnCode GetAllFromManager(std::shared_ptr<M> manager,
                                   std::mutex& sync,
                                   std::vector<std::shared_ptr<R>>& resources)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!manager)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        std::lock_guard<std::mutex> lk(sync);

        auto& set = manager->set;

        for (const auto& pair :set)
        {
            resources.push_back(pair.second);
        }
    }

    return rc;
}

/**
 * \brief Retrieves an object by name from inside a given object manager.
 *
 * \tparam R Foundation Services Resource type.
 * \tparam M ObjectManagerServer type
 *
 * \param[in] manager object manager
 * \param[in] name object name
 * \param[in] sync synchronization primitive
 * \param[out] object object manager object maped to the given name
 *
 * \return fscfg_kRcSuccess on success,\n
 *         fscfg_kRcNotFound if there's no object identified by name
 */
template <typename R, typename M>
fscfg_ReturnCode GetFromManager(std::shared_ptr<M> manager,
                                const std::string& name,
                                std::mutex& sync,
                                std::shared_ptr<R>& object)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!manager)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        std::lock_guard<std::mutex> lk(sync);

        auto& set = manager->set;
        auto it = set.find(name);

        if (it == set.end())
        {
            rc = fscfg_kRcBadResource;
        }
        else
        {
            object = it->second;
        }
    }

    return rc;
}

/**
 * \brief Foundation Services Config proxy interface
 */
class ConfigProxy : public ConfigBind, public std::enable_shared_from_this<ConfigProxy>
{
public:
    /**
     * \brief Constructor.
     */
    ConfigProxy();

    /**
     * \brief Destructor.
     */
    ~ConfigProxy();

    /**
     * \brief Instantiate config encoding, config source and config resource object manager
     *        objects, updates containers and binds to the configproxy properties changed.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if object manager not initialized properly.
     *         fscfg_kRcError on Error.
     */
    fscfg_ReturnCode Init();

    /**
     * \copydoc fsm::ConfigInterface::GetAll(std::vector<std::shared_ptr<EncodingInterface>>& encodings)
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<EncodingInterface>>& encodings);

    /**
     * \copydoc fsm::ConfigInterface::GetAll(std::vector<std::shared_ptr<SourceInterface>>& sources)
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<SourceInterface>>& sources);

    /**
     * \copydoc fsm::ConfigInterface::GetAll(std::vector<std::shared_ptr<ResourceInterface>>& resources)
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<ResourceInterface>>& resources);

    /**
     * \copydoc fsm::ConfigInterface::Get(const std::string& name, std::shared_ptr<EncodingInterface>& encoding)
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<EncodingInterface>& encoding);

    /**
     * \copydoc fsm::ConfigInterface::Get(const std::string& name, std::shared_ptr<SourceInterface>& source)
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<SourceInterface>& source);

    /**
     * \copydoc fsm::ConfigInterface::Get(const std::string& name, std::shared_ptr<ResourceInterface>& resource)
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<fsm::ResourceInterface> &resource);

    /**
     * \brief Retrieve a encoding proxy by name.
     *
     * \param[in] name name of the encoding.
     * @param[out] encoding encoding proxy instance.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if object manager not initalize properly.
     *         fscfg_kRcBadResource if resource was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<EncodingProxy>& encoding);

    /**
     * \brief Retrieve a source proxy by name.
     *
     * \param[in] name name of the source.
     * @param[out] source source proxy instance.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if object manager not initalize properly.
     *         fscfg_kRcBadResource if resource was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<SourceProxy>& source);

    /**
     * \brief Retrieve a resource proxy by name.
     *
     * \param[in] name name of the resource.
     * @param[out] resource resource proxy instance.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if object manager not initalize properly.
     *         fscfg_kRcBadResource if resource was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<ResourceProxy>& resource);


protected:
    /**
     * \copydoc fsm::ConfigInterface::OnEncodingsChanged
     */
    fscfg_ReturnCode OnEncodingsChanged(std::vector<std::shared_ptr<EncodingInterface>> encodings);

    /**
     * \copydoc fsm::ConfigInterface::OnSourcesChanged
     */
    fscfg_ReturnCode OnSourcesChanged(std::vector<std::shared_ptr<SourceInterface>> sources);

    /**
     * \copydoc fsm::ConfigInterface::OnResourcesChanged
     */
    fscfg_ReturnCode OnResourcesChanged(std::vector<std::shared_ptr<ResourceInterface>> resources);

private:
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

    /**
     * \brief Update all Object Managers.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if failed to update container.
     */
    fscfg_ReturnCode Update();

    /**
     * \brief Update all Encodings in Object Manager and call binds.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if failed to update container.
     */
    fscfg_ReturnCode UpdateEncodings();

    /**
     * \brief Update all Sources in Object Manager and call binds.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if failed to update container.
     */
    fscfg_ReturnCode UpdateSources();

    /**
     * \brief Update all Resources in Object Manager and call binds.
     *
     * \return fscfg_kRcSuccess on success,
     *         fscfg_kRcBadState if failed to update container.
     */
    fscfg_ReturnCode UpdateResources();

    ///! Proxy-side Encoding Object Manager.
    typedef ObjectManagerProxy<fsm::ConfigProxy, EncodingProxy, _ConfigEncoding> ObjMgrEncoding;

    ///! Proxy-side Source Object Manager.
    typedef ObjectManagerProxy<fsm::ConfigProxy, SourceProxy, _ConfigSource> ObjMgrSource;

    ///! Proxy-side Resource Object Manager.
    typedef ObjectManagerProxy<fsm::ConfigProxy, ResourceProxy, _ConfigResource> ObjMgrResource;

    std::mutex sync_;                                   ///< Concurrent access synchronization primitive.

    std::shared_ptr<ObjMgrEncoding> objmgr_encoding_;   ///< Encodings object manager.
    std::shared_ptr<ObjMgrSource> objmgr_source_;       ///< Sources object manager.
    std::shared_ptr<ObjMgrResource> objmgr_resource_;   ///< Resources object manager.

    _Config* config_dbus_obj__;                          ///< Proxy-side DBus config object.
};

} // namespace fsm

#endif // FSM_CONFIG_PROXY_H_INC_

/** \}    end of addtogroup */
