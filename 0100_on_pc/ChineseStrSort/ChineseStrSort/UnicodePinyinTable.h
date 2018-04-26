#ifndef UNICODEPINYINTABLE_H_899897832747235787841273167676778234762232276773888
#define UNICODEPINYINTABLE_H_899897832747235787841273167676778234762232276773888

#include "CSSGlobal.h"
#include "IUnicodePinyinTable.h"

namespace CSS {

class UnicodePinyinTable : public IUnicodePinyinTable
{
private:
    typedef std::map<wchar_t, std::vector<std::wstring>> UnicodePinyinMap;

public:
    UnicodePinyinTable();
    ~UnicodePinyinTable();

public:
    ErrorCode LoadData();
    ErrorCode UnloadData();
    ErrorCode Translate(wchar_t chineseChar, std::wstring &pinyin, unsigned int &condidateCount);
    ErrorCode GenCppCode(std::string &cppCodeContent);

private:
    ErrorCode ParseLine(const std::wstring &line);

    std::string BuildSymbolStrForStr(const std::wstring &s);
    std::string BuildContnetStrForStr(const std::wstring &s);

private:
    UnicodePinyinMap m_unicodePinyinMap;
    std::string m_dataFilePath;
    unsigned int m_curLineNo;
};

} // namespace CSS {

#endif // #ifndef UNICODEPINYINTABLE_H_899897832747235787841273167676778234762232276773888
