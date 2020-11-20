#include "rmc_door_tailgate.h"

namespace volvo_on_call {

    ServiceEntity_RMCDTG::ServiceEntity_RMCDTG(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName, Context &rContex)
        : ServiceEntity_RMCC(rServiceStateMachine, serviceName, rContex)
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    bool ServiceEntity_RMCDTG::IsMySignal(const uint32_t nSignalId) const
    {
        return ((nSignalId == fsm::kVDServiceRdl)
            || (nSignalId == fsm::kVDServiceRdu)
            || (nSignalId == fsm::kVDServiceRtl)
            || (nSignalId == fsm::kVDServiceRtu)
            );
    }

    void ServiceEntity_RMCDTG::SendResponse()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

#ifdef TCAM_TARGET
        if (m_rContex.m_tiggerSignal) {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            m_rContex.m_tiggerSignal->SetResultPayload(m_rContex.m_result);
            m_rContex.m_tiggerSignal->SetRmcStatusPayload(RmcResponseTypeDoor, m_rContex.m_status);
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

    JobEntity_RMCDTG_SendRequest::JobEntity_RMCDTG_SendRequest(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex)
        : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex), m_transectionId()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    const bool JobEntity_RMCDTG_SendRequest::OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
    {
        bool bConsumed = false;

        std::shared_ptr<fsm::DoorLockStatusSignal> sig = std::dynamic_pointer_cast<fsm::DoorLockStatusSignal>(signal);
        if (sig != 0 && sig->GetTransactionId() == m_transectionId) {
            BSFWK_LOG_JOB_GEN_PRINTF("\n");
            m_rContex.m_eventDoorLockState = sig->GetData();
            bConsumed = true;
            GetJobStateMachine().OnJobFinished(JobState_Completed);
        }

        return bConsumed;
    }

    void JobEntity_RMCDTG_SendRequest::OnJobBegin()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
        m_transectionId = fsm::VehicleCommTransactionId();
        vc::ReqDoorLockUnlock request{};
        if (GetVehicleCommonGeneral().Request_LockDoor(&request, m_transectionId.GetId()) != vc::RET_OK) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }
    }

    std::shared_ptr<IServiceEntity> EntityFactory_RMCDTG::CreateServiceEntity(IServiceStateMachine &rServiceStateMachine)
    {
        return std::make_shared<ServiceEntity_RMCDTG>(rServiceStateMachine, std::string("Service_RMCDTG"), m_context);
    }

    std::shared_ptr<IJobEntity> EntityFactory_RMCDTG::CreateJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex)
    {
        std::shared_ptr<IJobEntity> pJobEntity = 0;

        switch (nIndex) {
            case 0:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCC_RequestRMTStatus>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCDTG_RequestRMTStatus"), m_context);
                }
                break;
            case 1:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCC_RequestPosition>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCDTG_RequestPosition"), m_context);
                }
                break;
            case 2:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCC_RequestVFCActivate>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCDTG_RequestVFCActivate"), m_context);
                }
                break;
            case 3:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCC_RequestCarMode_RequestUsageMode>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCDTG_RequestCarMode_RequestUsageMode"), m_context);
                }
                break;
            case 4:
                {
                    pJobEntity = std::make_shared<JobEntity_RMCDTG_SendRequest>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("Job_RMCDTG_SendRequest"), m_context);
                }
                break;
            default:
                {
                }
                break;
        }

        return pJobEntity;
    }

    const uint32_t EntityFactory_RMCDTG::GetJobCount() const
    {
        return 5;
    }

    const std::string EntityFactory_RMCDTG::GetServiceStateMachineName()
    {
        return std::string("ServiceStateMachine_RMCDTG");
    }

    const std::string EntityFactory_RMCDTG::GetJobStateMachineName(const uint32_t nIndex)
    {
        std::string JobStateMachineName;

        JobStateMachineName = std::string("JobStateMachine_RMCDTG_");
        JobStateMachineName += FormatStr("%u", nIndex);
        return JobStateMachineName;
    }


} // namespace volvo_on_call {
