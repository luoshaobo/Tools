/**
 * Copyright (C) 2016, 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     internal_signal.h
 *  \brief    VOC Service temlate class for internal signal.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_INTERNAL_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_INTERNAL_SIGNAL_H_

#include "voc_framework/signals/internal_signal.h"
#include "voc_framework/signals/signal.h"

#include <memory>

namespace fsm
{
/**
 * \brief Template for internal signal that can carry any type of data object.
 *        Can be used as-is or as a base class
 */
template <class T>
class InternalSignal: public Signal
{

public:

    /**
     * \brief Constructor
     * \param[in] transaction_id transaction id
     * \param[in] signal_type type of the signal
     * \param[in] payload payload to be carried
     */
    InternalSignal (const TransactionId& transaction_id,
                    const Signal::SignalType signal_type,
                    const std::shared_ptr<T> payload) : Signal(transaction_id,
                                                            signal_type)
    {
        payload_ = payload;
    }

    /**
     * \brief Returns the payload
     * \return carried payload
     */
    std::shared_ptr<T> GetPayload()
    {
        return payload_;
    }

    std::string ToString()
    {
        return "internal signal";
    }

private:
    /**
     * \brief No default constructor
     */
    InternalSignal();

    /**
     * \brief stores carried data
     */
    std::shared_ptr<T> payload_;

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_INTERNAL_SIGNAL_H_

/** \}    end of addtogroup */
