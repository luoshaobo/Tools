// UnicodePinyinTableData.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "UnicodePinyinTableData.h"


//// This is an example of an exported variable
//UNICODEPINYINTABLEDATA_API int nUnicodePinyinTableData=0;
//
//// This is an example of an exported function.
//UNICODEPINYINTABLEDATA_API int fnUnicodePinyinTableData(void)
//{
//	return 42;
//}
//
//// This is the constructor of a class that has been exported.
//// see UnicodePinyinTableData.h for the class definition
//CUnicodePinyinTableData::CUnicodePinyinTableData()
//{
//	return;
//}

struct CharPinyins {
    unsigned int itemCount;
    const wchar_t **items;
};

struct PinyinMapItem {
    wchar_t ch;
    const CharPinyins *charPinyins;
};

struct PinyinMap {
    unsigned int itemCount;
    const PinyinMapItem *items;
};

#include "UnicodePingyinTableDataGenenrated.h"

extern "C" {
UNICODEPINYINTABLEDATA_API const void *GetData()
{
    return &map_root;
}
}
