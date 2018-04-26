// tree_copy.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "TK_Tools.h"

#define LOG_INF(format,...)           fprintf(stdout, format, __VA_ARGS__)
#define LOG_ERR(format,...)           fprintf(stderr, format, __VA_ARGS__)

using namespace TK_Tools;

int CompareNoCaseT(const std::tstring &sText1, const std::tstring &sText2)
{
    return CompareNoCase(tstr2str(sText1), tstr2str(sText1));
}

void Usage(int argc, _TCHAR* argv[])
{
    LOG_INF("Usage:\n");
    LOG_INF("    %s <src_dir> <dst_dir>\n", tstr2str(argv[0]).c_str());
    LOG_INF("\n");
}

#ifndef UNDER_CE
#define FILE_ATTRIBUTE_INROM                    0
#define FILE_ATTRIBUTE_ROMMODULE                0
#define FILE_ATTRIBUTE_ROMSTATICREF             0
#endif // #ifndef UNDER_CE

std::string MakeFileAttribStr(DWORD dwFileAtrrib)
{
    int nRet = 0;
    std::string sFileAtrribStr;

    if (nRet == 0) {
        if (dwFileAtrrib == 0xFFFFFFFF) {
            nRet = -1;
        }
    }

    if (nRet == 0) {
        sFileAtrribStr = FormatStr("%s%s%s%s%s%s%s%s:%s:%s:%s",
            (dwFileAtrrib & FILE_ATTRIBUTE_ARCHIVE) ? "A" : "-",
            (dwFileAtrrib & FILE_ATTRIBUTE_COMPRESSED) ? "C" : "-",
            (dwFileAtrrib & FILE_ATTRIBUTE_DIRECTORY) ? "D" : "-",
            (dwFileAtrrib & FILE_ATTRIBUTE_ENCRYPTED) ? "E" : "-",
            (dwFileAtrrib & FILE_ATTRIBUTE_HIDDEN) ? "H" : "-",
            (dwFileAtrrib & FILE_ATTRIBUTE_READONLY) ? "R" : "-",
            (dwFileAtrrib & FILE_ATTRIBUTE_SYSTEM) ? "S" : "-",
            (dwFileAtrrib & FILE_ATTRIBUTE_TEMPORARY) ? "T" : "-",

            (dwFileAtrrib & FILE_ATTRIBUTE_INROM) ? "ROM" : "---",
            (dwFileAtrrib & FILE_ATTRIBUTE_ROMMODULE) ? "ROMM" : "----",
            (dwFileAtrrib & FILE_ATTRIBUTE_ROMSTATICREF) ? "ROMR" : "----"
        );
    }

    return sFileAtrribStr;
}

std::tstring GetBaseName(const std::tstring &sPath)
{
    std::tstring sBaseName;
    std::tstring::size_type nPos;

    nPos = sPath.find_last_of(str2tstr("\\"));
    if (nPos == std::tstring::npos) {
        sBaseName = sPath;
    } else {
        sBaseName = sPath.substr(nPos + 1);
    }

    return sBaseName;
}

std::tstring GetDirPath(const std::tstring &sPath)
{
    std::tstring sDirName;
    std::tstring::size_type nPos;

    nPos = sPath.find_last_of(str2tstr("\\"));
    if (nPos == std::tstring::npos) {
        sDirName = str2tstr("");
    } else {
        sDirName = sPath.substr(0, nPos);
    }

    return sDirName;
}

std::tstring MakePath(const std::tstring &sDirPath, const std::tstring &sBaseName)
{
    std::tstring sFullPath;
    
    if (sDirPath == str2tstr("")) {
        sFullPath = sBaseName;
    } else if (sBaseName == str2tstr("")) {
        sFullPath = sDirPath;
    } else {
        if (sDirPath.length() >= 2 && sDirPath[sDirPath.length() - 1] == str2tstr("\\")[0]) {
            sFullPath = sDirPath + sBaseName;
        } else {
            sFullPath = sDirPath + str2tstr("\\") + sBaseName;
        }
    }

    return sFullPath;
}

bool IsFileOrDir(const std::tstring &sPath)
{
    int nRet = 0;
    bool bIsFileOrDir = false;
    DWORD dwFileAttrib = 0xFFFFFFFF;
    std::tstring sPathTmp = sPath;

    if (sPath.length() >= 2 && sPath[sPath.length() - 1] == str2tstr("\\")[0]) {
        sPathTmp = sPath.substr(0, sPath.length() -1);
    }
    
    dwFileAttrib = ::GetFileAttributes(sPathTmp.c_str());
    if (dwFileAttrib != 0xFFFFFFFF) {
        bIsFileOrDir = true;
    }

    return bIsFileOrDir;
}

