#ifndef PHRASEPINYINTABLE_H_7632874230042395782348923747823489278523423489249294
#define PHRASEPINYINTABLE_H_7632874230042395782348923747823489278523423489249294

#include "CSSGlobal.h"
#include "IPhrasePinyinTable.h"

namespace CSS {

class PhrasePinyinTable : public IPhrasePinyinTable
{
private:
    struct PhrasePinyinMapValue {
        PhrasePinyinMapValue() : pinyins(NULL), subMap(NULL) {}
        std::vector<std::wstring> *pinyins;
        std::map<wchar_t, PhrasePinyinMapValue> *subMap;
    };
    typedef std::map<wchar_t, PhrasePinyinMapValue> PhrasePinyinMap;

public:
    PhrasePinyinTable();
    ~PhrasePinyinTable();

public:
    ErrorCode LoadData();
    ErrorCode UnloadData();
    ErrorCode Translate(const std::wstring &chineseStr, std::vector<std::wstring> &pingyins);
    ErrorCode GenCppCode(std::string &cppCodeContent);

private:
    ErrorCode ParseLine(const std::wstring &line);

    ErrorCode InsertItem(const std::wstring &keys, const std::vector<std::wstring> &pinyins);
    ErrorCode FindItem_CreateIfNotExist(const std::wstring &keys, PhrasePinyinMapValue *&value);
    ErrorCode FindItem_Partial(const std::wstring &keys, PhrasePinyinMapValue *&value, unsigned int &firstMatchedLen);

    ErrorCode GenCppCode_Node(std::string &cppCodeContent, const PhrasePinyinMapValue &mapValue, unsigned int level, const std::wstring &currentStr);

    std::string BuildSymbolStrForStr(const std::wstring &s);
    std::string BuildContnetStrForStr(const std::wstring &s);

private:
    PhrasePinyinMap m_phrasePinyinMap;
    std::string m_dataFilePath;
    unsigned int m_curLineNo;
};

} // namespace CSS {

#endif // #ifndef PHRASEPINYINTABLE_H_7632874230042395782348923747823489278523423489249294
