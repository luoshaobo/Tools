#ifndef SURNAMEPINYINTABLE_H_76478239651262374782342374561247812478237642388234
#define SURNAMEPINYINTABLE_H_76478239651262374782342374561247812478237642388234

#include "CSSGlobal.h"

namespace CSS {

class SurnamePinyinTable
{
private:
    typedef std::map<std::wstring, std::vector<std::wstring>> SurnamePinyinMap;

public:
    SurnamePinyinTable();
    ~SurnamePinyinTable();

public:
    ErrorCode LoadData();
    ErrorCode UnloadData();
    ErrorCode Translate(const std::wstring &surname, std::vector<std::wstring> &pingyins);  // surname could be more than 1 character

private:
    ErrorCode ParseLine(const std::wstring &line);

private:
    SurnamePinyinMap m_surnamePinyinMap;
    std::string m_dataFilePath;
    unsigned int m_curLineNo;
};

} // namespace CSS {

#endif // #ifndef SURNAMEPINYINTABLE_H_76478239651262374782342374561247812478237642388234