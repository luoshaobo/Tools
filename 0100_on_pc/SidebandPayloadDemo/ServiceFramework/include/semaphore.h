
#ifndef SEMAPHORE_H__
#define SEMAPHORE_H__

/**	@brief including all necessary headers
 *
*/
#include "operatingsystem.h"

#if defined(OS_WINDOWS)
#include "semaphore_WIN32.h"
#elif defined(OS_LINUX)
#include "semaphore_POSIX.h"
#endif


/**
 * @class Semaphore
 *
 * @brief
 *
 *
 */
class Lib_API Semaphore : private SemaphoreImpl
{
	public:
		Semaphore(unsigned int n);				//n=current value -> n must be greater zero
		Semaphore(unsigned int n,unsigned int max);		//max= max value for semaphore -> max>0 && n>0
		~Semaphore();

		void set();
		void wait();
		void wait( long milliseconds);

};

#endif //SEMAPHORE_H__
