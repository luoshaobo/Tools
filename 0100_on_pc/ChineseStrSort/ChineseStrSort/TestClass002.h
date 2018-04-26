#ifndef TESTCLASS002_H_723841092478124782376534860945097089378234723578348534349
#define TESTCLASS002_H_723841092478124782376534860945097089378234723578348534349

#include "CSSGlobal.h"

namespace CSS {

class TestClass002
{
public:
    struct ChStrContext {
        std::wstring inputStr;
        std::wstring sStr;
        std::wstring fullPinyinStrWithTone;
    };

public:
    TestClass002();
    ~TestClass002();

public:
    void DoTest();

public:
    void SetUserConfigFlags(unsigned int userConfigFlags);
    void ClearUserConfigFlags(unsigned int userConfigFlags);

private:
    void OutputResult(ChStrContext &context);
};

} // namespace CSS {

#endif // #ifndef TESTCLASS002_H_723841092478124782376534860945097089378234723578348534349
