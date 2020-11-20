#ifndef BSFWK_I_SERVICE_STATE_MACHINE_H
#define BSFWK_I_SERVICE_STATE_MACHINE_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class IServiceStateMachine
    {
    public:

        virtual ~IServiceStateMachine() {}

        /**
        * @brief                                  Function called by the thread event queue when a new oSignal received.
        *
        * @return                                 Whether the oSignal is consumed or not.
        *                                             true: consumed.
        *                                             false: not consumed.
        */
        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> /*oSignal*/) = 0;

        /**
        * @brief                                  Function called when to start this ServiceStateMachine.
        *
        * @return
        */
        virtual void Start() = 0;

        /**
        * @brief                                  Function called when to stop this ServiceStateMachine.
        *                                         The the current request and all of the pending requests will be cancelled.
        *
        * @return
        */
        virtual void Stop() = 0;

        /**
        * @brief                                  Function called when to pause this ServiceStateMachine.
        *
        * @return
        */
        virtual void Pause() = 0;

        /**
        * @brief                                  Function called when to resume this ServiceStateMachine.
        *                                         The the current request will be cancelled, and will be redone when Resume() called.
        *
        * @return
        */
        virtual void Resume() = 0;

        /**
        * @brief                                  Function called when to reset this ServiceStateMachine to default state.
        *
        * @return
        */
        virtual void Reset() = 0;

        /**
        * @brief                                  Function called when to post an event to this ServiceStateMachine instance.
        *
        * @param[in] nEventIndex                  The event index, which should be unique in the scope of this ServiceStateMachine instance.
        * @param[in] data                         The data attached with the event.
        *
        * @return
        */
        virtual void PostEvent(const uint32_t /*nEventIndex*/, const std::shared_ptr<BSEventDataBase> /*data*/ = 0) = 0;

        /**
        * @brief                                  Function called when to start a timer.
        *
        * @param[in] timeoutMicroSeconds          Timeout value.
        * @param[in] nEventIndex                  The event index, which should be unique in the scope of this ServiceStateMachine instance.
        * @param[in] data                         The data attached with the event.
        *
        * @return                                 The new Timer instance.
        */
        virtual BSTimer StartTimer(const TimeElapse::Difference /*timeoutMicroSeconds*/, const uint32_t /*nEventIndex*/, const std::shared_ptr<BSEventDataBase> /*data*/ = 0) = 0;

        /**
        * @brief                                  Function called when to stop a timer.
        *
        * @param[inout] timer                     The timer to stop.
        *
        */
        virtual void StopTimer(BSTimer &/*timer*/) = 0;

        /**
        * @brief                                  Function called when to add a new request to the inner request queue.
        *
        * @param[in] pServiceRequest              The new reqeust.
        *
        * @return
        */
        virtual void AddRequest(const std::shared_ptr<ServiceRequestBase> /*request*/) = 0;

        /**
        * @brief                                  Function called when to cancel handling of the current Request.
        *
        * @return
        */
        virtual void CancelCurrentRequest() = 0;

        /**
        * @brief                                  Function called when to cancel handling of the current Request and all of the pending requests.
        *
        * @return
        */
        virtual void CancelAllRequests() = 0;

        /**
        * @brief                                  Function called when to start the internal cyclic request.
        *
        * @return
        */
        virtual void StartCyclicRequest() = 0;

        /**
        * @brief                                  Function called when to stop the internal cyclic request.
        *
        * @return
        */
        virtual void StopCyclicRequest() = 0;

        /**
        * @brief                                  Function called when to get the current job index.
        *
        * @return                                 The current job index.
        */
        virtual const uint32_t GetCurrentJobIndex() = 0;

        /**
        * @brief                                  Function called by JobStateMachine when a job has finished.
        *
        * @param[in] nJobIndex                    The job index.
        * @param[in] jobState                     The job finish state.
        *
        * @return
        */
        virtual void OnJobFinished(const uint32_t /*nJobIndex*/, const JobState &/*jobState*/) = 0;

        /**
        * @brief                                  Function called by ServiceEntity when to get the current Service state.
        *
        * @return                                 The current Service state.
        */
        virtual const ServiceState GetServiceState() = 0;

        /**
        * @brief                                  Function called by ServiceEntity when to get the current ServiceStateMachine state.
        *
        * @return                                 The current Service state.
        */
        virtual const StateMachineState GetStateMachineState() = 0;

        /**
        * @brief                                  Function called to get the state machine name.
        *
        * @return                                 The state machine name.
        */
        virtual const std::string GetStateMachineName() = 0;

        /**
        * @brief                                  Function called to set the log output level.
        *
        * @param[in] logLevel                     The log content which level equal to or greater than logLevel will be output from this state machine.
        *
        * @return
        */
        virtual void SetLogLevel(const LogLevel /*logLevel*/) = 0;
    };

} // namespace bsfwk {

#endif // #ifndef BSFWK_I_SERVICE_STATE_MACHINE_H
