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
 *  \file     cloud_resources.h
 *  \brief    VOC specific cloud resources
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */


#ifndef VOC_SIGNALS_CLOUD_RESOURCES_H_
#define VOC_SIGNALS_CLOUD_RESOURCES_H_

#include "fscfg/bus_names.h"
#include "voc_framework/signals/cloud_resource.h"

#include "basic_car_control_signal.h"


namespace volvo_on_call
{

const std::string kBasicCarControlResourceName = std::string(fsm::fscfg_kBasicCarControlResource);;

/**
 * \brief BccFunc002CloudResource class is a specialized helper class for accessing
 *        the basic car control configuration.
 * */
class BccFunc002CloudResource: public fsm::CloudResource<BasicCarControlSignal>
{
public:
    /**
     * \brief Constructs BccFunc002CloudResource instance.
     */
    BccFunc002CloudResource();
};

} // namespace fsm

#endif //VOC_SIGNALS_CLOUD_RESOURCES_H_

/** \}    end of addtogroup */
