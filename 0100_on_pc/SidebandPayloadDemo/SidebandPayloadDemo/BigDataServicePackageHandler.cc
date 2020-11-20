#include "BigDataServiceGlobal.h"
#include "BigDataServicePackageHandler.h"

namespace volvo_on_call {
namespace bds {

    static const unsigned int DEFAULT_COLLECTION_DURATION = 1 * 1000;         // 1000 milliseconds
    static const unsigned int DEFAULT_TICK_DURATION = 1 * 1000;               // 1000 milliseconds

    BigDataServicePackageHandler::BigDataServicePackageHandler(const BigDataServiceConfig &serviceConfig, const SidebandPayloadConfig &payloadConfig, UploadTriggerType uploadTriggerType)
        : m_serviceConfig(serviceConfig)
        , m_payloadConfig(payloadConfig)
        , m_payload()
        , m_head(m_payload.head)
        , m_body(m_payload.body)
        , m_encoder(m_payload, m_serviceConfig.selectedPorts)
        , m_uploadTriggerType(uploadTriggerType)
        , m_collectionCount(0)
        , m_lastCollectionIndex((size_t)-1)
        , m_tickDuration(DEFAULT_TICK_DURATION)
        , m_tick(0)
    {
        initialize();
    }

    BigDataServicePackageHandler::~BigDataServicePackageHandler()
    {
        finalize();
    }

    bool BigDataServicePackageHandler::initialize()
    {
        doInitialization();

        return true;
    }

    void BigDataServicePackageHandler::doInitialization()
    {
        m_head.configId = m_serviceConfig.configId;
        m_head.version = m_serviceConfig.version;
        m_head.timestamp = SidebandPayload::DataItem::Timestamp::CurrentTimestamp();
        m_head.portCount = (uint16_t)m_serviceConfig.selectedPorts.size();
        m_head.collectingDuration = m_serviceConfig.collectingDuration;
        m_head.uploadingDuration = m_serviceConfig.uploadingDuration;
        m_head.triggeredTimes = m_serviceConfig.triggeringCount;

        m_body.ports.clear();
        m_body.timestamps.clear();
        m_body.maxSampleCount = getBodyMaxSampleCount();
        for (size_t i = 0; i < m_payloadConfig.portsInfo.size(); ++i) {
            const SidebandPayloadConfig::PortInfo &portInfo = m_payloadConfig.portsInfo[i];
            SidebandPayload::Port port;
            port.needToUpload = getPortNeededToUpload(i);
            if (port.needToUpload) {
                port.maxSampleCount = getPortMaxSampleCount(i);
                port.portName = portInfo.portName;
                for (size_t j = 0; j < port.maxSampleCount; ++j) {
                    port.appendSample({ portInfo.defaultItems, false });
                }
                m_body.ports[i] = port;
            }
        }
    }

    bool BigDataServicePackageHandler::finalize()
    {
        return true;
    }

    size_t BigDataServicePackageHandler::getCollectingDuration() const
    {
        size_t collectionDuration = m_serviceConfig.collectingDuration;

        if (collectionDuration == 0) {
            collectionDuration = DEFAULT_COLLECTION_DURATION;
        }

        return collectionDuration;
    }

    const BigDataServiceConfig &BigDataServicePackageHandler::getServiceConfig()
    {
        return m_serviceConfig;
    }

    SidebandPayload &BigDataServicePackageHandler::getPayload()
    {
        return m_payload;
    }

    BigDataServicePackageHandler::UploadTriggerType BigDataServicePackageHandler::getUploadTriggerType() const
    {
        return m_uploadTriggerType;
    }

    void BigDataServicePackageHandler::reset()
    {
        m_payload = SidebandPayload();
        m_collectionCount = 0;
        m_lastCollectionIndex = (size_t)-1;
        m_tick = 0;
        doInitialization();
    }

    bool BigDataServicePackageHandler::getPortNeededToUpload(size_t portIndex) const
    {
        if (m_serviceConfig.selectedPorts.empty()) {
            return true;
        }

        auto it = std::find(m_serviceConfig.selectedPorts.begin(), m_serviceConfig.selectedPorts.end(), portIndex);
        if (it == m_serviceConfig.selectedPorts.end()){
            return false;
        }

        return true;
    }

    size_t BigDataServicePackageHandler::getBodyMaxSampleCount() const
    {
        size_t count = 0;

        if (m_serviceConfig.collectingDuration > 0) {
            count =  m_serviceConfig.uploadingDuration * 1000 / m_serviceConfig.collectingDuration;
        }

        if (count == 0) {
            count = 1;
        }

        return count;
    }

