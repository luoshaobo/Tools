#ifndef BSFWK_GLOBAL_H
#define BSFWK_GLOBAL_H

#include "bsfwk_Common.h"
#include "bsfwk_Types.h"
#include "bsfwk_IServiceEntity.h"
#include "bsfwk_IJobEntity.h"
#include "bsfwk_IServiceStateMachine.h"
#include "bsfwk_IJobStateMachine.h"
#include "bsfwk_IEntityFactory.h"
#include "bsfwk_ServiceEntityBase.h"
#include "bsfwk_StatemachineServiceEntity.h"
#include "bsfwk_JobEntityBase.h"
#include "bsfwk_StatemachineJobEntity.h"
#include "bsfwk_EntityFactoryBase.h"
#include "bsfwk_BusinessServiceStateMachine.h"
#include "bsfwk_BusinessJobStateMachine.h"
#include "bsfwk_BusinessService.h"
#include "bsfwk_BusinessServer.h"

#ifdef WINDOWS
#define BSFWK_FUNC                      GetNiceFuncName(std::string(__FUNCTION__)).c_str()
#else
#define BSFWK_FUNC                      GetNiceFuncName(std::string(__PRETTY_FUNCTION__)).c_str()
#endif

#ifndef TCAM_TARGET
namespace bsfwk { void log_printf(const char *pFormat, ...); }
#define BSFWK_SLOG_DEBUG(format,...) do { if (LogLevel_Debug >= m_logLevel) { bsfwk::log_printf(format, ## __VA_ARGS__); } } while(false)
#define BSFWK_SLOG_INFO(format,...) do { if (LogLevel_Info >= m_logLevel) { bsfwk::log_printf(format, ## __VA_ARGS__); } } while(false)
#define BSFWK_SLOG_WARN(format,...) do { if (LogLevel_Warning >= m_logLevel) { bsfwk::log_printf(format, ## __VA_ARGS__); } } while(false)
#define BSFWK_SLOG_ERROR(format,...) do { if (LogLevel_Error >= m_logLevel) { bsfwk::log_printf(format, ## __VA_ARGS__); } } while(false)
#else
#include "dlt.h"
DLT_IMPORT_CONTEXT(dlt_bsfwk);
#define BSFWK_SLOG_DEBUG(format,...) do { if (LogLevel_Debug >= m_logLevel) { DLT_LOG_STRINGF(dlt_bsfwk, DLT_LOG_INFO, format, ## __VA_ARGS__); } } while(false)
#define BSFWK_SLOG_INFO(format,...) do { if (LogLevel_Info >= m_logLevel) { DLT_LOG_STRINGF(dlt_bsfwk, DLT_LOG_INFO, format, ## __VA_ARGS__); } } while(false)
#define BSFWK_SLOG_WARN(format,...) do { if (LogLevel_Warning >= m_logLevel) { DLT_LOG_STRINGF(dlt_bsfwk, DLT_LOG_INFO, format, ## __VA_ARGS__); } } while(false)
#define BSFWK_SLOG_ERROR(format,...) do { if (LogLevel_Error >= m_logLevel) { DLT_LOG_STRINGF(dlt_bsfwk, DLT_LOG_INFO, format, ## __VA_ARGS__); } } while(false)
#endif // #ifndef ON_TARGET

namespace bsfwk {

    const std::string GetNiceFuncName(const std::string& prettyFuncon);
    const char *GetLogLevelStr(const LogLevel logLevel);

} // namespace bsfwk

#endif // #ifndef BSFWK_GLOBAL_H
