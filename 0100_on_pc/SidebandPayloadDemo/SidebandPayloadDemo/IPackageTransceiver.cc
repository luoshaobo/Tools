#include "BigDataServiceGlobal.h"
#include "IPackageTransceiver.h"
#include "PackageTransceiver.h"

namespace volvo_on_call {
namespace bds {

    IPackageTransceiver &IPackageTransceiver::getInstance()
    {
        static PackageTransceiver packageTransceiver;

        return packageTransceiver;
    }

} // namespace bds {
} // namespace volvo_on_call {
