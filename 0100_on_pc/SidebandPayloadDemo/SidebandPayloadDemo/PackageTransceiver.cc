#include "BigDataServiceGlobal.h"
#include "PackageTransceiver.h"

namespace volvo_on_call {
namespace bds {

    PackageTransceiver::TransferRequestInfo::TransferRequestInfo()
        : token(0)
        , topic()
        , package()
        , valid(false)
    {

    }

    PackageTransceiver::TransferRequestInfo::TransferRequestInfo(PackageTransceiver::TranferToken a_token, const std::string &a_topic, const std::vector<uint8_t> &a_package, bool a_valid)
        : token(a_token)
        , topic(a_topic)
        , package(a_package)
        , valid(a_valid)
    {

    }

    PackageTransceiver::PackageTransceiver()
        : IPackageTransceiver()
        , m_newTranferToken(0)
        , m_newTransferResultCallbackToken(0)
        , m_tranferRequests()
        , m_tranferRequestsMutex()
        , m_tranferCallbackMap()
        , m_tranferCallbackMapMutex()
        , m_currentTransferRequestInfo()
    {
        m_currentTransferRequestInfo.valid = false;
    }

    PackageTransceiver::~PackageTransceiver()
    {

    }

    bool PackageTransceiver::transfer(const std::string &topic, const std::vector<uint8_t> &package, TranferToken &token)
    {
        bool ret = true;
#if 0
        //
        // NOTE:
        // There should be some bug with the multi-thread implementation, which will cause some packages to be transfered for multiple times.
        //
        {
            std::unique_lock<std::recursive_mutex> lock(m_tranferRequestsMutex);

            token = m_newTranferToken++;
            TransferRequestInfo transferRequestInfo{token, topic, package, true};
            m_tranferRequests.push_back(transferRequestInfo);
        }

        checkNexTransfer();
#else
        std::string content((const char *)&package[0], package.size());
        printf(content.c_str());
        printf("\n");
#endif // #if 0

        return ret;
    }

    bool PackageTransceiver::cancel(const TranferToken &token)
    {
        bool ret = true;
        bool found = false;

        {
            std::unique_lock<std::recursive_mutex> lock(m_tranferRequestsMutex);

            for (auto it = m_tranferRequests.begin(); it != m_tranferRequests.end(); ++it) {
                if (it->token == token) {
                    found = true;
                    m_tranferRequests.erase(it);
                    break;
                }
            }
        }

        if (found) {
            callTranferCallbacks(token, TRC_CANCELED);
        }

        return ret;
    }

    PackageTransceiver::TransferResultCallbackToken PackageTransceiver::registerTranferCallback(const TransferResultCallback &callback)
    {
        TransferResultCallbackToken token = m_newTransferResultCallbackToken++;

        {
            std::unique_lock<std::recursive_mutex> lock(m_tranferCallbackMapMutex);    
            m_tranferCallbackMap[token] = callback;
        }

        return token;
    }

    void PackageTransceiver::unregisterTranferCallback(const TransferResultCallbackToken &token)
    {
        {
            std::unique_lock<std::recursive_mutex> lock(m_tranferCallbackMapMutex);

            auto it = m_tranferCallbackMap.find(token);
            if (it != m_tranferCallbackMap.end()) {
                m_tranferCallbackMap.erase(it);
            }
        }
    }

    void PackageTransceiver::callTranferCallbacks(const TranferToken &token, TransferResultCode code)
    {
        {
            std::unique_lock<std::recursive_mutex> lock(m_tranferCallbackMapMutex);

            for (auto it = m_tranferCallbackMap.begin(); it != m_tranferCallbackMap.end(); ++it) {
                TransferResultCallback &callback = it->second;
                if (callback != nullptr) {
                    callback(token, code);
                }
            }
        }
    }

    void PackageTransceiver::checkNexTransfer()
    {
        {
            std::unique_lock<std::recursive_mutex> lock2(m_tranferRequestsMutex);

            if (m_currentTransferRequestInfo.valid) {
                return;
            }

            if (m_tranferRequests.empty()) {
                return;
            }

            m_currentTransferRequestInfo = m_tranferRequests.front();
        }

        lowLayertransfer();
    }

    void PackageTransceiver::removeTransfer(const TranferToken &token)
    {
        {
            std::unique_lock<std::recursive_mutex> lock(m_tranferRequestsMutex);

            for (auto it = m_tranferRequests.begin(); it != m_tranferRequests.end(); ++it) {
                if (it->token == token) {
                    m_tranferRequests.erase(it);
                    break;
                }
            }
        }
    }

    bool PackageTransceiver::lowLayertransfer()
    {
        {
            std::unique_lock<std::recursive_mutex> lock(m_tranferRequestsMutex);
            if (!m_currentTransferRequestInfo.valid) {
                return false;
            }
        }

        std::thread th(std::bind(&PackageTransceiver::lowLayerTransferCallback, this));
        th.detach();

        return true;
    }

    void PackageTransceiver::lowLayerTransferCallback()
    {
        TranferToken token = 0;

        {
            std::unique_lock<std::recursive_mutex> lock(m_tranferRequestsMutex);
            m_currentTransferRequestInfo.valid = false;
            token = m_currentTransferRequestInfo.token;

            std::string content((const char *)&m_currentTransferRequestInfo.package[0], m_currentTransferRequestInfo.package.size());
            printf(content.c_str());
            printf("\n");
        }

        callTranferCallbacks(token, TRC_OK);
        removeTransfer(token);
        checkNexTransfer();
    }

} // namespace bds {
} // namespace volvo_on_call {
