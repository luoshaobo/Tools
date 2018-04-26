// TestXML001.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace TK_Tools;

int usage(int argc, TCHAR* argv[])
{
    printf("Usage:\n");
    printf("  %s [-K|-M|-G] <mem_count>\n", wstr2str(argv[0]).c_str());
    printf("\n");
    
    return 0;
}

unsigned char *do_allocation(unsigned int nMemCount)
{
    unsigned char *pMem = NULL;
    
    pMem = (unsigned char *)malloc(nMemCount);
    if (pMem != NULL) {
        memset(pMem, 0, nMemCount);
    }
    
    return pMem;
}

int do_free(unsigned char *pMem)
{
    int nRet = 0;
    
    if (pMem != NULL) {
        free(pMem);
    }
    
    return nRet;
}

int wmain(int argc, TCHAR* argv[])
{
	int nRet = 0;
    std::string sArg1;
    std::string sArg2;
    unsigned char *pMem = NULL;
    unsigned int nUnit = 1;
    unsigned int nMemCount = 0;
    
    if (argc < 2) {
        usage(argc, argv);
        return 0;
    }
    
    if (argc >= 2) {
        sArg1 = wstr2str(argv[1]);
    }
    
    if (argc >= 3) {
        sArg2 = wstr2str(argv[2]);
    }
    
    if (sArg1 == "-H" || sArg1 == "-h" || sArg1 == "--help") {
        usage(argc, argv);
        return 0;
    } else if (sArg1 == "-K") {
        nUnit = 1024;
        nMemCount = StrToUL(sArg2) * nUnit;
    } else if (sArg1 == "-M") {
        nUnit = 1024 * 1024;
        nMemCount = StrToUL(sArg2) * nUnit;
    } else if (sArg1 == "-G") {
        nUnit = 1024 * 1024 * 1024;
        nMemCount = StrToUL(sArg2) * nUnit;
    } else {
        nUnit = 1;
        nMemCount = StrToUL(sArg1) * nUnit;
    }
    
    pMem = do_allocation(nMemCount);
    if (pMem != 0) {
        printf("INFO: the memory with the size equal to %d is allocated successfully.\n", nMemCount);
        printf("Enter any key to exit...");
        getchar();
        do_free(pMem);
        pMem = NULL;
    } else {
        printf("*ERROR: failed to allocate the memory with the size equal to %d.\n", nMemCount);
    }
    
	return nRet;
}
