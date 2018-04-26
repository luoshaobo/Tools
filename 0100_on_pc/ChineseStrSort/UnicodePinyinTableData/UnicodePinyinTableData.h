// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the UNICODEPINYINTABLEDATA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// UNICODEPINYINTABLEDATA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef UNICODEPINYINTABLEDATA_EXPORTS
#define UNICODEPINYINTABLEDATA_API __declspec(dllexport)
#else
#define UNICODEPINYINTABLEDATA_API __declspec(dllimport)
#endif

//// This class is exported from the UnicodePinyinTableData.dll
//class UNICODEPINYINTABLEDATA_API CUnicodePinyinTableData {
//public:
//	CUnicodePinyinTableData(void);
//	// TODO: add your methods here.
//};
//
//extern UNICODEPINYINTABLEDATA_API int nUnicodePinyinTableData;
//
//UNICODEPINYINTABLEDATA_API int fnUnicodePinyinTableData(void);
