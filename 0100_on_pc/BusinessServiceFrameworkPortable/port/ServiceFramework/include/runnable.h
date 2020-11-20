/**
 * @file
 *
 * @brief
 *          Header file
 * @author  (last changes):
 *          - Elmar Weber
 *          - elmar.weber@continental-corporation.com
 *          - Continental AG
 * @par Project:
 * @par SW-Package:
 *
 * @par SW-Module:
 *
 * @note
 *
 * @par Module-History:
 *  Date        Author                   Reason
 *
 * @par Copyright Notice:
 * Copyright (C) Continental AG 2014
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 */

#ifndef OSAL_RUNNABLE_H__
#define OSAL_RUNNABLE_H__

#include "operatingsystem.h"

/**
* @class Runnable
*
* @brief definition of a run-method
*
*
*/
class Lib_API Runnable
{
public:
  Runnable();
  virtual ~Runnable();

  virtual void run() = 0;
};

#endif //OSAL_RUNNABLE_H__
