#include "TK_RegKey.h"

#ifdef WINCE
#define RK_REGSAM_KEY_ALL_ACCESS                    0
#define RK_REGSAM_KEY_READ_ACCESS                   0
#else
#define RK_REGSAM_KEY_ALL_ACCESS                    KEY_ALL_ACCESS
#define RK_REGSAM_KEY_READ_ACCESS                   KEY_READ
#endif // #ifdef WINCE

namespace TK_Tools {

//////////////////////////////////////////////////////////////////////////////////////////
// RegKey::KeyValue
//

RegKey::KeyValue::KeyValue() 
    : type(KVT_UNKNOWN)
    , value()
{

}

RegKey::KeyValue::KeyValue(const RegKey::KeyValue &other)
    : type(other.type)
    , value(other.value)
{
    
}
	
RegKey::KeyValue::KeyValue(const std::vector<BYTE> &vBinary) 
    : type(KVT_BINARY)
    , value(vBinary)
{

}
	
RegKey::KeyValue::KeyValue(const BYTE *pData, unsigned int nDataSize) 
    : type(KVT_BINARY)
    , value()
{
    std::vector<BYTE> &vBinary = value.vBinary;
    
    if (pData != NULL && nDataSize != 0) {
        vBinary = std::vector<BYTE>(pData, pData + nDataSize);
    }
}
	
RegKey::KeyValue::KeyValue(DWORD vDword) 
    : type(KVT_DWORD)
    , value(vDword)
{

}
	
RegKey::KeyValue::KeyValue(const std::wstring &vSz, bool bExpend /*= false*/)
    : type(bExpend ? KVT_EXPAND_SZ : KVT_SZ)
    , value()
{
	if (bExpend) {
		value.vExpendSz = vSz; 
	} else {
		value.vSz = vSz;
	}
}
	
RegKey::KeyValue::KeyValue(const std::vector<std::wstring> &vMultiSz) 
    : type(KVT_MULTI_SZ)
    , value(vMultiSz)
{

}

RegKey::KeyValue::KeyValue(const wchar_t **ppMultiSz, unsigned int nCount)
    : type(KVT_MULTI_SZ)
    , value()
{
    std::vector<std::wstring> &vMultiSz = value.vMultiSz;
    unsigned int i;
    
    if (ppMultiSz != NULL && nCount != 0) {
        for (i = 0; i < nCount; i++) {
            if (ppMultiSz[i] == NULL) {
                vMultiSz.push_back(std::wstring(L""));
            } else {
                vMultiSz.push_back(std::wstring(ppMultiSz[i]));
            }
        }
    }
}

RegKey::KeyValue &RegKey::KeyValue::operator =(const RegKey::KeyValue &other)
{
    type = other.type;
    value = other.value;

    return *this;
}

std::wstring RegKey::KeyValue::ToStr()
{
    std::wstring sResult;
    unsigned int i;

    switch (type) {
    case KeyValue::KVT_BINARY:
        {
            sResult += L"hex:";
            for (i = 0; i < value.vBinary.size(); i++) {
                if (i > 0) {
                    sResult += L" ";
                }
                sResult += FormatWStr(L"%02X", value.vBinary[i]);
            }
        }
        break;
    case KeyValue::KVT_DWORD:
        {
            sResult += L"dword:";
            sResult += FormatWStr(L"0x%08X", value.vDword);
        }
        break;
    case KeyValue::KVT_SZ:
        {
            sResult += L"";
            sResult += FormatWStr(L"\"%s\"", value.vSz.c_str());
        }
        break;
    case KeyValue::KVT_EXPAND_SZ:
        {
            sResult += L"expand_sz:";
            sResult += FormatWStr(L"\"%s\"", value.vExpendSz.c_str());
        }
        break;
    case KeyValue::KVT_MULTI_SZ:
        {
            sResult += L"multi_sz:";
            for (i = 0; i < value.vMultiSz.size(); i++) {
                if (i > 0) {
                    sResult += L",";
                }
                sResult += FormatWStr(L"\r\n");
                sResult += FormatWStr(L"    \"%s\"", value.vMultiSz[i].c_str());
            }
        }
        break;
    default:
        {
            sResult += L"(unknown):";
        }
        break;
    }

    return sResult;
}

namespace {

bool ParseKeyValueStr__binary(const std::string &sValuePart, RegKey::KeyValue &rKeyValue)
{
    bool bRet = true;
    std::string sValuePartTemp = sValuePart;
    std::vector<unsigned char> arrUnsignedCharArray;

    if (bRet) {
        sValuePartTemp = TK_Tools::TrimAll(sValuePartTemp);
        bRet = TK_Tools::GetUnsignedCharArrayFormStr(sValuePart, &arrUnsignedCharArray);
    }

    if (bRet) {
        rKeyValue = RegKey::KeyValue(arrUnsignedCharArray);
    }
    
    return bRet;
}

bool ParseKeyValueStr__dword(const std::string &sValuePart, RegKey::KeyValue &rKeyValue)
{
    bool bRet = true;
    std::string sValuePartTemp = sValuePart;
    unsigned int nUnsignedIntValue = 0;

    if (bRet) {
        sValuePartTemp = TK_Tools::TrimAll(sValuePartTemp);
        bRet = GetUnsignedIntFormStr(sValuePart, &nUnsignedIntValue);
    }

    if (bRet) {
        rKeyValue = RegKey::KeyValue(nUnsignedIntValue);
    }
    
    return bRet;
}

bool ParseKeyValueStr__sz(const std::string &sValuePart, RegKey::KeyValue &rKeyValue)
{
    bool bRet = true;
    std::string sValuePartTemp = sValuePart;

    if (bRet) {
        sValuePartTemp = TK_Tools::TrimAll(sValuePartTemp);
        if (sValuePartTemp.length() < 2) {
            bRet = false;
        }
    }

    if (bRet) {
        if (sValuePartTemp[0] != '\"' || sValuePartTemp[sValuePartTemp.length() - 1] != '\"')
        {
            bRet = false;
        } else {
            sValuePartTemp = sValuePartTemp.substr(1, sValuePartTemp.length() - 2);
        }
    }

    if (bRet) {
        rKeyValue = RegKey::KeyValue(TK_Tools::str2wstr(sValuePartTemp));
    }
    
    return bRet;
}

bool ParseKeyValueStr__expand_sz(const std::string &sValuePart, RegKey::KeyValue &rKeyValue)
{
    bool bRet = true;
    std::string sValuePartTemp = sValuePart;

    if (bRet) {
        sValuePartTemp = TK_Tools::TrimAll(sValuePartTemp);
        if (sValuePartTemp.length() < 2) {
            bRet = false;
        }
    }

    if (bRet) {
        if (sValuePartTemp[0] != '\"' || sValuePartTemp[sValuePartTemp.length() - 1] != '\"')
        {
            bRet = false;
        } else {
            sValuePartTemp = sValuePartTemp.substr(1, sValuePartTemp.length() - 2);
        }
    }

    if (bRet) {
        rKeyValue = RegKey::KeyValue(TK_Tools::str2wstr(sValuePartTemp), true);
    }
    
    return bRet;
}

bool ParseKeyValueStr__multi_sz__Parse(std::vector<std::wstring> &arrRetStrings, const std::string &sMultiStrings, bool bEscapeSupported)
{
    bool bRet = true;
    std::string sArgTemp;
    const char *pCh;
    bool bQuoted;
    bool bEscaped;
    bool bComma;
    bool bOK;
    unsigned int nUnsignedInt;

    arrRetStrings.clear();
    
    sArgTemp = "";
    pCh = sMultiStrings.c_str();
    bQuoted = false;
    bEscaped = false;
    bComma = false;
    while (*pCh != '\0') {
        if (bEscapeSupported && bEscaped) {
            if (       *pCh == '\"' 
                    || *pCh == '\\' 
                    || *pCh == '\a' 
                    || *pCh == '\b' 
                    //|| *pCh == '\c' 
                    //|| *pCh == '\e' 
                    || *pCh == '\f' 
                    || *pCh == '\n' 
                    || *pCh == '\r' 
                    || *pCh == '\t' 
                    || *pCh == '\v' 
                ) {
                sArgTemp += '\\';
                sArgTemp += *pCh;
                pCh++;
            } else if (*pCh == 'x') {
                pCh++;

                std::string sTemp(pCh, 0, 2);
                if (sTemp.length() != 2) {
                    bRet = false;
                    break;
                }
                std::string sTemp2 = std::string("0x") + sTemp;
                bOK = GetUnsignedIntFormStr(sTemp2, &nUnsignedInt);
                if (!bOK) {
                    bRet = false;
                    break;
                }
                sArgTemp += '\\';
                sArgTemp += sTemp;

                pCh++;
                pCh++;
            } else {
                sArgTemp += '\\';
                sArgTemp += *pCh;
                pCh++;
            }
            bEscaped = false;
        } else {
            if (*pCh == '\"') {
                if (!bQuoted) {
                    if (!sArgTemp.empty()) {
                        bRet = false;
                        break;
                    }
                    bQuoted = true;
                    if (!arrRetStrings.empty() && !bComma) {
                        bRet = false;
                        break;
                    }
                    bComma = false;
                } else {
                    arrRetStrings.push_back(TK_Tools::str2wstr(sArgTemp));
                    sArgTemp = "";
                    bQuoted = false;
                }
            } else {
                if (!bQuoted && IsSpaceChar(*pCh)) {
                    if (!sArgTemp.empty()) {
                        arrRetStrings.push_back(TK_Tools::str2wstr(sArgTemp));
                        sArgTemp = "";
                    }
                } else if (!bQuoted && *pCh == ',') {
                    if (bComma) {
                        bRet = false;
                        break;
                    }
                    if (!sArgTemp.empty()) {
                        bRet = false;
                        break;
                    }
                    bComma = true;
                } else {
                    if (bEscapeSupported && *pCh == '\\') {
                        bEscaped = true;
                    } else {
                        if (bComma) {
                            bRet = false;
                            break;
                        }
                        sArgTemp += *pCh;
                    }
                }
            }

            pCh++;
        }
    }
    if (bComma) {
        bRet = false;
    } else {
        if (!sArgTemp.empty()) {
            arrRetStrings.push_back(TK_Tools::str2wstr(sArgTemp));
            sArgTemp = "";
        }
    }

    return bRet;
}

bool ParseKeyValueStr__multi_sz(const std::string &sValuePart, RegKey::KeyValue &rKeyValue)
{
    bool bRet = true;
    std::vector<std::wstring> arrRetStrings;

    if (bRet) {
        bRet = ParseKeyValueStr__multi_sz__Parse(arrRetStrings, sValuePart, true);
    }

    if (bRet) {
        rKeyValue = RegKey::KeyValue(arrRetStrings);
    }
    
    return bRet;
}

bool ParseKeyValueStr(const std::wstring &sKeyValueStr, RegKey::KeyValue &rKeyValue)
{
    bool bRet = true;
    std::string sKeyValueStrTemp = TK_Tools::wstr2str(sKeyValueStr);
    std::wstring sType;
    std::wstring sValue;
    
    rKeyValue = RegKey::KeyValue();

    if (bRet) {
        sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
        if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, "hex") == 0) {
            sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen("hex"));
            sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
            if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, ":") == 0) {
                sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen(":"));
                sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
                bRet = ParseKeyValueStr__binary(sKeyValueStrTemp, rKeyValue);
            }
            else {
                bRet = false;
            }
        } else if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, "dword") == 0) {
            sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen("dword"));
            sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
            if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, ":") == 0) {
                sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen(":"));
                sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
                bRet = ParseKeyValueStr__dword(sKeyValueStrTemp, rKeyValue);
            }
            else {
                bRet = false;
            }
        } else if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, "expand_sz") == 0) {
            sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen("expand_sz"));
            sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
            if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, ":") == 0) {
                sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen(":"));
                sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
                bRet = ParseKeyValueStr__expand_sz(sKeyValueStrTemp, rKeyValue);
            }
            else {
                bRet = false;
            }
        } else if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, "multi_sz") == 0) {
            sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen("multi_sz"));
            sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
            if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, ":") == 0) {
                sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen(":"));
                sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
                bRet = ParseKeyValueStr__multi_sz(sKeyValueStrTemp, rKeyValue);
            }
            else {
                bRet = false;
            }
        } else if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, "sz") == 0) {
            sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen("sz"));
            sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
            if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, ":") == 0) {
                sKeyValueStrTemp = sKeyValueStrTemp.substr(::strlen(":"));
                sKeyValueStrTemp = TK_Tools::TrimLeft(sKeyValueStrTemp);
                bRet = ParseKeyValueStr__sz(sKeyValueStrTemp, rKeyValue);
            }
            else {
                bRet = false;
            }
        } else if (TK_Tools::CompareNoCaseRightLen(sKeyValueStrTemp, "\"") == 0) {
            bRet = ParseKeyValueStr__sz(sKeyValueStrTemp, rKeyValue);
        } else {
            bRet = ParseKeyValueStr__dword(sKeyValueStrTemp, rKeyValue);
        }
    }

    return bRet;
}

} // namespace {

