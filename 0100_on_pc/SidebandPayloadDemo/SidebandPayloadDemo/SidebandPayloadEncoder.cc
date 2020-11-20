#include "BigDataServiceGlobal.h"
#include "SidebandPayloadEncoder.h"

namespace volvo_on_call {
namespace bds {

    static const char *SIDEBAND_PAYLOAD_ENCODER_VERBOSE = "SIDEBAND_PAYLOAD_ENCODER_VERBOSE";

    SidebandPayloadEncoder::SidebandPayloadEncoder(SidebandPayload &payload, const std::vector<size_t> &selectedPorts)
        : m_payload(payload)
        , m_selectedPorts(selectedPorts)
        , m_crc(0)
        , m_crc2(0)
        , m_verbose(false)
    {
        char *pEnv = ::getenv(SIDEBAND_PAYLOAD_ENCODER_VERBOSE);
        if (pEnv != nullptr) {
            if (std::string(pEnv) == "1") {
                m_verbose = true;
            }
        }
    }

    SidebandPayloadEncoder::~SidebandPayloadEncoder()
    {

    }

    void SidebandPayloadEncoder::setVerbose(bool verbose)
    {
        m_verbose = verbose;
    }

    bool SidebandPayloadEncoder::encode(std::string &result)
    {
        bool ret = true;

        result.clear();

        ret = encodeHead(result);
        if (!ret) {
            return false;
        }

        ret = encodeBody(result);
        if (!ret) {
            return false;
        }

        return ret;
    }

    bool SidebandPayloadEncoder::encodeHead(std::string &result)
    {
        bool ret = true;

        if (m_verbose) {
            result += "[";
            result += "head";
            result += "]\n";
        }

        if (m_verbose) {
            result += "    [";
            result += "configId";
            result += "]";
        }
        ret = encodeDataItem(SidebandPayload::DataItem(m_payload.head.configId), result);
        if (!ret) {
            return false;
        }
        result += ",";
        if (m_verbose) {
            result += "\n";
        }

        if (m_verbose) {
            result += "    [";
            result += "version";
            result += "]";
        }
        ret = encodeDataItem(SidebandPayload::DataItem(m_payload.head.version), result);
        if (!ret) {
            return false;
        }
        result += ",";
        if (m_verbose) {
            result += "\n";
        }

        if (m_verbose) {
            result += "    [";
            result += "timestamp";
            result += "]";
        }
        ret = encodeDataItem(SidebandPayload::DataItem(m_payload.head.timestamp), result);
        if (!ret) {
            return false;
        }
        result += ",";
        if (m_verbose) {
            result += "\n";
        }

        if (m_verbose) {
            result += "    [";
            result += "portCount";
            result += "]";
        }
        ret = encodeDataItem(SidebandPayload::DataItem(m_payload.head.portCount), result);
        if (!ret) {
            return false;
        }
        result += ",";
        if (m_verbose) {
            result += "\n";
        }

        if (m_verbose) {
            if (m_verbose) {
                result += "    [";
                result += "collectingDuration";
                result += "]";
            }
            ret = encodeDataItem(SidebandPayload::DataItem(m_payload.head.collectingDuration), result);
            if (!ret) {
                return false;
            }
            result += ",";
            if (m_verbose) {
                result += "\n";
            }

            if (m_verbose) {
                result += "    [";
                result += "uploadingDuration";
                result += "]";
            }
            ret = encodeDataItem(SidebandPayload::DataItem(m_payload.head.uploadingDuration), result);
            if (!ret) {
                return false;
            }
            result += ",";
            if (m_verbose) {
                result += "\n";
            }
        }

        if (m_verbose) {
            result += "    [";
            result += "triggeredTimes";
            result += "]";
        }
        ret = encodeDataItem(SidebandPayload::DataItem(m_payload.head.triggeredTimes), result);
        if (!ret) {
            return false;
        }

        result += ";";
        if (m_verbose) {
            result += "\n";
        }

        return true;
    }

