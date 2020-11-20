#ifndef CLIMATE_CONTROL_PACKAGE_HANDLER_EX_H
#define CLIMATE_CONTROL_PACKAGE_HANDLER_EX_H

#include "BigDataServiceGlobal.h"
#include "BigDataServicePackageHandler.h"
#include "ClimateControlPayloadConfig.h"
#include "ClimateControlPackageHandler.h"

namespace volvo_on_call {
namespace bds {

    class ClimateControlPackageHandlerEx : public ClimateControlPackageHandler
    {
    public:
        ClimateControlPackageHandlerEx(const BigDataServiceConfig &serviceConfig, const SidebandPayloadConfig &payloadConfig, UploadTriggerType uploadTriggerType = UTT_Condition);
        virtual ~ClimateControlPackageHandlerEx();

    private:
        virtual bool checkNeededToUpload() override;
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef CLIMATE_CONTROL_PACKAGE_HANDLER_EX_H
