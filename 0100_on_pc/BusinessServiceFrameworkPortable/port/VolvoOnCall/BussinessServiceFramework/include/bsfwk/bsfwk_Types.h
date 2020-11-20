#ifndef BSFWK_TYPES_H
#define BSFWK_TYPES_H

#include "bsfwk_Common.h"

namespace bsfwk {

    enum LogLevel {
        LogLevel_Verbose,
        LogLevel_Debug,
        LogLevel_Info,
        LogLevel_Warning,
        LogLevel_Error,
        LogLevel_Critical,
        LogLevel_Fatal,
        LogLevel_Off,
    };

    enum ServiceState
    {
        ServiceState_Idle,
        ServiceState_Began,
        ServiceState_Running,
        ServiceState_FailureRetryDelaying,
        ServiceState_Completed,
        ServiceState_Failed,
        ServiceState_Timedout,
        ServiceState_Cancelled,
    };

    enum JobState
    {
        JobState_Idle,
        JobState_Delaying,
        JobState_Began,
        JobState_Running,
        JobState_FailureRetryDelaying,
        JobState_Completed,
        JobState_Failed,
        JobState_Timedout,
        JobState_Cancelled,
    };

    enum StateMachineState
    {
        StateMachineState_Running,
        StateMachineState_Stopped,
        StateMachineState_Paused,
    };

    class BSEventDataBase
    {
    public:
        virtual ~BSEventDataBase() {}
    };

    class BSEvent : public Event
    {
    public:
        BSEvent(const uint32_t nMasterIndex, const uint32_t nStatemachineIndex, const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data = 0)
            : Event(), m_nMasterIndex(nMasterIndex), m_nStatemachineIndex(nStatemachineIndex), m_nEventIndex(nEventIndex), m_data(data)
        {
        }
        virtual ~BSEvent()
        {
        }

    public:
        uint32_t GetMasterIndex() const { return m_nMasterIndex; }
        uint32_t GetStatemachineIndex() const { return m_nStatemachineIndex; }
        uint32_t GetEventIndex() const { return m_nEventIndex; }
        std::shared_ptr<BSEventDataBase> GetData() const { return m_data; }

    public:
        // {{{ Event
        virtual const char *getName() const
        {
            return "BSEvent";
        }

        virtual BSEvent *clone() const
        {
            return new BSEvent(m_nMasterIndex, m_nStatemachineIndex, m_nEventIndex, m_data);
        }
        // Event }}}

    private:
        uint32_t m_nMasterIndex;
        uint32_t m_nStatemachineIndex;
        uint32_t m_nEventIndex;
        std::shared_ptr<BSEventDataBase> m_data;
    };

    class BSSysEventDataBase
    {
    public:
        virtual ~BSSysEventDataBase() {}
    };

    class BSSysEvent : public Event
    {
    public:
        BSSysEvent(const uint32_t nMasterIndex, const uint32_t nStatemachineIndex, const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data = 0)
            : Event(), m_nMasterIndex(nMasterIndex), m_nStatemachineIndex(nStatemachineIndex), m_nEventIndex(nEventIndex), m_data(data)
        {
        }
        virtual ~BSSysEvent()
        {
        }

    public:
        uint32_t GetMasterIndex() const { return m_nMasterIndex; }
        uint32_t GetStatemachineIndex() const { return m_nStatemachineIndex; }
        uint32_t GetEventIndex() const { return m_nEventIndex; }
        std::shared_ptr<BSSysEventDataBase> GetData() const { return m_data; }

    public:
        // {{{ Event
        virtual const char *getName() const
        {
            return "BSSysEvent";
        }

        virtual BSSysEvent *clone() const
        {
            return new BSSysEvent(m_nMasterIndex, m_nStatemachineIndex, m_nEventIndex, m_data);
        }
        // Event }}}

    private:
        uint32_t m_nMasterIndex;
        uint32_t m_nStatemachineIndex;
        uint32_t m_nEventIndex;
        std::shared_ptr<BSSysEventDataBase> m_data;
    };

    class BSSignalEventDataBase
    {
    public:
        BSSignalEventDataBase(const std::shared_ptr<fsm::Signal> oSignal = 0) 
            : m_oSignal(oSignal)
        {
        }
        virtual ~BSSignalEventDataBase()
        {
        }

        const std::shared_ptr<fsm::Signal> GetSignal() const { return m_oSignal; }

    private:
        std::shared_ptr<fsm::Signal> m_oSignal;
    };

    class BSSignalEvent : public Event
    {
    public:
        BSSignalEvent(const std::shared_ptr<BSSignalEventDataBase> data)
            : Event(), m_data(data)
        {
        }
        virtual ~BSSignalEvent()
        {
        }

    public:
        const std::shared_ptr<BSSignalEventDataBase> GetData() const { return m_data; }

    public:
        // {{{ Event
        virtual const char *getName() const
        {
            return "BSSignalEvent";
        }

