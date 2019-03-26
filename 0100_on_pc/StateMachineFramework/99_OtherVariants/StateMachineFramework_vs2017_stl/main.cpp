// StateMachineFramework.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include "SMF_BaseThread.h"
#include "SMF_BaseEventQueue.h"
#include "SMF_BaseEngine.h"
#include "APP_afx.hpp"
#include "APP_Engines.hpp"

using namespace APP;

SMF_BaseThread &GetMainStateMachineThread()
{
    static SMF_BaseThread oMainStateMachineThread("MainStateMachineThread");

    return oMainStateMachineThread;
}

SMF_BaseEventQueue &GetMainEventQueue()
{
    static SMF_BaseEventQueue oMainEventQueue("MainEventQueue");

    return oMainEventQueue;
}

SMF_BaseEngine &GetMainEngine()
{
    static SMF_BaseEngine &rMainEngine = SMF_GET_ENGINE(MainEngine, 0);

    return rMainEngine;
}

enum {
    EVENT_QUEUE_ID_MAIN_EVENT_QUEUE = 0,
};

enum {
    ENGINE_ID_ENGINE_001 = 0,
};

void help()
{
    TK_MSG("Commands:\n");
    TK_MSG("  help|h|?: print the help information.\n");
    TK_MSG("  start: start the state machine.\n");
    TK_MSG("  exit: exit the state machine.\n");
    TK_MSG("  current_state|cs: show the current state name.\n");
    TK_MSG("  e|event <event_name>: post an event.\n");
    TK_MSG("  pe|post_event <event_name>: post an event.\n");
    TK_MSG("  se|send_event <event_name>: send an event.\n");
    TK_MSG("\n");
}

int main(int argc, char* argv[])
{
    int nRet = -1;
    char sBuf[1024];
    std::vector<std::string> vCmdLine;


    GetMainEventQueue().AttachToThread(GetMainStateMachineThread(), EVENT_QUEUE_ID_MAIN_EVENT_QUEUE);
    GetMainEngine().AttachToEventQueue(GetMainEventQueue(), ENGINE_ID_ENGINE_001);
    GetMainStateMachineThread().Start();
    GetMainEngine().Start();

    while (true) {
        char *pLine = fgets(sBuf, sizeof(sBuf), stdin);
        if (pLine == NULL) {
            continue;
        }

        vCmdLine = TK_Tools::SplitString(TK_Tools::TrimAll(sBuf));
        if (vCmdLine.size() <= 0) {
            continue;
        }

        if (vCmdLine[0] == "exit") {
            nRet = 0;
            break;
        } else if (vCmdLine[0] == "h" || vCmdLine[0] == "help" || vCmdLine[0] == "?") {
            help();
            continue;
        } else if (vCmdLine[0] == "start") {
            GetMainEngine().PostEvent(SMF_D_EVENT_ID(_GOTO_FIRST_CHILD));
            continue;
        } else if (vCmdLine[0] == "current_state" || vCmdLine[0] == "cs") {
            if (GetMainEngine().GetCurrentState() != NULL) {
                TK_MSG("%s\n", GetMainEngine().GetCurrentState()->GetStateName().c_str());
            } else {
                TK_ERR("*** Unknown current state.\n");
            }
            continue;
        } else if (vCmdLine[0] == "e" || vCmdLine[0] == "event") {
            if (vCmdLine.size() >= 2) {
                unsigned int nEventId = GetMainEngine().GetEventIdByName(vCmdLine[1]);
                if (nEventId != SMF_D_EVENT_ID(_INVALID)) {
                    GetMainEngine().PostEvent(nEventId);
                } else {
                    TK_ERR("*** Unknown event ID: %s\n", vCmdLine[1].c_str());
                }
                continue;
            }
		} else if (vCmdLine[0] == "pe" || vCmdLine[0] == "post_event") {
            if (vCmdLine.size() >= 2) {
                unsigned int nEventId = GetMainEngine().GetEventIdByName(vCmdLine[1]);
                if (nEventId != SMF_D_EVENT_ID(_INVALID)) {
                    TK_MSG("<<<=== PostEvent(%s)\n", vCmdLine[1].c_str());
                    GetMainEngine().PostEvent(nEventId);
                    TK_MSG("===>>> PostEvent(%s)\n", vCmdLine[1].c_str());
                } else {
                    TK_ERR("*** Unknown event ID: %s\n", vCmdLine[1].c_str());
                }
                continue;
            }
        } else if (vCmdLine[0] == "se" || vCmdLine[0] == "send_event") {
            if (vCmdLine.size() >= 2) {
                unsigned int nEventId = GetMainEngine().GetEventIdByName(vCmdLine[1]);
                if (nEventId != SMF_D_EVENT_ID(_INVALID)) {
                    TK_MSG("<<<=== SendEvent(%s)\n", vCmdLine[1].c_str());
                    GetMainEngine().SendEvent(nEventId);
                    TK_MSG("===>>> SendEvent(%s)\n", vCmdLine[1].c_str());
                } else {
                    TK_ERR("*** Unknown event ID: %s\n", vCmdLine[1].c_str());
                }
                continue;
            }
        }

        TK_ERR("*** Unknown command: %s\n", vCmdLine[0].c_str());
    }

	return nRet;
}

