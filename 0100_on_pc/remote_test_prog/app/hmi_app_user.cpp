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
#include <dlt/dlt.h>
#include <dlt/dlt_user_macros.h>

#include <QCoreApplication>
#include <QGuiApplication>
#include <QQuickPaintedItem> 
#include <QColor>

#include "TK_Tools.h"
#include "TK_DebugServer.h"

//#include <CorSsw_EmbRegistryIds.h>
//#include <persistence_client_library.h>
//#include <persistence_client_library_key.h>

#include "hmi_app_user.h"
#include "screens.generated"

//#include "event.h"
#include "api-hardkeys-events.h"

#define SCREEN_WIDTH        1280
#define SCREEN_HEIGHT       720

////////////////////////////////////////////////////////////////////////////////////
// general log
//
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <qlogger/qml_logger.h>

#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", basename((char *)__FILE__), __LINE__, __PRETTY_FUNCTION__); } while(0)
#define LOG_GEN_PRINTF(format,...) do { log_printf((std::string("=== LOG_GEN: [%s: %u: %s] ")+format).c_str(), basename((char *)__FILE__), __LINE__, __PRETTY_FUNCTION__, ## __VA_ARGS__); } while(0)

extern void log_printf(const char *pFormat, ...);

void log_printf(const char *pFormat, ...)
{
    int nError = 0;
    va_list args;
    int len = 0;
    char *buffer = NULL;

    if (nError == 0) {
        if (pFormat == NULL) {
            nError = 1;
        }
    }

    if (nError == 0) {
        va_start(args, pFormat);
        len = vsnprintf(NULL, 0, pFormat, args);
        if (len <= 0) {
            len = 1024;
        }
        buffer = (char *)malloc((len + 1) * sizeof(char));
        if (buffer == NULL) {
           nError = -1;
        }
        va_end(args);
    }

    if (nError == 0) {
        va_start(args, pFormat);
        vsnprintf(buffer, len + 1, pFormat, args);
        buffer[len] = '\0';
        //printf(buffer);                                                       // to be changed to any function which can output a string
        LOG_INFO(buffer);
        //printf(buffer);
        //qWarning(buffer);
        va_end(args);
    }

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////////
// misc
//
/*
SOURCES += \
    hmi_app_user.cpp \
    TK_DebugServer.cpp \
    TK_Tools.cpp \
    TK_sock.cpp \
    
HEADERS += \
    TK_DebugServer.h \
    TK_Tools.h \    
    TK_sock.h \
    hmi_app_user.h
*/

/*
[Socket]
ListenDatagram=1059
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

    return TK_Tools::DebugServer::GetInstance(TK_Tools::DSIID_NAVI).SendTextToPeer2(buff);
}

static struct {
    bool bThreadStopped;
    uint32_t evt;
    uint8_t* pData;
    uint32_t size;
    int timeout;
    bool (*IsCanceled)(void);
} SendUmsgToNaviHmi_data = { true, 0, NULL, 0, 0, NULL };

static void *SendUmsgToNaviHmi_ThreadProc(void *lpParameter)
{
    TK_UNUSED_VAR(lpParameter);
    
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

    TK_UNUSED_VAR(nArr);
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
	if (fp == NULL){
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
        //int *p = (int *)0;
        //*p = 1;
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
            TK_UNUSED_VAR(n);
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

//     if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "kp") == 0) {
//         std::string sExeName(vCmdLine[2]);
//         if (sExeName == "-1") {
//             sExeName = "TestLoadExternalSymbal_debugee.exe";
//             bSuc = TK_Tools::KillProcessByName(TK_Tools::str2tstr(sExeName));
//             if (bSuc) {
//                 GS_MSG("%s(): process \'%s\' is killed!\n", __FUNCTION__, sExeName.c_str());
//             } else {
//                 GS_MSG("*** %s(): process \'%s\' is not killed!\n", __FUNCTION__, sExeName.c_str());
//             }
//
//             sExeName = "TestLoadExternalSymbal.exe";
//             bSuc = TK_Tools::KillProcessByName(TK_Tools::str2tstr(sExeName));
//             if (bSuc) {
//                 GS_MSG("%s(): process \'%s\' is killed!\n", __FUNCTION__, sExeName.c_str());
//             } else {
//                 GS_MSG("*** %s(): process \'%s\' is not killed!\n", __FUNCTION__, sExeName.c_str());
//             }
//         } else {
//             bSuc = TK_Tools::KillProcessByName(TK_Tools::str2tstr(sExeName));
//             if (bSuc) {
//                 GS_MSG("%s(): process \'%s\' is killed!\n", __FUNCTION__, sExeName.c_str());
//             } else {
//                 GS_MSG("*** %s(): process \'%s\' is not killed!\n", __FUNCTION__, sExeName.c_str());
//             }
//         }

//           ret = TRUE;
//    }

    TK_UNUSED_VAR(vCmdLine);
    return ret;
}

//
// exec - <ExeName> <Arg1> <Arg2> ...
//
static BOOL GS_Execute(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
//     unsigned int i;
//
//     if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "exec") == 0) {
//         std::string sExePath(vCmdLine[2]);
//         std::string sArgs;
//         for (i = 3; i < vCmdLine.size(); i++) {
//             if (!sArgs.empty()) {
//                 sArgs += " ";
//                 sArgs += vCmdLine[i];
//             } else {
//                 sArgs = vCmdLine[i];
//             }
//         }
//         std::string sCmdLine = TK_Tools::FormatStr("%s %s", sExePath.c_str(), sArgs.c_str());
//         GS_MSG("%s(): sCmdLine: %s\n", __FUNCTION__, sCmdLine.c_str());
//         {
//             if (!TK_Tools::Execute(sExePath, sArgs, false)) {
//                 GS_MSG("%s(): TK_Tools::Execute() is failed!\n", __FUNCTION__);
//             }
//         }
//
//         ret = TRUE;
//     }

    TK_UNUSED_VAR(vCmdLine);
    return ret;
}

// namespace api_gui {
    // namespace audio {
        // void selectAudioSource(int id);
    // }
// }

//
// AUDIO_selectAudioSource - <nSourceId>
// enum EHmiSource
// {
//     HMI_SRC_UNKNOWN           = 0,
//     HMI_SRC_RADIO             = 1,  // FM Radio, DAB, TA, timeshifted radio
//     HMI_SRC_AUX               = 3,  // Auxiliary input
//     HMI_SRC_USB_1             = 4,  // USB 1 Audio
//     HMI_SRC_USB_VIDEO         = 6,  // USB Video
//     HMI_SRC_IPOD              = 7,  // iPod/iPhone
//     HMI_SRC_BT                = 9,  // Bluetooth A2DP streaming
//     HMI_SRC_PHONE             = 10, // Phone call
//     HMI_SRC_VOICE_RECOGNITION = 13, // Voice recognition
//     HMI_SRC_NAV               = 15, // Navigation
//     HMI_SRC_CAR_PLAY_MEDIA    = 17, // CarPlay Media
//     HMI_SRC_CAR_PLAY_PHONE    = 18, // CarPlay Phone
//     HMI_SRC_CAR_PLAY_FACETIME = 19, // CarPlay Facetime
//     HMI_SRC_CAR_PLAY_SIRI     = 20, // CarPlay SIRI
//     HMI_SRC_SD_STEREO         = 26, // SD card stereo
//     HMI_SRC_SD_VIDEO_STEREO   = 27, // SD card video stereo
//     HMI_SRC_CARLIFE_MEDIA     = 28, // Carlife media
//     HMI_SRC_CARLIFE_VR        = 29, // Carlife VR
//     HMI_SRC_TBOX              = 30, // TBox
//     HMI_SRC_STANDBY           = 31, // enter AV OFF
//     HMI_SRC_STANDBY_EXIT      = 255,// exit AV OFF
//     HMI_SRC_MAX
// };

static BOOL GS_AUDIO_selectAudioSource(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    UINT32_T nSourceId;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "AUDIO_selectAudioSource") == 0) {
        nSourceId = (UINT32_T)TK_Tools::StrToL(vCmdLine[2]);
        GS_MSG("%s(): nSourceId=%d\n", __FUNCTION__, nSourceId);
        api_gui::audio::selectAudioSource(nSourceId);
    }
    
    return ret;
}

//
// AUDIO_setSystemMute - <nMute>
//     typedef Bool SystemMute;
//
static BOOL GS_AUDIO_setSystemMute(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    UINT32_T nMute;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "AUDIO_setSystemMute") == 0) {
        nMute = (UINT32_T)TK_Tools::StrToL(vCmdLine[2]);
        GS_MSG("%s(): nSourceId=%d\n", __FUNCTION__, nMute);
        api_gui::audio::setSystemMute(nMute != 0);
    }
    
    return ret;
}

namespace api_bl {
    namespace multimedia {
        namespace browser {
            using namespace api_data::multimedia;
            void changeBrowserCategory(PlayerID playerId, BrowserCategoryType category);
        }
    }
}

//
// MEDB_changeBrowserCategory - <nPlayerId> <nCategory>
//	        enum class PlayerID : std::uint32_t
//	        {
//	            AUDIO_PLAYER,
//	            VIDEO_PLAYER,
//	            IMAGE_PLAYER,
//	            MAX
//	        };
//	        enum class BrowserCategoryType : std::uint32_t
//	        {
//	            INVALID, /**< Invalid type of browser category */
//	            ALL, /**< All type which match category with any type */
//	            FOLDERS, /**< Folders type of browser category */
//	            TRACKLISTS, /**< Tracklists type of browser category */
//	            GENRES, /**< Genres type of browser category */
//	            ARTISTS, /**< Artists type of browser category */
//	            ALBUMS, /**< Albums type of browser category */
//	            AUDIOBOOKS, /**< Audio books type of browser category */
//	            COMPOSERS, /**< Composers type of browser category */
//	            PODCASTS /**< Podcasts type of browser category */
//	        };
//
static BOOL GS_MEDB_changeBrowserCategory(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    INT32_T nPlayerId;
    INT32_T nCategory;

    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "MEDB_changeBrowserCategory") == 0) {
        nPlayerId = (INT32_T)TK_Tools::StrToL(vCmdLine[2]);
        nCategory = (INT32_T)TK_Tools::StrToL(vCmdLine[3]);
        GS_MSG("%s(): nPlayerId=%d, nCategory=%d\n", __FUNCTION__, nPlayerId, nCategory);
        using namespace api_data::multimedia;
        api_bl::multimedia::browser::changeBrowserCategory((PlayerID)nPlayerId, (BrowserCategoryType)nCategory);
    }
    
    return ret;
}

