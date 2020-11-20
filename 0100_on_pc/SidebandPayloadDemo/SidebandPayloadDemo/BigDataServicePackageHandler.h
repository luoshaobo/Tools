#ifndef BIG_DATA_SERVICE_PACKAGE_HANDLER_H
#define BIG_DATA_SERVICE_PACKAGE_HANDLER_H

#include "BigDataServiceGlobal.h"
#include "BigDataServiceConfig.h"
#include "SidebandPayload.h"
#include "SidebandPayloadConfig.h"
#include "SidebandPayloadEncoder.h"

namespace volvo_on_call {
namespace bds {

    class BigDataServicePackageHandler
    {
    public:
        enum UploadTriggerType {
            UTT_Cyclic,
            UTT_Condition,
            UTT_Customized,
        };

        struct Context {
            virtual ~Context() {}
        };

    public:
        BigDataServicePackageHandler(const BigDataServiceConfig &serviceConfig, const SidebandPayloadConfig &payloadConfig, UploadTriggerType uploadTriggerType);
        virtual ~BigDataServicePackageHandler();

    public:
        void reset();
        bool collectData();
        bool forceUpload();
        void StepTick();

    public:
        const BigDataServiceConfig &getServiceConfig();
        SidebandPayload &getPayload();
        UploadTriggerType getUploadTriggerType() const;
        static size_t getDefaultTickDuration();                         // miliseconds
        size_t getTickDuration() const;                                 // miliseconds

    protected:
        virtual bool collectPortData(size_t packageIndex, size_t portIndex, std::vector<SidebandPayload::DataItem> &items, bool &valid) = 0;
        virtual void sendPackage(const std::string &encodedPayload) = 0;
        virtual bool checkNeededToUpload() = 0;
        virtual bool checkRunning(Context *context, bool &toReset) = 0;

    protected:
        bool initialize();
        bool finalize();
        void doInitialization();
        bool getPortNeededToUpload(size_t portIndex) const;
        size_t getBodyMaxSampleCount() const;
        size_t getPortMaxSampleCount(size_t portIndex) const;
        size_t getPortCollectingDuration(size_t portIndex) const;
        size_t getCollectingDuration() const;

    protected:
        const BigDataServiceConfig &m_serviceConfig;
        const SidebandPayloadConfig &m_payloadConfig;
        SidebandPayload m_payload;
        SidebandPayload::Head &m_head;
        SidebandPayload::Body &m_body;
        SidebandPayloadEncoder m_encoder;
        UploadTriggerType m_uploadTriggerType;
        size_t m_collectionCount;
        size_t m_lastCollectionIndex;
        size_t m_tickDuration;              // miliseconds
        size_t m_tick;
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef BIG_DATA_SERVICE_PACKAGE_HANDLER_H
