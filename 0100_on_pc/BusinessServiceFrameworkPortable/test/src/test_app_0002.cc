#include "test_app_0002.h"

namespace test_app_0002 {

    static void post_service_trigger_signal_0001_01()
    {
        msleep(0 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<ServiceTriggerSignal> signal = std::make_shared<ServiceTriggerSignal>(transactionId);
        PostSignalEvent(signal);
    }
    
    static void post_service_trigger_signal_0001_02()
    {
        msleep(2 * 1000);

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

    int test_main(int argc, char *argv[])
    {
        BSFWK_LOG_PRINTF("##########################################################################################\n");
        BSFWK_LOG_PRINTF("## [%s: %s]\n", BSFWK_FILEINFO, BSFWK_FUNCINFO);
        BSFWK_LOG_PRINTF("##\n");

        BusinessServer businessServer;
        g_PostSignalEvent_Driver = [&businessServer](std::shared_ptr<fsm::Signal> signal) {
            businessServer.PostSignalEvent(signal);
        };

        EntityFactory_0001_01 factory_0001_01;
        BusinessService businessService_0001_01(&businessServer, &factory_0001_01);
        businessServer.AddService(&businessService_0001_01);
        //businessService_0001_01.SetLogLevel(LogLevel_Verbose);
        businessService_0001_01.Start();
        
        EntityFactory_0001_02 factory_0001_02;
        BusinessService businessService_0001_02(&businessServer, &factory_0001_02);
        businessServer.AddService(&businessService_0001_02);
        //businessService_0001_02.SetLogLevel(LogLevel_Verbose);
        businessService_0001_02.Start();

        really_async(std::bind(&post_service_trigger_signal_0001_01));
        really_async(std::bind(&post_service_trigger_signal_0001_02));

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

        businessService_0001_01.Stop();
        businessService_0001_02.Stop();

        return 0;
    }

} // namespace test_app_0002 {
