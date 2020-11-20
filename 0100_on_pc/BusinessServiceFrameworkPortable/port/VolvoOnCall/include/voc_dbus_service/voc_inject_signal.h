/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     voc_inject_signal.h
 *  \brief    VOC test inject signal
 *  \author   Niklas Robertsson
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_INJECT_SIGNAL_H
#define VOC_INJECT_SIGNAL_H

// ** INCLUDES *****************************************************************
#include "features/car_access_feature.h"

namespace volvo_on_call
{

/**
 * \brief This class executes signal injection commands for the VOC app.
 * \note Should only be used for testing purposes.
 */
class VolvoOnCallTestInjectSignal
{
public:
    /**
     *  \brief Set CarAccessFeature object to be used in signal injections
     *  \param[in] ca The CarAccessFeature object
     */
    void SetCarAccessFeature(CarAccessFeature *ca);
    /**
     *  \brief Inject signals into the VOC app.
     *         Will try to execute the command line as a signal injection command
     *         Syntax is:
     *         inject_signal --signal-type type --signal-file file
     *         where signal-type should match a signal and signal_source type
     *         and signal-file should contain the signal in binary form.
     *
     *         signal-file will be read into memory and passed to signal_factory
     *         to create a signal of the appropriate type, which is then injected
     *         into the appropriate signal_source.
     *  \param[in] line The command line to handle.
     *  \return TRUE if command line can be parsed, FALSE otherwise.
     */
    bool InjectSignals(char *line);
    /**
     * \brief Returns reference of VolvoOnCallTestInjectSignal, implemented as a singelton
     * \return Reference to the instance of VolvoOnCallTestInjectSignal
     */
    static VolvoOnCallTestInjectSignal& GetInstance();
private:
    CarAccessFeature *ca_;
    VolvoOnCallTestInjectSignal() : ca_(nullptr) { }
    ~VolvoOnCallTestInjectSignal() { }
};

} // namespace volvo_on_call

#endif // VOC_INJECT_SIGNAL_H

/** \}    end of addtogroup */
