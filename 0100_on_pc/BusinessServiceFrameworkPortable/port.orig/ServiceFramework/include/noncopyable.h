/**
* @file
*          noncopyable.h
*
* @brief
*          Private copy constructor and copy assignment ensure classes derived
*          from class cannot be copied.
*
* @par Author (last changes):
*          - Elmar Weber
*          - Continental Automotive GmbH / A Company of the Continental Corporation
* @par PL:
* @par Responsible Architect:
* @par Project or Platform:
* @par SW-Component:
* @par SW-Package:
* @par SW-Module:
* @par PackageID
*          N/A: used by different packages
* @par Interface Status
*          Not released
*
* @par Bugs:
*          -
* @note
*          -
*
* @warning
*          -
*
* @par Description:
*
* @par Module-History:
* @verbatim
*  Date        Author                  Reason
*  03.03.2015  Elmar Weber             First version
*
* @endverbatim
*
* @par Copyright Notice:
* @verbatim
* Copyright (C) 2015
* Continental Automotive GmbH / A Company of the Continental Corporation
* Alle Rechte vorbehalten. All Rights Reserved.
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
* @endverbatim
*/

#ifndef NON_COPYABLE_H__
#define NON_COPYABLE_H__

class  NonCopyable
{
protected:
  NonCopyable() {}
  virtual ~NonCopyable() {}
//private:  //NOTE: ARM-2012.03-Compiler does not accept to have the Copyconstructor private
  //DO NOT IMPLEMENT THESE!!!
  NonCopyable ( NonCopyable const & );
  NonCopyable & operator = ( NonCopyable const & );
};

#endif // NON_COPYABLE_H__
