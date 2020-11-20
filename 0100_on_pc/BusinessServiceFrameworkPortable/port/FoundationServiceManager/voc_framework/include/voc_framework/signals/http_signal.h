///////////////////////////////////////////////////////////////////
// Copyright (C) 2019 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file http_signal.h
//  This file handles the http callback response.
// @project     GLY_TCAM
// @subsystem
// @author     
// @Init date   20-Mar-2019
///////////////////////////////////////////////////////////////////

#ifndef HTTP_RESPONSE_SIGNAL_H_
#define HTTP_RESPONSE_SIGNAL_H_

#include "voc_framework/signals/signal.h"

typedef struct 
{
    uint64_t fileSize;
    std::string payload;
}HttpData;


namespace fsm
{

class HttpSignal : public Signal
{

 public:

    /**
     * \brief Create a HttpSignal.
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
    static std::shared_ptr<HttpSignal> CreateHttpSignal (HttpData payload, TransactionId& transaction_id)
    {
        std::shared_ptr<HttpSignal> return_signal;
        return_signal = std::shared_ptr<HttpSignal>(
                            new HttpSignal(payload, transaction_id, kHttpSignal));

        return return_signal;
    }

    /**
     * \brief Get the payload contained in the signal.
     * \return the payload.
     */
    const HttpData& GetPayload ()
    {
        return payload_;
    }


    /**
     * \brief Serialize to string, used for debug printouts. Must be implemented by subclasses.
     * \return Name or other text identifier of the signal.
     */
    std::string ToString ()
    {
        return "HttpSignal";
    }


 private:

    HttpSignal (HttpData payload,TransactionId& transaction_id,SignalType signal_type) : 
        Signal(transaction_id, signal_type),
        payload_(payload) {}


    /**
     * \brief The payload contained in this signal.
     *        Typically a struct define by VpomSignal
     */
    const HttpData payload_;

};

} // namespace fsm

#endif // HTTP_RESPONSE_SIGNAL_H_

/** \}    end of addtogroup */
