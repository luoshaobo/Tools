#include "test_app_0301.h"

namespace test_app_0301 {

    static void post_service_trigger_signal_0301(uint32_t nValue)
    {
        msleep(0 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<ServiceTriggerSignal> signal = std::make_shared<ServiceTriggerSignal>(transactionId, nValue);
        PostSignalEvent(signal);
    }

    static void post_service_exit_signal_0301()
    {
        msleep(60 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<ServiceExitSignal> signal = std::make_shared<ServiceExitSignal>(transactionId);
        PostSignalEvent(signal);
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

        EntityFactory_0301 factory_0301;
        BusinessService businessService_0301(&businessServer, &factory_0301);
        businessServer.AddService(&businessService_0301);
        //businessService_0301.SetLogLevel(LogLevel_Verbose);
        businessService_0301.Start();

        really_async(std::bind(&post_service_trigger_signal_0301, TRIGGER_SIGNAL_ID_1));
        really_async(std::bind(&post_service_trigger_signal_0301, TRIGGER_SIGNAL_ID_2));

        if (IsAutoTest()) {
            SetTestCaseRunning(true);
            really_async(std::bind(&post_service_exit_signal_0301));

            while (IsTestCaseRunning()) {
                msleep(1000);
            }
        } else {
            // NOTE: enther any character to exit this program.
            getchar();
        }

        g_PostSignalEvent_Driver = NULL;

        businessService_0301.Stop();

        return 0;
    }

} // namespace test_app_0301 {
