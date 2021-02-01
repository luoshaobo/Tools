#include "bsfwk_Global.h"
#include "bsfwk_BusinessJobStateMachine.h"
#include "bsfwk_BusinessServiceStateMachine.h"

namespace bsfwk {

    namespace {
        class BSSysEventData_HandleService : public BSSysEventDataBase
        {
        public:
            explicit BSSysEventData_HandleService(const bool bNewRequest)
                : BSSysEventDataBase()
                , m_bNewRequest(bNewRequest)
            {
            }

            virtual ~BSSysEventData_HandleService()
            {
            }

            bool IsNewRequest() const { return m_bNewRequest; }

        private:
            bool m_bNewRequest;
        };
    } // namespace {

    BusinessServiceStateMachine::BusinessServiceStateMachine(
        IEntityFactory *pEntityFactory, 
        const uint32_t nMasterIndex, 
        const uint32_t nStatemachineIndex,
        const std::string &stateMachineName,
        ITimerMaster<IStatemachine, Event> *pITimerMaster,
        IStatemachineMaster *pIStatemachineMaster,
        const StatemachineId smid, 
        const StatemachineType type)
        : IStatemachineExt(pITimerMaster, pIStatemachineMaster, smid, type)
        , m_pEntityFactory(pEntityFactory)
        , m_pServiceEntity(0)
        , m_bsJobSMs()
        , m_bsJobsToDo()
        , m_nMasterIndex(nMasterIndex)
        , m_nStatemachineIndex(nStatemachineIndex)
        , m_stateMachineName(stateMachineName)
        , m_serviceState(ServiceState_Idle)
        , m_stateMachineState(StateMachineState_Stopped)
        , m_nCurrentJobNo(INVALID_CURRENT_JOB_NO)
        , m_retryConfig()
        , m_nTimedoutCount(0)
        , m_timeoutRetryTimer()
        , m_lastRequest()
        , m_nFailedCount(0)
        , m_failureRetryDelayTimer()
        , m_retryDurationTimer()
        , m_cyclicalRequestConfig()
        , m_cyclicRequestTimer()
        , m_logLevel(LogLevel_Verbose)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        if (IsPtrNotNull(m_pEntityFactory)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pEntityFactory->CreateServiceEntity()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_pServiceEntity = m_pEntityFactory->CreateServiceEntity(*this);
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pEntityFactory is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    BusinessServiceStateMachine::~BusinessServiceStateMachine()
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        BusinessServiceStateMachine::Stop();
        
        if (IsPtrNotNull(m_pEntityFactory)) {
            if (IsSharedPtrNotNull(m_pServiceEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity.reset()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pServiceEntity.reset();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pEntityFactory is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    // {{{ IServiceStateMachine
    //

    const bool BusinessServiceStateMachine::OnHandleSignal(const std::shared_ptr<fsm::Signal> oSignal)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        bool bConsumed = false;

        if (IsSharedPtrNotNull(m_pServiceEntity)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnHandleSignal()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            bConsumed = m_pServiceEntity->OnHandleSignal(oSignal);
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }

        return bConsumed;
    }

    void BusinessServiceStateMachine::Start()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        if (m_stateMachineState == StateMachineState_Stopped) {
            m_stateMachineState = StateMachineState_Running;
            if (IsSharedPtrNotNull(m_pServiceEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnStateMachineStart()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pServiceEntity->OnStateMachineStart();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
        }

        BSFWK_SLOG_DEBUG("%s[%s]: end: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);
    }

    void BusinessServiceStateMachine::Stop()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        StopCyclicRequest();

        if ((m_stateMachineState == StateMachineState_Running) || (m_stateMachineState == StateMachineState_Paused)) {
            CancelAllRequests();
            if (IsSharedPtrNotNull(m_pServiceEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnStateMachineStop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pServiceEntity->OnStateMachineStop();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
            m_stateMachineState = StateMachineState_Stopped;
        }

        BSFWK_SLOG_DEBUG("%s[%s]: end: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);
    }

    void BusinessServiceStateMachine::Reset()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        StopCyclicRequest();

        if ((m_stateMachineState == StateMachineState_Running) || (m_stateMachineState == StateMachineState_Paused)) {
            CancelAllRequests();
            if (IsSharedPtrNotNull(m_pServiceEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnStateMachineStop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pServiceEntity->OnStateMachineStop();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }
            m_stateMachineState = StateMachineState_Stopped;
        }

        BSFWK_SLOG_DEBUG("%s[%s]: end: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);
    }

    void BusinessServiceStateMachine::Pause()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        if (m_stateMachineState == StateMachineState_Running) {
            CancelCurrentRequest();

            m_stateMachineState = StateMachineState_Paused;
        }

        BSFWK_SLOG_DEBUG("%s[%s]: end: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);
    }

    void BusinessServiceStateMachine::Resume()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        if (m_stateMachineState == StateMachineState_Paused) {
            m_stateMachineState = StateMachineState_Running;

            if (IsSharedPtrNotNull(m_lastRequest)) {
                m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
                m_serviceState = ServiceState_Began;
                std::shared_ptr<BSSysEventDataBase> data = std::make_shared<BSSysEventData_HandleService>(false);
                PostSysEvent(static_cast<uint32_t>(SSMEI_HANDLE_SERVICE), data);
            }

            CheckNewRequest();
        }

        BSFWK_SLOG_DEBUG("%s[%s]: end: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);
    }

    void BusinessServiceStateMachine::PostEvent(const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data /*= 0*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: nEventIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), nEventIndex);

        BSEvent bsEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        fireEvent(bsEvent);
    }

    BSTimer BusinessServiceStateMachine::StartTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data /*= 0*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: timeoutMicroSeconds=%d, nEventIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), timeoutMicroSeconds, nEventIndex);

        BSEvent bsEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        return BSTimer(startTimer(timeoutMicroSeconds * 1000, bsEvent), this);
    }

    void BusinessServiceStateMachine::StopTimer(BSTimer &timer)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        timer.Reset();
    }

    void BusinessServiceStateMachine::AddRequest(const std::shared_ptr<ServiceRequestBase> request)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        m_pendingRequests.push(request);
        CheckNewRequest();
    }

    void BusinessServiceStateMachine::CancelCurrentRequest()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: begin: m_serviceState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_serviceState);

        if (IsServiceStarted()) {
            if (m_nCurrentJobNo < static_cast<uint32_t>(m_bsJobsToDo.size())) {
                BusinessJobStateMachine *pBusinessJobStateMachine = m_bsJobsToDo[m_nCurrentJobNo].get();
                if (IsPtrNotNull(pBusinessJobStateMachine)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->CancelCurrentJob()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    pBusinessJobStateMachine->CancelCurrentJob();
                    BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->Stop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    pBusinessJobStateMachine->Stop();
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: pBusinessJobStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }

                if (IsSharedPtrNotNull(m_pServiceEntity)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceCancelled()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pServiceEntity->OnServiceCancelled();
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pServiceEntity->OnServiceEnd();
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            } else {
                BSFWK_SLOG_DEBUG("%s[%s]: m_nCurrentJobNo=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nCurrentJobNo);
            }
        } else {
            BSFWK_SLOG_DEBUG("%s[%s]: m_nCurrentJobNo=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nCurrentJobNo);
        }

        StopSysTimer(m_timeoutRetryTimer);
        StopSysTimer(m_failureRetryDelayTimer);
        StopSysTimer(m_retryDurationTimer);

        m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
        m_serviceState = ServiceState_Cancelled;
        CheckNewRequest();

        BSFWK_SLOG_DEBUG("%s[%s]: end: m_serviceState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_serviceState);
    }

    void BusinessServiceStateMachine::CancelAllRequests()
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        m_pendingRequests = std::queue<std::shared_ptr<ServiceRequestBase> >();
        CancelCurrentRequest();
    }

    void BusinessServiceStateMachine::StartCyclicRequest()
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        if (IsSharedPtrNotNull(m_pServiceEntity)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->GetCyclicalRequestConfig()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_cyclicalRequestConfig = m_pServiceEntity->GetCyclicalRequestConfig();
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }

        if (m_cyclicalRequestConfig.GetEnabled() && (m_cyclicalRequestConfig.GetInterval() != 0)) {
            BSFWK_SLOG_DEBUG("%s[%s]: cyclical config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_cyclicRequestTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_cyclicalRequestConfig.GetInterval()), static_cast<uint32_t>(SSMEI_CYCLIC_REQUEST_TIMER));
        }
    }

    void BusinessServiceStateMachine::StopCyclicRequest()
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        StopSysTimer(m_cyclicRequestTimer);
    }

    const uint32_t BusinessServiceStateMachine::GetCurrentJobIndex()
    {
        uint32_t nCurrentJobIndex = INVALID_CURRENT_JOB_INDEX;

        if (m_nCurrentJobNo < m_bsJobsToDo.size()) {
            BusinessJobStateMachine *pBusinessJobStateMachine = m_bsJobsToDo[m_nCurrentJobNo].get();
            if (IsPtrNotNull(pBusinessJobStateMachine)) {
                nCurrentJobIndex = pBusinessJobStateMachine->GetJobIndex();
            }
        }

        BSFWK_SLOG_DEBUG("%s[%s]: m_nCurrentJobNo=%u, nCurrentJobIndex=%u\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nCurrentJobNo, nCurrentJobIndex);

        return nCurrentJobIndex;
    }

    void BusinessServiceStateMachine::OnJobFinished(const uint32_t nJobIndex, const JobState &jobState)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: nJobIndex=%d, jobState=%d, m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), nJobIndex, jobState, m_stateMachineState);

        bool bIsCurrentJob = true;

        if (IsStateMachineRunning()) {
            if ((m_nCurrentJobNo < static_cast<uint32_t>(m_bsJobsToDo.size()))) {
                BusinessJobStateMachine *pBusinessJobStateMachine = m_bsJobsToDo[m_nCurrentJobNo].get();
                if (IsPtrNotNull(pBusinessJobStateMachine)) {                   
                    const uint32_t nCurrentJobIndex = pBusinessJobStateMachine->GetJobIndex();
                    if (nCurrentJobIndex != nJobIndex) {
                        bIsCurrentJob = false;
                        BSFWK_SLOG_ERROR("%s[%s]: nCurrentJobIndex=%u, nJobIndex=%u\n", BSFWK_FUNC, m_stateMachineName.c_str(), nCurrentJobIndex, nJobIndex);
                    }

                    if (bIsCurrentJob) {
                        switch (jobState) {
                            case JobState_Completed:
                                {
                                    BSFWK_SLOG_DEBUG("%s[%s]: JobState_Completed\n", BSFWK_FUNC, m_stateMachineName.c_str());

                                    BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->Stop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                    pBusinessJobStateMachine->Stop();

                                    m_nCurrentJobNo++;
                                    PostSysEvent(static_cast<uint32_t>(SSMEI_HANDLE_JOB_N));
                                }
                                break;
                            case JobState_Failed:       // NOTE: go though
                            case JobState_Timedout:
                                {
                                    BSFWK_SLOG_DEBUG("%s[%s]: JobState_Failed JobState_Timedout\n", BSFWK_FUNC, m_stateMachineName.c_str());

                                    BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->Stop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                    pBusinessJobStateMachine->Stop();
                                }
                                break;
                            default:
                                {
                                    BSFWK_SLOG_WARN("%s[%s]: default\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                    // do nothing
                                }
                                break;
                        } // switch (jobState) {
                    } // if (bIsCurrentJob) {
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: pBusinessJobStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                } // if (pBusinessJobStateMachine != 0) {

                if (bIsCurrentJob) {
                    switch (jobState) {
                        case JobState_Completed:
                            {
                                BSFWK_SLOG_DEBUG("%s[%s]: JobState_Completed\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                // do nothing
                            }
                            break;
                        case JobState_Failed:           // NOTE: go though
                        case JobState_Timedout:
                            {
                                BSFWK_SLOG_DEBUG("%s[%s]: JobState_Failed JobState_Timedout\n", BSFWK_FUNC, m_stateMachineName.c_str());

                                if (IsServiceStarted()) {
                                    if (IsSharedPtrNotNull(m_pServiceEntity)) {

                                        BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceFailed()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                        m_pServiceEntity->OnServiceFailed(nJobIndex, m_nFailedCount);
                                        BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                        m_pServiceEntity->OnServiceEnd();
                                    } else {
                                        BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                    }
                                }

                                if (m_retryConfig.GetFailureRetryInfo().GetEnabled()
                                    && ((m_nFailedCount < m_retryConfig.GetFailureRetryInfo().GetRetryCount()) || (m_retryConfig.GetFailureRetryInfo().GetRetryCount() == 0))) {
                                    BSFWK_SLOG_DEBUG("%s[%s]: retry failure config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                    m_failureRetryDelayTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetFailureRetryInfo().GetDelayTime()), static_cast<uint32_t>(SSMEI_FAILURE_RETRY_DELAY_TIMER));

                                    m_serviceState = ServiceState_FailureRetryDelaying;

                                    m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
                                    m_nFailedCount++;
                                }
                                else {
                                    BSFWK_SLOG_DEBUG("%s[%s]: no retry failure config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                    StopSysTimer(m_timeoutRetryTimer);
                                    StopSysTimer(m_failureRetryDelayTimer);
                                    StopSysTimer(m_retryDurationTimer);

                                    m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
                                    m_nFailedCount++;

                                    m_serviceState = ServiceState_Failed;
                                    m_lastRequest.reset();
                                    CheckNewRequest();
                                }
                            }
                            break;
                        default:
                            {
                                BSFWK_SLOG_WARN("%s[%s]: default\n", BSFWK_FUNC, m_stateMachineName.c_str());
                                // do nothing
                            }
                            break;
                    } // switch (jobState) {
                } // if (bIsCurrentJob) {
            }
            else { // if ((m_nCurrentJobNo < static_cast<uint32_t>(m_bsJobsToDo.size()))) {
                BSFWK_SLOG_WARN("%s[%s]: nJobIndex=%d, m_nCurrentJobNo=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), nJobIndex, m_nCurrentJobNo);
            } // if ((m_nCurrentJobNo < static_cast<uint32_t>(m_bsJobsToDo.size()))) {
        } // if (IsStateMachineRunning()) {
    }

    const ServiceState BusinessServiceStateMachine::GetServiceState()
    {
        return m_serviceState;
    }

    const StateMachineState BusinessServiceStateMachine::GetStateMachineState()
    {
        return m_stateMachineState;
    }

    const std::string BusinessServiceStateMachine::GetStateMachineName()
    {
        return m_stateMachineName;
    }

    void BusinessServiceStateMachine::SetLogLevel(const LogLevel logLevel)
    {
        m_logLevel = logLevel;
    }

    //
    // IServiceStateMachine }}}
    //////////////////////////////////////////////////////////////////////////////////////////

    void BusinessServiceStateMachine::PostSysEvent(const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data /*= 0*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: nEventIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), nEventIndex);

        BSSysEvent bsSysEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        fireEvent(bsSysEvent);
    }

    BSTimer BusinessServiceStateMachine::StartSysTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data /*= 0*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: timeoutMicroSeconds=%d, nEventIndex=%d,\n", BSFWK_FUNC, m_stateMachineName.c_str(), timeoutMicroSeconds, nEventIndex);
        
        BSSysEvent bsSysEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        return BSTimer(startTimer(timeoutMicroSeconds * 1000, bsSysEvent), this);
    }

    void BusinessServiceStateMachine::StopSysTimer(BSTimer &timer)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        timer.Reset();
    }

    uint32_t BusinessServiceStateMachine::GetMasterIndex() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_nMasterIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex);
        
        return m_nMasterIndex;
    }

    uint32_t BusinessServiceStateMachine::GetStatemachineIndex() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_nStatemachineIndex=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nStatemachineIndex);
        
        return m_nStatemachineIndex;
    }

    void BusinessServiceStateMachine::fireEvent(Event &sendEvent)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        IStatemachineMaster *pStatemachineMaster = getStatemachineMaster();
        if (IsPtrNotNull(pStatemachineMaster)) {
            pStatemachineMaster->fireEvent(getStatemachineId(), sendEvent);
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: pStatemachineMaster is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    void BusinessServiceStateMachine::CheckNewRequest()
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_stateMachineState=%d, m_pendingRequests.size=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState, m_pendingRequests.size());

        if (IsStateMachineRunning()) {
            if (!m_pendingRequests.empty()) {
                if (!IsServiceRunning()) {
                    BSFWK_SLOG_DEBUG("%s[%s]: do\n", BSFWK_FUNC, m_stateMachineName.c_str());

                    m_nTimedoutCount = static_cast<uint32_t>(0);
                    m_nFailedCount = static_cast<uint32_t>(0);

                    m_serviceState = ServiceState_Began;
                    PostSysEvent(static_cast<uint32_t>(SSMEI_NEW_REQUEST));
                }
            }
        }
    }

    bool BusinessServiceStateMachine::IsStateMachineRunning() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);
        
        const bool bIsRunning = (
               (m_stateMachineState == StateMachineState_Running)
        );
        return bIsRunning;
    }

    bool BusinessServiceStateMachine::IsStateMachineRunningOrPaused() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_stateMachineState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_stateMachineState);

        const bool bIsRunning = (
               (m_stateMachineState == StateMachineState_Running)
            || (m_stateMachineState == StateMachineState_Paused)
            );
        return bIsRunning;
    }

    bool BusinessServiceStateMachine::IsServiceRunning() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_serviceState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_serviceState);

        const bool bIsRunning = (
               (m_serviceState == ServiceState_Began)
            || (m_serviceState == ServiceState_Running)
            || (m_serviceState == ServiceState_FailureRetryDelaying)
        );
        return bIsRunning;
    }

    bool BusinessServiceStateMachine::IsServiceStarted() const
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_serviceState=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_serviceState);

        const bool bIsStarted = (
               (m_serviceState == ServiceState_Began)
            || (m_serviceState == ServiceState_Running)
        );
        return bIsStarted;
    }

    void BusinessServiceStateMachine::GetJobsToDo()
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        bool bSuc = false;

        m_bsJobsToDo.clear();

        if (IsPtrNotNull(m_pServiceEntity)) {
            std::vector<uint32_t> jobList;
            BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->GetJobsToDo()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            if (m_pServiceEntity->GetJobsToDo(jobList)) {
                size_t i = 0;
                for (; i < jobList.size(); ++i) {
                    if (jobList[i] < m_bsJobSMs.size()) {
                        m_bsJobsToDo.push_back(m_bsJobSMs[jobList[i]]);
                    } else {
                        BSFWK_SLOG_ERROR("%s[%s] a job index is too big!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        break;
                    }
                }
                if (i == jobList.size()) {
                    BSFWK_SLOG_DEBUG("%s[%s] customized job list is used\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    bSuc = true;
                }
            }
        }

        if (!bSuc) {
            BSFWK_SLOG_DEBUG("%s[%s] default job list is used\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_bsJobsToDo = m_bsJobSMs;
        }
    }

    void BusinessServiceStateMachine::SetJobs(const std::vector<std::shared_ptr<BusinessJobStateMachine> > &bsJobSMs)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        m_bsJobSMs = bsJobSMs;
    }

    bool BusinessServiceStateMachine::OnBSSignalEvent(const BSSignalEvent * const pEvent)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        bool bConsumed = false;

        if (IsPtrNotNull(pEvent)) {
            if (IsStateMachineRunningOrPaused()) {
                bConsumed = OnHandleSignal(pEvent->GetData()->GetSignal());
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: pEvent == 0!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }

        return bConsumed;
    }

    void BusinessServiceStateMachine::OnBSSysEvent_NewRequest(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: m_pendingRequests.size=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_pendingRequests.size());

        if (!m_pendingRequests.empty()) {
            m_lastRequest = m_pendingRequests.front();
            m_pendingRequests.pop();

            m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
            m_serviceState = ServiceState_Began;
            std::shared_ptr<BSSysEventDataBase> data = std::make_shared<BSSysEventData_HandleService>(true);
            PostSysEvent(static_cast<uint32_t>(SSMEI_HANDLE_SERVICE), data);
        }
    }

    void BusinessServiceStateMachine::OnBSSysEvent_HandleService(const BSSysEvent * const pEvent)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        if (IsSharedPtrNotNull(m_lastRequest)) {
            bool bNewRequest = false;

            if (IsPtrNotNull(pEvent)) {
                std::shared_ptr<BSSysEventData_HandleService> data = std::dynamic_pointer_cast<BSSysEventData_HandleService>(pEvent->GetData());
                if (IsSharedPtrNotNull(data)) {
                    bNewRequest = data->IsNewRequest();
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: data is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: pEvent is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }

            StopSysTimer(m_timeoutRetryTimer);

            if (IsSharedPtrNotNull(m_pServiceEntity)) {
                BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->GetRetryConfig()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_retryConfig = m_pServiceEntity->GetRetryConfig();
            } else {
                BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
            }

            if (m_retryConfig.DurationEnabled()) {
                BSFWK_SLOG_DEBUG("%s[%s]: retry duration config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                if (bNewRequest) {
                    m_retryDurationTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetDuration()), static_cast<uint32_t>(SSMEI_RETRY_DURATION_TIMER));
                }
            }

            if (IsSharedPtrNotNull(m_pServiceEntity)) {
                if (m_retryConfig.GetTimeoutRetryInfo().GetEnabled()) {
                    BSFWK_SLOG_DEBUG("%s[%s]: retry timeout config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_timeoutRetryTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetTimeoutRetryInfo().GetTimeout()), static_cast<uint32_t>(SSMEI_TIMEOUT_TIMER));
                }

                m_nCurrentJobNo = static_cast<uint32_t>(0);
                m_serviceState = ServiceState_Running;
                BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceBegin()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                m_pServiceEntity->OnServiceBegin(m_lastRequest);
                if (m_serviceState == ServiceState_Running) { // NOTE: the service might be cancelled.
                    BSFWK_SLOG_DEBUG("%s[%s]: GetJobsToDo()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    GetJobsToDo();
                    PostSysEvent(static_cast<uint32_t>(SSMEI_HANDLE_JOB_N));
                }
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_lastRequest is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    void BusinessServiceStateMachine::OnBSSysEvent_HandleJobN(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        if (IsPtrNotNull(m_pEntityFactory)) {
            if (m_nCurrentJobNo < static_cast<uint32_t>(m_bsJobsToDo.size())) {
                BusinessJobStateMachine *pBusinessJobStateMachine = m_bsJobsToDo[m_nCurrentJobNo].get();
                if (IsPtrNotNull(pBusinessJobStateMachine)) {
                    if (!pBusinessJobStateMachine->GetJobIgnored()) {
                        BSFWK_SLOG_DEBUG("%s[%s]: ignore config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->Start()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        pBusinessJobStateMachine->Start();
                        BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->StartNewJob()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        pBusinessJobStateMachine->StartNewJob();
                    } else {
                        BSFWK_SLOG_DEBUG("%s[%s]: no ignore config\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        m_nCurrentJobNo++;
                        m_serviceState = ServiceState_Running;
                        PostSysEvent(static_cast<uint32_t>(SSMEI_HANDLE_JOB_N));
                    }
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: pBusinessJobStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            } else {
                BSFWK_SLOG_DEBUG("%s[%s]: m_nCurrentJobNo=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nCurrentJobNo);
                m_serviceState = ServiceState_Running;
                PostSysEvent(static_cast<uint32_t>(SSMEI_REQUEST_END));
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pEntityFactory is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    void BusinessServiceStateMachine::OnBSSysEvent_RequestEnd(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        if (IsSharedPtrNotNull(m_pServiceEntity)) {
            BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceSucceeded()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_pServiceEntity->OnServiceSucceeded();
            BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_pServiceEntity->OnServiceEnd();

            StopSysTimer(m_timeoutRetryTimer);
            StopSysTimer(m_failureRetryDelayTimer);
            StopSysTimer(m_retryDurationTimer);

            m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
            m_serviceState = ServiceState_Completed;
            m_lastRequest.reset();
            CheckNewRequest();
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    void BusinessServiceStateMachine::OnBSSysEvent_TimeoutTimer(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        StopSysTimer(m_timeoutRetryTimer);

        if ((m_nTimedoutCount < m_retryConfig.GetTimeoutRetryInfo().GetRetryCount()) || (m_retryConfig.GetTimeoutRetryInfo().GetRetryCount() == 0)) {
            BSFWK_SLOG_DEBUG("%s[%s]: retry timeout config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_timeoutRetryTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_retryConfig.GetTimeoutRetryInfo().GetTimeout()), static_cast<uint32_t>(SSMEI_TIMEOUT_TIMER));

            if (IsServiceStarted()) {
                if (m_nCurrentJobNo < static_cast<uint32_t>(m_bsJobsToDo.size())) {
                    BusinessJobStateMachine *pBusinessJobStateMachine = m_bsJobsToDo[m_nCurrentJobNo].get();
                    if (IsPtrNotNull(pBusinessJobStateMachine)) {
                        BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->CancelCurrentJob()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        pBusinessJobStateMachine->CancelCurrentJob();
                        BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->Stop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        pBusinessJobStateMachine->Stop();
                    } else {
                        BSFWK_SLOG_ERROR("%s[%s]: pBusinessJobStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    }
                } else {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_nCurrentJobNo=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nCurrentJobNo);
                }

                if (IsSharedPtrNotNull(m_pServiceEntity)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceTimedout()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pServiceEntity->OnServiceTimedout(m_nTimedoutCount);
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pServiceEntity->OnServiceEnd();
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            }

            if (m_serviceState != ServiceState_FailureRetryDelaying) {
                m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
                m_serviceState = ServiceState_Began;
                std::shared_ptr<BSSysEventDataBase> data = std::make_shared<BSSysEventData_HandleService>(false);
                PostSysEvent(static_cast<uint32_t>(SSMEI_HANDLE_SERVICE), data);
            }

            m_nTimedoutCount++;
        } else {
            BSFWK_SLOG_DEBUG("%s[%s]: no retry timeout config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            if (IsServiceStarted()) {
                if (m_nCurrentJobNo < static_cast<uint32_t>(m_bsJobsToDo.size())) {
                    BusinessJobStateMachine *pBusinessJobStateMachine = m_bsJobsToDo[m_nCurrentJobNo].get();
                    if (IsPtrNotNull(pBusinessJobStateMachine)) {
                        BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->CancelCurrentJob()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        pBusinessJobStateMachine->CancelCurrentJob();
                        BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->Stop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        pBusinessJobStateMachine->Stop();
                    } else {
                        BSFWK_SLOG_ERROR("%s[%s]: pBusinessJobStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    }
                } else {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_nCurrentJobNo=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nCurrentJobNo);
                }

                if (IsSharedPtrNotNull(m_pServiceEntity)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceTimedout()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pServiceEntity->OnServiceTimedout(m_nTimedoutCount);
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pServiceEntity->OnServiceEnd();
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            }

            StopSysTimer(m_timeoutRetryTimer);
            StopSysTimer(m_failureRetryDelayTimer);
            StopSysTimer(m_retryDurationTimer);

            m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
            m_serviceState = ServiceState_Timedout;
            m_lastRequest.reset();
            CheckNewRequest();
        }
    }

    void BusinessServiceStateMachine::OnBSSysEvent_FailureRetryDelayTimer(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        StopSysTimer(m_failureRetryDelayTimer);

        m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
        m_serviceState = ServiceState_Began;
        std::shared_ptr<BSSysEventDataBase> data = std::make_shared<BSSysEventData_HandleService>(false);
        PostSysEvent(static_cast<uint32_t>(SSMEI_HANDLE_SERVICE), data);
    }

    void BusinessServiceStateMachine::OnBSSysEvent_RetryDurationTimer(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]: %d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_serviceState);

        if (IsServiceStarted()) {
            if (m_nCurrentJobNo < static_cast<uint32_t>(m_bsJobsToDo.size())) {
                BusinessJobStateMachine *pBusinessJobStateMachine = m_bsJobsToDo[m_nCurrentJobNo].get();
                if (IsPtrNotNull(pBusinessJobStateMachine)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->CancelCurrentJob()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    pBusinessJobStateMachine->CancelCurrentJob();
                    BSFWK_SLOG_DEBUG("%s[%s]: pBusinessJobStateMachine->Stop()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    pBusinessJobStateMachine->Stop();
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: pBusinessJobStateMachine is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }

                if (IsSharedPtrNotNull(m_pServiceEntity)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceFailed()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pServiceEntity->OnServiceFailed(m_nCurrentJobNo, m_nFailedCount);
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnServiceEnd()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pServiceEntity->OnServiceEnd();
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            }
        } else {
            BSFWK_SLOG_DEBUG("%s[%s]: m_nCurrentJobNo=%d\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nCurrentJobNo);
        }

        StopSysTimer(m_timeoutRetryTimer);
        StopSysTimer(m_failureRetryDelayTimer);
        StopSysTimer(m_retryDurationTimer);

        m_nCurrentJobNo = INVALID_CURRENT_JOB_NO;
        m_serviceState = ServiceState_Failed;
        m_lastRequest.reset();
        CheckNewRequest();
    }

    void BusinessServiceStateMachine::OnBSSysEvent_CyclicRequestTimer(const BSSysEvent * const /*pEvent*/)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        StopSysTimer(m_cyclicRequestTimer);

        std::shared_ptr<ServiceRequestBase> request = std::make_shared<ServiceCyclicalRequest>();
        m_pendingRequests.push(request);
        CheckNewRequest();

        if (m_cyclicalRequestConfig.GetEnabled() && (m_cyclicalRequestConfig.GetInterval() != 0)) {
            BSFWK_SLOG_DEBUG("%s[%s]: cyclical config\n", BSFWK_FUNC, m_stateMachineName.c_str());
            m_cyclicRequestTimer = StartSysTimer(static_cast<const TimeElapse::Difference>(m_cyclicalRequestConfig.GetInterval()), static_cast<uint32_t>(SSMEI_CYCLIC_REQUEST_TIMER));
        }
    }

    void BusinessServiceStateMachine::OnBSSysEvent(const BSSysEvent * const pEvent)
    {
        BSFWK_SLOG_DEBUG("%s[%s]\n", BSFWK_FUNC, m_stateMachineName.c_str());

        if (IsPtrNotNull(pEvent)) {
            BSFWK_SLOG_DEBUG("%s[%s][%d,%d,%d]\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex, m_nStatemachineIndex, pEvent->GetEventIndex());

            switch (pEvent->GetEventIndex()) {
                case SSMEI_START_STATE_MACHINE:
                    {
                        BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_START_STATE_MACHINE\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        Start();
                    }
                    break;
                case SSMEI_STOP_STATE_MACHINE:
                    {
                        BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_STOP_STATE_MACHINE\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        Stop();
                    }
                    break;
                default:
                    {
                        BSFWK_SLOG_WARN("%s[%s]: default\n", BSFWK_FUNC, m_stateMachineName.c_str());
                        // do nothing
                    }
                    break;
            } // switch (pEvent->GetEventIndex()) {

            if (IsStateMachineRunning()) {
                switch (pEvent->GetEventIndex()) {
                    case SSMEI_NEW_REQUEST:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_NEW_REQUEST\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            OnBSSysEvent_NewRequest(pEvent);
                        }
                        break;
                    case SSMEI_HANDLE_SERVICE:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_HANDLE_SERVICE\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            OnBSSysEvent_HandleService(pEvent);
                        }
                        break;
                    case SSMEI_HANDLE_JOB_N:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_HANDLE_JOB_N\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            OnBSSysEvent_HandleJobN(pEvent);
                        }
                        break;
                    case SSMEI_REQUEST_END:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_REQUEST_END\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            OnBSSysEvent_RequestEnd(pEvent);
                        }
                        break;
                    case SSMEI_TIMEOUT_TIMER:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_TIMEOUT_TIMER\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            OnBSSysEvent_TimeoutTimer(pEvent);
                        }
                        break;
                    case SSMEI_FAILURE_RETRY_DELAY_TIMER:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_FAILURE_RETRY_DELAY_TIMER\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            OnBSSysEvent_FailureRetryDelayTimer(pEvent);
                        }
                        break;
                    case SSMEI_RETRY_DURATION_TIMER:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_RETRY_DURATION_TIMER\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            OnBSSysEvent_RetryDurationTimer(pEvent);
                        }
                        break;
                    case SSMEI_CYCLIC_REQUEST_TIMER:
                        {
                            BSFWK_SLOG_DEBUG("%s[%s]: SSMEI_CYCLIC_REQUEST_TIMER\n", BSFWK_FUNC, m_stateMachineName.c_str());
                            OnBSSysEvent_CyclicRequestTimer(pEvent);
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
        } else { // if (pEvent != 0) {
            BSFWK_SLOG_DEBUG("%s[%s][%d,%d]: pEvent == 0!\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex, m_nStatemachineIndex);
        } // if (pEvent != 0) {
    }

    void BusinessServiceStateMachine::OnBSEvent(const BSEvent * const pEvent)
    {
        if (IsPtrNotNull(pEvent)) {
            BSFWK_SLOG_DEBUG("%s[%s][%d,%d,%d]\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex, m_nStatemachineIndex, pEvent->GetEventIndex());

            if (IsStateMachineRunning()) {
                if (IsSharedPtrNotNull(m_pServiceEntity)) {
                    BSFWK_SLOG_DEBUG("%s[%s]: m_pServiceEntity->OnBSEvent()\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    m_pServiceEntity->OnBSEvent(std::shared_ptr<BSEvent>(pEvent->clone()));
                } else {
                    BSFWK_SLOG_ERROR("%s[%s]: m_pServiceEntity is NULL!\n", BSFWK_FUNC, m_stateMachineName.c_str());
                }
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s][%d,%d]: pEvent == 0!\n", BSFWK_FUNC, m_stateMachineName.c_str(), m_nMasterIndex, m_nStatemachineIndex);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    // {{{ IStatemachine
    //

    const char *BusinessServiceStateMachine::getStateName()
    {
        return "(UnknownState)";
    }

    const char *BusinessServiceStateMachine::getStatemachineName()
    {
        return "BusinessServiceStateMachine";
    }

    void BusinessServiceStateMachine::onEventHandler(const Event *pEvent)
    {
        if (IsPtrNotNull(pEvent)) {
            BSFWK_SLOG_DEBUG("%s[%s]: name=%s\n", BSFWK_FUNC, m_stateMachineName.c_str(), pEvent->getName());
            bool bHandled = false;

            /*if (!bHandled)*/ {
                if (std::string(pEvent->getName()) == std::string("BSEvent")) {
                    BSFWK_SLOG_DEBUG("%s[%s]: BSEvent\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    const BSEvent *pBSEvent = DynamicCast<const Event * const, const BSEvent * const>(pEvent);
                    OnBSEvent(pBSEvent);
                    bHandled = true;
                }
            }

            if (!bHandled) {
                if (std::string(pEvent->getName()) == std::string("BSSysEvent")) {
                    BSFWK_SLOG_DEBUG("%s[%s]: BSSysEvent\n", BSFWK_FUNC, m_stateMachineName.c_str());
                    const BSSysEvent *pBSSysEvent = DynamicCast<const Event * const, const BSSysEvent * const>(pEvent);
                    OnBSSysEvent(pBSSysEvent);
                    bHandled = true;
                }
            }

            if (bHandled) {
                // do nothing, just to get rid of SCC warining
            }
        } else {
            BSFWK_SLOG_ERROR("%s[%s]: pEvent == 0!\n", BSFWK_FUNC, m_stateMachineName.c_str());
        }
    }

    void BusinessServiceStateMachine::onStart()
    {

    }

    bool BusinessServiceStateMachine::abortExecution()
    {
        return true;
    }

    //
    // IStatemachine }}}
    //////////////////////////////////////////////////////////////////////////////////////////

} // namespace bsfwk
