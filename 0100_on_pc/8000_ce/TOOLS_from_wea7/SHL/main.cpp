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

#include "shell.h"

//-------------------------------------------------------------------------
//
//      Function: WinMain
//
//      Synopsis: Main Win32 entry point
//
//      Arguments: Standard Win32 WinMain arguments
//
//      Returns:  TRUE on app exit
//
//---------------------------------------------------------------------------

int APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR pszCmd, int nCmdShow )
{

    HRESULT hr=NOERROR;

    CShell*    csh=NULL;

    csh = new CShell;
    if(!csh)
        return E_FAIL;


        // create a log object for debug output
    hr = csh->Init();
    if(FAILED (hr))
        goto LReturn;

    // parse command and excute it
    csh->ParseCommandLine(pszCmd);

LReturn:
    // done, clean up
    if(csh)
        delete csh;
     
      return TRUE; 
    
}
