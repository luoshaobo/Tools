#ifndef RMC_COMMON_H
#define RMC_COMMON_H

#include "rmc_global.h"

namespace volvo_on_call {

	const int RemoteCtrlVFCIndex_Clima = 3;
	const int RemoteCtrlVFCIndex_HornLi = 10;
	const int RemoteCtrlVFCIndex_Locking = 16;
	const int RemoteCtrlVFCIndex_Window = 37;

    enum ErrorCode {
    	ErrorCode_incorrectState	= 1,
    	ErrorCode_invalidData	= 2,
    	ErrorCode_invalidFormat	= 3,
    	ErrorCode_powderModeNotCorrect	= 4,
    	ErrorCode_conditionsNotCorrect	= 5,
    	ErrorCode_executionFailure	= 6,
    	ErrorCode_permissionDenied	= 7,
    	ErrorCode_timeout	= 8,
    	ErrorCode_backupBatteryUsage	= 9
    };

	using namespace bsfwk;

	struct Context_RmcCommon
	{
		Context_RmcCommon()
			: m_tiggerSignal()
			, m_currentRequest()
            , m_result()
            , m_status()
			, m_resRMTStatus()
			, m_resVFCActivate()
			, m_resCarMode()
			, m_resCarUsageMode()
			, m_GNSSData()
		{
            BSFWK_LOG_GEN_PRINTF("\n"); 
        }

        virtual ~Context_RmcCommon()
        {
            BSFWK_LOG_GEN_PRINTF("\n"); 
        }

        std::shared_ptr<GlyVdsRmcSignal> m_tiggerSignal;
		std::shared_ptr<fsm::RemoteCtrlBasicRequest> m_currentRequest;
        fsm::RemoteCtrlBasicResult m_result;
        fsm::RemoteCtrlBasicStatus m_status;
		std::shared_ptr<vc::ResRMTStatus> m_resRMTStatus;
		std::shared_ptr<vc::ResVFCActivate> m_resVFCActivate;
		std::shared_ptr<vc::ResCarMode> m_resCarMode;
		std::shared_ptr<vc::ResCarUsageMode> m_resCarUsageMode;
		vpom::GNSSData m_GNSSData;
	};

	class ServiceEntity_RMCC : public ServiceEntityBase
	{
    public:
        typedef Context_RmcCommon Context;
        
	public:
		ServiceEntity_RMCC(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName, Context &rContex);
        virtual ~ServiceEntity_RMCC() { BSFWK_LOG_SERVICE_GEN_PRINTF("\n"); }

		virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal);
		virtual void OnBSEvent(const std::shared_ptr<BSEvent> event) { BSFWK_LOG_SERVICE_GEN_PRINTF("\n"); }
		virtual void OnStateMachineStart() { BSFWK_LOG_SERVICE_GEN_PRINTF("\n"); }
		virtual void OnStateMachineStop() { BSFWK_LOG_SERVICE_GEN_PRINTF("\n"); }
		virtual void OnServiceBegin(const std::shared_ptr<ServiceRequestBase> request);
		virtual void OnServiceEnd();
        virtual void OnServiceSucceeded();
		virtual void OnServiceFailed(const uint32_t nJobIndex, const uint32_t nIndex);
		virtual void OnServiceTimedout(const uint32_t nIndex);
		virtual void OnServiceCancelled();

	protected:
		virtual bool IsMySignal(const uint32_t nSignalId) const = 0;
        virtual void SendResponse() = 0;

	protected:
		Context &m_rContex;
	};

	class JobEntity_RMCC_RequestRMTStatus : public JobEntityBase
	{
    public:
        typedef Context_RmcCommon Context;
        
	public:
		JobEntity_RMCC_RequestRMTStatus(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex);
        virtual ~JobEntity_RMCC_RequestRMTStatus() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }

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

	class JobEntity_RMCC_RequestPosition : public JobEntityBase
	{
    public:
        typedef Context_RmcCommon Context;
        
	public:
		JobEntity_RMCC_RequestPosition(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex);
        virtual ~JobEntity_RMCC_RequestPosition() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }

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
		fsm::VpomTransactionId m_transectionId;
	};

	class JobEntity_RMCC_RequestVFCActivate : public JobEntityBase
	{
    public:
        typedef Context_RmcCommon Context;
        
	public:
		JobEntity_RMCC_RequestVFCActivate(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex);
        virtual ~JobEntity_RMCC_RequestVFCActivate() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }

		virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal);
		virtual void OnBSEvent(const std::shared_ptr<BSEvent> event) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnStateMachineStart() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnStateMachineStop() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobBegin();
        virtual void OnJobEnd();
        virtual void OnJobSucceeded();
        virtual void OnJobFailed(const uint32_t nIndex);
        virtual void OnJobTimedout(const uint32_t nIndex);
		virtual void OnJobCancelled() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual const bool GetJobIgnored() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); return false; }
        virtual const RetryConfig &GetRetryConfig();

	protected:
		Context &m_rContex;
		fsm::VehicleCommTransactionId m_transectionId;
        RetryConfig m_retryConfig;
	};

	class JobEntity_RMCC_RequestCarMode_RequestUsageMode : public JobEntityBase
	{
    public:
        typedef Context_RmcCommon Context;
        
	public:
		JobEntity_RMCC_RequestCarMode_RequestUsageMode(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex);
        virtual ~JobEntity_RMCC_RequestCarMode_RequestUsageMode() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }

		virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal);
		virtual void OnBSEvent(const std::shared_ptr<BSEvent> event) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnStateMachineStart();
		virtual void OnStateMachineStop() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobBegin();
		virtual void OnJobEnd() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
        virtual void OnJobSucceeded() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobFailed(const uint32_t nIndex) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobTimedout(const uint32_t nIndex) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobCancelled() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual const bool GetJobIgnored() {  BSFWK_LOG_JOB_GEN_PRINTF("\n"); return false; }
        virtual const DelayConfig &GetDelayConfig();

	protected:
		Context &m_rContex;
		fsm::VehicleCommTransactionId m_transectionId1;
		fsm::VehicleCommTransactionId m_transectionId2;
		bool m_bCarModeRes;
		bool m_bCarUsageModeRes;
        DelayConfig m_delayConfig;
	};

	class JobEntity_RMCC_SendRequest : public JobEntityBase
	{
    public:
        typedef Context_RmcCommon Context;
        
	public:
		JobEntity_RMCC_SendRequest(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex)
			: JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex)
		{
			BSFWK_LOG_JOB_GEN_PRINTF("\n");
		}
        virtual ~JobEntity_RMCC_SendRequest() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }

		virtual const bool OnHandleSignal(const std::shared_ptr<fsm::Signal> signal) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); return false; }
		virtual void OnStateMachineStart() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnStateMachineStop() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobBegin() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobEnd() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
        virtual void OnJobSucceeded() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobFailed(const uint32_t nIndex) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobTimedout(const uint32_t nIndex) { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual void OnJobCancelled() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); }
		virtual const bool GetJobIgnored() { BSFWK_LOG_JOB_GEN_PRINTF("\n"); return false; }

	protected:
		Context &m_rContex;
	};

} // namespace volvo_on_call {

#endif // #ifndef RMC_COMMON_H
