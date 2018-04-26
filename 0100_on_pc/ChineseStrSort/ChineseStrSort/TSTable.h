#ifndef TSTABLE_H_276389190457678347823489095095609070496389478258734889345898
#define TSTABLE_H_276389190457678347823489095095609070496389478258734889345898

#include "CSSGlobal.h"

namespace CSS {

class TSTable
{
private:
    typedef std::map<wchar_t, std::vector<wchar_t>> TSMap;

public:
    TSTable();
    ~TSTable();

public:
    ErrorCode LoadData();
    ErrorCode UnloadData();
    ErrorCode Translate(wchar_t fromTradChar, wchar_t &toSimpleChar, unsigned int &candidateCount); // only the best posibble simple character is returned.

private:
    ErrorCode ParseLine(const std::wstring &line);

private:
    TSMap m_tsMap;
    std::string m_dataFilePath;
    unsigned int m_curLineNo;
};

} // namespace CSS {

#endif // #ifndef TSTABLE_H_276389190457678347823489095095609070496389478258734889345898
