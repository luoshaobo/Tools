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
 *  \brief    Mock of foundation services config generated code.
 *            For unittest the mocked code will be used instead of
 *            using actual generated code.
 *            Note: This is not a generated code.
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FOUNDATION_SERVICES_CONFIG_GENERATED_
#define FOUNDATION_SERVICES_CONFIG_GENERATED_

#include <gio/gio.h>

struct GDBusObjectFake
{
    char *object_name;
    char *object_path;
};

typedef struct GDBusObjectFake GDBusObjectManagerFake;

/////// GDBus Fake Container Objects
typedef struct GDBusObjectFake ConfigFake;
typedef struct GDBusObjectFake DiscoveryFake;
typedef struct GDBusObjectFake ProvisioningFake;

/////// GDBus Fake Objects
typedef struct GDBusObjectFake ConfigEncodingFake;
typedef struct GDBusObjectFake ConfigSourceFake;
typedef struct GDBusObjectFake ConfigResourceFake;
typedef struct GDBusObjectFake ConfigProvisionedResourceFake;
typedef struct GDBusObjectFake ConfigFeatureFake;

/**
 * \brief Fake gtypes
 */
enum GDBusObjectProxyType
{
    kGTypeUndefined = 0,            ///< Undefined.

    kGTypeObjectProxy,              ///< Object proxy GType.
    kGTypeDBusProxy,                ///< DBus proxy GType.
    kGTypeConfigEncoding,           ///< Config Encoding GType.
    kGTypeConfigSource,             ///< Config Source GType.
    kGTypeConfigResource,           ///< Config Resource GType.
    kGTypeProvisioningResource,     ///< Provisioning ProvisionedResource GType.
    kGTypeDiscoveryFeature,         ///< Discovery Feature GType.

    kGTypeMax
};

/////// GDBus Objects
struct _Object;
typedef struct _Object Object;

struct _Config;
typedef struct _Config Config;

struct _Discovery;
typedef struct _Discovery Discovery;

struct _Provisioning;
typedef struct _Provisioning Provisioning;

struct _ConfigEncoding;
typedef struct _ConfigEncoding ConfigEncoding;

struct _ConfigSource;
typedef struct _ConfigSource ConfigSource;

struct _ConfigResource;
typedef struct _ConfigResource ConfigResource;

struct _ConfigFeature;
typedef struct _ConfigFeature ConfigFeature;

struct _ConfigProvisionedResource;
typedef struct _ConfigProvisionedResource ConfigProvisionedResource;

#define TYPE_OBJECT (object_get_type ())
#define OBJECT(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_OBJECT, Object))

#define G_TYPE_DBUS_OBJECT (g_dbus_object_get_type())
#define G_DBUS_OBJECT(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), G_TYPE_DBUS_OBJECT, GDBusObject))

Config *config_proxy_new_for_bus_sync(GBusType             bus_type,
                                      GDBusProxyFlags      flags,
                                      const gchar          *name,
                                      const gchar          *object_path,
                                      GCancellable         *cancellable,
                                      GError               **error);

Discovery *
discovery_proxy_new_for_bus_sync(GBusType             bus_type,
                                 GDBusProxyFlags      flags,
                                 const gchar          *name,
                                 const gchar          *object_path,
                                 GCancellable         *cancellable,
                                 GError               **error);

Provisioning *
provisioning_proxy_new_for_bus_sync(GBusType             bus_type,
                                    GDBusProxyFlags      flags,
                                    const gchar          *name,
                                    const gchar          *object_path,
                                    GCancellable         *cancellable,
                                    GError               **error);

ConfigEncoding *object_get_config_encoding(Object *object);

ConfigSource *object_get_config_source(Object *object);

ConfigResource *object_get_config_resource(Object *object);

ConfigFeature *object_get_config_feature(Object *object);

ConfigProvisionedResource *object_get_config_provisioned_resource(Object *object);

//////// Object type

const gchar *g_dbus_object_get_object_path(GDBusObject  *object);

GTypeInstance* g_type_check_instance_cast(GTypeInstance *instance, GType iface_type);

GType g_dbus_proxy_get_type(void);

GType object_get_type(void);

GType object_proxy_get_type(void);

GType config_encoding_proxy_get_type(void);

GType config_source_proxy_get_type(void);

GType config_resource_proxy_get_type(void);

GType config_provisioned_resource_proxy_get_type(void);

GType config_feature_proxy_get_type(void);


const gchar *
config_encoding_get_name(ConfigEncoding *object);

guint
config_feature_get_state(ConfigFeature *object);

const gchar *
config_feature_get_description(ConfigFeature *object);

const gchar *
config_feature_get_uri(ConfigFeature *object);

const gchar *
config_feature_get_icon(ConfigFeature *object);

const gchar *const *
config_feature_get_tags(ConfigFeature *object);

const gchar *
config_provisioned_resource_get_source(ConfigProvisionedResource *object);

const gchar *
config_provisioned_resource_get_encoding(ConfigProvisionedResource *object);

const gchar *
config_source_get_name(ConfigSource *object);

gboolean
config_source_call_get_encodings_sync(ConfigSource *proxy,
                                      const gchar *arg_resource,
                                      gchar ***out_encodings,
                                      GCancellable *cancellable,
                                      GError **error);

gboolean
config_source_call_get_payload_sync(ConfigSource *proxy,
                                    const gchar *arg_resource,
                                    const gchar *arg_encoding,
                                    GVariant **out_payload,
                                    GCancellable *cancellable,
                                    GError **error);

gboolean
config_source_call_has_resource_sync(ConfigSource *proxy,
                                     const gchar *arg_resource,
                                     gboolean *out_has_resource,
                                     GCancellable *cancellable,
                                     GError **error);

const gchar *
config_resource_get_name(ConfigResource *object);

const gchar *const *
config_resource_get_sources(ConfigResource *object);

const gchar *const *
config_resource_get_encodings(ConfigResource *object);

gboolean
config_resource_call_get_payload_sync(ConfigResource *proxy,
                                      const gchar *arg_encoding,
                                      const gchar *arg_source,
                                      GVariant **out_payload,
                                      GCancellable *cancellable,
                                      GError **error);

GVariant *discovery_get_cloud_connection_status (Discovery *object);

void discovery_set_cloud_connection_status (Discovery *object, GVariant *value);

gulong
g_signal_connect_data(gpointer           instance,
                      const gchar        *detailed_signal,
                      GCallback          c_handler,
                      gpointer           data,
                      GClosureNotify	 destroy_data,
                      GConnectFlags	     connect_flags);

GType g_dbus_object_get_type (void) G_GNUC_CONST;

GList *g_dbus_object_manager_get_objects (GDBusObjectManager *manager);

GDBusObjectManager *
g_dbus_object_manager_client_new_for_bus_sync(GBusType                       bus_type,
                                              GDBusObjectManagerClientFlags  flags,
                                              const gchar                    *name,
                                              const gchar                    *object_path,
                                              GDBusProxyTypeFunc             get_proxy_type_func,
                                              gpointer                       get_proxy_type_user_data,
                                              GDestroyNotify                 get_proxy_type_destroy_notify,
                                              GCancellable                   *cancellable,
                                              GError                         **error);

#endif // FOUNDATION_SERVICES_CONFIG_GENERATED_

/** \}    end of addtogroup */
