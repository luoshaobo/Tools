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
 *  \file     manual_test.cpp
 *  \brief    Foundation Services Daemon manual testing interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <future>

#include <glib.h>
#include <gio/gio.h>

#include <dlt/dlt.h>

#include <manual_test.h>
#include <test_utilities.h>

#include <fscfgd/fscfgd.h>

DLT_IMPORT_CONTEXT(dlt_fscfgd)

namespace fsm
{

    void PostResourceExpiry()
    {
        ExpiryEventNotification entry_point_expiry_notification;

        entry_point_expiry_notification.immediate = true;
        entry_point_expiry_notification.resource_name = "Features";

        std::vector<ExpiryEventNotification> expired_resources;

        expired_resources.push_back(entry_point_expiry_notification);
        ExpiryEvent expiry_event(expired_resources);
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            GetDaemon()->GetQueue().PostEvent(expiry_event);
            std::this_thread::sleep_for(std::chrono::seconds(10));
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

class TestInvalidator : public EventListenerInterface
{
public:
    fscfg_ReturnCode ExecuteEvent(Event ev)
    {
        fscfg_ReturnCode rc = fscfg_kRcSuccess;

        if (ev.event_class == EventClass::kDiscovery)
        {
            if (ev.subsystem_event.discovery_event.type == DiscoveryEventType::kStateTransition)
            {
                if (ev.subsystem_event.discovery_event.parameter.transition == DiscoveryProvider::State::kReady)
                {
                    std::thread([](){ PostResourceExpiry(); }).detach();
                }
            }
        };

        return rc;
    }
};

// Test EventListener
class TestEventListener : public EventListenerInterface
{
public:
    fscfg_ReturnCode ExecuteEvent(Event ev)
    {
        std::string ev_class_lut[] = {
            "kUndefined",
            "kDiscovery",
            "kExpiry",
            "kMax"
        };

        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO,
                        "Executing event class: %s", ev_class_lut[static_cast<int>(ev.event_class)].c_str());

        switch (ev.event_class)
        {
            case EventClass::kDiscovery:
            {
                DiscoveryEvent& event = ev.subsystem_event.discovery_event;

                std::string event_name_lut[] = {
                    "kUndefined",
                    "kExecuteTransition",
                    "kStateTransition",
                    "kMax"
                };

                DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO,
                                "Executing DiscoveryEvent: %s", event_name_lut[static_cast<int>(event.type)].c_str());

                std::string state_lut[] = {
                    "kUndefined",
                    "kInitial",
                    "kEntryPoint",
                    "kFeatureList",
                    "kReady",
                    "kMax"
                };

                switch (event.type)
                {

                    case DiscoveryEventType::kExecuteTransition:
                    {
                        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO,
                                        "DiscoveryEvent::ExecuteTransition %s",
                                        state_lut[static_cast<int>(event.parameter.transition)].c_str());
                        break;
                    }

                    case DiscoveryEventType::kStateTransition:
                    {

                        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO,
                                        "DiscoveryEvent::StateTransition %s",
                                        state_lut[static_cast<int>(event.parameter.transition)].c_str());
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }

                break;
            }

            case EventClass::kExpiry:
            {
                ExpiryEvent& event = ev.subsystem_event.expiry_event;

                std::string event_name_lut[]  = {
                    "kUndefined",
                    "kExpired",
                    "kMax"
                };

                DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO,
                                "Executing ExpiryEvent: %s",
                                event_name_lut[static_cast<int>(event.type)].c_str());

                switch (event.type)
                {
                    case ExpiryEventType::kExpired:
                    {
                        const std::vector<ExpiryEventNotification> & expired_resources
                                = event.parameter.expired_events;

                        for (const auto& expired_resource : expired_resources)
                        {
                            const std::string& resource_name = expired_resource.resource_name;


                            DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_INFO,
                                            "ExpiryEvent::Expired %s imd: %u",
                                            resource_name.c_str(),
                                            expired_resource.immediate);
                        }
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }

                break;
            }

            default:
            {
                DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_WARN, "Unexpected event class passed for execution");
                break;
            }
        }

        return fscfg_kRcSuccess;
    }
};

