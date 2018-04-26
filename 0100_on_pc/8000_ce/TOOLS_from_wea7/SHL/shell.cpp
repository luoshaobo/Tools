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

    supports copy, dir, del, deltree, type, attrib, md, rd, create

****************************************************************************/

#include "shell.h"
#include <ctype.h>
#include <strsafe.h>

// supported commands
#define CMD_COPY                  (L"copy")
#define CMD_ATTRIB                (L"attrib")
#define CMD_DELTREE               (L"deltree")
#define CMD_DEL                   (L"del")
#define CMD_MD                    (L"md")
#define CMD_RD                    (L"rd")
#define CMD_DIR                   (L"dir")
#define CMD_CREATE                (L"create")
#define CMD_TYPE                  (L"type")
#define CMD_HELP                  (L"help")
#define CMD_EXAMPLE               (L"example")

#define countof(x)  (sizeof(x)/sizeof(*(x)))

// void Debug(LPCTSTR szFormat, ...)
// {
	// static  TCHAR   szHeader[] = TEXT("TUXTEST: ");
    // TCHAR   szFormatBuffer[1024] = {0};
    // TCHAR   szOutputBuffer[1024] = {0};
    // HRESULT hr = S_OK;

	// va_list pArgs;
	// va_start(pArgs, szFormat);
    // hr = StringCchVPrintf(
        // szFormatBuffer,
        // countof(szFormatBuffer),
    	// szFormat,
	    // pArgs);
	// va_end(pArgs);

    // hr = StringCchCopy( szOutputBuffer, countof(szOutputBuffer), szHeader );
    // hr = StringCchCat( szOutputBuffer, countof(szOutputBuffer), szFormatBuffer );

	// OutputDebugString(szOutputBuffer);
// }

//--------------------------------------------------------
//
//    Function: CShell Constructor
//
//    Synopsis: Initialization for CShell
//
//    Arguments: none
//
//    Returns: none
//
//--------------------------------------------------------

CShell::CShell()
{
    memset(m_Command, 0, sizeof(m_Command));       // command
    memset(m_fArgument, 0, sizeof(m_fArgument));   // the first option for the command
    memset(m_sArgument, 0, sizeof(m_sArgument));   // the second option for the command
}

//--------------------------------------------------------
//
//    Function: CShell destructor
//
//    Synopsis: house keeping work, delete log object
//
//    Arguments: none
//
//    Returns: none
//
//--------------------------------------------------------

CShell::~CShell()
{
}

//--------------------------------------------------------
//
//    Function: CShell Init
//
//    Synopsis: create a log object
//
//    Arguments: none
//
//    Returns: NOERROR indicates success
//
//--------------------------------------------------------

HRESULT CShell::Init()
{
    HRESULT hr=NOERROR;

    return hr;
}

//--------------------------------------------------------
//
//    Function: CShell ParseCommandLine
//
//    Synopsis: parse command line options,
//              call corresponding function for processing command
//
//    Arguments:
//
//              pCmd - command line options
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::ParseCommandLine(LPTSTR pCmd)
{
    LPTSTR pTmp=NULL;
    TCHAR pfArg[MAX_PATH]={0};  // tmp space holding mid value for 1st argument
    TCHAR psArg[MAX_PATH]={0};  // tmp space holding mid value for 2nd argument
    TCHAR tArg[MAX_PATH]={0};

    // get command
    if( pTmp = _tcstok(pCmd, TEXT(" \t\n")) )
    {
        StringCchCopyN(m_Command, countof(m_Command), pTmp, MAX_PATH-1);
        pCmd +=  _tcslen(m_Command)+1;

        // parse quote for command options
        if(!ParseQuote(&pCmd, pfArg))
            goto LReturn;

        if(!ParseQuote(&pCmd, psArg))
            goto LReturn;

        StringCchCopyN(m_fArgument, countof(m_fArgument), pfArg, sizeof(m_fArgument)/sizeof(m_fArgument[0]));
        m_fArgument[sizeof(m_fArgument)/sizeof(m_fArgument[0])-1] = '\0';

        StringCchCopyN(m_sArgument, countof(m_sArgument), psArg, sizeof(m_sArgument)/sizeof(m_sArgument[0]));
        m_sArgument[sizeof(m_sArgument)/sizeof(m_sArgument[0])-1] = '\0';

        Debug( TEXT("SH: \n"));

        // diff command goes to diff branch
        if(!_tcsicmp(m_Command,CMD_COPY))
        {
            if( !MultiFile(m_fArgument, countof(m_fArgument), m_sArgument, countof(m_sArgument), 1) )
            {
                LPTSTR tmp=NULL;

                if(_tcslen(m_sArgument) && m_sArgument[_tcslen(m_sArgument)-1] == '\\')  // dest is a directory
                {
                    HRESULT hrIgnore = S_OK;
                    tmp = _tcsrchr(m_fArgument, '\\');
                    if(tmp)
                        hrIgnore = StringCchCat(m_sArgument, countof(m_sArgument), tmp+1);
                    else   // root file
                        hrIgnore = StringCchCat(m_sArgument, countof(m_sArgument), m_fArgument);
                }
                QALibCopyFile(m_fArgument, m_sArgument);
            }
        }
        else if(!_tcsicmp(m_Command,CMD_TYPE))
        {
            APCTypeFile(m_fArgument);
        }
        else if(!_tcsicmp(m_Command,CMD_ATTRIB))
        {
            if( !MultiFile(m_fArgument, countof(m_fArgument), m_sArgument, countof(m_sArgument), 3) )
                APCFileAttribute(m_fArgument, countof(m_fArgument), m_sArgument, countof(m_sArgument), FALSE);
        }
        else if(!_tcsicmp(m_Command,CMD_DELTREE))
        {
            APCDeleteTree(m_fArgument, countof(m_fArgument));
        }
        else if(!_tcsicmp(m_Command,CMD_DEL))
        {
            if( !MultiFile(m_fArgument, countof(m_fArgument), m_sArgument, countof(m_sArgument), 2) )
                APCDeleteFile(m_fArgument);
        }
        else if(!_tcsicmp(m_Command,CMD_MD))
        {
            APCCreateDirectory(m_fArgument);
        }
        else if(!_tcsicmp(m_Command,CMD_RD))
        {
            APCRemoveDirectory(m_fArgument);
        }
        else if(!_tcsicmp(m_Command,CMD_DIR))
        {
            APCListFiles(m_fArgument, countof(m_fArgument));
        }
        else if(!_tcsicmp(m_Command,CMD_CREATE))
        {
            APCFileCreate(m_fArgument);
        }
        else if(!_tcsicmp(m_Command,CMD_HELP))
        {
            PrintUsage();
        }
        else if(!_tcsicmp(m_Command,CMD_EXAMPLE))
        {
            PrintExample();
        }
        else
            goto LReturn;
    }
    else
        goto LReturn;

    Debug( TEXT("SH: \n"));

    return;

LReturn:

    PrintUsage();
    return;
}

