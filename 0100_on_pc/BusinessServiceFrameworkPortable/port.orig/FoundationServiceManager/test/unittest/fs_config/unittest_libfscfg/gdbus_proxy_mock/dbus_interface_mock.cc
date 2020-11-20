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
 *  \file     dbus_interface_mock.cc
 *  \brief    dbus interface mock implementation
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm
 *  \{
 */

#include <gio/gio.h>
#include <cstring>
#include <thread>
#include <cassert>
#include <functional>
#include <algorithm>
#include <chrono>
#include <thread>

#include "dbus_interface_mock.h"


namespace fsm
{

void DBusInterfaceMock::CreateDefaultGDBusObjects()
{
    CreateGDBusObject(encoding_objects_, fscfg_kBotConfigEncoding);
    CreateGDBusObject(source_objects_, fscfg_kBotConfigSource);
    CreateGDBusObject(resource_objects_, fscfg_kBotConfigResource);

    // Creating ProvivisonedResources
    // First register as a resource then make it provisioned resource
    CreateGDBusObject(prov_resource_objects_, fscfg_kBotConfigResource);
    CreateGDBusObject(prov_resource_objects_, fscfg_kBotProvisioningResource);

    // Creating discovery feature resource
    CreateGDBusObject(feature_objects_, fscfg_kBotConfigResource);
    CreateGDBusObject(feature_objects_, fscfg_kBotProvisioningResource);
    CreateGDBusObject(feature_objects_, fscfg_kBotDiscoveryFeature);

}

void DBusInterfaceMock::CreateGDBusObject(std::vector<std::string> names, BusObjectType object_type)
{
    const char* container_path = nullptr;

    fscfg_GetObjectPath(&container_path, object_type);

    std::string full_path(container_path);

    std::string base_name;
    GetBasename(full_path, base_name);

    for (auto it : names)
    {
        std::string name(it);

        // Create G Variant
        GVariant *g_var = nullptr;
        g_var = g_variant_new("s", name.c_str());

        std::string object_path = full_path + "/" + name;

        // Allocate memory for the dbus object
        GDBusObjectFake *object_fake = static_cast<GDBusObjectFake*>(calloc(1, sizeof(GDBusObjectFake)));

        if (object_fake)
        {
            object_fake->object_name = static_cast<char*>(calloc(1, name.size()));
            object_fake->object_path = static_cast<char*>(calloc(1, object_path.size()));

            if (object_fake->object_name
                && object_fake->object_path)
            {
                // Copy name and path to GDBusObject
                std::strcpy(object_fake->object_name, name.c_str());
                std::strcpy(object_fake->object_path, object_path.c_str());

                // Trigger object-added signal.
                TriggerObjectAddRemoveSignal("object-added", object_fake, full_path);

                // Create and trigger properties change signal
                CreatePropertiesChangedSignal(base_name, object_type, g_var);

                // Store in container
                object_fake_set_.push_back(std::make_tuple(name, object_type, full_path, object_fake, g_var));

            }
        }
        else
        {
            assert(("Failed to allocate memory for object fake", 0));
        }
    }
}

void DBusInterfaceMock::CreatePropertiesChangedSignal(std::string base_name,
                                                      BusObjectType object_type,
                                                      GVariant* g_var)
{
    // Prepare properties changed signal.

    const char* base_path = nullptr;

    if (object_type == fsm::fscfg_kBotProvisioningResource)
    {
        // Trigger Properties update on provisioning


        // It is not called "ProvisionedResource" as property name
        // in dbus configuration file. So, it hardcoded here.
        // Provisioning only has "Resources" as property.
        base_name = "Resources";

        fscfg_GetObjectPath(&base_path, fsm::fscfg_kBotProvisioning);
    }
    else if (object_type == fsm::fscfg_kBotDiscoveryFeature)
    {
        // Discovery property name is "Resources"
        base_name = "Resources";

        fscfg_GetObjectPath(&base_path, fsm::fscfg_kBotDiscovery);
    }
    else
    {
        fscfg_GetObjectPath(&base_path, fsm::fscfg_kBotConfig);
    }
    GVariant *changed = nullptr;
    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE ("a{sv}"));

    g_variant_builder_add(builder, "{sv}", base_name.c_str(), g_var);

    changed = g_variant_new ("a{sv}", builder);
    g_variant_builder_unref (builder);

    // Trigger g-properties-changed signal
    TriggerPropertiesChangedSignal("g-properties-changed", changed, base_path);

    g_variant_unref(changed);
}

void DBusInterfaceMock::CreatePayload(std::string source_name,
                                      std::string encoding_name,
                                      std::string resource_name,
                                      BusObjectType resource_type,
                                      std::vector<std::uint8_t> payload)
{
    // Prepare full paths.
    const char* source_base;
    const char* encoding_base;
    const char* resource_base;

    fscfg_GetObjectPath(&source_base, fsm::fscfg_kBotConfigSource);
    fscfg_GetObjectPath(&encoding_base, fsm::fscfg_kBotConfigEncoding);

    if (resource_type == fsm::fscfg_kBotProvisioningResource)
    {
        fscfg_GetObjectPath(&resource_base, fsm::fscfg_kBotProvisioningResource);
    }
    else
    {
        fscfg_GetObjectPath(&resource_base, fsm::fscfg_kBotConfigResource);
    }

    // Create full paths
    const std::string source_path = std::string(source_base) + "/" + source_name;
    const std::string encoding_path = std::string(encoding_base) + "/" + encoding_name;
    const std::string resource_path = std::string(resource_base) + "/" + resource_name;

    // Create payload GVariant builder
    GVariant* g_payload_builder = ConvertByteToVariant(payload);

    // Get g variant with value and store in container for later reference
    GVariant* g_payload;
    g_variant_get(g_payload_builder, "@ay", &g_payload);

    // Store in payload container
    payload_set_.push_back(std::make_tuple(source_path, encoding_path, resource_name, g_payload));

    // Trigger payload changed signal to the resource
    TriggerPayloadChangedSignal("payloads-changed", source_path, encoding_path, resource_path, g_payload);

    // Trigger updated signal to the resource
    TriggerResourceUpdateSignal("updated", source_path, resource_path);


    // Prepare property change signal to make changes updated
    GVariant* changed = nullptr;

    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE ("a{sv}"));

    // ProvisioningResource expecting "source" and payload
    if (resource_type == fsm::fscfg_kBotProvisioningResource)
    {
        GVariant* g_source_path = g_variant_new("o", source_path.c_str());

        g_variant_builder_add(builder, "{sv}", "Source", g_source_path);
        g_variant_builder_add(builder, "{sv}", "Payload", g_payload);

    }
    else
    {
        g_variant_builder_add(builder, "{sv}", "Sources", g_payload);
    }

    changed = g_variant_new ("a{sv}", builder);
    g_variant_builder_unref (builder);

    // Trigger properties changed signal.
    TriggerPropertiesChangedSignal("g-properties-changed", changed, resource_path);

    g_variant_unref(changed);
}