        virtual BSSignalEvent *clone() const
        {
            return new BSSignalEvent(m_data);
        }
        // Event }}}

    private:
        std::shared_ptr<BSSignalEventDataBase> m_data;
    };

    typedef std::function<void ()> AsyncCallback;

    class BSAsyncCallEvent : public Event
    {
    public:
        BSAsyncCallEvent(AsyncCallback asyncCallback)
            : Event(), m_asyncCallback(asyncCallback)
        {
        }
        virtual ~BSAsyncCallEvent()
        {
        }

    public:
        const AsyncCallback GetCallback() const { return m_asyncCallback; }

    public:
        // {{{ Event
        virtual const char *getName() const
        {
            return "BSAsyncCallEvent";
        }

        virtual BSAsyncCallEvent *clone() const
        {
            return new BSAsyncCallEvent(m_asyncCallback);
        }
        // Event }}}

    private:
        AsyncCallback m_asyncCallback;
    };

    class RetryConfig
    {
    public:
        class TimeoutRetryInfo
        {
        public:
            TimeoutRetryInfo()
                : m_bEnabled(false), m_nTimeout(0), m_nRetryCount(0) {}
            TimeoutRetryInfo(const TimeoutRetryInfo &other)
                : m_bEnabled(other.m_bEnabled), m_nTimeout(other.m_nTimeout), m_nRetryCount(other.m_nRetryCount) {}
            TimeoutRetryInfo & operator =(const TimeoutRetryInfo &other) {
                m_bEnabled = other.m_bEnabled;
                m_nTimeout = other.m_nTimeout;
                m_nRetryCount = other.m_nRetryCount;
                return *this;
            }
            virtual ~TimeoutRetryInfo() {}

            bool GetEnabled() const { return m_bEnabled; }
            void SetEnabled(const bool bEnabled) { m_bEnabled = bEnabled; }
            uint32_t GetTimeout() const { return m_nTimeout; }
            void SetTimeout(const uint32_t nTimeout) { m_nTimeout = nTimeout; }
            uint32_t GetRetryCount() const { return m_nRetryCount; }
            void SetRetryCount(const uint32_t nRetryCount) { m_nRetryCount = nRetryCount; }

        private:
            bool m_bEnabled;                    // NOTE: timeout retry is enabled or not.
            uint32_t m_nTimeout;            // NOTE: 0 for timed out immediately.
            uint32_t m_nRetryCount;         // NOTE: 0 for unlimited retry count.
        };

        class FailureRetryInfo
        {
        public:
            FailureRetryInfo()
                : m_bEnabled(false), m_nRetryCount(0), m_nDelayTime(0) {}
            FailureRetryInfo(const FailureRetryInfo &other)
                : m_bEnabled(other.m_bEnabled), m_nRetryCount(other.m_nRetryCount), m_nDelayTime(other.m_nDelayTime) {}
            FailureRetryInfo & operator =(const FailureRetryInfo &other) {
                m_bEnabled = other.m_bEnabled;
                m_nRetryCount = other.m_nRetryCount;
                m_nDelayTime = other.m_nDelayTime;
                return *this;
            }
            virtual ~FailureRetryInfo() {}

            bool GetEnabled() const { return m_bEnabled; }
            void SetEnabled(const bool bEnabled) { m_bEnabled = bEnabled; }
            uint32_t GetRetryCount() const { return m_nRetryCount; }
            void SetRetryCount(const uint32_t nRetryCount) { m_nRetryCount = nRetryCount; }
            uint32_t GetDelayTime() const { return m_nDelayTime; }
            void SetDelayTime(const uint32_t nDelayTime) { m_nDelayTime = nDelayTime; }

        private:
            bool m_bEnabled;                    // NOTE: failure retry is enabled or not.
            uint32_t m_nRetryCount;         // NOTE: 0 for unlimited retry count.
            uint32_t m_nDelayTime;          // NOTE: 0 for no delay.
        };

    public:
        RetryConfig()
            : m_timeoutRetryInfo(), m_failureRetryInfo(), m_nDuration(0) {}
        RetryConfig(const RetryConfig &other)
            : m_timeoutRetryInfo(other.m_timeoutRetryInfo), m_failureRetryInfo(other.m_failureRetryInfo), m_nDuration(other.m_nDuration) {}
        RetryConfig & operator =(const RetryConfig &other) {
            m_timeoutRetryInfo = other.m_timeoutRetryInfo;
            m_failureRetryInfo = other.m_failureRetryInfo;
            m_nDuration = other.m_nDuration;
            return *this;
        }
        virtual ~RetryConfig() {}

        bool DurationEnabled() const { return (m_nDuration != 0); }

        TimeoutRetryInfo &GetTimeoutRetryInfo() { return m_timeoutRetryInfo; }
        const TimeoutRetryInfo &GetTimeoutRetryInfo() const { return m_timeoutRetryInfo; }
        FailureRetryInfo &GetFailureRetryInfo() { return m_failureRetryInfo; }
        const FailureRetryInfo &GetFailureRetryInfo() const { return m_failureRetryInfo; }
        uint32_t GetDuration() const { return m_nDuration; }
        void SetDuration(const uint32_t nDuration) { m_nDuration = nDuration; }

