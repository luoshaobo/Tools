#ifndef SMF_PLATFORM_SHARED_MEM_H__7248923957291478236541284780294895729047234894738
#define SMF_PLATFORM_SHARED_MEM_H__7248923957291478236541284780294895729047234894738

#include "SMF_afx.h"

class SMF_PlatformSharedMemImpl;

class SMF_PlatformSharedMem : private SMF_NonCopyable
{
public:
    SMF_PlatformSharedMem(unsigned int nSharedMemSize, const char *pName = NULL);
    virtual ~SMF_PlatformSharedMem();

public:
    SMF_ErrorCode Map();
    SMF_ErrorCode Unmap();

public:
    bool AlreadyExists();
    void *GetSharedMemAddr();

public:
    SMF_PlatformSharedMemImpl *m_pImpl;
};

#endif // #ifndef SMF_PLATFORM_SHARED_MEM_H__7248923957291478236541284780294895729047234894738
