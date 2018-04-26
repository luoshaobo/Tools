// PhrasePinyinTableData.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PhrasePinyinTableData.h"

//// This is an example of an exported variable
//PHRASEPINYINTABLEDATA_API int nPhrasePinyinTableData=0;
//
//// This is an example of an exported function.
//PHRASEPINYINTABLEDATA_API int fnPhrasePinyinTableData(void)
//{
//	return 42;
//}
//
//// This is the constructor of a class that has been exported.
//// see PhrasePinyinTableData.h for the class definition
//CPhrasePinyinTableData::CPhrasePinyinTableData()
//{
//	return;
//}

struct PhrasePinyinNode;

struct PhrasePinyinMapItem {
    wchar_t ch;
    const PhrasePinyinNode *node;
};

struct PhrasePinyinNode {
    unsigned int pinyinCount;
    const wchar_t **pinyins;
    unsigned int mapItemCount;
    const PhrasePinyinMapItem *mapItems;
};

#include "PhrasePingyinTableDataGenenrated.h"

extern "C" {
PHRASEPINYINTABLEDATA_API const void *GetData()
{
    return &node_root;
}
}
