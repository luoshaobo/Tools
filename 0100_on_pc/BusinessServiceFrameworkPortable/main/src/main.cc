#include "rmc_global.h"
#include "theft_notification_global.h"
#include "test_app_global.h"

int main(int argc, char *argv[])
{
    int nRet = 0;

    nRet = test_app_global::test_main(argc, argv);
    //nRet = volvo_on_call::test_rmc_main(argc, argv);
    //nRet = volvo_on_call::test_tn_main(argc, argv);

    return nRet;
}
