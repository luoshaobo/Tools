#ifndef BSFWK_COMMON_H
#define BSFWK_COMMON_H

#ifdef WINDOWS
#include <SDKDDKVer.h>
#include <Windows.h>
#else
#include <unistd.h>
#endif // #ifdef WINDOWS

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <future>
#include <random>

#include <threadeventqueue.h>
#include <statemachinemaster.h>
#include <mutex.h>
#include <scopedlock.h>

#include "voc_framework/signals/vpom_signal.h"

namespace bsfwk {

    template<typename T>
    bool IsPtrNotNull(T p)
    {
        return (p != static_cast<T>(0));
    }

    template<typename T>
    bool IsSharedPtrNotNull(std::shared_ptr<T> p)
    {
        return p.operator bool();
    }

    template<typename T1, typename T2>
    T2 DynamicCast(T1 v)
    {
        //return dynamic_cast<T2>(v);
        return static_cast<T2>(v);
    }

} // namespace bsfwk

#endif // #define BSFWK_COMMON_H