    size_t BigDataServicePackageHandler::getPortMaxSampleCount(size_t portIndex) const
    {
        size_t count = 0;

        if (m_serviceConfig.collectingDuration > 0) {
            count =  m_serviceConfig.uploadingDuration * 1000 / m_serviceConfig.collectingDuration;
        } else {
            if (portIndex < m_payloadConfig.portsInfo.size() && m_payloadConfig.portsInfo[portIndex].defaultCollectingDuration > 0) {
                count = m_serviceConfig.uploadingDuration / m_payloadConfig.portsInfo[portIndex].defaultCollectingDuration;
            }
        }

        if (count == 0) {
            count = 1;
        }

        return count;
    }

    size_t BigDataServicePackageHandler::getPortCollectingDuration(size_t portIndex) const
    {
        if (m_serviceConfig.collectingDuration != 0) {
            return m_serviceConfig.collectingDuration;
        }

        if (m_body.ports.find(portIndex) == m_body.ports.end()) {
            return size_t(-1);
        }

        const SidebandPayloadConfig::PortInfo &portInfo = m_payloadConfig.portsInfo[portIndex];
        return portInfo.defaultCollectingDuration;
    }

    bool BigDataServicePackageHandler::collectData()
    {
        if (m_serviceConfig.collectingDuration > 0) {
            if (((m_collectionCount * getCollectingDuration()) % m_serviceConfig.collectingDuration) == 0) {
                SidebandPayload::DataItem::Timestamp timestamp(SidebandPayload::DataItem::Timestamp::CurrentTimestamp());
                m_body.appendTimestamp(timestamp);
            }
        }

        for (size_t i = 0; i < m_payloadConfig.portsInfo.size(); i++) {
            if (!getPortNeededToUpload(i)) {
                continue;
            }

            if (m_body.ports.find(i) == m_body.ports.end()) {
                continue;
            }

            SidebandPayload::Port &port = m_body.ports[i];
            if (!port.needToUpload) {
                continue;
            }

            if (((m_collectionCount * getCollectingDuration()) % getPortCollectingDuration(i)) == 0) {
                std::vector<SidebandPayload::DataItem> items;
                bool valid = true;
                if (collectPortData(m_collectionCount, i, items, valid)) {
                    port.appendSample({ items, valid });
                }
            }
        }

        m_collectionCount++;

        if (m_uploadTriggerType == UTT_Cyclic) {
            if (m_serviceConfig.uploadingDuration > 0) {
                if (((m_collectionCount * getCollectingDuration()) % (m_serviceConfig.uploadingDuration * 1000)) == 0) {
                    m_head.timestamp = SidebandPayload::DataItem::Timestamp::CurrentTimestamp();
                    std::string encodedPayload;
                    bool ret = m_encoder.encode(encodedPayload);
                    if (ret) {
                        sendPackage(encodedPayload);
                    }
                    m_body.timestamps.clear();
                }
            }
        } else if (m_uploadTriggerType == UTT_Condition) {
            if (checkNeededToUpload()) {
                m_head.timestamp = SidebandPayload::DataItem::Timestamp::CurrentTimestamp();
                std::string encodedPayload;
                bool ret = m_encoder.encode(encodedPayload);
                if (ret) {
                    sendPackage(encodedPayload);
                }
                m_body.timestamps.clear();
            }
        } else { // UTT_Customized
            // do nothing
        }

        if (m_collectionCount == 4000000) {
            m_collectionCount = 0;
        }

        return true;
    }

    bool BigDataServicePackageHandler::forceUpload()
    {
        bool ret = true;

        if (m_body.timestamps.size() > 0) {
            m_head.timestamp = m_body.timestamps.back();
        } else {
            m_head.timestamp = SidebandPayload::DataItem::Timestamp::CurrentTimestamp();
        }

        std::string encodedPayload;
        ret = m_encoder.encode(encodedPayload);
        if (!ret) {
            return false;
        }
        sendPackage(encodedPayload);

        return true;
    }

    size_t BigDataServicePackageHandler::getDefaultTickDuration()
    {
        return DEFAULT_TICK_DURATION;
    }

    size_t BigDataServicePackageHandler::getTickDuration() const
    {
        return m_tickDuration;
    }

    void BigDataServicePackageHandler::StepTick()
    {
        if (((m_tick * m_tickDuration) % getCollectingDuration()) == 0) {
            collectData();
        }

        ++m_tick;
        if (m_tick >= 4000) {
            m_tick = 0;
        }
    }

} // namespace bds {
} // namespace volvo_on_call {
