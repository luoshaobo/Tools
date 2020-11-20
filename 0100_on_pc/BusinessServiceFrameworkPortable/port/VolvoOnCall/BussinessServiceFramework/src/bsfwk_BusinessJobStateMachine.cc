#include "bsfwk_Global.h"
#include "bsfwk_BusinessJobStateMachine.h"

namespace bsfwk {
    BusinessJobStateMachine::BusinessJobStateMachine(
        IEntityFactory *pEntityFactory, 
        IServiceStateMachine *pServiceStateMachine, 
        const uint32_t nMasterIndex, 
        const uint32_t nStatemachineIndex, 
        const std::string &stateMachineName,
        ITimerMaster<IStatemachine, Event> *pITimerMaster,
        IStatemachineMaster *pStatemachineMaster,
        const StatemachineId smid, 
        const StatemachineType type)
        : IStatemachineExt(pITimerMaster, pStatemachineMaster, smid, type)
        , m_pEntityFactory(pEntityFactory)
        , m_pServiceStateMachine(pServiceStateMachine)
        , m_pJobEntity(0)
        , m_nMasterIndex(nMasterIndex)
        , m_nStatemachineIndex(nStatemachineIndex)
        , m_stateMachineName(stateMachineName)
        , m_jobState(JobState_Idle)
        , m_stateMachineState(StateMachineState_Stopped)
        , m_retryConfig()
        , m_nTimedoutCount(0)
        , m_timeoutRetryTimer()
        , m_delayConfig()
        , m_jobBeginDelayTimer()
        , m_nFailedCount(0)
        , m_failureRetryDelayTimer()
        , m_retryDurationTimer()
        , m_logLevel(LogLevel_Verbose)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        if (IsPtrNotNull(m_pEntityFactory)) {
            if (IsPtrNotNull(m_pServiceStateMachine)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pEntityFactory->CreateJobEntity()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity = m_pEntityFactory->CreateJobEntity(*m_pServiceStateMachine, *this, m_nStatemachineIndex);
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pServiceStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pEntityFactory is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    BusinessJobStateMachine::~BusinessJobStateMachine()
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        BusinessJobStateMachine::Stop();

        if (IsPtrNotNull(m_pEntityFactory)) {
            if (IsSharedPtrNotNull(m_pJobEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity.reset()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity.reset();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pEntityFactory is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    // {{{ IServiceStateMachine
    //

    const bool BusinessJobStateMachine::OnHandleSignal(const std::shared_ptr<fsm::Signal> oSignal)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        bool bConsumed = false;

        if (IsSharedPtrNotNull(m_pJobEntity)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnHandleSignal()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            bConsumed = m_pJobEntity->OnHandleSignal(oSignal);
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }

        return bConsumed;
    }

    void BusinessJobStateMachine::Start()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        if (m_stateMachineState != StateMachineState_Running) {
            m_stateMachineState = StateMachineState_Running;
            if (IsSharedPtrNotNull(m_pJobEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnStateMachineStart()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity->OnStateMachineStart();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
        }

        BSFWK_SLOG_DEBUG("%s[%s]: end: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);
    }

    void BusinessJobStateMachine::Stop()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        CancelCurrentJob();

        if (m_stateMachineState != StateMachineState_Stopped) {
            if (IsSharedPtrNotNull(m_pJobEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnStateMachineStop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity->OnStateMachineStop();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
            m_stateMachineState = StateMachineState_Stopped;
        }

        BSFWK_SLOG_DEBUG("%s[%s]: end:  m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);
    }

    void BusinessJobStateMachine::Reset()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        CancelCurrentJob();

        if (m_stateMachineState != StateMachineState_Stopped) {
            if (IsSharedPtrNotNull(m_pJobEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnStateMachineStop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity->OnStateMachineStop();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
            m_stateMachineState = StateMachineState_Stopped;
        }

        BSFWK_SLOG_DEBUG("%s[%s]: end: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);
    }

    void BusinessJobStateMachine::PostEvent(const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data /*= 0*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: nEventIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), nEventIndex);

        BSEvent bsEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        fireEvent(bsEvent);
    }

    BSTimer BusinessJobStateMachine::StartTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data /*= 0*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: timeoutMicroSeconds=%d, nEventIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), timeoutMicroSeconds, nEventIndex);

        BSEvent bsEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        return BSTimer(startTimer(timeoutMicroSeconds * 1000, bsEvent), this);
    }

    void BusinessJobStateMachine::StopTimer(BSTimer &timer)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        timer.Reset();
    }

    void BusinessJobStateMachine::StartNewJob()
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        m_nTimedoutCount = static_cast<uint32_t>(0);
        m_nFailedCount = static_cast<uint32_t>(0);
        m_retryConfig = RetryConfig();
        m_delayConfig = DelayConfig();

        m_jobState = JobState_Began;

        if (IsSharedPtrNotNull(m_pJobEntity)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->GetRetryConfig()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_retryConfig = m_pJobEntity->GetRetryConfig();
            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->GetDelayConfig()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_delayConfig = m_pJobEntity->GetDelayConfig();
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }

        if (m_delayConfig.GetEnabled()) {
            BSFWK_SLOG_DEBUG("%s[%s]: delay config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_jobState = JobState_Delaying;
            m_jobBeginDelayTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_delayConfig.GetDelayTime()), static_cast<uint32_t>(JSMEI_JOB_BEGIN_DELAY_TIMER));
        } else {
            BSFWK_SLOG_DEBUG("%s[%s]: no delay config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            if (m_retryConfig.DurationEnabled()) {
                BSFWK_SLOG_DEBUG("%s[%s]: retry duration config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_retryDurationTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetDuration()), static_cast<uint32_t>(JSMEI_RETRY_DURATION_TIMER));
            }

            if (m_retryConfig.GetTimeoutRetryInfo().GetEnabled()) {
                BSFWK_SLOG_DEBUG("%s[%s]: retry timeout config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_timeoutRetryTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetTimeoutRetryInfo().GetTimeout()), static_cast<uint32_t>(JSMEI_TIMEOUT_TIMER));
            }

            if (IsSharedPtrNotNull(m_pJobEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobBegin()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity->OnJobBegin();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
        }
    }

    void BusinessJobStateMachine::CancelCurrentJob()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_jobState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_jobState);

        if (IsJobStarted()) {
            if (IsSharedPtrNotNull(m_pJobEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobCancelled()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity->OnJobCancelled();
                BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity->OnJobEnd();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
        }
        m_jobState = JobState_Idle;
        StopSysTimer(m_jobBeginDelayTimer);
        StopSysTimer(m_timeoutRetryTimer);
        StopSysTimer(m_failureRetryDelayTimer);
        StopSysTimer(m_retryDurationTimer);

        BSFWK_SLOG_DEBUG("%s[%s]: end: m_jobState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_jobState);
    }

    void BusinessJobStateMachine::OnJobFinished(const JobState &jobState)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_stateMachineState=%d, m_jobState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState, m_jobState);

        if (IsStateMachineRunning() && IsJobStarted()) {
            if (IsSharedPtrNotNull(m_pJobEntity)) {
                switch (jobState) {
                    case JobState_Completed:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: JobState_Completed\n", BSFWK_FUNC, m_stateMachineName.c_str());

                            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobSucceeded()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            m_pJobEntity->OnJobSucceeded();
                            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            m_pJobEntity->OnJobEnd();
                            m_jobState = JobState_Completed;
                            if (IsPtrNotNull(m_pServiceStateMachine)) {
                                BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceStateMachine->OnJobFinished()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                m_pServiceStateMachine->OnJobFinished(m_nStatemachineIndex, jobState);
                            } else {
                                BSFWK_SLOG_ERROR("%s[%s]: m_pServiceStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            }
                        }
                        break;
                    case JobState_Failed:           // NOTE: go though
                    case JobState_Timedout:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: JobState_Failed JobState_Timedout\n", BSFWK_FUNC, m_stateMachineName.c_str());

                            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobFailed()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            m_pJobEntity->OnJobFailed(m_nFailedCount);
                            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            m_pJobEntity->OnJobEnd();

                            if (m_retryConfig.GetFailureRetryInfo().GetEnabled()
                                && ((m_nFailedCount < m_retryConfig.GetFailureRetryInfo().GetRetryCount()) 
                                    || (m_retryConfig.GetFailureRetryInfo().GetRetryCount() == 0))) {
                                BSFWK_SLOG_DEBUG("%s[%s]: retry failure config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                m_jobState = JobState_FailureRetryDelaying;
                                m_failureRetryDelayTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetFailureRetryInfo().GetDelayTime()), static_cast<uint32_t>(JSMEI_FAILURE_RETRY_DELAY_TIMER));
                            }
                            else {
                                BSFWK_SLOG_DEBUG("%s[%s]: no retry failure config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                m_jobState = JobState_Failed;
                                if (IsPtrNotNull(m_pServiceStateMachine)) {
                                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceStateMachine->OnJobFinished()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                    m_pServiceStateMachine->OnJobFinished(m_nStatemachineIndex, jobState);
                                } else {
                                    BSFWK_SLOG_ERROR("%s[%s]: m_pServiceStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                }
                            }

                            m_nFailedCount++;
                        }
                        break;
                    default:
                        {
                            BSFWK_SLOG_WARN("%s[%s]: default\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            // do nothing
                        }
                        break;
                } // switch (jobState) {
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            } // if (IsPtrNotNull(m_pJobEntity)) {
        } // if (IsStateMachineRunning && IsJobStarted()) {
    }

    const JobState BusinessJobStateMachine::GetJobState()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_jobState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_jobState);

        return m_jobState;
    }

    const bool BusinessJobStateMachine::GetJobIgnored()
    {
        bool bRet = false;

        if (IsSharedPtrNotNull(m_pJobEntity)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->GetJobIgnored()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            bRet = m_pJobEntity->GetJobIgnored();
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }

        BSFWK_SLOG_DEBUG("%s[%s]: bRet=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), bRet);

        return bRet;
    }

    const StateMachineState BusinessJobStateMachine::GetStateMachineState()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        return m_stateMachineState;
    }

    const std::string BusinessJobStateMachine::GetStateMachineName()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_stateMachineName=%s\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineName.c_str());

        return m_stateMachineName;
    }

    void BusinessJobStateMachine::SetLogLevel(const LogLevel logLevel)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: logLevel=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), logLevel);

        m_logLevel = static_cast<LogLevel>(logLevel);
    }

    //
    // IServiceStateMachine }}}
    //////////////////////////////////////////////////////////////////////////////////////////

    void BusinessJobStateMachine::PostSysEvent(const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data /*= 0*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: nEventIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), nEventIndex);

        BSSysEvent bsSysEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        fireEvent(bsSysEvent);
    }

    BSTimer BusinessJobStateMachine::StartSysTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data /*= 0*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: timeoutMicroSeconds=%d, nEventIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), timeoutMicroSeconds, nEventIndex);

        BSSysEvent bsSysEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        return BSTimer(startTimer(timeoutMicroSeconds * 1000, bsSysEvent), this);
    }

    void BusinessJobStateMachine::StopSysTimer(BSTimer &timer)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        timer.Reset();
    }

    uint32_t BusinessJobStateMachine::GetMasterIndex() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_nMasterIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex);

        return m_nMasterIndex;
    }

    uint32_t BusinessJobStateMachine::GetStatemachineIndex() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_nStatemachineIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nStatemachineIndex);

        return m_nStatemachineIndex;
    }

    void BusinessJobStateMachine::fireEvent(Event &sendEvent)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        IStatemachineMaster *pStatemachineMaster = getStatemachineMaster();
        if (IsPtrNotNull(pStatemachineMaster)) {
            pStatemachineMaster->fireEvent(getStatemachineId(), sendEvent);
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: pStatemachineMaster is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    bool BusinessJobStateMachine::IsStateMachineRunning() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        const bool bIsRunning = (m_stateMachineState == StateMachineState_Running);
        return bIsRunning;
    }

    bool BusinessJobStateMachine::IsJobRunning() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_jobState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_jobState);

        const bool bIsRunning = (
               (m_jobState == JobState_Delaying)
            || (m_jobState == JobState_Began)
            || (m_jobState == JobState_Running)
            || (m_jobState == JobState_FailureRetryDelaying)
        );
        return bIsRunning;
    }

    bool BusinessJobStateMachine::IsJobStarted() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_jobState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_jobState);

        const bool bIsStared = (
               (m_jobState == JobState_Began)
            || (m_jobState == JobState_Running)
        );
        return bIsStared;
    }

    bool BusinessJobStateMachine::OnBSSignalEvent(const BSSignalEvent * const pEvent)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        bool bConsumed = false;

        if (IsPtrNotNull(pEvent)) {
            if (IsStateMachineRunning()) {
                bConsumed = OnHandleSignal(pEvent->GetData()->GetSignal());
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: pEvent == 0!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }

        return bConsumed;
    }

    void BusinessJobStateMachine::OnBSSysEvent_JobBeginDelayTimer(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        m_jobState = JobState_Began;

        StopSysTimer(m_jobBeginDelayTimer);

        if (m_retryConfig.DurationEnabled()) {
            BSFWK_SLOG_DEBUG("%s[%s]: retry duration config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_retryDurationTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetDuration()), static_cast<uint32_t>(JSMEI_RETRY_DURATION_TIMER));
        }

        if (m_retryConfig.GetTimeoutRetryInfo().GetEnabled()) {
            BSFWK_SLOG_DEBUG("%s[%s]: retry timeout config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_timeoutRetryTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetTimeoutRetryInfo().GetTimeout()), static_cast<uint32_t>(JSMEI_TIMEOUT_TIMER));
        }

        if (IsSharedPtrNotNull(m_pJobEntity)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobBegin()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_pJobEntity->OnJobBegin();
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    void BusinessJobStateMachine::OnBSSysEvent_TimeoutTimer(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        StopSysTimer(m_timeoutRetryTimer);

        if ((m_nTimedoutCount < m_retryConfig.GetTimeoutRetryInfo().GetRetryCount()) 
            || (m_retryConfig.GetTimeoutRetryInfo().GetRetryCount() == 0)) {
            BSFWK_SLOG_DEBUG("%s[%s]: retry timeout config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_timeoutRetryTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetTimeoutRetryInfo().GetTimeout()), static_cast<uint32_t>(JSMEI_TIMEOUT_TIMER));
            
            if (IsJobStarted()) {
                if (IsSharedPtrNotNull(m_pJobEntity)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobTimedout()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pJobEntity->OnJobTimedout(m_nTimedoutCount);
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pJobEntity->OnJobEnd();
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            }

            if (m_jobState != JobState_FailureRetryDelaying) {
                m_jobState = JobState_Running;
                if (IsSharedPtrNotNull(m_pJobEntity)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobBegin()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pJobEntity->OnJobBegin();
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            }
        }
        else {
            BSFWK_SLOG_DEBUG("%s[%s]: no retry timeout config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            if (IsSharedPtrNotNull(m_pJobEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobTimedout()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity->OnJobTimedout(m_nTimedoutCount);
                BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pJobEntity->OnJobEnd();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
            m_jobState = JobState_Timedout;
            if (IsPtrNotNull(m_pServiceStateMachine)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceStateMachine->OnJobFinished()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pServiceStateMachine->OnJobFinished(m_nStatemachineIndex, JobState_Timedout);
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pServiceStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
        }

        m_nTimedoutCount++;
    }

    void BusinessJobStateMachine::OnBSSysEvent_FailureRetryDelayTimer(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        m_jobState = JobState_Running;

        StopSysTimer(m_failureRetryDelayTimer);

        if (IsSharedPtrNotNull(m_pJobEntity)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnJobBegin()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_pJobEntity->OnJobBegin();
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    void BusinessJobStateMachine::OnBSSysEvent_RetryDurationTimer(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        StopSysTimer(m_retryDurationTimer);

        CancelCurrentJob();
        m_jobState = JobState_Failed;

        if (IsPtrNotNull(m_pServiceStateMachine)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceStateMachine->OnJobFinished()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_pServiceStateMachine->OnJobFinished(m_nStatemachineIndex, JobState_Failed);
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pServiceStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    void BusinessJobStateMachine::OnBSSysEvent(const BSSysEvent * const pEvent)
    {
        if (IsPtrNotNull(pEvent)) {
            BSFWK_SLOG_DEBUG("%s[%s][%d,%d,%d]\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex, m_nStatemachineIndex, pEvent->GetEventIndex());

            if (IsStateMachineRunning()) {
                switch (pEvent->GetEventIndex()) {
                    case JSMEI_JOB_BEGIN_DELAY_TIMER:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: JSMEI_JOB_BEGIN_DELAY_TIMER\n", BSFWK_FUNC, m_stateMachineName.c_str());

                            OnBSSysEvent_JobBeginDelayTimer(pEvent);
                        }
                        break;
                    case JSMEI_TIMEOUT_TIMER:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: JSMEI_TIMEOUT_TIMER\n", BSFWK_FUNC, m_stateMachineName.c_str());

                            OnBSSysEvent_TimeoutTimer(pEvent);
                        }
                        break;
                    case JSMEI_FAILURE_RETRY_DELAY_TIMER:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: JSMEI_FAILURE_RETRY_DELAY_TIMER\n", BSFWK_FUNC, m_stateMachineName.c_str());

                            OnBSSysEvent_FailureRetryDelayTimer(pEvent);
                        }
                        break;
                    case JSMEI_RETRY_DURATION_TIMER:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: JSMEI_RETRY_DURATION_TIMER\n", BSFWK_FUNC, m_stateMachineName.c_str());

                            OnBSSysEvent_RetryDurationTimer(pEvent);
                        }
                        break;
                    default:
                        {
                            BSFWK_SLOG_WARN("%s[%s]: default\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            // do nothing
                        }
                        break;
                } // switch (pEvent->GetEventIndex()) {
            } // if (IsStateMachineRunning()) {
        } else { // if (IsPtrNotNull(pEvent)) {
            BSFWK_SLOG_ERROR("%s[%s][%d,%d]: pEvent == 0!\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex, m_nStatemachineIndex);
        } // if (IsPtrNotNull(pEvent)) {
    }

    void BusinessJobStateMachine::OnBSEvent(const BSEvent * const pEvent)
    {
        if (IsPtrNotNull(pEvent)) {
            BSFWK_SLOG_DEBUG("%s[%s][%d,%d,%d]\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex, m_nStatemachineIndex, pEvent->GetEventIndex());

            if (IsStateMachineRunning()) {
                if (IsSharedPtrNotNull(m_pJobEntity)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pJobEntity->OnBSEvent()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pJobEntity->OnBSEvent(std::shared_ptr<BSEvent>(pEvent->clone()));
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: m_pJobEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            }
        } else { // if (IsPtrNotNull((pEvent)) {
            BSFWK_SLOG_ERROR("%s[%s][%d,%d]: pEvent == 0!\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex, m_nStatemachineIndex);
        } // if (IsPtrNotNull(pEvent)) {
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    // {{{ IStatemachine
    //

    const char *BusinessJobStateMachine::getStateName()
    {
        return "(UnknownState)";
    }

    const char *BusinessJobStateMachine::getStatemachineName()
    {
        return "BusinessJobStateMachine";
    }

    void BusinessJobStateMachine::onEventHandler(const Event * const pEvent)
    {
        if (IsPtrNotNull(pEvent)) {
            BSFWK_SLOG_DEBUG("%s[%s]: name=%s\n", BSFWK_FUNC, m_stateMachineName.c_str(), pEvent->getName());
            bool bHandled = false;

            /*if (!bHandled)*/ {
                if (std::string(pEvent->getName()) == std::string("BSEvent")) {
                    BSFWK_SLOG_DEBUG("%s[%s]: BSEvent\n", BSFWK_FUNC, m_stateMachineName.c_str());

                    const BSEvent * const pBSEvent = DynamicCast<const Event * const, const BSEvent * const>(pEvent);
                    OnBSEvent(pBSEvent);
                    bHandled = true;
                }
            }

            if (!bHandled) {
                if (std::string(pEvent->getName()) == std::string("BSSysEvent")) {
                    BSFWK_SLOG_DEBUG("%s[%s]: BSSysEvent\n", BSFWK_FUNC, m_stateMachineName.c_str());

                    const BSSysEvent * const pBSSysEvent = DynamicCast<const Event * const, const BSSysEvent * const>(pEvent);
                    OnBSSysEvent(pBSSysEvent);
                    bHandled = true;
                }
            }

            if (bHandled) {
                // do nothing, just to get rid of SCC warning
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: pEvent == 0!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    void BusinessJobStateMachine::onStart()
    {
        // do nothing
    }

    bool BusinessJobStateMachine::abortExecution()
    {
        // do nothing
        return true;
    }

    //
    // IStatemachine }}}
    //////////////////////////////////////////////////////////////////////////////////////////

} // namespace bsfwk