//--------------------------------------------------------
//
//    Function: CShell ParseQuote
//
//    Synopsis: parse paired Quote
//
//    Arguments:
//
//              *pCmd - [in] content to be processed
//              filename - [out] return place for the parsed result
//
//    Returns: TRUE indicates success
//
//--------------------------------------------------------

BOOL CShell::ParseQuote(LPTSTR *pCmd, LPTSTR filename)
{
    LPTSTR tmp=NULL;
    TCHAR save[MAX_PATH]={0};
    UINT i=0,quote=0;

    while(_istspace(**pCmd))
        (*pCmd)++;

    StringCchCopyN(save,countof(save),*pCmd,MAX_PATH-1);

    if( *pCmd && ((*pCmd)[0] == '\"') ) {
        if(!(tmp=_tcschr((*pCmd)+1,'\"')) ) {
            Debug( TEXT("SH::ParseQuote() missed quote pair \"\n"));
            return FALSE;
        }

        i=1;
        while((*pCmd)[i] != '\0') {
            if((*pCmd)[i] == '\"')
                break;
            filename[i-1] = (*pCmd)[i];
            i++;
        }
        filename[i-1] = '\0';

        (*pCmd) = (*pCmd)+i+2;
    }
    else {
        i=0;
        while((*pCmd)[i] != '\0')  {
            if(_istspace((*pCmd)[i]))
                break;
            filename[i] = (*pCmd)[i];
            i++;
        }
        filename[i] = '\0';

        (*pCmd) = (*pCmd)+i;
    }

    return TRUE;
}

//--------------------------------------------------------
//
//    Function: CShell MultiFile
//
//    Synopsis: handle wildcard
//
//    Arguments:
//
//              pForm - [in] 1st command argument
//              pTo - [in] 2nd command argument
//              method - [in] 1: copy 2: delete 3: attrib
//
//    Returns: TRUE indicates success
//
//--------------------------------------------------------

