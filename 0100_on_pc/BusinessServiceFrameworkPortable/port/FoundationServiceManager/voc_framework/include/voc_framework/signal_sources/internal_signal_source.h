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
 *  \file     internal_signal_source.h
 *  \brief    VOC Service internal signal source class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNAL_SOURCES_INTERNAL_SIGNAL_SOURCE_H_
#define VOC_FRAMEWORK_SIGNAL_SOURCES_INTERNAL_SIGNAL_SOURCE_H_

#include "voc_framework/signal_sources/signal_source.h"
#include "voc_framework/signals/signal.h"


namespace fsm
{

/**
 * \brief Class used for distributing internal signals in the applications
 *        based on the voc framework.
 *        Any type of signal can be distributed, including application
 *        specific signals. For a generic data carrier see InternalSignal template
 */
class InternalSignalSource: public SignalSource
{

 public:

    /**
     * \brief Returns reference of InternalSignalSource, implemented as a singelton
     * \return reference to the instance of InternalSignalSource
     */
    static InternalSignalSource& GetInstance();

 private:

    /**
     * \brief Private contructor
     */
    InternalSignalSource();

    /**
     * \brief Private destructor
     */
    ~InternalSignalSource();
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNAL_SOURCES_INTERNAL_SIGNAL_SOURCE_H_

/** \}    end of addtogroup */
