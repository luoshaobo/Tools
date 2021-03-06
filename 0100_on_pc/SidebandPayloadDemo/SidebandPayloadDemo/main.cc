#include "BigDataServiceGlobal.h"
#include "ClimateControlPackageHandler.h"
#include "ClimateControlPackageHandlerEx.h"
#include "BigDataServiceConfigManager.h"

using namespace volvo_on_call;
using namespace volvo_on_call::bds;

//
// test case for basic payload encoding
//
void TestCase_BasicPayloadEncoding()
{
    SidebandPayload sidebandPayload;
    sidebandPayload.head.configId = "ClimateControl";
    sidebandPayload.body.maxSampleCount = 2;
    sidebandPayload.body.ports[1] = SidebandPayload::Port {
        "port1",
        3,
        {
            SidebandPayload::PortSample {
                {
                    SidebandPayload::DataItem{ (uint8_t)11, "port1_element1" },
                    SidebandPayload::DataItem{ (uint16_t)22, "port2_element2" },
                    SidebandPayload::DataItem{ (uint32_t)33, "port3_element3" },
                },
                true,
            },
            SidebandPayload::PortSample {
                {
                    SidebandPayload::DataItem{ (uint8_t)44, "port1_element1" },
                    SidebandPayload::DataItem{ (uint16_t)55, "port2_element2" },
                    SidebandPayload::DataItem{ (uint32_t)66, "port3_element3" },
                },
                true,
            },
        },
        true
    };
    sidebandPayload.body.ports[3] = SidebandPayload::Port {
        "port3",
        2,
        {
            SidebandPayload::PortSample {
                {
                    SidebandPayload::DataItem{ (int16_t)111, "port3_element1" },
                    SidebandPayload::DataItem{ (int32_t)222, "port3_element2" },
                    SidebandPayload::DataItem{ "333", "port3_element3" },
                },
                true,
            },
            SidebandPayload::PortSample {
                {
                    SidebandPayload::DataItem{ (int16_t)444, "port3_element1" },
                    SidebandPayload::DataItem{ (int32_t)555, "port3_element2" },
                    SidebandPayload::DataItem{ "666", "port3_element3" },
                },
                true,
            },
        },
        true
    };
    sidebandPayload.body.ports[4] = SidebandPayload::Port {
        "port4",
        2,
        {
            SidebandPayload::PortSample {
                {
                    SidebandPayload::DataItem{ SidebandPayload::DataItem::Timestamp::CurrentTimestamp(), "port4_element1" },
                },
                true,
            },
            SidebandPayload::PortSample {
                {
                    SidebandPayload::DataItem{ SidebandPayload::DataItem::Timestamp::CurrentTimestamp(), "port4_element1" },
                },
                true,
            },
        },
        true
    };
    const std::vector<size_t> selectedPorts = { 3, 1, 4 };
    SidebandPayloadEncoder sidebandPayloadEncoder(sidebandPayload, selectedPorts);
    std::string encodedSidebandPayload;
    sidebandPayloadEncoder.encode(encodedSidebandPayload);

    printf(encodedSidebandPayload.c_str());
}

//
// test case for reload config  
//
void TestCase_ReloadConfig()
{
    //
    // Initialization
    //
    BigDataServiceConfig climateControlServiceConfig;
    const ClimateControlPayloadConfig climateControlPayloadConfig;
    ClimateControlPackageHandler climateControlPackageHandler(climateControlServiceConfig, climateControlPayloadConfig);

    //
    // Collect with the first config
    //
    climateControlServiceConfig.configId = "BigDataClimateControl_1";
    climateControlServiceConfig.version = 123;
    climateControlServiceConfig.selectedPorts = { 0, 1, 2 };
    climateControlServiceConfig.collectingDuration = 1;
    climateControlServiceConfig.uploadingDuration = 10;

    climateControlPackageHandler.reset();

    for (size_t i = 0; i < 11; ++i) {
        climateControlPackageHandler.collectData();
        msleep(BigDataServicePackageHandler::getDefaultTickDuration());
    }

    //
    // Collect with the second config
    //
    climateControlServiceConfig.configId = "BigDataClimateControl_2";
    climateControlServiceConfig.version = 321;
    climateControlServiceConfig.selectedPorts = { 0, 1 };
    climateControlServiceConfig.collectingDuration = 1;
    climateControlServiceConfig.uploadingDuration = 10;

    climateControlPackageHandler.reset();

    for (size_t i = 0; i < 11; ++i) {
        climateControlPackageHandler.collectData();
        msleep(BigDataServicePackageHandler::getDefaultTickDuration());
    }

    //
    // Collect with the third config
    //
    climateControlServiceConfig.configId = "BigDataClimateControl_3";
    climateControlServiceConfig.version = 231;
    climateControlServiceConfig.selectedPorts = { 0, 2 };
    climateControlServiceConfig.collectingDuration = 5;
    climateControlServiceConfig.uploadingDuration = 10;

    climateControlPackageHandler.reset();

    for (size_t i = 0; i < 11; ++i) {
        climateControlPackageHandler.collectData();
        msleep(BigDataServicePackageHandler::getDefaultTickDuration());
    }
}

