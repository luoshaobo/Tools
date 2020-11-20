#include "bsfwk_Global.h"

namespace bsfwk {

    const std::string GetNiceFuncName(const std::string& prettyFuncon)
    {
        std::string sFunc = prettyFuncon;

        {
            auto pos = sFunc.find('(');
            if (pos != std::string::npos) {
                sFunc.erase(sFunc.begin() + pos, sFunc.end());
            }
        }

        {
            auto pos = sFunc.rfind("::");
            if (pos != std::string::npos) {
                auto pos2 = sFunc.rfind("::", pos - 2);
                if (pos2 != std::string::npos) {
                    sFunc = sFunc.substr(pos2 + 2);
                } else {
                    auto pos3 = sFunc.rfind(" ", pos - 1);
                    if (pos3 != std::string::npos) {
                        sFunc = sFunc.substr(pos3 + 1);
                    }
                }
            } else {
                auto pos3 = sFunc.rfind(" ", std::string::npos);
                if (pos3 != std::string::npos) {
                    sFunc = sFunc.substr(pos3 + 1);
                }
            }
        }

        sFunc += std::string("()");
        return sFunc;
    }

    const char *GetLogLevelStr(const LogLevel logLevel)
    {
        const char *logLevelStr = "Unknown";

        switch (logLevel) {
            case LogLevel_Verbose:
                {
                    logLevelStr = "Verbose";
                }
                break;
            case LogLevel_Debug:
                {
                    logLevelStr = "Debug";
                }
                break;
            case LogLevel_Info:
                {
                    logLevelStr = "Info";
                }
                break;
            case LogLevel_Warning:
                {
                    logLevelStr = "Warning";
                }
                break;
            case LogLevel_Error:
                {
                    logLevelStr = "Error";
                }
                break;
            case LogLevel_Critical:
                {
                    logLevelStr = "Critical";
                }
                break;
            case LogLevel_Fatal:
                {
                    logLevelStr = "Fatal";
                }
                break;
            default:
                {
                    // do nothing
                }
                break;
        }

        return logLevelStr;
    }

} // namespace bsfwk
