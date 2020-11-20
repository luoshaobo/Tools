#ifndef CLIMATE_CONTROL_PAYLOAD_CONFIG_H
#define CLIMATE_CONTROL_PAYLOAD_CONFIG_H

#include "BigDataServiceGlobal.h"
#include "SidebandPayloadConfig.h"

namespace volvo_on_call {
namespace bds {

    struct ClimateControlPayloadConfig : public SidebandPayloadConfig
    {
        enum {
#ifndef USE_GEN_FILES
            PI_PORT_NAME1 = 0,
            PI_PORT_NAME2,
            PI_PORT_NAME3,
            PI_MAX
#else
#include "gen/gen_ClimateControlPayloadConfig_enum.h"
#endif // #ifndef USE_GEN_FILES
        };

        ClimateControlPayloadConfig();

        bool ProbeServiceConfig(BigDataServiceConfig &config);
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef CLIMATE_CONTROL_PAYLOAD_CONFIG_H