    bool SidebandPayloadEncoder::encodeBody(std::string &result)
    {
        bool ret = true;
        size_t sampleIndex = 0;
        auto it_ts =  m_payload.body.timestamps.begin();

        resetCRC();

        if (m_verbose) {
            result += "[";
            result += "body";
            result += "]\n";
        }

        for (; sampleIndex < m_payload.body.maxSampleCount; ++sampleIndex) {
            SidebandPayload::DataItem::Timestamp ts;
            if (it_ts != m_payload.body.timestamps.end()) {
                ts = *it_ts;
                ++it_ts;
            } else {
                ts = SidebandPayload::DataItem::Timestamp::CurrentTimestamp();
            }

            resetCRC2();

            if (m_verbose) {
                result += "    [";
                result += "timestamp";
                result += "]";
            }
            ret = encodeDataItem(SidebandPayload::DataItem(ts), result, true);
            if (!ret) {
                return false;
            }
            result += ";";
            appendCRC2(";");
            if (m_verbose) {
                result += "\n";
            }

            if (m_selectedPorts.size() == 0) {
                for (auto &portPar : m_payload.body.ports) {
                    const SidebandPayload::Port &port = portPar.second;

                    if (!port.needToUpload) {
                        continue;
                    }

                    ret = encodePort(port, sampleIndex, result);
                    if (!ret) {
                        return false;
                    }
                }
            } else {
                for (size_t portIndex : m_selectedPorts) {
                    auto it = m_payload.body.ports.find(portIndex);
                    if (it == m_payload.body.ports.end()) {
                        return false;
                    }

                    const SidebandPayload::Port &port = it->second;

                    if (!port.needToUpload) {
                        continue;
                    }

                    ret = encodePort(port, sampleIndex, result);
                    if (!ret) {
                        return false;
                    }
                }
            }

            if (m_verbose) {
                result += "    [";
                result += "crc";
                result += "]";
            }
            ret = encodeDataItem(SidebandPayload::DataItem(m_crc2), result);
            if (!ret) {
                return false;
            }
            result += ";";
            if (m_verbose) {
                result += "\n";
            }
        }

        return true;
    }

    bool SidebandPayloadEncoder::encodePort(const SidebandPayload::Port &port, size_t sampleIndex, std::string &result)
    {
        bool ret = true;
        size_t i = 0;
        bool encoded = false;

        if (m_verbose) {
            if (port.samples.size() != 0) {
                result += "    [";
                result += port.portName;
                result += "]";
                if (m_verbose) {
                    result += "\n";
                }
            }
        }

        for (const SidebandPayload::PortSample &sample : port.samples) {
            if (i == sampleIndex) {
                ret = encodePortSample(sample, result);
                if (!ret) {
                    return false;
                }
                encoded = true;
                break;
            }

            ++i;
        }

        if (!encoded) {
            return false;
        }

        if (port.samples.size() != 0) {
            result += ";";
            appendCRC2(";");
            if (m_verbose) {
                result += "\n";
            }
        }

        return true;
    }

    bool SidebandPayloadEncoder::encodePortSample(const SidebandPayload::PortSample &sample, std::string &result)
    {
        bool ret = true;
        size_t sampleIndex = 0;

        for (const SidebandPayload::DataItem &dataItem : sample.items) {
            if (m_verbose) {
                result += "        [";
                result += dataItem.itemName;
                result += "]";
            }

            ret = encodeDataItem(dataItem, result, true);
            if (!ret) {
                return false;
            }
            result += ",";
            appendCRC2(",");
            if (m_verbose) {
                result += "\n";
            }

            if (m_verbose) {
                result += "        [";
                result += "dataValid";
                result += "]";
            }
            ret = encodeDataItem(SidebandPayload::DataItem(sample.dataValid), result, true);
            if (!ret) {
                return false;
            }

            if (sampleIndex < sample.items.size() - 1) {
                result += ",";
                appendCRC2(",");
                if (m_verbose) {
                    result += "\n";
                }
            }

            sampleIndex++;
        }

        return true;
    }

