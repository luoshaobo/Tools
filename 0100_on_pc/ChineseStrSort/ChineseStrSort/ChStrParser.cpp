#include "UserConfig.h"
#include "ChStrParser.h"
#include "FHTable.h"
#include "ULTable.h"
#include "TSTable.h"
#include "SurnamePinyinTable.h"
#include "UnicodePinyinTable.h"
#include "UnicodePinyinTableWithPlugin.h"
#include "PhrasePinyinTable.h"
#include "PhrasePinyinTableWithPlugin.h"

namespace CSS {

////////////////////////////////////////////////////////////////////////////////
// global
//
static bool IsDigitChar(wchar_t ch)
{
    bool bRet = false;

    if (ch >= L'0' && ch <= '9') {
        bRet = true;
    }

    return bRet;
}

static bool IsLetterChar(wchar_t ch)
{
    bool bRet = false;

    if (ch >= L'a' && ch <= 'z') {
        bRet = true;
    } else if (ch >= L'A' && ch <= 'Z') {
        bRet = true;
    }

    return bRet;
}

static bool IsSymbalChar(wchar_t ch)
{
    bool bRet = false;

    if (ch <= 0x0080) {
        if (!IsDigitChar(ch) && !IsLetterChar(ch)) {
            bRet = true;
        }
    }

    return bRet;
}

static CharCata GetCharCatagory(wchar_t ch)
{
    CharCata cc = EN_CC_HANZI;

    if (IsSymbalChar(ch)) {
        cc = EN_CC_SYMBOL;
    } else if (IsDigitChar(ch)) {
        cc = EN_CC_DIGTAL;
    } else if (IsLetterChar(ch)) {
        cc = EN_CC_LETTER;
    }

    return cc;
}

////////////////////////////////////////////////////////////////////////////////
// ChStrParser
//
FHTable *ChStrParser::m_fhTable = NULL;
ULTable *ChStrParser::m_ulTable = NULL;
TSTable *ChStrParser::m_tsTable = NULL;
SurnamePinyinTable *ChStrParser::m_surnamePinyinTable = NULL;
IUnicodePinyinTable *ChStrParser::m_unicodePinyinTable = NULL;
IPhrasePinyinTable *ChStrParser::m_phrasePinyinTable = NULL;

ChStrParser::ChStrParser(const std::wstring &chinsedStr)
    : m_userConfig(UserConfig::GetInstance())
    , m_inputStr(chinsedStr)
    , m_chStr()
    , m_isChStrBuild(false)
{

}

ChStrParser::~ChStrParser()
{

}

ErrorCode ChStrParser::GetSString(std::wstring &sStr)
{
    ErrorCode ec = EC_OK;
    unsigned int i;
    std::wstring sCode;

    sStr.clear();

    if (ec == EC_OK) {
        ec = BuildChString();
    }

    if (ec == EC_OK) {
        for (i = 0; i < m_chStr.size(); ++i) {
            ec = GetSCode(i, sCode);
            if (ec != EC_OK) {
                break;
            } else {
                sStr += sCode;
            }
        }
    }

    return ec;
}

ErrorCode ChStrParser::GetFullPinyinString(std::wstring &fullPinyinStr)
{
    ErrorCode ec = EC_OK;
    unsigned int i;

    fullPinyinStr.clear();

    if (ec == EC_OK) {
        ec = BuildChString();
    }

    if (ec == EC_OK) {
        for (i = 0; i < m_chStr.size(); ++i) {
            ChChar &chChar = m_chStr[i];
            if (chChar.cata == EN_CC_LETTER) {
                fullPinyinStr += std::wstring(1, chChar.tranformedCode);
            } else if (chChar.cata == EN_CC_HANZI) {
                fullPinyinStr += chChar.pinyinWithoutTone;
            }
        }
    }

    return ec;
}

ErrorCode ChStrParser::GetFullPinyinStringWithTone(std::wstring &fullPinyinStr)
{
    ErrorCode ec = EC_OK;
    unsigned int i;
    CharCata lastCC = EN_CC_OTHER;

    fullPinyinStr.clear();

    if (ec == EC_OK) {
        ec = BuildChString();
    }

    if (ec == EC_OK) {
        for (i = 0; i < m_chStr.size(); ++i) {
            ChChar &chChar = m_chStr[i];

            if (i > 0) {
                if (lastCC != chChar.cata || chChar.cata == EN_CC_HANZI) {
                    if (fullPinyinStr.length() > 0 && fullPinyinStr[fullPinyinStr.length() - 1] != L' ') {
                        fullPinyinStr += std::wstring(L" ");
                    }
                }
            }
            lastCC = chChar.cata;

            if (chChar.cata == EN_CC_HANZI) {
                fullPinyinStr += chChar.pinyinWithoutTone + FormatStr(L"%u", chChar.tone);
            } else {
                fullPinyinStr += std::wstring(1, chChar.tranformedCode);
            }
        }
    }

    return ec;
}

ErrorCode ChStrParser::GetFullPinyinStringArray(std::vector<std::wstring> &fullPinyinStrArray)
{
    ErrorCode ec = EC_OK;
    unsigned int i;
    unsigned int j;
    std::wstring letterStr;
    std::vector<std::wstring> fullPinyinStrs;

    fullPinyinStrArray.clear();

    if (ec == EC_OK) {
        ec = BuildChString();
    }

    if (ec == EC_OK) {
        for (i = 0; i < m_chStr.size(); ++i) {
            ChChar &chChar = m_chStr[i];
            if (chChar.cata == EN_CC_LETTER) {
                letterStr += std::wstring(1, chChar.tranformedCode);
            } else {
                if (letterStr.length() > 0) {
                    fullPinyinStrs.push_back(letterStr);
                    letterStr.clear();
                }
                if (chChar.cata == EN_CC_HANZI) {
                    fullPinyinStrs.push_back(chChar.pinyinWithoutTone);
                }
            }
        }
        if (letterStr.length() > 0) {
            fullPinyinStrs.push_back(letterStr);
            letterStr.clear();
        }
    }

    if (ec == EC_OK) {
        for (i = 0; i < fullPinyinStrs.size(); ++i) {
            std::wstring pinyinStrTmp;
            for (j = i; j < fullPinyinStrs.size(); ++j) {
                pinyinStrTmp += fullPinyinStrs[j];
            }
            fullPinyinStrArray.push_back(pinyinStrTmp);
        }
    }

    return ec;
}

ErrorCode ChStrParser::GetFirstPinyinString(std::wstring &firstPinyinStr)
{
    ErrorCode ec = EC_OK;
    unsigned int i;

    firstPinyinStr.clear();

    if (ec == EC_OK) {
        ec = BuildChString();
    }

    if (ec == EC_OK) {
        for (i = 0; i < m_chStr.size(); ++i) {
            ChChar &chChar = m_chStr[i];
            if (chChar.cata == EN_CC_LETTER) {
                firstPinyinStr += std::wstring(1, chChar.tranformedCode);
            } else if (chChar.cata == EN_CC_HANZI) {
                if (chChar.pinyinWithoutTone.length() > 0) {
                    firstPinyinStr += std::wstring(1, chChar.pinyinWithoutTone[0]);
                }
            }
        }
    }

    return ec;
}

bool ChStrParser::IsInitialized()
{
    bool bRet = false;

    if (m_fhTable != NULL 
        && m_ulTable != NULL
        && m_tsTable != NULL
        && m_surnamePinyinTable != NULL
        && m_unicodePinyinTable != NULL
        && m_phrasePinyinTable != NULL
        ) {
        bRet = true;
    }

    return bRet;
}

ErrorCode ChStrParser::Initialize()
{
    CSS_TIME_LOG_INITIALIZE(t);
    ErrorCode ec = EC_OK;

    CSS_TIME_LOG_RESET(t);
    
    if (ec == EC_OK) {
        if (m_fhTable == NULL) {
            m_fhTable = new FHTable();
            ec = m_fhTable->LoadData();
            if (ec != EC_OK) {
                delete m_fhTable;
                m_fhTable = NULL;
            }
            CSS_TIME_LOG_STEP(t, "m_fhTable->LoadData()");
        }
    }

    if (ec == EC_OK) {
        if (m_ulTable == NULL) {
            m_ulTable = new ULTable();
            ec = m_ulTable->LoadData();
            if (ec != EC_OK) {
                delete m_ulTable;
                m_ulTable = NULL;
            }
            CSS_TIME_LOG_STEP(t, "m_ulTable->LoadData()");
        }
    }

    if (ec == EC_OK) {
        if (m_tsTable == NULL) {
            m_tsTable = new TSTable();
            ec = m_tsTable->LoadData();
            if (ec != EC_OK) {
                delete m_tsTable;
                m_tsTable = NULL;
            }
            CSS_TIME_LOG_STEP(t, "m_tsTable->LoadData()");
        }
    }

    if (ec == EC_OK) {
        if (m_surnamePinyinTable == NULL) {
            m_surnamePinyinTable = new SurnamePinyinTable();
            ec = m_surnamePinyinTable->LoadData();
            if (ec != EC_OK) {
                delete m_surnamePinyinTable;
                m_surnamePinyinTable = NULL;
            }
            CSS_TIME_LOG_STEP(t, "m_surnamePinyinTable->LoadData()");
        }
    }

    if (ec == EC_OK) {
        if (m_unicodePinyinTable == NULL) {
            if (UserConfig::GetInstance().IsFlagSet(USERCONF_UNICODEPINYIN_LOADED_FROM_PLUGIN)) {
                m_unicodePinyinTable = new UnicodePinyinTableWithPlugin();
                ec = m_unicodePinyinTable->LoadData();
                if (ec != EC_OK) {
                    delete m_unicodePinyinTable;
                    m_unicodePinyinTable = NULL;
                }
            } else {
                m_unicodePinyinTable = new UnicodePinyinTable();
                ec = m_unicodePinyinTable->LoadData();
                if (ec != EC_OK) {
                    delete m_unicodePinyinTable;
                    m_unicodePinyinTable = NULL;
                }
            }
            CSS_TIME_LOG_STEP(t, "m_unicodePinyinTable->LoadData()");
        }
    }

    if (ec == EC_OK) {
        if (m_phrasePinyinTable == NULL) {
            if (UserConfig::GetInstance().IsFlagSet(USERCONF_PHRASEPINYIN_LOADED_FROM_PLUGIN)) {
                m_phrasePinyinTable = new PhrasePinyinTableWithPlugin();
                ec = m_phrasePinyinTable->LoadData();
                if (ec != EC_OK) {
                    delete m_phrasePinyinTable;
                    m_phrasePinyinTable = NULL;
                }
            } else {
                m_phrasePinyinTable = new PhrasePinyinTable();
                ec = m_phrasePinyinTable->LoadData();
                if (ec != EC_OK) {
                    delete m_phrasePinyinTable;
                    m_phrasePinyinTable = NULL;
                }
            }
            CSS_TIME_LOG_STEP(t, "m_phrasePinyinTable->LoadData()");
        }
    }

    CSS_TIME_LOG_DEINITIALIZE(t);
    return ec;
}

ErrorCode ChStrParser::Deinitialize()
{
    ErrorCode ec = EC_OK;

    if (m_fhTable != NULL) {
        delete m_fhTable;
        m_fhTable = NULL;
    }

    if (m_ulTable != NULL) {
        delete m_ulTable;
        m_ulTable = NULL;
    }

    if (m_tsTable != NULL) {
        delete m_tsTable;
        m_tsTable = NULL;
    }

    if (m_surnamePinyinTable != NULL) {
        delete m_surnamePinyinTable;
        m_surnamePinyinTable = NULL;
    }

    if (m_unicodePinyinTable != NULL) {
        delete m_unicodePinyinTable;
        m_unicodePinyinTable = NULL;
    }

    if (m_phrasePinyinTable != NULL) {
        delete m_phrasePinyinTable;
        m_phrasePinyinTable = NULL;
    }

    return ec;
}

ErrorCode ChStrParser::GetSCode(unsigned int index, std::wstring &sCode)
{
    ErrorCode ec = EC_OK;

    sCode.clear();

    if (ec == EC_OK) {
        if (index >= m_chStr.size()) {
            ec = EC_INDEX_OUT_OF_SCOPE;
        }
    }

    ChChar &chChar = m_chStr[index];
    if (chChar.cata == EN_CC_SYMBOL || chChar.cata == EN_CC_DIGTAL || chChar.cata == EN_CC_LETTER) {
        FormatStr(sCode, L"~%01X%04X%01X", chChar.cata, chChar.tranformedCode, chChar.attrib);
    } else {
        if (chChar.pinyinWithoutTone.length() != 0) {
            FormatStr(sCode, L"~%01X%s!%01X%04X%01X", 
                chChar.cata, 
                chChar.pinyinWithoutTone.c_str(),
                chChar.tone,
                chChar.tranformedCode, 
                chChar.attrib
            );
        } else {
            chChar.cata = EN_CC_OTHER;
            FormatStr(sCode, L"~%01X%04X%01X", chChar.cata, chChar.tranformedCode, chChar.attrib);
        }
    }

    return ec;
}

ErrorCode ChStrParser::BuildChString()
{
    ErrorCode ec = EC_OK;

    if (ec == EC_OK) {
        if (!m_isChStrBuild) {
            ec = BuildChStringInternal();
            if (ec == EC_OK) {
                m_isChStrBuild = true;
            }
        }
    }

    return ec;
}

ErrorCode ChStrParser::BuildChStringInternal()
{
    ErrorCode ec = EC_OK;
    unsigned int i;
    unsigned int j;
    unsigned int len = m_inputStr.length();
    wchar_t tmpCh;

    if (ec == EC_OK) {
        m_chStr.resize(len);
    }

    if (ec == EC_OK) {
        if (!IsInitialized()) {
            ec = EC_LOOKUP_TABLES_NOT_INITED;
        }
    }

    if (ec == EC_OK) {
        for (i = 0; i < len; ++i) {
            m_chStr[i].cata = GetCharCatagory(m_inputStr[i]);
            m_chStr[i].code = m_inputStr[i];
            m_chStr[i].tranformedCode = m_inputStr[i];
            if (m_chStr[i].cata == EN_CC_HANZI) {
                std::wstring pinyin;
                unsigned int condidateCount = 0;
                ec = m_unicodePinyinTable->Translate(m_inputStr[i], pinyin, m_chStr[i].pinyinCandidateCount);
                if (ec != EC_OK) {
                    break;
                } else {
                    GetPinyinInfo(pinyin, m_chStr[i].pinyinWithoutTone, m_chStr[i].tone);
                }
            }
        }
    }

    if (ec == EC_OK) {
        for (i = 0; i < len; ++i) {
            ec = m_fhTable->Translate(m_chStr[i].tranformedCode, tmpCh);
            if (ec != EC_OK) {
                break;
            } else {
                if (m_chStr[i].tranformedCode != tmpCh) {
                    m_chStr[i].tranformedCode = tmpCh;
                    if (!m_userConfig.IsFlagSet(USERCONF_FULL_CODE_EQUAL_TO_HALF_CODE)) {
                        m_chStr[i].attrib |= EN_CCA_FULLCODE;
                    }
                    m_chStr[i].cata = GetCharCatagory(m_chStr[i].tranformedCode);
                }
            }
        }
    }

    if (ec == EC_OK) {
        for (i = 0; i < len; ++i) {
            ec = m_ulTable->Translate(m_chStr[i].tranformedCode, tmpCh);
            if (ec != EC_OK) {
                break;
            } else {
                if (m_chStr[i].tranformedCode != tmpCh) {
                    m_chStr[i].tranformedCode = tmpCh;
                    m_chStr[i].attrib |= EN_CCA_UPCASE;
                }
            }
        }
    }

    if (ec == EC_OK) {
        for (i = 0; i < len; ++i) {
            ec = m_tsTable->Translate(m_chStr[i].tranformedCode, tmpCh, m_chStr[i].simpleChCandidateCount);
            if (ec != EC_OK) {
                break;
            } else {
                if (m_chStr[i].tranformedCode != tmpCh) {
                    m_chStr[i].tranformedCode = tmpCh;
                    if (!m_userConfig.IsFlagSet(USERCONF_TRAD_CHAR_EQUAL_TO_SIMPLE_CODE)) {
                        m_chStr[i].attrib |= EN_CCA_TRADCH;
                    }
                }
            }
        }
    }

    if (ec == EC_OK) {
        bool isFirstHanzi = true;

        for (i = 0; i < len; ) {
            if (m_chStr[i].cata != EN_CC_HANZI) {
                ++i;
                continue;
            }

            std::vector<std::wstring> pinyins;

            if (m_userConfig.IsFlagSet(USERCONF_SURNAME_FIRST)) {
                if (isFirstHanzi) {
                    isFirstHanzi = false;
                    ec = m_surnamePinyinTable->Translate(m_inputStr.substr(i), pinyins);
                    if (ec != EC_OK) {
                        break;
                    } else {
                        if (pinyins.size() > 0) {
                            for (j = 0; j < pinyins.size(); ++j) {
                                if (m_chStr[i + j].cata == EN_CC_HANZI) {
                                    GetPinyinInfo(pinyins[j], m_chStr[i + j].pinyinWithoutTone, m_chStr[i + j].tone);
                                }
                            }

                            i += pinyins.size();
                            continue;
                        }
                    }
                }
            }
            
            ec = m_phrasePinyinTable->Translate(m_inputStr.substr(i), pinyins);
            if (ec != EC_OK) {
                break;
            } else {
                if (pinyins.size() == 0) {
                    ++i;
                    continue;
                } else {
                    for (j = 0; j < pinyins.size(); ++j) {
                        if (m_chStr[i + j].cata == EN_CC_HANZI) {
                            GetPinyinInfo(pinyins[j], m_chStr[i + j].pinyinWithoutTone, m_chStr[i + j].tone);
                        }
                    }

                    i += pinyins.size();
                    continue;
                }
            }
        }
    }

    return ec;
}

} // namespace CSS {
