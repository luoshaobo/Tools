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
 *  \file     signal_receiver_interface.h
 *  \brief    VOC Service signal reciever interface.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_SIGNAL_RECEIVER_INTERFACE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_SIGNAL_RECEIVER_INTERFACE_H_

#include "voc_framework/signals/signal.h"



namespace fsm
{

class SignalReceiverInterface
{
public:
    /**
     * \brief Interface to receive a new signal. Must be defined by a class implementing this interface.
     *  \param[in]  signal - signal for processing.
     */
    virtual void ProcessSignal (std::shared_ptr<Signal> signal) = 0;
};

} // namespace fsm
/** \}    end of addtogroup */

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_SIGNAL_RECEIVER_INTERFACE_H_
