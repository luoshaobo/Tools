#ifndef RELEASE_ARRAY_POLICY_H__
#define RELEASE_ARRAY_POLICY_H__

//#include "atomicCounter.h"
//#include <algorithm>

template <class C>
class  ReleaseArrayPolicy
{
public:
	static void release(C* pObj)
	{
		delete [] pObj;
	}
};


#endif //RELEASE_ARRAY_POLICY_H__