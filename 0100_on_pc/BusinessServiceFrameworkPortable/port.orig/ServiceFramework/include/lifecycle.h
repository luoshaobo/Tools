/**
 * @file
 *          lifecycle.h
 * @brief
 *          Header file containing lifecycle interface
 * @author  (last changes):
 *          - Elmar Weber
 *          - elmar.weber@continental-corporation.com
 *          - Phone +49(941)790-90597
 *          - Continental AG
 * @par Project:
 * @par SW-Package:
 * @par SW-Module:
 * @note
 *          Header file containing lifecycle interface that has to be implemented
 *          by every component to participate on the lifecyle process
 *
 * @par Module-History:
 *  Date        Author                   Reason
 *
 * @par Copyright Notice:
 * Copyright (C) Continental AG 2016
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 */

#ifndef LIFECYCLE_H__
#define LIFECYCLE_H__

#include "operatingsystem.h"

class Lib_API ILifeCycle
{
public:

    ILifeCycle();
    virtual ~ILifeCycle();

    //----- lifecycle interface
    virtual void initialize() = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual void terminate() = 0;
    virtual void changeConfigMode(int mode) = 0;

protected:
    int m_CompId;
    long m_InitializationTime;
    long m_ActivationTime;
    long m_DeactivationTime;
};

#endif //LIFECYCLE_H__
