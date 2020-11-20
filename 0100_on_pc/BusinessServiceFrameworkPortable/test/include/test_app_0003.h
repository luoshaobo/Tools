#ifndef TEST_APP_0003_H
#define TEST_APP_0003_H

#include "test_app_global.h"

namespace test_app_0003 {

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
        ServiceEntity_0001(IServiceStateMachine &rServiceStateMachine, Context &rContex, const std::string &serviceName, uint32_t nHint)
            : ServiceEntityBase(rServiceStateMachine, serviceName)
            , m_rContex(rContex)
            , m_nHint(nHint)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
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
            return ServiceEntityBase::GetRetryConfig();
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
        uint32_t m_nHint;
    };

    class JobEntity_0001_0001 : public JobEntityBase
    {
        typedef Context_0001 Context;

        enum {
            EVENT_API_CALL_RESPONSE,
        };

    public:
        JobEntity_0001_0001(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, Context &rContex, const std::string &jobName)
            : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName)
            , m_rContex(rContex)
            , m_apiSimulatorTimer()
        {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
        }

        virtual ~JobEntity_0001_0001()
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

    class EntityFactory_0001_01 : public EntityFactoryBase
    {
        typedef Context_0001 Context;

    public:
        EntityFactory_0001_01() 
            : EntityFactoryBase()
            , m_context()
        {
        }

        virtual std::shared_ptr<IServiceEntity> CreateServiceEntity(IServiceStateMachine &rServiceStateMachine)
        {
            return std::make_shared<ServiceEntity_0001>(rServiceStateMachine, m_context, std::string("Service_0001_01"), 1);
        }

        virtual std::shared_ptr<IJobEntity> CreateJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex)
        {
            std::shared_ptr<IJobEntity> pJobEntity = 0;

            switch (nIndex) {
                case 0:
                    {
                        pJobEntity = std::make_shared<JobEntity_0001_0001>(rServiceStateMachine, rJobStateMachine, nIndex, m_context, std::string("Job_0001_01_1"));
                    }
                    break;
                case 1:
                    {
                        pJobEntity = std::make_shared<JobEntity_0001_0002>(rServiceStateMachine, rJobStateMachine, nIndex, m_context, std::string("Job_0001_01_2"));
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
            return std::string("ServiceSM_0001_01");
        }

        virtual const std::string GetJobStateMachineName(const uint32_t nIndex)
        {
            return std::string("JobSM_0001_01_") + FormatStr("%u", nIndex + 1);
        }

    private:
        Context m_context;
    };

    class EntityFactory_0001_02 : public EntityFactoryBase
    {
        typedef Context_0001 Context;

    public:
        EntityFactory_0001_02() 
            : EntityFactoryBase()
            , m_context()
        {
        }

        virtual std::shared_ptr<IServiceEntity> CreateServiceEntity(IServiceStateMachine &rServiceStateMachine)
        {
            return std::make_shared<ServiceEntity_0001>(rServiceStateMachine, m_context, std::string("Service_0001_02"), 2);
        }

        virtual std::shared_ptr<IJobEntity> CreateJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, uint32_t nIndex)
        {
            std::shared_ptr<IJobEntity> pJobEntity = 0;

            switch (nIndex) {
                case 0:
                    {
                        pJobEntity = std::make_shared<JobEntity_0001_0001>(rServiceStateMachine, rJobStateMachine, nIndex, m_context, std::string("Job_0001_02_1"));
                    }
                    break;
                case 1:
                    {
                        pJobEntity = std::make_shared<JobEntity_0001_0002>(rServiceStateMachine, rJobStateMachine, nIndex, m_context, std::string("Job_0001_02_2"));
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
            return std::string("ServiceSM_0001_02");
        }

        virtual const std::string GetJobStateMachineName(const uint32_t nIndex)
        {
            return std::string("JobSM_0001_02_") + FormatStr("%u", nIndex + 1);
        }

    private:
        Context m_context;
    };

    int test_main(int argc, char *argv[]);

} // namespace test_app_0003 {

#endif // #ifndef TEST_APP_0003_H