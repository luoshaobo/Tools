// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PHRASEPINYINTABLEDATA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PHRASEPINYINTABLEDATA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PHRASEPINYINTABLEDATA_EXPORTS
#define PHRASEPINYINTABLEDATA_API __declspec(dllexport)
#else
#define PHRASEPINYINTABLEDATA_API __declspec(dllimport)
#endif

//// This class is exported from the PhrasePinyinTableData.dll
//class PHRASEPINYINTABLEDATA_API CPhrasePinyinTableData {
//public:
//	CPhrasePinyinTableData(void);
//	// TODO: add your methods here.
//};
//
//extern PHRASEPINYINTABLEDATA_API int nPhrasePinyinTableData;
//
//PHRASEPINYINTABLEDATA_API int fnPhrasePinyinTableData(void);