/// callbacks.
gboolean config_add_encodings_callback(gpointer user_data);
gboolean config_add_encoding_callback(gpointer user_data);
gboolean config_remove_encodings_callback(gpointer user_data);
gboolean test_config_add_resources_callback(gpointer user_data);
gboolean test_config_change_resources_callback(gpointer user_data);
gboolean test_config_change_resource_encoding_callback(gpointer user_data);
gboolean test_provisioning_add_resources_callback(gpointer user_data);
gboolean test_discovery_add_resources_callback(gpointer user_data);
gboolean test_cleanup(gpointer user_data);

ManualTest::ManualTest(std::shared_ptr<Config> config,
                       std::shared_ptr<Provisioning> provisioning,
                       std::shared_ptr<Discovery> discovery)
    : config_(config),
      provisioning_(provisioning),
      discovery_(discovery)
{
    SetConfig(config);
    SetProvisioning(provisioning);
    SetDiscovery(discovery);
};

ManualTest::~ManualTest()
{
}

void ManualTest::Start()
{
    Daemon* dmn = GetDaemon();
    DaemonEventQueue& event_queue = dmn->GetQueue();

    TestEventListener* test_listener = new TestEventListener();
    TestInvalidator* test_invalidator = new TestInvalidator();

    event_queue.RegisterListener(test_listener, EventClass::kDiscovery);
    event_queue.RegisterListener(test_invalidator, EventClass::kDiscovery);
}


gboolean config_add_encodings_callback(gpointer user_data)
{
    std::shared_ptr<Config> config = GetConfig();

    config->Register(std::make_shared<Encoding>("CCM"));
    config->Register(std::make_shared<Encoding>("ASN1"));

    config->Update();

    g_timeout_add(1000, config_add_encoding_callback, nullptr);

    return false;
}

gboolean config_add_encoding_callback(gpointer user_data)
{
    std::shared_ptr<Config> config = GetConfig();

    config->Register(std::make_shared<Encoding>("XML"));

    config->Update();

    g_timeout_add(1000, config_remove_encodings_callback, nullptr);

    return false;
}

gboolean config_remove_encodings_callback(gpointer user_data)
{
    std::shared_ptr<Config> config = GetConfig();

    config->RemoveEncoding("CCM");
    config->RemoveEncoding("ASN1");
    config->RemoveEncoding("XML");

    config->Update();

    g_timeout_add(1000, test_config_add_resources_callback,  reinterpret_cast<void*>(&config));

    return false;
}

gboolean test_config_add_resources_callback(gpointer user_data)
{
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "test_config_add_resources_callback");

    std::shared_ptr<Config> config = GetConfig();

    std::shared_ptr<Resource> resource_1 = CreateResource("resource_1");
    std::shared_ptr<Resource> resource_2 = CreateResource("resource_2");
    std::shared_ptr<Resource> resource_3 = CreateResource("resource_3");

    std::shared_ptr<Encoding> encoding_xml = CreateEncoding("XML");
    std::shared_ptr<Source> source_test = CreateSource("test");

    config->Register(encoding_xml);
    config->Register(source_test);

    config->Register(resource_1);
    config->Register(resource_2);
    config->Register(resource_3);

    source_test->SetPayloadEntry(resource_1, encoding_xml, CreatePayload("<resource>resource_1</resource>"));
    source_test->SetPayloadEntry(resource_2, encoding_xml, CreatePayload("<resource>resource_2</resource>"));
    source_test->SetPayloadEntry(resource_3, encoding_xml, CreatePayload("<resource>resource_3</resource>"));

    config->Update();
    source_test->Update();

    g_timeout_add(1000, test_config_change_resources_callback, reinterpret_cast<void*>(&config));

    return false;
}

gboolean test_config_change_resources_callback(gpointer user_data)
{
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "test_config_change_resources_callback");

    std::shared_ptr<Config> config = GetConfig();

    std::shared_ptr<Encoding> encoding_text = CreateEncoding("text");
    std::shared_ptr<Resource> resource_4 = CreateResource("resource_4");

    config->Register(encoding_text);

    config->Register(resource_4);

    std::shared_ptr<Source> source_test;
    std::shared_ptr<Encoding> encoding_xml;

    config->Get("test", source_test);
    config->Get("XML", encoding_xml);

    source_test->SetPayloadEntry(resource_4, encoding_text, CreatePayload("resource_4"));
    source_test->SetPayloadEntry(resource_4, encoding_xml, CreatePayload("<resource>resource_4</resource>"));

    config->Update();
    source_test->Update();

    g_timeout_add(1000, test_config_change_resource_encoding_callback, reinterpret_cast<void*>(&config));

    return false;
}

