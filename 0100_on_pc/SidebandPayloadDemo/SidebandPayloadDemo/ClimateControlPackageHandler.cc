#include "BigDataServiceGlobal.h"
#include "ClimateControlPackageHandler.h"
#include "PackageTransceiver.h"

namespace volvo_on_call {
namespace bds {

    static const char *TOPIC = "SBA/TEM/BDR/AIC";

    ClimateControlPackageHandler::ClimateControlPackageHandler(const BigDataServiceConfig &serviceConfig, const SidebandPayloadConfig &payloadConfig, UploadTriggerType uploadTriggerType)
        : BigDataServicePackageHandler(serviceConfig, payloadConfig, uploadTriggerType)
    {

    }

    ClimateControlPackageHandler::~ClimateControlPackageHandler()
    {

    }

    void ClimateControlPackageHandler::sendPackage(const std::string &encodedPayload)
    {
        IPackageTransceiver &packageTransceiver = IPackageTransceiver::getInstance();

        IPackageTransceiver::TranferToken token = 0;
        std::vector<uint8_t> package(encodedPayload.c_str(), encodedPayload.c_str() + encodedPayload.length());
        packageTransceiver.transfer(TOPIC, package, token);
    }

    bool ClimateControlPackageHandler::checkNeededToUpload()
    {
        return false;
    }

    bool ClimateControlPackageHandler::checkRunning(Context *context, bool &toReset)
    {
        if (context == nullptr) {
            return false;
        }

        toReset = false;
        return true;
    }

    bool ClimateControlPackageHandler::collectPortData(size_t packageIndex, size_t portIndex, std::vector<SidebandPayload::DataItem> &items, bool &valid)
    {
        typedef ClimateControlPayloadConfig PayloadConfig;

        if (m_lastCollectionIndex != packageIndex) {
            // TODO: to pick a raw data frame to collect data from.

            //
            // NOTE:
            //     1) Usually, the last collected raw data frame, and the previous one of it should be buffered.
            //     2) The last collected raw data frame will be collected for multi times for all of the ports.
            //     3) We can compare the last collected raw data frame with the previous one of it to find out whether there is any change.
            //

            m_lastCollectionIndex = packageIndex;
        }

        switch (portIndex) {
#ifndef USE_GEN_FILES
            case PayloadConfig::PI_PORT_NAME1:
                {
                    static int hint = 33;
                    items = {
                        SidebandPayload::DataItem((uint8_t)hint++, m_payloadConfig.portsInfo[portIndex].defaultItems[0].itemName),
                        SidebandPayload::DataItem((uint16_t)hint++, m_payloadConfig.portsInfo[portIndex].defaultItems[1].itemName),
                        SidebandPayload::DataItem((uint32_t)hint++, m_payloadConfig.portsInfo[portIndex].defaultItems[2].itemName),
                    };
                    valid = true;
                }
                break;
            case PayloadConfig::PI_PORT_NAME2:
                {
                    static int hint = 333;
                    items = {
                        SidebandPayload::DataItem((uint16_t)hint++, m_payloadConfig.portsInfo[portIndex].defaultItems[0].itemName),
                        SidebandPayload::DataItem((uint16_t)hint++, m_payloadConfig.portsInfo[portIndex].defaultItems[1].itemName),
                        SidebandPayload::DataItem((uint16_t)hint++, m_payloadConfig.portsInfo[portIndex].defaultItems[2].itemName),
                    };
                    valid = true;
                }
                break;
            case PayloadConfig::PI_PORT_NAME3:
                {
                    static int hint = 3;
                    items = {
                        SidebandPayload::DataItem((uint8_t)hint++, m_payloadConfig.portsInfo[portIndex].defaultItems[0].itemName),
                        SidebandPayload::DataItem((uint8_t)hint++, m_payloadConfig.portsInfo[portIndex].defaultItems[1].itemName),
                        SidebandPayload::DataItem((std::string)std::to_string(hint++), m_payloadConfig.portsInfo[portIndex].defaultItems[2].itemName),
                    };
                    valid = true;
                }
                break;
#else
#include "gen/gen_ClimateControlPackageHandler_collectPort.h"
#endif // #ifndef USE_GEN_FILES
                default:
                {
                    return false;
                }
                break;
        }

        return true;
    }

} // namespace bds {
} // namespace volvo_on_call {
