#include <stdio.h>
#include <stdlib.h>
#include <string>

#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); } while(0)
#define LOG_GEN_PRINTF(format,...) do { log_printf((std::string("=== LOG_GEN: [%s: %u: %s] ")+format).c_str(), __FILE__, __LINE__, __PRETTY_FUNCTION__, ## __VA_ARGS__); } while(0)
#define log_printf printf

namespace core
{
    //
    // TypeCollection: to wrap the List argument
    //
    template <typename... List>
    struct TypeCollection
    {
        typedef TypeCollection<List...> this_type;
    };

    //
    // Nth: to be used by NthType internally
    //
    template <unsigned N, typename... List>
    struct Nth;

    template <unsigned N, typename Head, typename... Tail>
    struct Nth<N, Head, Tail...>
    {
        typedef typename Nth<N-1, Tail...>::type type;
    };

    template <typename Head, typename... Tail>
    struct Nth<0, Head, Tail...>
    {
        typedef Head type;
    };

    //
    // NthType
    //
    template <unsigned N, typename... List>
    struct NthType;

    template <unsigned N, typename... List>
    struct NthType<N, TypeCollection<List...>>
    {
        typedef typename Nth<N, List...>::type type;
    };

    //
    // IndexOf: to be used by IndexInCollection internally
    //
    template <typename Type, typename... List>
    struct IndexOf;

    template <typename Type, typename Head, typename... Tail>
    struct IndexOf<Type, Head, Tail...>
    {
        enum { value = 1 + IndexOf<Type, Tail...>::value };
    };

    template <typename Type, typename... Tail>
    struct IndexOf<Type, Type, Tail...>
    {
        enum { value = 0 };
    };

    //
    // IndexInCollection
    //
    template <typename Type, typename... List>
    struct IndexInCollection;

    template <typename Type, typename... List>
    struct IndexInCollection<Type, TypeCollection<List...>>
    {
        enum { value = IndexOf<Type, List...>::value };
    };

    //
    // SizeOfCollection
    //
    template <typename... List>
    struct SizeOfCollection;

    template <typename... List>
    struct SizeOfCollection<TypeCollection<List...>>
    {
        enum { value = sizeof...(List) };
    };
}

using namespace core;

#define MY_CLASS(index) \
struct MyClass ## index \
{ \
    MyClass ## index() \
    { \
        LOG_GEN(); \
    } \
};

#include "mytypes.h"

int main(int argc, char *argv[])
{
    int nSize;
    int nIndex;

    nSize = SizeOfCollection<MyTypeCollection>::value;
    LOG_GEN_PRINTF("nSize=%d\n", nSize);

    nIndex = IndexInCollection<MyClass002, MyTypeCollection>::value;
    LOG_GEN_PRINTF("nIndex=%d\n", nIndex);

    NthType<SizeOfCollection<MyTypeCollection>::value - 1, MyTypeCollection>::type instance;

    getchar();
    return 0;
}
