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
 *  \file     timeout_signal.h
 *  \brief    VOC Service timeout signal class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_TIMEOUT_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_TIMEOUT_SIGNAL_H_

#include "voc_framework/signals/signal.h"



namespace fsm
{

class TimeoutSignal : public Signal
{

 public:


    /**
     * \brief Create timeout signal with a given id
     * \param[in] timeout_id timeout id
     */
    TimeoutSignal (TimeoutTransactionId& timeout_id);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString();

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_TIMEOUT_SIGNAL_H_

/** \}    end of addtogroup */
