#include "test_app_global.h"

#include "test_app_0001.h"     // normal case: 1 sever, 1 service per server.
#include "test_app_0002.h"     // normal case: 1 sever, 2 services per server.
#include "test_app_0003.h"     // normal case: 2 severs, 1 service per server.
#include "test_app_0004.h"     // normal case: 2 severs, 2 services per server.
#include "test_app_0005.h"     // normal case: 1 sever, 1 service per server. cyclical request.
#include "test_app_0006.h"     // normal case: 1 sever, 1 service per server. job delay.
#include "test_app_0007.h"     // normal case: 1 sever, 1 service per server. cancel service on OnServiceBegin().
#include "test_app_0008.h"     // normal case: 1 sever, 1 service per server. service to pause and resume.
#include "test_app_0010.h"     // normal case: 1 sever, 1 service per server. post event to ServiceStateMachine from outside.
#include "test_app_0011.h"     // normal case: 1 sever, 1 service per server. no job.

#include "test_app_0101.h"     // normal case: 1 sever, 1 service per server. job timeout retry.
#include "test_app_0102.h"     // normal case: 1 sever, 1 service per server. job failure retry.
#include "test_app_0103.h"     // normal case: 1 sever, 1 service per server. job timeout retry, and job failure retry.
#include "test_app_0104.h"     // normal case: 1 sever, 1 service per server. job timeout retry, and job failure retry, and job duration.

#include "test_app_0201.h"     // normal case: 1 sever, 1 service per server. service timeout retry.
#include "test_app_0202.h"     // normal case: 1 sever, 1 service per server. service failure retry.
#include "test_app_0203.h"     // normal case: 1 sever, 1 service per server. service timeout retry, and service failure retry.
#include "test_app_0204.h"     // normal case: 1 sever, 1 service per server. service timeout retry, and service failure retry, and service duration.

#include "test_app_0301.h"     // normal case: 1 sever, 1 service per server. job list.

#include "test_app_0401.h"     // service without job
#include "test_app_0501.h"     // customized statemachine service
#include "test_app_0502.h"     // customized statemachine job

#include "test_app_5001.h"     // normal case: 1 sever, 1 service per server. 
                               // job timeout retry, and job failure retry, and job duration.
                               // service timeout retry, and service failure retry, and service duration.
                               // cyclical request.
                               // job delay.

#include "test_app_6001.h"     // server with async call and timer

namespace test_app_global {

    int test_main(int argc, char *argv[])
    {
        int nRet = 0;

        SetAutoTest(true);

        nRet = test_app_0001::test_main(argc, argv);
        nRet = test_app_0002::test_main(argc, argv);
        nRet = test_app_0003::test_main(argc, argv);
        nRet = test_app_0004::test_main(argc, argv);
        nRet = test_app_0005::test_main(argc, argv);
        nRet = test_app_0006::test_main(argc, argv);
        nRet = test_app_0007::test_main(argc, argv);
        nRet = test_app_0008::test_main(argc, argv);
        nRet = test_app_0010::test_main(argc, argv);
        nRet = test_app_0011::test_main(argc, argv);

        nRet = test_app_0101::test_main(argc, argv);
        nRet = test_app_0102::test_main(argc, argv);
        nRet = test_app_0103::test_main(argc, argv);
        nRet = test_app_0104::test_main(argc, argv);

        nRet = test_app_0201::test_main(argc, argv);
        nRet = test_app_0202::test_main(argc, argv);
        nRet = test_app_0203::test_main(argc, argv);
        nRet = test_app_0204::test_main(argc, argv);

        nRet = test_app_0301::test_main(argc, argv);

        nRet = test_app_0401::test_main(argc, argv);

        nRet = test_app_0501::test_main(argc, argv);
        nRet = test_app_0502::test_main(argc, argv);

        nRet = test_app_5001::test_main(argc, argv);

        nRet = test_app_6001::test_main(argc, argv);

        return nRet;
    }

    bool g_bAutoTest = false;
    bool g_bTestCaseRunning = false;

    void SetAutoTest(bool bAutoTest)
    {
        g_bAutoTest = bAutoTest;
    }

    bool IsAutoTest()
    {
        return g_bAutoTest;
    }

    void SetTestCaseRunning(bool bRunning)
    {
        g_bTestCaseRunning = bRunning;
    }

    bool IsTestCaseRunning()
    {
        return g_bTestCaseRunning;
    }

} // namespace test_app_global {
