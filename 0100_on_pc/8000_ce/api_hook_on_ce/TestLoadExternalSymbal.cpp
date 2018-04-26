// TestLoadExternalSymbal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include "FuncHooker.h"

#define _EV_TAG                                         "__ExternalVariable__"
#define _EV_(symbal)                                    symbal ## __ExternalVariable__

#define _EF_TAG                                         "__ExternalFunction__"
#define _EF_(symbal)                                    symbal ## __ExternalFunction__

#define _EC_TAG                                         "__ExternalClass__"
#define _EC_(symbal)                                    symbal ## __ExternalClass__

#define CCC_SIZE                                        1024

class _EC_(CCC) {
public:
    _EC_(CCC)();
    ~_EC_(CCC)();
    
    static _EC_(CCC) &GetInstance();
    static _EC_(CCC) &GetInstance(int nPlaceHolder);

    virtual int PubF(char *p);

private:
    unsigned char m_RealObject[CCC_SIZE];       // NOTE: this member must be the first data member!
};


_EC_(CCC)::_EC_(CCC)()
{
    printf("in %s()\n", __FUNCTION__);
}

_EC_(CCC)::~_EC_(CCC)()
{
    printf("in %s()\n", __FUNCTION__);
}

_EC_(CCC) &_EC_(CCC)::GetInstance()
{
    printf("in %s()\n", __FUNCTION__);
    static _EC_(CCC) oThis;
    return oThis;
}

_EC_(CCC) &_EC_(CCC)::GetInstance(int nPlaceHolder)
{
    printf("in %s()\n", __FUNCTION__);
    static _EC_(CCC) oThis;
    return oThis;
}

int _EC_(CCC)::PubF(char *p)
{
    printf("in %s()\n", __FUNCTION__);
    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////

int _EF_(FFF)(int n)
{
    printf("in %s()\n", __FUNCTION__);
    return -1;
}

int *_EV_(VVVn) = NULL;
char *_EV_(VVVc) = NULL;
short *_EV_(VVVs) = NULL;

struct {
    const char *name;
    unsigned int age;
    bool sex;
} *_EV_(VVVst_Girl) = NULL;

////////////////////////////////////////////////////////////////////////////////////////

class SymbolAddressParser
{
private:
    typedef std::map<std::string, unsigned int> RecordMap;
    typedef std::map<std::string, unsigned int>::iterator RecordIterator;
    typedef std::pair<std::string, unsigned int> RecordPair;
    enum ParseType {
        PT_NORMAL,
        PT_WITH_TAG,
        PT_WITHOUT_TAG
    };

public:
    SymbolAddressParser() {}
    ~SymbolAddressParser() {}
    
    void SetMapFilePath(const std::string &sMapFilePath) { m_sMapFilePath = sMapFilePath; }
    bool Parse();
    bool ParseWithTag(const std::string &sTag);
    bool ParseWithoutTag(const std::string &sTag);
    unsigned int GetSymbolAddress(const std::string &sSymbol);
    void EnumAllSymbolBegin();
    bool EnumAllSymbolNext(std::string &sSymbal, unsigned int &nAddress);

private:
    bool ParseCommon(ParseType pt, const std::string &sTag);

private:
    char m_cLineBuf[1024];
    char m_cFuncNameBuf[1024];
    std::string m_sMapFilePath;
    RecordMap m_mapRecords;
    RecordIterator m_EnumAllSymbolIterator;
};

bool SymbolAddressParser::ParseCommon(ParseType pt, const std::string &sTag)
{
    bool bRet = false;
    FILE *pFile = NULL;
    unsigned int nFuncAddr;

    m_mapRecords.clear();

    pFile = fopen(m_sMapFilePath.c_str(), "r");
    if (pFile == NULL) {
        bRet = false;
        goto FAILED;
    }

    while (true) {
        char *pLine = fgets(m_cLineBuf, sizeof(m_cLineBuf), pFile);
        if (pLine == NULL) {
            if (feof(pFile)) {
                break;
            } else {
                bRet = false;
                goto FAILED;
            }
        }

        memset(m_cFuncNameBuf, 0, sizeof(m_cFuncNameBuf));
        nFuncAddr = 0;
        int nField = sscanf(m_cLineBuf, "%*s%s%x", m_cFuncNameBuf, &nFuncAddr);
        if (nField == 2) {
            if (m_cFuncNameBuf[0] != '\0' && nFuncAddr != 0) {
                std::string sFuncName(m_cFuncNameBuf);
                if (sFuncName.substr(0, 3) == std::string("??_")) {                     // ignore the internal symbols
                    continue;
                }

                if (pt == PT_NORMAL) {
                    m_mapRecords.insert(RecordPair(m_cFuncNameBuf, nFuncAddr));
                } else if (pt == PT_WITH_TAG) {
                    std::string::size_type nPos = sFuncName.find(sTag);
                    if (nPos != std::string::npos) {
                        sFuncName.erase(nPos, sTag.length());
                        if (sTag == _EV_TAG) {                                          // modify variable name
                            std::string sTemp("@@3PA");
                            if (sFuncName.substr(nPos, sTemp.length()) == sTemp) {
                                sFuncName.erase(nPos, sTemp.length());
                                sFuncName.insert(nPos, "@@3");
                            }
                        }
                        m_mapRecords.insert(RecordPair(sFuncName, nFuncAddr));
                    }
                    while (true) {
                        nPos = sFuncName.find(sTag);                                   // remove more tags for complex types
                        if (nPos != std::string::npos) {
                            sFuncName.erase(nPos, sTag.length());
                            m_mapRecords.insert(RecordPair(sFuncName, nFuncAddr));
                        } else {
                            break;
                        }
                    }
                } else if (pt == PT_WITHOUT_TAG) {
                    std::string::size_type nPos = sFuncName.find(sTag);
                    if (nPos == std::string::npos) {
                        m_mapRecords.insert(RecordPair(sFuncName, nFuncAddr));
                    }
                } else {
                    // do nothing
                }
            }
        }
    }

    bRet = true;

FAILED:
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }

    return bRet;
}

