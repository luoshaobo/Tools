#ifndef TESTCLASS001_H_78234823409238489234892349230942358495049593458439689456
#define TESTCLASS001_H_78234823409238489234892349230942358495049593458439689456

#include "CSSGlobal.h"

namespace CSS {

class TestClass001
{
public:
    struct ChStrContext {
        std::wstring inputStr;
        std::wstring sStr;
        std::wstring fullPinyinStr;
        std::vector<std::wstring> fullPinyinStrArray;
        std::wstring firstPinyinStr;
    };

public:
    TestClass001();
    ~TestClass001();

public:
    void DoTest();

public:
    void SetInputFilePath(const std::string &inputFilePath) { m_inputFilePath = inputFilePath; }
    void SetUserConfigFlags(unsigned int userConfigFlags);
    void ClearUserConfigFlags(unsigned int userConfigFlags);

private:
    static void OutputResult(std::vector<ChStrContext> &chStrContexts);
    static bool GreaterBySStr(const ChStrContext &context1, const ChStrContext &context2);
    static bool MatchByAll(const ChStrContext &context, const std::wstring &key);

private:
    std::string m_inputFilePath;
};

} // namespace CSS {

#endif // #ifndef TESTCLASS001_H_78234823409238489234892349230942358495049593458439689456
