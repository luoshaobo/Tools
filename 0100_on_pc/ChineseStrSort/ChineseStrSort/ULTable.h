#ifndef ULTABLE_H_76381204092385723739124782364782561242385940423478238423747823
#define ULTABLE_H_76381204092385723739124782364782561242385940423478238423747823

#include "CSSGlobal.h"

namespace CSS {

class ULTable
{
private:
    typedef std::map<wchar_t, wchar_t> ULMap;

public:
    ULTable();
    ~ULTable();

public:
    ErrorCode LoadData();
    ErrorCode UnloadData();
    ErrorCode Translate(wchar_t fromUpperChar, wchar_t &toLowerChar);

private:
    ErrorCode ParseLine(const std::wstring &line);

private:
    ULMap m_ulMap;
    std::string m_dataFilePath;
    unsigned int m_curLineNo;
};

} // namespace CSS {

#endif // #ifndef ULTABLE_H_76381204092385723739124782364782561242385940423478238423747823