    bool SidebandPayloadEncoder::encodeDataItem(const SidebandPayload::DataItem &dataItem, std::string &result, bool toAppendCrc2 /*= false*/)
    {
        bool ret = true;
        std::string partialResult;

        switch (dataItem.type) {
            case SidebandPayload::DataItem::DIT_UINT:
                {
                    partialResult = std::to_string(dataItem.ivalue.u);
                }
                break;
            case SidebandPayload::DataItem::DIT_UINT8:
                {
                    partialResult = std::to_string(dataItem.ivalue.u8);
                }
                break;
            case SidebandPayload::DataItem::DIT_UINT16:
                {
                    partialResult = std::to_string(dataItem.ivalue.u16);
                }
                break;
            case SidebandPayload::DataItem::DIT_UINT32:
                {
                    partialResult = std::to_string(dataItem.ivalue.u32);
                }
                break;
            case SidebandPayload::DataItem::DIT_UINT64:
                {
                    partialResult = std::to_string(dataItem.ivalue.u64);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT:
                {
                    partialResult = std::to_string(dataItem.ivalue.i);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT8:
                {
                    partialResult = std::to_string(dataItem.ivalue.i8);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT16:
                {
                    partialResult = std::to_string(dataItem.ivalue.i16);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT32:
                {
                    partialResult = std::to_string(dataItem.ivalue.i32);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT64:
                {
                    partialResult = std::to_string(dataItem.ivalue.i64);
                }
                break;
            case SidebandPayload::DataItem::DIT_BOOL:
                {
                    partialResult = dataItem.ivalue.b ? "1" : "0";
                }
                break;
            case SidebandPayload::DataItem::DIT_TIMESTAMP:
                {
                    struct tm tm = { 0 };
                    if (m_verbose) {
                            if (gettm(dataItem.ivalue.ts.sec, tm)) {
                                char buf[64] = { 0 };
                                snprintf(buf, sizeof(buf) - 1, "%u-%u-%u %02u:%02u:%02u.%03u",
                                    tm.tm_year + 1900,
                                    tm.tm_mon + 1,
                                    tm.tm_mday,
                                    tm.tm_hour,
                                    tm.tm_min,
                                    tm.tm_sec,
                                    dataItem.ivalue.ts.msec
                                );
                                partialResult = std::string(buf);
                            } else {
                                return false;
                            }
                    }
                    else {
                        uint32_t n = (uint32_t)dataItem.ivalue.ts.sec;
                        partialResult = std::to_string(n);
                    }
                }
                break;
            case SidebandPayload::DataItem::DIT_STR:
                {
                    partialResult = dataItem.svalue;
                }
                break;
                default:
                    {
                        return false;
                    }
                    break;
        }

        appendCRC2(partialResult);
        result += partialResult;
        return ret;
    }

    void SidebandPayloadEncoder::resetCRC()
    {
        m_crc = 0;
    }

    void SidebandPayloadEncoder::appendCRC(uint8_t n)
    {
        appendCRC((const void *)&n, 1);
    }

    void SidebandPayloadEncoder::appendCRC(uint16_t n)
    {
        appendCRC((const void *)&n, 2);
    }

    void SidebandPayloadEncoder::appendCRC(uint32_t n)
    {
        appendCRC((const void *)&n, 4);
    }

    void SidebandPayloadEncoder::appendCRC(uint64_t n)
    {
        appendCRC((const void *)&n, 8);
    }

    void SidebandPayloadEncoder::appendCRC(int8_t n)
    {
        appendCRC((const void *)&n, 1);
    }

    void SidebandPayloadEncoder::appendCRC(int16_t n)
    {
        appendCRC((const void *)&n, 2);
    }

    void SidebandPayloadEncoder::appendCRC(int32_t n)
    {
        appendCRC((const void *)&n, 4);
    }

    void SidebandPayloadEncoder::appendCRC(int64_t n)
    {
        appendCRC((const void *)&n, 8);
    }

    void SidebandPayloadEncoder::appendCRC(bool b)
    {
        uint8_t n = b ? 1 : 0;
        appendCRC((const void *)&n, 1);
    }

    void SidebandPayloadEncoder::appendCRC(const std::string &s)
    {
        appendCRC((const void *)s.c_str(), s.length());
    }

    void SidebandPayloadEncoder::appendCRC(const SidebandPayload::DataItem::Timestamp &t)
    {
        uint32_t n = (uint32_t)t.sec;
        appendCRC((const void *)&n, 4);
    }

    void SidebandPayloadEncoder::appendCRC(const SidebandPayload::DataItem &dataItem)
    {
        switch (dataItem.type) {
            case SidebandPayload::DataItem::DIT_UINT:
                {
                    appendCRC(dataItem.ivalue.u);
                }
                break;
            case SidebandPayload::DataItem::DIT_UINT8:
                {
                    appendCRC(dataItem.ivalue.u8);
                }
                break;
            case SidebandPayload::DataItem::DIT_UINT16:
                {
                    appendCRC(dataItem.ivalue.u16);
                }
                break;
            case SidebandPayload::DataItem::DIT_UINT32:
                {
                    appendCRC(dataItem.ivalue.u32);
                }
                break;
            case SidebandPayload::DataItem::DIT_UINT64:
                {
                    appendCRC(dataItem.ivalue.u64);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT:
                {
                    appendCRC(dataItem.ivalue.i);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT8:
                {
                    appendCRC(dataItem.ivalue.i8);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT16:
                {
                    appendCRC(dataItem.ivalue.i16);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT32:
                {
                    appendCRC(dataItem.ivalue.i32);
                }
                break;
            case SidebandPayload::DataItem::DIT_INT64:
                {
                    appendCRC(dataItem.ivalue.i64);
                }
                break;
            case SidebandPayload::DataItem::DIT_BOOL:
                {
                    appendCRC(dataItem.ivalue.b);
                }
                break;
            case SidebandPayload::DataItem::DIT_TIMESTAMP:
                {
                    appendCRC(dataItem.ivalue.ts);
                }
                break;
            case SidebandPayload::DataItem::DIT_STR:
                {
                    appendCRC(dataItem.svalue);
                }
                break;
            default:
                {
                }
                break;
       }
    }

    void SidebandPayloadEncoder::appendCRC(const void *data, size_t size)
    {
        // TDB
    }

    void SidebandPayloadEncoder::resetCRC2()
    {
        m_crc2 = 0;
    }
    void SidebandPayloadEncoder::appendCRC2(const std::string &s)
    {
        for (char ch : s) {
            uint8_t n = (uint8_t)ch;
            m_crc2 += n;
        }
    }

} // namespace bds {
} // namespace volvo_on_call {
