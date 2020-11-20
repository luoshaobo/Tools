#include "theft_notification.h"

namespace volvo_on_call {

    ServiceEntity_TN::ServiceEntity_TN(IServiceStateMachine &rServiceStateMachine, const std::string &serviceName, Context &rContex)
        : ServiceEntityBase(rServiceStateMachine, serviceName), m_rContex(rContex)
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    ServiceEntity_TN::~ServiceEntity_TN()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    const bool ServiceEntity_TN::OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
    {
        bool bConsumed = false;

        std::shared_ptr<GlyVdsTheftSignal> sig = std::dynamic_pointer_cast<GlyVdsTheftSignal>(signal);
        if (sig != 0) {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            std::shared_ptr<ServiceSignalRequest> request(new ServiceSignalRequest(signal));
            GetServiceStateMachine().AddRequest(request);
            bConsumed = true;
        }

        return bConsumed;
    }

    void ServiceEntity_TN::OnBSEvent(const std::shared_ptr<BSEvent> event)
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    void ServiceEntity_TN::OnStateMachineStart()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    void ServiceEntity_TN::OnStateMachineStop()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    void ServiceEntity_TN::OnServiceBegin(const std::shared_ptr<ServiceRequestBase> request)
    {
        std::shared_ptr<ServiceSignalRequest> req = std::dynamic_pointer_cast<ServiceSignalRequest>(request);
        if (req != 0) {
            std::shared_ptr<GlyVdsTheftSignal> signal = std::dynamic_pointer_cast<GlyVdsTheftSignal>(req->GetSignal());
            if (signal != 0) {
                BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
                m_rContex.m_tiggerSignal = signal;
#ifndef TCAM_TARGET
                m_rContex.m_theftUpload = signal->GetTheftPayload();
#endif // #ifdef TCAM_TARGET
            }
        }
    }

