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
 *  \file     object_manager_proxy.h
 *  \brief    Manages all data related to D-Bus Proxy-side object manager in an object-oriented manner.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_OBJECT_MANAGER_PROXY_H_INC_
#define FSM_OBJECT_MANAGER_PROXY_H_INC_

#include <mutex>
#include <memory>

#include <gio/gio.h>

#include <dlt/dlt.h>

#include <fscfg/fscfg_types.h>


#include <fscfg_common/generated/foundation-services-config-generated.h>

#include <fscfg_common/buffered_data_set.h>
#include <fscfg_common/binding_set.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

/**
 * \brief GDBus proxy-type getter callback.
 *
 * \param[in] manager GDBus object manager client pointer.
 * \param[in] object_path path of the object that is to be instantiated
 * \param[in] interface_name DBus interface name to instantiate, null if GType for instance needs to be instantiated.
 * \param[in] user_data always null.
 *
 * \return GType computed out of the given parameters.
 */
GType GetProxyTypeCb(GDBusObjectManagerClient* manager,
                     const gchar* object_path,
                     const gchar* interface_name,
                     gpointer user_data);
/**
 * \brief GDBus ObjectManager object operation callback (added or deleted).
 * When executed, it just adds the name-object pair into store for later handling.
 *
 * \param[in] manager object manager that was signaled.
 * \param[in] object bus object to be operated on.
 * \param[in,out] obj_store storage to insert the object into.
 */
void OnObjectManagerSignalCb(GDBusObjectManager* manager,
                             GDBusObject* object,
                             std::map<std::string, GDBusObject*>* obj_store);


/**
 * \brief Object factory for all proxy-side DBus objects managed by an ObjectManager.
 *
 * \tparam C_ Foundation Services container type (eg: ConfigProxy, ProvisioningProxy, DiscoveryProxy etc).
 * \tparam O_ FOundation Services object type (eg: EncodingProxy, SourceProxy, ResourceProxy etc).
 * \tparam BO_ Associated GDBus codegen object.
 */
template <typename C_, typename O_, typename BO_>
struct ObjectManagerBase
{
    typedef BO_* (*InterfaceGetter)(Object* object);
    typedef std::map<std::string, std::shared_ptr<O_>> Map;

    Map set;                                            ///< current data set according to latest update.
    InterfaceGetter iface_getter;                       ///< Glib object to interface converter.
    std::shared_ptr<C_> container;                      ///< Foundation Services container.
    std::mutex& container_lock;                         ///< Foundation Services container sync primitive.
    GDBusObjectManager* dbus_objmgr;                    ///< GDBus ObjectManager.
    std::string dbus_name;                              ///< Bus name.
    std::string dbus_objmgr_path;                       ///< ObjectManager bus path.
    std::map<std::string, GDBusObject*> new_objs;       ///< New objects set since last update.
    std::map<std::string, GDBusObject*> deleted_objs;   ///< Deleted objects set since last update.

    /**
     * \brief ObjectManagerBase Constructor.
     *
     * \param[in] container Foundation Services container.
     * \param[in] container_lock Foundation Services container sync primitive.
     * \param[in] iface_getter Glib object to interface converter.
     * \param[in] dbus_name Bus name.
     * \param[in] dbus_objmgr_path ObjectManager bus path.
     */
    ObjectManagerBase(std::shared_ptr<C_> container,
                      std::mutex& container_lock,
                      InterfaceGetter iface_getter,
                      std::string dbus_name,
                      std::string dbus_objmgr_path)
        : set(),
          iface_getter(iface_getter),
          container(container),
          container_lock(container_lock),
          dbus_objmgr(nullptr),
          dbus_name(dbus_name),
          dbus_objmgr_path(dbus_objmgr_path),
          new_objs(),
          deleted_objs()
    {
        GError* error = nullptr;

        dbus_objmgr = g_dbus_object_manager_client_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                                    G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
                                                                    dbus_name.c_str(),
                                                                    dbus_objmgr_path.c_str(),
                                                                    GetProxyTypeCb,
                                                                    nullptr,
                                                                    nullptr,
                                                                    nullptr,
                                                                    &error);