namespace api_bl {
    namespace multimedia {
        namespace browser {
            using namespace api_data::multimedia;
            void requestBrowserListItem(PlayerID playerId, Index index);
        }
    }
}

//
// MEDB_requestBrowserListItem - <nPlayerId> <nIndex>
//	        enum class PlayerID : std::uint32_t
//	        {
//	            AUDIO_PLAYER,
//	            VIDEO_PLAYER,
//	            IMAGE_PLAYER,
//	            MAX
//	        };
//
static BOOL GS_MEDB_requestBrowserListItem(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    INT32_T nPlayerId;
    INT32_T nIndex;

    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "MEDB_requestBrowserListItem") == 0) {
        nPlayerId = (INT32_T)TK_Tools::StrToL(vCmdLine[2]);
        nIndex = (INT32_T)TK_Tools::StrToL(vCmdLine[3]);
        GS_MSG("%s(): nPlayerId=%d, nIndex=%d\n", __FUNCTION__, nPlayerId, nIndex);
        using namespace api_data::multimedia;
        api_bl::multimedia::browser::requestBrowserListItem((PlayerID)nPlayerId, (Index)nIndex);
    }
    
    return ret;
}

namespace api_bl {
    namespace multimedia {
        namespace browser {
            using namespace api_data::multimedia;
            void browse(PlayerID playerId, Index index);
        }
    }
}

//
// MEDB_browse - <nPlayerId> <nIndex>
//	        enum class PlayerID : std::uint32_t
//	        {
//	            AUDIO_PLAYER,
//	            VIDEO_PLAYER,
//	            IMAGE_PLAYER,
//	            MAX
//	        };
//
static BOOL GS_MEDB_browse(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    INT32_T nPlayerId;
    INT32_T nIndex;

    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "MEDB_browse") == 0) {
        nPlayerId = (INT32_T)TK_Tools::StrToL(vCmdLine[2]);
        nIndex = (INT32_T)TK_Tools::StrToL(vCmdLine[3]);
        GS_MSG("%s(): nPlayerId=%d, nIndex=%d\n", __FUNCTION__, nPlayerId, nIndex);
        using namespace api_data::multimedia;
        api_bl::multimedia::browser::browse((PlayerID)nPlayerId, (Index)nIndex);
    }
    
    return ret;
}

namespace api_bl {
    namespace multimedia {
        namespace browser {
            using namespace api_data::multimedia;
            void top(PlayerID playerId);
        }
    }
}

//
// MEDB_top - <nPlayerId>
//	        enum class PlayerID : std::uint32_t
//	        {
//	            AUDIO_PLAYER,
//	            VIDEO_PLAYER,
//	            IMAGE_PLAYER,
//	            MAX
//	        };
//
static BOOL GS_MEDB_top(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    INT32_T nPlayerId;
    INT32_T nIndex;

    TK_UNUSED_VAR(nIndex);

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "MEDB_top") == 0) {
        nPlayerId = (INT32_T)TK_Tools::StrToL(vCmdLine[2]);
        GS_MSG("%s(): nPlayerId=%d\n", __FUNCTION__, nPlayerId);
        using namespace api_data::multimedia;
        api_bl::multimedia::browser::top((PlayerID)nPlayerId);
    }
    
    return ret;
}

namespace api_bl {
    namespace multimedia {
        namespace browser {
            using namespace api_data::multimedia;
            void up(PlayerID playerId);
        }
    }
}

//
// MEDB_up - <nPlayerId>
//	        enum class PlayerID : std::uint32_t
//	        {
//	            AUDIO_PLAYER,
//	            VIDEO_PLAYER,
//	            IMAGE_PLAYER,
//	            MAX
//	        };
//
static BOOL GS_MEDB_up(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    INT32_T nPlayerId;
    INT32_T nIndex;

    TK_UNUSED_VAR(nIndex);

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "MEDB_up") == 0) {
        nPlayerId = (INT32_T)TK_Tools::StrToL(vCmdLine[2]);
        GS_MSG("%s(): nPlayerId=%d\n", __FUNCTION__, nPlayerId);
        using namespace api_data::multimedia;
        api_bl::multimedia::browser::up((PlayerID)nPlayerId);
    }
    
    return ret;
}

namespace api_bl {
    namespace multimedia {
        namespace browser {
            using namespace api_data::multimedia;
            void setPath(PlayerID playerId, const String &path);
        }
    }
}

//
// MEDB_setPath - <nPlayerId> <sPath>
//	        enum class PlayerID : std::uint32_t
//	        {
//	            AUDIO_PLAYER,
//	            VIDEO_PLAYER,
//	            IMAGE_PLAYER,
//	            MAX
//	        };
//
static BOOL GS_MEDB_setPath(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    INT32_T nPlayerId;
    std::string sPath;

    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "MEDB_setPath") == 0) {
        nPlayerId = (INT32_T)TK_Tools::StrToL(vCmdLine[2]);
        sPath = vCmdLine[3];
        GS_MSG("%s(): nPlayerId=%d, sPath=%s\n", __FUNCTION__, nPlayerId, sPath.c_str());
        using namespace api_data::multimedia;
        api_bl::multimedia::browser::setPath((PlayerID)nPlayerId, sPath);
    }
    
    return ret;
}

using namespace api_data::hardkeys;
#define HARK_KEY_MAP_ITEM(name) { #name, &name }

static struct HarkKeyMapItem {
    const std::string sEventName;
    const event::Event *pEvent;
} harkKeyMap[] = {
    HARK_KEY_MAP_ITEM(HK_ANY_P), 
    HARK_KEY_MAP_ITEM(HK_POWER_SP),
    HARK_KEY_MAP_ITEM(HK_POWER_LP),
    HARK_KEY_MAP_ITEM(HK_MENU_SP),
    HARK_KEY_MAP_ITEM(HK_MENU_LP),
    HARK_KEY_MAP_ITEM(HK_BACK_SP),
    HARK_KEY_MAP_ITEM(HK_BACK_LP),
    HARK_KEY_MAP_ITEM(HK_POWER_ROT_CLOCKWISE),
    HARK_KEY_MAP_ITEM(HK_POWER_ROT_COUNTERCLOCKWISE),
    HARK_KEY_MAP_ITEM(HK_NAVI_SP),
    HARK_KEY_MAP_ITEM(HK_NAVI_LP),
    HARK_KEY_MAP_ITEM(HK_IDRIVE_PHONE_SP),
    HARK_KEY_MAP_ITEM(HK_IDRIVE_PHONE_VVLPW),
    HARK_KEY_MAP_ITEM(HK_MEDIA_SP),
    HARK_KEY_MAP_ITEM(HK_MEDIA_LP),
    HARK_KEY_MAP_ITEM(HK_RADIO_SP),
    HARK_KEY_MAP_ITEM(HK_RADIO_LP),
    HARK_KEY_MAP_ITEM(HK_SETUP_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_MID_P),
    HARK_KEY_MAP_ITEM(HK_CONTROL_MID_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_MID_LP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_MID_HP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_MID_HPR),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UP_P),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UP_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UP_HP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UP_HPR),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UPRIGHT_P),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UPRIGHT_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UPRIGHT_HP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UPRIGHT_HPR),
    HARK_KEY_MAP_ITEM(HK_CONTROL_RIGHT_P),
    HARK_KEY_MAP_ITEM(HK_CONTROL_RIGHT_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_RIGHT_HP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_RIGHT_HPR),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWNRIGHT_P),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWNRIGHT_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWNRIGHT_HP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWNRIGHT_HPR),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWN_P),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWN_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWN_HP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWN_HPR),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWNLEFT_P),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWNLEFT_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWNLEFT_HP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_DOWNLEFT_HPR),
    HARK_KEY_MAP_ITEM(HK_CONTROL_LEFT_P),
    HARK_KEY_MAP_ITEM(HK_CONTROL_LEFT_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_LEFT_HP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_LEFT_HPR),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UPLEFT_P),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UPLEFT_SP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UPLEFT_HP),
    HARK_KEY_MAP_ITEM(HK_CONTROL_UPLEFT_HPR),
    HARK_KEY_MAP_ITEM(HK_CONTROL_KNOB_CENTERED),
    HARK_KEY_MAP_ITEM(HK_CONTROL_CLOCKWISE),
    HARK_KEY_MAP_ITEM(HK_CONTROL_COUNTERCLOCKWISE),
    HARK_KEY_MAP_ITEM(HK_VOL_UP_SP),
    HARK_KEY_MAP_ITEM(HK_VOL_UP_HP),
    HARK_KEY_MAP_ITEM(HK_VOL_UP_HPR),
    HARK_KEY_MAP_ITEM(HK_VOL_DOWN_SP),
    HARK_KEY_MAP_ITEM(HK_VOL_DOWN_HP),
    HARK_KEY_MAP_ITEM(HK_VOL_DOWN_HPR),
    HARK_KEY_MAP_ITEM(HK_PREV_SP),
    HARK_KEY_MAP_ITEM(HK_PREV_LP),
    HARK_KEY_MAP_ITEM(HK_PREV_HP),
    HARK_KEY_MAP_ITEM(HK_PREV_HPR),
    HARK_KEY_MAP_ITEM(HK_NEXT_SP),
    HARK_KEY_MAP_ITEM(HK_NEXT_LP),
    HARK_KEY_MAP_ITEM(HK_NEXT_HP),
    HARK_KEY_MAP_ITEM(HK_NEXT_HPR),
    HARK_KEY_MAP_ITEM(HK_MODE_SP),
    HARK_KEY_MAP_ITEM(HK_MODE_LP),
    HARK_KEY_MAP_ITEM(HK_SWRC_PHONE_SP),
    HARK_KEY_MAP_ITEM(HK_SWRC_PHONE_LP),
    HARK_KEY_MAP_ITEM(HK_SWRC_PHONE_VVLPW),
    HARK_KEY_MAP_ITEM(HK_VR_P),
    HARK_KEY_MAP_ITEM(HK_VR_SP),
    HARK_KEY_MAP_ITEM(HK_VR_LP),
    HARK_KEY_MAP_ITEM(HK_SEAT_SP),
    HARK_KEY_MAP_ITEM(HK_SEAT_LP),
    HARK_KEY_MAP_ITEM(HK_CAMERA_SP),
    HARK_KEY_MAP_ITEM(HK_CAMERA_LP),
    HARK_KEY_MAP_ITEM(HK_START_UPDATE_SP),
    HARK_KEY_MAP_ITEM(HK_PRINT_SCREEN_SP),
    HARK_KEY_MAP_ITEM(HK_START_UPDATE_SMP),
    HARK_KEY_MAP_ITEM(HK_PRINT_SCREEN_SMP),
    HARK_KEY_MAP_ITEM(HK_ENGMODE_SP),
    HARK_KEY_MAP_ITEM(HK_ENGMODE_P),
    HARK_KEY_MAP_ITEM(HK_ENGMODE_R),
    HARK_KEY_MAP_ITEM(HK_ENGMODE_SMP),
    HARK_KEY_MAP_ITEM(HK_LOGGING_SP),
    HARK_KEY_MAP_ITEM(HK_LOGGING_P),
    HARK_KEY_MAP_ITEM(HK_LOGGING_R),
    HARK_KEY_MAP_ITEM(HK_LOGGING_SMP),
    HARK_KEY_MAP_ITEM(HK_OK_SP),
    HARK_KEY_MAP_ITEM(HK_OK_LP),
    HARK_KEY_MAP_ITEM(HK_RETURN_SP),
    HARK_KEY_MAP_ITEM(HK_RETURN_LP),
};

