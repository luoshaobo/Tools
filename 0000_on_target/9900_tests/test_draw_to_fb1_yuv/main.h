#ifndef MAIN_H
#define MAIN_H

#include <QObject>

#define LOG_FULL_FUNC_SIGNATURE __PRETTY_FUNCTION__
#define LOG_BASE_FILE_NAME log_basename(__FILE__)

#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", LOG_BASE_FILE_NAME, __LINE__, LOG_FULL_FUNC_SIGNATURE); } while(0)
#define LOG_GEN_PRINTF(format,...) do { const char *s = NULL; log_printf(s=log_strcat(2, "=== LOG_GEN: [%s: %u: %s] ", format), LOG_BASE_FILE_NAME, __LINE__, LOG_FULL_FUNC_SIGNATURE, ## __VA_ARGS__); if (s != NULL) { free((void *)s); } } while(0)
#define LOG_GEN_PRINTF_NL(format,...) do { const char *s = NULL; log_printf(s=log_strcat(3, "=== LOG_GEN: [%s: %u: %s] ", format, "\n"), LOG_BASE_FILE_NAME, __LINE__, LOG_FULL_FUNC_SIGNATURE, ## __VA_ARGS__); if (s != NULL) { free((void *)s); } } while(0)


extern void log_printf(const char *format, ...);
extern const char *log_basename(const char *path);
extern const char *log_strcat(unsigned int arg_count, ...);

class AnimationScreen : public QObject
{
    Q_OBJECT

public:
    AnimationScreen() : m_timerId(0) {}
    virtual ~AnimationScreen() {}

    void startShowing()
    {
        if (m_timerId != 0) {
            killTimer(m_timerId);
            m_timerId = 0;
        }
        m_timerId = startTimer(100);
    }
    void stopShowing()
    {
        if (m_timerId != 0) {
            killTimer(m_timerId);
            m_timerId = 0;
        }
    }

private:
    virtual void timerEvent(QTimerEvent *event);

private:
    int m_timerId;
};


#endif // MAIN_H
