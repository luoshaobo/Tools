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
 *  \file     object_manager_server.h
 *  \brief    Manages all data related to D-Bus Server-side object manager in an object-oriented manner.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_OBJECT_MANAGER_SERVER_H_INC_
#define FSM_OBJECT_MANAGER_SERVER_H_INC_

#include <memory>

#include <gio/gio.h>

#include <dlt/dlt.h>

#include <fscfg/fscfg_types.h>

#include <fscfg_common/utilities.h>
#include <fscfg_common/buffered_data_set.h>
#include <fscfg_common/binding_set.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd);

namespace fsm
{

/**
 * \brief Object factory for all DBus skeleton objects managed by an ObjectManager.
 *
 * \tparam C Foundation Services DBus object container type (eg: _Config, _Provisioning, _Discovery etc).
 * \tparam R Foundation Services object type (eg: Encoding, Source, Resource etc).
 * \tparam O Foundation Services DBus object resource type (eg: _ConfigEncoding, _ConfigSource, _ConfigResource etc).
 */
template <typename C, typename R, typename O>
struct ObjectManagerServer
{
    typedef void (*ObjectArraySetter)(C* dbus_obj_container, const gchar* const* values);
    typedef std::map<std::string, std::shared_ptr<R>> Map;

    BufferedDataSet<Map> set;              ///< current data set according to latest update.
    ObjectArraySetter obj_arr_setter;       ///< GDBus array property setter.
    C* dbus_obj_container;                  ///< Foundation Services GDBus generated object.
    GDBusObjectManagerServer* dbus_objmgr;  ///< GDBus Object Manager.
    std::string dbus_objmgr_path;          ///< GDBus Object Manager path.

    /**
     * \brief Object Manager Server Constructor.
     *
     * \param[in] dbus_obj_container Foundation Services GDBus generated object.
     * \param[in] obj_arr_setter GDBus array property setter.
     * \param[in] dbus_objmgr_path GDBus Object Manager path.
     */
    ObjectManagerServer(C* dbus_obj_container,
                  ObjectArraySetter obj_arr_setter,
                  const std::string& dbus_objmgr_path)
        : set(),
          obj_arr_setter(obj_arr_setter),
          dbus_obj_container(dbus_obj_container),
          dbus_objmgr(nullptr),
          dbus_objmgr_path(dbus_objmgr_path)
    {
        dbus_objmgr = g_dbus_object_manager_server_new(dbus_objmgr_path.c_str());
    }

    /**
     * \brief Retrieves the relative name of the ObjectManager instance.
     *
     * \return string with relative name
     */
    std::string GetName() const
    {
        std::string basename;

        GetBasename(dbus_objmgr_path, basename);

        return basename;
    }

    fscfg_ReturnCode SetConnection(GDBusConnection* connection)
    {
        fscfg_ReturnCode rc = fscfg_kRcSuccess;

        if (!dbus_objmgr)
        {
            rc = fscfg_kRcBadState;
        }
        else if (!connection)
        {
            rc = fscfg_kRcBadParam;
        }
        else
        {
            DLT_LOG_STRINGF(dlt_fscfgd,
                            DLT_LOG_INFO,
                            "ObjectManager(%s): Exporting at %s",
                            GetName().c_str(),
                            dbus_objmgr_path.c_str());

            g_dbus_object_manager_server_set_connection(dbus_objmgr, connection);
        }

        return rc;
    }
    /**
     * \brief Updates the internal data set with the latest state.
     *
     * \param[out] new_names new relative names compared to last update.
     * \param[out] deleted_names deleted relative names compared to last update.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode UpdateContainer(std::pair<const std::vector<std::string>&, const std::vector<std::string>&> names)
    {
        fscfg_ReturnCode rc = fscfg_kRcSuccess;

        if (!dbus_obj_container || !dbus_objmgr)
        {
            rc = fscfg_kRcBadState; // surely, instance was not initialized properly.
        }
        else
        {
            // Update object manager.
            // We have two sets of names that we iterate through:
            // fresh objects that are new compared to last update...
            // objects that were removed.

            // Export new objects.
            for (const std::string& new_name : names.first)
            {
                O* dbus_obj;
                std::shared_ptr<R> resource = set.Get()[new_name];
                resource->GetObject(dbus_obj);

                if (!dbus_obj)
                {
                    rc = fscfg_kRcError; // Unexpected, really. Still, it's an internal error.
                }
                else
                {
                    const std::string dbus_obj_path = dbus_objmgr_path + "/" + new_name;

                    DLT_LOG_STRINGF(dlt_fscfgd,
                                    DLT_LOG_INFO,
                                    "ObjectManager(%s): Added object at %s",
                                    GetName().c_str(),
                                    dbus_obj_path.c_str());

                    GDBusObjectSkeleton* dbus_obj_skel = g_dbus_object_skeleton_new(dbus_obj_path.c_str());

                    g_dbus_object_skeleton_add_interface(dbus_obj_skel, G_DBUS_INTERFACE_SKELETON(dbus_obj));
                    g_dbus_object_manager_server_export(dbus_objmgr, dbus_obj_skel);

                    g_object_unref(dbus_obj_skel);
                }
            }

            // Unexport removed objects.
            for (const std::string& removed_name : names.second)
            {
                const std::string dbus_obj_path = dbus_objmgr_path + "/" + removed_name;

                DLT_LOG_STRINGF(dlt_fscfgd,
                                DLT_LOG_INFO,
                                "ObjectManager(%s): Removed object at %s",
                                GetName().c_str(),
                                dbus_obj_path.c_str());

                g_dbus_object_manager_server_unexport(dbus_objmgr, dbus_obj_path.c_str());
            }

            set.Update();
        }

        return rc;
    }

    /**
     * \brief Updates the GDBus property holding the array of exported names.
     *
     * \param[in] names relative names to be exported.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode UpdateNames(const std::vector<std::string>& names)
    {
        fscfg_ReturnCode rc = fscfg_kRcSuccess;
        std::vector<std::string> full_paths_storage;
        std::vector<const char*> full_paths;

        ConvertNames(names, dbus_objmgr_path, full_paths_storage, full_paths);

        DLT_LOG_STRINGF(dlt_fscfgd,
                        DLT_LOG_INFO,
                        "ObjectManager(%s): Updating names...",
                        GetName().c_str());

        if (full_paths.size())
        {
            obj_arr_setter(dbus_obj_container, &full_paths[0]);
        }

        return rc;
    }

};

} // namespace fsm

#endif // FSM_OBJECT_MANAGER_SERVER_H_INC_

/** \}    end of addtogroup */
