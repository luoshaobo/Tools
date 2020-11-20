#ifndef BSFWK_I_JOB_STATE_MACHINE_H
#define BSFWK_I_JOB_STATE_MACHINE_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class IJobStateMachine
    {
    public:

        virtual ~IJobStateMachine() {}

        /**
        * @brief                                  Function called by ServiceStateMachine when:
        *                                         1) A new oSignal is received and not consumed in ServiceStateMachine.
        *                                         2) This job is the current job.
        *
        * @return                                 Whether the oSignal is consumed or not.
        *                                             true: consumed.
        *                                             false: not consumed.
        */
        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> /*oSignal*/) = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when to start this JobStateMachine.
        *
        * @return
        */
        virtual void Start() = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when to stop this JobStateMachine.
        *
        * @return
        */
        virtual void Stop() = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when to reset this JobStateMachine to default state.
        *
        * @return
        */
        virtual void Reset() = 0;

        /**
        * @brief                                  Function called when to post an event to this JobStateMachine instance.
        *
        * @param[in] nEventIndex                  The event index, which should be unique in the scope of this JobStateMachine instance.
        * @param[in] data                         The data attached with the event.
        *
        * @return
        */
        virtual void PostEvent(const uint32_t /*nEventIndex*/, const std::shared_ptr<BSEventDataBase> /*data*/ = 0) = 0;

        /**
        * @brief                                  Function called when to start a timer.
        *
        * @param[in] timeoutMicroSeconds          Timeout value.
        * @param[in] nEventIndex                  The event index, which should be unique in the scope of this JobStateMachine instance.
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
        * @brief                                  Function called by ServiceStateMachine when to start to handle a new job.
        *                                         It will cancel the current job firstly if necessary.
        *
        * @return
        */
        virtual void StartNewJob() = 0;

        /**
        * @brief                                  Function called when to cancel handling of the current job.
        *
        * @return
        */
        virtual void CancelCurrentJob() = 0;

        /**
        * @brief                                  Function called by JobEntity when the job has finished.
        *
        * @param[in] jobState                     The job finish state.
        *
        * @return
        */
        virtual void OnJobFinished(const JobState &/*jobState*/) = 0;

        /**
        * @brief                                  Function called by JobEntity when to get the current job state.
        *
        * @return                                 The current job state.
        */
        virtual const JobState GetJobState() = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine to check whether this job should be ignored.
        *
        * @return                                 Whether this job should be ignored.
        *                                             true: ignored.
        *                                             false: not ignored.
        */
        virtual const bool GetJobIgnored() = 0;

        /**
        * @brief                                  Function called by JobEntity when to get the current JobStateMachine state.
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

#endif // #ifndef BSFWK_I_JOB_STATE_MACHINE_H
