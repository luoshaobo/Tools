#include "simulator_common.h"
#include "rmc_simulator.h"

using namespace bsfwk;

namespace volvo_on_call {
    namespace simulator {

        void GeelySendMessage(fsm::RemoteCtrlBasicStatus status)
        {
        }

        std::shared_ptr<GlyVdsRmcSignal> CreateGlyVdsSignal(int signalId)
        {
            fsm::VdServiceTransactionId transaction_id;
            std::shared_ptr<GlyVdsRmcSignal> signal = std::make_shared<GlyVdsRmcSignal>(transaction_id, (void *)NULL, signalId);
            return signal;
        }

        void GeelySendResponse(fsm::RemoteCtrlBasicResult result)
        {
            BSFWK_LOG_GEN_PRINTF("\n");
            
            switch (result.operationSucceeded) {
            case 0:
                {
                    BSFWK_LOG_GEN_PRINTF("Successful!\n");
                }
                break;
            case 1:
                {
                    BSFWK_LOG_GEN_PRINTF("Failed!\n");
                }
                break;
            case 2:
                {
                BSFWK_LOG_GEN_PRINTF("Timed out!\n");
                }
                break;
            }
        }

        void GeelySendResponse(fsm::RemoteCtrlBasicResult result, fsm::RemoteCtrlBasicStatus status)
        {
            BSFWK_LOG_GEN_PRINTF("\n");

            switch (result.operationSucceeded) {
            case 0:
                {
                    BSFWK_LOG_GEN_PRINTF("Successful!\n");
                }
                break;
            case 1:
                {
                    BSFWK_LOG_GEN_PRINTF("Failed!\n");
                }
                break;
            case 2:
                {
                    BSFWK_LOG_GEN_PRINTF("Timed out!\n");
                }
                break;
            }
        }

    } // namespace simulator {
} // namespace vc {
