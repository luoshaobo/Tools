#ifndef BIG_DATA_SERVICE_CONFIG_H
#define BIG_DATA_SERVICE_CONFIG_H

#include "BigDataServiceGlobal.h"

namespace volvo_on_call {
namespace bds {

    class BigDataServicePackageHandler;

    struct BigDataServiceConfig
    {
        //
        // Basic information to be persisted
        //
        std::string configId{};
        uint16_t version{0};
        std::string selectedPortNames;
        uint16_t collectingDuration{0};     // (unit: millisecond) 0: to use the predefined duration for each port; otherwise, to use this duration for all ports.
        uint16_t uploadingDuration{0};      // (unit: second) 0: not to upload automatically; otherwise, to upload automatically.
        std::string condition{};            // not in use.
        uint16_t triggeringCount{0};        // not in use.

        //
        // Additional information, filled after proded
        //
        bool valid{false};
        std::vector<size_t> selectedPorts{};
        std::shared_ptr<BigDataServicePackageHandler> packageHandler{nullptr};
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef BIG_DATA_SERVICE_CONFIG_H
