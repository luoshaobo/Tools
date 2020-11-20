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
 *  \file     cloud_resource.h
 *  \brief    Cloud Resource
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */


#include "dlt/dlt.h"

#include "voc_framework/signals/cloud_resource.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

CaFunc002CloudResource::CaFunc002CloudResource() : CloudResource(kCarAccessResourceName) {};

} // namespace fsm


/** \}    end of addtogroup */
