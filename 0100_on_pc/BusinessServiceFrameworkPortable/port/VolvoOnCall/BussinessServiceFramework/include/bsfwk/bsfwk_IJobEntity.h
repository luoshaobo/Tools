#ifndef BSFWK_I_JOB_ENTITY_H
#define BSFWK_I_JOB_ENTITY_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class IJobEntity
    {
    public:

        virtual ~IJobEntity() {}

        /**
        * @brief                                  Function called by JobStateMachine when an oSignal received.
        * 
        * @param[in] oSignal                       The received oSignal.
        *
        * @return                                 Whether the oSignal is consumed or not.
        *                                             true: consumed.
        *                                             false: not consumed.
        */
        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> /*oSignal*/) = 0;

        /**
        * @brief                                  Function called by JobStateMachine when an BSEvent received.
        *
        * @param[in] event                        The received event.
        *
        * @return
        */
        virtual void OnBSEvent(const std::shared_ptr<BSEvent> /*event*/) = 0;

        /**
        * @brief                                  Function called by JobStateMachine when it starts.
        *
        * @return
        */
        virtual void OnStateMachineStart() = 0;

        /**
        * @brief                                  Function called by JobStateMachine when it stops.
        *
        * @return
        */
        virtual void OnStateMachineStop() = 0;

        /**
        * @brief                                  Function called by JobStateMachine when it begins to handle a new job.
        *
        * @return
        */
        virtual void OnJobBegin() = 0;

        /**
        * @brief                                  Function called by JobStateMachine when it ends to handle the current job.
        *
        * @return
        */
        virtual void OnJobEnd() = 0;

        /**
        * @brief                                  Function called by JobStateMachine when the handling of the current job is succesful.
        *
        * @return
        */
        virtual void OnJobSucceeded() = 0;

        /**
        * @brief                                  Function called by JobStateMachine when the handling of the current job is failed.
        *
        * @param[in] nIndex                       The index of the failure.
        *
        * @return
        */
        virtual void OnJobFailed(const uint32_t /*nIndex*/) = 0;

        /**
        * @brief                                  Function called by JobStateMachine when the handling of the current job is timed out.
        *
        * @param[in] nIndex                       The index of the timeout.
        *
        * @return
        */
        virtual void OnJobTimedout(const uint32_t /*nIndex*/) = 0;

        /**
        * @brief                                  Function called by JobStateMachine when the handling of the current job is cancelled.
        *
        * @return
        */
        virtual void OnJobCancelled() = 0;

        /**
        * @brief                                  Function called by JobStateMachine for the retry configuration.
        *
        * @return                                 The retry configuration.
        */
        virtual const RetryConfig &GetRetryConfig() = 0;

        /**
        * @brief                                  Function called by JobStateMachine for a delay before the job.
        *
        * @return                                 The timer configuration.
        */
        virtual const DelayConfig &GetDelayConfig() = 0;

        /**
        * @brief                                  Function called by JobStateMachine to check whether this job should be ignored.
        *
        * @return                                 Whether this job should be ignored.
        *                                             true: ignored.
        *                                             false: not ignored.
        */
        virtual const bool GetJobIgnored() = 0;

        /**
        * @brief                                  Function called to get the job name.
        *
        * @return                                 The job name.
        */
        virtual const std::string GetJobName() = 0;
    };

} // namespace bsfwk {

#endif // #ifndef BSFWK_I_JOB_ENTITY_H
