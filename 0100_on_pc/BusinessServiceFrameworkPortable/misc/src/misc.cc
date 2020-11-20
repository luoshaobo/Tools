#ifndef WINDOWS
#include <sys/time.h>
#endif // #ifdef WINDOWS

#include "misc.h"

#ifdef TCAM_TARGET
#include "dlt/dlt.h"
DLT_IMPORT_CONTEXT(dlt_voc);
#endif // #ifdef TCAM_TARGET

namespace bsfwk {

#ifdef TCAM_TARGET
    const char * const LOG_FILE_PATH = "/log.txt";
#else
    const char * const LOG_FILE_PATH = "./log.txt";
#endif // #ifdef TCAM_TARGET

#ifdef WINDOWS
    const char LOG_FILE_SEP_CHAR = '\\';
#else
    const char LOG_FILE_SEP_CHAR = '/';
#endif // #ifdef WINDOWS

    const unsigned int MAX_STR_LEN = 1024;

    class FileLogger
    {
    public:
        static FileLogger &GetInstance()
        {
            static FileLogger fileLogger(LOG_FILE_PATH);
            return fileLogger;
        }

    public:
        FileLogger(const std::string &logFilePath)
            : m_logFilePath(logFilePath), m_pFile(NULL)
        {
            m_pFile = fopen(m_logFilePath.c_str(), "w+");
        }

        ~FileLogger()
        {
            if (m_pFile != NULL) {
                fclose(m_pFile);
                m_pFile = NULL;
            }
        }

        void WriteLogLine(const std::string &line)
        {
            if (m_pFile != NULL) {
                std::string sCurrentTimeStr = GetCurrentTimeStr();
                fwrite(sCurrentTimeStr.c_str(), sCurrentTimeStr.length(), 1, m_pFile);
                fwrite(line.c_str(), line.length(), 1, m_pFile);
                fflush(m_pFile);
            }
        }

    private:
        std::string GetCurrentTimeStr()
        {
            std::string sTimeStr;

#ifdef WINDOWS
            DWORD t = ::GetTickCount();
            FormatStr(sTimeStr, "[%lu.%03u] ", (t / 1000), (t % 1000));
#else
            struct timeval tv {};
            gettimeofday(&tv, NULL);
            FormatStr(sTimeStr, "[%lu.%03u] ", (tv.tv_sec), (tv.tv_usec / 1000));
#endif

            return sTimeStr;
        }

    private:
        std::string m_logFilePath;
        FILE *m_pFile;
    };

    class StdoutLogger
    {
    public:
        static StdoutLogger &GetInstance()
        {
            static StdoutLogger stdoutLogger;
            return stdoutLogger;
        }

    public:
        StdoutLogger()
            : m_pFile(stdout)
        {
        }

        ~StdoutLogger()
        {
        }

        void WriteLogLine(const std::string &line)
        {
            if (m_pFile != NULL) {
                std::string sCurrentTimeStr = GetCurrentTimeStr();
                fwrite(sCurrentTimeStr.c_str(), sCurrentTimeStr.length(), 1, m_pFile);
                fwrite(line.c_str(), line.length(), 1, m_pFile);
                fflush(m_pFile);
            }
        }

    private:
        std::string GetCurrentTimeStr()
        {
            std::string sTimeStr;

#ifdef WINDOWS
            DWORD t = ::GetTickCount();
            FormatStr(sTimeStr, "[%lu.%03u] ", (t / 1000), (t % 1000));
#else
            struct timeval tv {};
            gettimeofday(&tv, NULL);
            FormatStr(sTimeStr, "[%lu.%03u] ", (tv.tv_sec), (tv.tv_usec / 1000));
#endif

            return sTimeStr;
        }

    private:
        FILE * m_pFile;
    };

    const char *log_basename(const char *path)
    {
        const char *pRet = path;

        if (path != NULL) {
            char *p = strrchr(const_cast<char *>(path), LOG_FILE_SEP_CHAR);
            if (p != NULL) {
                pRet = (const char *)(p + 1);
            }
        }

        return pRet;
    }

    void log_printf(const char *pFormat, ...)
    {
        int nError = 0;
        va_list args;
        int len = 0;
        char *buffer = NULL;

        if (nError == 0) {
            if (pFormat == NULL) {
                nError = 1;
            }
        }

        if (nError == 0) {
            va_start(args, pFormat);
            len = vsnprintf(NULL, 0, pFormat, args);
            if (len <= 0) {
                len = 1024;
            }
            buffer = static_cast<char *>(malloc((len + 1) * sizeof(char)));
            if (buffer == NULL) {
                nError = -1;
            }
            va_end(args);
        }

        if (nError == 0) {
            va_start(args, pFormat);
            vsnprintf(buffer, len + 1, pFormat, args);
            buffer[len] = '\0';
            //printf(buffer);                                                       // to be changed to any function which can output a string
            StdoutLogger::GetInstance().WriteLogLine(buffer);
            FileLogger::GetInstance().WriteLogLine(buffer);
#ifdef TCAM_TARGET
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, (const char *)buffer, 0);
#endif // #ifdef TCAM_TARGET
            va_end(args);
        }

        if (buffer != NULL) {
            free(buffer);
            buffer = NULL;
        }

        return;
    }

    std::string FormatStr(const char *pFormat, ...)
    {
        std::string strRet;
        va_list args;
        int len;
        char *buffer;

        va_start(args, pFormat);
        len = vsnprintf(NULL, 0, pFormat, args);
        if (len <= 0) {
            len = 1024;
        }
        va_end(args);

        va_start(args, pFormat);
        buffer = static_cast<char *>(malloc((len + 1) * sizeof(char)));
        if (buffer != NULL) {
            vsnprintf(buffer, len + 1, pFormat, args);
            buffer[len] = '\0';
            strRet = std::string(buffer);
            free(buffer);
        }
        else {
            strRet = std::string("");
        }
        va_end(args);

        return strRet;
    }

    void FormatStr(std::string &strRet, const char *pFormat, ...)
    {
        va_list args;
        int len;
        char *buffer;

        va_start(args, pFormat);
        len = vsnprintf(NULL, 0, pFormat, args);
        if (len <= 0) {
            len = 1024;
        }
        va_end(args);

        va_start(args, pFormat);
        buffer = static_cast<char *>(malloc((len + 1) * sizeof(char)));
        if (buffer != NULL) {
            vsnprintf(buffer, len + 1, pFormat, args);
            buffer[len] = '\0';
            strRet = std::string(buffer);
            free(buffer);
        }
        else {
            strRet = std::string("");
        }
        va_end(args);
    }

    void msleep(unsigned long msec)
    {
#ifdef WINDOWS
        ::Sleep(msec);
#else
        ::usleep(msec * 1000);
#endif // #ifdef WINDOWS
    }

} // namespace bsfwk {