static const event::Event *findEvent(const std::string sEventName)
{
    const event::Event *pEvent = NULL;
    unsigned int nSize = TK_ARR_LEN(harkKeyMap);
    unsigned int i;

    for (i = 0; i < nSize; i++) {
        if (harkKeyMap[i].sEventName == sEventName) {
            pEvent = harkKeyMap[i].pEvent;
        }
    }

    return pEvent;
}

//
// listHkEvents - <sKeyword>|all
//
static BOOL GS_listHkEvents(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sKeyword;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "listHkEvents") == 0) {
        if (vCmdLine.size() >= 3) {
            sKeyword = vCmdLine[2];
        }
        if (sKeyword == "all") {
            sKeyword.clear();
        }
        
        GS_MSG("Available HKs:\n");
        if (sKeyword.empty()) {
            for (unsigned int i = 0; i < TK_ARR_LEN(harkKeyMap); i++) {
                struct HarkKeyMapItem *item = &harkKeyMap[i];
                GS_MSG("    %s\n", item->sEventName.c_str());
            }
        } else {
            for (unsigned int i = 0; i < TK_ARR_LEN(harkKeyMap); i++) {
                struct HarkKeyMapItem *item = &harkKeyMap[i];
                std::string sUpCaseKeyword = TK_Tools::UpperCase(sKeyword);
                std::string sUpCaseEventName = TK_Tools::UpperCase(item->sEventName);
                std::string::size_type nPos = sUpCaseEventName.find(sUpCaseKeyword);
                if (nPos != std::string::npos) {
                    GS_MSG("    %s\n", item->sEventName.c_str());
                }
            }
        }
    }
    
    return ret;
}

static std::vector<unsigned int> GS_PostHkEvent__FindEvents(const std::string &sKeyword)
{
    std::vector<unsigned int> matchedEventIndexes;
    
    for (unsigned int i = 0; i < TK_ARR_LEN(harkKeyMap); i++) {
        struct HarkKeyMapItem *item = &harkKeyMap[i];
        std::string sUpCaseKeyword = TK_Tools::UpperCase(sKeyword);
        std::string sUpCaseEventName = TK_Tools::UpperCase(item->sEventName);
        if (sUpCaseEventName == sUpCaseKeyword) {
            matchedEventIndexes.clear();
            matchedEventIndexes.push_back(i);
            break;
        }
        
        std::string::size_type nPos = sUpCaseEventName.find(sUpCaseKeyword);
        if (nPos != std::string::npos) {
            matchedEventIndexes.push_back(i);
        }
    }
    
    return matchedEventIndexes;
}

//
// PostHkEvent - <sEventKeyword>
//
static BOOL GS_PostHkEvent(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sEventKeyword;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "PostHkEvent") == 0) {
        if (vCmdLine.size() >= 3) {
            if (sEventKeyword != "-") {
                sEventKeyword = vCmdLine[2];
            }
        }
        GS_MSG("%s(): sEventKeyword=%s\n", __FUNCTION__, sEventKeyword.c_str());
        
        std::vector<unsigned int> matchedEventIndexes = GS_PostHkEvent__FindEvents(sEventKeyword);
        if (matchedEventIndexes.size() == 1) {
            struct HarkKeyMapItem *item = &harkKeyMap[matchedEventIndexes[0]];
            api::post(*item->pEvent);
            GS_MSG("%s(): The event %s is posted.\n", __FUNCTION__, item->sEventName.c_str());
        } else if (matchedEventIndexes.size() == 0) {
            GS_MSG("%s(): *** No matched events: %s\n", __FUNCTION__, sEventKeyword.c_str());
        } else {
            GS_MSG("%s(): *** Too many matched events:\n", __FUNCTION__);
            for (unsigned int i = 0; i < matchedEventIndexes.size(); i++) {
                struct HarkKeyMapItem *item = &harkKeyMap[matchedEventIndexes[i]];
                GS_MSG("  %s\n", item->sEventName.c_str());
            }
        }
    }
    
    return ret;
}

//
// triggerUserPerceivedOff
//
namespace api_bl
{
    namespace system
    {
        void updateLifeCycleState(const api_data::system::LifeCycleState &state);
    }
}
static BOOL GS_triggerUserPerceivedOff(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sEvent;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "triggerUserPerceivedOff") == 0) {
        GS_MSG("%s()\n", __FUNCTION__);
        api_bl::system::updateLifeCycleState(api_data::system::LifeCycleState::USER_PERCEIVED_OFF);
    }
    
    return ret;
}

//
// restartSystem
//
namespace api_bl
{
    namespace system
    {
        void restartSystem();
    }
}
static BOOL GS_restartSystem(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sEvent;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "restartSystem") == 0) {
        GS_MSG("%s()\n", __FUNCTION__);
        api_bl::system::restartSystem();
    }
    
    return ret;
}

//
// restartWithVisualOff
//
static BOOL GS_restartWithVisualOff(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sEvent;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "restartWithVisualOff") == 0) {
        GS_MSG("%s()\n", __FUNCTION__);
        api_bl::system::updateLifeCycleState(api_data::system::LifeCycleState::USER_PERCEIVED_OFF);
        sleep(3);
        api_bl::system::restartSystem();
    }
    
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// some ERGs for NS_ReadErgData() and NS_WriteErgData()
//
// static bool GS_InitErgLib_bInit = false;
// static std::string GS_InitErgLib_sAppName = "ovip-sys-veh-drive-app";
// static void GS_InitErgLib()
// {
//     int nRet;
//
//     if (!GS_InitErgLib_bInit) {
//         nRet = pclInitLibrary(GS_InitErgLib_sAppName.c_str(), PCL_SHUTDOWN_TYPE_NONE);
//         if (nRet < 0) {
//             GS_MSG("*%s(): pclInitLibrary() is failed! [ret=%d]\n", __FUNCTION__, nRet);
//         } else {
//             GS_MSG("%s(): pclInitLibrary() is successful! [AppName=%s]\n", __FUNCTION__, GS_InitErgLib_sAppName.c_str());
//             GS_InitErgLib_bInit = true;
//         }
//     }
// }

// static void GS_DeinitErgLib()
// {
//     int nRet;
//
//     if (GS_InitErgLib_bInit) {
//         nRet = pclDeinitLibrary();
//         if (nRet < 0) {
//             GS_MSG("*%s(): pclDeinitLibrary() is failed! [ret=%d]\n", __FUNCTION__, nRet);
//         } else {
//             GS_MSG("%s(): pclDeinitLibrary() is successful! [AppName=%s]\n", __FUNCTION__, GS_InitErgLib_sAppName.c_str());
//             GS_InitErgLib_bInit = false;
//         }
//     }
// }

//
// SetErgAppName - <sAppName>
//
//static BOOL GS_SetErgAppName(std::vector<std::string> &vCmdLine)
//{
// BOOL ret = FALSE;
//
// std::string sAppName;
//
// if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "SetErgAppName") == 0) {
//     if (vCmdLine.size() >= 3) {
//         GS_DeinitErgLib();
//         GS_InitErgLib_sAppName = vCmdLine[2];
//         GS_InitErgLib();
//
//         ret = TRUE;
//
//     }
// }
//
// return ret;
//}

