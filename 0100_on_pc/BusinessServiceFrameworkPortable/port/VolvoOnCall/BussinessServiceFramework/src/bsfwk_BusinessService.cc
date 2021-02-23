#include "bsfwk_Global.h"
#include "bsfwk_BusinessService.h"

namespace bsfwk {
    uint32_t BusinessService::m_nCurrentMasterIndex = 0;

    uint32_t BusinessService::MakeNewMasterIndex()
    {
        return m_nCurrentMasterIndex++;
    }

    BusinessService::BusinessService(ThreadEventQueue *pThreadEventQueue, IEntityFactory *pEntityFactory, const std::string &businessServiceName /*= "UnknownBusinessService"*/)
        : StatemachineMaster(pThreadEventQueue)
        , m_nMasterIndex(MakeNewMasterIndex())
        , m_pThreadEventQueue(pThreadEventQueue)
        , m_pEntityFactory(pEntityFactory)
        , g_businessServiceName(businessServiceName)
        , m_pBSServiceSM(0)
        , m_BSJobSMs()
    {
        uint32_t nIndex;
        int id;

        if (IsPtrNotNull(m_pEntityFactory)) {
            id = getNewStatemachineId();
            m_pBSServiceSM = std::make_shared<BusinessServiceStateMachine>(pEntityFactory, m_nMasterIndex, SERVICE_STATEMACHINE_INDEX, m_pEntityFactory->GetServiceStateMachineName(), 
                this, this, id, id);
            startStatemachine(static_cast<IStatemachine *>(m_pBSServiceSM.get()), 0);

            for (nIndex = static_cast<uint32_t>(0); nIndex < m_pEntityFactory->GetJobCount(); nIndex++) {
                id = getNewStatemachineId();
                std::shared_ptr<BusinessJobStateMachine> pBSJobSM = std::make_shared<BusinessJobStateMachine>(pEntityFactory, m_pBSServiceSM.get(), m_nMasterIndex, nIndex, m_pEntityFactory->GetJobStateMachineName(nIndex), 
                    this, this, id, id);
                m_BSJobSMs.push_back(pBSJobSM);
                startStatemachine(static_cast<IStatemachine *>(pBSJobSM.get()), 0);
            }

            m_pBSServiceSM->SetJobs(m_BSJobSMs);
        }
    }

    BusinessService::~BusinessService()
    {
        uint32_t nIndex;

        Stop();

        for (nIndex = static_cast<uint32_t>(0); nIndex < static_cast<uint32_t>(m_BSJobSMs.size()); nIndex++) {
            std::shared_ptr<BusinessJobStateMachine> &pBSJobSM = m_BSJobSMs[nIndex];
            if (IsPtrNotNull(pBSJobSM)) {
                removeStatemachine(pBSJobSM->getStatemachineId(), false);
                pBSJobSM.reset();
            }
        }

        if (IsPtrNotNull(m_pBSServiceSM)) {
            removeStatemachine(m_pBSServiceSM->getStatemachineId(), false);
            m_pBSServiceSM.reset();
        }
    }

    uint32_t BusinessService::GetMasterIndex() const
    {
        return m_nMasterIndex;
    }

    void BusinessService::PostJobSysEvent(const uint32_t nJobIndex, const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data /*= 0*/)
    {
        BSSysEvent bsSysEvent(m_nMasterIndex, nJobIndex, nEventIndex, data);
        fireEvent(0, bsSysEvent);
    }

    void BusinessService::PostJobEvent(const uint32_t nJobIndex, const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data /*= 0*/)
    {
        BSEvent bsEvent(m_nMasterIndex, nJobIndex, nEventIndex, data);
        fireEvent(0, bsEvent);
    }

    void BusinessService::PostServiceSysEvent(const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data /*= 0*/)
    {
        BSSysEvent bsSysEvent(m_nMasterIndex, SERVICE_STATEMACHINE_INDEX, nEventIndex, data);
        fireEvent(0, bsSysEvent);
    }

    void BusinessService::PostServiceEvent(const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data /*= 0*/)
    {
        BSEvent bsEvent(m_nMasterIndex, SERVICE_STATEMACHINE_INDEX, nEventIndex, data);
        fireEvent(0, bsEvent);
    }

    void BusinessService::PostSignalEvent(const std::shared_ptr<BSSignalEventDataBase> data)
    {
        BSSignalEvent bsSignalEvent(data);
        fireEvent(0, bsSignalEvent);
    }

    void BusinessService::PostSignalEvent(const std::shared_ptr<fsm::Signal> oSignal)
    {
        std::shared_ptr<BSSignalEventDataBase> data = std::make_shared<BSSignalEventDataBase>(oSignal);
        PostSignalEvent(data);
    }

