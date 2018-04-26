#ifndef IUNICODEPINYINTABLE_H_7840129781242341741763782842358239504204147823423
#define IUNICODEPINYINTABLE_H_7840129781242341741763782842358239504204147823423

#include "CSSGlobal.h"

namespace CSS {

class IUnicodePinyinTable
{
public:
    virtual ~IUnicodePinyinTable() {}

    virtual ErrorCode LoadData() = 0;
    virtual ErrorCode UnloadData() = 0;
    virtual ErrorCode Translate(wchar_t chineseChar, std::wstring &pinyin, unsigned int &condidateCount) = 0; // only the most possible pingyin is returned.
};

} // namespace CSS {

#endif // #ifndef IUNICODEPINYINTABLE_H_7840129781242341741763782842358239504204147823423
