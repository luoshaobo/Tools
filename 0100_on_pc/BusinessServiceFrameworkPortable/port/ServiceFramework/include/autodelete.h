/**
* @file
*          autodelete.h
*
* @brief
*          Private copy constructor and copy assignment ensure classes derived
*          from class cannot be copied. The class is a helper to ensure memory
*          gets freed when leaving scope
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

#ifndef AUTO_DELETE_H__
#define AUTO_DELETE_H__


#include "noncopyable.h"

template <class T>
class AutoDelete : private NonCopyable
{
  public:
    AutoDelete ( T* pObject, bool isArray = false ) : NonCopyable(),
                                                          m_pObject( pObject ),
                                                          m_bArray( isArray )
    {
    }

    virtual ~AutoDelete()
    {
      release();
    }

    void assign( T* pObject = 0, bool isArray = false )
    {
      release();

      m_pObject = pObject;
      m_bArray = isArray;
    }

    T& operator*() const
    {
      return ( *(T *)m_pObject );
    }

    T *operator->()
    {
      return (&**this);
    }

    T *get() const
    {
      return ((T *)m_pObject);
    }
  protected:
    void release()
    {
      if ( m_bArray == false )
      {
        delete m_pObject;
      }
      else
      {
        delete [] m_pObject;
      }
      m_pObject = 0;
    }

  T* m_pObject;
  bool m_bArray;
};

#endif //AUTO_DELETE_H__
