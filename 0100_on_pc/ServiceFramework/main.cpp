#include "stdafx.h"
#include "MyService.h"

int main(int argc, char* argv[])
{
    MyEntityFactor myEntityFactor;
    ServiceStateMachine myServiceStateMachine(&myEntityFactor);

    LOG_GEN_PRINTF("### [main] myServiceStateMachine.Start();\n");
    myServiceStateMachine.Start();

    for (unsigned int i = 0; i < 30; ++i)
    {
        //
        // TODO: Simulate a request received from the client.
        //
        myServiceStateMachine.NewRequest(NULL);
        Sleep(1 * 1000);
    }

    LOG_GEN_PRINTF("### [main] myServiceStateMachine.Stop();\n");
    myServiceStateMachine.Stop();

    for (;;) {
        Sleep(1 * 1000);
    }

	return 0;
}
