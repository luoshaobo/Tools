#include <cstdlib>

#include <gio/gio.h>
#include <glib.h>
#include <functional>
#include <iostream>

#include <dlt/dlt.h>

#include <fscfg/fscfg.h>
#include <fscfg/config_interface.h>
#include <fscfg/provisioning_interface.h>
#include <fscfg/provisionedresource_interface.h>

DLT_IMPORT_CONTEXT(dlt_libfscfg);

fscfg_ReturnCode Config_EncodingsChangedCb(fsm::ConfigInterface::EncodingsChangedEvent event);
fscfg_ReturnCode Config_SourcesChangedCb(fsm::ConfigInterface::SourcesChangedEvent event);
fscfg_ReturnCode Config_ResourcesChangedCb(fsm::ConfigInterface::ResourcesChangedEvent event);

fscfg_ReturnCode Resource_SourcesChangedCb(fsm::ResourceInterface::SourcesChangedEvent event);
fscfg_ReturnCode Resource_PayloadsChangedCb(fsm::ResourceInterface::PayloadsChangedEvent event);

fscfg_ReturnCode Provisioning_ResourcesChangedCb(fsm::ProvisioningInterface::ResourcesChangedEvent event);
fscfg_ReturnCode ProvisionedResource_SourceChangedCb(fsm::ProvisionedResourceInterface::SourceChangedEvent event);
fscfg_ReturnCode ProvisionedResource_PayloadChangedCb(fsm::ProvisionedResourceInterface::PayloadChangedEvent event);

fscfg_ReturnCode Discovery_ResourcesChangedCb(fsm::DiscoveryInterface::ResourcesChangedEvent event);

fscfg_ReturnCode Feature_StateChangedCb(fsm::FeatureInterface::StateChangedEvent event);
fscfg_ReturnCode Feature_DescriptionChangedCb(fsm::FeatureInterface::DescriptionChangedEvent event);
fscfg_ReturnCode Feature_UriChangedCb(fsm::FeatureInterface::UriChangedEvent event);
fscfg_ReturnCode Feature_IconChangedCb(fsm::FeatureInterface::IconChangedEvent event);
fscfg_ReturnCode Feature_TagsChangedCb(fsm::FeatureInterface::TagsChangedEvent event);

int main(int argc, const char* argv[])
{

    std::shared_ptr<fsm::ConfigInterface> cfg_if = fsm::GetConfigInterface();
    std::shared_ptr<fsm::ProvisioningInterface> pro_if = fsm::GetProvisioningInterface();
    std::shared_ptr<fsm::DiscoveryInterface> dis_if = fsm::GetDiscoveryInterface();

    std::uint32_t enc_bind_id;
    std::uint32_t src_bind_id;
    std::uint32_t rsc_bind_id;

    std::uint32_t pro_bind_id;

    std::uint32_t dis_bind_id;

    cfg_if->BindSourcesChanged(Config_SourcesChangedCb, src_bind_id);
    cfg_if->BindEncodingsChanged(Config_EncodingsChangedCb, enc_bind_id);
    cfg_if->BindResourcesChanged(Config_ResourcesChangedCb, rsc_bind_id);

    pro_if->BindResourcesChanged(Provisioning_ResourcesChangedCb, pro_bind_id);

    dis_if->BindResourcesChanged(Discovery_ResourcesChangedCb, dis_bind_id);

    GMainLoop* main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    g_main_loop_unref(main_loop);

    cfg_if->Unbind(src_bind_id);
    cfg_if->Unbind(enc_bind_id);

    return EXIT_SUCCESS;
}

fscfg_ReturnCode Config_EncodingsChangedCb(fsm::ConfigInterface::EncodingsChangedEvent param)
{
    std::shared_ptr<fsm::ConfigInterface> cfg_if = fsm::GetConfigInterface();

    std::vector<std::shared_ptr<fsm::EncodingInterface>> encodings;

    cfg_if->GetAll(encodings);

    for (std::shared_ptr<fsm::EncodingInterface>& encoding : encodings)
    {
        std::string name;

        encoding->GetName(name);

        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO, "Encoding: %s", name.c_str());
    }

    return fscfg_kRcSuccess;
}


