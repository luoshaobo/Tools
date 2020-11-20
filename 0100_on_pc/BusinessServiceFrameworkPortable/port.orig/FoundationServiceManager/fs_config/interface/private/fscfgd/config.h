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
 *  \file     config.h
 *  \brief    Foundation Services Config interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_CONFIG_H_INC_
#define FSM_CONFIG_H_INC_

#include <map>

#include <gio/gio.h>
#include <glib.h>

#include<fscfg_common/config_bind.h>
#include <fscfg_common/multi_binding_set.h>
#include <fscfg_common/generated/foundation-services-config-generated.h>

#include "object_manager_server.h"
#include "encoding.h"
#include "source.h"
#include "resource.h"

DLT_IMPORT_CONTEXT(dlt_fscfgd);

namespace fsm
{

/**
 * \brief Retrieves all objects inside a given object manager.
 *
 * \tparam R Foundation Services Resource type.
 * \tparam M ObjectManagerServer type
 *
 * \param[in] manager object manager
 * \param[out] resources all resources in object manager
 *
 * \return fscfg_kRcSuccess on success.
 */
template <typename R, typename M>
fscfg_ReturnCode GetAllFromManager(std::shared_ptr<M> manager, std::vector<std::shared_ptr<R>>& resources)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!manager)
    {
        rc = fscfg_kRcBadState;
    }
    else
    {
        for (const auto& pair : manager->set.Get())
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
 * \param[out] object object manager object maped to the given name
 *
 * \return fscfg_kRcSuccess on success.
 */
template <typename R, typename M>
fscfg_ReturnCode GetFromManager(std::shared_ptr<M> manager, const std::string& name, std::shared_ptr<R>& object)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!manager)
    {
        rc = fscfg_kRcBadState; // the object manager member was not properly initialized.
    }
    else
    {
        auto& front = manager->set.Get();
        auto it = front.find(name);

        if (it == front.end())
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
 * \brief Registers an object inside a object manager.
 * When Update is called on the object manager, the objects are published on the bus.
 *
 * \tparam R Foundation Services Resource type.
 * \tparam M ObjectManagerServer type
 *
 * \param[in] manager object manager
 * \param[in] name object name
 *
 * \return fscfg_kRcSuccess on success.
 */
template <typename R, typename M>
fscfg_ReturnCode RegisterInManager(std::shared_ptr<M> manager, std::shared_ptr<R> object)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!manager)
    {
        rc = fscfg_kRcBadState; // the object manager member was not properly initialized.
    }
    else if (!object)
    {
        rc = fscfg_kRcBadParam; // null pointer for object to be registered.
    }
    else
    {
        // if an object with the same name doesn't currently exist, we add it;
        // As we promise the names are unique at the base level (resources, encoding, sources etc),
        // we return error in the situation when the name is already taken.
        std::string object_name;

        rc = object->GetName(object_name);

        auto& front = manager->set.Get();

        if (rc == fscfg_kRcSuccess)
        {
            if (front.count(object_name))
            {
                rc = fscfg_kRcKeyExists;

                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_WARN,
                                "RegisterInManager: name %s already exists",
                                object_name.c_str());
            }
            else
            {
                front[object_name] = object;
            }
        }
    }

    return rc;
}

/**
 * \brief Removes an object from object manager by it's unique name.
 *
 * \tparam M ObjectManagerServer type
 *
 * \param[in] manager object manager
 * \param[in] name object name
 *
 * \return fscfg_kRcSuccess on success.
 */
template <typename M>
fscfg_ReturnCode RemoveFromManager(std::shared_ptr<M> manager, const std::string& object_name)
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    if (!manager)
    {
        rc = fscfg_kRcBadState; // the object manager member was not properly initialized.
    }
    else
    {
        // Find the object by it's unique name;
        // If it doesn't exist, we report error.

        auto& front = manager->set.Get();
        auto it = front.find(object_name);

        if (it == front.end())
        {
            rc = fscfg_kRcBadResource;

            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_WARN,
                            "RemoveFromManager: name %s does not exist",
                            object_name.c_str());
        }
        else
        {
            front.erase(it);
        }
    }

    return rc;
}

/**
 * \brief Foundation Services Config interface
*/
class Config : public ConfigBind
{
public:
    /**
     * \brief Bus Acquired Event.
     */
    struct BusAcquiredEvent
    {
        GDBusConnection* connection;        ///< GDBus connection.
        const gchar* name;                  ///< DBus name that got acquired.
        gpointer parent_instance;           ///< instance of event generator.
    };

    /** \brief BusAcquired Callback. */
    typedef Callback<BusAcquiredEvent> BusAcquiredCb;

    /**
     * \brief Constructor.
     */
    Config();

    /**
     * \brief Destructor.
     */
    ~Config();

