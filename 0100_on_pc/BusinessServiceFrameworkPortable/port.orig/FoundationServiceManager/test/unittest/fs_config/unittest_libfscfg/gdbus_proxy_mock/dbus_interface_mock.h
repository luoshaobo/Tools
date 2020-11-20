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
 *  \file     dbus_interface_mock.h
 *  \brief    DBus interface mock it constructs GDBus objects with dummy
 *            contents and facilities GDBus mocked generated code and unittest
 *            to get, add, modify and/or remove objects and contents on the bus.
 *  \author   Imran Siddique
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_DBUS_INTERFACE_MOCK_H_INC_
#define FSM_DBUS_INTERFACE_MOCK_H_INC_


#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <gio/gio.h>

#include <fscfg_common/generated/foundation-services-config-generated.h>

#include <fscfg_common/utilities.h>
#include <fscfg/bus_names.h>


namespace fsm
{

    //Feature properties tuple indexes defines
    const std::size_t kObjectPathIndex = 0;                /// Path of object
    const std::size_t kStateIndex = 1;                     /// Feature state
    const std::size_t kDescriptionIndex = 2;               /// Feature description
    const std::size_t kUriIndex = 3;                       /// Feature URI
    const std::size_t kIconIndex = 4;                      /// Feature icon
    const std::size_t kTagsIndex = 5;                      /// Feature Tags
    const std::size_t kFeatureGVariantIndex = 6;           /// Feature g_variant

    // feature_properties data container
    typedef std::tuple<std::string,                 /// Object path
                       guint,                       /// Feature state
                       std::string,                 /// Feature description
                       std::string,                 /// Feature URI
                       std::string,                 /// Feature icon
                       std::vector<std::string>,    /// Feature Tags
                       GVariant*                    /// Feature g_variant
                       > feature_properties;        /// Dataset for fs config feature properties

    // Set of feature properties
    static std::vector<feature_properties> feature_properties_;

    // Callback tuple indexes defines
    const std::size_t kCallbackObjectPathIndex = 0;     /// Path of object which signal is registered
    const std::size_t kObjectIndex = 1;                 /// Object
    const std::size_t kSingalNameIndex = 2;             /// Name of the signal
    const std::size_t kCallbackFunctionIndex = 3;       /// Callback function
    const std::size_t kUserDataIndex = 4;               /// User data provided by the signal register

    // Callback data container
    typedef std::tuple<std::string,         /// Object path
                       gpointer,            /// Object pointer
                       const char*,         /// Singal name
                       GCallback,           /// Callback function
                       gpointer             /// User data pointer
                       > callback_data;     /// dataset for callback data

    // Set of all registered callbacks
    static std::vector<callback_data> callback_data_;


    // Payload tuple indexes defines
    const std::size_t kSourcePathIndex = 0;     /// Payload source path
    const std::size_t kEncodingPathIndex = 1;   /// Payload encoding path
    const std::size_t kResourceNameIndex = 2;   /// Payload resource name
    const std::size_t kGVariantIndex = 3;       /// Payload G Variant

    // Payload data container
    typedef std::tuple<std::string,         /// Source path
                       std::string,         /// Encoding path
                       std::string,         /// Resource name
                       GVariant*            /// Payload g_variant
                       > payload_set;       /// Dataset for payloads

    // Set of payloads data
    static std::vector<payload_set> payload_set_;


    //Fake object tuple indexes defines
    const std::size_t kObjectNameIndex = 0;
    const std::size_t kObjectTypeIndex = 1;
    const std::size_t kFullPathIndex = 2;
    const std::size_t kObjectFakeIndex = 3;
    const std::size_t kObjectGVariantIndex = 4;

    typedef std::tuple<std::string,         /// Object name
                       BusObjectType,       /// Object type
                       std::string,         /// Object full path
                       GDBusObjectFake*,    /// Object fake pointer
                       GVariant*            /// g_variant created for object
                       > object_fake_set;   /// Dataset for object fake

    // Set of object fake data
    static std::vector<object_fake_set> object_fake_set_;

    // Tempory hold the string data.
    static std::vector<const char*> names_;
    static std::string name_;


/**
 * \brief D-Bus interface mock
 */
class DBusInterfaceMock
{
public:

    /**
     * \brief Default dbus object properties used to create dbus objects
     */
    std::vector<std::string> encoding_objects_ = {"Encoding1", "Encoding2", "Encoding3"};
    std::vector<std::string> source_objects_ = {"Source1", "Source2", "Source3"};
    std::vector<std::string> resource_objects_ = {"Resource1", "Resource2", "Resource3"};

