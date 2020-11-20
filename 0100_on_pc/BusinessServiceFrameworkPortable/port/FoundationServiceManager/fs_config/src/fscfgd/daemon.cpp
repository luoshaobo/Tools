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
 *  \file     daemon.cpp
 *  \brief    Foundation Services Daemon implementation
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#include <fscfgd/daemon.h>

#include <dlt/dlt.h>
#include <fsm.h>

#include <fscfgd/local_config.h>

#include <fscfgd/encoding.h>
#include <fscfgd/config.h>
#include <fscfgd/vsdpsource.h>

#include "persistence_client_library.h"

#ifdef FSM_FSCFG_TEST
#include <manual_test.h>
#endif

DLT_DECLARE_CONTEXT(dlt_fscfgd)

namespace fsm
{

Daemon::Daemon()
 : main_loop_(nullptr),
   config_(),
   provisioning_(),
   discovery_(),
   local_config_(),
   discovery_provider_(),
   transfer_manager_(),
   event_queue_(),
   bus_acquired_id(0)
{
    DLT_REGISTER_APP("FSCD", "Foundation Services Configuration Daemon");
    DLT_REGISTER_CONTEXT(dlt_fscfgd, "FSCD", "fscfgd");
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Initializing fscfgd");

    fsm::transfermanager::FSM_TransferManager_Initialise();
}

Daemon::~Daemon()
{
    if (transfer_manager_ != nullptr)
    {
        transfer_manager_.reset();  // explicitely delete the managed pointer
    }
    config_->Unbind(bus_acquired_id);
    fsm::transfermanager::FSM_TransferManager_Terminate();
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Terminating fscfgd");
    DLT_UNREGISTER_CONTEXT(dlt_fscfgd);
}

fscfg_ReturnCode Daemon::Start()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    OpenSSL_add_all_algorithms();

    tpPCL_Error_t rcpcl = E_PCL_ERROR_NONE;
    rcpcl = tpPCL_init("fscfg_daemon", nullptr, FALSE);
    if(E_PCL_ERROR_NONE != rcpcl)
    {
        DLT_LOG_STRINGF(dlt_fscfgd, DLT_LOG_ERROR, "tpPCL_init rc=%x", rcpcl);
        return fscfg_kRcBadResource; // Resource does not exist or is invalid
    }

    FsmInitialize();

    main_loop_ = g_main_loop_new(NULL, FALSE);

    local_config_ = std::make_shared<LocalConfig>();

    transfer_manager_ = std::make_shared<fsm::transfermanager::TransferManager>();

    config_ = std::make_shared<Config>();
    provisioning_ = config_ ? std::make_shared<Provisioning>(config_) : provisioning_;
    discovery_ = provisioning_ ? std::make_shared<Discovery>(provisioning_) : discovery_;

    std::shared_ptr<VsdpSource> vsdp_source = discovery_ && transfer_manager_
                                              ? std::make_shared<VsdpSource>(provisioning_, local_config_, transfer_manager_)
                                              : nullptr;

    if (!vsdp_source)
    {
        // Due to conditional initialization above, it could mean that either vsdp_source failed to initialize or
        // any other of the dependencies. What we can deduce at this point is that our deamon is not able to perform
        // start-up.
        DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Daemon::Start: Failed to allocate base instances");

        rc = fscfg_kRcBadState;
    }
    else
    {

        rc = config_->Register(vsdp_source);
        rc = rc == fscfg_kRcSuccess ? provisioning_->SetPriority(vsdp_source, Provisioning::Priority::kMedium) : rc;

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Daemon::Start: Failed to register and prioritize VSDP Source");
        }
        else
        {
#ifdef FSM_FSCFG_TEST
                    ManualTest manual_test(config_, provisioning_, discovery_);
                    manual_test.Start();
#endif
            // Instantiate the service discovery state-machine.
            discovery_provider_ = std::make_shared<DiscoveryProvider>(discovery_,
                                                                      vsdp_source,
                                                                      local_config_,
                                                                      transfer_manager_);

            if (!discovery_provider_)
            {
                DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_ERROR, "Daemon::Start: Failed to instantiate DiscoveryProvider");

                rc = fscfg_kRcBadState;
            }
            else
            {
                // Once GDBus confirms the bus is acquired, we enable the event queue. It is allowed to push events in the
                // event queue after event queue initialization, but they only get executed once the bus is acquired.
                rc = config_->BindBusAcquired(std::bind(&Daemon::BusAcquiredEvent, this, std::placeholders::_1),
                                              bus_acquired_id);
                rc = rc == fscfg_kRcSuccess ? config_->OwnBusName() : rc;

                if (rc == fscfg_kRcSuccess)
                {
                    // perform a service discovery later.
                    // The way it works is that we stimulate a transition to EntryPoint which will trigger the rest of the
                    // state-machine accordingly. The stimulus is passed for execution only after the bus is acquired.

                    DiscoveryEvent entry_point_event(DiscoveryEventType::kExecuteTransition,
                                                     DiscoveryProvider::State::kInitial,
                                                     true);
                    /* 20190411, Wenxin Fang, comment temporarily to avoid access volvo website, this module can be removed later */
                    //event_queue_.PostEvent(entry_point_event);

                    g_main_loop_run(main_loop_);

                    config_->Unbind(bus_acquired_id);
                }
            }
        }
    }

    g_main_loop_unref(main_loop_);

    FsmTerminate();

    return rc;
}

DaemonEventQueue& Daemon::GetQueue()
{
    return event_queue_;
}

std::shared_ptr<Config> Daemon::GetConfig()
{
    return config_;
}


fscfg_ReturnCode Daemon::BusAcquiredEvent(Config::BusAcquiredEvent ev)
{
    // Internal events are only handled AFTER the bus is acquired.
    DLT_LOG_STRING(dlt_fscfgd, DLT_LOG_INFO, "Bus acquired - attaching main event queue");
    event_queue_.Attach(g_main_context_default());

    return fscfg_kRcSuccess;
}

} // namespace fsm

/** \}    end of addtogroup */
