#include "simulator_common.h"
#include "vpom_simulator.h"

using namespace bsfwk;

namespace vpom {
    namespace simulator {

        class VpomIPositioningServiceSimulator : public VpomIPositioningService {
        public:

            virtual bool GNSSPositionDataRawRequest(uint64_t request_id);
            virtual bool RegisterGNSSPositionDataRawResponseCb(GNSSPositionDataRawResponseCb_t cb) { return true; }
            virtual bool DeregisterGNSSPositionDataRawResponseCb(void) { return true; }

        private:
            void GNSSPositionDataRawRequest_Response(uint64_t request_id);
        };

        bool VpomIPositioningServiceSimulator::GNSSPositionDataRawRequest(uint64_t request_id)
        {
            really_async(std::bind(&VpomIPositioningServiceSimulator::GNSSPositionDataRawRequest_Response, this, request_id));
            return true;
        }

        void VpomIPositioningServiceSimulator::GNSSPositionDataRawRequest_Response(uint64_t request_id)
        {
			msleep(GetRandom(500, 3000));

            vpom::GNSSData data{};
            fsm::VpomTransactionId transaction_id(request_id);
            std::shared_ptr<fsm::VpomGNSSPositionDataSignal> signal = fsm::VpomGNSSPositionDataSignal::CreateVpomSignal(data, transaction_id, fsm::Signal::kGNSSPositionDataSignal);
            PostSignalEvent(signal);
        }

        VpomIPositioningService& GetSimulatorVpomPositioningService()
        {
            static VpomIPositioningServiceSimulator vpomIPositioningServiceSimulator;

            return vpomIPositioningServiceSimulator;
        }

    } // namespace simulator {
} // namespace vc {
