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
 *  \file     dlt.h
 *  \brief    DLT stubs
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup unittest
 *  \{
 */

#ifndef __STUB_DLT_H_INC__
#define __STUB_DLT_H_INC__

#include <stdio.h>

#define DLT_IMPORT_CONTEXT(...)

#ifdef __UT_DLT_DISABLE_PRINTF__
#define DLT_LOG_STRING(...)
#define DLT_LOG_STRINGF(...)
#else
#define DLT_LOG_STRINGF(context, logZone, format, ...) \
    printf(format "\n", __VA_ARGS__)
#define DLT_LOG_STRING(context, logZone, str) \
    printf("%s\n", str)
#endif

#endif

/** \}    end of addtogroup */