bool IsDir(const std::tstring &sPath)
{
    int nRet = 0;
    bool bIsDir = false;
    DWORD dwFileAttrib = 0xFFFFFFFF;
    std::tstring sPathTmp = sPath;

    if (sPath.length() >= 2 && sPath[sPath.length() - 1] == str2tstr("\\")[0]) {
        sPathTmp = sPath.substr(0, sPath.length() -1);
    }
    
    dwFileAttrib = ::GetFileAttributes(sPathTmp.c_str());
    if (dwFileAttrib != 0xFFFFFFFF) {
        if ((dwFileAttrib & (FILE_ATTRIBUTE_DIRECTORY)) != 0) {
            bIsDir = true;
        }
    }

    return bIsDir;
}

bool IsRegularFile(const std::tstring &sPath)
{
    int nRet = 0;
    bool bIsRegularFile = false;
    DWORD dwFileAttrib = 0xFFFFFFFF;
    std::tstring sPathTmp = sPath;

    if (sPath.length() >= 2 && sPath[sPath.length() - 1] == str2tstr("\\")[0]) {
        sPathTmp = sPath.substr(0, sPath.length() -1);
    }
    
    dwFileAttrib = ::GetFileAttributes(sPathTmp.c_str());
    if (dwFileAttrib != 0xFFFFFFFF) {
        if ((dwFileAttrib & (FILE_ATTRIBUTE_DIRECTORY)) == 0) {
            bIsRegularFile = true;
        }
    }

    return bIsRegularFile;
}

int CreateDir(const std::tstring &sPath)
{
    int nRet = 0;
    BOOL bSuc;
    std::tstring sPathTmp = sPath;

    if (sPath.length() >= 2 && sPath[sPath.length() - 1] == str2tstr("\\")[0]) {
        sPathTmp = sPath.substr(0, sPath.length() -1);
    }

    if (nRet == 0) {
        if (!IsDir(sPathTmp)) {
            bSuc = ::CreateDirectory(sPathTmp.c_str(), NULL);
            if (!bSuc) {
                nRet = -1;
                LOG_ERR("*** ERROR: [%s():%lu][errno=%lu] failed to create the direcotry \"%s\"!\n", 
                    __FUNCTION__, 
                    __LINE__, 
                    ::GetLastError(),
                    tstr2str(sPathTmp).c_str()
                );
            }
        }
    }

    return nRet;
}

int CopyFile(const std::tstring &sSrcFilePath, const std::tstring &sDstFilePath)
{
    int nRet = 0;
    BOOL bSuc;
    DWORD dwFileAttrib = 0xFFFFFFFF;
    DWORD dwFileAttribToClear = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;
    bool bFileAttribChanged = false;

    dwFileAttrib = ::GetFileAttributes(sSrcFilePath.c_str());
    if (dwFileAttrib != 0xFFFFFFFF) {
        if (dwFileAttrib & dwFileAttribToClear) {
#ifndef UNDER_CE
            if (::SetFileAttributes(sSrcFilePath.c_str(), 0)) {
                bFileAttribChanged = true;
            }
#endif // #ifndef UNDER_CE
        }
    }

    if (nRet == 0) {
        bSuc = ::CopyFile(sSrcFilePath.c_str(), sDstFilePath.c_str(), FALSE);
        if (!bSuc) {
            nRet = -1;
            LOG_ERR("*** ERROR: [%s():%lu][errno=%lu][%s][%s] failed to copy the file \"%s\" to the file \"%s\"!\n", 
                __FUNCTION__, 
                __LINE__, 
                ::GetLastError(),
                MakeFileAttribStr(dwFileAttrib).c_str(),
                MakeFileAttribStr(::GetFileAttributes(sSrcFilePath.c_str())).c_str(),
                tstr2str(sSrcFilePath).c_str(), 
                tstr2str(sDstFilePath).c_str()
            );
        }
    }

    if (dwFileAttrib != 0xFFFFFFFF) {
        if (bFileAttribChanged) {
            ::SetFileAttributes(sSrcFilePath.c_str(), dwFileAttrib);
        }
    }

    return nRet;
}

