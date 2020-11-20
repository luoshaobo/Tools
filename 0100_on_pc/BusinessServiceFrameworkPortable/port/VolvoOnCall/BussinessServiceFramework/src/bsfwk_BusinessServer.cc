#include "bsfwk_Global.h"
#include "bsfwk_BusinessServer.h"

namespace bsfwk {

    BusinessServer::BusinessServer(const std::string &businessServiceName /*= "UnknownBusinessServer"*/)
        : ThreadEventQueue(businessServiceName + std::string("_ThreadEventQueue"))
        , m_businessServices()
        , g_businessServiceName(businessServiceName)
        , m_dataMutex()
    {
        m_EventHandler.registerEventHandler(this, &BusinessServer::OnBSSignalEvent);
        m_EventHandler.registerEventHandler(this, &BusinessServer::OnBSSysEvent);
        m_EventHandler.registerEventHandler(this, &BusinessServer::OnBSEvent);
        m_EventHandler.registerEventHandler(this, &BusinessServer::OnBSAsyncCallEvent);
    }

    BusinessServer::~BusinessServer()
    {
        stopThread();
        getThread().join();
    }

    void BusinessServer::AddService(BusinessService *pBusinessService)
    {
        ScopedLock lock(m_dataMutex);

        if (IsPtrNotNull(pBusinessService)) {
            uint32_t nMasterIndex = pBusinessService->GetMasterIndex();
            std::map<uint32_t, BusinessService *>::iterator it;

            it = m_businessServices.find(nMasterIndex);
            if (it != m_businessServices.end()) {
                it->second = pBusinessService;
            } else {
                m_businessServices.insert(std::pair<uint32_t, BusinessService *>(nMasterIndex, pBusinessService));
            }
        }
    }

    void BusinessServer::RemoveService(BusinessService *pBusinessService)
    {
        ScopedLock lock(m_dataMutex);

        if (IsPtrNotNull(pBusinessService)) {
            uint32_t nMasterIndex = pBusinessService->GetMasterIndex();
            std::map<uint32_t, BusinessService *>::iterator it;

            it = m_businessServices.find(nMasterIndex);
            if (it != m_businessServices.end()) {
                it->second = static_cast<BusinessService *>(0);
                m_businessServices.erase(it);
            }
        }
    }

    void BusinessServer::PostSignalEvent(std::shared_ptr<BSSignalEventDataBase> data)
    {
        BSSignalEvent bsSignalEvent(data);
        fireEvent(bsSignalEvent);
    }

    void BusinessServer::PostSignalEvent(std::shared_ptr<fsm::Signal> oSignal)
    {
        std::shared_ptr<BSSignalEventDataBase> data = std::make_shared<BSSignalEventDataBase>(oSignal);
        PostSignalEvent(data);
    }

    void BusinessServer::PostAsyncCall(AsyncCallback asyncCallback)
    {
        BSAsyncCallEvent bsAsyncCallEvent(asyncCallback);
        fireEvent(bsAsyncCallEvent);
    }

    void BusinessServer::fireEvent(Event &sendEvent)
    {
        insertEvent(sendEvent.clone());
    }

    void BusinessServer::eventHandler(Event::Ptr event)
    {
        m_EventHandler.handleEvent(event);
    }

    void BusinessServer::OnBSSignalEvent(const BSSignalEvent *const pEvent)
    {
        ScopedLock lock(m_dataMutex);

        if (IsPtrNotNull(pEvent)) {
            std::map<uint32_t, BusinessService*>::iterator it;

            for (it = m_businessServices.begin(); it != m_businessServices.end(); ++it) {
                BusinessService *pBusinessService = it->second;
                if (IsPtrNotNull(pBusinessService)) {
                    pBusinessService->OnBSSignalEvent(pEvent);
                }
            }
        }
    }

    void BusinessServer::OnBSSysEvent(const BSSysEvent * const pEvent)
    {
        ScopedLock lock(m_dataMutex);

        if (IsPtrNotNull(pEvent)) {
            uint32_t nMasterIndex = pEvent->GetMasterIndex();
            std::map<uint32_t, BusinessService*>::iterator it;

            it = m_businessServices.find(nMasterIndex);
            if (it != m_businessServices.end()) {
                BusinessService *pBusinessService = it->second;
                if (IsPtrNotNull(pBusinessService)) {
                    pBusinessService->OnBSSysEvent(pEvent);
                }
            }
        }
    }

    void BusinessServer::OnBSEvent(const BSEvent * const pEvent)
    {
        ScopedLock lock(m_dataMutex);

        if (IsPtrNotNull(pEvent)) {
            uint32_t nMasterIndex = pEvent->GetMasterIndex();
            std::map<uint32_t, BusinessService*>::iterator it;

            it = m_businessServices.find(nMasterIndex);
            if (it != m_businessServices.end()) {
                BusinessService *pBusinessService = it->second;
                if (IsPtrNotNull(pBusinessService)) {
                    pBusinessService->OnBSEvent(pEvent);
                }
            }
        }
    }

    void BusinessServer::OnBSAsyncCallEvent(const BSAsyncCallEvent *const pEvent)
    {
        if (IsPtrNotNull(pEvent)) {
            AsyncCallback asyncCallback = pEvent->GetCallback();
            if (asyncCallback != nullptr) {
                asyncCallback();
            }
        }
    }

} // namespace bsfwk
