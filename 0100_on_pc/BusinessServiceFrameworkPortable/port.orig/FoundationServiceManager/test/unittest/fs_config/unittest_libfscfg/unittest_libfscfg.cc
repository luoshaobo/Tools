/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     unittest_libfscfg.ccc
 *  \brief    Foundation Services config Test Cases
 *  \author   Imran Siddique
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <unittest_libfscfg.h>

#include <gtest/gtest.h>


// Convert Payload from string to vector of uint8_t
std::vector<std::uint8_t> CovertStringToBytes(const std::string& content)
{
    return std::vector<std::uint8_t>(content.begin(), content.end());
}

// Convert Payload from vector of uint8_t to string
std::string ConvertBytesToString(const std::vector<std::uint8_t>& content)
{
    return std::string(content.begin(), content.end());
}

/**
 * ConfigInterface encoding get all
 * - Gets all default config encodings registered on dbus mock.
 * - Gets all config encodings from fs config interface
 * - Compare the encoding names
 *
 * TEST PASSES:
 *      fs config encoding name matches with registered encoding name on dbus mock
 *
 * TEST FAILS:
 *      fs config encoding name failed to match with registered encoding name on dbus mock
 */
TEST(FscfgTest, FscfgConfigEncodingGetAllTest)
{
    fscfg_ReturnCode rc;

    std::vector<std::shared_ptr<fsm::EncodingInterface>> encodings;
    std::vector<std::string> encoding_names;

    // Get all added encoding object through config interface
    rc = config_interface_->GetAll(encodings);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    for (std::shared_ptr<fsm::EncodingInterface>& encoding : encodings)
    {
        std::string name;
        rc = encoding->GetName(name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        encoding_names.push_back(name);
    }

    // Encoding names from dbus mock
    std::vector<std::string> expected_encoding_names = dbus_interface_->encoding_objects_;

    // sort contents so, vectors are comparable
    std::sort(expected_encoding_names.begin(), expected_encoding_names.end());
    std::sort(encoding_names.begin(), encoding_names.end());

    // Compare the names from dbus mock and from config interface
    EXPECT_TRUE(expected_encoding_names == encoding_names);
}

/**
 * ConfigInterface encoding bind and get
 * - Bind a callback function for encoding changed
 * - Create a new encoding on dbus mock
 * - Verify that callback function is called
 * - Get encoding object newly added
 * - Compare the encoding names
 *
 * TEST PASSES:
 *      Gets the callback function and newly add encoding matches with get fs config encoding
 *
 * TEST FAILS:
 *      Fail to get callback,
 *      Encoding name doesn't match
 */
TEST(FscfgTest, FscfgConfigEncodingBindAndGetTest)
{
    fscfg_ReturnCode rc;

    // bind encoding changed callback function
    is_encodings_changed_cb_ = false;
    std::uint32_t encoding_bind_id;

    rc = config_interface_->BindEncodingsChanged(Config_EncodingsChangedCb, encoding_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Create new encoding object on dbus
    std::string add_encoding_name = "TestEnc";
    dbus_interface_->CreateGDBusObject({add_encoding_name}, fsm::fscfg_kBotConfigEncoding);

    // Check if gets the callback
    EXPECT_TRUE(is_encodings_changed_cb_);

    // Get encoding object of the added encoding
    std::shared_ptr<fsm::EncodingInterface> encoding;
    rc = config_interface_->Get(add_encoding_name, encoding);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Get name of the added ecoding object
    std::string encoding_name;
    rc = encoding->GetName(encoding_name);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Compare added encoding name and config ecoding object name
    EXPECT_STREQ(add_encoding_name.c_str(), encoding_name.c_str());

    // Unbind callback
    rc = config_interface_->Unbind(encoding_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_encodings_changed_cb_ = false;

    // Remove added object
    dbus_interface_->RemoveGDBusObject(add_encoding_name);
}

/**
 * ConfigInterface source get all
 * - Gets all default config sources registered on dbus mock.
 * - Gets all config sources from fs config interface
 * - Compare the source names
 *
 * TEST PASSES:
 *      fs config source name matches with registered source name on dbus mock
 *
 * TEST FAILS:
 *      fs config source name failed to match with registered source name on dbus mock
 */
TEST(FscfgTest, FscfgConfigSourceGetAllTest)
{
    fscfg_ReturnCode rc;

    std::vector<std::shared_ptr<fsm::SourceInterface>> sources;
    std::vector<std::string> source_names;

    // Get all added source objects through config interface
    rc = config_interface_->GetAll(sources);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    for (std::shared_ptr<fsm::SourceInterface>& source : sources)
    {
        std::string name;
        rc = source->GetName(name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        source_names.push_back(name);
    }

    // Encoding names from dbus mock
    std::vector<std::string> expected_source_names = dbus_interface_->source_objects_;

    // sort contents so, vectors are comparable
    std::sort(expected_source_names.begin(), expected_source_names.end());
    std::sort(source_names.begin(), source_names.end());

    // Compare dbus and config interface source names.
    EXPECT_TRUE(expected_source_names == source_names);
}

/**
 * ConfigInterface source bind and get
 * - Bind a callback function for source changed
 * - Create a new source on dbus mock
 * - Verify that callback function is called
 * - Get source object newly added
 * - Compare the source names
 *
 * TEST PASSES:
 *      Gets the callback function and newly add source matches with the get fs config source
 *
 * TEST FAILS:
 *      Fail to get callback,
 *      Source name doesn't match
 */
TEST(FscfgTest, FscfgConfigSourceBindAndGetTest)
{
    fscfg_ReturnCode rc;

    // bind source changed callback function
    is_sources_changed_cb_ = false;
    std::uint32_t src_bind_id;

    rc = config_interface_->BindSourcesChanged(Config_SourcesChangedCb, src_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Add new source object to the Dbus
    std::string add_source_name = "TestSource";
    dbus_interface_->CreateGDBusObject({add_source_name}, fsm::fscfg_kBotConfigSource);

    // Check if gets the callback
    EXPECT_TRUE(is_sources_changed_cb_);

    // Get source object of the added source
    std::shared_ptr<fsm::SourceInterface> source;
    rc = config_interface_->Get(add_source_name, source);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Get name of the added source object
    std::string source_name;
    rc = source->GetName(source_name);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Compare added source name and added source object name
    EXPECT_STREQ(add_source_name.c_str(), source_name.c_str());

    // Unbind callback
    rc = config_interface_->Unbind(src_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_sources_changed_cb_ = false;

    // Remove added object
    dbus_interface_->RemoveGDBusObject(add_source_name);
}

/**
 * ConfigInterface resource get all
 * - Gets all default config resources registered on dbus mock.
 * - Gets all config resources from fs config interface
 * - Compare the resource names
 *
 * TEST PASSES:
 *      fs config resource name matches with registered resource name on dbus mock
 *
 * TEST FAILS:
 *      fs config resource name failed to match with registered resource name on dbus mock
 */
TEST(FscfgTest, FscfgConfigResourceGetAllTest)
{
    fscfg_ReturnCode rc;

    std::vector<std::shared_ptr<fsm::ResourceInterface>> resources;
    std::vector<std::string> resource_names;

    // Get all added resource objects through config interface
    rc = config_interface_->GetAll(resources);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    for (std::shared_ptr<fsm::ResourceInterface>& resource : resources)
    {
        std::string name;
        rc = resource->GetName(name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        resource_names.push_back(name);
    }

    // All expected resources on dbus mock (resources + provisioned resources + features)
    std::vector<std::string> all_resource_names = dbus_interface_->resource_objects_;

    all_resource_names.insert(all_resource_names.end(),
                              dbus_interface_->prov_resource_objects_.begin(),
                              dbus_interface_->prov_resource_objects_.end());

    all_resource_names.insert(all_resource_names.end(),
                              dbus_interface_->feature_objects_.begin(),
                              dbus_interface_->feature_objects_.end());

    // sort contents so, vectors are comparable
    std::sort(all_resource_names.begin(), all_resource_names.end());
    std::sort(resource_names.begin(), resource_names.end());

    // Compare dbus and config interface resource names.
    EXPECT_TRUE(all_resource_names == resource_names);
}

/**
 * ConfigInterface resource bind and get
 * - Bind a callback function for resource changed
 * - Create a new resource on dbus mock
 * - Verify that callback function is called
 * - Get resource object newly added
 * - Compare the resource names
 *
 * TEST PASSES:
 *      Gets the callback function and newly add resource matches with the get fs config resource
 *
 * TEST FAILS:
 *      Fail to get callback,
 *      Resource name doesn't match
 */
TEST(FscfgTest, FscfgConfigResourceBindAndGetTest)
{
    fscfg_ReturnCode rc;

    // bind a callback function
    is_resources_changed_cb_ = false;
    std::uint32_t res_bind_id;

    rc = config_interface_->BindResourcesChanged(Config_ResourcesChangedCb, res_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Add new resource object to the Dbus
    std::string add_resource_name = "TestResource";
    dbus_interface_->CreateGDBusObject({add_resource_name}, fsm::fscfg_kBotConfigResource);

    // Check if gets the callback
    EXPECT_TRUE(is_resources_changed_cb_);

    // Get resource object of the added resource
    std::shared_ptr<fsm::ResourceInterface> resource;
    rc = config_interface_->Get(add_resource_name, resource);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Get name of the added Resource object
    std::string resource_name;
    rc = resource->GetName(resource_name);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Compare added resource name and added resource object name
    EXPECT_STREQ(add_resource_name.c_str(), resource_name.c_str());

    // Unbind callback
    rc = config_interface_->Unbind(res_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_resources_changed_cb_ = false;

    // Remove added object
    dbus_interface_->RemoveGDBusObject(add_resource_name);

}

/**
 * Resource expired notification
 * - Create a resource on dbus mock
 * - Gets the resource object from fs config interface
 * - Bind a callback
 * - Trigger a resource expired signal from dbus mock
 * - Verify if gets resource expired callback
 *
 * TEST PASSES:
 *      Gets the resource expired callback
 *
 * TEST FAILS:
 *      Failed to get the resource expired callback
 */
TEST(FscfgTest, FscfgResourceExpiredTestTest)
{
    fscfg_ReturnCode rc;

    std::vector<std::shared_ptr<fsm::ResourceInterface>> resources;
    std::shared_ptr<fsm::ResourceInterface> resource;

    std::string add_resource_name = {"ResExp"};
    std::vector<std::string> resource_names;

    // Create dbus resource objects
    dbus_interface_->CreateGDBusObject({add_resource_name}, fsm::fscfg_kBotConfigResource);

    // Get resource second object
    rc = config_interface_->Get(add_resource_name, resource);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Bind expiry notification
    is_expired_dbus_cb_ = false;
    std::uint32_t exp_bind_id;

    rc = resource->BindExpired(Resource_ExpiredDbusCb, exp_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Trigger expired signal
    dbus_interface_->SetResourceExpired(add_resource_name);

    // Check if gets the callback
    EXPECT_TRUE(is_expired_dbus_cb_);

    rc = resource->Unbind(exp_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_expired_dbus_cb_ = false;

    // Remove added object
    dbus_interface_->RemoveGDBusObject(add_resource_name);
}

/**
 * Resource payload usecase
 * Add payload triggers PayloadChanged, Updated, SourcesChanged signals on
 * resource and the calls the Get functions to verify that contents are matched
 *
 * - Resource Binds (PayloadChanged, Updated, SourcesChanged)
 * - Get payload and verify the content matches with the one created payload
 * - Test GetEncodings and GetSources
 *
 * TEST PASSES:
 *      - Get callbacks for PayloadChanged, Updated, SourcesChanged
 *      - Resource GetPayload content matches with the created payload
 *      - GetEncodings and GetSources for the specific resource matches
 *
 * TEST FAILS:
 *      - Failed to get any of the callbacks PayloadChanged, Updated, SourcesChanged
 *      - Payload doesn't match
 *      - GetEncodings and GetSources names doesn't matched
 *
 */
TEST(FscfgTest, FscfgResourcePayloadBindAndGetsTest)
{
    fscfg_ReturnCode rc;

    // Get resource object from one of the default resources
    std::shared_ptr<fsm::ResourceInterface> resource;
    std::string dbus_resource_name = dbus_interface_->resource_objects_[0];
    rc = config_interface_->Get(dbus_resource_name, resource);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Create payload on dbus with given source, encoding, resource.
    std::string dbus_source_name = dbus_interface_->source_objects_[0];
    std::string dbus_encoding_name = dbus_interface_->encoding_objects_[0];

    // Payload bind usecases
    {
        // Bind Payload changed
        is_payloads_changed_cb_ = false;
        std::uint32_t payload_bind_id;
        rc = resource->BindPayloadsChanged(Resource_PayloadsChangedCb, payload_bind_id);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Bind Resource update
        is_updated_cb_ = false;
        std::uint32_t updated_bind_id;
        rc = resource->BindUpdated(Resource_UpdatedCb, updated_bind_id);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Bind Resource sources changed
        is_resource_sources_changed_cb_ = false;
        std::uint32_t sources_changed_bind_id;
        rc = resource->BindSourcesChanged(Resource_SourcesChangedCb, sources_changed_bind_id);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        std::string add_payload(kTestPayloadResource);

        // Publish payload on dbus
        dbus_interface_->CreatePayload(dbus_source_name,
                                       dbus_encoding_name,
                                       dbus_resource_name,
                                       fsm::fscfg_kBotConfigResource,
                                       CovertStringToBytes(add_payload));

        // Check if gets all expected the callbacks
        EXPECT_TRUE(is_payloads_changed_cb_);
        EXPECT_TRUE(is_updated_cb_);
        EXPECT_TRUE(is_resource_sources_changed_cb_);

        rc = resource->Unbind(payload_bind_id);
        EXPECT_EQ(rc, fscfg_kRcSuccess);
        is_payloads_changed_cb_ = false;

        rc = resource->Unbind(updated_bind_id);
        EXPECT_EQ(rc, fscfg_kRcSuccess);
        is_updated_cb_ = false;

        rc = resource->Unbind(sources_changed_bind_id);
        EXPECT_EQ(rc, fscfg_kRcSuccess);
        is_resource_sources_changed_cb_ = false;

        // Resource GetPayload

        // Get Payload to compare what was created on bus
        std::shared_ptr<fsm::SourceInterface> source;
        std::shared_ptr<fsm::EncodingInterface> encoding;

        rc = config_interface_->Get(dbus_source_name, source);
        EXPECT_EQ(rc, fscfg_kRcSuccess);

        rc = config_interface_->Get(dbus_encoding_name, encoding);
        EXPECT_EQ(rc, fscfg_kRcSuccess);

        std::vector<std::uint8_t> expected_payload;
        rc = resource->GetPayload(encoding, source, expected_payload);
        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Compare added payload on dbus and getted through fs config resource
        EXPECT_STREQ(add_payload.c_str(), (ConvertBytesToString(expected_payload)).c_str());
    }

    // Resource GetEncodings
    {
        // Create another payload on dbus with different encoding
        // for the same resource and source
        std::vector<std::string> expected_encoding_names = {dbus_encoding_name,
                                                            dbus_interface_->encoding_objects_[1]};

        std::string add_payload(kTestPayloadEncoding);

        // Create payload on dbus
        dbus_interface_->CreatePayload(dbus_source_name,
                                       expected_encoding_names[1],
                                       dbus_resource_name,
                                       fsm::fscfg_kBotConfigResource,
                                       CovertStringToBytes(add_payload));

        // Get all encodings for the resource
        std::vector<std::shared_ptr<fsm::EncodingInterface>> encodings;
        rc = resource->GetEncodings(encodings);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        std::vector<std::string> encoding_names;

        for (std::shared_ptr<fsm::EncodingInterface>& encoding : encodings)
        {
            std::string name;
            rc = encoding->GetName(name);

            EXPECT_EQ(rc, fscfg_kRcSuccess);

            encoding_names.push_back(name);
        }

        // Sort the result is in order
        std::sort(expected_encoding_names.begin(), expected_encoding_names.end());
        std::sort(encoding_names.begin(), encoding_names.end());

        // Compare dbus encoding names and encoding names from config interface
        EXPECT_TRUE(expected_encoding_names == encoding_names);
    }

    // Resource GetSources
    {
        // Create another payload on dbus with different encoding
        // for the same resource and source
        // This resource have two same source with different encodings
        std::vector<std::string> expected_source_names = {dbus_source_name,
                                                          dbus_source_name,
                                                          dbus_interface_->source_objects_[1]};

        std::string add_payload(kTestPayloadSource);

        // Publish payload on dbus
        dbus_interface_->CreatePayload(expected_source_names[2],
                                       dbus_encoding_name,
                                       dbus_resource_name,
                                       fsm::fscfg_kBotConfigResource,
                                       CovertStringToBytes(add_payload));

        std::vector<std::shared_ptr<fsm::SourceInterface>> sources;
        rc = resource->GetSources(sources);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        std::vector<std::string> source_names;

        for (std::shared_ptr<fsm::SourceInterface>& source: sources)
        {
            std::string name;
            rc = source->GetName(name);

            EXPECT_EQ(rc, fscfg_kRcSuccess);

            source_names.push_back(name);
        }

        // Sort so, the result is in order
        std::sort(expected_source_names.begin(), expected_source_names.end());
        std::sort(source_names.begin(), source_names.end());

        // Compare dbus source names and source names from config interface
        EXPECT_TRUE(expected_source_names == source_names);
    }
}

/**
 * Fs config source getters GetEncodings, GetName and HasResource
 *
 * TEST PASSES:
 *      - Source name matches with the dbus source name
 *      - Source encoding for specific source matches the encoding names from dbus mock
 *      - The source has resource for specified resource object
 *
 * TEST FAILS:
 *      - Source names doesn't match
 *      - Source encodings doesn't match
 *      - Source has no resource
 */
TEST(FscfgTest, FscfgSourceGetters)
{
    fscfg_ReturnCode rc;

    // Get source object
    std::string dbus_source_name = dbus_interface_->source_objects_[0];
    std::shared_ptr<fsm::SourceInterface> source;

    rc = config_interface_->Get(dbus_source_name, source);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Check source name
    {
        std::string name;
        rc = source->GetName(name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        EXPECT_STREQ(dbus_source_name.c_str(), name.c_str());
    }

    // Get resource object
    std::shared_ptr<fsm::ResourceInterface> resource;

    // Resource1 are expected to have two sources with different encodings
    std::string dbus_resource_name = dbus_interface_->resource_objects_[0];

    rc = config_interface_->Get(dbus_resource_name, resource);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Get source encodings for the specific resource
    {
        std::vector<std::shared_ptr<fsm::EncodingInterface>> encodings;

        rc = source->GetEncodings(resource, encodings);
        EXPECT_EQ(rc, fscfg_kRcSuccess);

        std::vector<std::string> encoding_names;

        for (std::shared_ptr<fsm::EncodingInterface>& encoding : encodings)
        {
            std::string name;
            rc = encoding->GetName(name);

            EXPECT_EQ(rc, fscfg_kRcSuccess);

            encoding_names.push_back(name);
        }

        // Construct expected the source encoding names
        std::vector<std::string> dbus_encoding_names = {dbus_interface_->encoding_objects_[0],
                                                        dbus_interface_->encoding_objects_[1]};

        // Sort the the result is in order
        std::sort(dbus_encoding_names.begin(), dbus_encoding_names.end());
        std::sort(encoding_names.begin(), encoding_names.end());

        // Compare dbus encoding names and encoding names from config interface
        EXPECT_TRUE(dbus_encoding_names == encoding_names);
    }

    // Source has resource
    {
        bool has_resource;
        rc = source->HasResource(resource, has_resource);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Expected source has resource
        EXPECT_TRUE(has_resource);
    }
}

/**
 * Fs config source get payload for specific resource and encoding
 *
 * TEST PASSES:
 *      - Successfully get the payload and compare the contents
 *
 * TEST FAILS:
 *      - Fail to get payload or contents doesn't match
 */
TEST(FscfgTest, FscfgSourceGetPayloadTest)
{
    fscfg_ReturnCode rc;

    // Get source object
    std::string dbus_source_name = dbus_interface_->source_objects_[0];
    std::shared_ptr<fsm::SourceInterface> source;

    rc = config_interface_->Get(dbus_source_name, source);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Get encoding object
    std::shared_ptr<fsm::EncodingInterface> encoding;
    std::string dbus_encoding_name = dbus_interface_->encoding_objects_[0];

    rc = config_interface_->Get(dbus_encoding_name, encoding);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Get resource object
    std::shared_ptr<fsm::ResourceInterface> resource;
    std::string dbus_resource_name = dbus_interface_->resource_objects_[0];

    rc = config_interface_->Get(dbus_resource_name, resource);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Get source payload for specific resource and encoding
    std::vector<std::uint8_t> expected_payload;
    rc = source->GetPayload(resource, encoding, expected_payload);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    std::string payload(kTestPayloadResource);

    // Compare added payload on bus and getted payload through proxy
    EXPECT_STREQ(payload.c_str(), (ConvertBytesToString(expected_payload)).c_str());
}

/**
 * Provisioning get all
 * - Gets all default Provisioned resource registered on dbus mock.
 * - Gets all provisioned resource from fs config Provisioning interface
 * - Compare the provisioned resource names
 *
 * TEST PASSES:
 *      fs config provisioned resource name matches with
 *      registered provisioned resource name on dbus mock
 *
 * TEST FAILS:
 *      fs config provisioned resource name failed to match with registered provisined resource name on dbus mock
 */
TEST(FscfgTest, FscfgProvisioningGetAllTest)
{
    fscfg_ReturnCode rc;

    std::vector<std::shared_ptr<fsm::ProvisionedResourceInterface>> prov_resources;
    std::vector<std::string> prov_resource_names;

    // Get all added resource objects through config interface
    rc = provisioning_interface_->GetAll(prov_resources);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    for (std::shared_ptr<fsm::ProvisionedResourceInterface>& prov_resource : prov_resources)
    {
        std::string name;
        rc = prov_resource->GetName(name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        prov_resource_names.push_back(name);
    }

    // All expected provisioned resources on dbus (provisioned resources + features)
    std::vector<std::string> all_prov_resource_names = dbus_interface_->prov_resource_objects_;

    all_prov_resource_names.insert(all_prov_resource_names.end(),
                                   dbus_interface_->feature_objects_.begin(),
                                   dbus_interface_->feature_objects_.end());

    std::sort(all_prov_resource_names.begin(), all_prov_resource_names.end());
    std::sort(prov_resource_names.begin(), prov_resource_names.end());

    // Compare dbus and config interface resource names.
    EXPECT_TRUE(all_prov_resource_names == prov_resource_names);
}

/**
 * Provisioning resource changed bind and get by name
 * - Bind a callback function for resource changed
 * - Create a new resource on dbus mock
 * - Verify that callback function is called
 * - Get provisioned resource object newly added
 * - Compare the provisioned resource names
 *
 * TEST PASSES:
 *      Gets the callback function and newly add provisioned resource
 *      matches with get through fs config provisioned resource interface
 *
 * TEST FAILS:
 *      Fail to get callback,
 *      provisioned resource name doesn't match
 */
TEST(FscfgTest, FscfgProvisioningBindAndGetTest)
{
    fscfg_ReturnCode rc;

    // Add new resource object to the Dbus
    std::string add_prov_resource = "TestProvRes";

    // bind resource changed
    {
        // bind a callback function
        is_provResources_changed_cb_ = false;
        std::uint32_t prov_res_bind_id;

        rc = provisioning_interface_->BindResourcesChanged(Provisioning_ResourcesChangedCb, prov_res_bind_id);
        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Create dbus resource objects
        dbus_interface_->CreateGDBusObject({add_prov_resource}, fsm::fscfg_kBotConfigResource);

        // Make it provisioned
        dbus_interface_->CreateGDBusObject({add_prov_resource}, fsm::fscfg_kBotProvisioningResource);

        // Check if gets the callback
        EXPECT_TRUE(is_provResources_changed_cb_);
        is_provResources_changed_cb_ = false;

        // Unbind callback
        rc = provisioning_interface_->Unbind(prov_res_bind_id);
        EXPECT_EQ(rc, fscfg_kRcSuccess);
        is_provResources_changed_cb_ = false;
    }

    // Get add provisioned resource and compare
    {
        // Get resource object of the added resource
        std::shared_ptr<fsm::ProvisionedResourceInterface> prov_resource;

        rc = provisioning_interface_->Get(add_prov_resource, prov_resource);
        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Get name of the added Resource object
        std::string prov_resource_name;
        rc = prov_resource->GetName(prov_resource_name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Compare added resource name and added resource object name
        EXPECT_STREQ(add_prov_resource.c_str(), prov_resource_name.c_str());
    }
    // Remove added object
    dbus_interface_->RemoveGDBusObject(add_prov_resource);
}

/**
 * Provisioned resource payload usecase
 * Create payload triggers PayloadChanged, SourcesChanged signals on
 * fs config provisioned resource and calls to Get functions to verify that contents are matched.
 *
 * - Provisioned resource Binds (PayloadChanged, SourcesChanged)
 * - Get payload and verify the content matches with the one created payload
 * - Test GetEncodings and GetSources
 *
 * TEST PASSES:
 *      - Get callbacks for PayloadChanged, Updated, SourcesChanged
 *      - Resource GetPayload content matches with the created payload
 *      - GetEncoding and GetSource matches names get from config source and config encoding
 *
 * TEST FAILS:
 *      - Failed to get any of the callbacks PayloadChanged, SourcesChanged
 *      - Payload doesn't match
 *      - GetEncodings and GetSources names doesn't matched
 *
 */
TEST(FscfgTest, FscfgProvisionedResourceBindsAndGettersTest)
{
    fscfg_ReturnCode rc;

    // Names on the dbus mock
    std::string dbus_source_name = dbus_interface_->source_objects_[2];
    std::string dbus_encoding_name = dbus_interface_->encoding_objects_[2];
    std::string dbus_prov_resource_name = dbus_interface_->prov_resource_objects_[0];

    // Get provisioned resource object
    std::shared_ptr<fsm::ProvisionedResourceInterface> prov_resource;
    rc = provisioning_interface_->Get(dbus_prov_resource_name, prov_resource);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Provisioned Resource binds Payload bind

    // Bind Payload changed
    is_prov_payload_changed_cb_ = false;
    std::uint32_t prov_payload_bind_id;
    rc = prov_resource->BindPayloadChanged(ProvisionedResource_PayloadChangedCb, prov_payload_bind_id);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Bind Payload changed
    is_prov_source_changed_cb_ = false;
    std::uint32_t prov_source_bind_id;
    rc = prov_resource->BindSourceChanged(ProvisionedResource_SourceChangedCb, prov_source_bind_id);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    std::string add_payload(kTestPayloadProv);

    // Publish payload on dbus
    dbus_interface_->CreatePayload(dbus_source_name,
                                   dbus_encoding_name,
                                   dbus_prov_resource_name,
                                   fsm::fscfg_kBotProvisioningResource,
                                   CovertStringToBytes(add_payload));

    // Check if gets the callback
    EXPECT_TRUE(is_prov_payload_changed_cb_);
    EXPECT_TRUE(is_prov_source_changed_cb_);

    // Unbind callback
    rc = prov_resource->Unbind(prov_payload_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_prov_payload_changed_cb_ = false;

    // Unbind callback
    rc = prov_resource->Unbind(prov_source_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_prov_source_changed_cb_ = false;

    // Provisioned resource get payload
    {
        std::vector<std::uint8_t> payload;
        rc = prov_resource->GetPayload(payload);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Compare added payload on bus and getted payload through proxy
        EXPECT_STREQ(add_payload.c_str(), (ConvertBytesToString(payload)).c_str());
    }

    // Provisioned resource get encoding
    {
        std::shared_ptr<fsm::EncodingInterface> encoding;

        // Get encoding for the given provisioned resource
        // Can provisionedresource could have multiple encodings? due to multiple payloads?
        rc = prov_resource->GetEncoding(encoding);
        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Get config encoding name
        std::string encoding_name;
        rc = encoding->GetName(encoding_name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Compare added encoding name and config encoding object name
        EXPECT_STREQ(dbus_encoding_name.c_str(), encoding_name.c_str());
    }


    // Provisioned resource get source
    {
        std::shared_ptr<fsm::SourceInterface> source;

        // Get source for the given provisioned resource
        rc = prov_resource->GetSource(source);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Get source encoding name
        std::string source_name;
        rc = source->GetName(source_name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Compare added source name and config source object name
        EXPECT_STREQ(dbus_source_name.c_str(), source_name.c_str());
    }
}

/**
 * Provisioning get functions (GetEncoding, GetSource)
 *
 * TEST PASSES:
 *      - GetEncoding name matches with dbus mock encoding name
 *      - GetSource name matches with dbus mock source name
 *
 * TEST FAILS:
 *      - Fail to get encoding or name doesn't match
 *      - Fail to get encoding or name doesn't match
 */
TEST(FscfgTest, FscfgProvisioningGettersTest)
{
    fscfg_ReturnCode rc;

    // Names on the dbus
    std::string dbus_source_name = dbus_interface_->source_objects_[2];
    std::string dbus_encoding_name = dbus_interface_->encoding_objects_[2];
    std::string dbus_prov_resource_name = dbus_interface_->prov_resource_objects_[0];

    // Get provisioned resource object
    std::shared_ptr<fsm::ProvisionedResourceInterface> prov_resource;
    rc = provisioning_interface_->Get(dbus_prov_resource_name, prov_resource);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Provisioning Get encoding
    {
        std::shared_ptr<fsm::EncodingInterface> encoding;

        // Get encoding for the given provisioned resource
        // Can provisionedresource could have multiple encodings? due to multiple payloads?
        rc = provisioning_interface_->GetEncoding(prov_resource, encoding);
        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Get config encoding name
        std::string encoding_name;
        rc = encoding->GetName(encoding_name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Compare added encoding name and config encoding object name
        EXPECT_STREQ(dbus_encoding_name.c_str(), encoding_name.c_str());
    }


    // Provisioning Get source
    {
        std::shared_ptr<fsm::SourceInterface> source;

        // Get source for the given provisioned resource
        rc = provisioning_interface_->GetSource(prov_resource, source);
        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Get source encoding name
        std::string source_name;
        rc = source->GetName(source_name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        // Compare added source name and config source object name
        EXPECT_STREQ(dbus_source_name.c_str(), source_name.c_str());
    }
}

/**
 * Discovery features get all
 * - Gets all default features registered on dbus mock.
 * - Gets all feature from discovery interface
 * - Compare the feature names
 *
 * TEST PASSES:
 *      Feature names matche with registered feature names on dbus mock
 *
 * TEST FAILS:
 *      Feature names doesn't match
 */
TEST(FscfgTest, FscfgDiscoveryGetAllTest)
{
    fscfg_ReturnCode rc;

    std::vector<std::shared_ptr<fsm::FeatureInterface>> features;
    std::vector<std::string> feature_names;

    // Get all added encoding object through config interface
    rc = discovery_interface_->GetAll(features);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    for (std::shared_ptr<fsm::FeatureInterface>& feature : features)
    {
        std::string name;
        rc = feature->GetName(name);

        EXPECT_EQ(rc, fscfg_kRcSuccess);

        feature_names.push_back(name);
    }

    std::vector<std::string> expected_feature_names = dbus_interface_->feature_objects_;

    // sort so, vectors are comparable
    std::sort(expected_feature_names.begin(), expected_feature_names.end());
    std::sort(feature_names.begin(), feature_names.end());

    // Compare dbus encoding names and encoding names from config interface
    EXPECT_TRUE(expected_feature_names == feature_names);
}

/**
 * Discovery resource bind and get feature
 * - Bind a callback function for resource changed
 * - Create a new feature on dbus mock
 * - Verify that callback function is called
 * - Get feature object newly added
 * - Compare the feature names
 *
 * TEST PASSES:
 *      Gets the callback function and newly add feature matches with the get fs config feature
 *
 * TEST FAILS:
 *      Fail to get callback,
 *      Feature name doesn't match
 */
TEST(FscfgTest, FscfgDiscoveryBindAndGetTest)
{
    fscfg_ReturnCode rc;

    // bind discovery resource changed callback function
    is_discovery_resources_changed_cb_ = false;
    std::uint32_t discovery_bind_id;

    rc = discovery_interface_->BindResourcesChanged(Discovery_ResourcesChangedCb, discovery_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    std::string add_feature_name = "TestFeature";

    // Register feature resource in config resource
    dbus_interface_->CreateGDBusObject({add_feature_name}, fsm::fscfg_kBotConfigResource);

    // Register feature resource in provisioned resource
    dbus_interface_->CreateGDBusObject({add_feature_name}, fsm::fscfg_kBotProvisioningResource);

    // Register feature resource in discovery feature
    dbus_interface_->CreateGDBusObject({add_feature_name}, fsm::fscfg_kBotDiscoveryFeature);

    // Check if gets the callback
    EXPECT_TRUE(is_discovery_resources_changed_cb_);

    // Get encoding object of the added encoding
    std::shared_ptr<fsm::FeatureInterface> feature;
    rc = discovery_interface_->Get(add_feature_name, feature);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Get name of the added feature object
    std::string feature_name;
    rc = feature->GetName(feature_name);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Compare added feature name and discovery object name
    EXPECT_STREQ(add_feature_name.c_str(), feature_name.c_str());

    // Unbind callback
    rc = discovery_interface_->Unbind(discovery_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_discovery_resources_changed_cb_ = false;

    // Remove added object
    dbus_interface_->RemoveGDBusObject(add_feature_name);
}

/**
 * Discovery retrieve cloud connection status details.
 * - Retrieve the dummy values via the Discovery interface and check all parameters for equality against the known,
 *   previously-set hardcoded values.
 */
TEST(FscfgTest, FscfgDiscoveryGetCloudConnectionStatus)
{
    // Check the default, currently hardcoded values.
    // The retrieved values shall match the dummy values we just set. When a full implementation
   // on the cloud connection status is available, this test needs to be adapted.
    char expected_certificate_validity_status = 0;

    char expected_central_connection_status = 0;
    std::string expected_central_server_uri = "http://192.168.1.113:8088/CN";
    std::string expected_central_date_time = "2017-05-31 13:50:00";

    char expected_regional_connection_status = 0;
    std::string expected_regional_server_uri = "http://192.168.1.113:8088/RN";
    std::string expected_regional_date_time = "2017-05-31 13:50:00";

    GVariant* connection_status = g_variant_new("(yyssyss)",
                                                expected_certificate_validity_status,
                                                expected_central_connection_status,
                                                expected_central_server_uri.c_str(),
                                                expected_central_date_time.c_str(),
                                                expected_regional_connection_status,
                                                expected_regional_server_uri.c_str(),
                                                expected_regional_date_time.c_str());

    discovery_set_cloud_connection_status(nullptr, connection_status);

    // Values retrieved from the bus via the discovery client-side interface.
    char bus_certificate_validity_status;

    char bus_central_connection_status;
    std::string bus_central_server_uri;
    std::string bus_central_date_time;

    char bus_regional_connection_status;
    std::string bus_regional_server_uri;
    std::string bus_regional_date_time;

    ASSERT_TRUE(discovery_interface_);

    fscfg_ReturnCode rc = discovery_interface_->GetCloudConnectionStatus(bus_certificate_validity_status,
                                                                         bus_central_connection_status,
                                                                         bus_central_server_uri,
                                                                         bus_central_date_time,
                                                                         bus_regional_connection_status,
                                                                         bus_regional_server_uri,
                                                                         bus_regional_date_time);

    // Check all retrieved values against their know hardcoded counterparts.
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    EXPECT_EQ(expected_certificate_validity_status, bus_certificate_validity_status);

    EXPECT_EQ(expected_central_connection_status, bus_central_connection_status);
    EXPECT_EQ(expected_central_server_uri, bus_central_server_uri);
    EXPECT_EQ(expected_central_date_time, bus_central_date_time);

    EXPECT_EQ(expected_regional_connection_status, bus_regional_connection_status);
    EXPECT_EQ(expected_regional_server_uri, bus_regional_server_uri);
    EXPECT_EQ(expected_regional_date_time, bus_regional_date_time);
}

/**
 * Feature properties binds (StateChange, DescriptionChanged, UriChanged, IconChanged, TagsChanged)
 * FeatureInterface Get functions are covered in callback functions.
 * FeatureProxy gets properties update signal which basically calls get feature property and calls
 * then binded funcation.
 *
 * TEST PASSES:
 *      - Gets all callbacks
 *
 * TEST FAILS:
 *      - Failed to get any of the callback
 */
TEST(FscfgTest, FscfgFeaturePropertiesBindTest)
{
    fscfg_ReturnCode rc;

    // Get encoding object of the added encoding
    std::shared_ptr<fsm::FeatureInterface> feature;
    std::string feature_name = dbus_interface_->feature_objects_[0];

    rc = discovery_interface_->Get(feature_name, feature);
    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // bind state changed callback function
    is_state_changed_cb_ = false;
    std::uint32_t state_bind_id;
    rc = feature->BindStateChanged(Feature_StateChangedCb, state_bind_id);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // bind description changed callback function
    is_description_changed_cb_ = false;
    std::uint32_t description_bind_id;
    rc = feature->BindDescriptionChanged(Feature_DescriptionChangedCb, description_bind_id);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // bind uri changed callback function
    is_uri_changed_cb_ = false;
    std::uint32_t uri_bind_id;
    rc = feature->BindUriChanged(Feature_UriChangedCb, uri_bind_id);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // bind icon changed callback function
    is_icon_changed_cb_ = false;
    std::uint32_t icon_bind_id;
    rc = feature->BindIconChanged(Feature_IconChangedCb, icon_bind_id);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // bind tags changed callback function
    is_tags_changed_cb_ = false;
    std::uint32_t tags_bind_id;
    rc = feature->BindTagsChanged(Feature_TagsChangedCb, tags_bind_id);

    EXPECT_EQ(rc, fscfg_kRcSuccess);

    // Create feature properties on dbus and trigger "properties-changed" signal
    dbus_interface_->CreateFeatureProperties(feature_name);

    // Expected callbacks
    EXPECT_TRUE(is_state_changed_cb_);
    EXPECT_TRUE(is_description_changed_cb_);
    EXPECT_TRUE(is_uri_changed_cb_);
    EXPECT_TRUE(is_icon_changed_cb_);
    EXPECT_TRUE(is_tags_changed_cb_);

    // Unbinds
    rc = feature->Unbind(state_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_state_changed_cb_ = false;

    rc = feature->Unbind(description_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_description_changed_cb_ = false;

    rc = feature->Unbind(uri_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_uri_changed_cb_ = false;

    rc = feature->Unbind(icon_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_icon_changed_cb_ = false;

    rc = feature->Unbind(tags_bind_id);
    EXPECT_EQ(rc, fscfg_kRcSuccess);
    is_tags_changed_cb_ = false;
}

fscfg_ReturnCode Config_EncodingsChangedCb(fsm::ConfigInterface::EncodingsChangedEvent param)
{
    is_encodings_changed_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Config_SourcesChangedCb(fsm::ConfigInterface::SourcesChangedEvent param)
{
    is_sources_changed_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Config_ResourcesChangedCb(fsm::ConfigInterface::ResourcesChangedEvent param)
{
    is_resources_changed_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Resource_ExpiredDbusCb(fsm::ResourceInterface::ExpiredEvent param)
{
    // Check if immediate flag correctly set
    EXPECT_TRUE(param.immediate);

    is_expired_dbus_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Resource_PayloadsChangedCb(fsm::ResourceInterface::PayloadsChangedEvent param)
{
    // TODO: Verify the param ?
    is_payloads_changed_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Resource_UpdatedCb(fsm::ResourceInterface::UpdatedEvent param)
{
    is_updated_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Resource_SourcesChangedCb(fsm::ResourceInterface::SourcesChangedEvent param)
{
    is_resource_sources_changed_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Provisioning_ResourcesChangedCb(fsm::ProvisioningInterface::ResourcesChangedEvent param)
{
    is_provResources_changed_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisionedResource_PayloadChangedCb(fsm::ProvisionedResourceInterface::PayloadChangedEvent param)
{
    is_prov_payload_changed_cb_ = true;
}

fscfg_ReturnCode ProvisionedResource_SourceChangedCb(fsm::ProvisionedResourceInterface::SourceChangedEvent param)
{
    is_prov_source_changed_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Discovery_ResourcesChangedCb(fsm::DiscoveryInterface::ResourcesChangedEvent param)
{
    is_discovery_resources_changed_cb_ = true;
    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature_StateChangedCb(fsm::FeatureInterface::StateChangedEvent param)
{
    EXPECT_TRUE(param.state == fsm::FeatureInterface::State::kEnabledVisible);
    is_state_changed_cb_ = true;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature_DescriptionChangedCb(fsm::FeatureInterface::DescriptionChangedEvent param)
{
    EXPECT_STREQ(param.description.c_str(), dbus_interface_->description_.c_str());
    is_description_changed_cb_ = true;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature_UriChangedCb(fsm::FeatureInterface::UriChangedEvent param)
{
    EXPECT_STREQ(param.uri.c_str(), dbus_interface_->uri_.c_str());
    is_uri_changed_cb_ = true;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature_IconChangedCb(fsm::FeatureInterface::IconChangedEvent param)
{
    EXPECT_STREQ(param.icon.c_str(), dbus_interface_->icon_.c_str());
    is_icon_changed_cb_ = true;

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Feature_TagsChangedCb(fsm::FeatureInterface::TagsChangedEvent param)
{
    std::vector<std::string> tags = dbus_interface_->tags_;

    // sort contents so, vector to be comparable
    std::sort(param.tags.begin(), param.tags.end());
    std::sort(tags.begin(), tags.end());

    EXPECT_TRUE(param.tags == tags);
    is_tags_changed_cb_ = true;

    return fscfg_kRcSuccess;
}

int main(int argc, char** argv)
{
    int rc = 0;

    ::testing::InitGoogleTest(&argc, argv);

    // Create default objects on DBus.
    dbus_interface_->CreateDefaultGDBusObjects();

    rc = RUN_ALL_TESTS();

    // Clean up
    dbus_interface_->DBusClean();

    return rc;
}
/** \}    end of addtogroup */
