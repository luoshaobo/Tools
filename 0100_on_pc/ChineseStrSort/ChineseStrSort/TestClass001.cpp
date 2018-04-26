#include "TestClass001.h"
#include "UserConfig.h"
#include "ChStrParser.h"

namespace CSS {

TestClass001::TestClass001()
{

}

TestClass001::~TestClass001()
{

}

void TestClass001::DoTest()
{
    CSS_TIME_LOG_INITIALIZE(t);
    bool bRet = true;
    ErrorCode ec = EC_OK;
    std::vector<std::wstring> inputLines;
    unsigned int i;
    std::wstring sStr;
    std::vector<ChStrContext> chStrContexts;

    CSS_TIME_LOG_RESET(t);

    if (bRet) {
        ec = ChStrParser::Initialize();
        if (ec != EC_OK) {
            bRet = false;
        }
    }
    CSS_TIME_LOG_STEP(t, "ChStrParser::Initialize()");

    if (bRet) {
        bRet = GetLinesFromFile(m_inputFilePath, inputLines);
    }
    CSS_TIME_LOG_STEP(t, "GetLinesFromFile()");

    if (bRet) {
        for (i = 0; i < inputLines.size(); ++i) {
            ChStrContext context;
            context.inputStr = inputLines[i];
            ChStrParser csp(context.inputStr);
            ec = csp.GetSString(context.sStr);
            if (ec != EC_OK) {
                bRet = false;
                break;
            }
            ec = csp.GetFullPinyinString(context.fullPinyinStr);
            if (ec != EC_OK) {
                bRet = false;
                break;
            }
            ec = csp.GetFullPinyinStringArray(context.fullPinyinStrArray);
            if (ec != EC_OK) {
                bRet = false;
                break;
            }
            ec = csp.GetFirstPinyinString(context.firstPinyinStr);
            if (ec != EC_OK) {
                bRet = false;
                break;
            }
            chStrContexts.push_back(context);
        }
    }
    CSS_TIME_LOG_STEP(t, "all of csp.GetSString()");

    if (bRet) {
        sort(chStrContexts.begin(), chStrContexts.end(), GreaterBySStr);
        CSS_TIME_LOG_STEP(t, "sort()");
        //OutputResult(chStrContexts);
        //CSS_TIME_LOG_STEP(t, "output result");
    }

    while (true) {
        wchar_t lineBuf[1024];
        wchar_t *line;
        std::wstring lineStr;
        std::vector<ChStrContext> result;

        printf("=== Search string (hanzi, full pinyin, or first pinyin)[! => show all; # => exit]: ");
        line = fgetws(lineBuf, TK_ARR_LEN(lineBuf), stdin);
        lineStr = TrimAll(line);
        if (lineStr.length() == 0) {
            continue;
        }

        if (lineStr == L"#") {
            break;
        }

        if (lineStr == L"!") {
            CSS_TIME_LOG_RESET(t);
            OutputResult(chStrContexts);
            CSS_TIME_LOG_STEP(t, "output result");
            continue;
        }
        
        CSS_TIME_LOG_RESET(t);
        for (i = 0; i < inputLines.size(); ++i) {
            ChStrContext &context = chStrContexts[i];
            if (MatchByAll(context, lineStr)) {
                result.push_back(context);
            }
        }
        CSS_TIME_LOG_STEP(t, "search");

        for (i = 0; i < result.size(); ++i) {
            ChStrContext &context = result[i];
            wprintf(L"%s\n", context.inputStr.c_str());
        }
    }

    CSS_TIME_LOG_DEINITIALIZE(t);
}

void TestClass001::SetUserConfigFlags(unsigned int userConfigFlags)
{
    UserConfig::GetInstance().SetFlag(userConfigFlags);
}

void TestClass001::ClearUserConfigFlags(unsigned int userConfigFlags)
{
    UserConfig::GetInstance().ClearFlag(userConfigFlags);
}

void TestClass001::OutputResult(std::vector<ChStrContext> &chStrContexts)
{
    unsigned int i;

    for (i = 0; i < chStrContexts.size(); ++i) {
        ChStrContext &context = chStrContexts[i];
        
        wprintf(L"%s\n", context.inputStr.c_str());
        //wprintf(L"%s <= %s\n", context.inputStr.c_str(), context.sStr.c_str());
        //wprintf(L"%s <= %s\n", context.inputStr.c_str(), context.fullPinyinStr.c_str());
        //wprintf(L"%s <= %s\n", context.inputStr.c_str(), context.firstPinyinStr.c_str());
    }
}

bool TestClass001::GreaterBySStr(const ChStrContext &context1, const ChStrContext &context2)
{
    const std::wstring &k1 = context1.sStr;
    const std::wstring &v1 = context1.inputStr;
    const std::wstring &k2 = context2.sStr;
    const std::wstring &v2 = context2.inputStr;

    int r = wcscmp(k1.c_str(), k2.c_str());
    if (r == 0) {
        r = wcscmp(v1.c_str(), v2.c_str());
    }

    return r < 0;
}

bool TestClass001::MatchByAll(const ChStrContext &context, const std::wstring &key)
{
    bool isMatched = false;
    size_t pos;
    unsigned int i;
    unsigned int j;

    do {
        pos = context.firstPinyinStr.find(key);
        if (pos != std::wstring::npos) {
            isMatched = true;
            break;
        }

        for (i = 0; i < context.fullPinyinStrArray.size(); ++i) {
            const std::wstring &pinyin = context.fullPinyinStrArray[i];
            if (CompareCaseLeftLen(key, pinyin) == 0) {
                std::wstring leftPartStr = pinyin.substr(key.length());
                if (leftPartStr.length() == 0) {
                    isMatched = true;
                    break;
                }
                for (j = i + 1; j < context.fullPinyinStrArray.size(); ++j) {
                    const std::wstring &pinyin2 = context.fullPinyinStrArray[j];
                    if (pinyin2 == leftPartStr) {
                        isMatched = true;
                        break;
                    }
                }
            }
        }
        if (isMatched) {
            break;
        }

        pos = context.inputStr.find(key);
        if (pos != std::wstring::npos) {
            isMatched = true;
            break;
        }
    } while (false);

    return isMatched;
}

} // namespace CSS {
