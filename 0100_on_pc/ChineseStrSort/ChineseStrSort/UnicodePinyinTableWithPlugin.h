#ifndef UNICODEPINYINTABLEWITHPLUGIN_H_76127493492378294723489235784785783485348
#define UNICODEPINYINTABLEWITHPLUGIN_H_76127493492378294723489235784785783485348

#include "CSSGlobal.h"
#include "IUnicodePinyinTable.h"

namespace CSS {

class UnicodePinyinTableWithPlugin : public IUnicodePinyinTable
{
private:
    struct CharPinyins {
        unsigned int itemCount;
        const wchar_t **items;
    };

    struct PinyinMapItem {
        wchar_t ch;
        const CharPinyins *charPinyins;
    };

    struct PinyinMap {
        unsigned int itemCount;
        const PinyinMapItem *items;
    };

public:
    UnicodePinyinTableWithPlugin();
    ~UnicodePinyinTableWithPlugin();

public:
    ErrorCode LoadData();
    ErrorCode UnloadData();
    ErrorCode Translate(wchar_t chineseChar, std::wstring &pinyin, unsigned int &condidateCount);

private:
    

private:
    const PinyinMap *m_pinyinMap;
    PluginLoader m_pluginLoader;
};

} // namespace CSS {

#endif // #ifndef UNICODEPINYINTABLEWITHPLUGIN_H_76127493492378294723489235784785783485348
