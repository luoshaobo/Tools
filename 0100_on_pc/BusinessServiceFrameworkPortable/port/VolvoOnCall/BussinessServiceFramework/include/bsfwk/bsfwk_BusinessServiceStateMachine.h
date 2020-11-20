#ifndef BSFWK_BUSINESS_SERVICE_STATE_MACHINE_H
#define BSFWK_BUSINESS_SERVICE_STATE_MACHINE_H

#include "bsfwk_Common.h"
#include <queue>

namespace bsfwk {

    class BusinessService;
    class BusinessJobStateMachine;

    class BusinessServiceStateMachine : public IStatemachineExt, public IServiceStateMachine
    {
        friend class BusinessService;

    private:
        const uint32_t INVALID_CURRENT_JOB_INDEX = static_cast<uint32_t>(-1);

        enum {
            SSMEI_START_STATE_MACHINE,
            SSMEI_STOP_STATE_MACHINE,

            SSMEI_NEW_REQUEST,
            SSMEI_HANDLE_SERVICE,
            SSMEI_HANDLE_JOB_N,
            SSMEI_REQUEST_END,

            SSMEI_TIMEOUT_TIMER,
            SSMEI_FAILURE_RETRY_DELAY_TIMER,
            SSMEI_RETRY_DURATION_TIMER,
            SSMEI_CYCLIC_REQUEST_TIMER,
        };

    public:
        BusinessServiceStateMachine(
            IEntityFactory *pEntityFactory, 
            const uint32_t nMasterIndex, 
            const uint32_t nStatemachineIndex,
            const std::string &stateMachineName,
            ITimerMaster<IStatemachine, Event> *pITimerMaster,
            IStatemachineMaster *pIStatemachineMaster,
            const StatemachineId smid, 
            const StatemachineType type);

        virtual ~BusinessServiceStateMachine();

    public:
        // {{{ IServiceStateMachine
        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> oSignal);
        virtual void Start();
        virtual void Stop();
        virtual void Reset();
        virtual void Pause();
        virtual void Resume();
        virtual void PostEvent(const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data = 0);
        virtual BSTimer StartTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data = 0);
        virtual void StopTimer(BSTimer &timer);
        virtual void AddRequest(const std::shared_ptr<ServiceRequestBase> request);
        virtual void CancelCurrentRequest();
        virtual void CancelAllRequests();
        virtual void StartCyclicRequest();
        virtual void StopCyclicRequest();
        virtual const uint32_t GetCurrentJobIndex();
        virtual void OnJobFinished(const uint32_t nJobIndex, const JobState &jobState);
        virtual const ServiceState GetServiceState();
        virtual const StateMachineState GetStateMachineState();
        virtual const std::string GetStateMachineName();
        virtual void SetLogLevel(const LogLevel logLevel);
        // IServiceStateMachine }}}

    private:
        virtual void PostSysEvent(const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data = 0);
        virtual BSTimer StartSysTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data = 0);
        virtual void StopSysTimer(BSTimer &timer);

        void SetJobs(const std::vector<std::shared_ptr<BusinessJobStateMachine> > &bsJobSMs);
        bool IsStateMachineRunning() const;
        bool IsStateMachineRunningOrPaused() const;
        bool IsServiceRunning() const;
        bool IsServiceStarted() const;

    private:
        uint32_t GetMasterIndex() const;
        uint32_t GetStatemachineIndex() const;

    private:
        void fireEvent(Event &sendEvent);

    private:
        void CheckNewRequest();

    private:
        bool OnBSSignalEvent(const BSSignalEvent * const pEvent);
        void OnBSSysEvent(const BSSysEvent *const pEvent);
        void OnBSEvent(const BSEvent *const pEvent);

    private:
        void OnBSSysEvent_NewRequest(const BSSysEvent * const pEvent);
        void OnBSSysEvent_HandleService(const BSSysEvent * const pEvent);
        void OnBSSysEvent_HandleJobN(const BSSysEvent * const pEvent);
        void OnBSSysEvent_RequestEnd(const BSSysEvent * const pEvent);
        void OnBSSysEvent_TimeoutTimer(const BSSysEvent * const pEvent);
        void OnBSSysEvent_FailureRetryDelayTimer(const BSSysEvent * const pEvent);
        void OnBSSysEvent_RetryDurationTimer(const BSSysEvent * const pEvent);
        void OnBSSysEvent_CyclicRequestTimer(const BSSysEvent * const pEvent);

    private:
        // {{{ IStatemachine
        virtual const char *getStateName();
        virtual const char *getStatemachineName();
        virtual void onEventHandler(const Event *pEvent);
        virtual void onStart();
        virtual bool abortExecution();
        // IStatemachine }}}

    private:
        IEntityFactory *m_pEntityFactory;
        std::shared_ptr<IServiceEntity> m_pServiceEntity;
        std::vector<std::shared_ptr<BusinessJobStateMachine> > m_bsJobSMs;
        uint32_t m_nMasterIndex;
        uint32_t m_nStatemachineIndex;
        std::string m_stateMachineName;

        ServiceState m_serviceState;
        StateMachineState m_stateMachineState;
        std::queue<std::shared_ptr<ServiceRequestBase> > m_pendingRequests;
        uint32_t m_nCurrentJobIndex;
        RetryConfig m_retryConfig;
        uint32_t m_nTimedoutCount;
        BSTimer m_timeoutRetryTimer;
        std::shared_ptr<ServiceRequestBase> m_lastRequest;
        uint32_t m_nFailedCount;
        BSTimer m_failureRetryDelayTimer;
        BSTimer m_retryDurationTimer;
        CyclicalRequestConfig m_cyclicalRequestConfig;
        BSTimer m_cyclicRequestTimer;
        LogLevel m_logLevel;
    };

} // namespace bsfwk

#endif // #ifndef BSFWK_BUSINESS_SERVICE_STATE_MACHINE_H