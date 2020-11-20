#ifndef TEST_APP_0501_H
#define TEST_APP_0501_H

#include "test_app_global.h"

namespace test_app_0501 {

    using namespace bsfwk;
    using namespace test_app_global;

    class ServiceEntity_0001 : public StatemachineServiceEntity
    {
    private:
        typedef ServiceEntity_0001 ThisClass;

        enum {
            SS_STATE_0001,
            SS_STATE_0002,
            SS_STATE_0003,
            SS_STATE_0004,
        };

        enum {
            EI_GOTO_STATE_0001,
            EI_GOTO_STATE_0002,
            EI_GOTO_STATE_0003,
            EI_GOTO_STATE_0004,
        };

        struct EventDataGotoState0003 : public BSEventDataBase {
            std::string s;
        };

        struct EventDataGotoState0004 : public BSEventDataBase {
            std::string s;
        };
        
        const StateId INITIAL_STATE = SS_STATE_0001;

    private:
        void InitStatemachineHandler()
        {
            StatemachineHandler &statemachineHandler = GetStatemachineHandler();
            StateEventTable &stateEventTable = statemachineHandler.stateEventTable;

            statemachineHandler.preOnStateEventFunc = std::bind(&ThisClass::Global_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            statemachineHandler.postOnStateEventFunc = std::bind(&ThisClass::Global_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);

            stateEventTable[SS_STATE_0001].onStateEnterFunc = std::bind(&ThisClass::State0001_OnEnter, this, std::placeholders::_1);
            stateEventTable[SS_STATE_0001].preOnStateEventFunc = std::bind(&ThisClass::State0001_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0001].postOnStateEventFunc = std::bind(&ThisClass::State0001_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0001].onStateEventFuncTable[EI_GOTO_STATE_0001] = std::bind(&ThisClass::State0001_OnEventGotoState001, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0001].onStateEventFuncTable[EI_GOTO_STATE_0002] = std::bind(&ThisClass::State0001_OnEventGotoState002, this, std::placeholders::_1, std::placeholders::_2);

            stateEventTable[SS_STATE_0002].preOnStateEventFunc = std::bind(&ThisClass::State0002_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0002].postOnStateEventFunc = std::bind(&ThisClass::State0002_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0002].onStateEventFuncTable[EI_GOTO_STATE_0003] = std::bind(&ThisClass::State0002_OnEventGotoState003, this, std::placeholders::_1, std::placeholders::_2);

            stateEventTable[SS_STATE_0003].preOnStateEventFunc = std::bind(&ThisClass::State0003_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0003].postOnStateEventFunc = std::bind(&ThisClass::State0003_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0003].onStateEventFuncTable[EI_GOTO_STATE_0004] = std::bind(&ThisClass::State0003_OnEventGotoState004, this, std::placeholders::_1, std::placeholders::_2);

            stateEventTable[SS_STATE_0004].onStateEnterFunc = std::bind(&ThisClass::State0004_OnEnter, this, std::placeholders::_1);
            stateEventTable[SS_STATE_0004].preOnStateEventFunc = std::bind(&ThisClass::State0004_PreOnEvent, this, std::placeholders::_1, std::placeholders::_2);
            stateEventTable[SS_STATE_0004].postOnStateEventFunc = std::bind(&ThisClass::State0004_PostOnEvent, this, std::placeholders::_1, std::placeholders::_2);
        }

        bool Global_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = false;

            return consumed;
        }

        bool Global_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = true;

            return consumed;
        }

        void State0001_OnEnter(StateId stateId)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            PostEvent(EI_GOTO_STATE_0001);
        }

        bool State0001_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = false;

            return consumed;
        }

        bool State0001_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = true;

            return consumed;
        }

        bool State0001_OnEventGotoState001(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = true;

            PostEvent(EI_GOTO_STATE_0002);

            return consumed;
        }

        bool State0001_OnEventGotoState002(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = true;

            GotoState(SS_STATE_0002);
            auto eventData3 = std::make_shared<EventDataGotoState0003>();
            if (eventData3 != nullptr) {
                eventData3->s = "EventDataGotoState0003_content";
            }
            PostEvent(EI_GOTO_STATE_0003, eventData3);

            return consumed;
        }

        bool State0002_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = false;
 
            return consumed;
        }

        bool State0002_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = true;
 
            return consumed;
        }

        bool State0002_OnEventGotoState003(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = true;

            auto eventData3 = std::dynamic_pointer_cast<EventDataGotoState0003>(event.GetData());
            if (eventData3 != nullptr) {
                BSFWK_LOG_SERVICE_GEN_PRINTF("EI_GOTO_STATE_0003: %s\n", eventData3->s.c_str());
            } else {
                BSFWK_LOG_SERVICE_GEN_PRINTF("EI_GOTO_STATE_0003: null event data\n");
            }
            GotoState(SS_STATE_0003);
            auto eventData4 = std::make_shared<EventDataGotoState0004>();
            if (eventData4 != nullptr) {
                eventData4->s = "EventDataGotoState0004_content";
            }
            PostEvent(EI_GOTO_STATE_0004, eventData4);
 
            return consumed;
        }

        bool State0003_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = false;

            return consumed;
        }

        bool State0003_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = true;

            return consumed;
        }

        bool State0003_OnEventGotoState004(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = true;

            auto eventData4 = std::dynamic_pointer_cast<EventDataGotoState0004>(event.GetData());
            if (eventData4 != nullptr) {
                BSFWK_LOG_SERVICE_GEN_PRINTF("EI_GOTO_STATE_0004: %s\n", eventData4->s.c_str());
            } else {
                BSFWK_LOG_SERVICE_GEN_PRINTF("EI_GOTO_STATE_0004: null event data\n");
            }
            GotoState(SS_STATE_0004);

            return consumed;
        }

        void State0004_OnEnter(StateId stateId)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            m_isOver = true;
        }

        bool State0004_PreOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = false;

            return consumed;
        }

        bool State0004_PostOnEvent(StateId stateId, const BSEvent &event)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

            bool consumed = true;

            return consumed;
        }

    public:
        bool IsOver() const { return m_isOver; }

    public:
        ServiceEntity_0001(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName)
            : StatemachineServiceEntity(rServiceStateMachine, serviceName), m_isOver(false)
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            InitStatemachineHandler();
        }

        virtual ~ServiceEntity_0001()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        }

        virtual void OnStateMachineStart()
        {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            GotoState(INITIAL_STATE);
        }

        virtual void OnStateMachineStop() {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        }

    private:
        bool m_isOver;
    };

    int test_main(int argc, char *argv[]);

} // namespace test_app_0501 {

#endif // #ifndef TEST_APP_0501_H
