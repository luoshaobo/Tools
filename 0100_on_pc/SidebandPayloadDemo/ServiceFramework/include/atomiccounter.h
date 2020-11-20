/**
* @file
*          atomiccounter.h
*
* @brief
*          Definiton of the constructors (including copy constructor) and of the
*          method value() and overloading of the operators =, ++, -- and !
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
#ifndef ATOMIC_COUNTER_H__
#define ATOMIC_COUNTER_H__

#include "mutex.h"

/**
 * @class AtomicCounter
 *
 */
class Lib_API AtomicCounter
{
public:

  /**	@brief typedef of int to ValueType
   *
   */
  typedef int ValueType;

  /**	@brief decalaration constructor that creates a new AtomicCounter and initializes it to zero
   *
   */
  AtomicCounter();

  /**	@brief decalaration constructor that creates a new AtomicCounter and initializes it with the given value
   *
   */
  explicit AtomicCounter(ValueType initialValue);

  /**	@brief decalaration constructor that creates the counter by copying another one.
   *
   */
  AtomicCounter(const AtomicCounter& counter);

  /**	@brief decalaration destructor
   *
   */
  ~AtomicCounter();

  /**	@brief declaration overwriting the operator = for the assignment of the value of another AtomicCounter
   *
   */
  AtomicCounter& operator = (const AtomicCounter& counter);

  /**	@brief declaration overwriting the operator = for the assignment of a value to the counter
   *
   */
  AtomicCounter& operator = (ValueType value);

  /**	@brief declaration of the method value() that returns the value of the counter
   *
   */
  ValueType value() const;

  /**	@brief declaration overwriting the operator ++ that increments the counter and returns the result (prefix)
   *
   */
  ValueType operator ++ ();

  /**	@brief declaration overwriting the operator ++ that increments the counter and returns the previous value (postfix)
   *
   */
  ValueType operator ++ (int);

  /**	@brief declaration overwriting the operator -- that decrements the counter and returns the result (prefix)
   *
   */
  ValueType operator -- ();

  /**	@brief declaration overwriting the operator -- that decrements the counter and returns the previous value (postfix)
   *
   */
  ValueType operator -- (int);

  /**	@brief declaration overwriting the operator ! that returns true if the counter is zero, false otherwise.
   *
   */
  bool operator ! () const;


private:

  /**	@brief declaration of several attributes
  */
  mutable Mutex m_mutex;
  volatile ValueType  m_value;
  bool m_bVerbose;


};

#endif //ATOMIC_COUNTER_H__
