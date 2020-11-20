#ifndef RMC_FLASH_HORN_H
#define RMC_FLASH_HORN_H

#include "rmc_global.h"
#include "rmc_common.h"

namespace volvo_on_call {

    using namespace bsfwk;

	struct Context_RmcFlashHorn : public Context_RmcCommon
	{
		Context_RmcFlashHorn()
			: Context_RmcCommon()
			, m_resHornNLight()
		{
            BSFWK_LOG_GEN_PRINTF("\n"); 
        }

        virtual ~Context_RmcFlashHorn()
        {
            BSFWK_LOG_GEN_PRINTF("\n"); 
        }

		std::shared_ptr<vc::ResHornNLight> m_resHornNLight;
	};

	class ServiceEntity_RMCFH : public ServiceEntity_RMCC
	{
	public:
		typedef Context_RmcFlashHorn Context;

	public:
		ServiceEntity_RMCFH(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName, Context &rContex);
        
		virtual bool IsMySignal(const uint32_t nSignalId) const;
        virtual void SendResponse();
	};

	class JobEntity_RMCFH_SendRequest : public JobEntityBase
	{
	public:
		typedef Context_RmcFlashHorn Context;

	public:
		JobEntity_RMCFH_SendRequest(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex);
        virtual ~JobEntity_RMCFH_SendRequest() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }

		virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal);
		virtual void OnBSEvent(const std::shared_ptr<BSEvent> event) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnStateMachineStart() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnStateMachineStop() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobBegin();
		virtual void OnJobEnd() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
        virtual void OnJobSucceeded() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobFailed(const uint32_t nIndex) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobTimedout(const uint32_t nIndex) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobCancelled() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual const bool GetJobIgnored() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); return false; }

	protected:
		Context &m_rContex;
		fsm::VehicleCommTransactionId m_transectionId;
	};

	class EntityFactory_RMCFH : public EntityFactoryBase
	{
	public:
		typedef Context_RmcFlashHorn Context;

	public:
		EntityFactory_RMCFH() : EntityFactoryBase(), m_context() {}

		virtual std::shared_ptr<IServiceEntity> CreateServiceEntity(IServiceStateMachine &rServiceStateMachine);
		virtual std::shared_ptr<IJobEntity> CreateJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, uint32_t nIndex);
		virtual const uint32_t GetJobCount() const;
        virtual const std::string GetServiceStateMachineName();
        virtual const std::string GetJobStateMachineName(const uint32_t nIndex);

	protected:
		Context m_context;
	};

} // namespace volvo_on_call {

#endif // #ifndef RMC_FLASH_HORN_H
