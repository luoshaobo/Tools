/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     fsm.c
 *  \brief    Foundation Services Manager library (libfsm) common functions.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup foundationservicemanager
 *  \{
 */


/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include "fsm.h"
#include "dlt/dlt.h"


/******************************************************************************
 * GLOBAL VARIABLES AND CONSTANTS
 ******************************************************************************/

DLT_DECLARE_CONTEXT(dlt_libfsm);

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

void FsmInitialize ()
{
    DLT_REGISTER_CONTEXT(dlt_libfsm, "LFSM","libfsm logs");
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "Initializing libfsm");
}

void FsmTerminate ()
{
    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_INFO, "Terminating libfsm");
    DLT_UNREGISTER_CONTEXT(dlt_libfsm);
}