    private:
        TimeoutRetryInfo m_timeoutRetryInfo;
        FailureRetryInfo m_failureRetryInfo;
        uint32_t m_nDuration;               // NOTE: 0 for unlimited duration.
    };

    class DelayConfig
    {
    public:
        DelayConfig()
            : m_bEnabled(false), m_nDelayTime(0) {}
        DelayConfig(const DelayConfig &other) : m_bEnabled(other.m_bEnabled), m_nDelayTime(other.m_nDelayTime) {}
        DelayConfig & operator =(const DelayConfig &other) {
            m_bEnabled = other.m_bEnabled;
            m_nDelayTime = other.m_nDelayTime;
            return *this;
        }
        virtual ~DelayConfig() {}

        bool GetEnabled() const { return m_bEnabled; }
        void SetEnabled(const bool bEnabled) { m_bEnabled = bEnabled; }
        uint32_t GetDelayTime() const { return m_nDelayTime; }
        void SetDelayTime(const uint32_t nDelayTime) { m_nDelayTime = nDelayTime; }

    private:
        bool m_bEnabled;                        // NOTE: job delay is enabled or not.
        uint32_t m_nDelayTime;              // NOTE: 0 for no delay.
    };

    class CyclicalRequestConfig
    {
    public:
        CyclicalRequestConfig()
            : m_bEnabled(false), m_nInterval(0) {}
        CyclicalRequestConfig(const CyclicalRequestConfig &other)
            : m_bEnabled(other.m_bEnabled), m_nInterval(other.m_nInterval) {}
        CyclicalRequestConfig & operator =(const CyclicalRequestConfig &other) {
            m_bEnabled = other.m_bEnabled;
            m_nInterval = other.m_nInterval;
            return *this;
        }
        virtual ~CyclicalRequestConfig() {}

        bool GetEnabled() const { return m_bEnabled; }
        void SetEnabled(const bool bEnabled) { m_bEnabled = bEnabled; }
        uint32_t GetInterval() const { return m_nInterval; }
        void SetInterval(const uint32_t nInterval) { m_nInterval = nInterval; }

    private:
        bool m_bEnabled;                        // NOTE: cyclical service request is enabled or not.
        uint32_t m_nInterval;               // NOTE: if 0, cyclical service request will not be started.
    };

    class ServiceRequestBase
    {
    public:
        virtual ~ServiceRequestBase() {}
    };

    class ServiceSignalRequest : public ServiceRequestBase
    {
    public:
        ServiceSignalRequest(std::shared_ptr<fsm::Signal> oSignal)
            : ServiceRequestBase(), m_oSignal(oSignal)
        {
        }
        virtual ~ServiceSignalRequest()
        {
        }

        const std::shared_ptr<fsm::Signal> GetSignal() const { return m_oSignal; }

    private:
        std::shared_ptr<fsm::Signal> m_oSignal;
    };

    class ServiceCyclicalRequest : public ServiceRequestBase
    {
    public:
        ServiceCyclicalRequest()
            : ServiceRequestBase()
        {
        }
        virtual ~ServiceCyclicalRequest()
        {
        }
    };

    class BSTimer;

    class IStatemachineExt : public IStatemachine
    {
        friend class BSTimer;
    public:
        IStatemachineExt(
            ITimerMaster<IStatemachine, Event> *pITimerMaster,
            IStatemachineMaster *pIStatemachineMaster,
            const StatemachineId smid, 
            const StatemachineType type)
            : IStatemachine(pITimerMaster, pIStatemachineMaster, smid, type)
        {
        }
        virtual ~IStatemachineExt()
        {
        }
    };

    class BSTimer
    {
    public:
        BSTimer()
            : m_timer()
            , m_pStatemachine(0)
        {
        }

        BSTimer(const Timer &other, IStatemachineExt *pStatemachine)
            : m_timer(other)
            , m_pStatemachine(pStatemachine)
        {
        }

        ~BSTimer()
        {
        }

        BSTimer &operator =(const BSTimer &other)
        {
            Reset();
            m_timer = other.m_timer;
            m_pStatemachine = other.m_pStatemachine;

            return *this;
        }

        void Reset()
        {
            if (IsPtrNotNull(m_pStatemachine)) {
                if (m_timer.getId() != Timer::InvalidTimerId) {
                    bool bRet = m_pStatemachine->stopTimer(m_timer.getId());
                    if (bRet) {
                        // do nothing, just to get rid of SCC warning
                    }
                }
            }
            m_timer = Timer();
            m_pStatemachine = NULL;
        }

    private:
        Timer m_timer;
        IStatemachineExt *m_pStatemachine;
    };
} // namespace bsfwk {

#endif // #ifndef BSFWK_TYPES_H