bool RegKey::KeyValue::FromStr(const std::wstring &sValueStr, KeyValue &rRetKeyValue)
{
    return ParseKeyValueStr(sValueStr, rRetKeyValue);
}

//////////////////////////////////////////////////////////////////////////////////////////
// RegKey
//

DWORD RegKey::m_nWin32LastError = 0;

RegKey::RegKey()
    : m_hKey(NULL)
{

}

RegKey::RegKey(HKEY hKey)
    : m_hKey(hKey)
{

}

RegKey::~RegKey()
{
    if (m_hKey != NULL) {
        ::RegCloseKey(m_hKey);
        m_hKey = NULL;
    }
}

RegKey::ErrCode RegKey::CreateKey(RegKey **ppRetKey, const std::wstring &sKeyPath, const RegKey *pParentKey /*= NULL*/)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    std::vector<std::wstring> sPathNodes;
    HKEY hParentKey = NULL;
    HKEY hCurentKey;
    HKEY hNewKey;
    unsigned int i;
    DWORD dwDisposition;

    if (nErrCode == EC_OK) {
        if (ppRetKey == NULL) {
            nErrCode = EC_NULL_PTR;
        }
    }

    if (nErrCode == EC_OK) {
        nErrCode = ParseKeyPath(sKeyPath, pParentKey, sPathNodes, hParentKey);
    }

    //if (nErrCode == EC_OK) {
    //    if (sPathNodes.empty()) {
    //        nErrCode = EC_NO_SUB_KEY;
    //    }
    //}

    if (nErrCode == EC_OK) {
        hCurentKey = hParentKey;
        for (i = 0; i < sPathNodes.size(); i++) {
            std::wstring &sPathNode = sPathNodes[i];
            nRet = ::RegCreateKeyEx(hCurentKey, sPathNode.c_str(), NULL, NULL, REG_OPTION_NON_VOLATILE, RK_REGSAM_KEY_ALL_ACCESS, NULL, &hNewKey, &dwDisposition);
            if (nRet != ERROR_SUCCESS) {
                m_nWin32LastError = nRet;
                nErrCode = EC_FAILED_TO_CREATE_KEY;
                if (hCurentKey != hParentKey) {
                    ::RegCloseKey(hCurentKey);
                }
                break;
            } else {
                if (hCurentKey != hParentKey) {
                    ::RegCloseKey(hCurentKey);
                }
                hCurentKey = hNewKey;
            }
        }
    }

    if (nErrCode == EC_OK) {
        *ppRetKey = new RegKey(hCurentKey);
        if (*ppRetKey == NULL) {
            if (hCurentKey != hParentKey) {
                ::RegCloseKey(hCurentKey);
            }
            nErrCode = EC_FAILED_TO_ALLOC_MEM;
        }
    }
    
    return nErrCode;
}

