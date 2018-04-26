//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef RETAILMSG
#undef RETAILMSG
#endif
#define RETAILMSG(level,format)             wprintf format

#ifdef Debug
#undef Debug
#endif
#define Debug(format,...)                   wprintf(format, __VA_ARGS__)

#define BUFFER_SIZE 255   // tmp buffer for type


class CShell
{
private:
    TCHAR           m_Command[MAX_PATH];      // command
    TCHAR           m_fArgument[MAX_PATH];    // the first command option
    TCHAR           m_sArgument[MAX_PATH];    // the second command option

public:
    CShell();
    ~CShell();

    HRESULT Init(void);

    void PrintUsage(void);

    void ParseCommandLine(LPTSTR pCmd);

    void PrintExample(void);

    BOOL ParseQuote(LPTSTR *pCmd, LPTSTR filename);

    BOOL QALibCopyFile(PTCHAR pszFrom, PTCHAR pszTo);

    void APCFileCreate(LPTSTR pFile);

    void APCCreateDirectory(LPTSTR pDir);

    void APCRemoveDirectory(LPTSTR pDir);

    void APCListFiles(LPTSTR pRoot, DWORD cchRoot);

    void APCPrintFileInfo(WIN32_FIND_DATA fd);

    void APCPrintComma(DWORD size, DWORD *r1, DWORD *r2, DWORD *r3);

    void APCFileAttribute(PTCHAR pAttrib, DWORD cchAttrib, PTCHAR pFile, DWORD cchFile, BOOL bNoSwap);

    void APCDeleteFile(PTCHAR pFile);

    BOOL APCDeleteTree(LPTSTR pPath, DWORD cchPath);

    void APCTypeFile(PTCHAR pFile);

    BOOL MultiFile(PTCHAR pFrom, DWORD cchFrom, PTCHAR pTo, DWORD cchTo, UINT method); // 1: copy 2: delete 3: attrib

    BOOL IsDirectory(PTCHAR pFile);
};
