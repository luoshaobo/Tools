#ifndef TEST_APP_GLOBAL_H
#define TEST_APP_GLOBAL_H

#include "simulator_common.h"
#include "bsfwk_Global.h"
#include "rmc_simulator.h"

namespace test_app_global {

    typedef fsm::InternalSignalTransactionId ServiceTriggerSignalTransactionId;

    class ServiceTriggerSignal : public fsm::Signal
    {
    public:
        ServiceTriggerSignal(ServiceTriggerSignalTransactionId transactionID, uint32_t nValue = 0)
            : fsm::Signal(transactionID)
            , m_nValue(nValue)
        {
        }

        virtual std::string ToString()
        {
            return "ServiceTriggerSignal";
        }

    public:
        uint32_t m_nValue;
    };

    typedef fsm::InternalSignalTransactionId ServiceExitSignalTransactionId;

    class ServiceExitSignal : public fsm::Signal
    {
    public:
        ServiceExitSignal(ServiceExitSignalTransactionId transactionID, uint32_t nValue = 0)
            : fsm::Signal(transactionID)
            , m_nValue(nValue)
        {
        }

        virtual std::string ToString()
        {
            return "ServiceExitSignal";
        }

    public:
        uint32_t m_nValue;
    };

    typedef fsm::InternalSignalTransactionId ServiceResumeSignalTransactionId;

    class ServiceResumeSignal : public fsm::Signal
    {
    public:
        ServiceResumeSignal(ServiceTriggerSignalTransactionId transactionID, uint32_t nValue = 0)
            : fsm::Signal(transactionID)
            , m_nValue(nValue)
        {
        }

        virtual std::string ToString()
        {
            return "ServiceResumeSignal";
        }

    public:
        uint32_t m_nValue;
    };

    void SetAutoTest(bool bAutoTest);
    bool IsAutoTest();

    void SetTestCaseRunning(bool bRunning);
    bool IsTestCaseRunning();

    int test_main(int argc, char *argv[]);

} // namespace test_app_global {

#endif // #ifndef TEST_APP_GLOBAL_H
