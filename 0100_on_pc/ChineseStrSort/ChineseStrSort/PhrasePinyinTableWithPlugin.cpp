#include "PhrasePinyinTableWithPlugin.h"

namespace CSS {

PhrasePinyinTableWithPlugin::PhrasePinyinTableWithPlugin()
    : m_rootNode(NULL)
    , m_pluginLoader("plugins", "PhrasePinyinTableData")
{

}

PhrasePinyinTableWithPlugin::~PhrasePinyinTableWithPlugin()
{

}

ErrorCode PhrasePinyinTableWithPlugin::Translate(const std::wstring &chineseStr, std::vector<std::wstring> &pingyins)
{
    ErrorCode ec = EC_OK;
    const PhrasePinyinNode *node = NULL;
    unsigned int firstMatchedLen = 0;
    unsigned int i;

    pingyins.clear();

    if (ec == EC_OK) {
        ec = FindItem_Partial(chineseStr, node, firstMatchedLen);
    }

    if (ec == EC_OK) {
        if (firstMatchedLen > 0 && node != NULL && node->pinyinCount > 0 && node->pinyins != NULL) {
            for (i = 0; i < node->pinyinCount; ++i) {
                pingyins.push_back(node->pinyins[i]);
            }
        }
    }

    return ec;
}

ErrorCode PhrasePinyinTableWithPlugin::FindItem_Partial(const std::wstring &keys, const PhrasePinyinNode *&node, unsigned int &firstMatchedLen)
{
    ErrorCode ec = EC_OK;
    const PhrasePinyinNode *curNode = m_rootNode;
    const PhrasePinyinMapItem *curMapItems = NULL;
    unsigned int curMapItemCount = 0;
    unsigned int i;
    wchar_t ch;
    unsigned int pos;

    if (curNode != NULL) {
        curMapItems = curNode->mapItems;
        curMapItemCount = curNode->mapItemCount;
    }

    node = NULL;
    firstMatchedLen = 0;

    if (ec == EC_OK) {
        for (i = 0; i < keys.length(); ++i) {
            if (curMapItemCount == 0 || curMapItems == NULL) {
                break;
            }

            ch = keys[i];
            pos = FindKey(curMapItems, curMapItemCount, ch);
            if (pos != npos) {
                curNode = curMapItems[pos].node;
                if (curNode == NULL) {
                    break;
                }
                if (curNode->pinyinCount > 0 && curNode->pinyins != NULL) {
                    firstMatchedLen = i + 1;
                    node = curNode;
                }
                curMapItems = curNode->mapItems;
                curMapItemCount = curNode->mapItemCount;
            } else {
                break;
            }
        }
    }

    return ec;
}

unsigned int PhrasePinyinTableWithPlugin::FindKey(const PhrasePinyinMapItem *mapItems, unsigned int mapItemCount, wchar_t key)
{
    bool bSuc = true;
    unsigned int pos = npos;
    int min, max, half;

    if (bSuc) {
        if (mapItemCount == 0 || mapItems == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (mapItemCount == 0x10000) {
            pos = key;
        } else {
            min = 0;
            max = mapItemCount - 1;
            while (true) {
                if (min == max) {
                    if (key == mapItems[min].ch) {
                        pos = min;
                    }
                    break;
                } else if (min > max) {
                    break;
                }

                half = (min + max) / 2;

                if (key < mapItems[half].ch) {
                    max = half - 1;
                } else if (key > mapItems[half].ch) {
                    min = half + 1;
                } else {
                    pos = half;
                    break;
                }
            }
        }
    }

    return pos;
}

ErrorCode PhrasePinyinTableWithPlugin::LoadData()
{
    ErrorCode ec = EC_OK;
    const PhrasePinyinNode *(*funcGetData)() = NULL;

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
        m_rootNode = funcGetData();
        if (m_rootNode == NULL) {
            ec = EC_FAILED;
        }
    }

    return ec;
}

ErrorCode PhrasePinyinTableWithPlugin::UnloadData()
{
    ErrorCode ec = EC_OK;

    // TODO: 

    return ec;
}

} // namespace CSS {
