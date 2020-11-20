#ifndef SIDEBAND_PAYLOAD_CONFIG_H
#define SIDEBAND_PAYLOAD_CONFIG_H

#include "BigDataServiceGlobal.h"
#include "SidebandPayloadEncoder.h"
#include "BigDataServiceConfig.h"

namespace volvo_on_call {
namespace bds {

    struct SidebandPayloadConfig
    {
        struct PortInfo {
            std::string portName;
            std::vector<SidebandPayload::DataItem> defaultItems;
            size_t defaultCollectingDuration;
        };
    
        std::vector<PortInfo> portsInfo;
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef SIDEBAND_PAYLOAD_CONFIG_H
