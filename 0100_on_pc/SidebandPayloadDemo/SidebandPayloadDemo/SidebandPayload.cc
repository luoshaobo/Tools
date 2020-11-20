#include "BigDataServiceGlobal.h"
#include "SidebandPayload.h"

namespace volvo_on_call {
namespace bds {

    SidebandPayload::DataItem::DataItem(Type type, IValue ivalue, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = type;
        this->ivalue = ivalue;
    }

    SidebandPayload::DataItem::DataItem(uint8_t value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_UINT8;
        this->ivalue.u8 = value;
    }

    SidebandPayload::DataItem::DataItem(uint16_t value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_UINT16;
        this->ivalue.u16 = value;
    }

    SidebandPayload::DataItem::DataItem(uint32_t value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_UINT32;
        this->ivalue.u32 = value;
    }

    SidebandPayload::DataItem::DataItem(uint64_t value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_UINT64;
        this->ivalue.u64 = value;
    }

    SidebandPayload::DataItem::DataItem(int8_t value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_INT8;
        this->ivalue.i8 = value;
    }

    SidebandPayload::DataItem::DataItem(int16_t value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_INT16;
        this->ivalue.i16 = value;
    }

    SidebandPayload::DataItem::DataItem(int32_t value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_INT32;
        this->ivalue.i32 = value;
    }

    SidebandPayload::DataItem::DataItem(int64_t value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_INT64;
        this->ivalue.i64 = value;
    }

    SidebandPayload::DataItem::DataItem(bool value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_BOOL;
        this->ivalue.b = value;
    }

    SidebandPayload::DataItem::DataItem(Timestamp value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_TIMESTAMP;
        this->ivalue.ts = value;
    }

    SidebandPayload::DataItem::DataItem(const char *value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_STR;
        this->svalue = value;
    }

    SidebandPayload::DataItem::DataItem(const std::string &value, const std::string &itemName /*= std::string()*/)
    {
        this->itemName = itemName;
        this->type = DIT_STR;
        this->svalue = value;
    }

    SidebandPayload::PortSample::PortSample(const std::vector<DataItem> &items, bool dataValid)
    {
        this->items = items;
        this->dataValid = dataValid;
    }

    SidebandPayload::Port::Port(const std::string &portName, unsigned int maxSampleCount)
    {
        this->portName = portName;
        this->maxSampleCount = maxSampleCount;
    }

    SidebandPayload::Port::Port(const std::string &portName, unsigned int maxSampleCount, const std::list<PortSample> &samples, bool needToUpload)
    {
        this->portName = portName;
        this->maxSampleCount = maxSampleCount;
        this->samples = samples;
        this->needToUpload = needToUpload;
    }

     SidebandPayload::DataItem::Timestamp SidebandPayload::DataItem::Timestamp::CurrentTimestamp()
     {
        Timestamp ts;
        ts.sec = ::time(nullptr);
        return ts;
     }

    void SidebandPayload::Port::appendSample(const PortSample &sample)
    {
        if (samples.size() >= maxSampleCount && samples.size() > 0) {
            samples.pop_front();
        }
        samples.push_back(sample);
    }

    void SidebandPayload::Body::appendTimestamp(DataItem::Timestamp &ts)
    {
        if (timestamps.size() >= maxSampleCount && timestamps.size() > 0) {
            timestamps.pop_front();
        }
        timestamps.push_back(ts);
    }

} // namespace bds {
} // namespace volvo_on_call {