BOOL CShell::MultiFile(PTCHAR pFrom, DWORD cchFrom, PTCHAR pTo, DWORD cchTo, UINT method)
{
    LPTSTR srcfile=NULL,destfile=NULL,tmp=NULL;
    TCHAR srcroot[MAX_PATH]={0};
    TCHAR destroot[MAX_PATH]={0};
    TCHAR deleteFile[MAX_PATH]={0};
    TCHAR attributeFile[MAX_PATH]={0};
    TCHAR fromFile[MAX_PATH]={0};
    TCHAR toFile[MAX_PATH]={0};
    BOOL    pos=0,fail=0;
    HANDLE    hFind;
    WIN32_FIND_DATA fd;
    HRESULT hrIgnore = S_OK;

    srcfile = _tcsrchr(pFrom,'\\');   // find the name of source

    if(!srcfile)
        srcfile = pFrom;
    else
        srcfile = srcfile+1;

    if(!srcfile)
        return FALSE;

    if(!_tcschr(srcfile,'*')) // source include wildcard?
        return FALSE;

    hrIgnore = StringCchCopyN(srcroot,countof(srcroot),pFrom,MAX_PATH-1);

    if(method < 2)    // it's a copy command
    {
        destfile = _tcsrchr(pTo,'\\');
        if(destfile)
        {
            if(_tcschr(destfile,'*'))   // dest includes wildcard, remove it, rememeber the root
            {
                if(method == 1)
                {
                    pTo[destfile-pTo+1] = '\0';
                    hrIgnore = StringCchCopyN(destroot,countof(destroot),pTo,MAX_PATH-1);
                }
                else
                {
                    if(pTo == destfile)  // it's root
                    {
                        hrIgnore = StringCchCopy(destroot,countof(destroot),TEXT("\\\0"));
                    }
                    else
                    {
                        pTo[destfile-pTo] = '\0';
                        hrIgnore = StringCchCopyN(destroot,countof(destroot),pTo,MAX_PATH-1);
                    }
                }
            }
            else // no wildcard
            {
                hrIgnore = StringCchCopyN(destroot,countof(destroot),pTo,MAX_PATH-1);
            }
        }
        else // it's root
        {
            hrIgnore = StringCchCopy(destroot,countof(destroot),TEXT("\\\0"));
        }
    }

    tmp = _tcsrchr(srcroot,'\\');  // make sure append file name to a directory
    if(tmp)
        tmp[1] = '\0';
    else
    {
        srcroot[0] = '\\';
        srcroot[1] = '\0';
    }

    if( (hFind = FindFirstFile(pFrom, &fd)) == INVALID_HANDLE_VALUE )
    {
        Debug( TEXT("SH: can not find %s - 0x%x\n"), pFrom, GetLastError() );
        return FALSE;
    }
    else
    {
        while(TRUE)
        {
            if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                if(method == 2)   // delete this file
                {
                    memset(deleteFile, 0, sizeof(deleteFile));
                    if(srcroot != NULL)
                        hrIgnore = StringCchCopyN(deleteFile, countof(deleteFile), srcroot,MAX_PATH-1);
                    if(fd.cFileName != NULL)
                        hrIgnore = StringCchCat(deleteFile, countof(deleteFile), fd.cFileName);

                    APCDeleteFile(deleteFile);
                }
                else if (method == 3)  // list the file attribute
                {
                    memset(attributeFile, 0, sizeof(attributeFile));
                    if(srcroot != NULL)
                        hrIgnore = StringCchCopyN(attributeFile, countof(attributeFile), srcroot, MAX_PATH-1);
                    if(fd.cFileName != NULL)
                        hrIgnore = StringCchCat(attributeFile, countof(attributeFile), fd.cFileName);

                    APCFileAttribute(pTo, cchTo, attributeFile, countof(attributeFile), TRUE);
                }
                else  // copy the file
                {
                    memset(fromFile, 0, sizeof(fromFile));
                    if( srcroot != NULL)
                        hrIgnore = StringCchCopyN(fromFile, countof(fromFile), srcroot, MAX_PATH-1);
                    if(fd.cFileName != NULL)
                        hrIgnore = StringCchCat(fromFile, countof(fromFile), fd.cFileName);

                    memset(toFile, 0, sizeof(toFile));
                    hrIgnore = StringCchCopyN(toFile, countof(toFile), destroot, MAX_PATH-1);
                    // if dest ends up with a backslash, we assume it's a directory
                    if( _tcslen(toFile) && toFile[_tcslen(toFile)-1] == '\\' && fd.cFileName )
                        hrIgnore = StringCchCat(toFile, countof(toFile), fd.cFileName);

                    QALibCopyFile(fromFile, toFile);
                }
            }

            if( !FindNextFile(hFind, &fd))
                break;
        }

        if( hFind != INVALID_HANDLE_VALUE )
        {
            FindClose( hFind);
        }
    }

    return TRUE;
}

//--------------------------------------------------------
//
//    Function: CShell QALibCopyFile
//
//
//    Arguments:
//
//              pszForm - [in] source file name
//              pszTo - [in] dest file name
//
//    Returns: TRUE indicates success
//
//--------------------------------------------------------

BOOL CShell::QALibCopyFile(PTCHAR pszFrom, PTCHAR pszTo)
{
	DWORD dwTime=GetTickCount();

	if(!pszFrom || !pszTo)
	    return FALSE;

	if( !CopyFile( pszFrom, pszTo, FALSE) )
	{
		Debug( TEXT("SH: copy %s to %s failed - 0x%x\n"), pszFrom, pszTo, GetLastError() );
		return FALSE;
	}
	else
	{
		Debug( TEXT("SH: copy %s to %s (%ld ms)\n"), pszFrom, pszTo, GetTickCount()-dwTime);
		return TRUE;
	}
}

