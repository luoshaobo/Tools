#ifndef TEST_APP_0011_H
#define TEST_APP_0011_H

#include "test_app_global.h"
#include <functional>

namespace test_app_0011 {

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

        enum {
            EVENT_001,
            EVENT_002,
            EVENT_003,
            EVENT_004,
        };

        enum State {
            STATE_0001,
            STATE_0002,
            STATE_0003,
        };

    private:
        void SetNextState(State state)
        {
            m_currentState = state;
        }

        State GetCurrentState() const
        {
            return m_currentState;
        }

        void State001_OnBSEvent(const std::shared_ptr<BSEvent> event)
        {
            if (event != 0) {
                switch (event->GetEventIndex()) {
                    case EVENT_001:
                        {
                            BSFWK_LOG_SERVICE_GEN_PRINTF("on EVENT_001\n");
                            SetNextState(STATE_0002);
                            GetServiceStateMachine().PostEvent(EVENT_002);
                        }
                        break;
                    default:
                        {
                        }
                        break;
                }
            }
        }

        void State002_OnBSEvent(const std::shared_ptr<BSEvent> event)
        {
            if (event != 0) {
                switch (event->GetEventIndex()) {
                    case EVENT_002:
                        {
                            BSFWK_LOG_SERVICE_GEN_PRINTF("on EVENT_002\n");
                            SetNextState(STATE_0003);
                            GetServiceStateMachine().PostEvent(EVENT_003);
                        }
                        break;
                    default:
                        {
                        }
                        break;
                }
            }
        }

        void State003_OnBSEvent(const std::shared_ptr<BSEvent> event)
        {
            if (event != 0) {
                switch (event->GetEventIndex()) {
                    case EVENT_003:
                        {
                            BSFWK_LOG_SERVICE_GEN_PRINTF("on EVENT_003\n");
                            m_timer = GetServiceStateMachine().StartTimer(5 * 1000, EVENT_004);
                        }
                        break;
                    case EVENT_004:
                        {
                            BSFWK_LOG_SERVICE_GEN_PRINTF("on EVENT_004\n");
                            m_timer.Reset();
                        }
                        break;
                    default:
                        {
                        }
                        break;
                }
            }
        }

    public:
        ServiceEntity_0001(IServiceStateMachine &rServiceStateMachine, Context &rContex, const std::string &serviceName)
            : ServiceEntityBase(rServiceStateMachine, serviceName)
            , m_rContex(rContex)
            , m_currentState(STATE_0001)
            , m_timer()
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
                    GetServiceStateMachine().PostEvent(EVENT_001);
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

            static struct StateInfoItem {
                State state;
                std::function<void (const std::shared_ptr<BSEvent> event)> func;
            } stateInfoMap[] = {
                { STATE_0001, std::bind(&ServiceEntity_0001::State001_OnBSEvent, this, std::placeholders::_1) }, 
                { STATE_0002, std::bind(&ServiceEntity_0001::State002_OnBSEvent, this, std::placeholders::_1) },
                { STATE_0003, std::bind(&ServiceEntity_0001::State003_OnBSEvent, this, std::placeholders::_1) },
            };

            for (unsigned int i = 0; i < sizeof(stateInfoMap) / sizeof(stateInfoMap[0]); i++) {
                StateInfoItem &item = stateInfoMap[i];
                if (GetCurrentState() == item.state) {
                    if (item.func != 0) {
                        item.func(event);
                    }
                }
            }
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
        State m_currentState;
        BSTimer m_timer;
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

            return pJobEntity;
        }

        virtual const uint32_t GetJobCount() const
        {
            return 0;
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

} // namespace test_app_0011 {

#endif // #ifndef TEST_APP_0011_H