//
// test case for multiple package handlers  
//
void TestCase_MultiplePackageHandlers()
{
    //
    // Initialization
    //
    BigDataServiceConfig climateControlServiceConfig1;
    BigDataServiceConfig climateControlServiceConfig2;
    BigDataServiceConfig climateControlServiceConfig3;

    const ClimateControlPayloadConfig climateControlPayloadConfig;

    climateControlServiceConfig1.configId = "BigDataClimateControl_1";
    climateControlServiceConfig1.version = 123;
    climateControlServiceConfig1.selectedPorts = { 0, 1, 2 };
    climateControlServiceConfig1.collectingDuration = 1;
    climateControlServiceConfig1.uploadingDuration = 10;

    climateControlServiceConfig2.configId = "BigDataClimateControl_2";
    climateControlServiceConfig2.version = 321;
    climateControlServiceConfig2.selectedPorts = { 2, 1, 0 };
    climateControlServiceConfig2.collectingDuration = 1;
    climateControlServiceConfig2.uploadingDuration = 10;

    climateControlServiceConfig3.configId = "BigDataClimateControl_3";
    climateControlServiceConfig3.version = 231;
    climateControlServiceConfig3.selectedPorts = { 0, 2 };
    climateControlServiceConfig3.collectingDuration = 10;
    climateControlServiceConfig3.uploadingDuration = 20;

    ClimateControlPackageHandler climateControlPackageHandler1(climateControlServiceConfig1, climateControlPayloadConfig);
    ClimateControlPackageHandler climateControlPackageHandler2(climateControlServiceConfig2, climateControlPayloadConfig);
    ClimateControlPackageHandler climateControlPackageHandler3(climateControlServiceConfig3, climateControlPayloadConfig);

    //
    // Run
    //
    for (size_t i = 0; ; ++i) {
        climateControlPackageHandler1.StepTick();
        climateControlPackageHandler2.StepTick();
        climateControlPackageHandler3.StepTick();

        msleep(BigDataServicePackageHandler::getDefaultTickDuration());
    }
}

//
// test case for customization triggered upload
//
void TestCase_CustomizationTriggeredUpload()
{
    //
    // Initialization
    //
    BigDataServiceConfig climateControlServiceConfig1;
    const ClimateControlPayloadConfig climateControlPayloadConfig;

    climateControlServiceConfig1.configId = "BigDataClimateControl_CustomizationTriggered";
    climateControlServiceConfig1.version = 123;
    climateControlServiceConfig1.selectedPorts = { 0, 3, 2, 1 };
    climateControlServiceConfig1.collectingDuration = 1;
    climateControlServiceConfig1.uploadingDuration = 3;

    ClimateControlPackageHandler climateControlPackageHandler1(climateControlServiceConfig1, climateControlPayloadConfig, ClimateControlPackageHandler::UTT_Customized);

    //
    // Run
    //
    for (size_t i = 0; ; ++i) {                             // Simulate that there is a raw data frame received.
        climateControlPackageHandler1.collectData();        // Collect from the latest raw data frame.
        if (i != 0 && (i % 10) == 0) {                      // Simulate that there are some changes in the fraw data frame.
            printf("\n=== CustomizationTriggeredUpload ===\n");
            climateControlPackageHandler1.forceUpload();    // Force to upload.
        }

        msleep(1000);
    }
}

//
// test case for condition triggered upload
//
void TestCase_ConditionTriggerdUpload()
{
    //
    // Initialization
    //
    BigDataServiceConfig climateControlServiceConfig1;
    const ClimateControlPayloadConfig climateControlPayloadConfig;

    climateControlServiceConfig1.configId = "BigDataClimateControl_Condition";
    climateControlServiceConfig1.version = 123;
    climateControlServiceConfig1.selectedPorts = { 0, 3, 2, 1 };
    climateControlServiceConfig1.collectingDuration = 1;
    climateControlServiceConfig1.uploadingDuration = 3;

    ClimateControlPackageHandlerEx climateControlPackageHandler1(climateControlServiceConfig1, climateControlPayloadConfig);

    //
    // Run
    //
    for (size_t i = 0; ; ++i) {
        climateControlPackageHandler1.StepTick();

        msleep(BigDataServicePackageHandler::getDefaultTickDuration());
    }
}