int CopyTree(const std::tstring &sSrcDirPath, const std::tstring &sDstDirPath)
{
    int nRet = 0;
    int nRetTmp;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA oFindFileData;
    std::tstring sFindPattern = MakePath(sSrcDirPath, str2tstr("*"));

    if (nRet == 0) {
        if (IsDir(sSrcDirPath)) {
            if (!IsDir(sDstDirPath)) {
                if (!CreateDir(sDstDirPath)) {
                    nRet = -1;
                    LOG_ERR("*** ERROR: [%s():%lu] failed to create the directory \"%s\"!\n", 
                        __FUNCTION__, 
                        __LINE__, 
                        tstr2str(sDstDirPath).c_str()
                    );
                }
            }
        } else {
            nRet = -1;
            LOG_ERR("*** ERROR: [%s():%lu] \"%s\" is not a directory!\n", 
                __FUNCTION__, 
                __LINE__, 
                tstr2str(sSrcDirPath).c_str()
            );
        }
    }

    if (nRet == 0) {
        hFind = ::FindFirstFile(sFindPattern.c_str(), &oFindFileData);
        if (hFind == INVALID_HANDLE_VALUE) {
            nRet = 1;
            //LOG_ERR("*** ERROR: [%s():%lu] \"%s\" does not exist!\n", 
            //    __FUNCTION__, 
            //    __LINE__, 
            //    tstr2str(sSrcDirPath).c_str()
            //);
        }
    }

    if (nRet == 0) {
        do {
            std::tstring sBaseName;
            sBaseName = GetBaseName(oFindFileData.cFileName);

            if (oFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (sBaseName != str2tstr("") && sBaseName != str2tstr(".") && sBaseName != str2tstr("..")) {
                    std::tstring sNewSrcDirPath = MakePath(sSrcDirPath, sBaseName);
                    std::tstring sNewDstDirPath = MakePath(sDstDirPath, sBaseName);
                    if (CreateDir(sNewDstDirPath) == 0) {
                        nRetTmp = CopyTree(sNewSrcDirPath, sNewDstDirPath);
                        if (nRetTmp != 0 && nRetTmp != 1) {
                            LOG_ERR("*** ERROR: [%s():%lu] failed to copy the direcotry \"%s\" to the direcotry \"%s\"!\n", 
                                __FUNCTION__, 
                                __LINE__, 
                                tstr2str(sNewSrcDirPath).c_str(), 
                                tstr2str(sNewDstDirPath).c_str()
                            );
                        }
                    } else {
                        LOG_ERR("*** ERROR: [%s():%lu] failed to create the directory \"%s\"!\n", 
                            __FUNCTION__, 
                            __LINE__, 
                            tstr2str(sNewDstDirPath).c_str()
                        ); 
                    }
                } else {
                    // do nothing
                }
            } else {
                std::tstring sNewSrcFilePath = MakePath(sSrcDirPath, sBaseName);
                std::tstring sNewDstFilePath = MakePath(sDstDirPath, sBaseName);
                if (CopyFile(sNewSrcFilePath, sNewDstFilePath) != 0) {
                    LOG_ERR("*** ERROR: [%s():%lu] failed to copy the file \"%s\" to the file \"%s\"!\n", 
                        __FUNCTION__, 
                        __LINE__, 
                        tstr2str(sNewSrcFilePath).c_str(), 
                        tstr2str(sNewDstFilePath).c_str()
                    );
                } else {
                    LOG_INF("Copy \"%s\" to \"%s\"\n",  
                        tstr2str(sNewSrcFilePath).c_str(), 
                        tstr2str(sNewDstFilePath).c_str()
                    );
                }
            }
        } while (::FindNextFile(hFind, &oFindFileData));
    }

    if (hFind != INVALID_HANDLE_VALUE) {
        ::FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }

    return nRet;
}

int _tmain(int argc, _TCHAR* argv[])
{
    int nRet = 0;
    std::tstring sSrcDirPath;
    std::tstring sDstDirPath;

    if (argc < 3) {
        Usage(argc, argv);
        nRet = 0;
        return nRet;
    }

    if (argc >= 2 
        && (
                std::tstring(argv[1]) == str2tstr("/?")
                || std::tstring(argv[1]) == str2tstr("/H")
                || std::tstring(argv[1]) == str2tstr("/h")
                || std::tstring(argv[1]) == str2tstr("-H")
                || std::tstring(argv[1]) == str2tstr("--help")
           )
    ) {
        Usage(argc, argv);
        nRet = 0;
        return nRet;
    }

    if (nRet == 0) {
        sSrcDirPath = argv[1];
        sDstDirPath = argv[2];
    }

    if (nRet == 0) {
        if (!IsDir(sSrcDirPath)) {
            LOG_ERR("*** ERROR: [%s():%lu] \"%s\" is not a directory!\n", 
                __FUNCTION__, 
                __LINE__, 
                tstr2str(sSrcDirPath).c_str()
            );
            nRet = -1;
        }
    }

    if (nRet == 0) {
        if (!IsDir(sDstDirPath)) {
            nRet = CreateDir(sDstDirPath);
            if (nRet != 0) {
                LOG_ERR("*** ERROR: [%s():%lu] failed to create the direcotry \"%s\"!\n", 
                    __FUNCTION__, 
                    __LINE__, 
                    tstr2str(sDstDirPath).c_str()
                );
                nRet = -1;
            }
        }
    }

    if (nRet == 0) {
        nRet = CopyTree(sSrcDirPath, sDstDirPath);
        if (nRet != 0) {
            LOG_ERR("*** ERROR: [%s():%lu] failed to copy the direcotry \"%s\" to the direcotry \"%s\"!\n", 
                __FUNCTION__, 
                __LINE__, 
                tstr2str(sSrcDirPath).c_str(), 
                tstr2str(sDstDirPath).c_str()
            );
        }
    }

	return nRet;
}