RegKey::ErrCode RegKey::OpenKey(RegKey **ppRetKey, const std::wstring &sKeyPath, const RegKey *pParentKey /*= NULL*/)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    std::vector<std::wstring> sPathNodes;
    HKEY hParentKey = NULL;
    HKEY hCurentKey;
    HKEY hNewKey;
    unsigned int i;

    if (nErrCode == EC_OK) {
        if (ppRetKey == NULL) {
            nErrCode = EC_NULL_PTR;
        }
    }

    if (nErrCode == EC_OK) {
        nErrCode = ParseKeyPath(sKeyPath, pParentKey, sPathNodes, hParentKey);
    }

    //if (nErrCode == EC_OK) {
    //    if (sPathNodes.empty()) {
    //        nErrCode = EC_NO_SUB_KEY;
    //    }
    //}

    if (nErrCode == EC_OK) {
        hCurentKey = hParentKey;
        for (i = 0; i < sPathNodes.size(); i++) {
            std::wstring &sPathNode = sPathNodes[i];
            nRet = ::RegOpenKeyEx(hCurentKey, sPathNode.c_str(), 0, RK_REGSAM_KEY_ALL_ACCESS, &hNewKey);
            if (nRet == ERROR_ACCESS_DENIED) {
                nRet = ::RegOpenKeyEx(hCurentKey, sPathNode.c_str(), 0, RK_REGSAM_KEY_READ_ACCESS, &hNewKey);
            } 
            if (nRet != ERROR_SUCCESS) {
                m_nWin32LastError = nRet;
                nErrCode = EC_FAILED_TO_OPEN_KEY;
                if (hCurentKey != hParentKey) {
                    ::RegCloseKey(hCurentKey);
                }
                break;
            } else {
                if (hCurentKey != hParentKey) {
                    ::RegCloseKey(hCurentKey);
                }
                hCurentKey = hNewKey;
            }
        }
    }

    if (nErrCode == EC_OK) {
        *ppRetKey = new RegKey(hCurentKey);
        if (*ppRetKey == NULL) {
            if (hCurentKey != hParentKey) {
                ::RegCloseKey(hCurentKey);
            }
            nErrCode = EC_FAILED_TO_ALLOC_MEM;
        }
    }
    
    return nErrCode;
}