//
// ReadErgData - <nLDBID> <sName>
//
//static BOOL GS_ReadErgData(std::vector<std::string> &vCmdLine)
//{
//    BOOL ret = FALSE;

//    unsigned int ldbid = (unsigned int)-1;
//    std::string sName;
//    int nDataSize = 0;
//    int nDataSizeRead = 0;
//    uint8_t *pDataBuf = NULL;
//
//    GS_InitErgLib();
//
//    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "ReadErgData") == 0) {
//        if (vCmdLine.size() >= 4) {
//            ldbid = (unsigned int)TK_Tools::StrToUL(vCmdLine[2]);
//            sName = vCmdLine[3];
//
//            nDataSize = pclKeyGetSize(ldbid, sName.c_str(), 0, 0);
//            if (nDataSize > 0) {
//                pDataBuf = new uint8_t[nDataSize];
//                if (pDataBuf != NULL) {
//                    nDataSizeRead = pclKeyReadData(ldbid, sName.c_str(), 0, 0, pDataBuf, nDataSize);
//                    if (nDataSizeRead == nDataSize) {
//                        GS_MSG("%s(): [0x%08X] Read ERG value (hex):", __FUNCTION__, ldbid);
//                        for (unsigned int i = 0; i < nDataSizeRead; i++) {
//                            GS_MSG(" %02X", pDataBuf[i]);
//                        }
//                        GS_MSG("\n");
//                    } else {
//                        GS_MSG("*%s(): pclKeyReadData() is failed! [ret=%d]\n", __FUNCTION__, nDataSizeRead);
//                    }
//                } else {
//                    GS_MSG("*%s(): new is failed!\n", __FUNCTION__);
//                }
//            } else {
//                GS_MSG("*%s(): pclKeyGetSize() is failed! [ret=%d]\n", __FUNCTION__, nDataSize);
//            }

//            ret = TRUE;
//        }
//    }
//
//    if (pDataBuf != NULL) {
//        delete [] pDataBuf;
//    }
//
//    return ret;
//}

//
// WriteErgData - <nLDBID> <sName> <nValueByte1> ...
//
//static BOOL GS_WriteErgData(std::vector<std::string> &vCmdLine)
//{
//    BOOL ret = FALSE;
//
//    unsigned int ldbid = (unsigned int)-1;
//    std::string sName;
//    int nDataSize = 0;
//    int nDataSizeWritten = 0;
//    uint8_t *pDataBuf = NULL;
//
//    GS_InitErgLib();
//
//    if (vCmdLine.size() >= 5 && TK_Tools::CompareNoCase(vCmdLine[0], "WriteErgData") == 0) {
//        if (vCmdLine.size() >= 5) {
//            ldbid = (unsigned int)TK_Tools::StrToUL(vCmdLine[2]);
//            sName = vCmdLine[3];
//
//            nDataSize = vCmdLine.size() - 4;
//            pDataBuf = new uint8_t[nDataSize];
//            if (pDataBuf != NULL) {
//                for (unsigned int i = 0; i < nDataSize; i++) {
//                    pDataBuf[i] = (uint8_t)TK_Tools::StrToUL(vCmdLine[i + 4]);
//                }
//
//                nDataSizeWritten = pclKeyWriteData(ldbid, sName.c_str(), 0, 0, pDataBuf, nDataSize);
//                if (nDataSizeWritten == nDataSize) {
//                    GS_MSG("%s(): [0x%08X] Write ERG value (hex):", __FUNCTION__, ldbid);
//                    for (unsigned int i = 0; i < nDataSize; i++) {
//                        GS_MSG(" %02X", pDataBuf[i]);
//                    }
//                    GS_MSG("\n");
//                } else {
//                    GS_MSG("*%s(): pclKeyWriteData() is failed! [ret=%d]\n", __FUNCTION__, nDataSizeWritten);
//                }
//            } else {
//                GS_MSG("*%s(): new is failed!\n", __FUNCTION__);
//            }
//
//            ret = TRUE;
//        }
//    }
//
//    if (pDataBuf != NULL) {
//        delete [] pDataBuf;
//    }
//
//    return ret;
//}

//
// TestCodeConversion - [utf8_to_unicode|unicode_to_utf8]
//
static BOOL GS_TestCodeConversion(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    std::string sConversion;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "TestCodeConversion") == 0) {
        if (vCmdLine.size() >= 3) {
            sConversion = vCmdLine[2];
            
            if (sConversion == "utf8_to_unicode") {
                std::wstring sResult = TK_Tools::str2wstr("ABC");
                std::vector<unsigned char> aFileContent((unsigned char *)sResult.c_str(), (unsigned char *)sResult.c_str() + sResult.length() * 2);
                TK_Tools::SaveContentToFile("/tmp/test_code_conv_utf8_to_unicode.txt", aFileContent);
            } else if (sConversion == "unicode_to_utf8") {
                std::string sResult = TK_Tools::wstr2str(L"ABC");
                std::vector<unsigned char> aFileContent((unsigned char *)sResult.c_str(), (unsigned char *)sResult.c_str() + sResult.length());
                TK_Tools::SaveContentToFile("/tmp/test_code_conv_unicode_to_utf8.txt", aFileContent);
            } 
            
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// DebugSetMagicValue - <nValue>
//
int g_nMagicValue = 0;
static BOOL GS_DebugSetMagicValue(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    int nMagicValue = 0;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "DebugSetMagicValue") == 0) {
        nMagicValue = TK_Tools::StrToL(vCmdLine[2]);
        GS_MSG("%s(): nMagicValue=%d\n", __FUNCTION__, nMagicValue);
        g_nMagicValue = nMagicValue;
    }
    
    return ret;
}

//
// VEHI_updateGWMDriveMode - <nMode>
//
namespace api_bl
{
    namespace vehicle
    {
        namespace status
        {
            using namespace api_data::vehicle::status;
            void updateGWMDrivingMode(GWMDrivingMode mode);
        }
    }
}
static BOOL GS_VEHI_updateGWMDriveMode(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    UINT_T nMode = 0;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "VEHI_updateGWMDriveMode") == 0) {
        nMode = TK_Tools::StrToUL(vCmdLine[2]);
        
        GS_MSG("%s(): nMode=%lu\n", __FUNCTION__, nMode);
        api_bl::vehicle::status::updateGWMDrivingMode((api_data::vehicle::status::GWMDrivingMode)nMode);
        
        ret = TRUE;
    }
    
    return ret;
}

//
// VEHI_updateAcDispState - <bState>
//
namespace api_bl
{
    namespace vehicle
    {
        namespace climate
        {
            using namespace api_data::vehicle;
            void updateAcDispState(State x);
        }
    }
}
static BOOL GS_VEHI_updateAcDispState(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    UINT_T nState = 0;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "VEHI_updateAcDispState") == 0) {
        nState = TK_Tools::StrToUL(vCmdLine[2]);
        
        GS_MSG("%s(): nState=%lu\n", __FUNCTION__, nState);
        api_bl::vehicle::climate::updateAcDispState((api_data::vehicle::State)nState);
        
        ret = TRUE;
    }
    
    return ret;
}

//
// NAVI_prepareShowNav - <nDestScreen>
//
namespace api_bl
{
    namespace navigation
    {
        void prepareShowNav(EDestScreen destScn);
    }
}
static BOOL GS_NAVI_prepareShowNav(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    UINT_T nDestScreen = 0;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NAVI_prepareShowNav") == 0) {
        nDestScreen = TK_Tools::StrToUL(vCmdLine[2]);
        
        GS_MSG("%s(): nDestScreen=%lu\n", __FUNCTION__, nDestScreen);
        api_bl::navigation::prepareShowNav((api_data::navigation::EDestScreen)nDestScreen);
        
        ret = TRUE;
    }
    
    return ret;
}

//
// c_exit: exit process
//
static BOOL GS_c_exit(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "c_exit") == 0) {
        GS_MSG("%s()\n", __FUNCTION__);
        exit(0);
        return TRUE;
    }
    
    return ret;
}


//
// qt_exit: exit process
//
//namespace event
//{
//    class Event;
//}
//namespace QtGlue
//{
//    class EventPod;
//}
//namespace QtGlue
//{
//    class EventProcessor
//    {
//    public:
//        virtual void process(const EventPod& ep) = 0;
//
//        virtual ~EventProcessor(){}
//    };
//}
//namespace QtGlue
//{
//    template <class EventType>
//    class QtCore : public QtGlue::EventProcessor
//    {
//    public:
//        QGuiApplication qapp_;
//        unsigned long x[200];
//    };
//
//}
//namespace gwmGui
//{
//  typedef QtGlue::QtCore<event::Event> GwmGuiSM;
//}

gwmGui::GwmGuiSM *pGwmGuiSM = NULL;
static BOOL GS_qt_exit(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "qt_exit") == 0) {
        GS_MSG("%s()\n", __FUNCTION__);
        //QCoreApplication::exit(0);
        if (pGwmGuiSM != NULL) {
            GS_MSG("%s(): pGwmGuiSM->qapp_.quit()\n", __FUNCTION__);
            // pGwmGuiSM->qapp_.quit();
        }
        return TRUE;
    }
    
    return ret;
}

//
// dialNumber - <sNumber>
//
namespace api_bl
{
    namespace connectivity
    {
        namespace call
        {
            void dialNumber(const std::string &number);
        }
    }
}
static BOOL GS_dialNumber(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sNumber;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "dialNumber") == 0) {
        sNumber = vCmdLine[2];
        GS_MSG("%s(): sNumber=%s\n", __FUNCTION__, sNumber.c_str());
        api_bl::connectivity::call::dialNumber(sNumber);
        
        ret = TRUE;
    }
    
    return ret;
}