void CShell::APCTypeFile(LPTSTR pFile)
{
	// create the file
	HANDLE hFile = INVALID_HANDLE_VALUE;
    BYTE bBuf[BUFFER_SIZE] = {0}; // buf for tmp reading
	BOOL bUnicode = FALSE;
	DWORD dwRead = 0;

	hFile = CreateFile( pFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == hFile )
	{
		Debug( TEXT("SH: create file %s failed - 0x%x\n"), pFile, GetLastError() );
		return;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if((DWORD)-1 == dwFileSize)
	{
		Debug( TEXT("SH: GetFileSize %s failed - 0x%x\n"), pFile, GetLastError() );
		goto Exit;
	}

	if(!ReadFile(hFile, bBuf, 2, &dwRead, NULL))
	{
		Debug( TEXT("SH: Error reading %s (retrieved %d bytes) failed - 0x%x\n"), pFile, dwRead, GetLastError() );
		goto Exit;
	}

    if( (*bBuf == 0xFF) && (*(bBuf+1) == 0xFE) )
        bUnicode = TRUE;

    if(!bUnicode)
        RETAILMSG(1, (L"%S", bBuf));

	if(dwFileSize > 2)
	{
	    if( (*bBuf == 0xFF) && (*(bBuf+1) == 0xFE) )
	        bUnicode = TRUE;
	}

    while(1)
    {
        memset(bBuf, 0, sizeof(bBuf));
		if(!ReadFile(hFile, bBuf, BUFFER_SIZE-1, &dwRead, NULL))
		{
			Debug( TEXT("SH: Error reading %s (retrieved %d bytes) failed - 0x%x\n"), pFile, dwRead, GetLastError() );
			goto Exit;
		}

		if(bUnicode)
			RETAILMSG(1, (L"%s", bBuf));
		else
			RETAILMSG(1, (L"%S", bBuf));

        // done...
        if(dwRead < BUFFER_SIZE-1)
            break;
    }


Exit:
	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
}


//--------------------------------------------------------
//
//    Function: CShell APCFileCreate
//
//    Synopsis: create an empty file
//
//    Arguments:
//
//              pFile - [in] file name
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::APCFileCreate(LPTSTR pFile)
{
    HANDLE hFile;

    hFile = CreateFile( pFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

    if( INVALID_HANDLE_VALUE == hFile )
        Debug( TEXT("SH: create file %s failed - 0x%x\n"), pFile, GetLastError() );
    else
    {
        Debug( TEXT("SH: create file %s \n"), pFile);
        CloseHandle( hFile );
    }
    return;
}

BOOL CShell::IsDirectory(PTCHAR pFile)
{
    HANDLE    hFind;
    WIN32_FIND_DATA fd;

    hFind = FindFirstFile(pFile, &fd);
    if( hFind == INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }

    if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        FindClose( hFind);
        return FALSE;
    }

    FindClose( hFind);
    return TRUE;
}

//--------------------------------------------------------
//
//    Function: CShell APCCreateDirectory
//
//    Synopsis: create a directory
//
//    Arguments:
//
//              pDir - [in] directory name
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::APCCreateDirectory(LPTSTR pDir)
{
    if( !CreateDirectory(pDir,NULL) )
        Debug( TEXT("SH: create dir %s failed - 0x%x \n"), pDir,GetLastError() );
    else
        Debug( TEXT("SH: create dir %s \n"), pDir);

    return;
}

//--------------------------------------------------------
//
//    Function: CShell APCRemoveDirectory
//
//    Synopsis: remove a directory
//
//    Arguments:
//
//              pDir - [in] directory name
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::APCRemoveDirectory(LPTSTR pDir)
{
    if( !RemoveDirectory(pDir) )
        Debug( TEXT("SH: remove dir %s failed - 0x%x \n"), pDir,GetLastError() );
    else
        Debug( TEXT("SH: remove dir %s \n"),pDir);

    return;
}

//--------------------------------------------------------
//
//    Function: CShell APCDeleteFile
//
//    Synopsis: delete a given file
//
//    Arguments:
//
//              pFile - [in] File name
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::APCDeleteFile(LPTSTR pFile)
{
    if( !DeleteFile(pFile) )
    {
        Debug( TEXT("SH: delete file %s failed - 0x%x\n"), pFile, GetLastError() );
        return;
    }

    Debug( TEXT("SH: delete file %s \n"),pFile);

    return;
}

//--------------------------------------------------------
//
//    Function: CShell APCListFiles
//
//    Synopsis: Displays a list of files in a given directory
//
//    Arguments:
//
//              pRoot - [in] root directory name
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::APCListFiles(LPTSTR pRoot, DWORD cchRoot)
{
    WIN32_FIND_DATA fd;
    HANDLE hFind;
    TCHAR *p=pRoot;
    TCHAR root[MAX_PATH];
    UINT filenum=0,filesize=0;
    BOOL multiFile=FALSE;
    TCHAR *tmp;
    HRESULT hrIgnore = S_OK;

    memset( root, 0, sizeof(root) );

    if( pRoot[1] != '\0' || (pRoot[0] == '*' && pRoot[1] == '\0') ) // special root case
    {
        tmp = _tcsrchr(p,'\\');   // find the last '\'
        if(tmp == NULL) {
            tmp = _tcschr(p,'*');
            if(tmp != NULL)
                multiFile = TRUE;
        }
        else if(tmp != NULL && tmp[1] == '\0')  // it's a directory ending
        {
            hrIgnore = StringCchCat(pRoot,cchRoot,L"*");
            multiFile = 1;
        }
        else if(tmp != NULL && tmp[1] != '\0')   //not a directory ending
        {
            tmp = tmp+1;
            tmp = _tcschr(tmp,'*');
            if(tmp != NULL)
                multiFile = 1;
        }

        if( (hFind = FindFirstFile(pRoot, &fd)) == INVALID_HANDLE_VALUE )
        {
            Debug( TEXT("SH: File %s is not found - 0x%x\n"), pRoot, GetLastError() );
            return ;
        }

        if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !multiFile)   // it's a file
        {
            APCPrintFileInfo(fd);

            filenum++;
            filesize = filesize+fd.nFileSizeLow;

            tmp = _tcsrchr(pRoot, '\\');
            if(!tmp)
            {
                hrIgnore = StringCchCopy(root, countof(root), L"\\");
            }
            else
            {
                if(tmp != pRoot)
                {
                    hrIgnore = StringCchCopyN(root, countof(root), pRoot, wcslen(pRoot)-wcslen(tmp) );
                }
                else
                {
                    hrIgnore = StringCchCopy(root, countof(root), L"\\");
                }
            }

            goto LReturn;

        }
        else if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  // it's a directory, list later
        {
            if( hFind != INVALID_HANDLE_VALUE )
            {
                FindClose( hFind);
            }
        }
    }

    // construct the wildcard
    if(!multiFile)
    {
        hrIgnore = StringCchCopyN(root, countof(root), pRoot, MAX_PATH-1);
        p = pRoot+_tcslen(pRoot)-1;

        if(p[0] == '\\')
            hrIgnore = StringCchCat(pRoot,cchRoot,TEXT("*.*\0"));
        else
            hrIgnore = StringCchCat(pRoot,cchRoot,TEXT("\\*.*\0"));
    }
    else
    {
        tmp = _tcsrchr(pRoot, '\\');
        if(!tmp)
        {
            hrIgnore = StringCchCopy(root, countof(root), L"\\");
        }
        else
        {
            if(tmp != pRoot)
                hrIgnore = StringCchCopyN(root, countof(root), pRoot, wcslen(pRoot)-wcslen(tmp) );
            else
                hrIgnore = StringCchCopy(root, countof(root), L"\\");
        }
    }

    if( (hFind = FindFirstFile(pRoot, &fd)) != INVALID_HANDLE_VALUE )
    {
        while( TRUE)
        {
            filenum++;
            filesize = filesize+fd.nFileSizeLow;

            APCPrintFileInfo(fd);

            if( !FindNextFile( hFind, &fd))
                break;
        }
    }

LReturn:

    // print out the summarize

    ULARGE_INTEGER FreeBytes, TotalBytes, TotalFreeBytes;

    Debug( TEXT("SH: \n"));
    if(!GetDiskFreeSpaceEx(root, &FreeBytes, &TotalBytes, &TotalFreeBytes))
        Debug( TEXT("SH: GetDiskFreeSpaceEx() failed -- 0x%x\n"),GetLastError() );
    else {
        DWORD r1, r2, r3;

        APCPrintComma(filesize, &r1, &r2, &r3);

        if(r1)
            Debug( TEXT("SH: \t\t%ld File(s)\t %ld,%03ld,%03ld bytes \n"), filenum, r1, r2, r3);
        else if(r2)
            Debug( TEXT("SH: \t\t%ld File(s)\t %ld,%03ld bytes \n"), filenum, r2, r3);
        else
            Debug( TEXT("SH: \t\t%ld File(s)\t %ld bytes \n"), filenum, r3);

	
		TCHAR tszFreeBytes[64]; 
		tszFreeBytes[0] = 0;
		WORD  dwLen = sizeof(tszFreeBytes) / sizeof(tszFreeBytes[0]);

		for ( ULONGLONG ull = TotalFreeBytes.QuadPart; ull > 0; )
		{
			
			DWORD dwr1 = (DWORD) (ull % 1000);
			ull = ull / 1000;			
			if (tszFreeBytes[0])
			{
				TCHAR tszFreeBytes1[64]; 			
				tszFreeBytes1[0] = 0;
				hrIgnore = StringCchPrintf(tszFreeBytes1, countof(tszFreeBytes),  ull >0 ? TEXT("%03ld,%s"):TEXT("%ld,%s"), dwr1, tszFreeBytes );
				hrIgnore = StringCchCopyN(tszFreeBytes, countof(tszFreeBytes), tszFreeBytes1, dwLen);	
			}
			else
			{
				hrIgnore = StringCchPrintf(tszFreeBytes, countof(tszFreeBytes), ull >0 ? TEXT("%03ld") : TEXT("%ld"), dwr1);	
			}									
		}

		Debug( TEXT("SH: \t\t\t%s bytes free\n"), tszFreeBytes);
	}

    FindClose( hFind);

    return;
}

