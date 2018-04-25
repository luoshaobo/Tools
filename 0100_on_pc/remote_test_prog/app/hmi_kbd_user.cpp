#include "TK_sock.h"
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // #ifdef WIN32
#include <string>
#include <vector>
#include <string.h>
#include <stdio.h>

#include <QCoreApplication>

#include "TK_Tools.h"
#include "TK_DebugServer.h"

//#include <CorSsw_EmbRegistryIds.h>
// #include <persistence_client_library.h>
// #include <persistence_client_library_key.h>

////////////////////////////////////////////////////////////////////////////////////
// misc
//
/*
SOURCES += \
    hmi_kbd_user.cpp \
    TK_DebugServer.cpp \
    TK_Tools.cpp \
    TK_sock.cpp \
    
HEADERS += \
    TK_DebugServer.h \
    TK_Tools.h \    
    TK_sock.h 
*/

/*
[Socket]
ListenDatagram=1060
*/

////////////////////////////////////////////////////////////////////////////////////
// for debug of PushObject
//


#ifdef WIN32
#ifdef address
#undef address
#endif
#endif // #ifdef WIN32

#ifdef WIN32
#ifdef type
#undef type
#endif
#endif // #ifdef WIN32

#define GS_ASSERT(expr)                 TK_Tools::Assert(expr)
#define GS_PRINT_TO_PEER1               TK_Tools::DebugServer::GetInstance(TK_Tools::DSIID_TLM).SendTextToPeer1
#define GS_PRINT_TO_PEER2               TK_Tools::DebugServer::GetInstance(TK_Tools::DSIID_TLM).SendTextToPeer2
#define GS_MSG                          GS_Msg

unsigned int GSOPT_PushObjectEnable = 0;

static void GSOPT_Init()
{
//	    GSOPT_PushObjectEnable = 1;
}

bool GS_Msg_bOn = true;
bool GS_Msg(const char *pFormat, ...)
{
    if (!GS_Msg_bOn) {
        return true;
    }

    if (pFormat == NULL) {
        return true;
    }

#define FORMAT_BUF_SIZE                         4096
    int nSize = 0;
    TK_UNUSED_VAR(nSize);
    char *buff = new char[FORMAT_BUF_SIZE];
    if (buff == NULL) {
        return false;
    }
    TK_Tools::AutoFreeArrayPtr<char> afaBuff(buff);
    memset(buff, 0, FORMAT_BUF_SIZE);
    va_list args;
    va_start(args, pFormat);
    nSize = _vsnprintf(buff, FORMAT_BUF_SIZE - 1, pFormat, args);
    va_end(args);
#undef FORMAT_BUF_SIZE

    return TK_Tools::DebugServer::GetInstance(TK_Tools::DSIID_TLM).SendTextToPeer2(buff);
}

static struct {
    bool bThreadStopped;
    uint32_t evt;
    uint8_t* pData;
    uint32_t size;
    int timeout;
    bool (*IsCanceled)(void);
} SendUmsgToNaviHmi_data = { true };

static void *SendUmsgToNaviHmi_ThreadProc(void *lpParameter)
{
    if (SendUmsgToNaviHmi_data.timeout > 0) {
        usleep(SendUmsgToNaviHmi_data.timeout * 1000);
    }

    if (SendUmsgToNaviHmi_data.IsCanceled != NULL) {
        if (SendUmsgToNaviHmi_data.IsCanceled()) {
            SendUmsgToNaviHmi_data.bThreadStopped = true;
            return 0;
        }
    }
    
    // TODO: call the function here directly!
    
    SendUmsgToNaviHmi_data.bThreadStopped = true;
    
    return 0;
}

