/**
 * Copyright (C) 2016 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     car_access_feature.h
 *  \brief    VOC Service Car Access fsm::Feature class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FEATURES_CAR_ACCESS_H_
#define VOC_FEATURES_CAR_ACCESS_H_

// fsm includes
#include "voc_framework/features/feature.h"

namespace volvo_on_call
{

class CarAccessFeature: public fsm::Feature
{

 public:

    CarAccessFeature ();

    /**
     * \brief Handle a signal.
     * \param[in] signal The signal to handle.
     * \return None.
     */
    void HandleSignal(std::shared_ptr<fsm::Signal> signal);

};

} // namespace volvo_on_call
/** \}    end of addtogroup */

#endif //VOC_FEATURES_CAR_ACCESS_H_
