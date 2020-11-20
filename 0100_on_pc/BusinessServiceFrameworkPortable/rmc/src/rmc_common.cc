#include "rmc_common.h"

namespace volvo_on_call {

    ServiceEntity_RMCC::ServiceEntity_RMCC(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName, Context &rContex)
        : ServiceEntityBase(rServiceStateMachine, serviceName), m_rContex(rContex)
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    const bool ServiceEntity_RMCC::OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
    {
        bool bConsumed = false;

        std::shared_ptr<GlyVdsRmcSignal> sig = std::dynamic_pointer_cast<GlyVdsRmcSignal>(signal);
        if (sig != 0 && IsMySignal(sig->GetRequestPayload()->serviceId)) {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            std::shared_ptr<ServiceSignalRequest> request(new ServiceSignalRequest(signal));
            GetServiceStateMachine().AddRequest(request);
            bConsumed = true;
        }

        return bConsumed;
    }

    void ServiceEntity_RMCC::OnServiceBegin(const std::shared_ptr<ServiceRequestBase> request)
    {
        std::shared_ptr<ServiceSignalRequest> req = std::dynamic_pointer_cast<ServiceSignalRequest>(request);
        if (req != 0) {
            std::shared_ptr<GlyVdsRmcSignal> signal = std::dynamic_pointer_cast<GlyVdsRmcSignal>(req->GetSignal());
            if (signal != 0) {
                BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
                m_rContex.m_tiggerSignal = signal;
                m_rContex.m_currentRequest = signal->GetRequestPayload();
            }
        }
    }

