#ifndef MISC_H
#define MISC_H

#ifdef WINDOWS
#include <SDKDDKVer.h>
#include <Windows.h>
#else
#include <unistd.h>
#endif // #ifdef WINDOWS

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <string>
#include <memory>
#include <functional>
#include <future>
#include <random>

#ifndef BSFWK_UNUSED
#define BSFWK_UNUSED(var)         (&(var))
#endif // #ifndef BSFWK_UNUSED

#ifdef WINDOWS
#define BSFWK_FUNCINFO            __FUNCTION__ "()"
#else
#define BSFWK_FUNCINFO            __PRETTY_FUNCTION__
#endif

#define BSFWK_FILEINFO            bsfwk::log_basename(__FILE__)

#define BSFWK_LOG_PRINTF(format,...) do { bsfwk::log_printf(format, ## __VA_ARGS__); } while(false)
#define BSFWK_LOG_GEN() do { bsfwk::log_printf("=== LOG_GEN: [%s: %4u: %s]\n", BSFWK_FILEINFO, __LINE__, BSFWK_FUNCINFO); } while(false)
#define BSFWK_LOG_GEN_PRINTF(format,...) do { bsfwk::log_printf((std::string("=== LOG_GEN: [%s: %4u: %s] ")+std::string(format)).c_str(), BSFWK_FILEINFO, __LINE__, BSFWK_FUNCINFO, ## __VA_ARGS__); } while(false)

#define BSFWK_LOG_JOB_GEN_PRINTF(format,...) do { bsfwk::log_printf((std::string("=== LOG_GEN: [%s: %4u: %s][%s] ")+std::string(format)).c_str(), BSFWK_FILEINFO, __LINE__, BSFWK_FUNCINFO, GetJobName().c_str(), ## __VA_ARGS__); } while(false)
#define BSFWK_LOG_SERVICE_GEN_PRINTF(format,...) do { bsfwk::log_printf((std::string("=== LOG_GEN: [%s: %4u: %s][%s] ")+std::string(format)).c_str(), BSFWK_FILEINFO, __LINE__, BSFWK_FUNCINFO, GetServiceName().c_str(), ## __VA_ARGS__); } while(false)

#define BSFWK_LOG_JOBSM_PRINTF(level,format,...) do { bsfwk::log_printf((std::string("=== LOG_GEN: [%s][%s] ")+std::string(format)).c_str(), GetStateMachineName().c_str(), level, ## __VA_ARGS__); } while(false)
#define BSFWK_LOG_SERVICESM_PRINTF(level, format,...) do { bsfwk::log_printf((std::string("=== LOG_GEN: [%s][%s] ")+std::string(format)).c_str(), GetStateMachineName().c_str(), level, ## __VA_ARGS__); } while(false)

namespace bsfwk {

    template <typename F, typename... Args>
    auto really_async(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using _Ret = typename std::result_of<F(Args...)>::type;
        auto _func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        std::packaged_task<_Ret()> tsk(std::move(_func));
        auto _fut = tsk.get_future();
        std::thread thd(std::move(tsk));
        thd.detach();
        return _fut;
    }

    void log_printf(const char *pFormat, ...);
    const char *log_basename(const char *path);

    std::string FormatStr(const char *pFormat, ...);
    void FormatStr(std::string &strRet, const char *pFormat, ...);

    void msleep(unsigned long msec);

} // namespace bsfwk

#endif // MISC_H
