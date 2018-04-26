#include "TestClass002.h"
#include "UserConfig.h"
#include "ChStrParser.h"

namespace CSS {

TestClass002::TestClass002()
{

}

TestClass002::~TestClass002()
{

}

void TestClass002::DoTest()
{
    CSS_TIME_LOG_INITIALIZE(t);
    bool bRet = true;
    ErrorCode ec = EC_OK;
    std::vector<std::wstring> inputLines;
    std::wstring sStr;
    ChStrContext context;

    CSS_TIME_LOG_RESET(t);

    if (bRet) {
        ec = ChStrParser::Initialize();
        if (ec != EC_OK) {
            bRet = false;
        }
    }
    CSS_TIME_LOG_STEP(t, "ChStrParser::Initialize()");

    while (true) {
        wchar_t lineBuf[1024];
        wchar_t *line;
        std::wstring lineStr;
        std::vector<ChStrContext> result;

        printf("=== Chinese string [# to exit]: ");
        line = fgetws(lineBuf, TK_ARR_LEN(lineBuf), stdin);
        lineStr = TrimAll(line);
        if (lineStr.length() == 0) {
            continue;
        }

        if (lineStr == L"#") {
            break;
        }

        CSS_TIME_LOG_RESET(t);
        ChStrParser csp(lineStr);
        context.inputStr = lineStr;
        ec = csp.GetFullPinyinStringWithTone(context.fullPinyinStrWithTone);
        if (ec != EC_OK) {
            bRet = false;
            break;
        }
        CSS_TIME_LOG_STEP(t, "search");

        OutputResult(context);
    }

    CSS_TIME_LOG_DEINITIALIZE(t);
}

void TestClass002::SetUserConfigFlags(unsigned int userConfigFlags)
{
    UserConfig::GetInstance().SetFlag(userConfigFlags);
}

void TestClass002::ClearUserConfigFlags(unsigned int userConfigFlags)
{
    UserConfig::GetInstance().ClearFlag(userConfigFlags);
}

void TestClass002::OutputResult(ChStrContext &context)
{       
    wprintf(L"%s <= %s\n", context.inputStr.c_str(), context.fullPinyinStrWithTone.c_str());
}

} // namespace CSS {
