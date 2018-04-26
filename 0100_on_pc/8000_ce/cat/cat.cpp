#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <wchar.h>
#include <locale.h>
#include <windows.h>
#include "stdafx.h"
#include "TK_Tools.h"
#include "TK_RegKey.h"

#define LINE_BUF_SIZE                   1024

using namespace TK_Tools;

void usage(int argc, char **argv)
{
    printf("usage:\n");
    printf("  %s <file_path> [<heard_size_to_show>]\n", argv[0]);
    printf("\n");
}

int main (int argc, char **argv)
{
    int nRet = 0;
    std::string sFilePath;
    unsigned int nSizeToShow = (unsigned int)-1;
    char arrBuf[LINE_BUF_SIZE + 1] = { 0 };
    FILE *pFile = NULL;
    size_t nRead;
    size_t nReadTotal = 0;
    
    if (nRet == 0) {
        if (argc < 2) {
            usage(argc, argv);
            nRet = -1;
        } else {
            sFilePath = argv[1];
        }
    }
    
    if (nRet == 0) {
        if (argc >= 3) {
            nSizeToShow = StrToUL(argv[2]);
        }
    }
    
    if (nRet == 0) {
        pFile = fopen(sFilePath.c_str(), "rb");
        if (pFile == NULL) {
            fprintf(stderr, "*** Error: failed to open the file \"%s\"!\n", sFilePath.c_str());
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        while (true) {
            nRead = fread(arrBuf, 1, LINE_BUF_SIZE, pFile);
            if (nRead < LINE_BUF_SIZE) {
                if (!feof(pFile)) {
                    fprintf(stderr, "*** Error: failed to read the file \"%s\"!\n", sFilePath.c_str());
                    nRet = -1;
                }
                
                if (nRead > 0) {
                    arrBuf[nRead] = '\0';
                    fprintf(stdout, arrBuf);
                }
                break;
            }
            if (nRead > 0) {
                arrBuf[nRead] = '\0';
                fprintf(stdout, arrBuf);
            }
            nReadTotal += nRead;
            if (nReadTotal >= nSizeToShow) {
                break;
            }
        }
    }
    
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }
    
    fprintf(stdout, "\n");
    
    return nRet;
}

#if defined(WINCE)
int wmain (int argc, wchar_t **_argv)
{
    int ret = -1;
    int i;
    char **_argv_2 = new char *[argc + 1];
    
    for (i = 0; i < argc; i++) {
        _argv_2[i] = new char[wstr2str(_argv[i]).length() + 1];
        strncpy(_argv_2[i], wstr2str(_argv[i]).c_str(), wstr2str(_argv[i]).length());
        _argv_2[i][wstr2str(_argv[i]).length()] = L'\0';
    }
    _argv_2[i] = NULL;
    
    ret = main(argc, _argv_2);
    
    for (i = 0; i < argc; i++) {
        delete [] _argv_2[i];
    }
    delete [] _argv_2;
    
    return ret;
}
#endif // #if defined(WINCE)
