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

/**************************************************************************

    Set the registry according to the command-line argument
	ie. s regset HCU "Software\Microsoft\Automotive\Speech\Log\File" "\release\log\ssvctest.log"

****************************************************************************/
#include "atlbase.h"
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

//*******************************************************
//GLOBALS
//*******************************************************

void usage();


int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPreviousInstance,LPWSTR lpCommandLine,int nCommandShow)
{
	HRESULT hr = NULL;

    if (*lpCommandLine)
    {
        HKEY    hKey = 0;
        TCHAR   szPath[256];
        TCHAR   szKeyName[256];
        TCHAR   szKeyValue[256];
        long    iKeyValue=0;
        BOOL    positive=TRUE;
        TCHAR   *tail=NULL, *end=NULL;

        memset(szPath, 0, sizeof(szPath));
        memset(szKeyName, 0, sizeof(szKeyName));
        memset(szKeyValue, 0, sizeof(szKeyValue));

        if (!_tcsnicmp (lpCommandLine, TEXT("HKEY_CLASSES_ROOT"), _tcslen(TEXT("HKEY_CLASSES_ROOT"))))
        {
            hKey = HKEY_CLASSES_ROOT;
            lpCommandLine += _tcslen(TEXT("HKEY_CLASSES_ROOT"));
            
        }
        else if (!_tcsnicmp (lpCommandLine, TEXT("CLASSES"), _tcslen(TEXT("CLASSES"))))
        {
            hKey = HKEY_CLASSES_ROOT;
            lpCommandLine += _tcslen(TEXT("CLASSES"));
            
        }
        else if (!_tcsnicmp (lpCommandLine, TEXT("HCR"), _tcslen(TEXT("HCR"))))
        {
            hKey = HKEY_CLASSES_ROOT;
            lpCommandLine += _tcslen(TEXT("HCR"));
            
        }
        else if  (!_tcsnicmp (lpCommandLine, TEXT("HKEY_CURRENT_USER"), _tcslen(TEXT("HKEY_CURRENT_USER"))))
        {
            hKey = HKEY_CURRENT_USER;
            lpCommandLine += _tcslen(TEXT("HKEY_CURRENT_USER"));
            
        }
        else if  (!_tcsnicmp (lpCommandLine, TEXT("CURRENT"), _tcslen(TEXT("CURRENT"))))
        {
            hKey = HKEY_CURRENT_USER;
            lpCommandLine += _tcslen(TEXT("CURRENT"));
            
        } else if  (!_tcsnicmp (lpCommandLine, TEXT("HCU"), _tcslen(TEXT("HCU")))) {
            
            hKey = HKEY_CURRENT_USER;
            lpCommandLine += _tcslen(TEXT("HCU"));
            
        } else if  (!_tcsnicmp (lpCommandLine, TEXT("HKEY_LOCAL_MACHINE"), _tcslen(TEXT("HKEY_LOCAL_MACHINE")))) {
            
            hKey = HKEY_LOCAL_MACHINE;
            lpCommandLine += _tcslen(TEXT("HKEY_LOCAL_MACHINE"));
            
        } else if  (!_tcsnicmp (lpCommandLine, TEXT("HLM"), _tcslen(TEXT("HLM")))) {
            
            hKey = HKEY_LOCAL_MACHINE;
            lpCommandLine += _tcslen(TEXT("HLM"));
            
        } else if  (!_tcsnicmp (lpCommandLine, TEXT("HKEY_USERS"), _tcslen(TEXT("HKEY_USERS")))) {
            
            hKey = HKEY_USERS;
            lpCommandLine += _tcslen(TEXT("HKEY_USERS"));
            
        } else if  (!_tcsnicmp (lpCommandLine, TEXT("USERS"), _tcslen(TEXT("USERS")))) {
            
            hKey = HKEY_USERS;
            lpCommandLine += _tcslen(TEXT("USERS"));
            
        } else if  (!_tcsnicmp (lpCommandLine, TEXT("HU"), _tcslen(TEXT("HU")))) {
            
            hKey = HKEY_USERS;
            lpCommandLine += _tcslen(TEXT("HU"));
            
        } 
        else 
            goto exit1;

        // anything more on the command line?
        while(_istspace(*lpCommandLine))
            lpCommandLine++;

        if (lpCommandLine && *lpCommandLine != '\"')
            goto exit1;
        else
        {
            lpCommandLine++;
            if(lpCommandLine && (tail=_tcschr(lpCommandLine, '\"')))
            {
                _tcsncpy(szPath, lpCommandLine, (tail-lpCommandLine));
                if(end = _tcsrchr(szPath, '\\'))
                {
                    _tcscpy(szKeyName, end+1);
                    szPath[end-szPath] = '\0';
                } 

                // we need key name
                
                if(tail)   // skip the ending "
                    tail++;
                while(_istspace(*tail))
                    tail++;

                if(tail && *tail == '\"')  // it's a string
                {
                    tail++;
                    if(tail && (end=_tcsrchr(tail, '\"')))
                        _tcsncpy(szKeyValue, tail, (end-tail));
                    else if(tail)
                        _tcscpy(szKeyValue, tail);

					RETAILMSG(TRUE, (TEXT("RegSet: [%s] '%s'='%s'\r\n"), szPath, szKeyName, szKeyValue));

                    // CReg
                    CRegKey reg;
                    
                    reg.Create(hKey, szPath);
                    reg.SetStringValue(szKeyName, szKeyValue);
                }
                else if(tail)   // it's an integer
                {
                    iKeyValue = _wtol(tail);

                    // CReg
                    CRegKey reg;
                    
                    reg.Create(hKey, szPath);
                    reg.SetDWORDValue(szKeyName, iKeyValue);

					RETAILMSG(TRUE, (TEXT("RegSet: [%s] '%s'=%d\r\n"), szPath, szKeyName, iKeyValue));
                }
                else
                    goto exit1;
            }
            else
                goto exit1;
        }
        return 0;
    }

exit1:
	usage();

    return E_FAIL;
}

void usage()
{
	RETAILMSG(TRUE, (TEXT("\r\nREGSET <ROOT_KEY> \"Path\\KeyName\" KeyValue \r\n")));
	RETAILMSG(TRUE, (TEXT("\twhere ROOT_KEY is: \r\n")));
	RETAILMSG(TRUE, (TEXT("\t\tHKEY_LOCAL_MACHINE or HLM \r\n")));
	RETAILMSG(TRUE, (TEXT("\t\tHKEY_CLASSES_ROOT or HCR or CLASSES\r\n")));
	RETAILMSG(TRUE, (TEXT("\t\tHKEY_USERS or HU or USERS\r\n")));
	RETAILMSG(TRUE, (TEXT("\texample:\r\n")));
	RETAILMSG(TRUE, (TEXT("\t\tregset HLM \"stations\\station1\\name\" \"user string\" \r\n")));
	RETAILMSG(TRUE, (TEXT("\t\tregset HLM \"stations\\station1\\id\" -10 \r\n\r\n")));
}
    
