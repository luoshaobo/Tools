/**
 * Copyright (C) 2016 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     basic_car_control_feature.h
 *  \brief    VOC Service BasicCarControl feature.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FEATURES_BASIC_CAR_CONTROL_H_
#define VOC_FEATURES_BASIC_CAR_CONTROL_H_

// fsm includes
#include "voc_framework/features/feature.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "svt/svt.h"
#include "rvs/rvs.h"
#include "remote_config/remote_config.h"
#include "remote_start/remote_start_engine.h"
#include "remote_start/remote_start_climate.h"
#include "remote_start/remote_start_seatHeat.h"
#include "remote_start/remote_start_seatVenti.h"
#include "remote_start/remote_start_steerwhlHeat.h"
#include "remote_start/remote_start_common.h"
#include "theft/theft_notification_transaction.h"
#include "journey_log/car_journeylog_transaction.h"
#include "rms/rms_transaction.h"
#include "rvdc/rvdc.h"
#include "applications_lcm/apps_lcm.h"
#include "download_agent/car_downloadagent_transaction.h"

// voc includes
#ifndef VOC_TESTS
#include "transactions/car_locator_position_update_transaction.h"
#endif

#include <mutex>

namespace volvo_on_call
{

class BasicCarControlFeature: public fsm::Feature
{

 public:

    /**
     * \brief Constructs a new BasicCarControlFeature
     */
    BasicCarControlFeature ();

    /**
     * \brief Handle a signal.
     * \param[in] signal The signal to handle.
     * \return None.
     */
    void HandleSignal(std::shared_ptr<fsm::Signal> signal);
    void ShutdownNotification(LCMShutdownType sig);
private:

#ifndef VOC_TESTS
    /**
     * \brief Long lived car locator transaction
     */
    std::shared_ptr<CarLocatorPositionUpdateTransaction> car_locator_position_update;
#endif

    std::shared_ptr<Svt> m_svt;
    std::shared_ptr<TheftNotificationTransaction> theft_notify;
    std::shared_ptr<RmsTransaction> mRmsTransaction;
    std::shared_ptr<Rvs> rvs;
    std::shared_ptr<RMT_Engine> m_pRes;
    std::shared_ptr<RMT_Climate> m_pRMT_Climate;
    std::shared_ptr<RMT_SeatHeat> m_pRMT_SeatHeat;
    std::shared_ptr<RMT_SeatVenti> m_pRMT_SeatVenti;
    std::shared_ptr<RMT_SteerWhlHeat> m_pRMT_SteerwhlHeat;
    std::shared_ptr<Rvdc> m_pRvdc;
    AppsLcm& m_appsLcm_;
    /**
     * \brief BasicCarControlFeature will subscribe to this VocmoSignalSource instance in the constructor.
     *        Used for Vocmo related communications.
     */
    fsm::VocmoSignalSource vocmo_signal_source_;
};

} // namespace volvo_on_call
/** \}    end of addtogroup */

#endif //VOC_FEATURES_BASIC_CAR_CONTROL_H_
