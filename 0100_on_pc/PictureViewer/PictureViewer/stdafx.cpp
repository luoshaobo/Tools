
// stdafx.cpp : source file that includes just the standard includes
// PictureViewer.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#include <io.h>
#include <fcntl.h>
#include <io.h>
#include <fcntl.h>

#if 1
void OpenConsole()
{
    if (!PathFileExists(_T("PictureViewer.vcproj"))) {
        return;
    }

    AllocConsole();
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long)handle,_O_TEXT);
    FILE* hf = _fdopen(hCrt, "w");
    *stdout = *hf;
}
#else
void OpenConsole()
{
}
#endif // #if 0

int PrintDebugInfo(const char *pFormat, ...)
{
    int nSize = 0;
    char *buff = NULL;

    if (pFormat == NULL) {
        nSize = 0;
        goto FAILED;
    }

#define FORMAT_BUF_SIZE                         4096
    buff = new char[FORMAT_BUF_SIZE];
    if (buff == NULL) {
        nSize = 0;
        goto FAILED;
    }
    
    memset(buff, 0, FORMAT_BUF_SIZE);
    va_list args;
    va_start(args, pFormat);
    nSize = _vsnprintf(buff, FORMAT_BUF_SIZE - 1, pFormat, args);
    va_end(args);
#undef FORMAT_BUF_SIZE

    OutputDebugStringA(buff);
    
FAILED:
    if (buff != NULL) {
        delete [] buff;
        buff = NULL;
    }
    return nSize;
}
