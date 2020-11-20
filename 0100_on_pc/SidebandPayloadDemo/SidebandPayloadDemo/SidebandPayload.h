#ifndef SIDEBAND_PAYLOAD_H
#define SIDEBAND_PAYLOAD_H

#include "BigDataServiceGlobal.h"

namespace volvo_on_call {
namespace bds {

    struct SidebandPayload
    {
        struct DataItem {
            enum Type {
                DIT_UINT,
                DIT_UINT8,
                DIT_UINT16,
                DIT_UINT32,
                DIT_UINT64,
                DIT_INT,
                DIT_INT8,
                DIT_INT16,
                DIT_INT32,
                DIT_INT64,
                DIT_BOOL,
                DIT_TIMESTAMP,
                DIT_STR,
            };
            struct Timestamp {
                static Timestamp CurrentTimestamp();
                Timestamp(time_t a_sec = 0, uint32_t a_msec = 0) : sec(a_sec), msec(a_msec) {}

                time_t sec{0};
                uint32_t msec{0};
            };
            union IValue {
                IValue() {}
            
                unsigned int u{0};
                uint8_t u8;
                uint16_t u16;
                uint32_t u32;
                uint64_t u64;
                int i;
                int8_t i8;
                int16_t i16;
                int32_t i32;
                int64_t i64;
                bool b;
                Timestamp ts;
            };

            DataItem() {}
            DataItem(Type type, IValue ivalue, const std::string &itemName = std::string());
            DataItem(uint8_t value, const std::string &itemName = std::string());
            DataItem(uint16_t value, const std::string &itemName = std::string());
            DataItem(uint32_t value, const std::string &itemName = std::string());
            DataItem(uint64_t value, const std::string &itemName = std::string());
            DataItem(int8_t value, const std::string &itemName = std::string());
            DataItem(int16_t value, const std::string &itemName = std::string());
            DataItem(int32_t value, const std::string &itemName = std::string());
            DataItem(int64_t value, const std::string &itemName = std::string());
            DataItem(bool value, const std::string &itemName = std::string());
            DataItem(Timestamp value, const std::string &itemName = std::string());
            DataItem(const char *value, const std::string &itemName = std::string());
            DataItem(const std::string &value, const std::string &itemName = std::string());

            std::string itemName{};
            Type type{DIT_UINT};
            IValue ivalue;
            std::string svalue{};
        };

        struct PortSample {
            PortSample(const std::vector<DataItem> &items, bool dataValid);
        
            std::vector<DataItem> items{};
            bool dataValid{true};
        };

        struct Port {
            Port() {}
            Port(const std::string &portName, unsigned int maxSampleCount);
            Port(const std::string &portName, unsigned int maxSampleCount, const std::list<PortSample> &samples, bool needToUpload);

            void appendSample(const PortSample &sample);

            std::string portName{};
            size_t maxSampleCount{0};
            std::list<PortSample> samples{};
            bool needToUpload{false};
        };

        struct Head {
            std::string configId{};
            uint16_t version{0};
            DataItem::Timestamp timestamp;
            uint16_t portCount{0};
            uint16_t collectingDuration{0};
            uint16_t uploadingDuration{0};
            uint32_t triggeredTimes{0};
        };

        struct Body {
            void appendTimestamp(DataItem::Timestamp &ts);

            std::map<size_t, Port> ports{};
            std::list<DataItem::Timestamp> timestamps;
            size_t maxSampleCount{0};
        };
    
        Head head{};
        Body body{};
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef SIDEBAND_PAYLOAD_H