//
// test case for loading and storing configs
//
void TestCase_LoadingAndStoringConfigs()
{
    ClimateControlPayloadConfig climateControlPayloadConfig;

    BigDataServiceConfigManager &serviceConfigManager = BigDataServiceConfigManager::getInstance();
    serviceConfigManager.addServiceConfigProdeFunc(std::bind(&ClimateControlPayloadConfig::ProbeServiceConfig, &climateControlPayloadConfig, std::placeholders::_1));
    serviceConfigManager.loadConfigs();

    std::vector<std::shared_ptr<BigDataServiceConfig> > &configs = serviceConfigManager.getConfigs();
    if (configs.size() >= 1) {
        {
            std::shared_ptr<BigDataServiceConfig> config(std::make_shared<BigDataServiceConfig>());
            *config = *configs.back();

            config->configId += "_new";
            serviceConfigManager.addConfig(config);
            serviceConfigManager.saveConfigs();
        }

        {
            std::shared_ptr<BigDataServiceConfig> config(std::make_shared<BigDataServiceConfig>());
            *config = *configs.back();

            config->configId += "_new";
            serviceConfigManager.addConfig(config);
            serviceConfigManager.saveConfigs();
        }
    }
}

//
// test case for loading cofings from persistency and running, then a new config is added/changed
//
void TestCase_LoadingConfigsFromPersistencyAndRunThenNewConfigAddedOrChanged()
{
    //
    // Initialization
    //
    ClimateControlPayloadConfig climateControlPayloadConfig;
    BigDataServiceConfigManager &serviceConfigManager = BigDataServiceConfigManager::getInstance();
    std::vector<std::shared_ptr<BigDataServiceConfig> > &serviceConfigs = serviceConfigManager.getConfigs();

    serviceConfigManager.addServiceConfigProdeFunc(std::bind(&ClimateControlPayloadConfig::ProbeServiceConfig, &climateControlPayloadConfig, std::placeholders::_1));
    serviceConfigManager.loadConfigs();
    
    {
        //
        // Run
        //
        printf("=== %s ===\n", CurrentTimeStr().c_str());
        for (size_t i = 0; i < 15; ++i) {
            for (std::shared_ptr<BigDataServiceConfig> &serviceConfig : serviceConfigs) {
                if (serviceConfig->valid && serviceConfig->packageHandler != nullptr) {
                    printf("=== %d: %s: %s ===\n", i, CurrentTimeStr().c_str(), serviceConfig->packageHandler->getServiceConfig().configId.c_str());
                    serviceConfig->packageHandler->StepTick();
                }
            }

            msleep(BigDataServicePackageHandler::getDefaultTickDuration());
        }
    }

    printf("\n");
    printf("==========================================================================================\n");
    printf("\n");

    {
        // To modify a config
        std::shared_ptr<BigDataServiceConfig> config0 = serviceConfigs.front();
        config0->selectedPortNames = "hvBattPmpPwrCns,eldtPmpCmd";
        serviceConfigManager.addConfig(config0);

        // To add a new config
        std::shared_ptr<BigDataServiceConfig> configN(std::make_shared<BigDataServiceConfig>());
        configN->configId = "configN";
        configN->selectedPortNames = "sunDataAzi";
        configN->collectingDuration = 3;
        configN->uploadingDuration = 6;
        serviceConfigManager.addConfig(configN);

        // To delete a config
        //if (serviceConfigs.size() >= 3) {
        //    auto it = serviceConfigs.begin();
        //    ++it;
        //    serviceConfigs.erase(it);
        //}

        //
        // Run
        //
        printf("=== %s ===\n", CurrentTimeStr().c_str());
        for (size_t i = 0; i < 30; ++i) {
            for (std::shared_ptr<BigDataServiceConfig> &serviceConfig : serviceConfigs) {
                if (serviceConfig->valid && serviceConfig->packageHandler != nullptr) {
                    printf("=== %d: %s: %s ===\n", i, CurrentTimeStr().c_str(), serviceConfig->packageHandler->getServiceConfig().configId.c_str());
                    serviceConfig->packageHandler->StepTick();
                }
            }

            msleep(BigDataServicePackageHandler::getDefaultTickDuration());
        }
    }
}

int main()
{
    //TestCase_BasicPayloadEncoding();
    //TestCase_ReloadConfig();
    //TestCase_MultiplePackageHandlers();
    //TestCase_CustomizationTriggeredUpload();
    //TestCase_ConditionTriggerdUpload();
    //TestCase_LoadingAndStoringConfigs();
    TestCase_LoadingConfigsFromPersistencyAndRunThenNewConfigAddedOrChanged();

    return 0;
}
