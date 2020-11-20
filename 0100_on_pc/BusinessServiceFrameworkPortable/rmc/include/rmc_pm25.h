#ifndef RMC_PM25_H
#define RMC_PM25_H

#include "rmc_global.h"
#include "rmc_common.h"

namespace volvo_on_call {

    using namespace bsfwk;

	struct Context_RmcPM25 : public Context_RmcCommon
	{
		Context_RmcPM25()
			: Context_RmcCommon()
			, m_eventPMLevelState()
        {
            BSFWK_LOG_GEN_PRINTF("\n"); 
        }

        virtual ~Context_RmcPM25()
        {
            BSFWK_LOG_GEN_PRINTF("\n"); 
        }

		std::shared_ptr<vc::EventPMLevelState> m_eventPMLevelState;
	};

	class ServiceEntity_RMCPM25 : public ServiceEntity_RMCC
	{
	public:
		typedef Context_RmcPM25 Context;

	public:
		ServiceEntity_RMCPM25(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName, Context &rContex);
        
		virtual bool IsMySignal(const uint32_t nSignalId) const;
        virtual void SendResponse();
	};

	class JobEntity_RMCPM25_SendRequest : public JobEntityBase
	{
	public:
		typedef Context_RmcPM25 Context;

	public:
		JobEntity_RMCPM25_SendRequest(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex);
        virtual ~JobEntity_RMCPM25_SendRequest() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }

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

	class EntityFactory_RMCPM25 : public EntityFactoryBase
	{
	public:
		typedef Context_RmcPM25 Context;

	public:
		EntityFactory_RMCPM25() : EntityFactoryBase(), m_context() {}

		virtual std::shared_ptr<IServiceEntity> CreateServiceEntity(IServiceStateMachine &rServiceStateMachine);
		virtual std::shared_ptr<IJobEntity> CreateJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex);
		virtual const uint32_t GetJobCount() const;
        virtual const std::string GetServiceStateMachineName();
        virtual const std::string GetJobStateMachineName(const uint32_t nIndex);

	protected:
		Context m_context;
	};

} // namespace volvo_on_call {

#endif // #ifndef RMC_PM25_H
