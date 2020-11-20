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

#ifndef THREAD_H__
#define THREAD_H__

/** brief including all necessary headers
*
*/
#include "operatingsystem.h"

#if defined(OS_WINDOWS)
#include "thread_WIN32.h"
#elif defined(OS_LINUX)
#include "thread_POSIX.h"
#endif

#include "mutex.h"
#include <string>


/**
* @class Thread
*
* @brief Integration of several methods and attributes concerning threads
*
* Definiton of the methods getTid, getName, setName, setStackSize, getStackSize, start, join, isRunning, sleep, yield and getCurrentTid
*/


class Lib_API Thread : private ThreadImpl
{

  typedef unsigned long TID;

public:

  /**	@brief decalaration construktor
  *
  */
  Thread();
  ~Thread();


  Thread(Thread const&) = delete;
  Thread& operator=(Thread const&) = delete;

  /**	@brief construktor
  *	@param name Name
  *
  */
  Thread(const std::string &name);

  /**	@brief declaration of getTid-function
  */
  TID getTid() const;

  /**	@brief declaration of getName-function
  */
  std::string getName() const;

  /**	@brief declaration of setName-function
  */
  void setName(std::string name);

  /**	@brief declaration of setStackSize-function
  */
  void setStackSize(int size);

  /**	@brief declaration of getStackSize-function
  */
  int getStackSize() const;

  /**	@brief declaration of start-function
  */
  void start(Runnable &runnable);

  /**	@brief declaration of join(void)-function
  */
  void join();

  /**	@brief declaration of join(bool)-function
  */
  bool join(long milliseconds);

  /**	@brief declaration of isRunning-function
  */
  bool isRunning() const;

  /**	@brief declaration of sleep-function
  */
  static void sleep(long milliseconds);

  /**	@brief declaration of yield-function
  */
  static void yield();

  /**	@brief declaration of getCurrentTid-function
  */
  static TID getCurrentTid();

protected:
  std::string makeName();
  static int uniqueId();

private:
  /**	@brief declaration of several attributes
  */
  int m_id;
  std::string m_name;
  bool m_run;
  mutable Mutex   m_mutex;
};

#endif //THREAD_H__
