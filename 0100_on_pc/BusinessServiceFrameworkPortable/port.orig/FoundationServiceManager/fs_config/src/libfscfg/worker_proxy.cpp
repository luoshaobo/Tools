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

#include <dlt/dlt.h>

#include <libfscfg/worker_proxy.h>

DLT_DECLARE_CONTEXT(dlt_libfscfg);

namespace fsm
{

WorkerProxy::WorkerProxy()
    : worker_(),
      local_sync_(),
      cv_(),
      config_(nullptr),
      main_loop(nullptr)
{
    DLT_REGISTER_CONTEXT(dlt_libfscfg, "FSCL", "libfscfg");
    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Initializing libfscfg...");

    // start the worker.
    worker_ = std::thread(&WorkerProxy::worker_main, this);
    std::unique_lock<std::mutex> lock(local_sync_);

    cv_.wait(lock, [this]{ return discovery_ != nullptr; });

    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Initialized libfscfg!");

}

WorkerProxy::~WorkerProxy()
{
    if (main_loop)
    {
        g_main_loop_quit(main_loop);
    }

    if (worker_.joinable())
    {
        worker_.join();
    }
    DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_INFO, "Terminating libfscfg");
    DLT_UNREGISTER_CONTEXT(dlt_libfscfg);
}

std::shared_ptr<ConfigProxy> WorkerProxy::GetConfig() const
{
    return config_;
}

std::shared_ptr<ProvisioningProxy> WorkerProxy::GetProvisioning() const
{
    return provisioning_;
}

std::shared_ptr<DiscoveryProxy> WorkerProxy::GetDiscovery() const
{
    return discovery_;
}

fscfg_ReturnCode WorkerProxy::worker_main()
{
    fscfg_ReturnCode rc = fscfg_kRcSuccess;

    main_loop = g_main_loop_new(NULL, false);

    config_ = std::make_shared<ConfigProxy>();
    provisioning_ = config_ ? std::make_shared<ProvisioningProxy>(config_) : nullptr;
    discovery_ = provisioning_ ? std::make_shared<DiscoveryProxy>(provisioning_) : nullptr;

    if (!main_loop || !config_ || !provisioning_ || !discovery_)
    {
        DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to allocate memory.");
        rc = fscfg_kRcError; // Memory allocation failures, generic error.
    }
    else
    {
        rc = rc == fscfg_kRcSuccess ? config_->Init() : rc;
        rc = rc == fscfg_kRcSuccess ? provisioning_->Init() : rc;
        rc = rc == fscfg_kRcSuccess ? discovery_->Init() : rc;

        cv_.notify_all();

        if (rc != fscfg_kRcSuccess)
        {
            DLT_LOG_STRING(dlt_libfscfg, DLT_LOG_ERROR, "Failed to initialize client proxies.");
            rc = fscfg_kRcError;
        }
        else
        {
            g_main_loop_run(main_loop);
            g_main_loop_unref(main_loop);
        }
    }

    return rc;
}

} // namespace fsm

/** \}    end of addtogroup */
