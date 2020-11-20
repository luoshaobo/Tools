#ifndef BSFWK_BUSINESS_SERVICE_H
#define BSFWK_BUSINESS_SERVICE_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class BusinessServer;

    class BusinessService : public StatemachineMaster
    {
        friend class BusinessServer;

    private:
        const uint32_t SERVICE_STATEMACHINE_INDEX = static_cast<uint32_t>(10000);

    public:
        BusinessService(ThreadEventQueue *pThreadEventQueue, IEntityFactory *pEntityFactory, const std::string &businessServiceName = "UnknownBusinessService");
        virtual ~BusinessService();

    public:
        void Start();
        void Stop();

    public:
        void SetLogLevel(const LogLevel &logLevel);

    public:
        void PostJobEvent(const uint32_t nJobIndex, const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data = 0);
        void PostServiceEvent(const uint32_t nEventIndex, const std::shared_ptr<BSEventDataBase> data = 0);

    private:
        void PostJobSysEvent(const uint32_t nJobIndex, const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data = 0);
        void PostServiceSysEvent(const uint32_t nEventIndex, const std::shared_ptr<BSSysEventDataBase> data = 0);
        void PostSignalEvent(const std::shared_ptr<BSSignalEventDataBase> data);
        void PostSignalEvent(const std::shared_ptr<fsm::Signal> oSignal);

    private:
        virtual void fireEvent(IStatemachine::StatemachineId smid, Event &sendEvent); // from StatemachineMaster2

    private:
        uint32_t GetMasterIndex() const;
        std::shared_ptr<IServiceStateMachine> GetServiceStateMachine();
        uint32_t GetJobStateMachineCount() const;
        std::shared_ptr<IJobStateMachine> GetJobStateMachine(const uint32_t nIndex);

    private:
        bool OnBSSignalEvent(const BSSignalEvent * const pEvent);
        void OnBSSysEvent(const BSSysEvent * const pEvent);
        void OnBSEvent(const BSEvent * const pEvent);

    private:
        static uint32_t MakeNewMasterIndex();

    private:
        static uint32_t m_nCurrentMasterIndex;
        uint32_t m_nMasterIndex;
        ThreadEventQueue *m_pThreadEventQueue;
        IEntityFactory *m_pEntityFactory;
        std::string g_businessServiceName;
        std::shared_ptr<BusinessServiceStateMachine> m_pBSServiceSM;
        std::vector<std::shared_ptr<BusinessJobStateMachine> > m_BSJobSMs;
    };

} // namespace bsfwk

#endif // #ifndef BSFWK_BUSINESS_SERVICE_H