static BOOL SendUmsgToNaviHmi_bUseThread = TRUE;
static void SendUmsgToNaviHmi(uint32_t evt, const uint8_t* pData, uint32_t size, int timeout, bool (*IsCanceled)(void) = NULL)
{
    if (!SendUmsgToNaviHmi_bUseThread) {
        // TODO: call the function here directly!
        return;
    }

    while (!SendUmsgToNaviHmi_data.bThreadStopped) {
        usleep(50 * 1000);
    }

    if (SendUmsgToNaviHmi_data.pData!= NULL) {
        delete [] SendUmsgToNaviHmi_data.pData;
    }

    memset(&SendUmsgToNaviHmi_data, 0, sizeof(SendUmsgToNaviHmi_data));
    SendUmsgToNaviHmi_data.bThreadStopped = false;
    SendUmsgToNaviHmi_data.evt = evt;
    if (pData != NULL && size > 0) {
        SendUmsgToNaviHmi_data.pData = new uint8_t[size];
        GS_ASSERT(SendUmsgToNaviHmi_data.pData != NULL);
        if (SendUmsgToNaviHmi_data.pData != NULL) {
            memcpy(SendUmsgToNaviHmi_data.pData, pData, size);
            SendUmsgToNaviHmi_data.size = size;
        }
    }
    SendUmsgToNaviHmi_data.timeout = timeout;
    SendUmsgToNaviHmi_data.IsCanceled = IsCanceled;

    pthread_t pth = 0;
    pthread_create(&pth, NULL, &SendUmsgToNaviHmi_ThreadProc, NULL);
}

//
// Switch
//
static BOOL GSC_Switch(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "GSC") == 0 
        && TK_Tools::CompareNoCase(vCmdLine[1], "SWITCH") == 0) {
        if (TK_Tools::CompareNoCase(vCmdLine[2], "ALL_ON") == 0) {
            GSOPT_PushObjectEnable = 1;

            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "ALL_OFF") == 0) {
            GSOPT_PushObjectEnable = 0;

            ret = TRUE;
        } else {
            
        }
    }
    
    return ret;
}

//
// Switch 2
//
//    { "GSC SWITCH help",                    &GSC_Switch2,                           "GSC SWITCH help" },
//    { "GSC SWITCH ON - <s>",                &GSC_Switch2,                           "GSC SWITCH ON - <FuncName>" },
//    { "GSC SWITCH OFF - <s>",               &GSC_Switch2,                           "GSC SWITCH OFF - <FuncName>" },
static BOOL GSC_Switch2(std::vector<std::string> &vCmdLine)
{
    static struct {
        const char *pFuncName;
        unsigned int *pFuncValue;
    } funcInfoTable[] = {
        { "PushObjectEnable",           &GSOPT_PushObjectEnable },
    };
    BOOL ret = FALSE;
    int i;
    int nFuncInfoTableItemCount = TK_ARR_LEN(funcInfoTable);

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "GSC") == 0 
        && TK_Tools::CompareNoCase(vCmdLine[1], "SWITCH") == 0) {
        if (TK_Tools::CompareNoCase(vCmdLine[2], "help") == 0) {
            for (i = 0; i < nFuncInfoTableItemCount; i++) {
                GS_MSG("%s(): %s=%d\n", __FUNCTION__, funcInfoTable[i].pFuncName, *funcInfoTable[i].pFuncValue);    
            }
            
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "ON") == 0) {
            i = nFuncInfoTableItemCount;
            if (vCmdLine.size() >= 5) {
                for (i = 0; i < nFuncInfoTableItemCount; i++) {
                    if (TK_Tools::CompareNoCase(vCmdLine[4], funcInfoTable[i].pFuncName) == 0) {
                        *funcInfoTable[i].pFuncValue = 1;
                        GS_MSG("%s(): set %s=%d\n", __FUNCTION__, funcInfoTable[i].pFuncName, *funcInfoTable[i].pFuncValue);
                        break;
                    }
                }
                if (i == nFuncInfoTableItemCount) {
                    GS_MSG("%s(): *** the switch \"%s\" does not exist!\n", __FUNCTION__, vCmdLine[4].c_str()); 
                }
            } else {
                GS_MSG("%s(): *** no switch!\n", __FUNCTION__); 
            }            

            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "OFF") == 0) {
            i = nFuncInfoTableItemCount;
            if (vCmdLine.size() >= 5) {
                for (i = 0; i < nFuncInfoTableItemCount; i++) {
                    if (TK_Tools::CompareNoCase(vCmdLine[4], funcInfoTable[i].pFuncName) == 0) {
                        *funcInfoTable[i].pFuncValue = 0;
                        GS_MSG("%s(): set %s=%d\n", __FUNCTION__, funcInfoTable[i].pFuncName, *funcInfoTable[i].pFuncValue);
                        break;
                    }
                    
                }
                if (i == nFuncInfoTableItemCount) {
                    GS_MSG("%s(): *** the switch \"%s\" does not exist!\n", __FUNCTION__, vCmdLine[4].c_str()); 
                }
            } else {
                GS_MSG("%s(): *** no switch!\n", __FUNCTION__); 
            }

            ret = TRUE;
        } else {
            
        }
    }
    
    return ret;
}

