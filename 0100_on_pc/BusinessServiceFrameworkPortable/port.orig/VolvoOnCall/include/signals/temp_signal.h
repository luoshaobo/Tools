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
 *  \file     temp_signal.h
 *  \brief    catalogue signal, triggering the catalogue upload.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_TEMP_SIGNAL_H_
#define VOC_SIGNALS_TEMP_SIGNAL_H_

#include "voc_framework/signals/signal.h"


namespace volvo_on_call
{
/*
 * TODO: This is a temporary signal that will trigger catalogue upload. Change to appropriate signal implementation
 */
class TempSignal: public fsm::Signal
{

 public:


    TempSignal(fsm::TransactionId& transaction_id, SignalType type): fsm::Signal::Signal(transaction_id,
                                                                                         type) {};

    std::string ToString()  {return "TempSignal of type" + GetSignalType(); };
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_CATALOGUE_SIGNAL_H_

/** \}    end of addtogroup */