RegKey::ErrCode RegKey::DeleteKey(const std::wstring &sKeyPath, const RegKey *pParentKey /*= NULL*/)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    std::vector<std::wstring> sPathNodes;
    HKEY hParentKey = NULL;
    HKEY hCurentKey;
    HKEY hNewKey;
    unsigned int i;

    if (nErrCode == EC_OK) {
        nErrCode = ParseKeyPath(sKeyPath, pParentKey, sPathNodes, hParentKey);
    }

    if (nErrCode == EC_OK) {
        if (sPathNodes.empty()) {
            nErrCode = EC_NO_SUB_KEY;
        }
    }

    if (nErrCode == EC_OK) {
        hCurentKey = hParentKey;
        for (i = 0; i < sPathNodes.size() - 1; i++) {
            std::wstring &sPathNode = sPathNodes[i];
            nRet = ::RegOpenKeyEx(hCurentKey, sPathNode.c_str(), 0, RK_REGSAM_KEY_ALL_ACCESS, &hNewKey);
            if (nRet != ERROR_SUCCESS) {
                m_nWin32LastError = nRet;
                nErrCode = EC_FAILED_TO_OPEN_KEY;
                if (hCurentKey != hParentKey) {
                    ::RegCloseKey(hCurentKey);
                }
                break;
            } else {
                if (hCurentKey != hParentKey) {
                    ::RegCloseKey(hCurentKey);
                }
                hCurentKey = hNewKey;
            }
        }
    }

    if (nErrCode == EC_OK) {
        std::wstring &sPathNode = sPathNodes[sPathNodes.size() - 1];
        nRet = ::RegDeleteKey(hCurentKey, sPathNode.c_str());
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_DELETE_KEY;
        }
        if (hCurentKey != hParentKey) {
            ::RegCloseKey(hCurentKey);
        }
    }
    
    return nErrCode;
}

bool RegKey::KeyExists(const std::wstring &sKeyPath, const RegKey *pParentKey /*= NULL*/)
{
    bool bRet = false;
    ErrCode nErrCode = EC_OK;
    RegKey *pRetKey = NULL;

    if (nErrCode == EC_OK) {
        nErrCode = OpenKey(&pRetKey, sKeyPath, pParentKey);
    }

    if (nErrCode == EC_OK) {
        if (pRetKey != NULL) {
            delete pRetKey;
            pRetKey = NULL;
            bRet = true;
        }
    }

    return bRet;
}

