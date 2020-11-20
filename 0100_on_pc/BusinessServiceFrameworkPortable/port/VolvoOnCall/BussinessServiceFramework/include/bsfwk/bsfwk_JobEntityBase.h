#ifndef BSFWK_JOB_ENTITY_BASE_H
#define BSFWK_JOB_ENTITY_BASE_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class JobEntityBase : public IJobEntity
    {
    public:
        JobEntityBase(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName = "UnknownJob")
            : IJobEntity(), m_rServiceStateMachine(rServiceStateMachine), m_rJobStateMachine(rJobStateMachine), m_nIndex(nIndex), m_jobName(jobName) {}
        virtual ~JobEntityBase() {}

        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> /*oSignal*/) { return false; }
        virtual void OnBSEvent(const std::shared_ptr<BSEvent> /*event*/) {}
        virtual void OnStateMachineStart() {}
        virtual void OnStateMachineStop() {}
        virtual void OnJobBegin() {}
        virtual void OnJobEnd() {}
        virtual void OnJobSucceeded() {}
        virtual void OnJobFailed(const uint32_t /*nIndex*/) {}
        virtual void OnJobTimedout(const uint32_t /*nIndex*/) {}
        virtual void OnJobCancelled() {}
        virtual const RetryConfig &GetRetryConfig() { return m_defaultRetryConfig; }
        virtual const DelayConfig &GetDelayConfig() { return m_defaultDelayConfig; }
        virtual const bool GetJobIgnored() { return false; }
        virtual const std::string GetJobName() { return m_jobName; }

    protected:
        IServiceStateMachine &GetServiceStateMachine() { return m_rServiceStateMachine; }
        const IServiceStateMachine &GetServiceStateMachine() const { return m_rServiceStateMachine; }
        IJobStateMachine &GetJobStateMachine() { return m_rJobStateMachine; }
        const IJobStateMachine &GetJobStateMachine() const { return m_rJobStateMachine; }
        uint32_t GetIndex() const { return m_nIndex; }

    private:
        static RetryConfig m_defaultRetryConfig;
        static DelayConfig m_defaultDelayConfig;
        IServiceStateMachine &m_rServiceStateMachine;
        IJobStateMachine &m_rJobStateMachine;
        uint32_t m_nIndex;
        std::string m_jobName;
    };

} // namespace bsfwk {

#endif // #ifndef BSFWK_JOB_ENTITY_BASE_H
