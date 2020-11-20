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
 *  \file     object_manager_proxy.cpp
 *  \brief    Manages all data related to D-Bus Proxy-side object manager in an object-oriented manner.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <libfscfg/object_manager_proxy.h>

#include <cstring>

#include <dlt/dlt.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

#include <libfscfg/config_proxy.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

namespace fsm
{

GType GetProxyTypeCb(GDBusObjectManagerClient* manager,
                             const gchar *object_path,
                             const gchar *interface_name,
                             gpointer user_data)
{
    GType g_type;

    if (!interface_name)
    {
        g_type = object_proxy_get_type();
    }
    else
    {
        if (strcmp(interface_name, "com.contiautomotive.tcam.FoundationServices.Config.Encoding") == 0)
        {
            g_type = config_encoding_proxy_get_type();
        }
        else if (strcmp(interface_name, "com.contiautomotive.tcam.FoundationServices.Config.Source") == 0)
        {
            g_type = config_source_proxy_get_type();
        }
        else if (strcmp(interface_name, "com.contiautomotive.tcam.FoundationServices.Config.Resource") == 0)
        {
            g_type = config_resource_proxy_get_type();
        }
        else if (strcmp(interface_name, "com.contiautomotive.tcam.FoundationServices.Config.ProvisionedResource") == 0)
        {
            g_type = config_provisioned_resource_proxy_get_type();
        }
        else if (strcmp(interface_name, "com.contiautomotive.tcam.FoundationServices.Config.Feature") == 0)
        {
            g_type = config_feature_proxy_get_type();
        }
        else
        {
            g_type = g_dbus_proxy_get_type();
        }
    }

    return g_type;
}

void OnObjectManagerSignalCb(GDBusObjectManager* manager,
                             GDBusObject* object,
                             std::map<std::string, GDBusObject*>* obj_store)
{
    std::string object_name;
    const char* object_path = g_dbus_object_get_object_path(object);

    GetBasename(object_path, object_name);

    (*obj_store)[object_name] = object;

}

} // namespace fsm

/** \}    end of addtogroup */
