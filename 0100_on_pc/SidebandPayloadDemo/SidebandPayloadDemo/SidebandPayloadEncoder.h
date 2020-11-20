#ifndef SIDEBAND_PAYLOAD_ENCODER_H
#define SIDEBAND_PAYLOAD_ENCODER_H

#include "BigDataServiceGlobal.h"
#include "SidebandPayload.h"

namespace volvo_on_call {
namespace bds {

    class SidebandPayloadEncoder
    {
    public:
        SidebandPayloadEncoder(SidebandPayload &payload, const std::vector<size_t> &selectedPorts);
        ~SidebandPayloadEncoder();

    public:
        void setVerbose(bool verbose);

    public:
        bool encode(std::string &result);

    private:
        bool encodeHead(std::string &result);
        bool encodeBody(std::string &result);

        bool encodePort(const SidebandPayload::Port &port, size_t sampleIndex, std::string &result);
        bool encodePortSample(const SidebandPayload::PortSample &sample, std::string &result);
        bool encodeDataItem(const SidebandPayload::DataItem &dataItem, std::string &result, bool toAppendCrc2 = false);

        void resetCRC();
        void appendCRC(uint8_t n);
        void appendCRC(uint16_t n);
        void appendCRC(uint32_t n);
        void appendCRC(uint64_t n);
        void appendCRC(int8_t n);
        void appendCRC(int16_t n);
        void appendCRC(int32_t n);
        void appendCRC(int64_t n);
        void appendCRC(bool b);
        void appendCRC(const std::string &s);
        void appendCRC(const SidebandPayload::DataItem::Timestamp &t);
        void appendCRC(const SidebandPayload::DataItem &dataItem);
        void appendCRC(const void *data, size_t size);

        void resetCRC2();
        void appendCRC2(const std::string &s);

    private:
        SidebandPayload &m_payload;
        const std::vector<size_t> &m_selectedPorts;
        uint32_t m_crc;
        uint16_t m_crc2;
        bool m_verbose;
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef SIDEBAND_PAYLOAD_ENCODER_H
