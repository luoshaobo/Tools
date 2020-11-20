#ifndef TEST_APP_0401_H
#define TEST_APP_0401_H

#include "test_app_global.h"

namespace test_app_0401 {

    using namespace bsfwk;
    using namespace test_app_global;

    class ServiceEntity_0001 : public ServiceEntityBase
    {
    public:
        ServiceEntity_0001(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName)
            : ServiceEntityBase(rServiceStateMachine, serviceName)
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

        virtual const std::string GetServiceName()
        {
            return ServiceEntityBase::GetServiceName();
        }
    };

    int test_main(int argc, char *argv[]);

} // namespace test_app_0401 {

#endif // #ifndef TEST_APP_0401_H