gboolean test_config_change_resource_encoding_callback(gpointer user_data)
{
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "test_config_change_resources_callback");

    std::shared_ptr<Config> config = GetConfig();

    std::shared_ptr<Source> source_test;
    std::shared_ptr<Encoding> encoding_text;
    std::shared_ptr<Resource> resource_4;

    config->Get("test", source_test);
    config->Get("text", encoding_text);
    config->Get("resource_4", resource_4);

    source_test->RemovePayloadEntry(resource_4, encoding_text);

    source_test->Update();

    g_timeout_add(1000, test_provisioning_add_resources_callback, nullptr);

    return false;
}

gboolean test_provisioning_add_resources_callback(gpointer user_data)
{
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "test_provisioning_add_resources_callback");

    std::shared_ptr<Config> config = GetConfig();
    std::shared_ptr<Provisioning> prov = GetProvisioning();

    std::shared_ptr<Source> source_dummy = CreateSource("dummy");
    std::shared_ptr<Source> source_test;

    std::shared_ptr<Encoding> encoding_xml;
    std::shared_ptr<Encoding> encoding_text;

    config->Get("test", source_test);

    config->Get("text", encoding_text);
    config->Get("XML", encoding_xml);

    config->Register(source_dummy);

    // resource configuration:
    // provres_1 : source_test->XML
    // provres_2 : source_test->XML, source_dummy->text
    // provres_3 : source_dummy->text
    std::shared_ptr<ProvisionedResource> provres_1 = CreateProvisionedResource("provres_1");
    std::shared_ptr<ProvisionedResource> provres_2 = CreateProvisionedResource("provres_2");
    std::shared_ptr<ProvisionedResource> provres_3 = CreateProvisionedResource("provres_3");

    prov->Register(provres_1);
    prov->Register(provres_2);
    prov->Register(provres_3);

    // provisioning configuration:
    // Encodings: XML, text
    // Sources: test, dummy

    prov->SetPriority(encoding_xml, Provisioning::Priority::kHighest);
    prov->SetPriority(encoding_text, Provisioning::Priority::kHigh);

    prov->SetPriority(source_test, Provisioning::Priority::kHighest);
    prov->SetPriority(source_dummy, Provisioning::Priority::kHigh);

    // update sources with payloads.
    source_test->SetPayloadEntry(provres_1, encoding_xml, CreatePayload("<resource>provres_1</resource>"));

    source_test->SetPayloadEntry(provres_2, encoding_xml, CreatePayload("<resource>provres_2</resource>"));
    source_dummy->SetPayloadEntry(provres_2, encoding_text, CreatePayload("provres_2"));

    source_dummy->SetPayloadEntry(provres_3, encoding_text, CreatePayload("provres_3"));

    // update, of course.
    prov->Update();

    source_test->Update();
    source_dummy->Update();

    g_timeout_add(1000, test_discovery_add_resources_callback, nullptr);

    return false;
}

gboolean test_discovery_add_resources_callback(gpointer user_data)
{
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "test_discovery_add_resources_callback");

    std::shared_ptr<Config> config = GetConfig();
    std::shared_ptr<Discovery> discovery = GetDiscovery();

    std::shared_ptr<Source> source_test;
    std::shared_ptr<Encoding> encoding_xml;

    config->Get("test", source_test);
    config->Get("XML", encoding_xml);

    std::shared_ptr<Feature> feat_1 = CreateFeature("feat_1");
    std::shared_ptr<Feature> feat_2 = CreateFeature("feat_2");
    std::shared_ptr<Feature> feat_3 = CreateFeature("feat_3");

    discovery->Register(feat_1);
    discovery->Register(feat_2);
    discovery->Register(feat_3);

    source_test->SetPayloadEntry(feat_1, encoding_xml, CreatePayload("<resource>feat_1</resource>"));
    source_test->SetPayloadEntry(feat_2, encoding_xml, CreatePayload("<resource>feat_2</resource>"));
    source_test->SetPayloadEntry(feat_3, encoding_xml, CreatePayload("<resource>feat_3</resource>"));

    feat_1->SetDescription("Feature One");
    feat_2->SetDescription("Feature Two");
    feat_3->SetDescription("Feature Three");

    discovery->Update();

    source_test->Update();

    feat_1->Update();
    feat_2->Update();
    feat_3->Update();

    g_timeout_add(1000, test_cleanup, nullptr);

    return false;
}

