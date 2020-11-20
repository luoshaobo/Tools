#include "bsfwk_Global.h"
#include "bsfwk_StatemachineJobEntity.h"

namespace bsfwk {

    StatemachineJobEntity::StatemachineJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName /*= "UnknownJob"*/)
        : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_currentState(INVALID_STATE_ID), m_statemachineHandler()
    {

    }

    StatemachineJobEntity::~StatemachineJobEntity()
    {

    }

    void StatemachineJobEntity::OnBSEvent(const std::shared_ptr<BSEvent> event)
    {
        if (event != nullptr) {
            EventId eventId = event->GetEventIndex();
            bool consumed = false;

            if (!consumed) {
                OnStateEventFunc preOnStateEventFunc = GetPreOnEventFunc();
                if (preOnStateEventFunc != nullptr) {
                    consumed = preOnStateEventFunc(m_currentState, *event);
                }
            }

            if (!consumed) {
                OnStateEventFunc preOnStateEventFunc = GetPreOnEventFunc(m_currentState);
                if (preOnStateEventFunc != nullptr) {
                    consumed = preOnStateEventFunc(m_currentState, *event);
                }
            }

            if (!consumed) {
                OnStateEventFunc onStateEventFunc = GetOnStateEventFunc(m_currentState, eventId);
                if (onStateEventFunc != nullptr) {
                    consumed = onStateEventFunc(m_currentState, *event);
                }
            }

            if (!consumed) {
                OnStateEventFunc postOnStateEventFunc = GetPostOnEventFunc(m_currentState);
                if (postOnStateEventFunc != nullptr) {
                    consumed = postOnStateEventFunc(m_currentState, *event);
                }
            }

            if (!consumed) {
                OnStateEventFunc postOnStateEventFunc = GetPostOnEventFunc();
                if (postOnStateEventFunc != nullptr) {
                    consumed = postOnStateEventFunc(m_currentState, *event);
                }
            }
        }
    }

    void StatemachineJobEntity::GotoState(StateId state)
    {
        if (m_currentState != state) {
            if (m_currentState != INVALID_STATE_ID) {
                OnStateExitFunc onStateExitFunc = GetOnStateExitFunc(m_currentState);
                if (onStateExitFunc != nullptr) {
                    onStateExitFunc(state);
                }
            }

            m_currentState = state;

            if (m_currentState != INVALID_STATE_ID) {
                OnStateEnterFunc onStateEnterFunc = GetOnStateEnterFunc(m_currentState);
                if (onStateEnterFunc != nullptr) {
                    onStateEnterFunc(state);
                }
            }
        }
    }

    StatemachineJobEntity::StateId StatemachineJobEntity::GetCurrentState() const
    {
        return m_currentState;
    }

    void StatemachineJobEntity::PostEvent(const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data /*= 0*/)
    {
        GetJobStateMachine().PostEvent(nEventIndex, data);
    }

    BSTimer StatemachineJobEntity::StartTimer(const TimeElapse::Difference timeoutMicroSeconds, const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data /*= 0*/)
    {
        return GetJobStateMachine().StartTimer(timeoutMicroSeconds, nEventIndex, data);
    }

    void StatemachineJobEntity::StopTimer(BSTimer &timer)
    {
        GetJobStateMachine().StopTimer(timer);
    }

    StatemachineJobEntity::StatemachineHandler &StatemachineJobEntity::GetStatemachineHandler()
    {
        return m_statemachineHandler;
    };

    StatemachineJobEntity::OnStateEnterFunc StatemachineJobEntity::GetOnStateEnterFunc(StateId stateId)
    {
        OnStateEnterFunc onStateEnterFunc;

        auto it = m_statemachineHandler.stateEventTable.find(stateId);
        if (it != m_statemachineHandler.stateEventTable.end()) {
            StateHandler &stateHandler = it->second;
            if (stateHandler.onStateEnterFunc != nullptr) {
                onStateEnterFunc = stateHandler.onStateEnterFunc;
            }
        }

        return onStateEnterFunc;
    }

    StatemachineJobEntity::OnStateExitFunc StatemachineJobEntity::GetOnStateExitFunc(StateId stateId)
    {
        OnStateExitFunc onStateExitFunc;

        auto it = m_statemachineHandler.stateEventTable.find(stateId);
        if (it != m_statemachineHandler.stateEventTable.end()) {
            StateHandler &stateHandler = it->second;
            if (stateHandler.onStateExitFunc != nullptr) {
                onStateExitFunc = stateHandler.onStateExitFunc;
            }
        }

        return onStateExitFunc;
    }

    StatemachineJobEntity::OnStateEventFunc StatemachineJobEntity::GetOnStateEventFunc(StateId stateId, EventId eventId)
    {
        OnStateEventFunc onStateEventFunc;

        auto it = m_statemachineHandler.stateEventTable.find(stateId);
        if (it != m_statemachineHandler.stateEventTable.end()) {
            StateHandler &stateHandler = it->second;
            OnStateEventFuncTable &onStateEventFuncTable = stateHandler.onStateEventFuncTable;
            auto it_eft = onStateEventFuncTable.find(eventId);
            if (it_eft != onStateEventFuncTable.end()) {
                if (it_eft->second != nullptr) {
                    onStateEventFunc = it_eft->second;
                }
            }
        }

        return onStateEventFunc;
    }

    StatemachineJobEntity::OnStateEventFunc StatemachineJobEntity::GetPreOnEventFunc(StateId stateId)
    {
        OnStateEventFunc onStateEventFunc;

        auto it = m_statemachineHandler.stateEventTable.find(stateId);
        if (it != m_statemachineHandler.stateEventTable.end()) {
            StateHandler &stateHandler = it->second;
            if (stateHandler.preOnStateEventFunc != nullptr) {
                onStateEventFunc = stateHandler.preOnStateEventFunc;
            }
        }

        return onStateEventFunc;
    }

    StatemachineJobEntity::OnStateEventFunc StatemachineJobEntity::GetPostOnEventFunc(StateId stateId)
    {
        OnStateEventFunc onStateEventFunc;

        auto it = m_statemachineHandler.stateEventTable.find(stateId);
        if (it != m_statemachineHandler.stateEventTable.end()) {
            StateHandler &stateHandler = it->second;
            if (stateHandler.postOnStateEventFunc != nullptr) {
                onStateEventFunc = stateHandler.postOnStateEventFunc;
            }
        }

        return onStateEventFunc;
    }

    StatemachineJobEntity::OnStateEventFunc StatemachineJobEntity::GetPreOnEventFunc()
    {
        OnStateEventFunc onStateEventFunc;

        if (m_statemachineHandler.preOnStateEventFunc != nullptr) {
            onStateEventFunc = m_statemachineHandler.preOnStateEventFunc;
        }

        return onStateEventFunc;
    }

    StatemachineJobEntity::OnStateEventFunc StatemachineJobEntity::GetPostOnEventFunc()
    {
        OnStateEventFunc onStateEventFunc;

        if (m_statemachineHandler.postOnStateEventFunc != nullptr) {
            onStateEventFunc = m_statemachineHandler.postOnStateEventFunc;
        }

        return onStateEventFunc;
    }

} // namespace bsfwk {
