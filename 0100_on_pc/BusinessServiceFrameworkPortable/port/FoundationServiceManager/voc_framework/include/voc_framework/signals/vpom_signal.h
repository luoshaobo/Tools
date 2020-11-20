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
 *  \file     vpom_signal.h
 *  \brief    VOC Service vpom signal base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_VPOM_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_VPOM_SIGNAL_H_

#include "voc_framework/signals/signal.h"

#include <memory>
#include <type_traits>

#include "vpom_IPositioningService.hpp"


namespace fsm
{

template<typename T>
class VpomSignal : public Signal
{

 public:

    /**
     * \brief Create a VpomSignal.
     *
     * \param[in] payload The payload which this signal should wrap.
     * \param[in] transaction_id The transaction id attached to this
     *                           signal.
     * \param[in] signal_type This signals specific type.
     *
     * \todo would it make sense to add a check, whether the signal_type is a valid VpomSignal?
     *
     * \return Shared pointer to the signal if successful, or empty
     *         shared pointer if not.
     */
    static std::shared_ptr<VpomSignal<T>> CreateVpomSignal (
                                                         T payload,
                                                         TransactionId& transaction_id,
                                                         SignalType signal_type)
    {
        std::shared_ptr<VpomSignal<T>> return_signal;
        return_signal = std::shared_ptr<VpomSignal<T>>(
                            new VpomSignal<T>(payload,
                            transaction_id,
                            signal_type));
        
        return return_signal;
    }

    /**
     * \brief Get the payload contained in the signal.
     * \return the payload.
     */
    const T& GetPayload ()
    {
        return payload_;
    }


    /**
     * \brief Serialize to string, used for debug printouts. Must be implemented by subclasses.
     * \return Name or other text identifier of the signal.
     */
    std::string ToString ()
    {
        return ("VpomSignal, type: " + GetSignalType());
    }


 private:

    /**
     * \brief Private costructor, no subclasses, only way to create is via factory.
     * \param[in] payload        The payload which the Signal should contain.
     * \param[in] transaction_id Transaction id of the signal.
     * \param[in] signal_type    Type of the signal.
     */
    VpomSignal (T payload,TransactionId& transaction_id,SignalType signal_type) : 
        Signal(transaction_id, signal_type),
        payload_(payload) {};


    /**
     * \brief The payload contained in this signal.
     *        Typically a struct define by VpomSignal
     */
    const T payload_;


    //no default constructor
    VpomSignal () {};
};


typedef VpomSignal<vpom::GNSSData> VpomGNSSPositionDataSignal;

} // namespace fsm

#endif // VOC_FRAMEWORK_SIGNALS_VPOM_SIGNAL_H_

/** \}    end of addtogroup */
