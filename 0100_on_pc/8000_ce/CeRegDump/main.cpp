#include <Windows.h>
#include <string>
#include "CeRegDump.h"


int wmain( int argc, wchar_t *argv[ ], wchar_t *envp[ ] )
{

/*
	::wprintf(L"CeRegDump.exe START\n");

	std::wstring arg1(argv[1]);
	std::wstring arg3(argv[3]);
	
	std::wstring fullName;
	std::wstring output;
	bool ok = true;
	
	// ::wprintf( L"arg1: %s\n", arg1.c_str() );
	// ::wprintf( L"arg3: %s\n", arg3.c_str() );
	if (5 == argc) {
		if ( L"-n" == arg1 && L"-o" == arg3 ) {
		
			fullName.assign(argv[2]);
			output.assign(argv[4]);
		}
		else if ( L"-n" == arg3 && L"-o" == arg1 ) {
		
			fullName.assign(argv[4]);
			output.assign(argv[2]);
		}
		else { ok = false; ::wprintf(L"Wrong paramteres detected\n"); }
		
		if (ok) {
		
			CeRegDump inst(fullName);
			inst.traverse();
			inst.dump(output);
		}
	}
	
	::wprintf(L"CeRegDump.exe END\n");
*/	
	
	
/*
	CeRegDump usbcdcncm1(L"HKEY_LOCAL_MACHINE\\Comm\\USBCDCNCM1");
	// CeRegDump inst(L"HKEY_LOCAL_MACHINE\\");
	usbcdcncm1.traverse();
	usbcdcncm1.dump(L"\\Hard Disk\\AUTOLAUNCH_PROD\\CeRegDumpResult_USBCDCNCM1.txt");
	// inst.dump("\\release\\CeRegDumpResult_HKEY_LOCAL_MACHINE.txt");
	
	CeRegDump ndis(L"HKEY_LOCAL_MACHINE\\Comm\\NDIS");
	ndis.traverse();
	ndis.dump(L"\\Hard Disk\\AUTOLAUNCH_PROD\\CeRegDumpResult_NDIS.txt");
*/
	
	CeRegDump localMachine(L"HKEY_LOCAL_MACHINE");
	localMachine.traverse();
	localMachine.dump(L"\\release\\CeRegDumpResult_HKEY_LOCAL_MACHINE.txt");

	
	return 0;
}