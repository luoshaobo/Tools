/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     foundation-services-config-generated.h
 *  \brief    Mocked implemenation of foundation services config generated code.
 *            For unittest the mocked code will be used instead of
 *            using actual generated code.
 *            Note: This is not a generated code.
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm
 *  \{
 */

#include <string.h>

#include <dbus_interface_mock.h>
#include <fscfg_common/utilities.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

//dbus mock interface
fsm::DBusInterfaceMock dbus_interface_mock;

Config *config_proxy_new_for_bus_sync(GBusType             bus_type,
                                      GDBusProxyFlags      flags,
                                      const gchar          *name,
                                      const gchar          *object_path,
                                      GCancellable         *cancellable,
                                      GError               **error)
{
    GDBusObjectFake* GDBus_obj_mgr_config = static_cast<GDBusObjectFake*>(calloc(1, sizeof(GDBusObjectFake)));

    if (GDBus_obj_mgr_config)
    {
        GDBus_obj_mgr_config->object_name = static_cast<char*>(calloc(1, strlen(name) + 1));
        GDBus_obj_mgr_config->object_path = static_cast<char*>(calloc(1, strlen(object_path) + 1));

        if (GDBus_obj_mgr_config->object_name
            && GDBus_obj_mgr_config->object_path)
        {
            strcpy(GDBus_obj_mgr_config->object_name, name);
            strcpy(GDBus_obj_mgr_config->object_path, object_path);
        }
    }

    return reinterpret_cast<Config*>(GDBus_obj_mgr_config);
}

Discovery *discovery_proxy_new_for_bus_sync(GBusType             bus_type,
                                            GDBusProxyFlags      flags,
                                            const gchar          *name,
                                            const gchar          *object_path,
                                            GCancellable         *cancellable,
                                            GError               **error)
{
    GDBusObjectFake *GDBus_obj_mgr_discovery = static_cast<GDBusObjectFake*>(calloc(1, sizeof(GDBusObjectFake)));

    if (GDBus_obj_mgr_discovery)
    {
        GDBus_obj_mgr_discovery->object_name = static_cast<char*>(calloc(1, strlen(name) + 1));
        GDBus_obj_mgr_discovery->object_path = static_cast<char*>(calloc(1, strlen(object_path) + 1));

        if (GDBus_obj_mgr_discovery->object_name
            && GDBus_obj_mgr_discovery->object_path)
        {
            strcpy(GDBus_obj_mgr_discovery->object_name, name);
            strcpy(GDBus_obj_mgr_discovery->object_path, object_path);
        }
    }

    return reinterpret_cast<Discovery*>(GDBus_obj_mgr_discovery);
}

Provisioning *provisioning_proxy_new_for_bus_sync(GBusType             bus_type,
                                                  GDBusProxyFlags      flags,
                                                  const gchar          *name,
                                                  const gchar          *object_path,
                                                  GCancellable         *cancellable,
                                                  GError               **error)
{
    GDBusObjectFake* GDBus_obj_mgr_provisioning = static_cast<GDBusObjectFake*>(calloc(1, sizeof(GDBusObjectFake)));


    if (GDBus_obj_mgr_provisioning)
    {
        GDBus_obj_mgr_provisioning->object_name = static_cast<char*>(calloc(1, strlen(name) + 1));
        GDBus_obj_mgr_provisioning->object_path = static_cast<char*>(calloc(1, strlen(object_path) + 1));

        if (GDBus_obj_mgr_provisioning->object_name
            && GDBus_obj_mgr_provisioning->object_path)
        {
            strcpy(GDBus_obj_mgr_provisioning->object_name, name);
            strcpy(GDBus_obj_mgr_provisioning->object_path, object_path);
        }
    }

    return reinterpret_cast<Provisioning*>(GDBus_obj_mgr_provisioning);
}

ConfigEncoding *object_get_config_encoding(Object *object)
{
    ConfigEncodingFake *config_encoding_fake;

    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    if (object_fake)
    {
        config_encoding_fake->object_name = object_fake->object_name;
        config_encoding_fake->object_path = object_fake->object_path;
    }

    return reinterpret_cast<ConfigEncoding*>(object);
}

ConfigSource *object_get_config_source(Object *object)
{
    ConfigSourceFake *config_source_fake;

    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    if (object_fake)
    {
        config_source_fake->object_name = object_fake->object_name;
        config_source_fake->object_path = object_fake->object_path;
    }

    return reinterpret_cast<ConfigSource*>(config_source_fake);
}

ConfigResource *object_get_config_resource(Object *object)
{
    ConfigResourceFake *config_resource_fake;

    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    if (object_fake)
    {
        config_resource_fake->object_name = object_fake->object_name;
        config_resource_fake->object_path = object_fake->object_path;
    }

    return reinterpret_cast<ConfigResource*>(config_resource_fake);
}

ConfigFeature *object_get_config_feature(Object *object)
{
    ConfigFeatureFake *feature_fake;

    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    if (object_fake)
    {
        feature_fake->object_name = object_fake->object_name;
        feature_fake->object_path = object_fake->object_path;
    }

    return reinterpret_cast<ConfigFeature*>(feature_fake);
}

ConfigProvisionedResource *object_get_config_provisioned_resource(Object *object)
{
    ConfigProvisionedResourceFake *prov_resource_fake;

    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    if (object_fake)
    {
        prov_resource_fake->object_name = object_fake->object_name;
        prov_resource_fake->object_path = object_fake->object_path;
    }

    return reinterpret_cast<ConfigProvisionedResource*>(prov_resource_fake);
}

//////// Object type //////////////////////

const gchar *g_dbus_object_get_object_path(GDBusObject *object)
{
    const gchar *object_path = nullptr;

    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    if (object_fake)
    {
        object_path = object_fake->object_path;
    }

    return object_path;
}

/* Not used in actual source code or in dbus mock */
GTypeInstance* g_type_check_instance_cast(GTypeInstance *instance, GType iface_type)
{
    // Used only in internal glib stuff. Just return instance as it is.
    return instance;
}

GType g_dbus_proxy_get_type(void)
{
    return kGTypeDBusProxy;
}

/* Not used in actual source code or in dbus mock */
GType object_get_type(void)
{
    return 0;
}

GType object_proxy_get_type(void)
{
    return kGTypeObjectProxy;
}

GType config_encoding_proxy_get_type(void)
{
    return kGTypeConfigEncoding;
}

GType config_source_proxy_get_type(void)
{
    return kGTypeConfigSource;
}

GType config_resource_proxy_get_type(void)
{
    return kGTypeConfigResource;
}

GType config_provisioned_resource_proxy_get_type(void)
{
    return kGTypeProvisioningResource;
}

GType config_feature_proxy_get_type(void)
{
    return kGTypeDiscoveryFeature;
}

const gchar *config_encoding_get_name(ConfigEncoding *object)
{
    const gchar *object_name = nullptr;

    ConfigEncodingFake *object_fake = reinterpret_cast<ConfigEncodingFake*>(object);

    if (object_fake)
    {
        object_name = object_fake->object_name;
    }

    return object_name;
}

guint config_feature_get_state(ConfigFeature *object)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    return dbus_interface_mock.GetState(object_fake);
}