//
// Msg
//
static BOOL GSC_Msg(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "GSC") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "MSG") == 0) {
        if (TK_Tools::CompareNoCase(vCmdLine[2], "ON") == 0) {
            GS_Msg_bOn = true;

            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "OFF") == 0) {
            GS_Msg_bOn = false;

            ret = TRUE;
        } else {
            
        }
    }
    
    return ret;
}

//
// SYS exit
//
static void GS_Sys_StackOverflow()
{
    int nArr[1024 * 1024 * 1] = { 0 };
}
#define HWKIM_MAKE_POINTER_ALIGN(p) ((((unsigned long)(p))+3)&0xFFFFFFFC)
static long* RaisedExceptionTest(const char *pcFileName, int * nLen)
{
	FILE *fp;
	int iDataLen;
	char * pcBuf = 0;
	long* plBuf = 0;
	*nLen = 0;
        
	if (pcFileName == 0){
	    plBuf = 0;
	}

	fp = fopen( pcFileName, "rb");
	if ( fp==0){
	    plBuf = 0;
	}else{
		fseek( fp, 0, SEEK_END );
		iDataLen = ftell(fp);
		//pcBuf = reinterpret_cast<char*>(malloc(iDataLen + 4));//allocate extra 4 bytes for alignment // 4 bytes alignment
		plBuf = reinterpret_cast<long*>(HWKIM_MAKE_POINTER_ALIGN(pcBuf));
		fseek(fp, 0, SEEK_SET);
		fread( plBuf, iDataLen, 1, fp );
		fclose(fp);
		*nLen = iDataLen;
	}
	return plBuf;
}