//--------------------------------------------------------
//
//    Function: CShell APCPrintFileInfo
//
//    Synopsis: print file info for a given file,
//              e.g. time, attrib, name and size
//
//    Arguments:
//
//              fd - [in] structure describes the given file
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::APCPrintFileInfo(WIN32_FIND_DATA fd)
{
    SYSTEMTIME    *pSystemTime=NULL;
    FILETIME    *plocalfiletime=NULL;

    pSystemTime = (SYSTEMTIME *) malloc(sizeof(SYSTEMTIME));
    plocalfiletime = (FILETIME *) malloc(sizeof(FILETIME));

    if(!pSystemTime || !plocalfiletime)
        goto LReturn;

    if(!FileTimeToLocalFileTime(&fd.ftLastWriteTime,plocalfiletime))
    {
        Debug( TEXT("\nSH: Get local file time failed - 0x%x \n"), GetLastError() );
        goto LReturn;
    }

    if(!FileTimeToSystemTime(plocalfiletime,pSystemTime))
    {
        Debug( TEXT("\nSH: convert to system time failed - 0x%x \n"), GetLastError() );
        goto LReturn;
    }

//    if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
    if( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
    {
        if(fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
            Debug( TEXT("Shell:: %.2ld/%.2ld/%.2ld %.2ld:%.2ld\t<-r->%15ld   %s\n"), pSystemTime->wMonth, pSystemTime->wDay,pSystemTime->wYear, pSystemTime->wHour, pSystemTime->wMinute, fd.nFileSizeLow, fd.cFileName);
        else
            Debug( TEXT("Shell:: %.2ld/%.2ld/%.2ld %.2ld:%.2ld\t<--->%15ld   %s\n"), pSystemTime->wMonth, pSystemTime->wDay,pSystemTime->wYear, pSystemTime->wHour, pSystemTime->wMinute, fd.nFileSizeLow, fd.cFileName);
    }
    else if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
    {
        if(fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
            Debug( TEXT("Shell:: %.2ld/%.2ld/%.2ld %.2ld:%.2ld\t<Drr>                  %s\n"), pSystemTime->wMonth, pSystemTime->wDay,pSystemTime->wYear, pSystemTime->wHour, pSystemTime->wMinute,fd.cFileName);
        else
            Debug( TEXT("Shell:: %.2ld/%.2ld/%.2ld %.2ld:%.2ld\t<Dir>                  %s\n"), pSystemTime->wMonth, pSystemTime->wDay,pSystemTime->wYear, pSystemTime->wHour, pSystemTime->wMinute,fd.cFileName);
    }

LReturn:

    if(pSystemTime)
        free(pSystemTime);
    if(plocalfiletime)
        free(plocalfiletime);

    return;
}

//--------------------------------------------------------
//
//    Function: CShell APCPrintComma
//
//    Synopsis: format the size of a file using M/K/Btytes
//
//    Arguments:
//
//              size - [in] the size of the file
//              *r1 - [out] at MBytes
//              *r2 - [out] at KBytes
//              *r3 - [out] at Bytes
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::APCPrintComma(DWORD size, DWORD *r1, DWORD *r2, DWORD *r3)
{
    DWORD m1, m2, m3;

    *r3 = size%1000;
    m1 = size/1000;
    *r2 = m1%1000;
    m2 = m1/1000;
    *r1 = m2%1000;
    m3 = m2/1000;
}




//--------------------------------------------------------
//
//    Function: CShell APCDeleteTree
//
//    Synopsis: Delete a list of files and subdirectories in a directory
//
//    Arguments:
//
//              pPath - [in] current directory name
//
//    Returns: delete status
//
//--------------------------------------------------------

BOOL CShell::APCDeleteTree(LPTSTR  pPath, DWORD cchPath)
{
    BOOL ReturnCode=TRUE;
    WIN32_FIND_DATA SearchData;
    HANDLE SearchHandle=INVALID_HANDLE_VALUE;
    WCHAR pathAnchor[MAX_PATH]={0};
    WCHAR fullPath[MAX_PATH]={0};
    WCHAR saveDir[MAX_PATH]={0};
    BOOL MoreToDo=TRUE;
    HRESULT hrIgnore = S_OK;

    if  ( (NULL == pPath) || !_tcslen(pPath))
    {
        ReturnCode  = FALSE;
		return ReturnCode;
	}

    hrIgnore = StringCchCopyN( pathAnchor, countof(pathAnchor), pPath, MAX_PATH-1);

    if(pPath[_tcslen(pPath)-1] != '\\')
    {
        hrIgnore = StringCchCat( pPath, cchPath, TEXT("\\*") );
        hrIgnore = StringCchCat( pathAnchor, countof(pathAnchor), TEXT("\\") );
    }
    else
    {
        hrIgnore = StringCchCat( pPath, cchPath, TEXT("*") );
    }

    SearchHandle = FindFirstFile( pPath, &SearchData );

    if( SearchHandle == INVALID_HANDLE_VALUE )
    {
        MoreToDo    = FALSE;
        if( GetLastError() != ERROR_FILE_NOT_FOUND )
            ReturnCode  = FALSE;
    }

//    while( ReturnCode && MoreToDo ) {
    while( MoreToDo )
    {
        memset( fullPath, 0, sizeof(fullPath) );
        memset( saveDir, 0, sizeof(saveDir) );
        hrIgnore = StringCchCopyN( fullPath, countof(fullPath), pathAnchor, MAX_PATH-1 );

        if(SearchData.cFileName != NULL)
            hrIgnore = StringCchCat( fullPath, countof(fullPath), SearchData.cFileName );
        if( SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            hrIgnore = StringCchCopyN( saveDir, countof(saveDir), fullPath, MAX_PATH-1 );
            ReturnCode  &= APCDeleteTree(fullPath,countof(fullPath));
            ReturnCode  &= RemoveDirectory(saveDir);
            Debug( TEXT("SH: delete directory %s\n"), saveDir);
        }
        else {
            ReturnCode  &= SetFileAttributes( fullPath, FILE_ATTRIBUTE_NORMAL );
            ReturnCode  &= DeleteFile( fullPath );
            if(ReturnCode)
                Debug( TEXT("SH: delete file %s\n"), fullPath);
            else
                Debug( TEXT("SH: delete file %s failed - 0x%x\n"), fullPath, GetLastError() );
        }
        MoreToDo  = FindNextFile( SearchHandle, &SearchData );
    }

    if( SearchHandle != INVALID_HANDLE_VALUE )
    {
        FindClose( SearchHandle );
    }

    return ReturnCode;
}

//--------------------------------------------------------
//
//    Function: CShell APCFileAttribute
//
//    Synopsis: Displays or changes a given file attributes
//
//    Arguments:
//
//              pAttrib - [in] file attrib to be changed
//              pFile - [in] file name
//              bNoSwap - [in] true on list, false on change
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::APCFileAttribute(PTCHAR pAttrib, DWORD cchAttrib, PTCHAR pFile, DWORD cchFile, BOOL bNoSwap)
{
    DWORD ret;
    TCHAR tmp[MAX_PATH]={0};
    HRESULT hrIgnore = S_OK;

    // swap parameter order if necessary
    if( !bNoSwap && ( (pFile[0] == '-' || pFile[0] == '+') && (pAttrib[0] != '-' && pAttrib[0] != '+') )
       || pFile[0] == '\0' )
    {
        hrIgnore = StringCchCopyN(tmp, countof(tmp), pFile, MAX_PATH-1);
        hrIgnore = StringCchCopyN(pFile, cchFile, pAttrib, MAX_PATH-1);
        hrIgnore = StringCchCopyN(pAttrib, cchAttrib, tmp, MAX_PATH-1);
    }

    ret = GetFileAttributes(pFile);
    if(ret ==  0xFFFFFFFF)
    {
        Debug( TEXT("SH: GetFileAttributes(%s) failed - 0x%x \n"), pFile, GetLastError() );
        return;
    }

    if(pAttrib[0] == '-' || pAttrib[0] == '+')
    {
        if( pAttrib[1] == 'r' || pAttrib[1] == 'R' || pAttrib[1] == 'H' || pAttrib[1] == 'h' ||  pAttrib[1] == 's' || pAttrib[1] == 'S' )
        {
            if( pAttrib[1] == 'r' || pAttrib[1] == 'R' )
            {
                if(pAttrib[0] == '-')
                {
                    if(!SetFileAttributes(pFile, ret & ~(FILE_ATTRIBUTE_READONLY)))
                    {
                        Debug( TEXT("SH: SetFileAttributes(%s) failed - 0x%x\n"),pFile, GetLastError() );
                        return;
                    }
                }
                else
                {
                    if(!SetFileAttributes(pFile, ret | FILE_ATTRIBUTE_READONLY))
                    {
                        Debug( TEXT("SH: SetFileAttributes(%s) failed - 0x%x\n"),pFile, GetLastError() );
                        return;
                    }
                }
            }
            else if ( pAttrib[1] == 'h' || pAttrib[1] == 'H' )
            {
                if(pAttrib[0] == '-')
                {
                    if(!SetFileAttributes(pFile, ret & ~(FILE_ATTRIBUTE_HIDDEN)))
                    {
                        Debug( TEXT("SH: SetFileAttributes(%s) failed - 0x%x\n"),pFile, GetLastError() );
                        return;
                    }
                }
                else {
                    if(!SetFileAttributes(pFile, ret | FILE_ATTRIBUTE_HIDDEN))
                    {
                        Debug( TEXT("SH: SetFileAttributes(%s) failed - 0x%x\n"),pFile, GetLastError() );
                        return;
                    }
                }
            }
            else if ( pAttrib[1] == 's' || pAttrib[1] == 'S' )
            {
                if(pAttrib[0] == '-')
                {
                    if(!SetFileAttributes(pFile, ret & ~(FILE_ATTRIBUTE_SYSTEM)))
                    {
                        Debug( TEXT("SH: SetFileAttributes(%s) failed - 0x%x\n"),pFile, GetLastError() );
                        return;
                    }
                }
                else {
                    if(!SetFileAttributes(pFile, ret | FILE_ATTRIBUTE_SYSTEM))
                    {
                        Debug( TEXT("SH: SetFileAttributes(%s) failed - 0x%x\n"),pFile, GetLastError() );
                        return;
                    }
                }
            }
        }
        else
        {
            Debug( TEXT("SH: Usage: s sh ATTRIB [+R | -R] [+H | -H] [+S | -S] [path] pFile]\n"));
            return;
        }
    }

    ret = GetFileAttributes(pFile);
    if(ret ==  0xFFFFFFFF)
    {
        Debug( TEXT("SH: GetFileAttributes(%s) failed - 0x%x\n"),pFile, GetLastError() );
        return;
    }

    if( (ret & FILE_ATTRIBUTE_ROMMODULE) && (ret & FILE_ATTRIBUTE_HIDDEN) )
        Debug( TEXT("SH: ROMMODULE-H\t\t%s\n"),pFile);
    else if( ret & FILE_ATTRIBUTE_ROMMODULE )
        Debug( TEXT("SH: ROMMODULE\t\t%s\n"),pFile);
    else if( (ret & FILE_ATTRIBUTE_INROM) && (ret & FILE_ATTRIBUTE_HIDDEN) )
        Debug( TEXT("SH: ROM-H\t\t%s\n"),pFile);
    else if( ret & FILE_ATTRIBUTE_INROM )
        Debug( TEXT("SH: ROM\t\t%s\n"),pFile);
    else if( (ret & FILE_ATTRIBUTE_HIDDEN) && (ret & FILE_ATTRIBUTE_READONLY) && (ret & FILE_ATTRIBUTE_SYSTEM) )
        Debug( TEXT("SH: SHR\t\t%s\n"),pFile);
    else if( (ret & FILE_ATTRIBUTE_HIDDEN) && (ret & FILE_ATTRIBUTE_SYSTEM) )
        Debug( TEXT("SH: SH\t\t%s\n"),pFile);
    else if( (ret & FILE_ATTRIBUTE_READONLY) && (ret & FILE_ATTRIBUTE_SYSTEM) )
        Debug( TEXT("SH: SR\t\t%s\n"),pFile);
    else if( (ret & FILE_ATTRIBUTE_HIDDEN) && (ret & FILE_ATTRIBUTE_READONLY) )
        Debug( TEXT("SH: RH\t\t%s\n"),pFile);
    else if( ret & FILE_ATTRIBUTE_SYSTEM )
        Debug( TEXT("SH: S\t\t%s\n"),pFile);
    else if( ret & FILE_ATTRIBUTE_READONLY )
        Debug( TEXT("SH: R\t\t%s\n"),pFile);
    else if( ret & FILE_ATTRIBUTE_HIDDEN )
        Debug( TEXT("SH: H\t\t%s\n"),pFile);
    else
        Debug( TEXT("SH:  \t%s\n"),pFile);

    return;
}

//--------------------------------------------------------
//
//    Function: CShell PrintUsage
//
//    Synopsis: print out usage infomation
//
//    Arguments: none
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::PrintUsage(void)
{
    Debug( TEXT("SHUsage: \n"));
    Debug( TEXT("SHUsage: Windows CE> s sh help\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh example\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh copy srcfile destfile\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh dir pathname\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh attrib [+R | -R] [+H | -H] [+S | -S] [path\\filename]\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh del filename\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh deltree treeRoot\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh type filename\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh md dirname\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh rd dirname\n"));
    Debug( TEXT("SHUsage: Windows CE> s sh create filename\n"));
    Debug( TEXT("SHUsage: \n"));
}

//--------------------------------------------------------
//
//    Function: CShell PrintExample
//
//    Synopsis: print out some examples
//
//    Arguments: none
//
//    Returns: none
//
//--------------------------------------------------------

void CShell::PrintExample(void)
{
    Debug( TEXT("SHExample: \n"));
    Debug( TEXT("SHExample: Windows CE> s sh copy \\release\\uivga.dll \\windows\\ \n"));
    Debug( TEXT("SHExample: Windows CE> s sh copy \"\\storage card\\watcher.log\" \\release\\w1.log \n"));
    Debug( TEXT("SHExample: Windows CE> s sh copy \"\\storage card\\*.log\" \\release\\ \n"));
    Debug( TEXT("SHExample: Windows CE> s sh dir \"\\storage card\\\" \n"));
    Debug( TEXT("SHExample: Windows CE> s sh dir \\windows\\*.exe \n"));
    Debug( TEXT("SHExample: Windows CE> s sh type \\windows\\radioapp.lnk \n"));
    Debug( TEXT("SHExample: Windows CE> s sh type \\release\\about.htm \n"));
    Debug( TEXT("SHExample: Windows CE> s sh attrib \\windows\\*.dll \n"));
    Debug( TEXT("SHExample: Windows CE> s sh attrib +h \\windows\\system.cpl \n"));
    Debug( TEXT("SHExample: Windows CE> s sh create \"\\storage card\\file\" \n"));
    Debug( TEXT("SHExample: Windows CE> s sh del \\windows\\* \n"));
    Debug( TEXT("SHExample: Windows CE> s sh deltree \"\\storage card\\\" \n"));
    Debug( TEXT("SHExample: Windows CE> s sh md \\temp\\dir1 \n"));
    Debug( TEXT("SHExample: Windows CE> s sh rd \\temp\\dir1 \n"));
    Debug( TEXT("SHExample: \n"));
}

