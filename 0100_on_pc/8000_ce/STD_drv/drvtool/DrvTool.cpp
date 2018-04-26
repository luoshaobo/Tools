#include <Windows.h>
#include "TK_Tools.h"

#define SAVED_DEV_HANLE_FILE_PATH               "\\~test_dev_handle"

void usage(int argc, char *argv[])
{
    printf("Usage:\n");
    printf("    %s activate <sDevRegisterKeyPath>\n", argv[0]);
    printf("    %s deactivate [<hDevice>]\n", argv[0]);
    printf("NOTE:\n", argv[0]);
    printf("    <sDevRegisterKeyPath> should be similar with \"Drivers\\TestDrives\\STD_DRIVER\"\n", argv[0]);
    printf("\n");
}

int MyActivateDevice(const std::string &sDevRegisterKeyPath)
{
    int nRet = 0;
    HANDLE hDevice;
    std::wstring wsDevRegisterKeyPath = TK_Tools::str2wstr(sDevRegisterKeyPath);
    
    hDevice = (HANDLE)ActivateDeviceEx(wsDevRegisterKeyPath.c_str(), NULL, NULL, NULL);
    if (hDevice == NULL) {
        printf("*** ERROR: ActivateDeviceEx() is failed! [nLastError=%lu]\n", GetLastError());
        nRet = -1;
    } else {
        printf("INFO: ActivateDeviceEx() is successfully. The return handle is 0x%08X.\n", hDevice);
        std::string sFileContent;
        sFileContent = TK_Tools::FormatStr("%lu", hDevice);
        if (TK_Tools::SaveContentToFile(TK_Tools::str2tstr(SAVED_DEV_HANLE_FILE_PATH), sFileContent))
        {
            printf("The value of hDevice has been saved to the file \"%s\".\n", SAVED_DEV_HANLE_FILE_PATH);
        }
    }
    
    return nRet;
}

int MyDeactivateDevice(HANDLE hDevice)
{
    int nRet = 0;
    BOOL bSuc;
    
    bSuc = DeactivateDevice(hDevice);
    if (!bSuc) {
        printf("*** ERROR: DeactivateDevice() is failed! [nLastError=%lu]\n", GetLastError());
        nRet = -1;
    } else {
        printf("INFO: DeactivateDevice() is successfully.\n");
    }
    
    return nRet;
}

int MyDeactivateDevice()
{
    int nRet = 0;
    HANDLE hDevice = NULL;
    std::string sFileContent;
    
    if (nRet == 0) {
        printf("Try to get the value of hDevice from the saved file \"%s\".\n", SAVED_DEV_HANLE_FILE_PATH);
        if (TK_Tools::GetContentFromFile(TK_Tools::str2tstr(SAVED_DEV_HANLE_FILE_PATH), sFileContent))
        {
            hDevice = (HANDLE)TK_Tools::StrToUL(sFileContent);
            if (hDevice == NULL) {
                printf("Failed to parse the value of hDevice from the saved file \"%s\".\n", SAVED_DEV_HANLE_FILE_PATH);
                nRet = -1;
            }
        } else {
            printf("Failed to get the value of hDevice from the saved file \"%s\".\n", SAVED_DEV_HANLE_FILE_PATH);
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        nRet = MyDeactivateDevice(hDevice);
    } else {
        printf("*** ERROR: DeactivateDevice() is failed! [hDevice is NULL]\n");
    }

    if (nRet == 0) {
        printf("Delete the file \"%s\".\n", SAVED_DEV_HANLE_FILE_PATH);
        TK_Tools::DeleteFile(SAVED_DEV_HANLE_FILE_PATH);
    }

    return nRet;
}

int main_ansi(int argc, char *argv[])
{
    int nRet = 0;
    
    if (argc <= 1) {
        usage(argc, argv);
        return 0;
    }
    
    if (std::string(argv[1]) == "activate" && argc >= 3) {
        std::string sDevRegisterKeyPath = argv[2];
        nRet = MyActivateDevice(sDevRegisterKeyPath);
    } else if (std::string(argv[1]) == "deactivate" && argc >= 3) {
        HANDLE hDevice = (HANDLE)TK_Tools::StrToUL(argv[2]);
        nRet = MyDeactivateDevice(hDevice);
    } else if (std::string(argv[1]) == "deactivate" && argc == 2) {
        nRet = MyDeactivateDevice();
    } else {
        usage(argc, argv);
    }
    
    return nRet;
}

int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{
    int nRet = 0;
    unsigned int i;
    char **argv_ansi = new char *[argc];
    std::vector<std::string> arrArgv;
    
    for (i = 0; i < argc; i++) {
        arrArgv.push_back(TK_Tools::tstr2str(argv[i]));
        argv_ansi[i] = (char *)arrArgv[i].c_str();
    }
    
    nRet = main_ansi(argc, argv_ansi);
    
    delete [] argv_ansi;
    
    return nRet;
}
