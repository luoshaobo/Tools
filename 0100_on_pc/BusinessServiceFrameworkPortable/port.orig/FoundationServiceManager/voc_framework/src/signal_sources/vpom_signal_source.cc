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
 *  \file     vpom_signal_source.cc
 *  \brief    VOC Service VolvoPositioningManager signal source.
 *  \author   Axel Fagerstedt, Florian Schindler
 *  \todo     there is lots of similar calls, could they be unified (e.g. macros, template) to avoid much typing?
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/vpom_signal.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"

#include <algorithm>
#include <map>
#include <functional>

#include "dlt/dlt.h"
#include "vpom_IClient.hpp"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

/**
 * \brief Callback proxy to be registered with VolvoPositioningManager.
 *        Needed as the interface expects c style callback functions.
 *  for internal use only
 *
 * \param[in] data  The payload data, see \ref GNSSPositionDataResponse
 * \param[in] request_id  The request id (should be mirrored back to us)
 */
static void GNSSPositionDataResponseCbProxy(vpom::GNSSData *data,uint64_t request_id)
{
    VpomSignalSource& vpom = VpomSignalSource::GetInstance();
    vpom.GNSSPositionDataResponseCb(data, request_id);
}


VpomSignalSource::VpomSignalSource()
{
    vpom_initialized_ = VpomIClient::Init();

    if (!vpom_initialized_)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
            "VpomSignalSource, failed to initialize vpom.");
    }
}

VpomSignalSource::~VpomSignalSource()
{
    if (vpom_initialized_)
    {
        // deregister first:
        for (auto const &signal_map_element : active_subscriptions_)
        {
            UnregisterSignal(signal_map_element.first, true);  // force deregistering, regardless of reference counter
        }
        vpom_initialized_ = ! VpomIClient::Deinit();
    }
}

VpomSignalSource& VpomSignalSource::GetInstance()
{
    static VpomSignalSource instance;
    return instance;
}

VpomIPositioningService& VpomSignalSource::GetPositioningServiceObject()
{
    return VpomIClient::GetPositioningService();
}


bool VpomSignalSource::SubscribeForSignal(Signal::SignalType signal_type)
{
    bool return_value = RegisterSignal(signal_type);
    if (!return_value)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,\
            "VpomSignalSource RegisterSignals failed", \
            static_cast<unsigned int>(signal_type));
    }
    
    return return_value;
}


bool VpomSignalSource::RegisterSignals(std::vector<Signal::SignalType> requested_subscriptions)
{
    bool return_value = true;
    bool temp_return_value = true;

    if (vpom_initialized_)
    {
        for (Signal::SignalType &signal_type : requested_subscriptions)
        {
            temp_return_value = RegisterSignal(signal_type);
            return_value = temp_return_value && return_value;  // keep false return_value
        }  // for
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
            "VpomSignalSource::RegisterSignals failed, not initialised");
        return_value = false;
    }
    return return_value;
}


bool VpomSignalSource::RegisterSignal(Signal::SignalType signal_type)
{
    // guard against concurrent registrations
    std::lock_guard<std::mutex> lock(subscriptions_mutex_);
    bool return_value = true;

    if (vpom_initialized_)
    {
        if ( active_subscriptions_.find(signal_type) == active_subscriptions_.end() )
        {  // if not in list: register callback
            switch (signal_type)
            {
                case Signal::kGNSSPositionDataSignal:
                {
                    return_value = VpomIClient::GetPositioningService().RegisterGNSSPositionDataRawResponseCb(
                        &GNSSPositionDataResponseCbProxy);
                    break;
                }
                default:
                {
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,\
                        "VpomSignalSource::RegisterSignal, received unhandeled signal: %u", \
                        static_cast<unsigned int>(signal_type));
                    return_value = false;
                }
            }  // switch
            if (return_value)
            {
                // add to active_subscriptions_ with reference counter = 1
                active_subscriptions_[signal_type] = 1;
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,\
                    "VpomSignalSource register callback for signal %u failed",
                    static_cast<unsigned int>(signal_type));
            }
        }
        else
        {
            // if in list then increment reference counter only
            active_subscriptions_[signal_type] ++;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
            "VpomSignalSource::RegisterSignal failed, not initialised");
        return_value = false;
    }
    return return_value;
}


bool VpomSignalSource::UnregisterSignals(std::vector<Signal::SignalType> requested_unsubscriptions)
{
    bool return_value = true;
    bool temp_return_value = true;

    if (vpom_initialized_)
    {
        for (Signal::SignalType &signal_type : requested_unsubscriptions)
        {
            temp_return_value = UnregisterSignal(signal_type, false);
            return_value = temp_return_value && return_value;  // keep false return_value
        }  // for
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
            "VpomSignalSource::UnregisterSignals failed, not initialised");
        return_value = false;
    }
    return return_value;
}


bool VpomSignalSource::UnregisterSignal(Signal::SignalType signal_type, bool forced)
{
    bool return_value = true;

    std::lock_guard<std::mutex> lock(subscriptions_mutex_);  // guard against concurrent (un)registrations
    if (vpom_initialized_)
    {
        if ( active_subscriptions_.find(signal_type) != active_subscriptions_.end() )
        {  // only if signal is already registered
            if ((active_subscriptions_[signal_type] > 1) && (!forced) )  // if forced: deregister regardless of ref counter
            {   // decrement reference counter only
                active_subscriptions_[signal_type] --;
            }
            else
            {  // reference counter was 1 or 0 (which should never happen): deregister signal
                switch (signal_type)
                {
                    case Signal::kGNSSPositionDataSignal:
                    {
                        return_value = VpomIClient::GetPositioningService().DeregisterGNSSPositionDataRawResponseCb();
                        break;
                    }
                    default:
                    {
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,\
                            "VpomSignalSource::UnregisterSignal received unhandeled signal: %u", \
                            static_cast<unsigned int>(signal_type));
                        return_value = false;
                    }
                }  // switch
                if (return_value)
                {   // unregister succeeded, now remove from list:
                    active_subscriptions_.erase(signal_type);
                }
                else
                {   // unregister failed
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,\
                        "VpomSignalSource deregister callback for signal %u failed",
                        static_cast<unsigned int>(signal_type));
                }
            }  // if have to deregister
        }  // if in list
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,\
                "VpomSignalSource::UnregisterSignal, signal %u not registered, ignored.", \
                static_cast<unsigned int>(signal_type));
            return_value = true;  // don't return error, this could happen at parallel requests
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,\
           "VpomSignalSource::UnregisterSignals failed, not initialised");
        return_value = false;
    }

    return return_value;
}

void VpomSignalSource::GNSSPositionDataResponseCb(vpom::GNSSData *data, uint64_t request_id)
{
    VpomTransactionId transaction_id(request_id);
    Signal::SignalType signal_type = Signal::kGNSSPositionDataSignal;

    std::shared_ptr<Signal> signal =
        VpomGNSSPositionDataSignal::CreateVpomSignal(*data,
                                                 transaction_id,
                                                 signal_type);
    BroadcastSignal(signal);
}


} // namespace fsm
/** \}    end of addtogroup */
