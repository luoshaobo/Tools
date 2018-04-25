// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TESTLOADEXTERNALSYMBAL_DLLTOINJECT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TESTLOADEXTERNALSYMBAL_DLLTOINJECT_EXPORTS
#define TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API __declspec(dllexport)
#else
#define TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API __declspec(dllimport)
#endif

enum HD_ERROR
{
    HDE_OK = 0,
    HDE_DLL_NOT_LOADED,
    HDE_FAILED_TO_INSTALL_HOOK,
    HDE_FAILED_TO_DEINSTALL_HOOK,
    HDE_HOOK_NOT_INSTALLED,
    HDE_HOOK_ALREADY_INSTALLED,
    HDE_HOOK_FUNC_PTR_NULL,
};

extern "C" {
TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API HD_ERROR InstallHook(void);
TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API HD_ERROR DeinstallHook(void);
} // extern "C" {

//// This class is exported from the TestLoadExternalSymbal_DllToInject.dll
//class TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API CTestLoadExternalSymbal_DllToInject {
//public:
//	CTestLoadExternalSymbal_DllToInject(void);
//	// TODO: add your methods here.
//};
//
//extern TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API int nTestLoadExternalSymbal_DllToInject;
//
//TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API int fnTestLoadExternalSymbal_DllToInject(void);
