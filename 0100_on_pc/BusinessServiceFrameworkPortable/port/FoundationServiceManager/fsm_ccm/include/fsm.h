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
 *  \file     fsm.h
 *  \brief    Foundation Services Manager library (libfsm) common functions.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup foundationservicemanager
 *  \{
 */


#ifndef FSM_FSM_H_
#define FSM_FSM_H_

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

/******************************************************************************
 * MACROS
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus*/

/******************************************************************************
 * DEFINES
 ******************************************************************************/


/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

/*****************************************************************************/
/** \brief Initialize the libfsm library
 *****************************************************************************/
void FsmInitialize ();


/*****************************************************************************/
/** \brief Terminate the libfsm library
 *****************************************************************************/
void FsmTerminate ();

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus*/

#endif /*FSM_FSM_H_ */
/** \}    end of addtogroup */
