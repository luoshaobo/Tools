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
 *  \file     feature_configs.cc
 *  \brief    VOC specific feature configs
 *  \author   Piotr Tomaszewski
 *
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "features/feature_configs.h"

#include "signals/cloud_resources.h" //for feature name

namespace volvo_on_call
{

BasicCarControlFeatureConfig::BasicCarControlFeatureConfig()
    : FeatureConfig(kBasicCarControlResourceName)
{

}

} // namespace fsm
/** \}    end of addtogroup */
