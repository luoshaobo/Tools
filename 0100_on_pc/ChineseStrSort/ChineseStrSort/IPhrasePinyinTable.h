#ifndef IPHRASEPINYINTABLE_H_78230104238923492375289492378234923954785278347878
#define IPHRASEPINYINTABLE_H_78230104238923492375289492378234923954785278347878

#include "CSSGlobal.h"

namespace CSS {

class IPhrasePinyinTable
{
public:
    virtual ~IPhrasePinyinTable() {}

    virtual ErrorCode LoadData() = 0;
    virtual ErrorCode UnloadData() = 0;
    virtual ErrorCode Translate(const std::wstring &chineseStr, std::vector<std::wstring> &pingyins) = 0;
};

} // namespace CSS {

#endif // #ifndef IPHRASEPINYINTABLE_H_78230104238923492375289492378234923954785278347878
