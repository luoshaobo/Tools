#ifndef PHRASEPINYINTABLEWITHPLUGIN_H_782384010427848912472348923478234823423992
#define PHRASEPINYINTABLEWITHPLUGIN_H_782384010427848912472348923478234823423992

#include "CSSGlobal.h"
#include "IPhrasePinyinTable.h"

namespace CSS {

class PhrasePinyinTableWithPlugin : public IPhrasePinyinTable
{
private:
    static const unsigned int npos = (unsigned int)-1;

    struct PhrasePinyinNode;
    struct PhrasePinyinMapItem {
        wchar_t ch;
        const PhrasePinyinNode *node;
    };
    struct PhrasePinyinNode {
        unsigned int pinyinCount;
        const wchar_t **pinyins;
        unsigned int mapItemCount;
        const PhrasePinyinMapItem *mapItems;
    };

public:
    PhrasePinyinTableWithPlugin();
    ~PhrasePinyinTableWithPlugin();

public:
    ErrorCode LoadData();
    ErrorCode UnloadData();
    ErrorCode Translate(const std::wstring &chineseStr, std::vector<std::wstring> &pingyins);

private:
    ErrorCode FindItem_Partial(const std::wstring &keys, const PhrasePinyinNode *&node, unsigned int &firstMatchedLen);
    unsigned int FindKey(const PhrasePinyinMapItem *mapItems, unsigned int mapItemCount, wchar_t key);

private:
    const PhrasePinyinNode *m_rootNode;
    PluginLoader m_pluginLoader;
};

} // namespace CSS {

#endif // #ifndef PHRASEPINYINTABLEWITHPLUGIN_H_782384010427848912472348923478234823423992