    std::vector<std::string> prov_resource_objects_ = {"Prov_Resource1", "Prov_Resource2", "Prov_Resource3"};

    std::vector<std::string> feature_objects_ = {"Feature1", "Feature2", "Feature3"};

    /**
     * \brief Default contents for feature properties
     */
    guint state_ = 1;
    std::string description_ = "First Feature";
    std::string uri_ = "<uri>/feature-1</uri>";
    std::string icon_ = "<icon>/img/icon1.png</icon>";
    std::vector<std::string> tags_ = {"tag1", "tag2", "tag3"};

    /**
     * \brief Creates default objects based on default object properties
     */
    void CreateDefaultGDBusObjects();

    /**
     * \brief Creates feature properties based on default feature properties
     *
     * \param[in] feature_name name of the feature to bind with feature properites
     */
    void CreateFeatureProperties(std::string feature_name);

    /**
     * \brief Creates dbus objects based on list of names provided with object type
     *        Also trigger object-added and g-properties-changed signals
     *
     * \param[in] names       list of names of object to be created
     * \param[in] object_type Type of the object
     */
    void CreateGDBusObject(std::vector<std::string> names, BusObjectType object_type);

    /**
     * \brief Creates payload and binds source, encoding and resource paths
     *        Also trigger payloads-changed, updated and g-properties-changed signals
     *
     * \param[in] source_name Source name which payload belongs to.
     * \param[in] encoding_name Payload encoding schema name
     * \param[in] resource_name Resource that owns payload
     * \param[in] resource_type Resource or Provisioned resource
     * \param[in] payload actual payload
     *
     */
    void CreatePayload(std::string source_name,
                       std::string encoding_name,
                       std::string resource_name,
                       BusObjectType resource_type,
                       std::vector<std::uint8_t> payload);

    /**
     * \brief Register a callback function
     *        mock implemention of "g_signal_connect_data()"
     *        For parameter detail see original function
     *
     * \param[in] object           GDbus object
     * \param[in] detailed_signal  Signal name
     * \param[in] c_handler        Callback function
     * \param[in] data             The data to be return with callback
     *
     */
    void RegisterSignal(gpointer object, const char *detailed_signal, GCallback c_handler, gpointer data);

    /**
     * \brief Triggers object add or remove callback function
     *
     * \param[in] signal_name Signal name
     * \param[in] object      GDbus object
     * \param[in] object_path To be match in registered signal list
     */
    void TriggerObjectAddRemoveSignal(std::string signal_name, gpointer object, std::string object_path);

    /**
     * \brief Triggers properties changed callback function
     *
     * \param[in] signal_name Signal name
     * \param[in] g_variant   To identify which object have been changed
     * \param[in] object_path To be match in registered signal list
     */
    void TriggerPropertiesChangedSignal(std::string signal_name, GVariant* g_variant, std::string object_path);

    /**
     * \brief Triggers payload changed callback function
     *
     * \param[in] signal_name    Signal name
     * \param[in] source_path    To be send to callback function
     * \param[in] encoding_path  To be send to callback function
     * \param[in] resource_path  To be match in registered signal list
     * \param[in] g_payload      GVariant of changed payload
     */
    void TriggerPayloadChangedSignal(std::string signal_name,
                                     std::string source_path,
                                     std::string encoding_path,
                                     std::string resource_path,
                                     GVariant* g_payload);

    /**
     * \brief Triggers resource update callback function
     *
     * \param[in] signal_name    Signal name
     * \param[in] source_path    To be send to callback function
     * \param[in] resource_path  To be match in registered signal list
     */
    void TriggerResourceUpdateSignal(std::string signal_name,
                                     std::string source_path,
                                     std::string resource_path);
    /**
     * \brief Triggers resource expired callback function
     *
     * \param[in] signal_name    Signal name
     * \param[in] object_path    To be match in registered signal list
     */
    void TriggerResourceExpiredSignal(std::string signal_name, std::string object_path);

    /**
     * \brief For unit test to trigger resource "expired" signal
     *
     * \param[in] resource_name  name of the resource that has expired
     */
    void SetResourceExpired(std::string resource_name);

    /**
     * \brief Mock implemention of "config_resource_call_get_payload_sync()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake    Pointer to object fake to get the payload
     * \param[in] encoding_path  Encoding path to get the payload
     * \param[in] source_path    Source path to get the payload
     * \param[out] out_payload   Payload for the given object
     */
    void GetResourcePayload(GDBusObjectFake *object_fake,
                            std::string encoding_path,
                            std::string source_path,
                            GVariant **out_payload);

