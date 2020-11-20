#ifndef THEFT_NOTIFICATION_H
#define THEFT_NOTIFICATION_H

#include "theft_notification_global.h"

namespace volvo_on_call {

    using namespace bsfwk;

    class Context_TN
    {
    public:
        Context_TN()
            : m_tiggerSignal()
            , m_theftUpload()
            , m_resGetTheftNotification()
            , m_resCarMode()
            , m_resCarUsageMode()
        {
            BSFWK_LOG_GEN_PRINTF("\n");
        }

        ~Context_TN()
        {
            BSFWK_LOG_GEN_PRINTF("\n");
        }

    public:
        std::shared_ptr<GlyVdsTheftSignal> m_tiggerSignal;
        TheftUpload_t m_theftUpload;
        std::shared_ptr<vc::ResGetTheftNotification> m_resGetTheftNotification;
        std::shared_ptr<vc::ResCarMode> m_resCarMode;
        std::shared_ptr<vc::ResCarUsageMode> m_resCarUsageMode;
    };

    class ServiceEntity_TN : public ServiceEntityBase
    {
    public:
        typedef Context_TN Context;

    public:
        ServiceEntity_TN(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName, Context &rContex);
        virtual ~ServiceEntity_TN();

        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal);
        virtual void OnBSEvent(const std::shared_ptr<BSEvent> event);
        virtual void OnStateMachineStart();
        virtual void OnStateMachineStop();
        virtual void OnServiceBegin(const std::shared_ptr<ServiceRequestBase> request);
        virtual void OnServiceEnd();
        virtual void OnServiceSucceeded();
        virtual void OnServiceFailed(const uint32_t nJobIndex, const uint32_t nIndex);
        virtual void OnServiceTimedout(const uint32_t nIndex);
        virtual void OnServiceCancelled();

    private:
        void NotifyTsp();

    protected:
        Context &m_rContex;
    };

    class JobEntity_TN_GetCarCfgTheftNotification : public JobEntityBase
    {
    public:
        typedef Context_TN Context;

    public:
        JobEntity_TN_GetCarCfgTheftNotification(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex);
        virtual ~JobEntity_TN_GetCarCfgTheftNotification();

        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal);
        virtual void OnBSEvent(const std::shared_ptr<BSEvent> event);
        virtual void OnStateMachineStart();
        virtual void OnStateMachineStop();
        virtual void OnJobBegin();
        virtual void OnJobEnd();
        virtual void OnJobSucceeded();
        virtual void OnJobFailed(const uint32_t nIndex);
        virtual void OnJobTimedout(const uint32_t nIndex);
        virtual void OnJobCancelled();
        virtual const DelayConfig &GetDelayConfig();

    protected:
        Context &m_rContex;
        fsm::VehicleCommTransactionId m_transectionId;
        DelayConfig m_delayConfig;
    };

    class JobEntity_TN_RequestCarMode_RequestUsageMode : public JobEntityBase
    {
    public:
        typedef Context_TN Context;

    public:
        JobEntity_TN_RequestCarMode_RequestUsageMode(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex);
        virtual ~JobEntity_TN_RequestCarMode_RequestUsageMode();

        virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal);
        virtual void OnBSEvent(const std::shared_ptr<BSEvent> event);
        virtual void OnStateMachineStart();
        virtual void OnStateMachineStop();
        virtual void OnJobBegin();
        virtual void OnJobEnd();
        virtual void OnJobSucceeded();
        virtual void OnJobFailed(const uint32_t nIndex);
        virtual void OnJobTimedout(const uint32_t nIndex);
        virtual void OnJobCancelled();
        virtual const DelayConfig &GetDelayConfig();

    protected:
        Context &m_rContex;
        fsm::VehicleCommTransactionId m_transectionId1;
        fsm::VehicleCommTransactionId m_transectionId2;
        bool m_bCarModeRes;
        bool m_bCarUsageModeRes;
        DelayConfig m_delayConfig;
    };

    class EntityFactory_TN : public EntityFactoryBase
    {
    public:
        typedef Context_TN Context;

    public:
        EntityFactory_TN() : EntityFactoryBase(), m_context() {}

        virtual std::shared_ptr<IServiceEntity> CreateServiceEntity(IServiceStateMachine &rServiceStateMachine);
        virtual std::shared_ptr<IJobEntity> CreateJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex);
        virtual const uint32_t GetJobCount() const;
        virtual const std::string GetServiceStateMachineName();
        virtual const std::string GetJobStateMachineName(const uint32_t nIndex);

    protected:
        Context m_context;
    };

} // namespace volvo_on_call {

#endif // #ifndef THEFT_NOTIFICATION_H
