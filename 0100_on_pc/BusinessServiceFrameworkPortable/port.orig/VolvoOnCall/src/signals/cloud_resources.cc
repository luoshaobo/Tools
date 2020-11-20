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
 *  \brief    Voc specific cloud resources
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */



#include "signals/cloud_resources.h"


namespace volvo_on_call
{

BccFunc002CloudResource::BccFunc002CloudResource() : CloudResource(kBasicCarControlResourceName) {};

} // namespace fsm


/** \}    end of addtogroup */
