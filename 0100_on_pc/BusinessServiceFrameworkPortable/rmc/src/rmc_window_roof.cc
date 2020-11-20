#include "rmc_window_roof.h"

namespace volvo_on_call {

    ServiceEntity_RMCWR::ServiceEntity_RMCWR(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName, Context &rContex)
        : ServiceEntity_RMCC(rServiceStateMachine, serviceName, rContex)
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    bool ServiceEntity_RMCWR::IsMySignal(const uint32_t nSignalId) const
    {
        return ((nSignalId == fsm::kVDServiceRws));
    }

    void ServiceEntity_RMCWR::SendResponse()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

#ifdef TCAM_TARGET
        if (m_rContex.m_tiggerSignal) {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            m_rContex.m_tiggerSignal->SetResultPayload(m_rContex.m_result);
            m_rContex.m_tiggerSignal->SetRmcStatusPayload(RmcResponseTypeWin, m_rContex.m_status);
            fsm::VocmoSignalSource vocmo_signal_source;
            if (vocmo_signal_source.GeelySendMessage(m_rContex.m_tiggerSignal)) {
                BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            } else {
                BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            }
        }
#else
        volvo_on_call::simulator::GeelySendResponse(m_rContex.m_result, m_rContex.m_status);
#endif // #ifdef TCAM_TARGET
    }

    JobEntity_RMCWR_SendRequest::JobEntity_RMCWR_SendRequest(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex)
        : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex), m_transectionId()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    const bool JobEntity_RMCWR_SendRequest::OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
    {
        bool bConsumed = false;

        std::shared_ptr<fsm::WinOpenStatusSignal> sig = std::dynamic_pointer_cast<fsm::WinOpenStatusSignal>(signal);
        if (sig != 0 && sig->GetTransactionId() == m_transectionId) {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            m_rContex.m_eventWinOpenState = sig->GetData();
            bConsumed = true;
            GetJobStateMachine().OnJobFinished(JobState_Completed);
        }

        return bConsumed;
    }

    void JobEntity_RMCWR_SendRequest::OnJobBegin()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
        m_transectionId = fsm::VehicleCommTransactionId();
        vc::ReqWinCtrl request{};
        if (GetVehicleCommonGeneral().Request_WindowsCtrl(&request, m_transectionId.GetId()) != vc::RET_OK) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }
    }

    std::shared_ptr<IServiceEntity> EntityFactory_RMCWR::CreateServiceEntity(IServiceStateMachine &rServiceStateMachine)
    {
        return std::make_shared<ServiceEntity_RMCWR>(rServiceStateMachine, std::string("Service_RMCWR"), m_context);
    }

    std::shared_ptr<IJobEntity> EntityFactory_RMCWR::CreateJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex)
    {
        std::shared_ptr<IJobEntity> pJobEntity = 0;

        switch (nIndex) {
            case 0:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCC_RequestRMTStatus>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCWR_RequestRMTStatus"), m_context);
                }
                break;
            case 1:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCC_RequestPosition>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCWR_RequestPosition"), m_context);
                }
                break;
            case 2:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCC_RequestVFCActivate>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCWR_RequestVFCActivate"), m_context);
                }
                break;
            case 3:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCC_RequestCarMode_RequestUsageMode>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCWR_RequestCarMode_RequestUsageMode"), m_context);
                }
                break;
            case 4:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCWR_SendRequest>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCWR_SendRequest"), m_context);
                }
                break;
            default:
                {
                }
                break;
        }

        return pJobEntity;
    }

    const uint32_t EntityFactory_RMCWR::GetJobCount() const
    {
        return 5;
    }

    const std::string EntityFactory_RMCWR::GetServiceStateMachineName()
    {
        return std::string("ServiceStateMachine_RMCWR");
    }

    const std::string EntityFactory_RMCWR::GetJobStateMachineName(const uint32_t nIndex)
    {
        std::string JobStateMachineName;

        JobStateMachineName = std::string("JobStateMachine_RMCWR_");
        JobStateMachineName += FormatStr("%u", nIndex);
        return JobStateMachineName;
    }

} // namespace volvo_on_call {