    /**
     * \brief Mock implemention of "config_source_call_get_payload_sync()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake    Pointer to object fake to get the payload
     * \param[in] encoding_path  Encoding path to get the payload
     * \param[in] resource_path  Source path to get the payload
     * \param[out] out_payload   Payload for the given object
     */
    void GetSourcePayload(GDBusObjectFake *object_fake,
                          std::string encoding_path,
                          std::string resource_path,
                          GVariant **out_payload);

    /**
     * \brief Mock implemention of "config_resource_get_encodings()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake The object
     *
     * \return list of encodings
     */
    const char* const * GetResourceEncodings(GDBusObjectFake* object_fake);

    /**
     * \brief Mock implemention of "config_resource_get_sources()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake The object
     *
     * \return list of sources
     */
    const char* const * GetResourceSources(GDBusObjectFake* object_fake);

    /**
     * \brief Mock implemention of "config_source_call_get_encodings_sync()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake  Object fake to get encoding names
     * \param[in] resource_path resource path to get encoding names
     * \param[out] out_encoding_names list of encoding names for given resource
     *                                Caller must free the memory.
     *
     * \return True On success
     *         False On failed
     */
    bool GetSourceEncodings(GDBusObjectFake* object_fake,
                            const char* resource_path,
                            char*** out_encoding_names);

    /**
     * \brief Mock implemention of "config_source_call_has_resource_sync()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake The resource object fake
     * \param[in] resource_path Given resource path
     * \param[out] out_has_resource true if given resource has resource
     *                              false if given resource doesn't have resource
     * \return True On success
     *         False On failed
     */
    bool GetSourceHasResource(GDBusObjectFake* object_fake,
                              const char* resource_path,
                              int *out_has_resource);

    /**
     * \brief Mock implemention of "config_provisioned_resource_get_source()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake The object
     *
     * \return Provisioned resource's source
     */
    const char* GetProvisionedResourceSource(GDBusObjectFake* object_fake);

    /**
     * \brief Mock implemention of "config_provisioned_resource_get_encoding()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake The object
     *
     * \return Provisioned resource's encoding
     */
    const char* GetProvisionedResourceEncoding(GDBusObjectFake* object_fake);

    /**
     * \brief Mock implemention of "config_feature_get_state()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake The object
     *
     * \return Feature state
     */
    guint GetState(GDBusObjectFake* object_fake);

    /**
     * \brief Mock implemention of "config_feature_get_description()",
     *                             "config_feature_get_uri()" and
     *                             "config_feature_get_icon()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake The object
     * \param[in] index index in feature_properties_ tuple to identify
     *                  which property to be return.
     *
     * \return returns one of feature Properties: Description, URI, Icon.
     */
    const char* GetFeatureProperty(GDBusObjectFake* object_fake, const std::size_t index);

    /**
     * \brief Mock implemention of "config_feature_get_tags()"
     *        For parameter detail see original function
     *
     * \param[in] object_fake The object
     *
     * \return Feature tags
     */
    const char* const * GetTags(GDBusObjectFake* object_fake);

    /**
     * \brief Deallocate and clean given object.
     *        Trigger "object-remove", g-properties-changed signals
     *
     * \param[in] object_fake The object
     */
    void RemoveGDBusObject(std::string name);

    /**
     * \brief Remove all associate signals to the object path
     *
     * \param[in] object_path Object path
     */
    void RemoveSignal(std::string object_path);

    /**
     * \brief Deallocate and clean data
     *        To be called after all unit tests are executed
     */
    void DBusClean();

private:

    /**
     * \brief Create and trigger properties changed signal
     *
     * \param[in] base_name    base object name
     * \param[in] object_type  object type
     * \param[in] g_var        Contains the key strings (name of the object)
     *                         that are changed.
     *                         g_var will be freed after trigger the signal.
     */
    void CreatePropertiesChangedSignal(std::string base_name,
                                       BusObjectType object_type,
                                       GVariant* g_var);

    /**
     * \brief Create g_variant from uint8_t vector
     *
     * \param[in] payload vector of uint8_t
     *
     * \return GVariant
     */
    GVariant* ConvertByteToVariant(std::vector<std::uint8_t> payload);

    /**
     * \brief Convert vector of string to vector of const char*
     *
     * \param[in] strings vector of string
     * \param[in] chars vector of const char*
     */
    void ConvertStringsToChars(std::vector<std::string>& strings, std::vector<const char*>& chars);
};

} // namespace fsm

#endif // FSM_DBUS_INTERFACE_MOCK_H_INC_

/** \}    end of addtogroup */
