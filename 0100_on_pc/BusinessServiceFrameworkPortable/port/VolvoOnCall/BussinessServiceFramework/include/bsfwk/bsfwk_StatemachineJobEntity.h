#ifndef BSFWK_STATEMACHINE_JOB_ENTITY_H
#define BSFWK_STATEMACHINE_JOB_ENTITY_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class StatemachineJobEntity : public JobEntityBase
    {
    protected:
        typedef unsigned int StateId;
        typedef unsigned int EventId;
        typedef std::function<void (StateId)> OnStateEnterFunc;
        typedef std::function<void (StateId)> OnStateExitFunc;
        typedef std::function<bool (StateId, const BSEvent &)> OnStateEventFunc;
        typedef std::map<EventId, OnStateEventFunc> OnStateEventFuncTable;
        struct StateHandler {
            OnStateEnterFunc onStateEnterFunc{nullptr};
            OnStateExitFunc onStateExitFunc{nullptr};
            OnStateEventFunc preOnStateEventFunc{nullptr};
            OnStateEventFunc postOnStateEventFunc{nullptr};
            OnStateEventFuncTable onStateEventFuncTable{};
        };
        typedef std::map<StateId, StateHandler> StateEventTable;
        struct StatemachineHandler {
            OnStateEventFunc preOnStateEventFunc{nullptr};
            OnStateEventFunc postOnStateEventFunc{nullptr};
            StateEventTable stateEventTable{};
        };

    protected:
        const StateId INVALID_STATE_ID = static_cast<StateId>(-1);

    protected:
        StatemachineJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName = "UnknownJob");
        virtual ~StatemachineJobEntity();

        void GotoState(StateId state);
        void PostEvent(const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data = 0);
        BSTimer StartTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data = 0);
        void StopTimer(BSTimer &timer);

        StateId GetCurrentState() const;
        StatemachineHandler &GetStatemachineHandler();

    protected:
        virtual void OnBSEvent(const std::shared_ptr<BSEvent> event) override;

private:
        OnStateEnterFunc GetOnStateEnterFunc(StateId stateId);
        OnStateExitFunc GetOnStateExitFunc(StateId stateId);
        OnStateEventFunc GetOnStateEventFunc(StateId stateId, EventId eventId);
        OnStateEventFunc GetPreOnEventFunc(StateId stateId);
        OnStateEventFunc GetPostOnEventFunc(StateId stateId);
        OnStateEventFunc GetPreOnEventFunc();
        OnStateEventFunc GetPostOnEventFunc();

    private:
        StateId m_currentState;
        StatemachineHandler m_statemachineHandler;
    };

} // namespace bsfwk {

#endif // #ifndef BSFWK_STATEMACHINE_JOB_ENTITY_H
