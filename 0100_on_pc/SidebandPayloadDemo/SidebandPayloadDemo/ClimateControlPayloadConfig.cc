#include "BigDataServiceGlobal.h"
#include "BigDataServiceConfigManager.h"
#include "ClimateControlPayloadConfig.h"
#include "ClimateControlPackageHandler.h"

namespace volvo_on_call {
namespace bds {

    ClimateControlPayloadConfig::ClimateControlPayloadConfig()
    {
        portsInfo = {
#ifndef USE_GEN_FILES
            {
                "PORT_NAME1",
                {
                    SidebandPayload::DataItem((uint8_t)11, "PORT_NAME1_ITEM1"),
                    SidebandPayload::DataItem((uint16_t)22, "PORT_NAME1_ITEM2"),
                    SidebandPayload::DataItem((uint32_t)33, "PORT_NAME1_ITEM3"),
                },
                5,
            },
            {
                "PORT_NAME2",
                {
                    SidebandPayload::DataItem((uint16_t)111, "PORT_NAME2_ITEM1"),
                    SidebandPayload::DataItem((uint16_t)222, "PORT_NAME2_ITEM2"),
                    SidebandPayload::DataItem((uint16_t)333, "PORT_NAME2_ITEM3"),
                },
                2,
            },
            {
                "PORT_NAME3",
                {
                    SidebandPayload::DataItem((uint8_t)1, "PORT_NAME3_ITEM1"),
                    SidebandPayload::DataItem((uint8_t)2, "PORT_NAME3_ITEM2"),
                    SidebandPayload::DataItem(std::string("3"), "PORT_NAME3_ITEM3"),
                },
                10,
            },
#else
#include "gen/gen_ClimateControlPayloadConfig_doInitailization.h"
#endif // #ifndef USE_GEN_FILES
        };
    }

    bool ClimateControlPayloadConfig::ProbeServiceConfig(BigDataServiceConfig &config)
    {
        bool ret = false;
        std::vector<std::string> selectedPortNames;
        std::vector<size_t> selectedPorts;

        selectedPortNames = BigDataServiceConfigManager::splitWithTrim(config.selectedPortNames, ',');

        for (const std::string &portName : selectedPortNames) {
            size_t i = 0;
            for (; i < portsInfo.size(); ++i) {
                PortInfo &portInfo = portsInfo[i];
                if (portInfo.portName == portName) {
                    selectedPorts.push_back(i);
                    break;
                }
            }
            if (i >= portsInfo.size()) {
                return false;
            }
        }

        config.valid = true;
        config.selectedPorts = selectedPorts;
        config.packageHandler = std::make_shared<ClimateControlPackageHandler>(config, *this);

        return ret;
    }

} // namespace bds {
} // namespace volvo_on_call {