RegKey::ErrCode RegKey::GetSubKeyNames(std::vector<std::wstring> &arrSubKeyNames, const std::wstring &sKeyPath, const RegKey *pParentKey /*= NULL*/)
{
    ErrCode nErrCode = EC_OK;
    RegKey *pRetKey = NULL;

    arrSubKeyNames.clear();

    if (nErrCode == EC_OK) {
        nErrCode = OpenKey(&pRetKey, sKeyPath, pParentKey);
    }

    if (nErrCode == EC_OK) {
        if (pRetKey != NULL) {
            nErrCode = pRetKey->GetSubKeyNames(arrSubKeyNames);
            delete pRetKey;
            pRetKey = NULL;
        }
    }

    return nErrCode;

}

RegKey::ErrCode RegKey::FlushKey(const std::wstring &sKeyPath, const RegKey *pParentKey /*= NULL*/)
{
    ErrCode nErrCode = EC_OK;
    RegKey *pRetKey = NULL;

    if (nErrCode == EC_OK) {
        nErrCode = OpenKey(&pRetKey, sKeyPath, pParentKey);
    }

    if (nErrCode == EC_OK) {
        if (pRetKey != NULL) {
            nErrCode = pRetKey->FlushKey();
            delete pRetKey;
            pRetKey = NULL;
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::FlushAllKeys()
{
    ErrCode nErrCode = EC_OK;

    if (nErrCode == EC_OK) {
        RegKey oRegKey(HKEY_CLASSES_ROOT);
        nErrCode = oRegKey.FlushKey();
        if (nErrCode != nErrCode) {
            nErrCode = EC_FAILED_TO_FLUSH_KEY;
        }
    }

#if !defined(WINCE)
    if (nErrCode == EC_OK) {
        RegKey oRegKey(HKEY_CURRENT_CONFIG);
        nErrCode = oRegKey.FlushKey();
        if (nErrCode != nErrCode) {
            nErrCode = EC_FAILED_TO_FLUSH_KEY;
        }
    }
#endif // #if !defined(WINCE)

    if (nErrCode == EC_OK) {
        RegKey oRegKey(HKEY_CURRENT_USER);
        nErrCode = oRegKey.FlushKey();
        if (nErrCode != nErrCode) {
            nErrCode = EC_FAILED_TO_FLUSH_KEY;
        }
    }

    if (nErrCode == EC_OK) {
        RegKey oRegKey(HKEY_LOCAL_MACHINE);
        nErrCode = oRegKey.FlushKey();
        if (nErrCode != nErrCode) {
            nErrCode = EC_FAILED_TO_FLUSH_KEY;
        }
    }

    if (nErrCode == EC_OK) {
        RegKey oRegKey(HKEY_USERS);
        nErrCode = oRegKey.FlushKey();
        if (nErrCode != nErrCode) {
            nErrCode = EC_FAILED_TO_FLUSH_KEY;
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::SetValue(const std::wstring &sKeyPath, const std::wstring &sValueName, const KeyValue &rValue, bool bCreatePathIfNotExists /*= false*/, const RegKey *pParentKey /*= NULL*/)
{
    ErrCode nErrCode = EC_OK;
    RegKey *pRetKey = NULL;

    if (nErrCode == EC_OK) {
        if (bCreatePathIfNotExists) {
            nErrCode = CreateKey(&pRetKey, sKeyPath, pParentKey);
        } else {
            nErrCode = OpenKey(&pRetKey, sKeyPath, pParentKey);
        }
    }

    if (nErrCode == EC_OK) {
        if (pRetKey != NULL) {
            nErrCode = pRetKey->SetValue(sValueName, rValue);
            delete pRetKey;
            pRetKey = NULL;
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::GetValue(const std::wstring &sKeyPath, const std::wstring &sValueName, KeyValue &rValue, const RegKey *pParentKey /*= NULL*/)
{
    ErrCode nErrCode = EC_OK;
    RegKey *pRetKey = NULL;

    if (nErrCode == EC_OK) {
        nErrCode = OpenKey(&pRetKey, sKeyPath, pParentKey);
    }

    if (nErrCode == EC_OK) {
        if (pRetKey != NULL) {
            nErrCode = pRetKey->GetValue(sValueName, rValue);
            delete pRetKey;
            pRetKey = NULL;
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::DeleteValue(const std::wstring &sKeyPath, const std::wstring &sValueName, const RegKey *pParentKey /*= NULL*/)
{
    ErrCode nErrCode = EC_OK;
    RegKey *pRetKey = NULL;

    if (nErrCode == EC_OK) {
        nErrCode = OpenKey(&pRetKey, sKeyPath, pParentKey);
    }

    if (nErrCode == EC_OK) {
        if (pRetKey != NULL) {
            nErrCode = pRetKey->DeleteValue(sValueName);
            delete pRetKey;
            pRetKey = NULL;
        }
    }

    return nErrCode;
}

bool RegKey::ValueExists(const std::wstring &sKeyPath, const std::wstring &sValueName, const RegKey *pParentKey /*= NULL*/)
{
    bool bRet = false;
    ErrCode nErrCode = EC_OK;
    RegKey *pRetKey = NULL;

    if (nErrCode == EC_OK) {
        nErrCode = OpenKey(&pRetKey, sKeyPath, pParentKey);
    }

    if (nErrCode == EC_OK) {
        if (pRetKey != NULL) {
            bRet = pRetKey->ValueExists(sValueName);
            delete pRetKey;
            pRetKey = NULL;
        }
    }

    return bRet;
}

RegKey::ErrCode RegKey::GetValues(std::vector<KeyNameValuePair> &arrKeyValues, const std::wstring &sKeyPath, const RegKey *pParentKey /*= NULL*/)
{
    ErrCode nErrCode = EC_OK;
    RegKey *pRetKey = NULL;

    arrKeyValues.clear();

    if (nErrCode == EC_OK) {
        nErrCode = OpenKey(&pRetKey, sKeyPath, pParentKey);
    }

    if (nErrCode == EC_OK) {
        if (pRetKey != NULL) {
            nErrCode = pRetKey->GetValues(arrKeyValues);
            delete pRetKey;
            pRetKey = NULL;
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::CreateKey(RegKey **ppRetKey, const std::wstring &sKeyName)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    HKEY hkResult = NULL;
    DWORD dwDisposition;

    if (nErrCode == EC_OK) {
        if (ppRetKey == NULL) {
            nErrCode = EC_NULL_PTR;
        }
    }

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegCreateKeyEx(m_hKey, sKeyName.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, RK_REGSAM_KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition);
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_CREATE_KEY;
        }
    }

    if (nErrCode == EC_OK) {
        *ppRetKey = new RegKey(hkResult);
        if (*ppRetKey == NULL) {
            nErrCode = EC_FAILED_TO_ALLOC_MEM;
        }

        if (*ppRetKey == NULL) {
            ::RegCloseKey(hkResult);
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::CreateKey(const std::wstring &sKeyName)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    HKEY hkResult = NULL;
    DWORD dwDisposition;

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegCreateKeyEx(m_hKey, sKeyName.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, RK_REGSAM_KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition);
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_CREATE_KEY;
        } else {
            ::RegCloseKey(hkResult);
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::OpenKey(RegKey **ppRetKey, const std::wstring &sKeyName)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    HKEY hkResult = NULL;

    if (nErrCode == EC_OK) {
        if (ppRetKey == NULL) {
            nErrCode = EC_NULL_PTR;
        }
    }

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegOpenKeyEx(m_hKey, sKeyName.c_str(), 0, RK_REGSAM_KEY_ALL_ACCESS, &hkResult);
        if (nRet == ERROR_ACCESS_DENIED) {
            nRet = ::RegOpenKeyEx(m_hKey, sKeyName.c_str(), 0, RK_REGSAM_KEY_READ_ACCESS, &hkResult);
        }
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_OPEN_KEY;
        }
    }

    if (nErrCode == EC_OK) {
        *ppRetKey = new RegKey(hkResult);
        if (*ppRetKey == NULL) {
            nErrCode = EC_FAILED_TO_OPEN_KEY;
        }

        if (*ppRetKey == NULL) {
            ::RegCloseKey(hkResult);
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::DeleteKey(const std::wstring &sKeyName)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegDeleteKey(m_hKey, sKeyName.c_str());
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_DELETE_KEY;
        }
    }

    return nErrCode;
}

bool RegKey::KeyExists(const std::wstring &sKeyName)
{
    bool bRet = false;
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    HKEY hkResult = NULL;

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegOpenKeyEx(m_hKey, sKeyName.c_str(), 0, RK_REGSAM_KEY_READ_ACCESS, &hkResult);
        if (nRet == ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            ::RegCloseKey(hkResult);
            bRet = true;
        }
    }

    return bRet;
}

RegKey::ErrCode RegKey::GetSubKeyNames(std::vector<std::wstring> &arrKeyNames)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    DWORD cSubKeys = 0;
    DWORD cbMaxSubKeyLen = 0;
    DWORD cbMaxSubKeyLenRet;
    wchar_t *pwsKeyNameBuf = NULL;
    unsigned int i;

    arrKeyNames.clear();

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegQueryInfoKey(m_hKey, NULL, NULL, NULL, &cSubKeys, &cbMaxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_GET_KEY_INFO;
        } else {
            cbMaxSubKeyLen++;
        }
    }

    if (nErrCode == EC_OK) {
        pwsKeyNameBuf = new wchar_t [cbMaxSubKeyLen];
        if (pwsKeyNameBuf == NULL) {
            nErrCode = EC_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrCode == EC_OK) {
        for (i = 0; i < cSubKeys; i++) {
            memset(pwsKeyNameBuf, 0, cbMaxSubKeyLen * sizeof(wchar_t));
            cbMaxSubKeyLenRet = cbMaxSubKeyLen;
            nRet = ::RegEnumKeyEx(m_hKey, i, pwsKeyNameBuf, &cbMaxSubKeyLenRet, NULL, NULL, NULL, NULL);
            if (nRet != ERROR_SUCCESS) {
                m_nWin32LastError = nRet;
                nErrCode = EC_FAILED_TO_GET_KEY_INFO;
                break;
            } else {
                arrKeyNames.push_back(std::wstring(pwsKeyNameBuf));
            }
        }
    }

    if (pwsKeyNameBuf != NULL) {
        delete [] pwsKeyNameBuf;
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::FlushKey()
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegFlushKey(m_hKey);
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_FLUSH_KEY;
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::SetValue(const std::wstring &sValueName, const KeyValue &rValue)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    DWORD dwType;
    const BYTE *lpData;
    DWORD cbData;
    unsigned int i;
    std::vector<wchar_t> arrMultiSzBuf;

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }
    
    if (nErrCode == EC_OK) {
        switch (rValue.type) {
        case KeyValue::KVT_BINARY:
            {
                dwType = REG_BINARY;
                lpData = (BYTE *)&rValue.value.vBinary[0];
                cbData = rValue.value.vBinary.size();
            }
            // pass through
        case KeyValue::KVT_DWORD:
            {
                dwType = REG_DWORD;
                lpData = (BYTE *)&rValue.value.vDword;
                cbData = sizeof(DWORD);
            }
            // pass through
        case KeyValue::KVT_SZ:
            {
                dwType = REG_SZ;
                lpData = (BYTE *)rValue.value.vSz.c_str();
                cbData = (rValue.value.vSz.length() + 1) * sizeof(wchar_t);
            }
            // pass through
        case KeyValue::KVT_EXPAND_SZ:
            {
                dwType = REG_EXPAND_SZ;
                lpData = (BYTE *)rValue.value.vExpendSz.c_str();
                cbData = (rValue.value.vExpendSz.length() + 1) * sizeof(wchar_t);
            }
            // pass through
        case KeyValue::KVT_MULTI_SZ:
            {
                for (i = 0; i < rValue.value.vMultiSz.size(); i++) {
                    const std::wstring &wsStr = rValue.value.vMultiSz[i];
                    arrMultiSzBuf.insert(arrMultiSzBuf.end(), wsStr.c_str(), wsStr.c_str() + wsStr.length());
                    arrMultiSzBuf.insert(arrMultiSzBuf.end(), L'\0');
                }
                if (arrMultiSzBuf.empty()) {
                    arrMultiSzBuf.insert(arrMultiSzBuf.end(), L'\0');
                }
                arrMultiSzBuf.insert(arrMultiSzBuf.end(), L'\0');
                
                dwType = REG_MULTI_SZ;
                lpData = (BYTE *)&arrMultiSzBuf[0];
                cbData = arrMultiSzBuf.size() * sizeof(wchar_t);
            }
            // pass through

            {
                nRet = ::RegSetValueEx(m_hKey, sValueName.c_str(), 0, dwType, lpData, cbData);
                if (nRet != ERROR_SUCCESS) {
                    m_nWin32LastError = nRet;
                    nErrCode = EC_FAILED_TO_SET_VALUE;
                }
            }
            break;
        default:
            {
                nErrCode = EC_UNKNOWN_VALUE_TYPE;
            }
            break;
        }
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::GetValue(const std::wstring &sValueName, KeyValue &rValue)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    DWORD dwType;
    BYTE *pData = NULL;
    DWORD cbData = 0;
    wchar_t *pwStr;

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegQueryValueEx(m_hKey, sValueName.c_str(), NULL, NULL, NULL, &cbData);
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_GET_VALUE;
        }
    }

    if (nErrCode == EC_OK) {
        pData = (BYTE *)new DWORD[(cbData + sizeof(DWORD) - 1) / sizeof(DWORD)];
        if (pData == NULL) {
            nErrCode = EC_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegQueryValueEx(m_hKey, sValueName.c_str(), NULL, &dwType, pData, &cbData);
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_GET_VALUE;
        }
    }

    if (nErrCode == EC_OK) {
        switch (dwType) {
        case REG_BINARY:
            {
                rValue.type = KeyValue::KVT_BINARY;
                rValue.value.vBinary = std::vector<BYTE>(pData, pData + cbData);
            }
            break;
        case REG_DWORD:
            {
                rValue.type = KeyValue::KVT_DWORD;
                rValue.value.vDword = *(DWORD *)pData;
            }
            break;
        case REG_SZ:
            {
                rValue.type = KeyValue::KVT_SZ;
                rValue.value.vSz = std::wstring((wchar_t *)pData);
            }
            break;
        case REG_EXPAND_SZ:
            {
                rValue.type = KeyValue::KVT_EXPAND_SZ;
                rValue.value.vExpendSz = std::wstring((wchar_t *)pData);
            }
            break;
        case REG_MULTI_SZ:
            {
                rValue.type = KeyValue::KVT_MULTI_SZ;
                rValue.value.vMultiSz.clear();

                pwStr = (wchar_t *)pData;
                if (*pwStr == L'\0') {
                    pwStr++;
                    rValue.value.vMultiSz.push_back(L"\0");
                }
                while (*pwStr != L'\0') {
                    rValue.value.vMultiSz.push_back(pwStr);
                    pwStr += std::wstring(pwStr).length() + 1;
                }
            }
            break;
        default:
            {
                nErrCode = EC_UNKNOWN_VALUE_TYPE;
            }
            break;
        }
    }

    if (pData != NULL) {
        delete [] (DWORD *)pData;
        pData = NULL;
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::DeleteValue(const std::wstring &sValueName)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegDeleteValue(m_hKey, sValueName.c_str());
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_DELETE_VALUE;
        }
    }

    return nErrCode;
}

bool RegKey::ValueExists(const std::wstring &sValueName)
{
    bool bRet = false;
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    HKEY hkResult = NULL;

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegQueryValueEx(m_hKey, sValueName.c_str(), 0, NULL, NULL, NULL);
        if (nRet == ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            ::RegCloseKey(hkResult);
            bRet = true;
        }
    }

    return bRet;
}

RegKey::ErrCode RegKey::GetValues(std::vector<KeyNameValuePair> &arrKeyValues)
{
    ErrCode nErrCode = EC_OK;
    LONG nRet;
    DWORD cValues = 0;
    DWORD cbMaxValueNameLen = 0;
    DWORD cbMaxValueNameLenRet;
    DWORD cbMaxValueLen = 0;
    wchar_t *pwsValueNameBuf = NULL;
    wchar_t *pwsValueBuf = NULL;
    unsigned int i;

    arrKeyValues.clear();

    if (nErrCode == EC_OK) {
        if (m_hKey == NULL) {
            nErrCode = EC_INVALID_HANDLE;
        }
    }

    if (nErrCode == EC_OK) {
        nRet = ::RegQueryInfoKey(m_hKey, NULL, NULL, NULL, NULL, NULL, NULL, &cValues, &cbMaxValueNameLen, &cbMaxValueLen, NULL, NULL);
        if (nRet != ERROR_SUCCESS) {
            m_nWin32LastError = nRet;
            nErrCode = EC_FAILED_TO_GET_VALUE_INFO;
        } else {
            cbMaxValueNameLen++;
            cbMaxValueLen++;
        }
    }

    if (nErrCode == EC_OK) {
        pwsValueNameBuf = new wchar_t [cbMaxValueNameLen];
        if (pwsValueNameBuf == NULL) {
            nErrCode = EC_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrCode == EC_OK) {
        for (i = 0; i < cValues; i++) {
            memset(pwsValueNameBuf, 0, cbMaxValueNameLen * sizeof(wchar_t));
            cbMaxValueNameLenRet = cbMaxValueNameLen;
            nRet = ::RegEnumValue(m_hKey, i, pwsValueNameBuf, &cbMaxValueNameLenRet, NULL, NULL, NULL, NULL);
            if (nRet != ERROR_SUCCESS) {
                m_nWin32LastError = nRet;
                nErrCode = EC_FAILED_TO_GET_VALUE_INFO;
                break;
            } else {
                KeyValue oValue;
                nRet = GetValue(pwsValueNameBuf, oValue);
                if (nErrCode != EC_OK) {
                    break;
                } else {
                    KeyNameValuePair oKeyNameValuePair;
                    oKeyNameValuePair.sKeyName = pwsValueNameBuf;
                    oKeyNameValuePair.oKeyValue = oValue;
                    arrKeyValues.push_back(oKeyNameValuePair);
                }
            }
        }
    }

    if (pwsValueNameBuf != NULL) {
        delete [] pwsValueNameBuf;
    }

    return nErrCode;
}

RegKey::ErrCode RegKey::ParseKeyPath(const std::wstring &sKeyPath, const RegKey *pParentKey, std::vector<std::wstring> &sPathNodes, HKEY &hParentKey)
{
    ErrCode nErrCode = EC_OK;
    std::wstring sKeyPathTemp = sKeyPath;
    wchar_t seps[]   = L"/\\";
    wchar_t *token;
    
    sPathNodes.clear();
    hParentKey = NULL;

    if (nErrCode == EC_OK) {
        token = wcstok((wchar_t *)sKeyPathTemp.c_str(), seps);
        while (token != NULL) {
            sPathNodes.push_back(std::wstring(token));
            token = wcstok(NULL, seps);
        }

        if (sPathNodes.size() >= 2) {
            if (sPathNodes[0] == L"") {
                sPathNodes.erase(sPathNodes.begin());
            }
        }
    }

    if (nErrCode == EC_OK) {
        if (pParentKey != NULL) {
            hParentKey = pParentKey->GetHandle();
        }
        
        if (hParentKey == NULL) {
            if (sPathNodes.size() >= 1) {
                hParentKey = GetRootHKeyForStr(sPathNodes[0]);
                if (hParentKey != NULL) {
                    sPathNodes.erase(sPathNodes.begin());
                }
            }
        }

        if (hParentKey == NULL) {
            nErrCode = EC_NO_ROOT_KEY;
        }
    }

    return nErrCode;
}

HKEY RegKey::GetRootHKeyForStr(const std::wstring &sPathNode)
{
    HKEY hKey = NULL;
    
    if (sPathNode == L"HKEY_CLASSES_ROOT" || sPathNode == L"HKCR") {
        hKey = HKEY_CLASSES_ROOT;
#if !defined(WINCE)
    } else if (sPathNode == L"HKEY_CURRENT_CONFIG" || sPathNode == L"HKCC") {
        hKey = HKEY_CURRENT_CONFIG;
#endif // #if !defined(WINCE)
    } else if (sPathNode == L"HKEY_CURRENT_USER" || sPathNode == L"HKCU") {
        hKey = HKEY_CURRENT_USER;
    } else if (sPathNode == L"HKEY_LOCAL_MACHINE" || sPathNode == L"HKLM") {
        hKey = HKEY_LOCAL_MACHINE;
    } else if (sPathNode == L"HKEY_USERS" || sPathNode == L"HKU") {
        hKey = HKEY_USERS;
    }

    return hKey;
}

} // namespace TK_Tools {

