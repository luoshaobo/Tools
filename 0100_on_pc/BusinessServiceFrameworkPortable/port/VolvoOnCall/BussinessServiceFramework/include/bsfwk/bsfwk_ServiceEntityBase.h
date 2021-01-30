#ifndef BSFWK_SERVICE_ENTITY_BASE_H
#define BSFWK_SERVICE_ENTITY_BASE_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class ServiceEntityBase : public IServiceEntity
    {
    public:
        ServiceEntityBase(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName = "UnknownService")
            : IServiceEntity(), m_rServiceStateMachine(rServiceStateMachine), m_serviceName(serviceName) {}
        virtual ~ServiceEntityBase() {}

        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> /*oSignal*/) { return false; }
        virtual void OnBSEvent(const std::shared_ptr<BSEvent> /*event*/) {}
        virtual void OnStateMachineStart() {}
        virtual void OnStateMachineStop() {}
        virtual void OnServiceBegin(const std::shared_ptr<ServiceRequestBase> /*request*/) {}
        virtual void OnServiceEnd() {}
        virtual void OnServiceSucceeded() {}
        virtual void OnServiceFailed(const uint32_t /*nJobIndex*/, const uint32_t /*nIndex*/) {}
        virtual void OnServiceTimedout(const uint32_t /*nIndex*/) {}
        virtual void OnServiceCancelled() {}
        virtual const RetryConfig &GetRetryConfig() { return m_defaultRetryConfig; }
        virtual const CyclicalRequestConfig &GetCyclicalRequestConfig() { return m_defaultCyclicalRequestConfig; }
        virtual bool GetJobsToDo(std::vector<uint32_t> &jobList) { return false; }
        virtual const std::string GetServiceName() { return m_serviceName; }

    protected:
        IServiceStateMachine &GetServiceStateMachine() { return m_rServiceStateMachine; }
        const IServiceStateMachine &GetServiceStateMachine() const { return m_rServiceStateMachine; }

    private:
        static RetryConfig m_defaultRetryConfig;
        static CyclicalRequestConfig m_defaultCyclicalRequestConfig;
        IServiceStateMachine &m_rServiceStateMachine;
        std::string m_serviceName;
    };

} // namespace bsfwk {

#endif // #ifndef BSFWK_SERVICE_ENTITY_BASE_H
