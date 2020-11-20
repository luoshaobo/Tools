#ifndef BIG_DATA_SERVICE_GLOBAL_H
#define BIG_DATA_SERVICE_GLOBAL_H

#define WINDOWS
#define USE_GEN_FILES

#ifdef WINDOWS
#include <SDKDDKVer.h>
#include <windows.h>
#else
#include <unistd.h>
#endif // #ifdef WINDOWS

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <limits>
#include <functional>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <sstream>
#include <pugixml/pugixml.hpp>

namespace volvo_on_call {
namespace bds {

    void msleep(unsigned long ms);
    bool gettm(const time_t &t, struct tm &tm);
    std::string CurrentTimeStr();

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef BIG_DATA_SERVICE_GLOBAL_H
