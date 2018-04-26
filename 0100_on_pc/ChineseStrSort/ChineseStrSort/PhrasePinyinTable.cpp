#include "PhrasePinyinTable.h"

namespace CSS {

static const char *CSS_PHRASEPINYINTABLE_FILE_PATH = "data/PhrasePinyinTable.txt";

PhrasePinyinTable::PhrasePinyinTable()
    : m_phrasePinyinMap()
    , m_dataFilePath(CSS_PHRASEPINYINTABLE_FILE_PATH)
    , m_curLineNo(0)
{

}

PhrasePinyinTable::~PhrasePinyinTable()
{

}

ErrorCode PhrasePinyinTable::Translate(const std::wstring &chineseStr, std::vector<std::wstring> &pingyins)
{
    ErrorCode ec = EC_OK;
    PhrasePinyinMapValue *value = NULL;
    unsigned int firstMatchedLen = 0;

    pingyins.clear();

    if (ec == EC_OK) {
        ec = FindItem_Partial(chineseStr, value, firstMatchedLen);
    }

    if (ec == EC_OK) {
        if (firstMatchedLen > 0 && value != NULL && value->pinyins != NULL) {
            pingyins = *value->pinyins;
        }
    }

    return ec;
}

ErrorCode PhrasePinyinTable::LoadData()
{
    ErrorCode ec = EC_OK;
    FILE *fp = NULL;
    wchar_t lineBuf[BUF_LEN_MAX];
    wchar_t *returnedLine;
    std::wstring trimedLine;

    if (ec == EC_OK) {
        fp = fopen(m_dataFilePath.c_str(), "rb");
        if (fp == NULL) {
            ec = EC_FAILED_TO_OPEN_PHRASEPINYINTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        size_t readCount = fread(lineBuf, 1, 2, fp);
        if (readCount != 2 || lineBuf[0] != 0xFEFF) {
            ec = EC_INVALID_FORMAT_OF_PHRASEPINYINTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        while (true) {
            m_curLineNo++;
            returnedLine = fgetws(lineBuf, BUF_LEN_MAX, fp);
            if (returnedLine == NULL) {
                if (feof(fp)) {
                    // do nothing
                } else {
                    ec = EC_FAILED_TO_READ_PHRASEPINYINTABLE_FILE;
                }
                break;
            } else {
                trimedLine = TrimAll(returnedLine);
                if (trimedLine.length() == 0) {
                    continue;
                }
                ec = ParseLine(returnedLine);
                if (ec != EC_OK) {
                    break;
                }
            }
        }
    }

    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }

    if (ec != EC_OK) {
        CSS_ERROR("%s(): failed to parse \"%s\"! [LastLineNo=%u]", CSS_CURFUNC, m_dataFilePath.c_str(), m_curLineNo);
    }
    return ec;
}

ErrorCode PhrasePinyinTable::ParseLine(const std::wstring &line)
{
    ErrorCode ec = EC_OK;
    std::vector<std::wstring> segments;
    unsigned int i;

    if (ec == EC_OK) {
        segments = SplitString(line);
        if (segments.size() < 2) {
            ec = EC_INVALID_LINE_IN_PHRASEPINYINTABLE_FILE;
        } else {
            if (segments.size() < 2) {
                ec = EC_INVALID_LINE_IN_PHRASEPINYINTABLE_FILE;
            }

            for (i = 1; i < segments.size(); ++i) {
                if (!IsPinyinStr(segments[i])) {
                    ec = EC_INVALID_LINE_IN_PHRASEPINYINTABLE_FILE;
                }
            }
        }
    }

    if (ec == EC_OK) {
        std::vector<std::wstring> pinyins;
        for (i = 1; i < segments.size(); ++i) {
            pinyins.push_back(segments[i]);
        }
        ec = InsertItem(segments[0], pinyins);
    }

    if (ec != EC_OK) {
        //CSS_ERROR("%s(): failed to parse \"%s\"! [LastLineNo=%u]", CSS_CURFUNC, m_dataFilePath.c_str(), m_curLineNo);
        ec = EC_OK;
    }

    return ec;
}

ErrorCode PhrasePinyinTable::InsertItem(const std::wstring &keys, const std::vector<std::wstring> &pinyins)
{
    ErrorCode ec = EC_OK;
    PhrasePinyinMapValue *value = NULL;

    if (ec == EC_OK) {
        ec = FindItem_CreateIfNotExist(keys, value);
    }

    if (ec == EC_OK) {
        if (value == NULL) {
            ec = EC_FAILED;
        }
    }

    if (ec == EC_OK) {
        value->pinyins = new std::vector<std::wstring>();
        if (value->pinyins == NULL) {
            ec = EC_NO_MEM_TO_NEW;
        }
    }

    if (ec == EC_OK) {
        *value->pinyins = pinyins;
    }

    return ec;
}

ErrorCode PhrasePinyinTable::FindItem_CreateIfNotExist(const std::wstring &keys, PhrasePinyinMapValue *&value)
{
    ErrorCode ec = EC_OK;
    PhrasePinyinMap *curMap = &m_phrasePinyinMap;
    PhrasePinyinMapValue *curValue = NULL;
    PhrasePinyinMap::iterator it;
    PhrasePinyinMapValue *tmpValue;
    unsigned int i;
    wchar_t ch;
    bool lastCh;

    value = NULL;

    if (ec == EC_OK) {
        for (i = 0; i < keys.length(); ++i) {
            lastCh = (i == keys.length() - 1);
            ch = keys[i];
            it = curMap->find(ch);
            if (it != curMap->end()) {
                tmpValue = &(it->second);
                if (tmpValue->subMap == NULL) {
                    if (!lastCh) {
                        tmpValue->subMap = new PhrasePinyinMap();
                        if (tmpValue->subMap == NULL) {
                            ec = EC_NO_MEM_TO_NEW;
                            break;
                        }
                    }
                }
                curValue = tmpValue;
                curMap = tmpValue->subMap;
            } else {
                PhrasePinyinMapValue v;
                if (!lastCh) {
                    v.subMap = new PhrasePinyinMap();
                    if (v.subMap == NULL) {
                        ec = EC_NO_MEM_TO_NEW;
                        break;
                    }
                }
                (*curMap)[ch] = v;
                curValue = &(*curMap)[ch];
                curMap = v.subMap;
            }
        }
    }

    if (ec == EC_OK) {
        value = curValue;
    }

    return ec;
}

ErrorCode PhrasePinyinTable::FindItem_Partial(const std::wstring &keys, PhrasePinyinMapValue *&value, unsigned int &firstMatchedLen)
{
    ErrorCode ec = EC_OK;
    PhrasePinyinMap *curMap = &m_phrasePinyinMap;
    PhrasePinyinMapValue *curValue = NULL;
    PhrasePinyinMap::iterator it;
    unsigned int i;
    wchar_t ch;

    value = NULL;
    firstMatchedLen = 0;

    if (ec == EC_OK) {
        for (i = 0; i < keys.length(); ++i) {
            if (curMap == NULL) {
                break;
            }

            ch = keys[i];
            it = curMap->find(ch);
            if (it != curMap->end()) {
                curValue = &it->second;
                if (curValue->pinyins != NULL) {
                    firstMatchedLen = i + 1;
                    value = curValue;
                }
                curMap = curValue->subMap;
            } else {
                break;
            }
        }
    }

    return ec;
}

ErrorCode PhrasePinyinTable::GenCppCode(std::string &cppCodeContent)
{
    ErrorCode ec = EC_OK;
    PhrasePinyinMapValue mapValue;

    if (ec == EC_OK) {
        mapValue.pinyins = NULL;
        mapValue.subMap = &m_phrasePinyinMap;
        ec = GenCppCode_Node(cppCodeContent, mapValue, 0, L"");
    }

    return ec;
}

ErrorCode PhrasePinyinTable::GenCppCode_Node(std::string &cppCodeContent, const PhrasePinyinMapValue &mapValue, unsigned int level, const std::wstring &currentStr)
{
    ErrorCode ec = EC_OK;
    PhrasePinyinMap *map = mapValue.subMap;
    PhrasePinyinMap::iterator it;
    std::vector<wchar_t> childKeys;
    unsigned int i;

    if (ec == EC_OK) {
        if (map != NULL) {
            for (it = map->begin(); it != map->end(); ++it) {
                childKeys.push_back(it->first);
            }
            std::sort(childKeys.begin(), childKeys.end());
        }
    }

    if (ec == EC_OK) {
        for (i = 0; i < childKeys.size(); ++i) {
            wchar_t &key = childKeys[i];
            ec = GenCppCode_Node(cppCodeContent, (*map)[key], level + 1, currentStr + std::wstring(1, key));
            if (ec != EC_OK) {
                break;
            }
        }
    }
    
    if (ec == EC_OK) {
        //
        // for str_*[]
        //
        if (mapValue.pinyins != NULL && (*mapValue.pinyins).size() > 0) {
            cppCodeContent += FormatStr("static const wchar_t *pinyins_%s[] = {%s", 
                BuildSymbolStrForStr(currentStr).c_str(),
                CSS_NEWLINE
            );
            for (i = 0; i < (*mapValue.pinyins).size(); ++i) {
                cppCodeContent += FormatStr("    L\"%S\",%s", 
                    (*mapValue.pinyins)[i].c_str(),
                    CSS_NEWLINE
                );
            }
            cppCodeContent += FormatStr("};%s", 
                CSS_NEWLINE
            );
        }

        //
        // for map_*[]
        //
        if (childKeys.size() > 0) {
            cppCodeContent += FormatStr("static const PhrasePinyinMapItem map_%s[] = {%s", 
                BuildSymbolStrForStr(currentStr).c_str(),
                CSS_NEWLINE
            );
            if (level == 0) {
                for (i = 0; i < 0x10000; ++i) {
                    wchar_t ch = (wchar_t)i;
                    it = map->find(ch);
                    if (it == map->end()) {
                        cppCodeContent += FormatStr("    { 0x%04X, NULL },%s", 
                            ch,
                            CSS_NEWLINE
                        );
                    } else {
                        cppCodeContent += FormatStr("    { 0x%04X, &node_%s },%s", 
                            ch,
                            BuildSymbolStrForStr(currentStr + std::wstring(1, ch)).c_str(),
                            CSS_NEWLINE
                        );
                    }
                }
            } else {
                for (i = 0; i < childKeys.size(); ++i) {
                    wchar_t &key = childKeys[i];
                    cppCodeContent += FormatStr("    { 0x%04X, &node_%s },%s", 
                        key,
                        BuildSymbolStrForStr(currentStr + std::wstring(1, key)).c_str(),
                        CSS_NEWLINE
                    );
                }
            }
            cppCodeContent += FormatStr("};%s", 
                CSS_NEWLINE
            );
        }

        //
        // for nod_*
        //
        cppCodeContent += FormatStr("static const PhrasePinyinNode node_%s = {%s", 
            BuildSymbolStrForStr(currentStr).c_str(),
            CSS_NEWLINE
        );
        if (mapValue.pinyins != NULL) {
            cppCodeContent += FormatStr("    %lu,%s", 
                (*mapValue.pinyins).size(),
                CSS_NEWLINE
            );
            cppCodeContent += FormatStr("    pinyins_%s,%s", 
                BuildSymbolStrForStr(currentStr).c_str(),
                CSS_NEWLINE
            );
        } else {
            cppCodeContent += FormatStr("    %lu,%s", 
                0,
                CSS_NEWLINE
            );
            cppCodeContent += FormatStr("    NULL,%s", 
                CSS_NEWLINE
            );
        }
        cppCodeContent += FormatStr("    %lu,%s", 
            (level == 0 ? 0x10000 : childKeys.size()),
            CSS_NEWLINE
        );
        if (childKeys.size() > 0) {
            cppCodeContent += FormatStr("    map_%s,%s", 
                BuildSymbolStrForStr(currentStr).c_str(),
                CSS_NEWLINE
            );
        } else {
            cppCodeContent += FormatStr("    NULL,%s", 
                CSS_NEWLINE
            );
        }
        cppCodeContent += FormatStr("};%s", 
            CSS_NEWLINE
        );
    }

    return ec;
}

std::string PhrasePinyinTable::BuildSymbolStrForStr(const std::wstring &s)
{
    std::string resultStr;
    unsigned int i;

    for (i = 0; i < s.length(); ++i) {
        if (resultStr.length() != 0) {
            resultStr += "_";
        }

        resultStr += FormatStr("%04X", s[i]);
    }

    if (resultStr.length() == 0) {
        resultStr = "root";
    }

    return resultStr;
}

std::string PhrasePinyinTable::BuildContnetStrForStr(const std::wstring &s)
{
    std::string resultStr;
    unsigned int i;

    for (i = 0; i < s.length(); ++i) {
        wchar_t ch = s[i];
        resultStr += FormatStr("0x%04X, ", ch);
    }
    resultStr += FormatStr("0x0000");

    return resultStr;
}

ErrorCode PhrasePinyinTable::UnloadData()
{
    ErrorCode ec = EC_OK;

    // TODO: the data allocated dynamically must be freed.

    return ec;
}

} // namespace CSS {
