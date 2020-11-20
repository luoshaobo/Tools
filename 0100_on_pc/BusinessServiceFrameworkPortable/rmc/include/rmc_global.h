#ifndef RMC_GLOBAL_H
#define RMC_GLOBAL_H

#ifdef TCAM_TARGET
#include "voc_framework/transactions/smart_transaction.h"
#include "voc_framework/signals/response_info_data.h"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "signals/geely/gly_vds_rdl_signal.h"
#include "signals/bcc_cl_001_signal.h"
#include "signals/ecdhe_approval_voc_frame.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rmc_signal.h"
#include "voc_framework/signals/vpom_signal.h"
#include "voc_framework/signals/internal_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#endif // #ifdef TCAM_TARGET

#include "bsfwk_Global.h"
#include "rmc_simulator.h"
#include "vc_simulator.h"
#include "vpom_simulator.h"
#include "simulator_common.h"

#include "signals/geely/gly_vds_rdl_signal.h"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "voc_framework/signals/vpom_signal.h"

namespace volvo_on_call {

    using namespace bsfwk;

    namespace rmc {
        inline vc::IGeneral &GetVehicleCommonGeneral() { return vc::simulator::GetSimulatorGeneralInterface(); }
        inline VpomIPositioningService &GetPositioningService() { return vpom::simulator::GetSimulatorVpomPositioningService(); }
    } // namespace rmc {

    using namespace rmc;

	int test_rmc_main(int argc, char *argv[]);

#ifdef TCAM_TARGET
    extern std::shared_ptr<fsm::Transaction> CreateRMCTransaction();
#endif // #ifdef TCAM_TARGET

} // namespace volvo_on_call {

#endif // #ifndef RMC_GLOBAL_H