fscfg_ReturnCode Config_SourcesChangedCb(fsm::ConfigInterface::SourcesChangedEvent param)
{
    std::shared_ptr<fsm::ConfigInterface> cfg_if = fsm::GetConfigInterface();

    std::vector<std::shared_ptr<fsm::SourceInterface>> sources;

    cfg_if->GetAll(sources);

    for (std::shared_ptr<fsm::SourceInterface>& source : sources)
    {
        std::string name;

        source->GetName(name);

        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO, "Source: %s", name.c_str());
    }

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Config_ResourcesChangedCb(fsm::ConfigInterface::ResourcesChangedEvent param)
{
    std::shared_ptr<fsm::ConfigInterface> cfg_if = fsm::GetConfigInterface();

    std::vector<std::shared_ptr<fsm::ResourceInterface>> resources;

    cfg_if->GetAll(resources);

    for (std::shared_ptr<fsm::ResourceInterface>& resource : resources)
    {
        std::string name;
        std::uint32_t rsc_bind_id;
        std::uint32_t py_bind_id;

        resource->GetName(name);

        // Unbind?
        resource->BindSourcesChanged(Resource_SourcesChangedCb, rsc_bind_id);
        resource->BindPayloadsChanged(Resource_PayloadsChangedCb, py_bind_id);

        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO, "Config Resource: %s", name.c_str());
    }

    return fscfg_kRcSuccess;
}


fscfg_ReturnCode Resource_SourcesChangedCb(fsm::ResourceInterface::SourcesChangedEvent event)
{
    for (std::shared_ptr<fsm::SourceInterface>& source : event.sources)
    {
        std::string name;

        source->GetName(name);

        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO, "Resource Sources: %s", name.c_str());
    }

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Resource_PayloadsChangedCb(fsm::ResourceInterface::PayloadsChangedEvent event)
{
    std::string src_name;
    std::string rsc_name;
    std::string py_str;

    event.source->GetName(src_name);
    event.resource->GetName(rsc_name);
    py_str.assign(event.payload.begin(), event.payload.end());

    DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO,
                    "Resource::PayloadsChangedCb Source: %s Resource: %s, Payload: %s",
                    src_name.c_str(),
                    rsc_name.c_str(),
                    py_str.c_str());

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Provisioning_ResourcesChangedCb(fsm::ProvisioningInterface::ResourcesChangedEvent event)
{
    std::shared_ptr<fsm::ProvisioningInterface> pro_if = fsm::GetProvisioningInterface();

    std::vector<std::shared_ptr<fsm::ProvisionedResourceInterface>> pro_resources;

    pro_if->GetAll(pro_resources);

    for (std::shared_ptr<fsm::ProvisionedResourceInterface>& pro_resource : pro_resources)
    {
        std::string name;
        uint32_t rsc_bind_id;
        uint32_t py_bind_id;

        pro_resource->GetName(name);

        // Unbind?
        pro_resource->BindSourceChanged(ProvisionedResource_SourceChangedCb, rsc_bind_id);
        pro_resource->BindPayloadChanged(ProvisionedResource_PayloadChangedCb, py_bind_id);

        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO, "ProvisionedResource: %s", name.c_str());
    }
}

fscfg_ReturnCode ProvisionedResource_SourceChangedCb(fsm::ProvisionedResourceInterface::SourceChangedEvent event)
{
    std::string name;
    std::string resource_name;

    event.resource->GetName(resource_name);

    event.source->GetName(name);
    DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO, "ProvisionedResource(%s):SourceChanged: %s", resource_name.c_str(), name.c_str());

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode ProvisionedResource_PayloadChangedCb(fsm::ProvisionedResourceInterface::PayloadChangedEvent event)
{
    std::string rsc_name;
    std::string py_str;

    event.resource->GetName(rsc_name);
    py_str.assign(event.payload.begin(), event.payload.end());

    DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO,
                    "ProvisionedResource::PayloadChangedCb Resource: %s, Payload: %s",
                    rsc_name.c_str(),
                    py_str.c_str());

    return fscfg_kRcSuccess;
}

fscfg_ReturnCode Discovery_ResourcesChangedCb(fsm::DiscoveryInterface::ResourcesChangedEvent event)
{
    std::shared_ptr<fsm::DiscoveryInterface> disc_if = fsm::GetDiscoveryInterface();

    std::vector<std::shared_ptr<fsm::FeatureInterface>> feat_resources;

    disc_if->GetAll(feat_resources);

    for (std::shared_ptr<fsm::FeatureInterface>& feat_resource : feat_resources)
    {
        std::string name;
        uint32_t rsc_bind_id;
        uint32_t py_bind_id;

        feat_resource->GetName(name);

        // Unbind?
        // feat_resource->BindSourceChanged(Feature_SourceChangedCb, rsc_bind_id);
        // feat_resource->BindPayloadChanged(Feature_PayloadChangedCb, py_bind_id);

        DLT_LOG_STRINGF(dlt_libfscfg, DLT_LOG_INFO, "Feature: %s", name.c_str());
    }
}