//
// listScreenIds - <keyWokd>|all
//
static BOOL GS_listScreenIds(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    unsigned int i;
    std::string sKeyword;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "listScreenIds") == 0) {
        if (vCmdLine.size() >= 3) {
            sKeyword = vCmdLine[2];
        }
        if (sKeyword == "all") {
            sKeyword = "";
        }
    
        GS_MSG("Available Screen IDs:\n");
        for (i = 0; i < ScreenId::SCREEN_COUNT; i++) {
            if (sKeyword.empty()) {
                GS_MSG("    %s\n", SCREEN_ID_NAME[i]);
            } else {
                std::string sUpCaseKeyword = TK_Tools::UpperCase(sKeyword);
                std::string sUpCaseScreenIdName = TK_Tools::UpperCase(SCREEN_ID_NAME[i]);
                std::string::size_type nPos = sUpCaseScreenIdName.find(sUpCaseKeyword);
                if (nPos != std::string::npos) {
                    GS_MSG("    %s\n", SCREEN_ID_NAME[i]);
                }
            }
        }
        ret = TRUE;
    }
    
    return ret;
}

static std::map<unsigned int, std::string> GetMatchedScreens(const std::string &sKeyword)
{
    std::map<unsigned int, std::string> screens;
    unsigned int i;
    
    for (i = 0; i < ScreenId::SCREEN_COUNT; i++) {
        if (sKeyword.empty()) {
            screens[i] = SCREEN_ID_NAME[i];
        } else {
            std::string sUpCaseKeyword = TK_Tools::UpperCase(sKeyword);
            std::string sUpCaseScreenIdName = TK_Tools::UpperCase(SCREEN_ID_NAME[i]);
            if (sUpCaseScreenIdName == sUpCaseKeyword) {    // The exact one!
                screens.clear();
                screens[i] = SCREEN_ID_NAME[i];
                break;
            }
            
            std::string::size_type nPos = sUpCaseScreenIdName.find(sUpCaseKeyword);
            if (nPos != std::string::npos) {
                screens[i] = SCREEN_ID_NAME[i];
            }
        }
    }
    
    return screens;
}

static std::string GetScreensInfoStr(std::map<unsigned int, std::string> &screens)
{
    std::string sRet;
    std::map<unsigned int, std::string>::iterator it;
    
    for (it = screens.begin(); it != screens.end(); ++it) {
        sRet += TK_Tools::FormatStr("  %s\n", (*it).second.c_str());
    }
    
    return sRet;
}

//
// showScreen - <sScreenId>
//
static BOOL GS_showScreen(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sScreenId;
    unsigned int nScreenId;
    std::string nScreenName;
    std::map<unsigned int, std::string> screens;
    std::map<unsigned int, std::string>::iterator it;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "showScreen") == 0) {
        sScreenId = vCmdLine[2];
        GS_MSG("%s(): sScreenId=%s\n", __FUNCTION__, sScreenId.c_str());
        
        screens = GetMatchedScreens(sScreenId);
        if (screens.size() == 1) {
            it = screens.begin();
            nScreenId = (*it).first;
            nScreenName = (*it).second;
            StubScreen *pStubScreen = MainThreadInterceptor::getStubScreen();
            if (pStubScreen == NULL) {
                GS_MSG("%s(): *** pStubScreen=NULL\n", __FUNCTION__);
            } else {
                pStubScreen->showScreen((gwmGui::ScreenId::ScreenIdEnum)nScreenId);
                GS_MSG("  %s is shown.\n", nScreenName.c_str());
                ret = TRUE;
            }
        } else if (screens.size() == 0) {
            GS_MSG("%s(): *** no screen matched.\n", __FUNCTION__);
        } else {
            GS_MSG("%s(): *** too many screens matched:\n", __FUNCTION__);
            GS_MSG("%s", GetScreensInfoStr(screens).c_str());
        }
    }
    
    return ret;
}

//
// switchScreen - <sScreenId>
//
static BOOL GS_switchScreen(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sScreenId;
    unsigned int nScreenId;
    std::string nScreenName;
    std::map<unsigned int, std::string> screens;
    std::map<unsigned int, std::string>::iterator it;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "switchScreen") == 0) {
        sScreenId = vCmdLine[2];
        GS_MSG("%s(): sScreenId=%s\n", __FUNCTION__, sScreenId.c_str());
        
        screens = GetMatchedScreens(sScreenId);
        if (screens.size() == 1) {
            it = screens.begin();
            nScreenId = (*it).first;
            nScreenName = (*it).second;
            StubScreen *pStubScreen = MainThreadInterceptor::getStubScreen();
            if (pStubScreen == NULL) {
                GS_MSG("%s(): *** pStubScreen=NULL\n", __FUNCTION__);
            } else {
                pStubScreen->switchScreen((gwmGui::ScreenId::ScreenIdEnum)nScreenId);
                GS_MSG("  %s is shown.\n", nScreenName.c_str());
                ret = TRUE;
            }
        } else if (screens.size() == 0) {
            GS_MSG("%s(): *** no screen matched.\n", __FUNCTION__);
        } else {
            GS_MSG("%s(): *** too many screens matched:\n", __FUNCTION__);
            GS_MSG("%s", GetScreensInfoStr(screens).c_str());
        }
    }
    
    return ret;
}

//
// showPopup - <sScreenId>
//
static BOOL GS_showPopup(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sScreenId;
    unsigned int nScreenId;
    std::string nScreenName;
    std::map<unsigned int, std::string> screens;
    std::map<unsigned int, std::string>::iterator it;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "showPopup") == 0) {
        sScreenId = vCmdLine[2];
        GS_MSG("%s(): sScreenId=%s\n", __FUNCTION__, sScreenId.c_str());
        
        screens = GetMatchedScreens(sScreenId);
        if (screens.size() == 1) {
            it = screens.begin();
            nScreenId = (*it).first;
            nScreenName = (*it).second;
            StubScreen *pStubScreen = MainThreadInterceptor::getStubScreen();
            if (pStubScreen == NULL) {
                GS_MSG("%s(): *** pStubScreen=NULL\n", __FUNCTION__);
            } else {
                pStubScreen->showPopup((gwmGui::ScreenId::ScreenIdEnum)nScreenId);
                GS_MSG("  %s is shown.\n", nScreenName.c_str());
                ret = TRUE;
            }
        } else if (screens.size() == 0) {
            GS_MSG("%s(): *** no screen matched.\n", __FUNCTION__);
        } else {
            GS_MSG("%s(): *** too many screens matched:\n", __FUNCTION__);
            GS_MSG("%s", GetScreensInfoStr(screens).c_str());
        }
    }
    
    return ret;
}

//
// hidePopup - <sScreenId>
//
static BOOL GS_hidePopup(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sScreenId;
    unsigned int nScreenId;
    std::string nScreenName;
    std::map<unsigned int, std::string> screens;
    std::map<unsigned int, std::string>::iterator it;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "hidePopup") == 0) {
        sScreenId = vCmdLine[2];
        GS_MSG("%s(): sScreenId=%s\n", __FUNCTION__, sScreenId.c_str());
        
        screens = GetMatchedScreens(sScreenId);
        if (screens.size() == 1) {
            it = screens.begin();
            nScreenId = (*it).first;
            nScreenName = (*it).second;
            StubScreen *pStubScreen = MainThreadInterceptor::getStubScreen();
            if (pStubScreen == NULL) {
                GS_MSG("%s(): *** pStubScreen=NULL\n", __FUNCTION__);
            } else {
                pStubScreen->hidePopup((gwmGui::ScreenId::ScreenIdEnum)nScreenId);
                GS_MSG("  %s is hidden.\n", nScreenName.c_str());
                ret = TRUE;
            }
        } else if (screens.size() == 0) {
            GS_MSG("%s(): *** no screen matched.\n", __FUNCTION__);
        } else {
            GS_MSG("%s(): *** too many screens matched:\n", __FUNCTION__);
            GS_MSG("%s", GetScreensInfoStr(screens).c_str());
        }
    }
    
    return ret;
}

//
// currentScreens
//
static BOOL GS_currentScreens(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    unsigned int i;
    int nScreenLevel;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "currentScreens") == 0) {
        GS_MSG("%s(): \n", __FUNCTION__);
        
        for (i = 0; i < ScreenId::SCREEN_COUNT; i++) {
            StubScreen *pStubScreen = MainThreadInterceptor::getStubScreen();
            if (pStubScreen != NULL) {
                if (pStubScreen->isShown((gwmGui::ScreenId::ScreenIdEnum)i)) {
                    nScreenLevel = getScreenLevel(i);
                    GS_MSG("    %s: %d\n", SCREEN_ID_NAME[i], nScreenLevel);
                }
            }
        }
        
        ret = TRUE;
    }
    
    return ret;
}

namespace {

class TestQmlView
{
public:
    class PaintedItem : public QQuickPaintedItem
    {
    public:
         PaintedItem();
         virtual void paint(QPainter *painter) { TK_UNUSED_VAR(painter); }
    protected:
         virtual void mousePressEvent(QMouseEvent *event);
    };

public:
    TestQmlView(const std::string &sQmlFilePath = "", unsigned int nZOrder = 0, const std::string &sBgColor = "#00000000") 
        : m_sQmlFilePath(sQmlFilePath), m_nZOrder(nZOrder), m_sBgColor(sBgColor), 
          m_pQmlComponent(NULL), m_pQmlContext(NULL), m_pQuickItem(NULL) {}
    ~TestQmlView();
    
    bool show();
    bool hide();
    
private:
    const std::string m_sQmlFilePath;
    unsigned int m_nZOrder;
    std::string m_sBgColor;
    
    QQmlComponent *m_pQmlComponent;
    QQmlContext *m_pQmlContext;
    QQuickItem *m_pQuickItem;
};

TestQmlView::PaintedItem::PaintedItem() : QQuickPaintedItem()
{
    setActiveFocusOnTab(true);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);
}

