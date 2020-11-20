#ifndef CLIMATE_CONTROL_PACKAGE_HANDLER_H
#define CLIMATE_CONTROL_PACKAGE_HANDLER_H

#include "BigDataServiceGlobal.h"
#include "BigDataServicePackageHandler.h"
#include "ClimateControlPayloadConfig.h"

namespace volvo_on_call {
namespace bds {

    class ClimateControlPackageHandler : public BigDataServicePackageHandler
    {
    public:
        ClimateControlPackageHandler(const BigDataServiceConfig &serviceConfig, const SidebandPayloadConfig &payloadConfig, UploadTriggerType uploadTriggerType = UTT_Cyclic);
        virtual ~ClimateControlPackageHandler();

    private:
        virtual bool collectPortData(size_t packageIndex, size_t portIndex, std::vector<SidebandPayload::DataItem> &items, bool &valid) override;
        virtual void sendPackage(const std::string &encodedPayload) override;
        virtual bool checkNeededToUpload() override;
        virtual bool checkRunning(Context *context, bool &toReset) override;
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef CLIMATE_CONTROL_PACKAGE_HANDLER_H