        if (!dbus_objmgr)
        {
            if (error)
            {
                DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_ERROR,
                                "ObjectManagerBase: Failed to create dbus object manager client object, error: %s",
                                error->message);

                g_clear_error(&error);
            }
            else
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to create dbus object manager client object");
            }
        }
        else
        {
            // connect d-bus signals.
            // We will just collect newly added and deleted objects in new_objs and deleted_objs.
            // The idea is that we perform the actual update on UpdateContainer and not immediately (allows us a finer
            // control to updates from the daemon-side).
            if (!g_signal_connect(dbus_objmgr, "object-added", G_CALLBACK(OnObjectManagerSignalCb), &new_objs))
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                               "ObjectManagerBase: Failed to connect to object-added signal");
            }

            if (!g_signal_connect(dbus_objmgr, "object-removed", G_CALLBACK(OnObjectManagerSignalCb), &deleted_objs))
            {
                DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR,
                               "ObjectManagerBase: Failed to connect to object-removed signal");
            }

            // Get the initial state of the objects and populate the new_objs array.
            GList* bus_objects = g_dbus_object_manager_get_objects(dbus_objmgr);

            for (GList* current_object = bus_objects; current_object != NULL; current_object = current_object->next)
            {
               const  gchar* full_object_path = g_dbus_object_get_object_path(G_DBUS_OBJECT(current_object->data));
               std::string resource_name;
               GetBasename(full_object_path, resource_name);

               new_objs[resource_name] = G_DBUS_OBJECT(current_object->data);
            }

            g_list_free(bus_objects);
        }
    }

    /**
     * \brief Updates the internal data set with the latest server-side state.
     *
     * \param[out] new_names new relative names compared to last update.
     * \param[out] deleted_names deleted relative names compared to last update.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode UpdateContainer(std::vector<std::string>& new_names, std::vector<std::string>& deleted_names)
    {
        fscfg_ReturnCode rc = fscfg_kRcSuccess;

        if (!container || !dbus_objmgr)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "ObjectManagerBase: Not initialized properly");

            rc = fscfg_kRcBadState;
        }
        else
        {
            container_lock.lock();

            // add all new objects.
            for (auto& pair_name_obj : new_objs)
            {
                std::shared_ptr<O_> object;

                // instantiate the Foundation Services object with the help of
                // the factory method.
                this->CreateObject(pair_name_obj, object);

                set[pair_name_obj.first] = object;

                new_names.push_back(pair_name_obj.first);
            }

            // remove all deleted objects.
            for (auto& pair_name_obj : deleted_objs)
            {
                set.erase(pair_name_obj.first);
                deleted_names.push_back(pair_name_obj.first);
            }

            container_lock.unlock();

            // Logging.
            if (new_objs.size() || deleted_objs.size())
            {
                std::string mgr_name;

                GetBasename(dbus_objmgr_path, mgr_name);

                DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO,
                                "ObjectManagerBase(%s): Start update....", mgr_name.c_str());

                for (auto& pair_name_obj : set)
                {
                    DLT_LOG_STRINGF(dlt_libfscfg,
                                    DLT_LOG_INFO,
                                    "ObjectManagerBase(%s): Object: %s",
                                    mgr_name.c_str(),
                                    pair_name_obj.first.c_str());
                }

                DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO,
                                "ObjectManagerProxy(%s): End update.", mgr_name.c_str());
            }

            container_lock.lock();
            // As we consider the object manager to be updated,
            // We clear the new and deleted set.
            new_objs.clear();
            deleted_objs.clear();

            container_lock.unlock();
        }

        return rc;
    }

    /**
     * \brief Creates Foundation Services Proxy-side object out of GDBus object.
     *
     * \param[in] pair_name_obj pair of relative name nad GDBus object.
     * \param[out] object Foundation Services Proxy-side object.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode CreateObject(std::pair<std::string, GDBusObject*> pair_name_obj,
                                          std::shared_ptr<O_>& object) = 0;
};


/**
 * \brief Manages primitive (objects that are not part of a hierarchy) data related to D-Bus proxy-side object manager.
 *
 * \tparam C_ Foundation Services container type (eg: ConfigProxy, ProvisioningProxy, DiscoveryProxy etc).
 * \tparam O_ FOundation Services object type (eg: EncodingProxy, SourceProxy, ResourceProxy etc).
 * \tparam BO_ Associated GDBus codegen object.
 */
