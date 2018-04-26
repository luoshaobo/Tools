#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>

#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); } while(0)
#define LOG_GEN_PRINTF(format,...) do { log_printf((std::string("=== LOG_GEN: [%s: %u: %s] ")+format).c_str(), __FILE__, __LINE__, __PRETTY_FUNCTION__, ## __VA_ARGS__); } while(0)
#define log_printf printf

namespace core {
    class BaseScreen {};
    typedef std::function<BaseScreen *()> ScreenCreator;

    template <class ScreenType>
    ScreenCreator GetScreenCreator(ScreenType *)
    {
       ScreenCreator screenCreator = []() {
           return new ScreenType();
       };

       return screenCreator;
    }

    template <class ...ScreenTypes>
    ScreenCreator* GetScreenCreators(ScreenTypes... screenPointers)
    {
       static ScreenCreator screenCreators[] = { GetScreenCreator(screenPointers)... };
       return &screenCreators[0];
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
    ScreenCreator *screenCreators = GetScreenCreators(SCREEN_POINTERS);
    screenCreators[SCREEN_POINTER_COUNT - 1]();

    getchar();
}
