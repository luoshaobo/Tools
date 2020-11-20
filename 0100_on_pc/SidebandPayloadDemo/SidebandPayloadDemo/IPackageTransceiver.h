#ifndef I_PACKAGE_TRANSCEIVER_H
#define I_PACKAGE_TRANSCEIVER_H

#include "BigDataServiceGlobal.h"

namespace volvo_on_call {
namespace bds {

    class IPackageTransceiver
    {
    public:
        typedef uint32_t TranferToken;
        typedef uint32_t TransferResultCallbackToken;

        enum TransferResultCode {
            TRC_OK = 0,
            TRC_FAILED,
            TRC_TIMEDOUT,
            TRC_CANCELED,
            TRC_DISCARDED,
        };

        typedef std::function<void (const TranferToken &token, TransferResultCode code)> TransferResultCallback;

    public:
        static IPackageTransceiver &getInstance();
    
        virtual ~IPackageTransceiver() {}

        virtual bool transfer(const std::string &topic, const std::vector<uint8_t> &package, TranferToken &token) = 0;
        virtual bool cancel(const TranferToken &token) = 0;

        virtual TransferResultCallbackToken registerTranferCallback(const TransferResultCallback &callback) = 0;
        virtual void unregisterTranferCallback(const TransferResultCallbackToken &token) = 0;
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef I_PACKAGE_TRANSCEIVER_H
