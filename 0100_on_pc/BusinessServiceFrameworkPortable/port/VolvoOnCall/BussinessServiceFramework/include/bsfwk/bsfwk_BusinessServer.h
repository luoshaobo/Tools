#ifndef BSFWK_BUSINESS_SERVER_H
#define BSFWK_BUSINESS_SERVER_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class BusinessServer : public ThreadEventQueue
    {
    public:
        BusinessServer(const std::string &businessServiceName = "UnknownBusinessServer");
        virtual ~BusinessServer();

    public:
        void AddService(BusinessService *pBusinessService); 
        void RemoveService(BusinessService *pBusinessService);
        void PostSignalEvent(std::shared_ptr<fsm::Signal> oSignal);
        void PostAsyncCall(AsyncCallback asyncCallback);

    private:
        virtual void eventHandler(Event::Ptr event); // from ThreadEventQueue
        void fireEvent(Event &sendEvent);
        void PostSignalEvent(std::shared_ptr<BSSignalEventDataBase> data);

    private:
        void OnBSSignalEvent(const BSSignalEvent * const pEvent);
        void OnBSSysEvent(const BSSysEvent *const pEvent);
        void OnBSEvent(const BSEvent *const pEvent);
        void OnBSAsyncCallEvent(const BSAsyncCallEvent *const pEvent);

    private:
        std::map<uint32_t, BusinessService *> m_businessServices;
        std::string g_businessServiceName;
        Mutex m_dataMutex;
    };

} // namespace bsfwk

#endif // #ifndef BSFWK_BUSINESS_SERVER_H
