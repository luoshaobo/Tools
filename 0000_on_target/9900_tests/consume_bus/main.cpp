#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <vector>

#define SIZE_1M         (1024 * 1024)

#define PL()                //fprintf(stdout, "%s(): %u: %s\n", __FUNCTION__, __LINE__, __FILE__)
#define P(format,...)       //fprintf(stdout, format, ## __VA_ARGS__)

void usage(int argc, char *argv[])
{
    printf("Usage:\n");
    printf("    %s <dir_path>\n", argv[0]);
}

std::vector<unsigned char> ReadFileContent(const std::string &sFilePath)
{
    int nRet = 0;
    FILE *pFile = NULL;
    std::vector<unsigned char> vFileContent;
    
    if (nRet == 0) {
        pFile = fopen(sFilePath.c_str(), "rb");
        if (pFile == NULL) {
            nRet = -1;
            fprintf(stderr, "*** Error: fopen(sFilePath) is failed: %s\n", sFilePath.c_str());
        }
    }
    
    if (nRet == 0) {
        for (;;) {
            unsigned char buf[1024];
            size_t nReadCount = fread(buf, 1, sizeof(buf), pFile);
            if (nReadCount > 0) {
                assert(nReadCount <= sizeof(buf));
                vFileContent.insert(vFileContent.end(), buf, buf + nReadCount);
            } else {
                if (ferror(pFile)) {
                    nRet = -1;
                    fprintf(stderr, "*** Error: fread(sFilePath) is failed: %s\n", sFilePath.c_str());
                    break;
                } else {
                    break;
                }
            }
            
        }
    }
    
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }
    
    return vFileContent;
}

int ReadOutContentFromDir(const std::string &sDirPath)
{
    int nRet = 0;
    DIR *pDir = NULL;
    struct dirent *pDirent = NULL;
    
    PL();
    if (nRet == 0) {
        pDir = opendir(sDirPath.c_str());
        if (pDir == NULL) {
            nRet = -1;
            fprintf(stderr, "*** Error: opendir(sDirPath) is failed: %s\n", sDirPath.c_str());
        }
    }
    
    PL();
    for (;;) {
        PL();
        errno = 0;
        pDirent = readdir(pDir);
        if (pDirent == NULL)
        {
            PL();
            if (errno != 0) {
                nRet = -1;
                fprintf(stderr, "*** Error: readdir(pDir) is failed: %s\n", strerror(errno));
                break;
            } else {
                break;
            }
        } else {
            PL();
            std::string sChildPath = sDirPath + "/" + std::string(pDirent->d_name);
            P("--- sChildPath=%s\n", sChildPath.c_str());
            
            if (std::string(pDirent->d_name) == "." || std::string(pDirent->d_name) == "..") {
                continue;
            }
            
            if (pDirent->d_type == DT_DIR) {
                ReadOutContentFromDir(sChildPath);
            } else if (pDirent->d_type == DT_REG) {
                fprintf(stdout, "### Info: read content: %s\n", sChildPath.c_str());
                ReadFileContent(sChildPath);
            } else {
                // do nothing
            }
        }
    }
    
    PL();
    if (pDir != NULL) {
        closedir(pDir);
        pDir = NULL;
    }
    
    return nRet;
}

int main (int argc, char *argv[])
{
    std::string sRootDirPath = ".";

    if (argc >= 2) {
        sRootDirPath = argv[1];
    } else {
        usage(argc, argv);
        exit(0);
    }
    
    printf("### INFO: to read out all of the files from \"%s\" repeatedly.\n", sRootDirPath.c_str());

    for (;;) {
        ReadOutContentFromDir(sRootDirPath);
    }
    
    return 0;
}

