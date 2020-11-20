#include "BigDataServiceGlobal.h"
#include "ClimateControlPackageHandlerEx.h"
#include "PackageTransceiver.h"

namespace volvo_on_call {
namespace bds {

    ClimateControlPackageHandlerEx::ClimateControlPackageHandlerEx(const BigDataServiceConfig &serviceConfig, const SidebandPayloadConfig &payloadConfig, UploadTriggerType uploadTriggerType)
        : ClimateControlPackageHandler(serviceConfig, payloadConfig, uploadTriggerType)
    {

    }

    ClimateControlPackageHandlerEx::~ClimateControlPackageHandlerEx()
    {

    }

    bool ClimateControlPackageHandlerEx::checkNeededToUpload()
    {
        //
        // Simulate to check whether there is any change between the last collected raw data frame and the previous one of it.
        //
        static size_t count = 0;
    
        if (++count % 20 == 0) {
            printf("=== Contion triggered to upload: ===\n");
            return true;
        } else {
            return false;
        }
    }

} // namespace bds {
} // namespace volvo_on_call {
