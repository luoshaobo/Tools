#ifndef TEST_APP_0502_H
#define TEST_APP_0502_H

#include "test_app_global.h"

namespace test_app_0502 {

    using namespace bsfwk;
    using namespace test_app_global;

    struct Context_0001
    {
        Context_0001()
        {
            BSFWK_LOG_GEN_PRINTF("\n");
        }

        ~Context_0001()
        {
            BSFWK_LOG_GEN_PRINTF("\n");
        }
    };

    class ServiceEntity_0001 : public ServiceEntityBase
    {
        typedef Context_0001 Context;

    public:
        ServiceEntity_0001(IServiceStateMachine &rServiceStateMachine, Context &rContex, const std::string &serviceName)
            : ServiceEntityBase(rServiceStateMachine, serviceName)
            , m_rContex(rContex)
            , m_retryConfig()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            m_retryConfig.GetTimeoutRetryInfo().SetEnabled(true);
            m_retryConfig.GetTimeoutRetryInfo().SetRetryCount(3);
            m_retryConfig.GetTimeoutRetryInfo().SetTimeout(4000);
        }

        virtual ~ServiceEntity_0001()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        }

        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
        {
            bool bConsumed = false;

            {
                std::shared_ptr<ServiceTriggerSignal> sig = std::dynamic_pointer_cast<ServiceTriggerSignal>(signal);
                if (IsSharedPtrNotNull(sig)) {
                    BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
                    std::shared_ptr<ServiceSignalRequest> request = std::make_shared<ServiceSignalRequest>(signal);
                    GetServiceStateMachine().AddRequest(request);
                    bConsumed = true;
                }
            }

            {
                std::shared_ptr<ServiceExitSignal> sig = std::dynamic_pointer_cast<ServiceExitSignal>(signal);
                if (IsSharedPtrNotNull(sig)) {
                    BSFWK_LOG_SERVICE_GEN_PRINTF("*** ServiceExitSignal ***\n");
                    std::shared_ptr<ServiceSignalRequest> request = std::make_shared<ServiceSignalRequest>(signal);
                    GetServiceStateMachine().AddRequest(request);
                    bConsumed = false;
                }
            }

            return bConsumed;
        }

        virtual void OnBSEvent(const std::shared_ptr<BSEvent> event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        }

        virtual void OnStateMachineStart()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        }

        virtual void OnStateMachineStop() {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        }

        virtual void OnServiceBegin(const std::shared_ptr<ServiceRequestBase> request)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            {
                std::shared_ptr<ServiceSignalRequest> req = std::dynamic_pointer_cast<ServiceSignalRequest>(request);
                if (IsSharedPtrNotNull(req)) {
                    std::shared_ptr<ServiceExitSignal> sig = std::dynamic_pointer_cast<ServiceExitSignal>(req->GetSignal());
                    if (IsSharedPtrNotNull(sig)) {
                        BSFWK_LOG_SERVICE_GEN_PRINTF("*** ServiceExitSignal ***\n");
                        SetTestCaseRunning(false);
                    }
                }
            }
        }

        virtual void OnServiceEnd()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        }

        virtual void OnServiceSucceeded()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        }

        virtual void OnServiceFailed(const uint32_t nJobIndex, const uint32_t nIndex)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("nJobIndex=%d, nIndex=%d\n", nJobIndex, nIndex);
        }

        virtual void OnServiceTimedout(const uint32_t nIndex)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("nIndex=%d\n", nIndex);
        }

        virtual void OnServiceCancelled()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        }

        virtual const RetryConfig &GetRetryConfig()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            return m_retryConfig;
        }

        virtual const CyclicalRequestConfig &GetCyclicalRequestConfig()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            return ServiceEntityBase::GetCyclicalRequestConfig();
        }

        virtual const std::string GetServiceName()
        {
            return ServiceEntityBase::GetServiceName();
        }

    private:
        Context &m_rContex;
        RetryConfig m_retryConfig;
    };

    class JobEntity_0001_0001 : public StatemachineJobEntity
    {
        typedef Context_0001 Context;

    private:
        typedef JobEntity_0001_0001 ThisClass;

        enum {
            SS_STATE_0001,
            SS_STATE_0002,
            SS_STATE_0003,
            SS_STATE_0004,
        };

        enum {
            EI_GOTO_STATE_0001,
            EI_GOTO_STATE_0002,
            EI_GOTO_STATE_0003,
            EI_GOTO_STATE_0004,
        };

        struct EventDataGotoState0003 : public BSEventDataBase {
            std::string s;
        };

        struct EventDataGotoState0004 : public BSEventDataBase {
            std::string s;
        };
        
        const StateId INITIAL_STATE = SS_STATE_0001;

    private:
        void InitStatemachineHandler()
        {
            StatemachineHandler &statemachineHandler = GetStatemachineHandler();
            StateEventTable &stateEventTable = statemachineHandler.stateEventTable;

            statemachineHandler.preOnStateEventFunc = std::bind(&ThisClass::Global_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            statemachineHandler.postOnStateEventFunc = std::bind(&ThisClass::Global_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);

            stateEventTable[SS_STATE_0001].onStateEnterFunc = std::bind(&ThisClass::State0001_OnEnter, this, std::placeholders::_1);
            stateEventTable[SS_STATE_0001].preOnStateEventFunc = std::bind(&ThisClass::State0001_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0001].postOnStateEventFunc = std::bind(&ThisClass::State0001_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0001].onStateEventFuncTable[EI_GOTO_STATE_0001] = std::bind(&ThisClass::State0001_OnEventGotoState001, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0001].onStateEventFuncTable[EI_GOTO_STATE_0002] = std::bind(&ThisClass::State0001_OnEventGotoState002, this, std::placeholders::_1, std::placeholders::_2);

            stateEventTable[SS_STATE_0002].preOnStateEventFunc = std::bind(&ThisClass::State0002_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0002].postOnStateEventFunc = std::bind(&ThisClass::State0002_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0002].onStateEventFuncTable[EI_GOTO_STATE_0003] = std::bind(&ThisClass::State0002_OnEventGotoState003, this, std::placeholders::_1, std::placeholders::_2);

            stateEventTable[SS_STATE_0003].preOnStateEventFunc = std::bind(&ThisClass::State0003_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0003].postOnStateEventFunc = std::bind(&ThisClass::State0003_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0003].onStateEventFuncTable[EI_GOTO_STATE_0004] = std::bind(&ThisClass::State0003_OnEventGotoState004, this, std::placeholders::_1, std::placeholders::_2);

            stateEventTable[SS_STATE_0004].onStateEnterFunc = std::bind(&ThisClass::State0004_OnEnter, this, std::placeholders::_1);
            stateEventTable[SS_STATE_0004].preOnStateEventFunc = std::bind(&ThisClass::State0004_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0004].postOnStateEventFunc = std::bind(&ThisClass::State0004_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);
        }

        bool Global_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = false;

            return consumed;
        }

        bool Global_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = true;

            return consumed;
        }

        void State0001_OnEnter(StateId stateId)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            PostEvent(EI_GOTO_STATE_0001);
        }

        bool State0001_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = false;

            return consumed;
        }

        bool State0001_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = true;

            return consumed;
        }

        bool State0001_OnEventGotoState001(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = true;

            PostEvent(EI_GOTO_STATE_0002);

            return consumed;
        }

        bool State0001_OnEventGotoState002(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = true;

            GotoState(SS_STATE_0002);
            auto eventData3 = std::make_shared<EventDataGotoState0003>();
            if (eventData3 != nullptr) {
                eventData3->s = "EventDataGotoState0003_content";
            }
            PostEvent(EI_GOTO_STATE_0003, eventData3);

            return consumed;
        }

        bool State0002_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = false;
 
            return consumed;
        }

        bool State0002_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = true;
 
            return consumed;
        }

        bool State0002_OnEventGotoState003(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = true;

            auto eventData3 = std::dynamic_pointer_cast<EventDataGotoState0003>(event.GetData());
            if (eventData3 != nullptr) {
                BSFWK_LOG_JOB_GEN_PRINTF("EI_GOTO_STATE_0003: %s\n", eventData3->s.c_str());
            } else {
                BSFWK_LOG_JOB_GEN_PRINTF("EI_GOTO_STATE_0003: null event data\n");
            }
            GotoState(SS_STATE_0003);
            auto eventData4 = std::make_shared<EventDataGotoState0004>();
            if (eventData4 != nullptr) {
                eventData4->s = "EventDataGotoState0004_content";
            }
            PostEvent(EI_GOTO_STATE_0004, eventData4);
 
            return consumed;
        }

        bool State0003_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = false;

            return consumed;
        }

        bool State0003_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = true;

            return consumed;
        }

        bool State0003_OnEventGotoState004(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = true;

            auto eventData4 = std::dynamic_pointer_cast<EventDataGotoState0004>(event.GetData());
            if (eventData4 != nullptr) {
                BSFWK_LOG_JOB_GEN_PRINTF("EI_GOTO_STATE_0004: %s\n", eventData4->s.c_str());
            } else {
                BSFWK_LOG_JOB_GEN_PRINTF("EI_GOTO_STATE_0004: null event data\n");
            }
            GotoState(SS_STATE_0004);

            return consumed;
        }

        void State0004_OnEnter(StateId stateId)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            GetJobStateMachine().OnJobFinished(JobState_Completed);
        }

        bool State0004_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = false;

            return consumed;
        }

        bool State0004_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            bool consumed = true;

            return consumed;
        }

    public:
        JobEntity_0001_0001(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, Context &rContex, const std::string &jobName)
            : StatemachineJobEntity(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            InitStatemachineHandler();
        }

        virtual ~JobEntity_0001_0001()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
        }

        virtual void OnStateMachineStart()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            GotoState(INITIAL_STATE);
        }

        virtual void OnStateMachineStop() {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
        }

    private:
        Context &m_rContex;
    };

    class JobEntity_0001_0002 : public JobEntityBase
    {
        typedef Context_0001 Context;

        enum {
            EVENT_API_CALL_RESPONSE,
        };

    public:
        JobEntity_0001_0002(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, Context &rContex, const std::string &jobName)
            : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName)
            , m_rContex(rContex)
            , m_apiSimulatorTimer()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
        }

        virtual ~JobEntity_0001_0002()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
        }

        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            return false;
        }

        virtual void OnBSEvent(const std::shared_ptr<BSEvent> event)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");

            if (IsSharedPtrNotNull(event)) {
                if (event->GetEventIndex() == EVENT_API_CALL_RESPONSE) {
                    GetJobStateMachine().OnJobFinished(JobState_Completed);
                }
            }
        }

        virtual void OnStateMachineStart()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
        }

        virtual void OnStateMachineStop()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
        }

        virtual void OnJobBegin()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            m_apiSimulatorTimer = GetJobStateMachine().StartTimer(3000, EVENT_API_CALL_RESPONSE);
        }

        virtual void OnJobEnd()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            GetJobStateMachine().StopTimer(m_apiSimulatorTimer);
        }

        virtual void OnJobSucceeded()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
        }

        virtual void OnJobFailed(const uint32_t nIndex)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("nIndex=%d\n", nIndex);
        }

        virtual void OnJobTimedout(const uint32_t nIndex)
        {
            BSFWK_LOG_JOB_GEN_PRINTF("nIndex=%d\n", nIndex);
        }

        virtual void OnJobCancelled()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
        }

        virtual const RetryConfig &GetRetryConfig()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            return JobEntityBase::GetRetryConfig();
        }

        virtual const DelayConfig &GetDelayConfig()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            return JobEntityBase::GetDelayConfig();
        }

        virtual const bool GetJobIgnored()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            return false;
        }

        virtual const std::string GetJobName()
        {
            return JobEntityBase::GetJobName();
        }

    private:
        Context &m_rContex;
        BSTimer m_apiSimulatorTimer;
    };

    class EntityFactory_0001 : public EntityFactoryBase
    {
        typedef Context_0001 Context;

    public:
        EntityFactory_0001() 
            : EntityFactoryBase()
            , m_context()
        {
        }

        virtual std::shared_ptr<IServiceEntity> CreateServiceEntity(IServiceStateMachine &rServiceStateMachine)
        {
            return std::make_shared<ServiceEntity_0001>(rServiceStateMachine, m_context, std::string("Service_0001"));
        }

        virtual std::shared_ptr<IJobEntity> CreateJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex)
        {
            std::shared_ptr<IJobEntity> pJobEntity = 0;

            switch (nIndex) {
                case 0:
                    {
                        pJobEntity = std::make_shared<JobEntity_0001_0001>(rServiceStateMachine, rJobStateMachine, nIndex, m_context, std::string("Job_0001_1"));
                    }
                    break;
                case 1:
                    {
                        pJobEntity = std::make_shared<JobEntity_0001_0002>(rServiceStateMachine, rJobStateMachine, nIndex, m_context, std::string("Job_0001_2"));
                    }
                    break;
                default:
                    {
                    }
                    break;
            }

            return pJobEntity;
        }

        virtual const uint32_t GetJobCount() const
        {
            return 2;
        }

        virtual const std::string GetServiceStateMachineName()
        {
            return std::string("ServiceSM_0001");
        }

        virtual const std::string GetJobStateMachineName(const uint32_t nIndex)
        {
            return std::string("JobSM_0001_") + FormatStr("%u", nIndex + 1);
        }

    private:
        Context m_context;
    };

    int test_main(int argc, char *argv[]);

} // namespace test_app_0502 {

#endif // #ifndef TEST_APP_0502_H