    void ServiceEntity_TN::OnServiceEnd()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    void ServiceEntity_TN::OnServiceSucceeded()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
        NotifyTsp();
    }

    void ServiceEntity_TN::OnServiceFailed(const uint32_t nJobIndex, const uint32_t nIndex)
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    void ServiceEntity_TN::OnServiceTimedout(const uint32_t nIndex)
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    void ServiceEntity_TN::OnServiceCancelled()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
    }

    void ServiceEntity_TN::NotifyTsp()
    {
        BSFWK_LOG_SERVICE_GEN_PRINTF("\n");

#ifdef TCAM_TARGET
        if (m_rContex.m_tiggerSignal) {
            BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            TheftUpload_t theft_upload;
            Timestamp::TimeVal utcTime = GetUtcTime();
            theft_upload.TimeSeconds= utcTime/1000000;
            theft_upload.TimeMilliseconds= (utcTime%1000000)/1000;
            theft_upload.activated = Actvd;
            m_rContex.m_tiggerSignal->SetTheftPayload(theft_upload);
            fsm::VocmoSignalSource vocmo_signal_source;
            if (vocmo_signal_source.GeelySendMessage(m_rContex.m_tiggerSignal)) {
                BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            } else {
                BSFWK_LOG_SERVICE_GEN_PRINTF("\n");
            }
        }
#else
        BSFWK_LOG_SERVICE_GEN_PRINTF("NotifyTsp(): m_rContex.m_theftUpload.activated=%d\n", m_rContex.m_theftUpload.activated, m_rContex.m_theftUpload.activated);
#endif // #ifdef TCAM_TARGET
    }

    JobEntity_TN_GetCarCfgTheftNotification::JobEntity_TN_GetCarCfgTheftNotification(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex)
        : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex), m_transectionId(), m_delayConfig()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        m_delayConfig.SetEnabled(true);
        m_delayConfig.SetDelayTime(1000);
    }

    JobEntity_TN_GetCarCfgTheftNotification::~JobEntity_TN_GetCarCfgTheftNotification()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    const bool JobEntity_TN_GetCarCfgTheftNotification::OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
    {
        bool bConsumed = false;

        if (!bConsumed) {
            std::shared_ptr<fsm::GetTheftNotificationSignal> sig = std::dynamic_pointer_cast<fsm::GetTheftNotificationSignal>(signal);
            if (sig != 0 && sig->GetTransactionId() == m_transectionId) {
                BSFWK_LOG_JOB_GEN_PRINTF("\n");
                m_rContex.m_resGetTheftNotification = sig->GetData();
                bConsumed = true;

                if ((m_rContex.m_resGetTheftNotification->theft_notification_validity == vc::CARCONF_NOT_VALID)
                    || (m_rContex.m_resGetTheftNotification->theft_notification == vc::HEFT_NOTIFICATION_INVALID)
                    || (m_rContex.m_resGetTheftNotification->theft_notification == vc::WITHOUT_THEFT_NOTIFICATION)
                    ) {
                    GetServiceStateMachine().CancelCurrentRequest();
                } else {
                    GetJobStateMachine().OnJobFinished(JobState_Completed);
                }
            }
        }

        return bConsumed;
    }

    void JobEntity_TN_GetCarCfgTheftNotification::OnBSEvent(const std::shared_ptr<BSEvent> event)
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_GetCarCfgTheftNotification::OnStateMachineStart()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_GetCarCfgTheftNotification::OnStateMachineStop()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_GetCarCfgTheftNotification::OnJobBegin()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        m_transectionId = fsm::VehicleCommTransactionId();
        if (GetVehicleCommonGeneral().Request_GetCarCfgTheftNotification(m_transectionId.GetId()) != vc::RET_OK) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }
    }

    void JobEntity_TN_GetCarCfgTheftNotification::OnJobEnd()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_GetCarCfgTheftNotification::OnJobSucceeded()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_GetCarCfgTheftNotification::OnJobFailed(const uint32_t nIndex)
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_GetCarCfgTheftNotification::OnJobTimedout(const uint32_t nIndex)
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_GetCarCfgTheftNotification::OnJobCancelled()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    const DelayConfig &JobEntity_TN_GetCarCfgTheftNotification::GetDelayConfig()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
        return m_delayConfig;
    }

    JobEntity_TN_RequestCarMode_RequestUsageMode::JobEntity_TN_RequestCarMode_RequestUsageMode(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex, const std::string &jobName, Context &rContex)
        : JobEntityBase(rServiceStateMachine, rJobStateMachine, nIndex, jobName), m_rContex(rContex), m_transectionId1(), m_transectionId2(), m_bCarModeRes(false), m_bCarUsageModeRes(false), m_delayConfig()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        m_delayConfig.SetEnabled(false);
        m_delayConfig.SetDelayTime(1000);
    }

    JobEntity_TN_RequestCarMode_RequestUsageMode::~JobEntity_TN_RequestCarMode_RequestUsageMode()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    const bool JobEntity_TN_RequestCarMode_RequestUsageMode::OnHandleSignal(const std::shared_ptr<fsm::Signal> signal)
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

            if ((m_rContex.m_resCarMode->carmode == vc::CAR_NORMAL)
                || (m_rContex.m_resCarMode->carmode == vc::CAR_CRASH)
                || (m_rContex.m_resCarMode->carmode == vc::CAR_DYNO)
                ) {
                GetJobStateMachine().OnJobFinished(JobState_Completed);
            } else {
                GetServiceStateMachine().CancelCurrentRequest();
            }
        }

        return bConsumed;
    }

    void JobEntity_TN_RequestCarMode_RequestUsageMode::OnBSEvent(const std::shared_ptr<BSEvent> event)
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_RequestCarMode_RequestUsageMode::OnStateMachineStart()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_RequestCarMode_RequestUsageMode::OnStateMachineStop()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_RequestCarMode_RequestUsageMode::OnJobBegin()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        m_bCarModeRes = false;
        m_bCarUsageModeRes = false;

        m_transectionId1 = fsm::VehicleCommTransactionId();
        if (GetVehicleCommonGeneral().Request_CarMode(m_transectionId1.GetId()) != vc::RET_OK) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }

        m_transectionId2 = fsm::VehicleCommTransactionId();
        if (GetVehicleCommonGeneral().Request_CarUsageMode(m_transectionId2.GetId()) != vc::RET_OK) {
            GetJobStateMachine().OnJobFinished(JobState_Failed);
        }
    }

    void JobEntity_TN_RequestCarMode_RequestUsageMode::OnJobEnd()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_RequestCarMode_RequestUsageMode::OnJobSucceeded()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_RequestCarMode_RequestUsageMode::OnJobFailed(const uint32_t nIndex)
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_RequestCarMode_RequestUsageMode::OnJobTimedout(const uint32_t nIndex)
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    void JobEntity_TN_RequestCarMode_RequestUsageMode::OnJobCancelled()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");
    }

    const DelayConfig &JobEntity_TN_RequestCarMode_RequestUsageMode::GetDelayConfig()
    {
        BSFWK_LOG_JOB_GEN_PRINTF("\n");

        return m_delayConfig;
    }

    std::shared_ptr<IServiceEntity> EntityFactory_TN::CreateServiceEntity(IServiceStateMachine &rServiceStateMachine)
    {
        return std::make_shared<ServiceEntity_TN>(rServiceStateMachine, std::string("Service_TN"), m_context);
    }

    std::shared_ptr<IJobEntity> EntityFactory_TN::CreateJobEntity(IServiceStateMachine &rServiceStateMachine, IJobStateMachine &rJobStateMachine, const uint32_t nIndex)
    {
        std::shared_ptr<IJobEntity> pJobEntity = 0;

        switch (nIndex) {
            case 0:
                {
                    pJobEntity = std::make_shared<JobEntity_TN_GetCarCfgTheftNotification>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("JobEntity_TN_GetCarCfgTheftNotification"), m_context);
                }
                break;
            case 1:
                {
                    pJobEntity = std::make_shared<JobEntity_TN_RequestCarMode_RequestUsageMode>(rServiceStateMachine, rJobStateMachine, nIndex, std::string("JobEntity_TN_RequestCarMode_RequestUsageMode"), m_context);
                }
                break;
            default:
                {
                }
                break;
        }

        return pJobEntity;
    }

    const uint32_t EntityFactory_TN::GetJobCount() const
    {
        return 2;
    }

    const std::string EntityFactory_TN::GetServiceStateMachineName()
    {
        return std::string("ServiceStateMachine_TN");
    }

    const std::string EntityFactory_TN::GetJobStateMachineName(const uint32_t nIndex)
    {
        std::string sJobStateMachineName;

        switch (nIndex) {
            case 0:
                {
                    sJobStateMachineName = std::string("JobStateMachine_TN_GetCarCfgTheftNotification");
                }
                break;
            case 1:
                {
                    sJobStateMachineName = std::string("JobStateMachine_TN_RequestCarMode_RequestUsageMode");
                }
                break;
            default:
                {
                }
                break;
        }
        return sJobStateMachineName;
    }

} // namespace volvo_on_call {
