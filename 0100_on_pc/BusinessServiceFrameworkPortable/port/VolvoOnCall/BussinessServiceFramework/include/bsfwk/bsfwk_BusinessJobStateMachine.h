#ifndef BSFWK_BUSINESS_JOB_STATE_MACHINE_H
#define BSFWK_BUSINESS_JOB_STATE_MACHINE_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class BusinessService;

    class BusinessJobStateMachine : public IStatemachineExt, public IJobStateMachine
    {
        friend class BusinessService;

    private:
        enum {
            JSMEI_JOB_BEGIN_DELAY_TIMER,
            JSMEI_TIMEOUT_TIMER,
            JSMEI_FAILURE_RETRY_DELAY_TIMER,
            JSMEI_RETRY_DURATION_TIMER,
        };

    public:
        BusinessJobStateMachine(
            IEntityFactory *pEntityFactory, 
            IServiceStateMachine *pServiceStateMachine, 
            const uint32_t nMasterIndex, 
            const uint32_t nStatemachineIndex, 
            const std::string &stateMachineName,
            ITimerMaster<IStatemachine, Event> *pITimerMaster,
            IStatemachineMaster *pStatemachineMaster,
            const StatemachineId smid, 
            const StatemachineType type);

        virtual ~BusinessJobStateMachine();

    public:
        // {{{ IJobStateMachine
        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> oSignal);
        virtual void Start();
        virtual void Stop();
        virtual void Reset();
        virtual void PostEvent(const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data = 0);
        virtual BSTimer StartTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data = 0);
        virtual void StopTimer(BSTimer &timer);
        virtual void StartNewJob();
        virtual void CancelCurrentJob();
        virtual void OnJobFinished(const JobState &jobState);
        virtual const JobState GetJobState();
        virtual const bool GetJobIgnored();
        virtual const StateMachineState GetStateMachineState();
        virtual const std::string GetStateMachineName();
        virtual void SetLogLevel(const LogLevel logLevel);
        // IJobStateMachine }}}

    private:
        virtual void PostSysEvent(const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data = 0);
        virtual BSTimer StartSysTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data = 0);
        virtual void StopSysTimer(BSTimer &timer);

    private:
        uint32_t GetMasterIndex() const;
        uint32_t GetStatemachineIndex() const;

    private:
        void fireEvent(Event &sendEvent);
        bool IsStateMachineRunning() const;
        bool IsJobRunning() const;
        bool IsJobStarted() const;

    private:
        bool OnBSSignalEvent(const BSSignalEvent * const pEvent);
        void OnBSSysEvent(const BSSysEvent * const pEvent);
        void OnBSEvent(const BSEvent *const pEvent);

    private:
        void OnBSSysEvent_JobBeginDelayTimer(const BSSysEvent * const pEvent);
        void OnBSSysEvent_TimeoutTimer(const BSSysEvent * const pEvent);
        void OnBSSysEvent_FailureRetryDelayTimer(const BSSysEvent * const pEvent);
        void OnBSSysEvent_RetryDurationTimer(const BSSysEvent * const pEvent);

    private:
        // {{{ IStatemachine
        virtual const char *getStateName();
        virtual const char *getStatemachineName();
        virtual void onEventHandler(const Event * const pEvent);
        virtual void onStart();
        virtual bool abortExecution();
        // IStatemachine }}}

    private:
        IEntityFactory *m_pEntityFactory;
        IServiceStateMachine *m_pServiceStateMachine;
        std::shared_ptr<IJobEntity> m_pJobEntity;
        uint32_t m_nMasterIndex;
        uint32_t m_nStatemachineIndex;
        std::string m_stateMachineName;

        JobState m_jobState;
        StateMachineState m_stateMachineState;
        RetryConfig m_retryConfig;
        uint32_t m_nTimedoutCount;
        BSTimer m_timeoutRetryTimer;
        DelayConfig m_delayConfig;
        BSTimer m_jobBeginDelayTimer;
        uint32_t m_nFailedCount;
        BSTimer m_failureRetryDelayTimer;
        BSTimer m_retryDurationTimer;
        LogLevel m_logLevel;
    };

} // namespace bsfwk

#endif // #ifndef BSFWK_BUSINESS_JOB_STATE_MACHINE_H
