#include <stdio.h>
#include <stdlib.h>
#include <string>

#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); } while(0)
#define LOG_GEN_PRINTF(format,...) do { log_printf((std::string("=== LOG_GEN: [%s: %u: %s] ")+format).c_str(), __FILE__, __LINE__, __PRETTY_FUNCTION__, ## __VA_ARGS__); } while(0)
#define log_printf printf

namespace core {
    class BaseScreen {};
    typedef BaseScreen *(*ScreenCreatorFunc)();

    template <class ScreenType>
    BaseScreen *CreateScreen()
    {
        return new ScreenType;
    }

    extern ScreenCreatorFunc ScreenCreatorFuncArray[];
    extern unsigned int ScreenCreatorFuncCount;

    ScreenCreatorFunc GetScreenCreatorFunc(unsigned int nIndex)
    {
       ScreenCreatorFunc screenCreatorFunc = NULL;
       if (nIndex < ScreenCreatorFuncCount) {
           screenCreatorFunc = ScreenCreatorFuncArray[nIndex];
       }
       return screenCreatorFunc;
    }
}

using namespace core;

#define MY_SCREEN(index) \
class MyScreen ## index : public BaseScreen \
{ \
public: \
    MyScreen ## index() \
    { \
        LOG_GEN(); \
    } \
};

#include "myscreens.h"

int main()
{
    ScreenCreatorFunc screenCreatorFunc = GetScreenCreatorFunc(ScreenCreatorFuncCount - 1);
    if (screenCreatorFunc != NULL) {
        screenCreatorFunc();
    }

    getchar();
}
