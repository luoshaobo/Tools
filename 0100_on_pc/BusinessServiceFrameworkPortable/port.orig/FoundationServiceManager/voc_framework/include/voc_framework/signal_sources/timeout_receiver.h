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
 *  \file     timeout_receiver.h
 *  \brief    VOC Service timeout receiver class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup SW_package
 *  \{
 */

#ifndef VOC_FRAMEWORK_TIMEOUT_RECEIVER_H_
#define VOC_FRAMEWORK_TIMEOUT_RECEIVER_H_

#include "voc_framework/transactions/transaction_id.h"
#include "voc_framework/signals/timeout_signal.h"


namespace fsm
{
/**
 * \brief Implements support for timeouts. Note, as this interface uses weak pointers to the timeout
 *        receivers, the objects using this interface must be heap allocated shared pointers
 */
class TimeoutReceiver: public std::enable_shared_from_this<TimeoutReceiver>
{
    /**
     * \brief Keeps track if any timeout has been requested from this interface.
     *        Used as an optimization, to avoid unecessary cleanup operations in
     *        destructors of receiver entities that never actually requested any
     *        timer. This is an optimization only, there is no guarantee that
     *        there is a timer to remove even if one was requested at some point,
     *        but the assumption here is that many entities will never request
     *        a timer, and therefore they never should need to request cleanup.
     */
    bool timeout_requested_ = false;

 public:

    /**
     * \brief Destructs the TimeoutReceiver object. Assures that no timeouts
     *        will be reported for a given listener after it has been destructed
     */
    virtual ~TimeoutReceiver ();

    /**
     * \brief Process the timeout signal. This method needs to be implemented by every receiver
     *  \param[in]  timeout_signal - signal for processing.
     */
    virtual void HandleTimeout (std::shared_ptr<TimeoutSignal> timeout_signal) = 0;

 protected:

    /**
     * \brief Request timeout with given parameters.
     *  \param[in]  delay - delay in seconds
     *  \param[in]  periodic - true if the request is for periodic timeout, false for single.
     *                         Default is false.
     */
    TimeoutTransactionId RequestTimeout (std::chrono::seconds delay, bool periodic = false);
    
    /**
     * \brief Request timeout with given parameters.
     *  \param[in]  transaction_id - timeout id
     *  \param[in]  delay - delay in seconds
     *  \param[in]  periodic - true if the request is for periodic timeout, false for single.
     *                         Default is false.
     */
    void RequestTimeout (TimeoutTransactionId transaction_id, std::chrono::seconds delay, bool periodic = false);

    /**
     * \brief Removes timeout request with a given timeout transaction id
     * \param[in] transaction_id id of the timeout to remove
     */
    void RemoveTimeout (TimeoutTransactionId transaction_id);


    /**
     * \brief Removes all timeout requests issued by this timer receiver
     */
    void RemoveMyTimeouts ();

};

} // namespace fsm

#endif //VOC_FRAMEWORK_TIMEOUT_RECEIVER_H_

/** \}    end of addtogroup */