    /**
     * \copydoc fsm::ConfigInterface::Unbind
     */
    fscfg_ReturnCode Unbind(std::uint32_t id);

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
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<ResourceInterface>& resource);

    /**
     * \brief GetAll retrieves all registered encoding instances.
     *
     * \param[out] encodings set of all registered encodings.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<Encoding>>& encodings);

    /**
     * \brief GetAll retrieves all registered source instances.
     *
     * \param[out] sources set of all registered sources.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<Source>>& sources);

    /**
     * \brief GetAll retrieves all registered resource instances.
     *
     * \param[out] resources set of all registered resources.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetAll(std::vector<std::shared_ptr<Resource>>& resources);

    /**
     * \brief Retrieve an encoding by name.
     *
     * \param[in] name name of the encoding.
     * \param[out] encoding encoding instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if encoding was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<Encoding>& encoding);

    /**
     * \brief Retrieve a source by name.
     *
     * \param[in] name name of the source.
     * \param[out] source source instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if source was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<Source>& source);

    /**
     * \brief Retrieve a resource by name.
     *
     * \param[in] name name of the resource.
     * \param[out] resource resource instance.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if encoding was not found.
     */
    fscfg_ReturnCode Get(const std::string& name, std::shared_ptr<Resource>& resource);

    /**
     * \brief Registers a new encoding.
     *
     * \param[in] encoding encoding instance to register.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if encoding name is already marked as registered.
     */
    fscfg_ReturnCode Register(std::shared_ptr<Encoding> encoding);

    /**
     * \brief Registers a new source.
     *
     * \param[in] source source instance to register.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if source name is already marked as registered.
     */
    fscfg_ReturnCode Register(std::shared_ptr<Source> source);

    /**
     * \brief Registers a new resource.
     *
     * \param[in] resource resource instance to register.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if resource name is already marked as registered.
     */
    fscfg_ReturnCode Register(std::shared_ptr<Resource> resource);

    /**
     * \brief Remove existing encoding by name.
     *
     * \param[in] encoding_name name of the encoding to be removed.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if name does not exist.
     */
    fscfg_ReturnCode RemoveEncoding(const std::string& encoding_name);

    /**
     * \brief Remove existing source by name.
     *
     * \param[in] source_name name of the encoding to be removed.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if name does not exist.
     */
    fscfg_ReturnCode RemoveSource(const std::string& source_name);

    /**
     * \brief Remove existing resource by name.
     *
     * \param[in] resource_name name of the encoding to be removed.
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
     * \brief Starts owning the Config bus name.
     * All listeners for bus acquired should be already registered.
     *
     * \return fscfg_kRcSucces on success.
     */
    fscfg_ReturnCode OwnBusName();

    /**
     * \brief Registers a callback function which will get called when DBus session is acquired.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode BindBusAcquired(BusAcquiredCb func, std::uint32_t& id);

protected:
    /**
     * \brief internal state tracking.
     */
    enum class State
    {
        kUndefined,                 ///< Initial undefined state.

        kError,                     ///< Error state.
        kInitialized,               ///< All objects initialized.
        kAquiringBusConnection,     ///< Requested bus acuired, waiting for confirmation.
        kBusConnectionActive,       ///< Bus connection active state.

        kMax                        ///< Last literal in enum. Keep this last.
    };

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

    /**
     * \brief Bus acquired DBus callback wrapper.
     *
     * \param[in] connection GDBus connection
     * \param[in] name name of the acquired bus
     * \param[in] config_instance config instance pointer
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode OnBusAcquired(GDBusConnection* connection, const gchar* name, gpointer config_instance);

private:
    /**
     * \brief Config binds.
     */
    enum BindEx
    {
        kBusAcquired,   ///< BusAcquired binding set tuple member position.

        kBindExMax      ///< Maximum value in this enum. Keep this last.
    };

    /**
     * \brief Bus acquired DBus callback.
     *
     * \param[in] connection GDBus connection
     * \param[in] name name of the acquired bus
     * \param[in] config_instance config instance pointer
     *
     * \return fscfg_kRcSuccess on success.
     */
    static void OnBusAcquiredDBus(GDBusConnection* connection, const gchar* name, gpointer config_instance);

    /**
     * \brief Name acquired DBus callback.
     *
     * \param[in] connection GDBus connection
     * \param[in] name name of the acquired bus
     * \param[in] config_instance config instance pointer
     *
     * \return fscfg_kRcSuccess on success.
     */
    static void OnNameAcquiredDBus(GDBusConnection* connection, const gchar* name, gpointer config_instance);

    /**
     * \brief Name lost DBus callback.
     *
     * \param[in] connection GDBus connection
     * \param[in] name name of the lost bus
     * \param[in] config_instance config instance pointer
     *
     * \return fscfg_kRcSuccess on success.
     */
    static void OnNameLostDBus(GDBusConnection* connection, const gchar* name, gpointer config_instance);

    ///! Encodings Object Manager.
    typedef ObjectManagerServer<_Config, Encoding, _ConfigEncoding> ObjMgrEncoding;

    ///! Sources Object Manager.
    typedef ObjectManagerServer<_Config, Source, _ConfigSource> ObjMgrSource;

    ///! Resources Object Manager.
    typedef ObjectManagerServer<_Config, Resource, _ConfigResource> ObjMgrResource;

    std::shared_ptr<ObjMgrEncoding> objmgr_encoding_;       ///< Encodings Object Manager.
    std::shared_ptr<ObjMgrSource> objmgr_source_;           ///< Sources Object Manager.
    std::shared_ptr<ObjMgrResource> objmgr_resource_;       ///< Resource Object Manager.

    MultiBindingSet<BusAcquiredEvent> bind_ex_;             ///< Extended Bindings.

    std::string config_container_path_;                     ///< Config container path.
    guint own_id_;                                          ///< DBus ownership id.
    State local_state_;                                     ///< Local state-tracking.

    _Config* config_dbus_obj_;                              ///< DBus config object.
    GDBusConnection* connection_;                           ///< GDBus connection.
};

} // namespace fsm

#endif // FSM_CONFIG_H_INC_

/** \}    end of addtogroup */