gboolean test_cleanup(gpointer user_data)
{
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "test_cleanup");

    // Clean-up everything.
    std::shared_ptr<Config> config;
    std::shared_ptr<Provisioning> prov;
    std::shared_ptr<Discovery> discovery;

    std::shared_ptr<Encoding> encoding_xml;
    std::shared_ptr<Encoding> encoding_text;
    std::shared_ptr<Source> source_test;
    std::shared_ptr<Source> source_dummy;

    config = GetConfig();
    prov = GetProvisioning();
    discovery = GetDiscovery();

    config->Get("test", source_test);
    config->Get("dummy", source_dummy);

    config->Get("XML", encoding_xml);
    config->Get("text", encoding_text);

    // Discovery.
    // Remove all feature payloads and features.
    std::shared_ptr<Feature> feat_1;
    std::shared_ptr<Feature> feat_2;
    std::shared_ptr<Feature> feat_3;

    discovery->Get("feat_1", feat_1);
    discovery->Get("feat_2", feat_2);
    discovery->Get("feat_3", feat_3);

    source_test->RemovePayloadEntry(feat_1, encoding_xml);
    source_test->RemovePayloadEntry(feat_2, encoding_xml);
    source_test->RemovePayloadEntry(feat_3, encoding_xml);

    source_test->Update();

    discovery->RemoveFeature("feat_1");
    discovery->RemoveFeature("feat_2");
    discovery->RemoveFeature("feat_3");

    discovery->Update();

    // Provisioning.
    // Remove all provisionedresource payloads and provisionedresources.
    std::shared_ptr<ProvisionedResource> provres_1;
    std::shared_ptr<ProvisionedResource> provres_2;
    std::shared_ptr<ProvisionedResource> provres_3;

    prov->Get("provres_1", provres_1);
    prov->Get("provres_2", provres_2);
    prov->Get("provres_3", provres_3);

    source_test->RemovePayloadEntry(provres_1, encoding_xml);
    source_test->RemovePayloadEntry(provres_2, encoding_xml);
    source_dummy->RemovePayloadEntry(provres_2, encoding_text);
    source_dummy->RemovePayloadEntry(provres_3, encoding_text);

    prov->RemoveResource("provres_1");
    prov->RemoveResource("provres_2");
    prov->RemoveResource("provres_3");

    source_test->Update();
    source_dummy->Update();

    prov->Update();

    // Config.
    // Remove all resource payloads and resources.
    std::shared_ptr<Resource> resource_1;
    std::shared_ptr<Resource> resource_2;
    std::shared_ptr<Resource> resource_3;
    std::shared_ptr<Resource> resource_4;

    config->Get("resource_1", resource_1);
    config->Get("resource_2", resource_2);
    config->Get("resource_3", resource_3);
    config->Get("resource_4", resource_4);

    source_test->RemovePayloadEntry(resource_1, encoding_xml);
    source_test->RemovePayloadEntry(resource_2, encoding_xml);
    source_test->RemovePayloadEntry(resource_3, encoding_xml);
    source_test->RemovePayloadEntry(resource_4, encoding_xml);

    source_test->Update();

    config->RemoveResource("resource_1");
    config->RemoveResource("resource_2");
    config->RemoveResource("resource_3");
    config->RemoveResource("resource_4");


    // Remove all sources and encodings.
    config->RemoveSource("test");
    config->RemoveSource("dummy");

    config->RemoveEncoding("XML");
    config->RemoveEncoding("text");

    config->Update();

    return false;
}

} // namespace fsm

/** \}    end of addtogroup */