const gchar *config_feature_get_description(ConfigFeature *object)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    return dbus_interface_mock.GetFeatureProperty(object_fake, fsm::kDescriptionIndex);
}

const gchar *config_feature_get_uri(ConfigFeature *object)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    return dbus_interface_mock.GetFeatureProperty(object_fake, fsm::kUriIndex);
}

const gchar *config_feature_get_icon(ConfigFeature *object)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    return dbus_interface_mock.GetFeatureProperty(object_fake, fsm::kIconIndex);
}

const gchar * const *config_feature_get_tags(ConfigFeature *object)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    return dbus_interface_mock.GetTags(object_fake);
}

const gchar *config_provisioned_resource_get_source(ConfigProvisionedResource *object)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    return dbus_interface_mock.GetProvisionedResourceSource(object_fake);
}

const gchar *config_provisioned_resource_get_encoding(ConfigProvisionedResource *object)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    return dbus_interface_mock.GetProvisionedResourceEncoding(object_fake);
}

const gchar *config_source_get_name(ConfigSource *object)
{
    const gchar *name = nullptr;

    ConfigSourceFake *object_fake = reinterpret_cast<ConfigSourceFake*>(object);

    if (object_fake)
    {
        name = object_fake->object_name;
    }

    return name;
}

gboolean config_source_call_get_encodings_sync(ConfigSource *proxy,
                                               const gchar *arg_resource,
                                               gchar ***out_encodings,
                                               GCancellable *cancellable,
                                               GError **error)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(proxy);

    return dbus_interface_mock.GetSourceEncodings(object_fake,
                                      arg_resource,
                                      out_encodings);
}

gboolean config_source_call_get_payload_sync(ConfigSource *proxy,
                                             const gchar *arg_resource,
                                             const gchar *arg_encoding,
                                             GVariant **out_payload,
                                             GCancellable *cancellable,
                                             GError **error)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(proxy);

    dbus_interface_mock.GetSourcePayload(object_fake, arg_encoding, arg_resource, out_payload);

    return true;
}

