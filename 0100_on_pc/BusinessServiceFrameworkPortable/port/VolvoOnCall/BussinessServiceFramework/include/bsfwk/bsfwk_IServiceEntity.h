#ifndef BSFWK_I_SERVICE_ENTITY_H
#define BSFWK_I_SERVICE_ENTITY_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class IServiceEntity
    {
    public:

        virtual ~IServiceEntity() {}

        /**
        * @brief                                  Function called by ServiceStateMachine when an oSignal received.
        * 
        * @param[in] oSignal                       The received oSignal.
        *
        * @return                                 Whether the oSignal is consumed or not.
        *                                             true: consumed.
        *                                             false: not consumed.
        */
        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> /*oSignal*/) = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when an BSEvent received.
        *
        * @param[in] event                        The received event.
        *
        * @return
        */
        virtual void OnBSEvent(const std::shared_ptr<BSEvent> /*event*/) = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when it starts.
        *
        * @return
        */
        virtual void OnStateMachineStart() = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when it stops.
        *
        * @return
        */
        virtual void OnStateMachineStop() = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when it begins to handle a request.
        *
        * @param[in] pRequest                     The new reqeust.
        *
        * @return
        */
        virtual void OnServiceBegin(const std::shared_ptr<ServiceRequestBase> /*request*/) = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when it ends to handle the current request.
        *
        * @return
        */
        virtual void OnServiceEnd() = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when the handling of the current request is succesful.
        *
        * @return
        */
        virtual void OnServiceSucceeded() = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when the handling of the current request is failed.
        *
        * @param[in] nJobIndex                    The index of the failed job.
        * @param[in] nIndex                       The index of the failure.
        *
        * @return
        */
        virtual void OnServiceFailed(const uint32_t /*nJobIndex*/, const uint32_t /*nIndex*/) = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when the handling of the current request is timed out.
        *
        * @param[in] nIndex                       The index of the timeout.
        *
        * @return
        */
        virtual void OnServiceTimedout(const uint32_t /*nIndex*/) = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when the handling of the current request is cancelled.
        *
        * @return
        */
        virtual void OnServiceCancelled() = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine for the retry configuration.
        *
        * @return                                 The retry configuration.
        */
        virtual const RetryConfig &GetRetryConfig() = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine for the cyclical configuration.
        *
        * @return                                 The cyclical configuration.
        */
        virtual const CyclicalRequestConfig &GetCyclicalRequestConfig() = 0;

        /**
        * @brief                                  Function called to get the service name.
        *
        * @return                                 The service name.
        */
        virtual const std::string GetServiceName() = 0;
    };

} // namespace bsfwk {

#endif // #ifndef BSFWK_I_SERVICE_ENTITY_H
