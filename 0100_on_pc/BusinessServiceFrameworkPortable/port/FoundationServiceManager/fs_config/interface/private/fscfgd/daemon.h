/**
 * Copyright (C) 2017, 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     daemon.h
 *  \brief    Foundation Services Daemon interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_DAEMON_H_INC_
#define FSM_DAEMON_H_INC_

#include <glib.h>
#include <gio/gio.h>

#include <fscfg/fscfg_types.h>

#include <fscfgd/daemon_event_queue.h>
#include <fscfgd/discovery.h>
#include <fscfgd/provisioning.h>
#include <fscfgd/config.h>

namespace fsm
{

/**
 * \brief Foundation Services Config Daemon.
 *
 * Wraps around the Daemon initialization logic and main-loop handling.
 */
class Daemon
{
public:
    /**
     * \brief Daemon Constructor.
     */
    Daemon();

    /**
     * \brief Daemon Destructor.
     */
    ~Daemon();

    /**
     * \brief Initialize all daemon resource and start main-loop to service client requests.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode Start();

    /**
     * \brief Returns the daemon's event queue.
     *
     * \return daemon's event queue.
     */
    DaemonEventQueue& GetQueue();

    /**
     * \brief Returns the Configuration object.
     *
     * \return pointer to the Configuration.
     */
    std::shared_ptr<Config> GetConfig();

private:
    /**
     * @brief Bus acquired event handler. Attaches the daemon's event queue in the main context, enabling event queue
     * processing.
     *
     * \param[in] event bus acquired event parameters.
     *
     * \return fscfg_kRcSuccess always.
     */
    fscfg_ReturnCode BusAcquiredEvent(Config::BusAcquiredEvent event);

    GMainLoop* main_loop_;                                                ///< Glib Main loop.
    std::shared_ptr<Config> config_;                                      ///< Configuration reference.
    std::shared_ptr<Provisioning> provisioning_;                          ///< Provisioning reference.
    std::shared_ptr<Discovery> discovery_;                                ///< Discovery reference.
    std::shared_ptr<LocalConfig> local_config_;                           ///< Local config reference.
    std::shared_ptr<DiscoveryProvider> discovery_provider_;               ///< Discovery provider reference.
    std::shared_ptr<transfermanager::TransferManager> transfer_manager_;  ///< transfer manager reference.
    DaemonEventQueue event_queue_;                                        ///< Event queue reference.
    std::uint32_t bus_acquired_id;                                        ///< Bus Acquired bind id.
};

} // namespace fsm

#endif // FSM_DAEMON_H_INC_

/** \}    end of addtogroup */
