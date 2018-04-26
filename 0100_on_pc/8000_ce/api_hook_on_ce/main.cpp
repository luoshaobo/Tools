// TestXML001.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

int FFF(int n)
{
    return n + 1;
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

    int (*pfFFF)(int n) = &FFF;

    goto XXX;

    nRetCode = 100;
    nRetCode++;

XXX:
    
    printf("[TestNavi] hehe\n");

	return nRetCode;
}