    void ServiceEntity_RMCC::OnServiceEnd()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    void ServiceEntity_RMCC::OnServiceSucceeded()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        m_rContex.m_result.operationSucceeded = true;
        m_rContex.m_result.message = std::string("succeeded");
        SendResponse();
    }

    void ServiceEntity_RMCC::OnServiceFailed(const uint32_t nJobIndex, const uint32_t nIndex)
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("nJobIndex=%d, nIndex=%d\n", nJobIndex, nIndex);
        m_rContex.m_result.operationSucceeded = false;
        m_rContex.m_result.errorCode = ErrorCode_executionFailure;
        m_rContex.m_result.vehicleErrorCode = 99;
        m_rContex.m_result.message = std::string("faild");
        SendResponse();
    }

    void ServiceEntity_RMCC::OnServiceTimedout(const uint32_t nIndex)
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("nIndex=%d\n", nIndex);
        m_rContex.m_result.operationSucceeded = false;
        m_rContex.m_result.errorCode = ErrorCode_timeout;
        m_rContex.m_result.vehicleErrorCode = 199;
        m_rContex.m_result.message = std::string("timedout");
        SendResponse();
    }

    void ServiceEntity_RMCC::OnServiceCancelled()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    JobEntity_RMCC_RequestRMTStatus::JobEntity_RMCC_RequestRMTStatus(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex)
        : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex), m_transectionId(-1)
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    const bool JobEntity_RMCC_RequestRMTStatus::OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
    {
        bool bConsumed = false;

        std::shared_ptr<fsm::RMTCarStatesSignal> sig = std::dynamic_pointer_cast<fsm::RMTCarStatesSignal>(signal);
        if (sig != 0 && sig->GetTransactionId() == m_transectionId) {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            m_rContex.m_resRMTStatus = sig->GetData();
            bConsumed = true;
            GetJobStateMachine().OnJobFinished(JobState_Completed);
        }

        return bConsumed;
    }

    void JobEntity_RMCC_RequestRMTStatus::OnJobBegin()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
        m_transectionId = fsm::VehicleCommTransactionId();
        if (GetVehicleCommonGeneral().Request_GetRMTStatus(m_transectionId.GetId()) != vc::RET_OK) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }
    }

    JobEntity_RMCC_RequestPosition::JobEntity_RMCC_RequestPosition(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex)
        : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex), m_transectionId()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    const bool JobEntity_RMCC_RequestPosition::OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
    {
        bool bConsumed = false;

        std::shared_ptr<fsm::VpomGNSSPositionDataSignal> sig = std::dynamic_pointer_cast<fsm::VpomGNSSPositionDataSignal>(signal);
        if (sig != 0 && sig->GetTransactionId() == m_transectionId) {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            m_rContex.m_GNSSData = sig->GetPayload();
            bConsumed = true;
            GetJobStateMachine().OnJobFinished(JobState_Completed);
        }

        return bConsumed;
    }

    void JobEntity_RMCC_RequestPosition::OnJobBegin()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
        m_transectionId = fsm::VpomTransactionId();
        if (!GetPositioningService().GNSSPositionDataRawRequest(m_transectionId.GetId())) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }
    }

    JobEntity_RMCC_RequestVFCActivate::JobEntity_RMCC_RequestVFCActivate(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex)
        : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex), m_transectionId(), m_retryConfig()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        m_retryConfig.GetTimeoutRetryInfo().SetEnabled(true);
        m_retryConfig.GetTimeoutRetryInfo().SetTimeout(3 * 1000);
        m_retryConfig.GetTimeoutRetryInfo().SetRetryCount(0);

        m_retryConfig.GetFailureRetryInfo().SetEnabled(true);
        m_retryConfig.GetFailureRetryInfo().SetRetryCount(0);
        m_retryConfig.GetFailureRetryInfo().SetDelayTime(5 * 1000);

        m_retryConfig.SetDuration(24 * 1000);
    }

    const bool JobEntity_RMCC_RequestVFCActivate::OnHandleSignal(std::shared_ptr<fsm::Signal> signal)
    {
        bool bConsumed = false;

        std::shared_ptr<fsm::VFCActivateSignal> sig = std::dynamic_pointer_cast<fsm::VFCActivateSignal>(signal);
        if (sig != 0 && sig->GetTransactionId() == m_transectionId) {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            m_rContex.m_resVFCActivate = sig->GetData();
            bConsumed = true;
            GetJobStateMachine().OnJobFinished(JobState_Completed);
        }

        return bConsumed;
    }

    void JobEntity_RMCC_RequestVFCActivate::OnJobBegin()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
        m_transectionId = fsm::VehicleCommTransactionId();
        vc::ReqVFCActivate request;
        request.id = RemoteCtrlVFCIndex_Locking;
        request.type = 1;
        if (GetVehicleCommonGeneral().Request_VFCActivate(&request, m_transectionId.GetId()) != vc::RET_OK) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }
    }

    void JobEntity_RMCC_RequestVFCActivate::OnJobEnd()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_RMCC_RequestVFCActivate::OnJobSucceeded()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_RMCC_RequestVFCActivate::OnJobFailed(const uint32_t nIndex)
    {
        BSFWK_LOG_JOB_GEN_PRINTF("nIndex=%d\n", nIndex);
    }

    void JobEntity_RMCC_RequestVFCActivate::OnJobTimedout(const uint32_t nIndex)
    {
        BSFWK_LOG_JOB_GEN_PRINTF("nIndex=%d\n", nIndex);
    }

    const RetryConfig &JobEntity_RMCC_RequestVFCActivate::GetRetryConfig()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
        return m_retryConfig;
    }

    JobEntity_RMCC_RequestCarMode_RequestUsageMode::JobEntity_RMCC_RequestCarMode_RequestUsageMode(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex)
        : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex), m_transectionId1(), m_transectionId2()
        , m_bCarModeRes(false), m_bCarUsageModeRes(false), m_delayConfig()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        m_delayConfig.SetEnabled(true);
        m_delayConfig.SetDelayTime(3000);
    }

    const bool JobEntity_RMCC_RequestCarMode_RequestUsageMode::OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
    {
        bool bConsumed = false;

        if (!bConsumed) {
            std::shared_ptr<fsm::CarModeSignal> sig = std::dynamic_pointer_cast<fsm::CarModeSignal>(signal);
            if (sig != 0 && sig->GetTransactionId() == m_transectionId1) {
                BSFWK_LOG_JOB_GEN_PRINTF("\n");
                m_rContex.m_resCarMode = sig->GetData();
                bConsumed = true;
                m_bCarModeRes = true;
            }
        }

        if (!bConsumed) {
            std::shared_ptr<fsm::CarUsageModeSignal> sig = std::dynamic_pointer_cast<fsm::CarUsageModeSignal>(signal);
            if (sig != 0 && sig->GetTransactionId() == m_transectionId2) {
                BSFWK_LOG_JOB_GEN_PRINTF("\n");
                m_rContex.m_resCarUsageMode = sig->GetData();
                bConsumed = true;
                m_bCarUsageModeRes = true;
            }
        }

        if (m_bCarModeRes && m_bCarUsageModeRes) {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            GetJobStateMachine().OnJobFinished(JobState_Completed);
        }

        return bConsumed;
    }

    void JobEntity_RMCC_RequestCarMode_RequestUsageMode::OnStateMachineStart()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        m_bCarModeRes = false;
        m_bCarUsageModeRes = false;
    }

    void JobEntity_RMCC_RequestCarMode_RequestUsageMode::OnJobBegin()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        m_transectionId1 = fsm::VehicleCommTransactionId();
        if (GetVehicleCommonGeneral().Request_CarMode(m_transectionId1.GetId()) != vc::RET_OK) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }

        m_transectionId2 = fsm::VehicleCommTransactionId();
        if (GetVehicleCommonGeneral().Request_CarUsageMode(m_transectionId2.GetId()) != vc::RET_OK) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }
    }

    const DelayConfig &JobEntity_RMCC_RequestCarMode_RequestUsageMode::GetDelayConfig()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        return m_delayConfig;
    }

} // namespace volvo_on_call {
