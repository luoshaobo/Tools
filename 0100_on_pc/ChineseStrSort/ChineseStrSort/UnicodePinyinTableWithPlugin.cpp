#include "UnicodePinyinTableWithPlugin.h"

namespace CSS {

UnicodePinyinTableWithPlugin::UnicodePinyinTableWithPlugin()
    : m_pinyinMap(NULL)
    , m_pluginLoader("plugins", "UnicodePinyinTableData")
{

}

UnicodePinyinTableWithPlugin::~UnicodePinyinTableWithPlugin()
{

}

ErrorCode UnicodePinyinTableWithPlugin::Translate(wchar_t chineseChar, std::wstring &pinyin, unsigned int &condidateCount)
{
    ErrorCode ec = EC_OK;

    pinyin.clear();
    condidateCount = 0;

    if (ec == EC_OK) {
        if (m_pinyinMap == NULL) {
            ec = EC_FAILED;
        }
    }

    if (ec == EC_OK) {
        if (m_pinyinMap->itemCount != 0x10000) {
            ec = EC_NOT_IMPLEMENTED;
        }
    }

    if (ec == EC_OK) {
        const PinyinMapItem *items = m_pinyinMap->items;
        if (items != NULL) {
            const PinyinMapItem &item = items[chineseChar];
            const CharPinyins *charPinyins = item.charPinyins;
            if (charPinyins != NULL) {
                if (charPinyins->itemCount > 0 && charPinyins->items != NULL) {
                    const wchar_t *pinyin0 = charPinyins->items[0];
                    if (pinyin0 != NULL) {
                        pinyin = pinyin0;
                        condidateCount = charPinyins->itemCount;
                    }
                }
            }
        }
    }

    return ec;
}

ErrorCode UnicodePinyinTableWithPlugin::LoadData()
{
    ErrorCode ec = EC_OK;
    const PinyinMap *(*funcGetData)() = NULL;

    if (ec == EC_OK) {
        ec = m_pluginLoader.LoadPlugin();
    }

    if (ec == EC_OK) {
        ec = m_pluginLoader.GetSymbolAddr("GetData", (void **)&funcGetData);
    }

    if (ec == EC_OK) {
        if (funcGetData == NULL) {
            ec = EC_FAILED;
        }
    }

    if (ec == EC_OK) {
        m_pinyinMap = funcGetData();
        if (m_pinyinMap == NULL) {
            ec = EC_FAILED;
        }
    }

    return ec;
}

ErrorCode UnicodePinyinTableWithPlugin::UnloadData()
{
    ErrorCode ec = EC_OK;

    // TODO:

    return ec;
}

} // namespace CSS {
