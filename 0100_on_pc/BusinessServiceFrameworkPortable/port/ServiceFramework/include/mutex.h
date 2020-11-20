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

#ifndef MUTEX_H__
#define MUTEX_H__

/**	@brief including all necessary headers
 *
*/
#include "operatingsystem.h"

#if defined(OS_WINDOWS)
  #include "mutex_WIN32.h"
#elif defined(OS_LINUX)
  #include "mutex_POSIX.h"
#endif


/**
 * @class Mutex
 *
 * @brief Definiton of a lock and an unlock method
 *
 *
 */

class Lib_API Mutex : private MutexImpl
{
	public:
		Mutex();
		~Mutex();

		void lock();
		void lock(long milliseconds);
		void unlock();


	private:
		long m_mutexMilliseconds;

};

#endif //MUTEX_H__
