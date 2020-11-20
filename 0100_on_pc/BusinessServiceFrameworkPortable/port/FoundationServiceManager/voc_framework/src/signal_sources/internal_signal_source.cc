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
 *  \file     internal_signal_source.cc
 *  \brief    VOC Service internal signal source class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signal_sources/internal_signal_source.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);


namespace fsm
{

InternalSignalSource::InternalSignalSource()
{
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "Initializing InternalSignalSource");
}

InternalSignalSource::~InternalSignalSource()
{
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "Destroying InternalSignalSource");
}


InternalSignalSource& InternalSignalSource::GetInstance()
{
    static InternalSignalSource instance;
    return instance;
}


} // namespace fsm
/** \}    end of addtogroup */