static BOOL GS_Sys(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 2 
        && TK_Tools::CompareNoCase(vCmdLine[0], "SYS") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "exit") == 0) {
        int *p = (int *)0;
        *p = 1;
        exit(0);
        ret = TRUE;
    } else if (vCmdLine.size() >= 4 
        && TK_Tools::CompareNoCase(vCmdLine[0], "SYS") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "Test") == 0) {
        if (TK_Tools::CompareNoCase(vCmdLine[3], "DataAbort") == 0) {
            GS_MSG("%s(): DataAbort\n", __FUNCTION__);
            int *p = (int *)0;
            *p = 1;
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "PrefetchAbort") == 0) {
            GS_MSG("%s(): PrefetchAbort\n", __FUNCTION__);
            void (*pFunc)(void) = NULL;
            pFunc();
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "StackOverflow") == 0) {
            GS_MSG("%s(): StackOverflow\n", __FUNCTION__);
            GS_Sys_StackOverflow();
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "StackRewrite") == 0) {
            GS_MSG("%s(): StackRewrite\n", __FUNCTION__);
            int nArr[1];
            memset(nArr, 0, 1024 * 1024 * 1);
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "DivZero") == 0) {
            GS_MSG("%s(): DivZero\n", __FUNCTION__);
            int nZero = 0;
            int n = 5 / nZero;
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "CppException") == 0) {
            GS_MSG("%s(): CppException\n", __FUNCTION__);
            //throw new std::string("TestCppException");
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "RaisedException") == 0) {
            GS_MSG("%s(): RaisedException\n", __FUNCTION__);
            int nLen;
            RaisedExceptionTest("\\windows\\entHMI2.exe", &nLen);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// SaveMemToFile - <nAddr> <nSize> <sFilePathName>
//
static BOOL GS_SaveMemToFile(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    UINT32_T nAddr = 0;
    UINT32_T nSize = 0;
    std::string sFilePathName;

    if (vCmdLine.size() >= 5 && TK_Tools::CompareNoCase(vCmdLine[0], "SaveMemToFile") == 0) {
        if (vCmdLine.size() >= 5) {
            nAddr = (UINT32_T)TK_Tools::StrToUL(vCmdLine[2]);
            nSize = (UINT32_T)TK_Tools::StrToUL(vCmdLine[3]);
            sFilePathName = vCmdLine[4];

            if (nAddr == 0) {
                GS_MSG("*%s(): nAddr == 0!\n", __FUNCTION__);
            } else if (nSize == 0) {
                GS_MSG("*%s(): nSize == 0!\n", __FUNCTION__);
            } else {
                std::vector<unsigned char> content((unsigned char *)nAddr, (unsigned char *)nAddr + nSize);
                bool bSuc = TK_Tools::SaveContentToFile(TK_Tools::str2tstr(sFilePathName), content);
                if (!bSuc) {
                    GS_MSG("*%s(): SaveContentToFile() is failed!\n", __FUNCTION__);
                }
            }
            
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// kp - <ExeName>
//
static BOOL GS_KillProce(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    // BOOL bSuc;
    
    // HWND hWndForSearch = NULL;
    // std::string strWindowsInfo;

    // if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "kp") == 0) {
        // std::string sExeName(vCmdLine[2]);
        // if (sExeName == "-1") {
            // sExeName = "TestLoadExternalSymbal_debugee.exe";
            // bSuc = TK_Tools::KillProcessByName(TK_Tools::str2tstr(sExeName));
            // if (bSuc) {
                // GS_MSG("%s(): process \'%s\' is killed!\n", __FUNCTION__, sExeName.c_str());
            // } else {
                // GS_MSG("*** %s(): process \'%s\' is not killed!\n", __FUNCTION__, sExeName.c_str());
            // }

            // sExeName = "TestLoadExternalSymbal.exe";
            // bSuc = TK_Tools::KillProcessByName(TK_Tools::str2tstr(sExeName));
            // if (bSuc) {
                // GS_MSG("%s(): process \'%s\' is killed!\n", __FUNCTION__, sExeName.c_str());
            // } else {
                // GS_MSG("*** %s(): process \'%s\' is not killed!\n", __FUNCTION__, sExeName.c_str());
            // }
        // } else {
            // bSuc = TK_Tools::KillProcessByName(TK_Tools::str2tstr(sExeName));
            // if (bSuc) {
                // GS_MSG("%s(): process \'%s\' is killed!\n", __FUNCTION__, sExeName.c_str());
            // } else {
                // GS_MSG("*** %s(): process \'%s\' is not killed!\n", __FUNCTION__, sExeName.c_str());
            // }
        // }

        // ret = TRUE;
    // }
    
    return ret;
}

//
// exec - <ExeName> <Arg1> <Arg2> ...
//
static BOOL GS_Execute(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    // unsigned int i;

    // if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "exec") == 0) {
        // std::string sExePath(vCmdLine[2]);
        // std::string sArgs;
        // for (i = 3; i < vCmdLine.size(); i++) {
            // if (!sArgs.empty()) {
                // sArgs += " ";
                // sArgs += vCmdLine[i];
            // } else {
                // sArgs = vCmdLine[i];
            // }
        // }
        // std::string sCmdLine = TK_Tools::FormatStr("%s %s", sExePath.c_str(), sArgs.c_str());
        // GS_MSG("%s(): sCmdLine: %s\n", __FUNCTION__, sCmdLine.c_str());
        // {
            // if (!TK_Tools::Execute(sExePath, sArgs, false)) {
                // GS_MSG("%s(): TK_Tools::Execute() is failed!\n", __FUNCTION__);
            // }
        // }

        // ret = TRUE;
    // }
    
    return ret;
}

class QtSYSServerAdaptor
{
public:
     static void SetSessionState(int n);
     static void SetDarkMode(int n);
};

//
// SetSessionState - <nState>
//     #define LcHMISESSIONSTATE                 "HmiSessionState"
//     #define LcHmiNotStarted                   0x01U
//     #define LcHmiNotReadyForDisplay           0x02U
//     #define LcHmiReadyForDisplay              0x03U
//
// static BOOL GS_SetSessionState(std::vector<std::string> &vCmdLine)
// {
    // BOOL ret = FALSE;
    // UINT32_T nState;

    // // if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "SetSessionState") == 0) {
        // // nState = (UINT32_T)TK_Tools::StrToL(vCmdLine[2]);
        // // GS_MSG("%s(): nState=%d\n", __FUNCTION__, nState);
        // // QtSYSServerAdaptor::SetSessionState(nState);
    // // }
    
    // return ret;
// }

//
// SetDarkMode - <n>
//     param 0 - no change, 1 - DarkMode inactive, 2 - DarkMode active
//
// static BOOL GS_SetDarkMode(std::vector<std::string> &vCmdLine)
// {
    // BOOL ret = FALSE;
    // UINT32_T nDarkMode;

    // // if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "SetDarkMode") == 0) {
        // // nDarkMode = (UINT32_T)TK_Tools::StrToL(vCmdLine[2]);
        // // GS_MSG("%s(): nDarkMode=%d\n", __FUNCTION__, nDarkMode);
        // // QtSYSServerAdaptor::SetDarkMode(nDarkMode);
    // // }
    
    // return ret;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////
// some ERGs for NS_ReadErgData() and NS_WriteErgData()
//
// static bool GS_InitErgLib_bInit = false;
// static std::string GS_InitErgLib_sAppName = "ovip-sys-veh-drive-app";
// static void GS_InitErgLib()
// {
    // int nRet;

    // if (!GS_InitErgLib_bInit) {
        // nRet = pclInitLibrary(GS_InitErgLib_sAppName.c_str(), PCL_SHUTDOWN_TYPE_NONE);
        // if (nRet < 0) {
            // GS_MSG("*%s(): pclInitLibrary() is failed! [ret=%d]\n", __FUNCTION__, nRet);
        // } else {
            // GS_MSG("%s(): pclInitLibrary() is successful! [AppName=%s]\n", __FUNCTION__, GS_InitErgLib_sAppName.c_str());
            // GS_InitErgLib_bInit = true;
        // }
    // }
// }

// static void GS_DeinitErgLib()
// {
    // int nRet;

    // if (GS_InitErgLib_bInit) {
        // nRet = pclDeinitLibrary();
        // if (nRet < 0) {
            // GS_MSG("*%s(): pclDeinitLibrary() is failed! [ret=%d]\n", __FUNCTION__, nRet);
        // } else {
            // GS_MSG("%s(): pclDeinitLibrary() is successful! [AppName=%s]\n", __FUNCTION__, GS_InitErgLib_sAppName.c_str());
            // GS_InitErgLib_bInit = false;
        // }
    // }
// }

//
// SetErgAppName - <sAppName>
//
// static BOOL GS_SetErgAppName(std::vector<std::string> &vCmdLine)
// {    
    // BOOL ret = FALSE;

    // std::string sAppName;

    // if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "SetErgAppName") == 0) {
        // if (vCmdLine.size() >= 3) {
            // GS_DeinitErgLib();
            // GS_InitErgLib_sAppName = vCmdLine[2];
            // GS_InitErgLib();
            
            // ret = TRUE;
        // }
    // }
    
    // return ret;
// }

//
// ReadErgData - <nLDBID> <sName>
//
// static BOOL GS_ReadErgData(std::vector<std::string> &vCmdLine)
// {    
    // BOOL ret = FALSE;

    // unsigned int ldbid = (unsigned int)-1;
    // std::string sName;
    // int nDataSize = 0;
    // int nDataSizeRead = 0;
    // uint8_t *pDataBuf = NULL;

    // GS_InitErgLib();

    // if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "ReadErgData") == 0) {
        // if (vCmdLine.size() >= 4) {
            // ldbid = (unsigned int)TK_Tools::StrToUL(vCmdLine[2]);
            // sName = vCmdLine[3];

            // nDataSize = pclKeyGetSize(ldbid, sName.c_str(), 0, 0);
            // if (nDataSize > 0) {
                // pDataBuf = new uint8_t[nDataSize];
                // if (pDataBuf != NULL) {
                    // nDataSizeRead = pclKeyReadData(ldbid, sName.c_str(), 0, 0, pDataBuf, nDataSize);
                    // if (nDataSizeRead == nDataSize) {
                        // GS_MSG("%s(): [0x%08X] Read ERG value (hex):", __FUNCTION__, ldbid);
                        // for (unsigned int i = 0; i < nDataSizeRead; i++) {
                            // GS_MSG(" %02X", pDataBuf[i]);
                        // }
                        // GS_MSG("\n");
                    // } else {
                        // GS_MSG("*%s(): pclKeyReadData() is failed! [ret=%d]\n", __FUNCTION__, nDataSizeRead);
                    // }
                // } else {
                    // GS_MSG("*%s(): new is failed!\n", __FUNCTION__);
                // }
            // } else {
                // GS_MSG("*%s(): pclKeyGetSize() is failed! [ret=%d]\n", __FUNCTION__, nDataSize);
            // }
            
            // ret = TRUE;
        // }
    // }

    // if (pDataBuf != NULL) {
        // delete [] pDataBuf;
    // }
    
    // return ret;
// }

//
// WriteErgData - <nLDBID> <sName> <nValueByte1> ...
//
// static BOOL GS_WriteErgData(std::vector<std::string> &vCmdLine)
// {    
    // BOOL ret = FALSE;

    // unsigned int ldbid = (unsigned int)-1;
    // std::string sName;
    // int nDataSize = 0;
    // int nDataSizeWritten = 0;
    // uint8_t *pDataBuf = NULL;

    // GS_InitErgLib();

    // if (vCmdLine.size() >= 5 && TK_Tools::CompareNoCase(vCmdLine[0], "WriteErgData") == 0) {
        // if (vCmdLine.size() >= 5) {
            // ldbid = (unsigned int)TK_Tools::StrToUL(vCmdLine[2]);
            // sName = vCmdLine[3];

            // nDataSize = vCmdLine.size() - 4;
            // pDataBuf = new uint8_t[nDataSize];
            // if (pDataBuf != NULL) {
                // for (unsigned int i = 0; i < nDataSize; i++) {
                    // pDataBuf[i] = (uint8_t)TK_Tools::StrToUL(vCmdLine[i + 4]);
                // }

                // nDataSizeWritten = pclKeyWriteData(ldbid, sName.c_str(), 0, 0, pDataBuf, nDataSize);
                // if (nDataSizeWritten == nDataSize) {
                    // GS_MSG("%s(): [0x%08X] Write ERG value (hex):", __FUNCTION__, ldbid);
                    // for (unsigned int i = 0; i < nDataSize; i++) {
                        // GS_MSG(" %02X", pDataBuf[i]);
                    // }
                    // GS_MSG("\n");
                // } else {
                    // GS_MSG("*%s(): pclKeyWriteData() is failed! [ret=%d]\n", __FUNCTION__, nDataSizeWritten);
                // }
            // } else {
                // GS_MSG("*%s(): new is failed!\n", __FUNCTION__);
            // }
            
            // ret = TRUE;
        // }
    // }

    // if (pDataBuf != NULL) {
        // delete [] pDataBuf;
    // }
    
    // return ret;
// }

//
// qt_exit: exit process
//
static BOOL GS_qt_exit(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "qt_exit") == 0) {
        GS_MSG("%s()\n", __FUNCTION__);
        QCoreApplication::exit(0);
        return TRUE;
    }
    
    return ret;
}

//
// TestCodeConversion - [utf8_to_unicode|unicode_to_utf8]
//
// static BOOL GS_TestCodeConversion(std::vector<std::string> &vCmdLine)
// {    
    // BOOL ret = FALSE;

    // std::string sConversion;

    // if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "TestCodeConversion") == 0) {
        // if (vCmdLine.size() >= 3) {
            // sConversion = vCmdLine[2];
            
            // if (sConversion == "utf8_to_unicode") {
                // std::wstring sResult = TK_Tools::str2wstr("ABC");
                // std::vector<unsigned char> aFileContent((unsigned char *)sResult.c_str(), (unsigned char *)sResult.c_str() + sResult.length() * 2);
                // TK_Tools::SaveContentToFile("/tmp/test_code_conv_utf8_to_unicode.txt", aFileContent);
            // } else if (sConversion == "unicode_to_utf8") {
                // std::string sResult = TK_Tools::wstr2str(L"ABC");
                // std::vector<unsigned char> aFileContent((unsigned char *)sResult.c_str(), (unsigned char *)sResult.c_str() + sResult.length());
                // TK_Tools::SaveContentToFile("/tmp/test_code_conv_unicode_to_utf8.txt", aFileContent);
            // } 
            
            // ret = TRUE;
        // }
    // }
    
    // return ret;
// }

//////////////////////////////////////////////////////////////////////////////////////////
// command line paser
//
// NOTE:
// 1) The words before "-" in the command line are fixed.
//    The words after "-" in the command line are variable.
// 2) The word count of the user command line must be less than or equal to the word count of the configuration command line.
// 3) If the words before "-" in the user command line match the first words in a configuration command line, then a match is found.
//    If all the words in the user command line match the first words in a configuration command line, then a match is found.
// 4) Once a match is found, the searching will stop right now.
static struct {
    const char *pCmdLine;
    BOOL (*pFunc)(std::vector<std::string> &vCmdLine);
    const char *pDesc;
} mCmdFuncMap[] = {
    { "SYS exit",                           &GS_Sys,                                "SYS exit: exit process" },
    { "SYS Test - <s>",                     &GS_Sys,                                "SYS Test - [DataAbort|CppException]" },
    
    { "qt_exit",                            &GS_qt_exit,                            "qt_exit: exit process" },

    
    // { "SetErgAppName - <s>",                &GS_SetErgAppName,                      "SetErgAppName - <sAppName>" },
    // { "ReadErgData - <n> <s>",              &GS_ReadErgData,                        "ReadErgData - <nLDBID> <sName>" },
    // { "WriteErgData - <n> <s> <n> ...",     &GS_WriteErgData,                       "WriteErgData - <nLDBID> <sName> <nValueByte1> ..." },

    // { "TestCodeConversion - <s>",           &GS_TestCodeConversion,                 "TestCodeConversion - [utf8_to_unicode|unicode_to_utf8]" },
    
    // { "SetSessionState - <n>",              &GS_SetSessionState,                    "SetSessionState - <n>" },
    // { "SetDarkMode - <n>",                  &GS_SetDarkMode,                        "SetDarkMode - <n>" },
};
bool /*NAVSIM_ProcessCmdLine*/TLMSIM_ProcessCmdLine(const char *pCmdLine)
{
    bool ret = false;
    
    if (pCmdLine == NULL) {
        return ret;
    }

    std::string sCmdLine(pCmdLine);
    uint32_t nItemCount = sizeof(mCmdFuncMap) / sizeof(mCmdFuncMap[0]);
    uint32_t i;

    std::vector<std::string> vCmdLine = TK_Tools::SplitString(pCmdLine);
    if (vCmdLine.size() >= 1  
        && (TK_Tools::CompareNoCase(vCmdLine[0], "help") == 0 
            || TK_Tools::CompareNoCase(vCmdLine[0], "H") == 0 
            || TK_Tools::CompareNoCase(vCmdLine[0], "?") == 0
           )
       ) {
        GS_PRINT_TO_PEER1("Remote Commands\n");
        if (vCmdLine.size() >= 2) {
            for (i = 0; i < nItemCount; i++) {
                std::vector<std::string> vItemCmdLine = TK_Tools::SplitString(mCmdFuncMap[i].pCmdLine);
                if (vItemCmdLine.size() >= vCmdLine.size() - 1) {
                    std::vector<std::string>::size_type j = 0;
                    for (; j < vCmdLine.size() - 1; j++) {
                        if (TK_Tools::CompareNoCase(vCmdLine[j + 1], vItemCmdLine[j]) != 0) {
                            break;
                        }
                    }
                    if (j == vCmdLine.size() - 1) {
                        GS_PRINT_TO_PEER1("  %s: %s\n", mCmdFuncMap[i].pCmdLine, mCmdFuncMap[i].pDesc);
                    }
                }
            }
        } else {
            for (i = 0; i < nItemCount; i++) {
                GS_PRINT_TO_PEER1("  %s: %s\n", mCmdFuncMap[i].pCmdLine, mCmdFuncMap[i].pDesc);
            }
        }

        ret = true;
    } else {
        if (vCmdLine.size() >= 1) {
            for (i = 0; i < nItemCount; i++) {
                std::vector<std::string> vItemCmdLine = TK_Tools::SplitString(mCmdFuncMap[i].pCmdLine);
                
                if (vItemCmdLine.size() >= vCmdLine.size() 
                    || (vItemCmdLine.size() > 0 && vItemCmdLine[vItemCmdLine.size() - 1] == "...")
                    ) {
                    std::vector<std::string>::size_type j = 0;
                    for (; j < vCmdLine.size(); j++) {
                        if (vCmdLine[j] == "-") {
                            if (mCmdFuncMap[i].pFunc(vCmdLine)) {
                                ret = true;
                                break;
                            }
                        }
                        if (ret) {
                            break;
                        }
                        if (TK_Tools::CompareNoCase(vCmdLine[j], vItemCmdLine[j]) != 0) {
                            break;
                        }
                    }
                    if (j == vCmdLine.size()) {
                        if (mCmdFuncMap[i].pFunc(vCmdLine)) {
                            ret = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return ret;
}