GVariant* DBusInterfaceMock::ConvertByteToVariant(std::vector<std::uint8_t> payload)
{
    GVariantBuilder *builder;
    GVariant *value;

    builder = g_variant_builder_new (G_VARIANT_TYPE ("ay"));

    for (auto it : payload)
    {
        g_variant_builder_add (builder, "y", it);
    }

    value = g_variant_new("ay", builder);
    g_variant_builder_unref (builder);

    return value;
}

void DBusInterfaceMock::CreateFeatureProperties(std::string feature_name)
{
    // Prepare full paths.
    const char* feature_base;

    fscfg_GetObjectPath(&feature_base, fsm::fscfg_kBotDiscoveryFeature);

    const std::string feature_path = std::string(feature_base) + "/" + feature_name;


    // Make a dummy gvariant. It is not used in mock or in actual source code.
    GVariant* g_feature = g_variant_new("(ousss)",
                                        feature_path.c_str(),
                                        state_,
                                        description_.c_str(),
                                        uri_.c_str(),
                                        icon_.c_str());

    // Store in feature property container
    feature_properties_.push_back(std::make_tuple(feature_path, state_, description_, uri_, icon_, tags_, g_feature));

    // Prepare property change signal to make changes updated
    GVariant* changed = nullptr;

    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE ("a{sv}"));

    // Create a list to be used as key in properties changed signal
    std::vector<std::string> feature_properties_names = {"State",
                                                         "Description",
                                                         "Uri",
                                                         "Icon",
                                                         "Tags"};

    // Add each property to GVariant builder
    for (auto it : feature_properties_names)
    {
        g_variant_builder_add(builder, "{sv}", it.c_str(), g_feature);
    }

    changed = g_variant_new ("a{sv}", builder);
    g_variant_builder_unref (builder);

    // Trigger properties changed signal.
    TriggerPropertiesChangedSignal("g-properties-changed", changed, feature_path);

    g_variant_unref(changed);
}

