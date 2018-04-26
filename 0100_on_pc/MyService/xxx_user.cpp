#include "TK_sock.h"
#ifdef WIN32
#include <windows.h>
#endif // #ifdef WIN32
#include <string>
#include <vector>
#include <string.h>
#include <stdio.h>

#include "TK_Tools.h"
#include "TK_DebugServer.h"

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
#define GS_PRINT_TO_PEER1               TK_Tools::DebugServer::GetInstance(TK_Tools::DSIID_NAVI).SendTextToPeer1
#define GS_PRINT_TO_PEER2               TK_Tools::DebugServer::GetInstance(TK_Tools::DSIID_NAVI).SendTextToPeer2
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

    return TK_Tools::DebugServer::GetInstance(TK_Tools::DSIID_NAVI).SendTextToPeer2(buff);
}

// static struct {
//     bool bThreadStopped;
//     uint32_t evt;
//     uint8_t* pData;
//     uint32_t size;
//     int timeout;
//     bool (*IsCanceled)(void);
// } SendUmsgToNaviHmi_data = { true };

static void *SendUmsgToNaviHmi_ThreadProc(void *lpParameter)
{  
    return 0;
}

static BOOL SendUmsgToNaviHmi_bUseThread = TRUE;
static void SendUmsgToNaviHmi(uint32_t evt, const uint8_t* pData, uint32_t size, int timeout, bool (*IsCanceled)(void) = NULL)
{

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
            throw new std::string("TestCppException");
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

//     UINT32_T nAddr = 0;
//     UINT32_T nSize = 0;
//     std::string sFilePathName;
// 
//     if (vCmdLine.size() >= 5 && TK_Tools::CompareNoCase(vCmdLine[0], "SaveMemToFile") == 0) {
//         if (vCmdLine.size() >= 5) {
//             nAddr = (UINT32_T)TK_Tools::StrToUL(vCmdLine[2]);
//             nSize = (UINT32_T)TK_Tools::StrToUL(vCmdLine[3]);
//             sFilePathName = vCmdLine[4];
// 
//             if (nAddr == 0) {
//                 GS_MSG("*%s(): nAddr == 0!\n", __FUNCTION__);
//             } else if (nSize == 0) {
//                 GS_MSG("*%s(): nSize == 0!\n", __FUNCTION__);
//             } else {
//                 std::vector<unsigned char> content((unsigned char *)nAddr, (unsigned char *)nAddr + nSize);
//                 bool bSuc = TK_Tools::SaveContentToFile(TK_Tools::str2tstr(sFilePathName), content);
//                 if (!bSuc) {
//                     GS_MSG("*%s(): SaveContentToFile() is failed!\n", __FUNCTION__);
//                 }
//             }
//             
//             ret = TRUE;
//         }
//     }
    
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

//
// TestCodeConversion - [utf8_to_unicode|unicode_to_utf8]
//
// static BOOL GS_TestCodeConversion(std::vector<std::string> &vCmdLine)
// {    
//     BOOL ret = FALSE;
// 
//     std::string sConversion;
// 
//     if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "TestCodeConversion") == 0) {
//         if (vCmdLine.size() >= 3) {
//             sConversion = vCmdLine[2];
//             
//             if (sConversion == "utf8_to_unicode") {
//                 std::wstring sResult = TK_Tools::str2wstr("ABC");
//                 std::vector<unsigned char> aFileContent((unsigned char *)sResult.c_str(), (unsigned char *)sResult.c_str() + sResult.length() * 2);
//                 TK_Tools::SaveContentToFile("/tmp/test_code_conv_utf8_to_unicode.txt", aFileContent);
//             } else if (sConversion == "unicode_to_utf8") {
//                 std::string sResult = TK_Tools::wstr2str(L"ABC");
//                 std::vector<unsigned char> aFileContent((unsigned char *)sResult.c_str(), (unsigned char *)sResult.c_str() + sResult.length());
//                 TK_Tools::SaveContentToFile("/tmp/test_code_conv_unicode_to_utf8.txt", aFileContent);
//             } 
//             
//             ret = TRUE;
//         }
//     }
//     
//     return ret;
// }

//
// StartProc - <cmd> ...
//
extern void StartProcess(const std::wstring &sCmdLine);
static BOOL GS_StartProc(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    unsigned int i;

    std::string sCmd;
    std::string sCmdLine;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "StartProc") == 0) {
        if (vCmdLine.size() >= 3) {
            sCmd = vCmdLine[2];
            
            sCmdLine = sCmd;
            for (i = 3; i < vCmdLine.size(); ++i) {
                sCmdLine += TK_Tools::FormatStr(" %s", vCmdLine[i].c_str());
            }
            for (i = 0; i < sCmdLine.length(); ++i) {
                char &ch = sCmdLine[i];
                if (ch == '^') {
                    ch = ' ';
                }
            }
            StartProcess(TK_Tools::str2wstr(sCmdLine));
            
            ret = TRUE;
        }
    }
    
    return ret;
}

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
    
    { "StartProc - <s> ...",                &GS_StartProc,                          "StartProc - <cmd> ..." },
};
bool NAVSIM_ProcessCmdLine(const char *pCmdLine)
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

