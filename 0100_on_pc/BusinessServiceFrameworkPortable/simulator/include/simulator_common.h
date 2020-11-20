#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "bsfwk_Common.h"
#include "misc.h"

#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "voc_framework/signals/vpom_signal.h"


#include "vc_simulator.h"
#include "vpom_simulator.h"
#include "rmc_simulator.h"

extern std::function<void (std::shared_ptr<fsm::Signal>)> g_PostSignalEvent_Driver;

int GetRandom(int nMin, int nMax);
void PostSignalEvent(std::shared_ptr<fsm::Signal> signal);


#endif // !SIMULATOR_H