void TestQmlView::PaintedItem::mousePressEvent(QMouseEvent *event)
{
    QQuickItem *pRootQuickItem;
    int x = event->globalX();
    int y = event->globalY();
    
    setVisible(false);
    
    pRootQuickItem = getRootQuickItem();
    if (pRootQuickItem != NULL) {
        QQuickItem *pChildItem = pRootQuickItem->childAt(x, y);
        if (pChildItem != NULL) {
            GS_MSG("%s(): pChildItem=0x%08X at (x=%d, y=%d)\n", __FUNCTION__, pChildItem, x, y);
        } else {
            GS_MSG("%s(): *** pChildItem=0x%08X at (x=%d, y=%d)\n", __FUNCTION__, pChildItem, x, y);
        }
    }
    
    setVisible(true);
}

TestQmlView::~TestQmlView()
{
    hide();
    
    if (m_pQuickItem != NULL) {
        delete m_pQuickItem;
        m_pQuickItem = NULL;
    }
    
    if (m_pQmlContext != NULL) {
        delete m_pQmlContext;
        m_pQmlContext = NULL;
    }
    
    if (m_pQmlComponent != NULL) {
        delete m_pQmlComponent;
        m_pQmlComponent = NULL;
    }
}

bool TestQmlView::show()
{
    bool bRet = true;
    QQmlContext *pRootQmlContext = NULL;
    QQuickWindow *pMainQmlWindow = NULL;
    QQuickItem *pParentQuickItem  = NULL;
    
    if (!m_sQmlFilePath.empty()) {
        if (bRet) {
            if (m_pQmlComponent == NULL) {
                m_pQmlComponent = new QQmlComponent(&getQmlEngine(), m_sQmlFilePath.c_str(), QQmlComponent::PreferSynchronous);
            }

            if (m_pQmlComponent == NULL) {
                GS_MSG("%s(): *** new QQmlComponent() is failed\n", __FUNCTION__);
                bRet = false;
            }
        }
        
        if (bRet) {
            pRootQmlContext = geRootQmlContext();
            if (pRootQmlContext == NULL) {
                GS_MSG("%s(): *** geRootQmlContext() is failed\n", __FUNCTION__);
                bRet = false;
            }
        }
        
        if (bRet) {
            if (m_pQmlContext == NULL) {
                m_pQmlContext = new QQmlContext(pRootQmlContext);
            }
            
            if (m_pQmlContext == NULL) {
                GS_MSG("%s(): *** new QQmlContext() is failed\n", __FUNCTION__);
                bRet = false;
            }
        }
        
        if (bRet) {
            if (m_pQuickItem == NULL) {
                m_pQuickItem = qobject_cast<QQuickItem *>(m_pQmlComponent->create(m_pQmlContext));
            }
            
            if (m_pQuickItem == NULL) {
                GS_MSG("%s(): *** m_pQmlComponent->create() is failed\n", __FUNCTION__);
                bRet = false;
            }
        }
    } else {
        PaintedItem *pPaintedItem = new PaintedItem();
        m_pQuickItem = pPaintedItem;
        if (m_pQuickItem != NULL) {
            pPaintedItem->setFillColor(QColor(m_sBgColor.c_str()));
            pPaintedItem->setX(0);
            pPaintedItem->setY(0);
            pPaintedItem->setWidth(SCREEN_WIDTH);
            pPaintedItem->setHeight(SCREEN_HEIGHT);
            pPaintedItem->update(QRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
        }
        
        if (m_pQuickItem == NULL) {
            GS_MSG("%s(): *** new PaintedItem() is failed\n", __FUNCTION__);
            bRet = false;
        }
    }
    
    if (bRet) {
        pMainQmlWindow = getMainQmlWindow();
        if (pMainQmlWindow == NULL) {
            GS_MSG("%s(): *** getMainQmlWindow() is failed\n", __FUNCTION__);
            bRet = false;
        } else {
            pParentQuickItem = pMainQmlWindow->contentItem();
        }
    }
    
    if (bRet) {
        m_pQuickItem->setParentItem(pParentQuickItem);
        m_pQuickItem->setZ(m_nZOrder);
    }
    
    return bRet;
}

bool TestQmlView::hide()
{
    bool bRet = true;
    
    if (bRet) {
        if (m_pQuickItem != NULL) {
            m_pQuickItem->setParentItem(NULL);
        }
    }
    
    return bRet;
}

TestQmlView *s_pTestQmlView = NULL;

} // namespace {
 
//
// showQml - <sQmlFilePath> <nZOrder> <sBgColor>
//
static BOOL GS_showQml(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sQmlFilePath;
    unsigned int nZOrder = 0;
    std::string sBgColor = "gray";

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "showQml") == 0) {
        if (vCmdLine.size() >= 3) {
            if (vCmdLine[2] != "-") {
                sQmlFilePath = vCmdLine[2];
            }
        }
        if (vCmdLine.size() >= 4) {
            if (vCmdLine[3] != "-") {
                nZOrder = TK_Tools::StrToUL(vCmdLine[3]);
            }
        }
        if (vCmdLine.size() >= 5) {
            if (vCmdLine[4] != "-") {
                sBgColor = vCmdLine[4];
            }
        }
        GS_MSG("%s(): sQmlFilePath=%s, nZOrder=%u, sBgColor=%s\n", __FUNCTION__, sQmlFilePath.c_str(), nZOrder, sBgColor.c_str());
        
        if (s_pTestQmlView != NULL) {
            delete s_pTestQmlView;
            s_pTestQmlView = NULL;
        }
        
        s_pTestQmlView = new TestQmlView(sQmlFilePath, nZOrder, sBgColor);
        if (!s_pTestQmlView->show()) {
            GS_MSG("%s(): *** s_pTestQmlView->show() is failed\n", __FUNCTION__);
        }
        
        ret = TRUE;
    }
    
    return ret;
}

//
// hideQml
//
static BOOL GS_hideQml(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "hideQml") == 0) {
        GS_MSG("%s()\n", __FUNCTION__);
        
        if (s_pTestQmlView != NULL) {
            delete s_pTestQmlView;
            s_pTestQmlView = NULL;
        }
        
        ret = TRUE;
    }
    
    return ret;
}

static std::string OutputChildrenQuickItems_MakePrefixStr(unsigned int nLevel)
{
    std::string sResult;
    unsigned int i;
    
    for (i = 0; i < nLevel; ++i) {
        sResult += TK_Tools::FormatStr("%u-", (i % 10));
    }
    
    return sResult;
}

static void OutputChildrenQuickItems(QQuickItem *pCurrentQuickItem, unsigned int nLevel, bool bRecursive, const std::string &sParentZorderStr, const std::string &sParentIndexStr)
{
    QList<QQuickItem *> childrenQuickItems;
    //std::string sPrefixSpaces(nLevel * 2, '-');
    std::string sPrefixSpaces = OutputChildrenQuickItems_MakePrefixStr(nLevel);
    
    if (pCurrentQuickItem != NULL) {
        childrenQuickItems = pCurrentQuickItem->childItems();
        int index = 0;
        for (QQuickItem *pChildQuickItem : childrenQuickItems) {
            QRectF rect = pChildQuickItem->childrenRect();
            std::string sZorder = TK_Tools::FormatStr("%s%d", sParentZorderStr.c_str(), (int)pChildQuickItem->z());
            std::string sIndex = TK_Tools::FormatStr("%s%d", sParentIndexStr.c_str(), (int)index);
            GS_MSG("%sQuickItem: addr=0x%08X, index=%s, z=%s, visible=%d, rect={x=%d,y=%d,w=%d,h=%d}, childrenRect={x=%d,y=%d,w=%d,h=%d}\n",  
                sPrefixSpaces.c_str(),
                pChildQuickItem,
                sIndex.c_str(),
                sZorder.c_str(),
                pChildQuickItem->isVisible() ? 1 : 0,
                (int)pChildQuickItem->x(), (int)pChildQuickItem->y(), (int)pChildQuickItem->width(), (int)pChildQuickItem->height(),
                (int)rect.x(), (int)rect.y(), (int)rect.width(), (int)rect.height()
            );
            usleep(1000 * 10);
            if (bRecursive) {
                OutputChildrenQuickItems(pChildQuickItem, nLevel + 1, bRecursive, sZorder + ":", sIndex + ":");
            }
            index++;
        }
    } else {
        GS_MSG("%s(): *** pCurrentQuickItem is NULL. nLevel=%u, bRecursive=%u, sParentZorderStr=%s, sParentIndexStr=%s\n", 
            __FUNCTION__, nLevel, bRecursive, sParentZorderStr.c_str(), sParentIndexStr.c_str());
    }
}

//
// listQuickItems - <bRecursive>
//
static BOOL GS_listQuickItems(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    QQuickItem *pRootQuickItem = NULL;
    bool bRecursive = false;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "listQuickItems") == 0) {
        if (vCmdLine.size() >= 3) {
            if (vCmdLine[2] != "-") {
                bRecursive = TK_Tools::StrToUL(vCmdLine[2]) != 0;
            }
        }
        GS_MSG("%s(): bRecursive=%d\n", __FUNCTION__, bRecursive ? 1 : 0);
        
        pRootQuickItem = getRootQuickItem();
        if (pRootQuickItem != NULL) {
            OutputChildrenQuickItems(pRootQuickItem, 1, bRecursive, "", "");
        } else {
            GS_MSG("%s(): *** getRootQuickItem() is failed\n", __FUNCTION__);
        }
        
        ret = TRUE;
    }
    
    return ret;
}