template <typename C_, typename O_, typename BO_>
struct ObjectManagerProxy : ObjectManagerBase<C_, O_, BO_>
{
    typedef ObjectManagerBase<C_, O_, BO_> Base;

    ObjectManagerProxy(std::shared_ptr<C_> container,
                       std::mutex& container_lock,
                       typename Base::InterfaceGetter iface_getter,
                       std::string dbus_name,
                       std::string dbus_objmgr_path)
        : Base(container, container_lock, iface_getter, dbus_name, dbus_objmgr_path)
    {
    }

    /**
     * \copydoc ObjectManagerBase::CreateObject
     */
    fscfg_ReturnCode CreateObject(std::pair<std::string, GDBusObject*> pair_name_obj,
                                              std::shared_ptr<O_>& object) override
    {
        fscfg_ReturnCode rc = fscfg_kRcSuccess;

        BO_* dbus_object = this->iface_getter(OBJECT(pair_name_obj.second));
        object = std::make_shared<O_>(this->container, dbus_object);

        return rc;
    }
};

/**
 * \brief Manages all data related to D-Bus proxy-side hierarchical object manager.
 * \tparam PC_ Foundation Services container type (eg: ConfigProxy, ProvisioningProxy etc).
 * \tparam C_ Foundation Services container type (eg: ProvisioningProxy, DiscoveryProxy etc).
 * \tparam PO_ Foundation Services object type (eg: ResourceProxy, ProvisionedResourceProxy etc).
 * \tparam O_ Foundation Services object type (eg: ProvisionedResourceProxy, FeatureProxy etc).
 * \tparam BO_ Associated GDBus codegen object.
 */
template <typename PC_, typename C_, typename PO_, typename O_, typename BO_>
struct ObjectManagerHierarchyProxy : ObjectManagerBase<C_, O_, BO_>
{
    typedef ObjectManagerBase<C_, O_, BO_> Base;
    std::shared_ptr<PC_> parent_container_; ///< Foundation Service parent container.

    /**
     * \brief ObjectManagerHierarchyProxy Constructor.
     * \param container Foundation Services container.
     * \param container_lock Foundation Services container sync primitive.
     * \param parent_container Foundation Services parent container.
     * \param iface_getter Glib object to interface converter.
     * \param dbus_name Bus name.
     * \param dbus_objmgr_path ObjectManager bus path.
     */
    ObjectManagerHierarchyProxy(std::shared_ptr<C_> container,
                                std::mutex& container_lock,
                                std::shared_ptr<PC_> parent_container,
                                typename Base::InterfaceGetter iface_getter,
                                std::string dbus_name,
                                std::string dbus_objmgr_path)
        : Base(container, container_lock, iface_getter, dbus_name, dbus_objmgr_path),
          parent_container_(parent_container)

    {
    }

    /**
     * \copydoc ObjectManagerBase::CreateObject
     */
    fscfg_ReturnCode CreateObject(std::pair<std::string, GDBusObject*> pair_name_obj,
                                              std::shared_ptr<O_>& object)
    {
        fscfg_ReturnCode rc = fscfg_kRcSuccess;

        BO_* dbus_object = this->iface_getter(OBJECT(pair_name_obj.second));


        std::shared_ptr<PO_> parent_object;
        parent_container_->Get(pair_name_obj.first, parent_object);

        object = std::make_shared<O_>(this->container, parent_object, dbus_object);

        return rc;
    }

};

} // namespace fsm

#endif // FSM_OBJECT_MANAGER_PROXY_H_INC_
