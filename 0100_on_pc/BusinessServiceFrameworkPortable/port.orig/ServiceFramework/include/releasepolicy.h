#ifndef RELEASE_POLICY_H__
#define RELEASE_POLICY_H__

template <class C>
class ReleasePolicy
{
public:
	static void release(C* pObj)
	{
		delete pObj;
	}
};


#endif //RELEASE_POLICY_H__