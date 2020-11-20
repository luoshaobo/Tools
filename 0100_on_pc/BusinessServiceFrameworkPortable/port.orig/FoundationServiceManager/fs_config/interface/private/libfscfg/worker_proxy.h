/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     worker_proxy.h
 *  \brief    Foundation Services Proxy-side dbus worker interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_WORKER_PROXY_H_INC_
#define FSM_WORKER_PROXY_H_INC_

#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>

#include <glib.h>

#include <fscfg/fscfg.h>

#include <libfscfg/config_proxy.h>
#include <libfscfg/provisioning_proxy.h>
#include <libfscfg/discovery_proxy.h>

namespace fsm
{

/**
 * \brief FsConfig D-bus worker proxy.
 */
class WorkerProxy
{
public:
    /**
     * \brief WorkerProxy Constructor.
     */
    WorkerProxy();

    /**
     * \brief WorkerProxy Destructor.
     */
    ~WorkerProxy();

    /**
     * \brief Retrieves the config proxy object.
     *
     * \return A pointer to config proxy object.
     */
    std::shared_ptr<ConfigProxy> GetConfig() const;

    /**
     * \brief Retrieves the provisioning proxy object.
     *
     * \return A pointer to provisioning proxy object.
     */
    std::shared_ptr<ProvisioningProxy> GetProvisioning() const;

    /**
     * \brief Retrieves the discovery proxy object.
     *
     * \return A pointer to discovery proxy object.
     */
    std::shared_ptr<DiscoveryProxy> GetDiscovery() const;

private:
    WorkerProxy(const WorkerProxy& other);
    WorkerProxy& operator=(const WorkerProxy& other);

    /**
     * \brief Main function for the worker proxy thread.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcError on error
     */
    fscfg_ReturnCode worker_main();

    std::thread worker_;                                ///< Worker thread.
    std::mutex local_sync_;                             ///< Concurrent access synchronization primitive.
    std::condition_variable cv_;                        ///< Local event object between caller and worker.
    std::shared_ptr<ConfigProxy> config_;               ///< Config container pointer.
    std::shared_ptr<ProvisioningProxy> provisioning_;   ///< Provisioning container pointer.
    std::shared_ptr<DiscoveryProxy> discovery_;         ///< Discovery container pointer.
    GMainLoop* main_loop;                               ///< main loop for GDBus proxy-side operations.
};

} // namespace fsm

#endif // FSM_WORKER_PROXY_H_INC_

/** \}    end of addtogroup */