bool SymbolAddressParser::Parse()
{
    return ParseCommon(PT_NORMAL, "");
}

bool SymbolAddressParser::ParseWithTag(const std::string &sTag)
{
    return ParseCommon(PT_WITH_TAG, sTag);
}

bool SymbolAddressParser::ParseWithoutTag(const std::string &sTag)
{
    return ParseCommon(PT_WITHOUT_TAG, sTag);
}

unsigned int SymbolAddressParser::GetSymbolAddress(const std::string &sSymbol)
{
    unsigned int nAddress = 0;

    RecordIterator it = m_mapRecords.find(sSymbol);
    if (it != m_mapRecords.end()) {
        nAddress = it->second;
    }

    return nAddress;
}

void SymbolAddressParser::EnumAllSymbolBegin()
{
    m_EnumAllSymbolIterator = m_mapRecords.begin();
    
}

bool SymbolAddressParser::EnumAllSymbolNext(std::string &sSymbal, unsigned int &nAddress)
{
    if (m_EnumAllSymbolIterator != m_mapRecords.end()) {
        sSymbal = m_EnumAllSymbolIterator->first;
        nAddress = m_EnumAllSymbolIterator->second;
        ++m_EnumAllSymbolIterator;
        return true;
    } else {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

#if defined(WIN32_X86) // for Win32 X86
#define MAP_FILE_PATH                   "TestLoadExternalSymbal.map"
#elif defined(WINCE_ARM32) // for WinCE ARM32
#define MAP_FILE_PATH                   "release\\TestNavi.map"
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

int _tmain(int argc, _TCHAR* argv[])
{
    SymbolAddressParser sap1;
    sap1.SetMapFilePath(MAP_FILE_PATH);
    sap1.Parse();

    SymbolAddressParser sap2;
    sap2.SetMapFilePath(MAP_FILE_PATH);
    sap2.ParseWithTag(_EF_TAG);

    SymbolAddressParser sap3;
    sap3.SetMapFilePath(MAP_FILE_PATH);
    sap3.ParseWithTag(_EC_TAG);

    SymbolAddressParser sap4;
    sap4.SetMapFilePath(MAP_FILE_PATH);
    sap4.ParseWithTag(_EV_TAG);

    FuncHookerHelper funcHookerHelper1(false);
    {
        FuncHookerHelper &funcHookerHelper = funcHookerHelper1;
        SymbolAddressParser &sap = sap2;

        funcHookerHelper.HookTransactionBegin();
        std::string sSymbalName;
        unsigned int nTargetAddess;
        sap.EnumAllSymbolBegin();
        while (sap.EnumAllSymbolNext(sSymbalName, nTargetAddess)) {
            unsigned int nDetourAddess = (unsigned int)sap1.GetSymbolAddress(sSymbalName);
            if (nDetourAddess != NULL && nTargetAddess != NULL) {
                funcHookerHelper.AddHooker((void *)nTargetAddess, (void *)nDetourAddess, NULL);
            }
        }
        funcHookerHelper.HookTransactionCommit();
    }

    FuncHookerHelper funcHookerHelper2(false);
    {
        FuncHookerHelper &funcHookerHelper = funcHookerHelper2;
        SymbolAddressParser &sap = sap3;

        funcHookerHelper.HookTransactionBegin();
        std::string sSymbalName;
        unsigned int nTargetAddess;
        sap.EnumAllSymbolBegin();
        while (sap.EnumAllSymbolNext(sSymbalName, nTargetAddess)) {
            unsigned int nDetourAddess = (unsigned int)sap1.GetSymbolAddress(sSymbalName);
            if (nDetourAddess != NULL && nTargetAddess != NULL) {
                funcHookerHelper.AddHooker((void *)nTargetAddess, (void *)nDetourAddess, NULL);
            }
        }
        funcHookerHelper.HookTransactionCommit();
    }

    FuncHookerHelper funcHookerHelper3(false);
    {
        FuncHookerHelper &funcHookerHelper = funcHookerHelper3;
        SymbolAddressParser &sap = sap4;

        funcHookerHelper.HookTransactionBegin();
        std::string sSymbalName;
        unsigned int nTargetAddess;
        sap.EnumAllSymbolBegin();
        while (sap.EnumAllSymbolNext(sSymbalName, nTargetAddess)) {
            unsigned int nDetourAddess = (unsigned int)sap1.GetSymbolAddress(sSymbalName);
            if (nDetourAddess != NULL && nTargetAddess != NULL) {
                *(unsigned int *)(nTargetAddess) = nDetourAddess;
            }
        }
        funcHookerHelper.HookTransactionCommit();
    }

    {
        _EC_(CCC) ccc;
        ccc.PubF("created by constructor");
    }

    printf("----------------------------------------------\n");

    {
        _EC_(CCC) &rccc = _EC_(CCC)::GetInstance();
        rccc.PubF("created by GetInstance()");
    }

    printf("----------------------------------------------\n");

    {
        _EC_(CCC) &rccc = _EC_(CCC)::GetInstance(0);
        rccc.PubF("created by inline GetInstance()");
    }

    printf("----------------------------------------------\n");

    {
        _EF_(FFF)(123);
    }

    printf("----------------------------------------------\n");

    {
        if (_EV_(VVVn) != NULL) {
            printf("*_EV_(VVVn)=%d\n", *_EV_(VVVn));
        } else {
            printf("_EV_(VVVn)=NULL\n");
        }

        if (_EV_(VVVc) != NULL) {
            printf("*_EV_(VVVc)=%c\n", *_EV_(VVVc));
        } else {
            printf("_EV_(VVVc)=NULL\n");
        }

        if (_EV_(VVVs) != NULL) {
            printf("*_EV_(VVVs)=%d\n", *_EV_(VVVs));
        } else {
            printf("_EV_(VVVs)=NULL\n");
        }

        
        if (_EV_(VVVst_Girl) != NULL) {
            printf("My name is %s.\nMy age is %u.\nI'm a %s.\n",
                _EV_(VVVst_Girl)->name,
                _EV_(VVVst_Girl)->age,
                _EV_(VVVst_Girl)->sex ? "boy" : "girl"
            );
        } else {
            printf("_EV_(VVVst_Girl)=NULL\n");
        }
    }

    printf("----------------------------------------------\n");

	return 0;
}

