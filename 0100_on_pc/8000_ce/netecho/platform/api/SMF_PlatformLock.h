#ifndef SMF_PLATFORM_LOCK_H__7393497209483787621483959340678258936892783534868944
#define SMF_PLATFORM_LOCK_H__7393497209483787621483959340678258936892783534868944

#include "SMF_afx.h"

class SMF_PlatformLockImpl;

class SMF_PlatformLock : private SMF_NonCopyable
{
public:
    SMF_PlatformLock(const char *pName = NULL);
    virtual ~SMF_PlatformLock();

public:
    SMF_ErrorCode Lock(unsigned int nTimeout = SMF_INFINITE);
    SMF_ErrorCode TryLock(bool &bLocked);
    SMF_ErrorCode Unlock();

public:
    SMF_PlatformLockImpl *m_pImpl;
};

#endif // #ifndef SMF_PLATFORM_LOCK_H__7393497209483787621483959340678258936892783534868944
