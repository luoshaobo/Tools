#ifndef REFERENCE_COUNTER_H__
#define REFERENCE_COUNTER_H__

#include "atomiccounter.h"
#include <algorithm>

class Lib_API ReferenceCounter
{
public:
	ReferenceCounter(): m_cnt(1)
	{
		//printf("ReferenceCounter=%d\n", m_cnt.value());
	}

	void duplicate()
	{
		++m_cnt;
		//printf("ReferenceCounter=%d\n", m_cnt.value());
	}

	int release()
	{
		//printf("ReferenceCounter=%d\n", m_cnt.value());
        //if (--m_cnt == 0) delete this;
        int cnt = (--m_cnt);

        return cnt;
	}

	int referenceCount() const
	{
		//printf("ReferenceCounter=%d\n", m_cnt.value());
		return m_cnt.value();
	}

private:
	AtomicCounter m_cnt;
};

#endif //REFERENCE_COUNTER_H__
