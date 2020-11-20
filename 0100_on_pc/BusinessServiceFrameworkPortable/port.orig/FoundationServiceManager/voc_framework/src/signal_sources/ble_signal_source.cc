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
 *  \file     ble_signal_source.cc
 *  \brief    BLE signal source.
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signal_sources/ble_signal_source.h"

#include <mutex>

#include "dlt/dlt.h"
#include "voc_framework/signals/signal_factory.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

const uint32_t BleSignalSource::kComaTimeout = 10;

BleSignalSource::BleSignalSource()
{
    ResultCode result = ComaGetCommunicationMgrProxy();

    if (ResultCode::kSuccess == result)
    {
        ComaAttachBleEvents();
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(): Failed to init COMA", __FUNCTION__);
#ifndef VOC_TESTS
        raise(SIGTERM);
#endif
    }
}

BleSignalSource::~BleSignalSource()
{
    if (coma_com_mgr_)
    {
        ComaDetachBleEvents();
    }
}

BleSignalSource::ResultCode BleSignalSource::ComaGetCommunicationMgrProxy()
{
    ResultCode result = ResultCode::kSuccess;

    // Get COMA communication manager proxy if not present
    if (!coma_com_mgr_)
    {
        coma_com_mgr_ = ::createCommunicationManagerProxy("BleSignalSource", "libcomaclient.so");

        if (!coma_com_mgr_)
        {
            result = ResultCode::kFailed;
        }
    }

    // If not yet done - initialize the proxy
    if (ResultCode::kSuccess == result)
    {
        ICommunicationManagerProxy::InitialisationStatus init_status = coma_com_mgr_->getInitStatus();

        // Initialize the proxy only if status is not SUCCESS, SUCCESS means it's already initialized
        if (ICommunicationManagerProxy::InitialisationStatus::INITIALISATIONSTATUS_SUCCESS != init_status)
        {
            result = ComaInitCommunicationMgrProxy();
        }
    }

    if (ResultCode::kSuccess == result)
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "BleSignalSource::%s COMA communication manager init success.",
                        __FUNCTION__);
    }
    else
    {
        coma_com_mgr_ = nullptr;  // We mustn't call destroy on COMA proxy because it's shared with ssm and transfermgr

        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "BleSignalSource::%s COMA communication manager init failed with ResultCode %d.",
                        __FUNCTION__, static_cast<int>(result));
    }

    return result;
}

BleSignalSource::ResultCode BleSignalSource::ComaInitCommunicationMgrProxy()
{
    ResultCode result = ResultCode::kSuccess;
    bool init_result = false;
    RequestStatus coma_req_status;

    std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();

    std::function<void (ResponseComaInitClient&)> init_callback =
        std::bind(&BleSignalSource::ComaInitCommunicationMgrProxyCallback,
                  this,
                  std::placeholders::_1,
                  cv);

    std::unique_lock<std::mutex> lck(global_sync_);

    init_result = coma_com_mgr_->init(coma_req_status,
                                      init_callback);

    if (!init_result)
    {
        result = ResultCode::kFailed;
    }
    else
    {
        // Wait until either:
        // 1. cv was notified by init_callback, or
        // 2. cv->wait_for has timed_out
        bool wait_status = cv->wait_for(lck,
                                        std::chrono::seconds(kComaTimeout),
                                        [this]{ return coma_init_callback_received_; });

        if (!wait_status)
        {
            result = ResultCode::kTimeout;
        }
        else if (ICommunicationManagerProxy::INITIALISATIONSTATUS_SUCCESS != coma_com_mgr_->getInitStatus())
        {
            result = ResultCode::kFailed;
        }
    }

    if (ResultCode::kSuccess == result)
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "BleSignalSource::%s COMA communication manager init success.",
                        __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "BleSignalSource::%s COMA communication manager init failed, ResultCode=%d.",
                        __FUNCTION__, static_cast<int>(result));
    }

    return result;
}

void BleSignalSource::ComaInitCommunicationMgrProxyCallback(ResponseComaInitClient& response,
                                       std::shared_ptr<std::condition_variable> cv)
{
    std::unique_lock<std::mutex> lck(global_sync_);

    coma_init_callback_received_ = true;

    if (Status::STATUS_SUCCESS != response.status())
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "BleSignalSource::%s COMA communication manager init failed, status=%d.",
                        __FUNCTION__,
                        static_cast<int>(response.status()));
    }

    cv->notify_one();
}

void BleSignalSource::ComaAttachBleEvents()
{
    coma_com_mgr_->attachEvent(
            ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_MESSAGE_ARRIVED,
            this);

    coma_com_mgr_->attachEvent(
            ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_CONNECTED_DEVICES_CHANGED,
            this);
}

void BleSignalSource::ComaDetachBleEvents()
{
    coma_com_mgr_->detachEvent(
            ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_MESSAGE_ARRIVED,
            this);

    coma_com_mgr_->detachEvent(
            ICommunicationManagerProxyEvents::EventType::UNSOLICITEDEVENTTYPE_BTLE_CONNECTED_DEVICES_CHANGED,
            this);
}

/*
 * ICommunicationManagerProxyEvents implementation
 */
void BleSignalSource::handleEvent(const NotificationBluetoothLEIncomingMessage* event)
{
    std::shared_ptr<Signal> signal =
        SignalFactory::DecodeCcm(reinterpret_cast<const unsigned char*>(event->payload().c_str()),
                                 event->payload().size());

    if (signal)
    {
        BroadcastSignal(signal);

        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_INFO,
                        "BleSignalSource::%s received new BLE message, broadcasting.",
                        __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm,
                        DLT_LOG_ERROR,
                        "BleSignalSource::%s failed to decode received BLE message.",
                        __FUNCTION__);
    }
}

} // namespace fsm
/** \}    end of addtogroup */
