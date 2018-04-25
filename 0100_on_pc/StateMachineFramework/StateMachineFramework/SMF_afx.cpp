#include "TK_Tools.h"
#include "SMF_afx.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEngine.h"

unsigned int SMF_nPlatformErrorCode = 0;

std::string SMF_GetDomainStr(bool bSharedDomain /*= false*/)
{
    std::string sDomainStr;
    unsigned int nPid = SMF_INFINITE;

    if (!bSharedDomain) {
#ifdef WIN32
        nPid = ::GetCurrentProcessId();
#else
        nPid = ::getpid();
#endif // WIN32
    }

    TK_Tools::FormatStr(sDomainStr, "%08X", nPid);

    return sDomainStr;
}