gboolean config_source_call_has_resource_sync(ConfigSource *proxy,
                                              const gchar *arg_resource,
                                              gboolean *out_has_resource,
                                              GCancellable *cancellable,
                                              GError **error)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(proxy);

    return dbus_interface_mock.GetSourceHasResource(object_fake,
                                                    arg_resource,
                                                    out_has_resource);
}

const gchar *config_resource_get_name(ConfigResource *object)
{
    const gchar *name = nullptr;

    ConfigResourceFake *object_fake = (ConfigResourceFake*)object;

    if (object_fake)
    {
        name = object_fake->object_name;
    }

    return name;
}

const gchar * const *config_resource_get_sources(ConfigResource *object)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    return dbus_interface_mock.GetResourceSources(object_fake);
}

const gchar * const *config_resource_get_encodings(ConfigResource *object)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(object);

    return dbus_interface_mock.GetResourceEncodings(object_fake);
}

gboolean config_resource_call_get_payload_sync(ConfigResource *proxy,
                                               const gchar *arg_encoding,
                                               const gchar *arg_source,
                                               GVariant **out_payload,
                                               GCancellable *cancellable,
                                               GError **error)
{
    GDBusObjectFake *object_fake = reinterpret_cast<GDBusObjectFake*>(proxy);

    dbus_interface_mock.GetResourcePayload(object_fake, arg_encoding, arg_source, out_payload);

    return true;
}

// Discovery instance to DBus Object cloud connection status.
struct CloudConnectionStatus
{
    char certificate_validity_status;

    char central_connection_status;
    std::string central_server_uri;
    std::string central_date_time;

    char regional_connection_status;
    std::string regional_server_uri;
    std::string regional_date_time;

    CloudConnectionStatus(char certificate_validity_status,
                          std::string central_server_uri,
                          std::string central_date_time,
                          char regional_connection_status,
                          std::string regional_server_uri,
                          std::string regional_date_time)
        : certificate_validity_status(certificate_validity_status),
          central_server_uri(central_server_uri),
          central_date_time(central_date_time),
          regional_connection_status(regional_connection_status),
          regional_server_uri(regional_server_uri),
          regional_date_time(regional_date_time)
    {
    }
};

static GVariant* global_connection_status;

GVariant* discovery_get_cloud_connection_status(Discovery* object)
{
    return global_connection_status;
}

void discovery_set_cloud_connection_status(Discovery *object, GVariant* value)
{
    // Free preivous connection status, if it exists.
    if (global_connection_status)
    {
        g_variant_unref(global_connection_status);
    }

    global_connection_status = value;
}


gulong g_signal_connect_data(gpointer instance,
                             const gchar *detailed_signal,
                             GCallback c_handler,
                             gpointer data,
                             GClosureNotify destroy_data,
                             GConnectFlags connect_flags)
{
    dbus_interface_mock.RegisterSignal(instance, detailed_signal, c_handler, data);

    return true;
}

GType g_dbus_object_get_type (void)
{
    return 0;
}

GList *g_dbus_object_manager_get_objects(GDBusObjectManager *manager)
{
    // Considering no object yet exist.
    return NULL;
}

GDBusObjectManager *g_dbus_object_manager_client_new_for_bus_sync(GBusType                       bus_type,
                                                                  GDBusObjectManagerClientFlags  flags,
                                                                  const gchar                    *name,
                                                                  const gchar                    *object_path,
                                                                  GDBusProxyTypeFunc             get_proxy_type_func,
                                                                  gpointer                       get_proxy_type_user_data,
                                                                  GDestroyNotify                 get_proxy_type_destroy_notify,
                                                                  GCancellable                   *cancellable,
                                                                  GError                         **error)
{
    GDBusObjectManagerFake* GDBus_obj_mgr = static_cast<GDBusObjectManagerFake*>(calloc(1, sizeof(GDBusObjectManagerFake)));

    if (GDBus_obj_mgr)
    {
        GDBus_obj_mgr->object_name = static_cast<char*>(calloc(1, strlen(name) + 1));
        GDBus_obj_mgr->object_path = static_cast<char*>(calloc(1, strlen(object_path) + 1));

        if (GDBus_obj_mgr->object_name
            && GDBus_obj_mgr->object_path)
        {
            strcpy(GDBus_obj_mgr->object_name, name);
            strcpy(GDBus_obj_mgr->object_path, object_path);
        }
    }

    return reinterpret_cast<GDBusObjectManager*>(GDBus_obj_mgr);
}

/** \}    end of addtogroup */
