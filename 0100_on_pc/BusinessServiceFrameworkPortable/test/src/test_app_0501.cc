#include "test_app_0501.h"

namespace test_app_0501 {
    
    int test_main(int argc, char *argv[])
    {
        BSFWK_LOG_PRINTF("##########################################################################################\n");
        BSFWK_LOG_PRINTF("## [%s: %s]\n", BSFWK_FILEINFO, BSFWK_FUNCINFO);
        BSFWK_LOG_PRINTF("##\n");

        BusinessServer businessServer;
        EntityFactoryWithoutJob<ServiceEntity_0001> factory_0001("CustomizedStatemachineService");
        BusinessService businessService_0001(&businessServer, &factory_0001);
        std::shared_ptr<ServiceEntity_0001> serviceEntity = factory_0001.GetServiceEntity();
        businessServer.AddService(&businessService_0001);
        businessService_0001.SetLogLevel(LogLevel_Off);
        businessService_0001.Start();

        if (IsAutoTest()) {
            while (!serviceEntity->IsOver()) {
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

} // namespace test_app_0501 {
