#include "test_app_0010.h"

namespace test_app_0010 {

    IServiceStateMachine *g_pServiceStateMachine = NULL;

    static void post_service_trigger_signal_0001()
    {
        msleep(0 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<ServiceTriggerSignal> signal = std::make_shared<ServiceTriggerSignal>(transactionId);
        PostSignalEvent(signal);
    }

    static void post_service_exit_signal_0001()
    {
        msleep(10 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<ServiceExitSignal> signal = std::make_shared<ServiceExitSignal>(transactionId);
        PostSignalEvent(signal);
    }

    static void post_event_to_service_state_machine()
    {
        msleep(5 * 1000);

        if (g_pServiceStateMachine != NULL) {
            g_pServiceStateMachine->PostEvent(ServiceEntity_0001::EVENT_CUSTOMIZED_EVENT1);
        }
    }

    int test_main(int argc, char *argv[])
    {
        BSFWK_LOG_PRINTF("##########################################################################################\n");
        BSFWK_LOG_PRINTF("## [%s: %s]\n", BSFWK_FILEINFO, BSFWK_FUNCINFO);
        BSFWK_LOG_PRINTF("##\n");

        BusinessServer businessServer;
        g_PostSignalEvent_Driver = [&businessServer](std::shared_ptr<fsm::Signal> signal) {
            businessServer.PostSignalEvent(signal);
        };

        EntityFactory_0001 factory_0001;
        BusinessService businessService_0001(&businessServer, &factory_0001);
        businessServer.AddService(&businessService_0001);
        //businessService_0001.SetLogLevel(LogLevel_Verbose);
        businessService_0001.Start();

        really_async(std::bind(&post_service_trigger_signal_0001));
        really_async(std::bind(&post_event_to_service_state_machine));

        if (IsAutoTest()) {
            SetTestCaseRunning(true);
            really_async(std::bind(&post_service_exit_signal_0001));

            while (IsTestCaseRunning()) {
                msleep(1000);
            }
        } else {
            // NOTE: enther any character to exit this program.
            getchar();
        }

        g_PostSignalEvent_Driver = NULL;

        businessService_0001.Stop();

        return 0;
    }

} // namespace test_app_0010 {
