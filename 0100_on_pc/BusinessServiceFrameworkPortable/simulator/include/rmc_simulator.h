#ifndef RMC_SIMULATOR_H
#define RMC_SIMULATOR_H

#include "signals/geely/gly_vds_rmc_signal.h"

namespace volvo_on_call {
   
    namespace simulator {

        void GeelySendMessage(fsm::RemoteCtrlBasicStatus status);
        std::shared_ptr<GlyVdsRmcSignal> CreateGlyVdsSignal(int signalId);
        void GeelySendResponse(fsm::RemoteCtrlBasicResult result);
        void GeelySendResponse(fsm::RemoteCtrlBasicResult result, fsm::RemoteCtrlBasicStatus status);

    } // namespace simulator {
} // namespace vc {

#endif // !RMC_SIMULATOR_H
