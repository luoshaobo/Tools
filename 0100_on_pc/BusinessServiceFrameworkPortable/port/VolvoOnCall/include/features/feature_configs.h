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
 *  \file     feature_configs.h
 *  \brief    VOC specific feature configs
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FEATURES_FEATURE_CONFIGS_H_
#define VOC_FEATURES_FEATURE_CONFIGS_H_

#include "voc_framework/features/feature_config.h"

namespace volvo_on_call
{

/**
 * \brief BasicCarControlFeatureConfig class is a specialized helper class for accessing
 *        the feature list level of the basic car control feature configuration.
 * */
class BasicCarControlFeatureConfig: public fsm::FeatureConfig
{
public:
    /**
     * \brief Constructs BasicCarControlFeatureConfig instance.
     */
    BasicCarControlFeatureConfig();
};


} // namespace fsm

#endif //VOC_FEATURES_FEATURE_CONFIGS_H_

/** \}    end of addtogroup */