    void BusinessService::Start()
    {
        PostServiceSysEvent(static_cast<uint32_t>(BusinessServiceStateMachine::SSMEI_START_STATE_MACHINE));
    }

    void BusinessService::Stop()
    {
        PostServiceSysEvent(static_cast<uint32_t>(BusinessServiceStateMachine::SSMEI_STOP_STATE_MACHINE));
    }

    std::shared_ptr<IServiceStateMachine> BusinessService::GetServiceStateMachine()
    {
        std::shared_ptr<BusinessServiceStateMachine> pServiceStateMachine = 0;

        if (IsPtrNotNull(m_pBSServiceSM)) {
            pServiceStateMachine = m_pBSServiceSM;
        }

        return pServiceStateMachine;
    }

    uint32_t BusinessService::GetJobStateMachineCount() const
    {
        return static_cast<uint32_t>(m_BSJobSMs.size());
    }

    std::shared_ptr<IJobStateMachine> BusinessService::GetJobStateMachine(const uint32_t nIndex)
    {
        std::shared_ptr<BusinessJobStateMachine> pJobStateMachine = 0;

        if (nIndex < static_cast<uint32_t>(m_BSJobSMs.size())) {
            pJobStateMachine = m_BSJobSMs[nIndex];
        }

        return pJobStateMachine;
    }

    void BusinessService::SetLogLevel(const LogLevel &logLevel)
    {
        uint32_t i;

        if (IsPtrNotNull(m_pBSServiceSM)) {
            m_pBSServiceSM->SetLogLevel(logLevel);
        }

        for (i = static_cast<uint32_t>(0); i < static_cast<uint32_t>(m_BSJobSMs.size()); ++i) {
            BusinessJobStateMachine *pBusinessJobStateMachine = m_BSJobSMs[i].get();
            if (IsPtrNotNull(pBusinessJobStateMachine)) {
                pBusinessJobStateMachine->SetLogLevel(logLevel);
            }
        }
    }

    void BusinessService::fireEvent(IStatemachine::StatemachineId /*smid*/, Event &sendEvent)
    {
        if (IsPtrNotNull(m_pThreadEventQueue)) {
            m_pThreadEventQueue->insertEvent(sendEvent.clone());
        }
    }

    bool BusinessService::OnBSSignalEvent(const BSSignalEvent * const pEvent)
    {
        bool bConsumed = false;

        if (IsPtrNotNull(m_pBSServiceSM)) {
            bConsumed = m_pBSServiceSM->OnBSSignalEvent(pEvent);

            if (!bConsumed) {
                uint32_t nCurrentJobIndex = m_pBSServiceSM->GetCurrentJobIndex();
                if (nCurrentJobIndex < static_cast<uint32_t>(m_BSJobSMs.size())) {
                    BusinessJobStateMachine *pBSJobSM = m_BSJobSMs[nCurrentJobIndex].get();
                    if (IsPtrNotNull(pBSJobSM)) {
                        bConsumed = pBSJobSM->OnBSSignalEvent(pEvent);
                    }
                }
            }
        }

        return bConsumed;
    }

    void BusinessService::OnBSSysEvent(const BSSysEvent * const pEvent)
    {
        uint32_t nStatemachineIndex = pEvent->GetStatemachineIndex();
        if (nStatemachineIndex == SERVICE_STATEMACHINE_INDEX) {
            if (IsPtrNotNull(m_pBSServiceSM)) {
                m_pBSServiceSM->OnBSSysEvent(pEvent);
            }
        } else if (nStatemachineIndex < static_cast<uint32_t>(m_BSJobSMs.size())) {
            BusinessJobStateMachine *pBSJobSM = m_BSJobSMs[nStatemachineIndex].get();
            if (IsPtrNotNull(pBSJobSM)) {
                pBSJobSM->OnBSSysEvent(pEvent);
            }
        } else {
            // do nothing
        }
    }

    void BusinessService::OnBSEvent(const BSEvent * const pEvent)
    {
        uint32_t nStatemachineIndex = pEvent->GetStatemachineIndex();
        if (nStatemachineIndex == SERVICE_STATEMACHINE_INDEX) {
            if (IsPtrNotNull(m_pBSServiceSM)) {
                m_pBSServiceSM->OnBSEvent(pEvent);
            }
        } else if (nStatemachineIndex < static_cast<uint32_t>(m_BSJobSMs.size())) {
            BusinessJobStateMachine *pBSJobSM = m_BSJobSMs[nStatemachineIndex].get();
            if (IsPtrNotNull(pBSJobSM)) {
                pBSJobSM->OnBSEvent(pEvent);
            }
        } else {
            // do nothing
        }
    }

} // namespace bsfwk