//
// setQuickItemVisibility - <nIndex> <bVisible>
//
static BOOL GS_setQuickItemVisibility(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    unsigned int nIndex = 0;
    bool bVisible = true;
    
    QQuickItem *pRootQuickItem = NULL;
    QList<QQuickItem *> childrenQuickItems;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "setQuickItemVisibility") == 0) {
        if (vCmdLine.size() >= 3) {
            if (vCmdLine[2] != "-") {
                nIndex = TK_Tools::StrToUL(vCmdLine[2]);
            }
        }
        if (vCmdLine.size() >= 4) {
            if (vCmdLine[3] != "-") {
                bVisible = TK_Tools::StrToUL(vCmdLine[3]) != 0;
            }
        }
        GS_MSG("%s(): nIndex=%d, bVisible=%d\n", __FUNCTION__, nIndex, bVisible ? 1 : 0);
        
        pRootQuickItem = getRootQuickItem();
        if (pRootQuickItem != NULL) {
            childrenQuickItems = pRootQuickItem->childItems();
            if (nIndex < (unsigned int)childrenQuickItems.count())
            {
                QQuickItem *pChildQuickItem = childrenQuickItems[nIndex];
                pChildQuickItem->setVisible(bVisible);
            } else {
                GS_MSG("%s(): *** nIndex is too big\n", __FUNCTION__);
            }
        } else {
            GS_MSG("%s(): *** getRootQuickItem() is failed\n", __FUNCTION__);
        }
        
        ret = TRUE;
    }
    
    return ret;
}

namespace {

class ColoredPaintedItem : public QQuickPaintedItem
{
public:
    ColoredPaintedItem(QQuickItem *pParentQuickItem, const std::string &sColor, unsigned int zOrder, bool bChildrenRect);
    ~ColoredPaintedItem();
    void paint(QPainter *painter) { TK_UNUSED_VAR(painter); }
};

ColoredPaintedItem::ColoredPaintedItem(QQuickItem *pParentQuickItem, const std::string &sColor, unsigned int zOrder, bool bChildrenRect)
    : QQuickPaintedItem(pParentQuickItem)
{
    setFillColor(QColor(sColor.c_str()));
    setZ((qreal)zOrder);
    setVisible(true);
    
    if (pParentQuickItem != NULL) {
        if (bChildrenRect) {
            QRectF childrenRect = pParentQuickItem->childrenRect();
            setX(childrenRect.x());
            setY(childrenRect.y());
            setWidth(childrenRect.width());
            setHeight(childrenRect.height());
            update(QRect(0, 0, childrenRect.width(), childrenRect.height()));
        } else {
            setX(0);
            setY(0);
            setWidth(pParentQuickItem->width());
            setHeight(pParentQuickItem->height());
            update(QRect(0, 0, pParentQuickItem->width(), pParentQuickItem->height()));
        }
    }
}

ColoredPaintedItem::~ColoredPaintedItem()
{
    setParentItem(NULL);
}

ColoredPaintedItem *g_pColoredPaintedItem = NULL;

} // namespace {

//
// addSubQuickItem - <nIndex> <sColor> <zOrder> <bChildrenRect>
//
static BOOL GS_addSubQuickItem(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    unsigned int nIndex = 0;
    std::string sColor = "#80FF00FF";
    unsigned int zOrder = 10000;
    bool bChildrenRect = false;
    
    QQuickItem *pRootQuickItem = NULL;
    QList<QQuickItem *> childrenQuickItems;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "addSubQuickItem") == 0) {
        if (vCmdLine.size() >= 3) {
            if (vCmdLine[2] != "-") {
                nIndex = TK_Tools::StrToUL(vCmdLine[2]);
            }
        }
        if (vCmdLine.size() >= 4) {
            if (vCmdLine[3] != "-") {
                sColor = vCmdLine[3];
            }
        }
        if (vCmdLine.size() >= 5) {
            if (vCmdLine[4] != "-") {
                zOrder = TK_Tools::StrToUL(vCmdLine[4]);
            }
        }
        if (vCmdLine.size() >= 6) {
            if (vCmdLine[5] != "-") {
                bChildrenRect = TK_Tools::StrToUL(vCmdLine[5]) != 0;
            }
        }
        GS_MSG("%s(): nIndex=%d, sColor=%s, zOrder=%d, bChildrenRect=%d\n", __FUNCTION__, nIndex, sColor.c_str(), zOrder, bChildrenRect ? 1 : 0);
        
        pRootQuickItem = getRootQuickItem();
        if (pRootQuickItem != NULL) {
            childrenQuickItems = pRootQuickItem->childItems();
            if (nIndex < (unsigned int)childrenQuickItems.count())
            {
                QQuickItem *pChildQuickItem = childrenQuickItems[nIndex];
                if (g_pColoredPaintedItem != NULL) {
                    delete g_pColoredPaintedItem;
                    g_pColoredPaintedItem = NULL;
                }
                g_pColoredPaintedItem = new ColoredPaintedItem(pChildQuickItem, sColor, zOrder, bChildrenRect);
            } else {
                GS_MSG("%s(): *** nIndex is too big\n", __FUNCTION__);
            }
        } else {
            GS_MSG("%s(): *** getRootQuickItem() is failed\n", __FUNCTION__);
        }
        
        ret = TRUE;
    }
    
    return ret;
}

//
// removeSubQuickItem
//
static BOOL GS_removeSubQuickItem(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "removeSubQuickItem") == 0) {
        GS_MSG("%s()\n", __FUNCTION__);
        
        if (g_pColoredPaintedItem != NULL) {
            delete g_pColoredPaintedItem;
            g_pColoredPaintedItem = NULL;
        }
        
        ret = TRUE;
    }
    
    return ret;
}

//
// dltPrint - <sContext> <nLevel> <sContent>
//
//typedef enum
//{
//        DLT_LOG_DEFAULT =    		  -1,   /**< Default log level */
//        DLT_LOG_OFF     =      		0x00,   /**< Log level off */
//        DLT_LOG_FATAL   =   		0x01, 	/**< fatal system error */
//        DLT_LOG_ERROR   =   		0x02, 	/**< error with impact to correct functionality */
//        DLT_LOG_WARN    =   		0x03, 	/**< warning, correct behaviour could not be ensured */
//        DLT_LOG_INFO    =   		0x04, 	/**< informational */
//        DLT_LOG_DEBUG   =   		0x05, 	/**< debug  */
//        DLT_LOG_VERBOSE =   		0x06 	/**< highest grade of information */
//} DltLogLevelType;
class DltLogger
{
public:
    DltLogger(const std::string &sContextId) : m_sContextId(sContextId), m_dltContext(), m_nDefaultLevel(DLT_LOG_INFO)
    {
        DLT_REGISTER_CONTEXT(m_dltContext, m_sContextId.c_str(), m_sContextId.c_str());
    }
    ~DltLogger()
    {
        DLT_UNREGISTER_CONTEXT(m_dltContext);
    }
    
    void log(int nLevel, const char *format, ...)
    {
        QString s;
        va_list args;

        if (nLevel == DLT_LOG_DEFAULT || nLevel == DLT_LOG_OFF) {
            nLevel = DLT_LOG_INFO;
        }

        va_start(args, format);
        s = QString::vasprintf(format, args);
        va_end(args);

        DLT_LOG(m_dltContext, (DltLogLevelType)nLevel, DLT_STRING(qPrintable(s.toUtf8().data())));
    }
    
private:
    const std::string m_sContextId;
    DltContext m_dltContext;
    int m_nDefaultLevel;
};
static BOOL GS_dltPrint(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    std::string sContextId;
    int nLevel = 0;
    std::string sContent;
    unsigned int i;

    if (vCmdLine.size() >= 5 && TK_Tools::CompareNoCase(vCmdLine[0], "dltPrint") == 0) {
        sContextId = vCmdLine[2];
        nLevel = TK_Tools::StrToL(vCmdLine[3]);
        sContent = vCmdLine[4];
        for (i = 5; i < vCmdLine.size(); i++) {
            sContent += " ";
            sContent += vCmdLine[i];
        }
        GS_MSG("%s(): sContextId=\"%s\", nLevel=%d, sContent=\"%s\"\n", __FUNCTION__, sContextId.c_str(), nLevel, sContent.c_str());
        
        DltLogger dltLogger(sContextId);
        dltLogger.log(nLevel, sContent.c_str());

        ret = TRUE;
    }
    
    return ret;
}