void DBusInterfaceMock::DBusClean()
{
    // Clean payload container
    for (auto it : payload_set_)
    {
        g_variant_unref(std::get<kGVariantIndex>(it));
    }

    payload_set_.clear();

    // Clean feature container
    feature_properties_.clear();


    // Clean Object container
    for (auto it : object_fake_set_)
    {
        RemoveGDBusObject(std::get<kObjectNameIndex>(it));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    object_fake_set_.clear();

    // Clean callback container
    callback_data_.clear();
}

void DBusInterfaceMock::RemoveGDBusObject(std::string name)
{
    for (int i=0; i < object_fake_set_.size(); i++)
    {
        auto item = object_fake_set_[i];

        if (!name.compare(std::get<kObjectNameIndex>(item)))
        {

            GDBusObjectFake* object_fake = std::get<kObjectFakeIndex>(item);

            if (object_fake)
            {
                // Trigger remove object
                TriggerObjectAddRemoveSignal("object-removed", object_fake, std::get<kFullPathIndex>(item));

                std::string object_path = std::get<kFullPathIndex>(item) + "/" + name;

                std::string base_name;
                GetBasename(std::get<kFullPathIndex>(item), base_name);

                // Create G Variant
                GVariant *g_var = g_variant_new("s", name.c_str());

                CreatePropertiesChangedSignal(base_name,
                                              std::get<kObjectTypeIndex>(item),
                                              g_var);

                // Remove signal from the registered signal list
                RemoveSignal(object_path);

                // Free memory
                std::free(object_fake->object_path);
                std::free(object_fake->object_name);
                std::free(object_fake);

                // Remove from the object list
                object_fake_set_.erase(object_fake_set_.begin() + i);
                i--;
            }
        }
    }
}

void DBusInterfaceMock::RemoveSignal(std::string object_path)
{
    for (int i=0; i < callback_data_.size(); i++)
    {
        if (!object_path.compare(std::get<kCallbackObjectPathIndex>(callback_data_[i])))
        {
            callback_data_.erase(callback_data_.begin() + i);
            i--;
        }
    }
}

void DBusInterfaceMock::GetResourcePayload(GDBusObjectFake* object_fake,
                                           std::string encoding_path,
                                           std::string source_path,
                                           GVariant **out_payload)
{
    for (auto it : payload_set_)
    {
        std::string resource_name;
        GetBasename(object_fake->object_path, resource_name);

        if (!source_path.compare(std::get<kSourcePathIndex>(it))
            && !encoding_path.compare(std::get<kEncodingPathIndex>(it))
            && !resource_name.compare(std::get<kResourceNameIndex>(it)))
        {
            GVariant *g_var = static_cast<GVariant*>(std::get<kGVariantIndex>(it));

            // Get payload value through getter
            g_variant_get(g_var, "@ay", out_payload);

            break;
        }
    }
}

void DBusInterfaceMock::GetSourcePayload(GDBusObjectFake* object_fake,
                                         std::string encoding_path,
                                         std::string resource_path,
                                         GVariant **out_payload)
{
    for (auto it : payload_set_)
    {
        std::string object_path(object_fake->object_path);

        std::string resource_name;
        GetBasename(resource_path, resource_name);

        if (!resource_name.compare(std::get<kResourceNameIndex>(it))
            && !encoding_path.compare(std::get<kEncodingPathIndex>(it))
            && !object_path.compare(std::get<kSourcePathIndex>(it)))
        {
            GVariant *g_var = static_cast<GVariant*>(std::get<kGVariantIndex>(it));

            // Get payload value through getter
            g_variant_get(g_var, "@ay", out_payload);

            break;
        }
    }
}


const char* const* DBusInterfaceMock::GetResourceEncodings(GDBusObjectFake* object_fake)
{
    const char* const* data = nullptr;

    std::vector<std::string> encoding_names;

    names_.clear();

    if (object_fake)
    {
        std::string resource_name;
        GetBasename(object_fake->object_path, resource_name);

        for (auto it : payload_set_)
        {
            if (!resource_name.compare(std::get<kResourceNameIndex>(it)))
            {
                std::string name;

                GetBasename(std::get<kEncodingPathIndex>(it), name);

                encoding_names.push_back(name);
            }
        }

        // push the values to names_
        ConvertStringsToChars(encoding_names, names_);

        // Ends with null terminator
        names_.push_back('\0');

        // Store data to return value
        data = names_.data();
    }

    return data;
}

bool DBusInterfaceMock::GetSourceEncodings(GDBusObjectFake* object_fake,
                                           const char *resource_path,
                                           char*** out_encoding_names)
{
    const char** data = nullptr;

    std::vector<std::string> encoding_names;

    names_.clear();

    if (object_fake)
    {
        std::string object_path(object_fake->object_path);

        std::string resource_name;
        GetBasename(resource_path, resource_name);

        for (auto it : payload_set_)
        {
            if (!object_path.compare(std::get<kSourcePathIndex>(it))
                && !resource_name.compare(std::get<kResourceNameIndex>(it)))
            {
                std::string name;

                GetBasename(std::get<kEncodingPathIndex>(it), name);

                encoding_names.push_back(name);
            }
        }

        // push the values to names_
        ConvertStringsToChars(encoding_names, names_);

        // Ends with null terminator
        names_.push_back('\0');

        *out_encoding_names = static_cast<char**>(calloc(1, sizeof(char**)*names_.size()));
        *out_encoding_names = const_cast<char**>(names_.data());
    }

    return true;
}

bool DBusInterfaceMock::GetSourceHasResource(GDBusObjectFake* object_fake,
                                             const char* resource_path,
                                             int* out_has_resource)
{
    *out_has_resource = false;

    if (object_fake)
    {
        std::string object_path(object_fake->object_path);

        std::string resource_name;
        GetBasename(resource_path, resource_name);

        for (auto it : payload_set_)
        {
            if (!object_path.compare(std::get<kSourcePathIndex>(it))
                && !resource_name.compare(std::get<kResourceNameIndex>(it)))
            {
                *out_has_resource = true;
            }
        }
    }

    return true;
}

const char* const * DBusInterfaceMock::GetResourceSources(GDBusObjectFake* object_fake)
{
    const char* const* data = nullptr;

    std::vector<std::string> source_names;

    names_.clear();

    if (object_fake)
    {
        std::string resource_name;
        GetBasename(object_fake->object_path, resource_name);

        for (auto it : payload_set_)
        {
            if (!resource_name.compare(std::get<kResourceNameIndex>(it)))
            {
                std::string name;

                GetBasename(std::get<kSourcePathIndex>(it), name);

                source_names.push_back(name);
            }
        }

        // push the values to names_
        ConvertStringsToChars(source_names, names_);

        // Ends with null terminator
        names_.push_back('\0');

        // Store data to return value
        data = names_.data();
    }

    return data;
}

const char* DBusInterfaceMock::GetProvisionedResourceSource(GDBusObjectFake* object_fake)
{
    const char* data = nullptr;

    name_.clear();

    if (object_fake)
    {
        std::string resource_name;
        GetBasename(object_fake->object_path, resource_name);

        for (auto it : payload_set_)
        {
            if (!resource_name.compare(std::get<kResourceNameIndex>(it)))
            {
                GetBasename(std::get<kSourcePathIndex>(it), name_);
                data = name_.c_str();
            }
        }
    }

    return data;
}

const char* DBusInterfaceMock::GetProvisionedResourceEncoding(GDBusObjectFake* object_fake)
{
    const char* data = nullptr;

    name_.clear();

    if (object_fake)
    {
        std::string resource_name;
        GetBasename(object_fake->object_path, resource_name);

        for (auto it : payload_set_)
        {
            if (!resource_name.compare(std::get<kResourceNameIndex>(it)))
            {
                GetBasename(std::get<kEncodingPathIndex>(it), name_);

                data = name_.c_str();
            }
        }
    }

    return data;
}

guint DBusInterfaceMock::GetState(GDBusObjectFake* object_fake)
{
    guint state = 0;

    if (object_fake)
    {
        std::string object_path(object_fake->object_path);

        for (auto it : feature_properties_)
        {
            if (!object_path.compare(std::get<kObjectPathIndex>(it)))
            {
                state = std::get<kStateIndex>(it);
            }
        }
    }

    return state;
}

const char* DBusInterfaceMock::GetFeatureProperty(GDBusObjectFake* object_fake, const std::size_t index)
{
    const char* data = nullptr;

    name_.clear();

    if (object_fake)
    {
        std::string object_path(object_fake->object_path);

        for (auto it : feature_properties_)
        {
            if (!object_path.compare(std::get<kObjectPathIndex>(it)))
            {
                std::string feature_property;

                if (index == kDescriptionIndex)
                {
                    feature_property = std::get<kDescriptionIndex>(it);
                }
                else if (index == kUriIndex)
                {
                    feature_property = std::get<kUriIndex>(it);
                }
                else if (index == kIconIndex)
                {
                    feature_property = std::get<kIconIndex>(it);
                }

                name_.append(feature_property);

                data = name_.c_str();
            }
        }
    }

    return data;
}

const char* const * DBusInterfaceMock::GetTags(GDBusObjectFake* object_fake)
{
    const char* const* data = nullptr;

    names_.clear();

    if (object_fake)
    {
        std::string object_path(object_fake->object_path);

        for (auto it : feature_properties_)
        {
            if (!object_path.compare(std::get<kObjectPathIndex>(it)))
            {
                std::vector<std::string> tags = std::get<kTagsIndex>(it);

                ConvertStringsToChars(tags, names_);
            }
        }

        // Ends with null terminator
        names_.push_back('\0');

        // Store data to return value
        data = names_.data();
    }

    return data;
}

void DBusInterfaceMock::ConvertStringsToChars(std::vector<std::string>& strings, std::vector<const char*>& chars)
{
    for (unsigned i=0; i < strings.size(); ++i)
    {
        chars.push_back(strings[i].c_str());
    }
}

void DBusInterfaceMock::RegisterSignal(gpointer object,
                                       const char *detailed_signal,
                                       GCallback c_handler,
                                       gpointer data)
{
    GDBusObjectFake *object_fake = static_cast<GDBusObjectFake*>(object);

    callback_data_.push_back(std::make_tuple(std::string(object_fake->object_path),
                                             object,
                                             detailed_signal,
                                             c_handler,
                                             data));
}

void DBusInterfaceMock::TriggerObjectAddRemoveSignal(std::string signal_name, gpointer object, std::string object_path)
{
    for (auto it : callback_data_)
    {
        if (!signal_name.compare(std::get<kSingalNameIndex>(it)))
        {

            GDBusObjectFake *obj_mgr = static_cast<GDBusObjectFake*>(std::get<kObjectIndex>(it));

            if (!object_path.compare(obj_mgr->object_path))
            {
                ((void (*)(GDBusObjectManager*, GDBusObject*, gpointer))(std::get<kCallbackFunctionIndex>(it)))
                                                                        (static_cast<GDBusObjectManager*>(std::get<kObjectIndex>(it)),
                                                                         static_cast<GDBusObject*>(object),
                                                                         std::get<kUserDataIndex>(it));
                break;
            }
        }
    }
}

void DBusInterfaceMock::SetResourceExpired(std::string resource_name)
{
    TriggerResourceExpiredSignal("expired", resource_name);
}


void DBusInterfaceMock::TriggerResourceExpiredSignal(std::string signal_name, std::string object_path)
{
    for (auto it : callback_data_)
    {
        if (!signal_name.compare(std::get<kSingalNameIndex>(it)))
        {

            GDBusObjectFake *obj_mgr = static_cast<GDBusObjectFake*>(std::get<kObjectIndex>(it));

            if (!object_path.compare(obj_mgr->object_name))
            {
                ((void (*)(_ConfigResource*, gboolean, gpointer))(std::get<kCallbackFunctionIndex>(it)))
                                                                 (static_cast<_ConfigResource*>(std::get<kObjectIndex>(it)),
                                                                  true,
                                                                  std::get<kUserDataIndex>(it));
                break;
            }
        }
    }
}

void DBusInterfaceMock::TriggerPropertiesChangedSignal(std::string signal_name, GVariant* g_variant, std::string object_path)
{
    for (auto it : callback_data_)
    {
        if (!signal_name.compare(std::get<kSingalNameIndex>(it)))
        {
            GDBusObjectFake *obj_mgr = static_cast<GDBusObjectFake*>(std::get<kObjectIndex>(it));

            if (!object_path.compare(obj_mgr->object_path))
            {
                ((void (*)(GDBusProxy*, GVariant*, GStrv, gpointer))(std::get<kCallbackFunctionIndex>(it)))(NULL,
                                                                                                            g_variant,
                                                                                                            NULL,
                                                                                                            std::get<kUserDataIndex>(it));
                break;
            }
        }
    }
}

void DBusInterfaceMock::TriggerPayloadChangedSignal(std::string signal_name,
                                                    std::string source_path,
                                                    std::string encoding_path,
                                                    std::string resource_path,
                                                    GVariant* g_payload)
{
    for (auto it : callback_data_)
    {

        if (!signal_name.compare(std::get<kSingalNameIndex>(it)))
        {

            GDBusObjectFake *obj_mgr = static_cast<GDBusObjectFake*>(std::get<kObjectIndex>(it));

            if (!resource_path.compare(obj_mgr->object_path))
            {

                ((void (*)(_ConfigResource*,  // Callback in parameters
                           const gchar *,
                           const gchar *,
                           GVariant*,
                           gpointer))(std::get<kCallbackFunctionIndex>(it)))   // Callback function name
                                     (static_cast<_ConfigResource*>(std::get<kObjectIndex>(it)), // Callback out paramters
                                      encoding_path.c_str(),
                                      source_path.c_str(),
                                      g_payload,
                                      std::get<kUserDataIndex>(it));
                break;
            }
        }
    }
}

void DBusInterfaceMock::TriggerResourceUpdateSignal(std::string signal_name,
                                                    std::string source_path,
                                                    std::string resource_path)
{
    for (auto it : callback_data_)
    {

        if (!signal_name.compare(std::get<kSingalNameIndex>(it)))
        {

            GDBusObjectFake *obj_mgr = static_cast<GDBusObjectFake*>(std::get<kObjectIndex>(it));

            if (!resource_path.compare(obj_mgr->object_path))
            {

                ((void (*)(_ConfigResource*,  // Callback in parameters
                           const gchar *,
                           gpointer))(std::get<kCallbackFunctionIndex>(it)))   // Callback function name
                                     (static_cast<_ConfigResource*>(std::get<kObjectIndex>(it)), // Callback out paramters
                                      source_path.c_str(),
                                      std::get<kUserDataIndex>(it));
                break;
            }
        }
    }
}

} // namespace fsm

/** \}    end of addtogroup */
