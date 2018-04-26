#include "UnicodePinyinTable.h"

namespace CSS {

static const char *CSS_UNICODEPINYINTABLE_FILE_PATH = "data/UnicodePinyinTable.txt";

UnicodePinyinTable::UnicodePinyinTable()
    : m_unicodePinyinMap()
    , m_dataFilePath(CSS_UNICODEPINYINTABLE_FILE_PATH)
    , m_curLineNo(0)
{

}

UnicodePinyinTable::~UnicodePinyinTable()
{

}

ErrorCode UnicodePinyinTable::Translate(wchar_t chineseChar, std::wstring &pinyin, unsigned int &condidateCount)
{
    ErrorCode ec = EC_OK;
    UnicodePinyinMap::iterator it;

    pinyin.clear();
    condidateCount = 0;

    it = m_unicodePinyinMap.find(chineseChar);
    if (it != m_unicodePinyinMap.end()) {
        std::vector<std::wstring> &pinyins = it->second;
        pinyin = pinyins[0];
        condidateCount = pinyins.size();
    }

    return ec;
}

ErrorCode UnicodePinyinTable::LoadData()
{
    ErrorCode ec = EC_OK;
    FILE *fp = NULL;
    wchar_t lineBuf[BUF_LEN_MAX];
    wchar_t *returnedLine;
    std::wstring trimedLine;

    if (ec == EC_OK) {
        fp = fopen(m_dataFilePath.c_str(), "rb");
        if (fp == NULL) {
            ec = EC_FAILED_TO_OPEN_UNICODEPINYINTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        size_t readCount = fread(lineBuf, 1, 2, fp);
        if (readCount != 2 || lineBuf[0] != 0xFEFF) {
            ec = EC_INVALID_FORMAT_OF_UNICODEPINYINTABLE_FILE;
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
                    ec = EC_FAILED_TO_READ_UNICODEPINYINTABLE_FILE;
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

ErrorCode UnicodePinyinTable::ParseLine(const std::wstring &line)
{
    ErrorCode ec = EC_OK;
    std::vector<std::wstring> segments;
    unsigned int i;

    if (ec == EC_OK) {
        segments = SplitString(line);
        if (segments.size() < 2) {
            ec = EC_INVALID_LINE_IN_UNICODEPINYINTABLE_FILE;
        } else {
            if (segments[0].length() != 4 || !IsHexDigitString(segments[0])) {
                ec = EC_INVALID_LINE_IN_UNICODEPINYINTABLE_FILE;
            }

            for (i = 1; i < segments.size(); ++i) {
                if (!IsPinyinStr(segments[i])) {
                    ec = EC_INVALID_LINE_IN_UNICODEPINYINTABLE_FILE;
                }
            }
        }
    }

    if (ec == EC_OK) {
        wchar_t key = static_cast<wchar_t>(StrToUL(segments[0], 16));
        std::vector<std::wstring> value;
        for (i = 1; i < segments.size(); ++i) {
            value.push_back(segments[i]);
        }
        if (value.size() > 0) {
            m_unicodePinyinMap[key] = value;
        }
    }

    if (ec != EC_OK) {
        //CSS_ERROR("%s(): failed to parse \"%s\"! [LastLineNo=%u]", CSS_CURFUNC, m_dataFilePath.c_str(), m_curLineNo);
        ec = EC_OK;
    }

    return ec;
}

ErrorCode UnicodePinyinTable::UnloadData()
{
    ErrorCode ec = EC_OK;

    // do nothing

    return ec;
}

ErrorCode UnicodePinyinTable::GenCppCode(std::string &cppCodeContent)
{
    ErrorCode ec = EC_OK;
    unsigned int i, j;
    UnicodePinyinMap::iterator it;

    cppCodeContent.clear();
    
    if (ec == EC_OK) {
        for (i = 0; i < 0x10000; ++i) {
            wchar_t ch = (wchar_t)i;
            it = m_unicodePinyinMap.find(ch);
            if (it != m_unicodePinyinMap.end()) {
                std::vector<std::wstring> &value = it->second;
                if (value.size() > 0) {
                    //
                    // for pinyin_*_n[]
                    //
                    for (j = 0; j < value.size(); ++j) {
                        cppCodeContent += FormatStr("static const wchar_t pinyin_%s_%02X[] = { %s };%s", 
                            BuildSymbolStrForStr(std::wstring(1, ch)).c_str(),
                            j,
                            BuildContnetStrForStr(value[j]).c_str(),
                            CSS_NEWLINE
                        );
                    }

                    //
                    // for pinyins_items_*[]
                    //
                    cppCodeContent += FormatStr("static const wchar_t *pinyins_items_%s[] = {%s", 
                        BuildSymbolStrForStr(std::wstring(1, ch)).c_str(),
                        CSS_NEWLINE
                    );
                    for (j = 0; j < value.size(); ++j) {
                        cppCodeContent += FormatStr("    pinyin_%s_%02X,%s", 
                            BuildSymbolStrForStr(std::wstring(1, ch)).c_str(),
                            j,
                            CSS_NEWLINE
                        );
                    }
                    cppCodeContent += FormatStr("};%s", 
                        CSS_NEWLINE
                    );

                    //
                    // for pinyins_*[]
                    //
                    cppCodeContent += FormatStr("static const CharPinyins pinyins_%s = {%s", 
                        BuildSymbolStrForStr(std::wstring(1, ch)).c_str(),
                        CSS_NEWLINE
                    );
                    cppCodeContent += FormatStr("    %lu,%s", 
                        value.size(),
                        CSS_NEWLINE
                    );
                    cppCodeContent += FormatStr("    pinyins_items_%s%s", 
                        BuildSymbolStrForStr(std::wstring(1, ch)).c_str(),
                        CSS_NEWLINE
                    );
                    cppCodeContent += FormatStr("};%s", 
                        CSS_NEWLINE
                    );
                }
            }
        }

        //
        // for map_root_items[]
        //
        cppCodeContent += FormatStr("static const PinyinMapItem map_root_items[] = {%s", 
            CSS_NEWLINE
        );
        for (i = 0; i < 0x10000; ++i) {
            wchar_t ch = (wchar_t)i;
            bool hasPinyin = false;
            it = m_unicodePinyinMap.find(ch);
            if (it != m_unicodePinyinMap.end()) {
                std::vector<std::wstring> &value = it->second;
                if (value.size() > 0) {
                    cppCodeContent += FormatStr("    { 0x%s, &pinyins_%s },%s", 
                        BuildSymbolStrForStr(std::wstring(1, ch)).c_str(),
                        BuildSymbolStrForStr(std::wstring(1, ch)).c_str(),
                        CSS_NEWLINE
                    );
                    hasPinyin = true;
                }
            }
            if (!hasPinyin) {
                cppCodeContent += FormatStr("    { 0x%s, NULL },%s", 
                    BuildSymbolStrForStr(std::wstring(1, ch)).c_str(),
                    CSS_NEWLINE
                );
            }
        }
        cppCodeContent += FormatStr("};%s", 
            CSS_NEWLINE
        );

        //
        // for map_root
        //
        cppCodeContent += FormatStr("static const PinyinMap map_root = {%s", 
            CSS_NEWLINE
        );
        cppCodeContent += FormatStr("    %lu,%s", 
            0x10000,
            CSS_NEWLINE
        );
        cppCodeContent += FormatStr("    map_root_items%s", 
            CSS_NEWLINE
        );
        cppCodeContent += FormatStr("};%s", 
            CSS_NEWLINE
        );
    }

    return ec;
}

std::string UnicodePinyinTable::BuildSymbolStrForStr(const std::wstring &s)
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

std::string UnicodePinyinTable::BuildContnetStrForStr(const std::wstring &s)
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

} // namespace CSS {
