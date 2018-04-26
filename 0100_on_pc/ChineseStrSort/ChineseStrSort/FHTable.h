#ifndef FHTABLE_H_23889410232385892304923785278892378238782340234892347823489289
#define FHTABLE_H_23889410232385892304923785278892378238782340234892347823489289

#include "CSSGlobal.h"

namespace CSS {

class FHTable
{
private:
    typedef std::map<wchar_t, wchar_t> FHMap;

public:
    FHTable();
    ~FHTable();

public:
    ErrorCode LoadData();
    ErrorCode UnloadData();
    ErrorCode Translate(wchar_t fromFullCodeChar, wchar_t &toHalfCodeChar);

private:
    ErrorCode ParseLine(const std::wstring &line);

private:
    FHMap m_fhMap;
    std::string m_dataFilePath;
    unsigned int m_curLineNo;
};

} // namespace CSS {

#endif // #ifndef FHTABLE_H_23889410232385892304923785278892378238782340234892347823489289
