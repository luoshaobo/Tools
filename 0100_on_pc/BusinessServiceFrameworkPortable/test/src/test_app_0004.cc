#include "test_app_0004.h"

namespace test_app_0004 {

    static void post_service_trigger_signal_0001_01(BusinessServer *&pBusinessServer)
    {
        msleep(0 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<fsm::Signal> signal = std::dynamic_pointer_cast<fsm::Signal>(std::make_shared<ServiceTriggerSignal>(transactionId, 1));
        if (pBusinessServer != NULL) {
            pBusinessServer->PostSignalEvent(signal);
        }
    }
    
    static void post_service_trigger_signal_0001_02(BusinessServer *&pBusinessServer)
    {
        msleep(2 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<fsm::Signal>  signal = std::dynamic_pointer_cast<fsm::Signal>(std::make_shared<ServiceTriggerSignal>(transactionId, 2));
        if (pBusinessServer != NULL) {
            pBusinessServer->PostSignalEvent(signal);
        }
    }
    
    static void post_service_trigger_signal_0002_01(BusinessServer *&pBusinessServer)
    {
        msleep(4 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<fsm::Signal>  signal = std::dynamic_pointer_cast<fsm::Signal>(std::make_shared<ServiceTriggerSignal>(transactionId, 3));
        if (pBusinessServer != NULL) {
            pBusinessServer->PostSignalEvent(signal);
        }
    }
    
    static void post_service_trigger_signal_0002_02(BusinessServer *&pBusinessServer)
    {
        msleep(6 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<fsm::Signal>  signal = std::dynamic_pointer_cast<fsm::Signal>(std::make_shared<ServiceTriggerSignal>(transactionId, 4));
        if (pBusinessServer != NULL) {
            pBusinessServer->PostSignalEvent(signal);
        }
    }
    
    static void post_service_exit_signal_0001_01(BusinessServer *&pBusinessServer)
    {
        msleep(10 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<fsm::Signal>  signal = std::dynamic_pointer_cast<fsm::Signal>(std::make_shared<ServiceExitSignal>(transactionId));
        if (pBusinessServer != NULL) {
            pBusinessServer->PostSignalEvent(signal);
        }
    }
    
    static void post_service_exit_signal_0001_02(BusinessServer *&pBusinessServer)
    {
        msleep(30 * 1000);

        ServiceTriggerSignalTransactionId transactionId;
        std::shared_ptr<fsm::Signal>  signal = std::dynamic_pointer_cast<fsm::Signal>(std::make_shared<ServiceExitSignal>(transactionId));
        if (pBusinessServer != NULL) {
            pBusinessServer->PostSignalEvent(signal);
        }
    }

    int test_main(int argc, char *argv[])
    {
        BSFWK_LOG_PRINTF("##########################################################################################\n");
        BSFWK_LOG_PRINTF("## [%s: %s]\n", BSFWK_FILEINFO, BSFWK_FUNCINFO);
        BSFWK_LOG_PRINTF("##\n");

        BSFWK_UNUSED(post_service_exit_signal_0001_01);

        BusinessServer businessServer01;
        BusinessServer businessServer02;

        EntityFactory_0001_01 factory_0001_01;
        BusinessService businessService_0001_01(&businessServer01, &factory_0001_01);
        businessServer01.AddService(&businessService_0001_01);
        //businessService_0001_01.SetLogLevel(LogLevel_Verbose);
        businessService_0001_01.Start();
        
        EntityFactory_0001_02 factory_0001_02;
        BusinessService businessService_0001_02(&businessServer01, &factory_0001_02);
        businessServer01.AddService(&businessService_0001_02);
        //businessService_0001_02.SetLogLevel(LogLevel_Verbose);
        businessService_0001_02.Start();
        
        EntityFactory_0002_01 factory_0002_01;
        BusinessService businessService_0002_01(&businessServer02, &factory_0002_01);
        businessServer02.AddService(&businessService_0002_01);
        //businessService_0002_01.SetLogLevel(LogLevel_Verbose);
        businessService_0002_01.Start();
        
        EntityFactory_0002_02 factory_0002_02;
        BusinessService businessService_0002_02(&businessServer02, &factory_0002_02);
        businessServer02.AddService(&businessService_0002_02);
        //businessService_0002_02.SetLogLevel(LogLevel_Verbose);
        businessService_0002_02.Start();

        BusinessServer *pBusinessServer1 = &businessServer01;
        BusinessServer *pBusinessServer2 = &businessServer02;

        really_async(std::bind(&post_service_trigger_signal_0001_01, pBusinessServer1));
        really_async(std::bind(&post_service_trigger_signal_0001_02, pBusinessServer1));
        really_async(std::bind(&post_service_trigger_signal_0002_01, pBusinessServer2));
        really_async(std::bind(&post_service_trigger_signal_0002_02, pBusinessServer2));

        if (IsAutoTest()) {
            SetTestCaseRunning(true);
            //really_async(std::bind(&post_service_exit_signal_0001_01, pBusinessServer1));
            really_async(std::bind(&post_service_exit_signal_0001_02, pBusinessServer2));

            while (IsTestCaseRunning()) {
                msleep(1000);
            }
        } else {
            // NOTE: enther any character to exit this program.
            getchar();
        }

        pBusinessServer1 = NULL;
        pBusinessServer2 = NULL;

        BSFWK_UNUSED(pBusinessServer1);
        BSFWK_UNUSED(pBusinessServer2);

        businessService_0001_01.Stop();
        businessService_0001_02.Stop();
        businessService_0002_01.Stop();
        businessService_0002_02.Stop();

        return 0;
    }

} // namespace test_app_0004 {
