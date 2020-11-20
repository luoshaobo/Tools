#ifndef PACKAGE_TRANSCEIVER_H
#define PACKAGE_TRANSCEIVER_H

#include "BigDataServiceGlobal.h"
#include "IPackageTransceiver.h"

namespace volvo_on_call {
namespace bds {

    class PackageTransceiver : public IPackageTransceiver
    {
    private:
        struct TransferRequestInfo {
            TransferRequestInfo();
            TransferRequestInfo(TranferToken a_token, const std::string &a_topic, const std::vector<uint8_t> &a_package, bool a_valid);
        
            TranferToken token{0};
            std::string topic{};
            std::vector<uint8_t> package{};
            bool valid{false};
        };

    public:
        PackageTransceiver();
        virtual ~PackageTransceiver();

        virtual bool transfer(const std::string &topic, const std::vector<uint8_t> &package, TranferToken &token) override;
        virtual bool cancel(const TranferToken &token) override;

        virtual TransferResultCallbackToken registerTranferCallback(const TransferResultCallback &callback) override;
        virtual void unregisterTranferCallback(const TransferResultCallbackToken &token) override;

    private:
        void callTranferCallbacks(const TranferToken &token, TransferResultCode code);

    private:
        void checkNexTransfer();
        void removeTransfer(const TranferToken &token);
        bool lowLayertransfer();
        void lowLayerTransferCallback();

    private:
        TranferToken m_newTranferToken;
        TransferResultCallbackToken m_newTransferResultCallbackToken;
        std::list<TransferRequestInfo> m_tranferRequests;
        std::recursive_mutex m_tranferRequestsMutex;
        std::map<TransferResultCallbackToken, TransferResultCallback> m_tranferCallbackMap;
        std::recursive_mutex m_tranferCallbackMapMutex;
        TransferRequestInfo m_currentTransferRequestInfo;
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef PACKAGE_TRANSCEIVER_H
