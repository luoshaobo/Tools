#include "BigDataServiceGlobal.h"
#include "SidebandPayload.h"

namespace volvo_on_call {
namespace bds {

    void msleep(unsigned long ms){
#ifdef WINDOWS
        Sleep(ms);
#else
        usleep(ms * 1000);
#endif // #ifdef WINDOWS
    }

    bool gettm(const time_t &t, struct tm &tm)
    {
#ifdef WINDOWS
        errno_t error = gmtime_s(&tm, &t);
        if (error == 0) {
            return true;
        }
#else
        if (gmtime_r((time_t *)&t, &tm) != nullptr) {
            return true;
        }
#endif // #ifdef WINDOWS
        return false;
    }

    std::string CurrentTimeStr()
    {
        std::string timeStr;
        struct tm tm = { 0 };
        SidebandPayload::DataItem::Timestamp ts = SidebandPayload::DataItem::Timestamp::CurrentTimestamp();

        if (gettm(ts.sec, tm)) {
            char buf[64] = { 0 };
            snprintf(buf, sizeof(buf) - 1, "%u-%u-%u %02u:%02u:%02u.%03u",
                tm.tm_year + 1900,
                tm.tm_mon + 1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec,
                ts.msec
            );
            timeStr = std::string(buf);
        }

        return timeStr;
    }

} // namespace bds {
} // namespace volvo_on_call {