void unused_func()
{
    TK_UNUSED_VAR(GSOPT_Init);
    TK_UNUSED_VAR(GSC_Switch);
    TK_UNUSED_VAR(GSC_Switch2);
    TK_UNUSED_VAR(GSC_Msg);
    TK_UNUSED_VAR(GS_SaveMemToFile);
    TK_UNUSED_VAR(GS_KillProce);
    TK_UNUSED_VAR(GS_Execute);
    TK_UNUSED_VAR(SendUmsgToNaviHmi);
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
    { "c_exit",                               &GS_c_exit,                               "c_exit: exit process" },
    { "qt_exit",                              &GS_qt_exit,                              "qt_exit: exit process" },

    { "SYS exit",                             &GS_Sys,                                  "SYS exit: exit process" },
    { "SYS Test - <s>",                       &GS_Sys,                                  "SYS Test - [DataAbort|CppException]" },
    
    // { "SetErgAppName - <s>",                  &GS_SetErgAppName,                        "SetErgAppName - <sAppName>" },
    // { "ReadErgData - <n> <s>",                &GS_ReadErgData,                          "ReadErgData - <nLDBID> <sName>" },
    // { "WriteErgData - <n> <s> <n> ...",       &GS_WriteErgData,                         "WriteErgData - <nLDBID> <sName> <nValueByte1> ..." },

    { "TestCodeConversion - <s>",             &GS_TestCodeConversion,                   "TestCodeConversion - [utf8_to_unicode|unicode_to_utf8]" },
    { "DebugSetMagicValue - <n>",             &GS_DebugSetMagicValue,                   "DebugSetMagicValue - <nValue>" },
    
    // {{{ audio
    { "AUDIO_selectAudioSource - <n>",        &GS_AUDIO_selectAudioSource,              "AUDIO_selectAudioSource - <nSourceId>" },
    { "AUDIO_setSystemMute - <n>",            &GS_AUDIO_setSystemMute,                  "AUDIO_setSystemMute - <nMute>" },
    // audiio }}}

    // {{{ media browser
    { "MEDB_changeBrowserCategory - <n> <n>", &GS_MEDB_changeBrowserCategory,           "MEDB_changeBrowserCategory - <nPlayerId> <nCategory>" },
    { "MEDB_requestBrowserListItem - <n> <n>",&GS_MEDB_requestBrowserListItem,          "MEDB_requestBrowserListItem - <nPlayerId> <nIndex>" },
    { "MEDB_browse - <n> <n>",                &GS_MEDB_browse,                          "MEDB_browse - <nPlayerId> <nIndex>" },
    { "MEDB_top - <n>",                       &GS_MEDB_top,                             "MEDB_top - <nPlayerId>" },
    { "MEDB_up - <n>",                        &GS_MEDB_up,                              "MEDB_up - <nPlayerId>" },
    { "MEDB_setPath - <n> <s>",               &GS_MEDB_setPath,                         "MEDB_setPath - <nPlayerId> <sPath>" },
    // media browser }}}
    
    // {{{ vehicle
    { "VEHI_updateGWMDriveMode - <n>",        &GS_VEHI_updateGWMDriveMode,              "VEHI_updateGWMDriveMode - <nMode>" },
    { "VEHI_updateAcDispState - <b>",         &GS_VEHI_updateAcDispState,               "VEHI_updateAcDispState - <bState>" },
    // vehicle }}}
    
    // {{{ Navigation
    { "NAVI_prepareShowNav - <n>",            &GS_NAVI_prepareShowNav,                  "NAVI_prepareShowNav - <nDestScreen>" },
    // Navigation }}}

    { "listHkEvents - <s>",                   &GS_listHkEvents,                         "listHkEvents - <sKeyword>|all" },
    { "PostHkEvent - <s>",                    &GS_PostHkEvent,                          "PostHkEvent - <sEvent>" },
    { "triggerUserPerceivedOff",              &GS_triggerUserPerceivedOff,              "triggerUserPerceivedOff" },
    { "restartSystem",                        &GS_restartSystem,                        "restartSystem" },
    { "restartWithVisualOff",                 &GS_restartWithVisualOff,                 "restartWithVisualOff" },
    
    { "dialNumber - <s>",                     &GS_dialNumber,                           "dialNumber - <sNumber>" },
    
    { "listScreenIds - <s>",                  &GS_listScreenIds,                        "listScreenIds - <keyWokd>|all" },
    { "showScreen - <s>",                     &GS_showScreen,                           "showScreen - <sScreenId>" },
    { "switchScreen - <s>",                   &GS_switchScreen,                          "switchScreen - <sScreenId>" },
    { "showPopup - <s>",                      &GS_showPopup,                            "showPopup - <sScreenId>" },
    { "hidePopup - <s>",                      &GS_hidePopup,                            "hidePopup - <sScreenId>" },
    { "currentScreens",                       &GS_currentScreens,                       "currentScreens" },
    
    { "showQml - <s> <n> <s>",                &GS_showQml,                              "showQml - <sQmlFilePath> <nZOrder> <sBgColor>" },
    { "hideQml",                              &GS_hideQml,                              "hideQml" },
    
    { "listQuickItems - <b>",                 &GS_listQuickItems,                       "listQuickItems - <bRecursive>" },
    { "setQuickItemVisibility - <n> <b>",     &GS_setQuickItemVisibility,               "setQuickItemVisibility - <nIndex> <bVisible>" },
    { "addSubQuickItem - <n> <s> <n> <b>",    &GS_addSubQuickItem,                      "addSubQuickItem - <nIndex> <sColor> <zOrder> <bChildrenRect>" },
    { "removeSubQuickItem",                   &GS_removeSubQuickItem,                   "removeSubQuickItem" },
    
    { "dltPrint - <s> <n> <s> ...",           &GS_dltPrint,                             "dltPrint - <sContext> <nLevel> <sContent>..." },
};
bool NAVSIM_ProcessCmdLine(const char *pCmdLine)
{
    MainThreadInterceptor::sendCmdLineToMainThread(pCmdLine);
    return true;
}

bool NAVSIM_ProcessCmdLine_InMainThread(const char *pCmdLine)
{
    bool ret = false;
    
    if (pCmdLine == NULL) {
        return ret;
    }
    
    LOG_GEN();

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

////////////////////////////////////////////////////////////////////////////////////////////////////
// MainThreadInterceptor
//

MainThreadInterceptor *MainThreadInterceptor::m_pInstance = NULL;

MainThreadInterceptor *MainThreadInterceptor::getInstance()
{
    QCoreApplication *pApp = QCoreApplication::instance();
    if (pApp == NULL) {
        GS_MSG("%s(): *** QCoreApplication::instance() == NULL!\n", __FUNCTION__);
        return NULL;
    }
    
    QThread *pMainThead = pApp->thread();
    if (pMainThead == NULL) {
        GS_MSG("%s(): *** pApp->thread() == NULL!\n", __FUNCTION__);
        return NULL;
    }

    if (m_pInstance == NULL) {
        m_pInstance = new MainThreadInterceptor;
        
        if (m_pInstance != NULL) {
            m_pInstance->moveToThread(pMainThead);
            connect(m_pInstance, SIGNAL(sigProcessCmdLine(const QString &)), m_pInstance, SLOT(sltProcessCmdLine(const QString &)));
        } else {
            GS_MSG("%s(): *** failed to new MainThreadInterceptor!\n", __FUNCTION__);
        }
    }
    
    return m_pInstance;
}

void MainThreadInterceptor::sendCmdLineToMainThread(const QString &sCmdLine)
{
    MainThreadInterceptor *pThis = getInstance();
    if (pThis != NULL) {
        emit pThis->sigProcessCmdLine(sCmdLine);
    } else {
        GS_MSG("%s(): *** failed to sendCmdLineToMainThread!\n", __FUNCTION__);
    }
}

void MainThreadInterceptor::sltProcessCmdLine(const QString &sCmdLine)
{
    NAVSIM_ProcessCmdLine_InMainThread(sCmdLine.toUtf8().data());
}

StubScreen *MainThreadInterceptor::getStubScreen()
{
    StubScreen *pStubScreen = NULL;
    
    MainThreadInterceptor *pThis = getInstance();
    if (pThis != NULL) {
        pStubScreen = &pThis->m_stubScreen;
    }
    
    return pStubScreen;
}

namespace gwmGui {

ScreenTable &getScreenTable()
{
    static ScreenTable screenTable; // NOTE: To destroy this object will cause HMI process to crash.
    
    return screenTable;
}

ScreenBase *getScreen(unsigned int nScreenId)
{
    ScreenBase *pScreen = NULL;
 
    pScreen = getScreenTable().acquire((gwmGui::ScreenId::ScreenIdEnum)nScreenId);
    return pScreen;
}

int getScreenLevel(int nScreenId)
{
    const int MAX_LEVEL = 10000;
    static std::map<int, ScreenL *> assistantScreens;
    ScreenBase::SortByPriority levelGreater;
    ScreenBase *pScreenMe = NULL;
    int min = 0;
    int max = MAX_LEVEL;
    int half;
    int resultLevel;
    
    pScreenMe = getScreen(nScreenId);
    if (pScreenMe == NULL) {
        return -1;
    }
    
    while (min < max) {
        half = (min + max) / 2;
    
        ScreenL *pScreenHalfL = assistantScreens[half];
        if (pScreenHalfL == NULL) {
            pScreenHalfL = new ScreenL(half);
            assistantScreens[half] = pScreenHalfL;
        }
        
        if (levelGreater(pScreenHalfL, pScreenMe)) {
            max = half - 1;
        } else {
            min = half + 1;
        }
    }
    
    resultLevel = max;
    for (;;) {
        ScreenL *pScreenTL = assistantScreens[resultLevel];
        if (pScreenTL == NULL) {
            pScreenTL = new ScreenL(resultLevel);
            assistantScreens[resultLevel] = pScreenTL;
        }
        
        if (levelGreater(pScreenTL, pScreenMe)) {
            resultLevel--;
        } else {
            break;
        }
    }
        
    return resultLevel;
}

QQmlEngine &getQmlEngine()
{
    static QtGlue::QmlEngineInterface qmlEngineInterface;
    
    return qmlEngineInterface.engine();
}

QQmlContext *geRootQmlContext()
{
    return getQmlEngine().rootContext();
}

QQuickWindow *getMainQmlWindow()
{
    static QQuickWindow *pQuickWindow = NULL;
    
    if (pQuickWindow == NULL) {
        QWindowList pWindows = QGuiApplication::topLevelWindows();
        for (QWindow *pWindow: pWindows) {
            pQuickWindow = qobject_cast<QQuickWindow *>(pWindow);
            if (pQuickWindow != NULL) {
                if (pQuickWindow->title() == "main_gui") {
                    break;
                } else {
                    pQuickWindow = NULL;
                    continue;
                }
            }
        }
    }
    
    return pQuickWindow;
}

QQuickItem * getRootQuickItem()
{
    static QQuickItem *pQuickItem = NULL;
    
    if (pQuickItem == NULL) {
        QQuickWindow *pQuickWindow = getMainQmlWindow();
        if (pQuickWindow != NULL) {
            pQuickItem = pQuickWindow->contentItem();
        }
    }
    
    return pQuickItem;
}

} // namespace gwmGui {

