#include "TK_sock.h"
#include <windows.h>
#include <string>
#include <vector>
#include <stdafx.h>
#include <string.h>
#include <stdio.h>
#include <ssw_IEmbRegistry.h>
//#include <app_IEmbRegistryIDs.h>
#include "entNavigation.h"
#include "NavInterfaceDefinition.h"
#include "entNavigation_user.h"
#include "navLogTrace.h"
#include "navIoctl.h"
#include "entIDictaphone.h"
#include "entIAvAudioVideo.h"

////////////////////////////////////////////////////////////////////////////////////
// for debug of PushObject
//
#include "TK_Tools.h"
#include "TK_Bitmap.h"
#include "TK_DebugServer.h"
#include "NavInterfaceDefinition.h"
#include "NavUMSGQ_def.h"

#ifdef address
#undef address
#endif

#ifdef type
#undef type
#endif

#define NAV_USER_PO_POI                     0
#define NAV_USER_PO_GUIDANCE                1

#define NAVSIM_ASSERT(expr)                 TK_Tools::Assert(expr)
#define NAVSIN_PRINT_TO_PEER1               TK_Tools::DebugServer::GetInstance(TK_Tools::DSIID_NAVI).SendTextToPeer1
#define NAVSIN_PRINT_TO_PEER2               TK_Tools::DebugServer::GetInstance(TK_Tools::DSIID_NAVI).SendTextToPeer2
#define NAVSIN_MSG                          NAVSIM_Msg

static HMINavigationAccessLibUser &rHMINavigationAccessLibUser = HMINavigationAccessLibUser::getInstance();

unsigned int NAVSIMOPT_PushObjectEnable = 0;
unsigned int NAVSIMOPT_FavoriteEnable = 0;
unsigned int NAVSIMOPT_RouteEnable = 0;
unsigned int NAVSIMOPT_Route2Enable = 0;
unsigned int NAVSIMOPT_StageEnable = 0;
unsigned int NAVSIMOPT_MapEnable = 0;
unsigned int NAVSIMOPT_TpegEnable = 0;
unsigned int NAVSIMOPT_WeatherEnable = 0;
unsigned int NAVSIMOPT_ElevatedRouteEnable = 0;
unsigned int NAVSIMOPT_AlertPointEnable = 0;
unsigned int NAVSIMOPT_Telematics = 0;
unsigned int NAVSIMOPT_NaviEnableSetting = 0;
unsigned int NAVSIMOPT_ConnNavi = 0;
unsigned int NAVSIMOPT_OthersEnable = 0;

static void NAVSIMOPT_Init()
{
//	    NAVSIMOPT_PushObjectEnable = 1;
//	    NAVSIMOPT_FavoriteEnable = 1;
//	    NAVSIMOPT_RouteEnable = 1;
//	    NAVSIMOPT_Route2Enable = 1;
        NAVSIMOPT_StageEnable = 1;
//	    NAVSIMOPT_MapEnable = 1;
//      NAVSIMOPT_TpegEnable = 1;
//      NAVSIMOPT_WeatherEnable = 1;
//      NAVSIMOPT_ElevatedRouteEnable = 1;
//      NAVSIMOPT_AlertPointEnable = 1;
//      NAVSIMOPT_Telematics = 1;
//      NAVSIMOPT_NaviEnableSetting = 1;
//	    NAVSIMOPT_OthersEnable = 1;
//      NAVSIMOPT_ConnNavi = 1;
}

HMINavigationAccessLibUser::HMINavigationAccessLibUser()
{
    NAVSIMOPT_Init();
}

HMINavigationAccessLibUser::~HMINavigationAccessLibUser()
{

}

bool NAVSIN_Msg_bOn = true;
bool NAVSIM_Msg(const char *pFormat, ...)
{
    if (!NAVSIN_Msg_bOn) {
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

static struct {
    bool bThreadStopped;
    uint32_t evt;
    uint8_t* pData;
    uint32_t size;
    int timeout;
    bool (*IsCanceled)(void);
} SendUmsgToNaviHmi_data = { true };

static DWORD WINAPI SendUmsgToNaviHmi_ThreadProc(LPVOID lpParameter)
{
    if (SendUmsgToNaviHmi_data.timeout > 0) {
        Sleep(SendUmsgToNaviHmi_data.timeout);
    }

    if (SendUmsgToNaviHmi_data.IsCanceled != NULL) {
        if (SendUmsgToNaviHmi_data.IsCanceled()) {
            SendUmsgToNaviHmi_data.bThreadStopped = true;
            return 0;
        }
    }
    
    TK_Tools::hmiNavAdaptor_handleUMSG(SendUmsgToNaviHmi_data.evt, SendUmsgToNaviHmi_data.pData, SendUmsgToNaviHmi_data.size);
    SendUmsgToNaviHmi_data.bThreadStopped = true;
    
    return 0;
}

static BOOL SendUmsgToNaviHmi_bUseThread = TRUE;
static void SendUmsgToNaviHmi(uint32_t evt, const uint8_t* pData, uint32_t size, int timeout, bool (*IsCanceled)(void) = NULL)
{
    if (!SendUmsgToNaviHmi_bUseThread) {
        TK_Tools::hmiNavAdaptor_handleUMSG(evt, pData, size);
        return;
    }

    while (!SendUmsgToNaviHmi_data.bThreadStopped) {
        Sleep(50);
    }

    if (SendUmsgToNaviHmi_data.pData!= NULL) {
        delete [] SendUmsgToNaviHmi_data.pData;
    }

    memset(&SendUmsgToNaviHmi_data, 0, sizeof(SendUmsgToNaviHmi_data));
    SendUmsgToNaviHmi_data.bThreadStopped = false;
    SendUmsgToNaviHmi_data.evt = evt;
    if (pData != NULL && size > 0) {
        SendUmsgToNaviHmi_data.pData = new uint8_t[size];
        NAVSIM_ASSERT(SendUmsgToNaviHmi_data.pData != NULL);
        if (SendUmsgToNaviHmi_data.pData != NULL) {
            memcpy(SendUmsgToNaviHmi_data.pData, pData, size);
            SendUmsgToNaviHmi_data.size = size;
        }
    }
    SendUmsgToNaviHmi_data.timeout = timeout;
    SendUmsgToNaviHmi_data.IsCanceled = IsCanceled;

    HANDLE hThread = CreateThread(NULL, 0, &SendUmsgToNaviHmi_ThreadProc, NULL, 0, NULL);
    NAVSIM_ASSERT(hThread != NULL);
    CloseHandle(hThread);
}

static int TriggerPushObject_type = 0;
static void TriggerPushObject(int type) {
    if (!NAVSIMOPT_PushObjectEnable) {
        return;
    }

    TriggerPushObject_type = type;
    
    uint8_t data[1];
    data[0] = NavSend_SendPushObjectShortName;

    SendUmsgToNaviHmi(entNaviStatusUpdate, data, sizeof(data), 0);
}

//
// SetNetworkSearchResultType - <nResultType>  # 0: successful; 1: successful without result; 2: failed;
//
enum NS_SetNetworkSearchResultType_t {
    NS_NSRT__OK = 0,
    NS_NSRT__OK_WITHOUT_RESULT,
    NS_NSRT__DELAY,
    NS_NSRT__FAILED,
};
UINT_T NS_SetNetworkSearchResultType__nResultType = 0;
static BOOL NS_SetNetworkSearchResultType(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SetNetworkSearchResultType") == 0) {
        if (vCmdLine.size() >= 3) {
            NS_SetNetworkSearchResultType__nResultType = (UINT_T)TK_Tools::StrToUL(vCmdLine[2]);
            NAVSIN_MSG("%s(): NS_SetNetworkSearchResultType__nResultType=%u\n", __FUNCTION__, NS_SetNetworkSearchResultType__nResultType);
            ret = TRUE;
        }
    }
    
    return ret;
}

class NAVSIM_NetSearchStateMsgData
{
public:
    NAVSIM_NetSearchStateMsgData() {}
    void DoPacking() {
        unsigned int nSize = 0;
        m_packedData.resize(sizeof(m_msg) + sizeof(m_stNetSearchState));
        UINT8_T *pData = &m_packedData[0];
        
        memcpy(pData + nSize, &m_msg, sizeof(m_msg));
        nSize += sizeof(m_msg);

        memcpy(pData + nSize, &m_stNetSearchState, sizeof(m_stNetSearchState));
        nSize += sizeof(m_stNetSearchState);
    }

    const UINT8_T *PackedData() const { return &m_packedData[0]; }
    unsigned int PackedDataSize() const { return m_packedData.size(); }

public:
    UINT8_T m_msg;
    struct_NetSearchState m_stNetSearchState;
private:
    std::vector<UINT8_T> m_packedData;
};

static enum_NetSearchType command_StartNetworkSearch__type = NST_POI;
BOOL HMINavigationAccessLibUser::command_StartNetworkSearch(enum_NetSearchType type) const
{
    if (!NAVSIMOPT_ConnNavi) {
        return RET_NOT_PROCESSED;
    }

    command_StartNetworkSearch__type = type;

    NAVSIM_NetSearchStateMsgData data;
    
    data.m_msg = NavSend_SendNetworkSearchStatus;
    data.m_stNetSearchState.searchType = command_StartNetworkSearch__type;
    data.m_stNetSearchState.searchStatus = NSS_STARTED;
    data.DoPacking();
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)data.PackedData(), data.PackedDataSize(), 10);
    NAVSIN_MSG("%s(): nsType=%u, status=%u\n", __FUNCTION__, 
        data.m_stNetSearchState.searchType, data.m_stNetSearchState.searchStatus);

    data.m_msg = NavSend_SendNetworkSearchStatus;
    data.m_stNetSearchState.searchType = command_StartNetworkSearch__type;
    data.m_stNetSearchState.searchStatus = NSS_FINISHED;
    data.DoPacking();
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)data.PackedData(), data.PackedDataSize(), 1000);
    NAVSIN_MSG("%s(): nsType=%u, status=%u\n", __FUNCTION__, 
        data.m_stNetSearchState.searchType, data.m_stNetSearchState.searchStatus);

    return TRUE;
}

//
// NetworkSearchDelayedDataIsAvailable
//
static BOOL NS_NetworkSearchDelayedDataIsAvailable(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "NetworkSearchDelayedDataIsAvailable") == 0) {
        NAVSIM_NetSearchStateMsgData data;
    
        data.m_msg = NavSend_SendNetworkSearchStatus;
        data.m_stNetSearchState.searchType = command_StartNetworkSearch__type;
        data.m_stNetSearchState.searchStatus = NSS_STARTED;
        data.DoPacking();
        SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)data.PackedData(), data.PackedDataSize(), 10);
        NAVSIN_MSG("%s(): nsType=%u, status=%u\n", __FUNCTION__, 
            data.m_stNetSearchState.searchType, data.m_stNetSearchState.searchStatus);

        data.m_msg = NavSend_SendNetworkSearchStatus;
        data.m_stNetSearchState.searchType = command_StartNetworkSearch__type;
        data.m_stNetSearchState.searchStatus = NSS_FINISHED;
        data.DoPacking();
        SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)data.PackedData(), data.PackedDataSize(), 10);
        NAVSIN_MSG("%s(): nsType=%u, status=%u\n", __FUNCTION__, 
        data.m_stNetSearchState.searchType, data.m_stNetSearchState.searchStatus);

        NS_SetNetworkSearchResultType__nResultType = NS_NSRT__OK;
        ret = TRUE;
    }
    
    return ret;
}

BOOL HMINavigationAccessLibUser::command_AbortNetworkSearch(enum_NetSearchType type) const
{
    if (!NAVSIMOPT_ConnNavi) {
        return RET_NOT_PROCESSED;
    }
    
    NAVSIM_NetSearchStateMsgData data;
    
    data.m_msg = NavSend_SendNetworkSearchStatus;
    data.m_stNetSearchState.searchType = type;
    data.m_stNetSearchState.searchStatus = NSS_ABORTED;
    data.DoPacking();
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)data.PackedData(), data.PackedDataSize(), 10);
    NAVSIN_MSG("%s(): nsType=%u, status=%u\n", __FUNCTION__, 
        data.m_stNetSearchState.searchType, data.m_stNetSearchState.searchStatus);

    return TRUE;
}

//	static short command_GetNetPOISearchElement__index = 0;
//	static short command_GetNetPOISearchElement__noOfElements = 0;
//	BOOL HMINavigationAccessLibUser::command_GetNetPOISearchElement(short index, short noOfElements) const
//	{
//	    if (!NAVSIMOPT_ConnNavi) {
//	        return RET_NOT_PROCESSED;
//	    }
//	
//	    command_GetNetPOISearchElement__index = index;
//	    command_GetNetPOISearchElement__noOfElements = noOfElements;
//	
//	    UINT8_T data[2];
//	
//	    data[0] = (const uint8_t)NavSend_NavDataAcknowledgement;
//	    data[1] = (const uint8_t)_ND_CMD_NET_POI_LIST;
//	    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 500);
//	    NAVSIN_MSG("%s(): index=%u, noOfElements=%u\n", __FUNCTION__, 
//	        index, noOfElements);
//	    
//	    return TRUE;
//	}

#if 0
typedef struct tstruct_NetPoiSearchList
{
    enum_NetSearchType poiType;
    struct_PoiSearchList poiSearchList;
}struct_NetPoiSearchList;
typedef struct tstruct_PoiSearchList
{
    unsigned short sizeOfList;
    unsigned short noOfElementsRetrieved;
    struct_PoiSearchInfo elements[MAX_LIST_SIZE];
    unsigned short index;
}struct_PoiSearchList;
typedef struct tstruct_PoiSearchInfo
{
    unsigned short categoryID;
    PoiName name;
    int distance;
}struct_PoiSearchInfo;
#endif
UINT8_T command_GetNetPOISearchElementData__nFunctionCode = (UINT8_T)NAV_FUN_POI_LIST /*NAV_FUN_PARKING_LIST*/ /*NAV_FUN_GAS_LIST*/;
UINT8_T command_GetNetPOISearchElementData__nErrorCode = (UINT8_T)NAV_REQ_TIMEOUT;
#if 1
BOOL HMINavigationAccessLibUser::command_GetNetPOISearchElementData(struct_NetPoiSearchList* poiSearchElements) const
{
    if (!NAVSIMOPT_ConnNavi) {
        return (BOOL)RET_NOT_PROCESSED;
    }

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__DELAY) {
        memset(poiSearchElements, 0, sizeof(struct_NetPoiSearchList));
        poiSearchElements->retVal = NAV_RET_NOT_READY;
        return TRUE;
    }

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__OK_WITHOUT_RESULT) {
        memset(poiSearchElements, 0, sizeof(struct_NetPoiSearchList));
        poiSearchElements->retVal = NAV_RET_OK;
        return TRUE;
    }

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__FAILED) {
        uint8_t data[2];
        memset(&data, 0, sizeof(data));
        data[0] = command_GetNetPOISearchElementData__nFunctionCode;
        data[1] = command_GetNetPOISearchElementData__nErrorCode;
        SendUmsgToNaviHmi(entNavSend_SendErrorData, (const uint8_t *)&data, sizeof(data), 2000);
    
        poiSearchElements->retVal = NAV_RET_FAILED;
        return TRUE;
    }

#define NET_POI_LIST_COUNT                  43

    static struct_PoiSearchInfo stPoiSearchInfo[NET_POI_LIST_COUNT];
    static unsigned int nListItemCount = NET_POI_LIST_COUNT;
    int i;
    unsigned short noOfElementsRetrieved = MAX_LIST_SIZE;
    unsigned short index;

    if (poiSearchElements == NULL) {
        return (NAV_API_RET_VAL)NAV_RET_FAILED;
    }

    index = poiSearchElements->poiSearchList.index;

    {
        for (i = 0; i < nListItemCount; i++) {
            stPoiSearchInfo[i].categoryID = 0; // ???
            switch (command_StartNetworkSearch__type) {
            case NST_POI:
                _snwprintf(stPoiSearchInfo[i].name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"Net Normal Poi %03u", i);
                break;
            case NST_PARKING:
                _snwprintf(stPoiSearchInfo[i].name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"Net Parking Poi %03u", i);
                break;
            case NST_GAS:
                _snwprintf(stPoiSearchInfo[i].name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"Net Gas Poi %03u", i);
                break;
            }
            stPoiSearchInfo[i].distance = Random() % 10000000; // ???
        }
    }

    memset(poiSearchElements, 0, sizeof(struct_NetPoiSearchList));
    poiSearchElements->poiType = command_StartNetworkSearch__type;
    struct_PoiSearchList &poiSearchList = poiSearchElements->poiSearchList;
    poiSearchList.sizeOfList = nListItemCount;
    noOfElementsRetrieved = min(noOfElementsRetrieved, (nListItemCount - index));
    noOfElementsRetrieved = max(noOfElementsRetrieved, 0);
    poiSearchList.noOfElementsRetrieved = noOfElementsRetrieved;
    memcpy(poiSearchList.elements, &stPoiSearchInfo[index], sizeof(struct_PoiSearchInfo) * noOfElementsRetrieved);
    poiSearchList.index = index;

    poiSearchElements->retVal = NAV_RET_OK;
    return TRUE;
}
#else
//	NAV_API_RET_VAL HMINavigationAccessLibUser::command_GetNetPOISearchElementData(struct_NetPoiSearchList* poiSearchElements) const
//	{
//	    if (!NAVSIMOPT_ConnNavi) {
//	        return (NAV_API_RET_VAL)RET_NOT_PROCESSED;
//	    }
//	
//	    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__DELAY) {
//	        memset(poiSearchElements, 0, sizeof(struct_NetPoiSearchList));
//	        return (NAV_API_RET_VAL)NAV_RET_NOT_READY;
//	    }
//	
//	    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__OK_WITHOUT_RESULT) {
//	        memset(poiSearchElements, 0, sizeof(struct_NetPoiSearchList));
//	        return (NAV_API_RET_VAL)NAV_RET_OK;
//	    }
//	
//	    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__FAILED) {
//	        uint8_t data[2];
//	        memset(&data, 0, sizeof(data));
//	        data[0] = command_GetNetPOISearchElementData__nFunctionCode;
//	        data[1] = command_GetNetPOISearchElementData__nErrorCode;
//	        SendUmsgToNaviHmi(entNavSend_SendErrorData, (const uint8_t *)&data, sizeof(data), 2000);
//	    
//	        return (NAV_API_RET_VAL)NAV_RET_FAILED;
//	    }
//	
//	#define NET_POI_LIST_COUNT                  43
//	
//	    static struct_PoiSearchInfo stPoiSearchInfo[NET_POI_LIST_COUNT];
//	    static unsigned int nListItemCount = NET_POI_LIST_COUNT;
//	    int i;
//	    unsigned short noOfElementsRetrieved = MAX_LIST_SIZE;
//	    unsigned short index;
//	
//	    if (poiSearchElements == NULL) {
//	        return (NAV_API_RET_VAL)NAV_RET_FAILED;
//	    }
//	
//	    index = poiSearchElements->poiSearchList.index;
//	
//	    {
//	        for (i = 0; i < nListItemCount; i++) {
//	            stPoiSearchInfo[i].categoryID = 0; // ???
//	            switch (command_StartNetworkSearch__type) {
//	            case NST_POI:
//	                _snwprintf(stPoiSearchInfo[i].name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"Net Normal Poi %03u", i);
//	                break;
//	            case NST_PARKING:
//	                _snwprintf(stPoiSearchInfo[i].name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"Net Parking Poi %03u", i);
//	                break;
//	            case NST_GAS:
//	                _snwprintf(stPoiSearchInfo[i].name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"Net Gas Poi %03u", i);
//	                break;
//	            }
//	            stPoiSearchInfo[i].distance = Random() % 10000000; // ???
//	        }
//	    }
//	
//	    memset(poiSearchElements, 0, sizeof(struct_NetPoiSearchList));
//	    poiSearchElements->poiType = command_StartNetworkSearch__type;
//	    struct_PoiSearchList &poiSearchList = poiSearchElements->poiSearchList;
//	    poiSearchList.sizeOfList = nListItemCount;
//	    noOfElementsRetrieved = min(noOfElementsRetrieved, (nListItemCount - index));
//	    noOfElementsRetrieved = max(noOfElementsRetrieved, 0);
//	    poiSearchList.noOfElementsRetrieved = noOfElementsRetrieved;
//	    memcpy(poiSearchList.elements, &stPoiSearchInfo[index], sizeof(struct_PoiSearchInfo) * noOfElementsRetrieved);
//	    poiSearchList.index = index;
//	
//	    return (NAV_API_RET_VAL)NAV_RET_OK;
//	}
#endif

static short command_GetNetPOISearchElementDetails__index = 0;
static UINT8_T command_GetNetPOISearchElementDetails__nFunctionCode = (UINT8_T)NAV_FUN_POI_DETAIL;
static UINT8_T command_GetNetPOISearchElementDetails__nErrorCode = (UINT8_T)NAV_REQ_TIMEOUT;
BOOL HMINavigationAccessLibUser::command_GetNetPOISearchElementDetails(short index, enum_NetSearchType poiType) const
{
    if (!NAVSIMOPT_ConnNavi) {
        return RET_NOT_PROCESSED;
    }

    command_GetNetPOISearchElementDetails__index = index;

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__OK_WITHOUT_RESULT
        || NS_SetNetworkSearchResultType__nResultType == NS_NSRT__FAILED) {
        uint8_t data[2];
        memset(&data, 0, sizeof(data));
        switch (poiType) {
        default:
        case NST_POI:
            command_GetNetPOISearchElementDetails__nFunctionCode = (const uint8_t)NAV_FUN_POI_DETAIL;
            break;
        case NST_PARKING:
            command_GetNetPOISearchElementDetails__nFunctionCode = (const uint8_t)NAV_FUN_PARKING_DETAIL;
            break;
        case NST_GAS:
            command_GetNetPOISearchElementDetails__nFunctionCode = (const uint8_t)NAV_FUN_GAS_DETAIL;
            break;
        }
        data[0] = command_GetNetPOISearchElementDetails__nFunctionCode;
        data[1] = command_GetNetPOISearchElementDetails__nErrorCode;
        SendUmsgToNaviHmi(entNavSend_SendErrorData, (const uint8_t *)&data, sizeof(data), 500);
    
        return FALSE;
    }

    UINT8_T data[2];

    data[0] = (const uint8_t)NavSend_NavDataAcknowledgement;
    switch (poiType) {
    case NST_POI:
        data[1] = (const uint8_t)_ND_CMD_NET_POI_LIST_ELEMENT_DETAIL_DATA;
        break;
    case NST_PARKING:
        data[1] = (const uint8_t)_ND_CMD_NET_PARKING_LIST_ELEMENT_DETAIL_DATA;
        break;
    case NST_GAS:
        data[1] = (const uint8_t)_ND_CMD_NET_GAS_LIST_ELEMENT_DETAIL_DATA;
        break;
    }
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 300);
    NAVSIN_MSG("%s(): index=%u, poiType=%u\n", __FUNCTION__, 
        index, poiType);
    
    return TRUE;
}

#if 0
typedef struct tstruct_NetPoiInfo
{
  enum_NetSearchType type;
  struct_PoiInfo poiInfo;
}struct_NetPoiInfo;
typedef struct tstruct_PoiInfo
{
    unsigned short categoryID;
    struct_LocationInfo location;
    short index;
}struct_PoiInfo;
typedef struct tstruct_LocationInfo
{
  struct_GeoLocation geoLocation;
  TCHAR name[MAX_SHORT_LOCATION_NAME_STRING_LENGTH];
  TCHAR city[MAX_CITY_STRING_LENGTH];
  TCHAR province[MAX_PROVINCE_STRING_LENGTH];
  TCHAR addr[MAX_ADDR_STRING_LENGTH];
  TCHAR tel[MAX_TEL_STRING_LENGTH];
  TCHAR town[MAX_TOWN_STRING_LENGTH];
}struct_LocationInfo;
#endif
static UINT8_T command_GetNetPOISearchElementDetailsData__nFunctionCode = (UINT8_T)NAV_FUN_POI_DETAIL;
static UINT8_T command_GetNetPOISearchElementDetailsData__nErrorCode = (UINT8_T)NAV_REQ_TIMEOUT;
static std::wstring command_GetNetPOISearchElementDetailsData_sPhoneNo = L"021-60804724";
BOOL HMINavigationAccessLibUser::command_GetNetPOISearchElementDetailsData(struct_NetPoiInfo* element) const
{
    if (!NAVSIMOPT_ConnNavi) {
        return RET_NOT_PROCESSED;
    }

    if (element == NULL) {
        return TRUE;
    }

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__OK_WITHOUT_RESULT) {
        memset(element, 0, sizeof(struct_NetPoiInfo));
        return TRUE;
    }

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__FAILED) {
        uint8_t data[2];
        memset(&data, 0, sizeof(data));
        data[0] = command_GetNetPOISearchElementDetailsData__nFunctionCode;
        data[1] = command_GetNetPOISearchElementDetailsData__nErrorCode;
        SendUmsgToNaviHmi(entNavSend_SendErrorData, (const uint8_t *)&data, sizeof(data), 500);
    
        return FALSE;
    }

    NAVSIN_MSG("%s():\n", __FUNCTION__);

    short index = command_GetNetPOISearchElementDetails__index;

    struct_NetPoiInfo &stNetPoiInfo = *element;
    memset(&stNetPoiInfo, 0, sizeof(struct_NetPoiInfo));
    
    // fill stNetPoiInfo
    {
        stNetPoiInfo.type = command_StartNetworkSearch__type;

        struct_PoiInfo &stPoiInfo = stNetPoiInfo.poiInfo;
        // fill stPoiInfo
        {   
            stPoiInfo.categoryID = 0; // ????
            struct_LocationInfo &stLocationInfo = stPoiInfo.location;
            // fill stLocationInfo
            {
                stLocationInfo.geoLocation.latitude = 31223501;
                stLocationInfo.geoLocation.longitude = 121445299;
                _snwprintf(stLocationInfo.name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"(name %03u)", index);
                _snwprintf(stLocationInfo.city, MAX_CITY_STRING_LENGTH, L"(city %03u)", index);
                _snwprintf(stLocationInfo.province, MAX_PROVINCE_STRING_LENGTH, L"(province %03u)", index);
                _snwprintf(stLocationInfo.addr, MAX_ADDR_STRING_LENGTH, L"(addr %03u)", index);
                _snwprintf(stLocationInfo.tel, MAX_TEL_STRING_LENGTH, command_GetNetPOISearchElementDetailsData_sPhoneNo.c_str(), index);
                _snwprintf(stLocationInfo.town, MAX_TOWN_STRING_LENGTH, L"(town %03u)", index);
            }
            stPoiInfo.index = index;
        }
    }
    
    return TRUE;
}

#if 0
typedef struct tstruct_NetParkingInfo
{
    struct_NetPoiInfo netPOIInfo;
    enum_ParkingStatus parkingStatus;
    unsigned short parkingStar;
    struct_ParkingData parkingData;
}struct_NetParkingInfo;
typedef struct tstruct_ParkingData
{
    enum_ParkingChargeType parkingChargeType;
    TCHAR chargeInfo1[PARKING_CHARGE_STRING_MAX_LENGTH];
    TCHAR chargeInfo2[PARKING_CHARGE_STRING_MAX_LENGTH];
    TCHAR chargeInfo3[PARKING_CHARGE_STRING_MAX_LENGTH];
    TCHAR chargeInfo4[PARKING_CHARGE_STRING_MAX_LENGTH];
}struct_ParkingData;
#endif
UINT8_T command_GetNetParkingSearchElementDetailsData__nFunctionCode = (UINT8_T)NAV_FUN_PARKING_DETAIL;
UINT8_T command_GetNetParkingSearchElementDetailsData__nErrorCode = (UINT8_T)NAV_REQ_TIMEOUT;
BOOL HMINavigationAccessLibUser::command_GetNetParkingSearchElementDetailsData(struct_NetParkingInfo* element) const
{
    if (!NAVSIMOPT_ConnNavi) {
        return RET_NOT_PROCESSED;
    }

    if (element == NULL) {
        return TRUE;
    }

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__OK_WITHOUT_RESULT) {
        memset(element, 0, sizeof(struct_NetParkingInfo));
        return TRUE;
    }

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__FAILED) {
        uint8_t data[2];
        memset(&data, 0, sizeof(data));
        data[0] = command_GetNetParkingSearchElementDetailsData__nFunctionCode;
        data[1] = command_GetNetParkingSearchElementDetailsData__nErrorCode;
        SendUmsgToNaviHmi(entNavSend_SendErrorData, (const uint8_t *)&data, sizeof(data), 500);
    
        return FALSE;
    }

    NAVSIN_MSG("%s():\n", __FUNCTION__);

#if 0
//	    short index = command_GetNetPOISearchElementDetails__index;
//	
//	    struct_NetParkingInfo &stNetParkingInfo = *element;
//	    memset(&stNetParkingInfo, 0, sizeof(struct_NetParkingInfo));
//	
//	    struct_NetPoiInfo &stNetPoiInfo = stNetParkingInfo.netPOIInfo;
//	    // fill stNetPoiInfo
//	    {
//	        stNetPoiInfo.type = command_StartNetworkSearch__type;
//	
//	        struct_PoiInfo &stPoiInfo = stNetPoiInfo.poiInfo;
//	        // fill stPoiInfo
//	        {
//	            stPoiInfo.categoryID = 0; // ????
//	            struct_LocationInfo &stLocationInfo = stPoiInfo.location;
//	            // fill stLocationInfo
//	            {
//	                stLocationInfo.geoLocation.latitude = 31223501;
//	                stLocationInfo.geoLocation.longitude = 121445299;
//	                _snwprintf(stLocationInfo.name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"(name %03u)", index);
//	                _snwprintf(stLocationInfo.city, MAX_CITY_STRING_LENGTH, L"(city %03u)", index);
//	                _snwprintf(stLocationInfo.province, MAX_PROVINCE_STRING_LENGTH, L"(province %03u)", index);
//	                _snwprintf(stLocationInfo.addr, MAX_ADDR_STRING_LENGTH, L"(addr %03u)", index);
//	                _snwprintf(stLocationInfo.tel, MAX_TEL_STRING_LENGTH, L"02160804724", index);
//	                _snwprintf(stLocationInfo.town, MAX_TOWN_STRING_LENGTH, L"(town %03u)", index);
//	            }
//	            stPoiInfo.index = index;
//	        }
//	    }
//	
//	    stNetParkingInfo.parkingStatus = (enum_ParkingStatus)(index % 3);
//	    stNetParkingInfo.parkingStar = index % 6;
//	
//	    struct_ParkingData &stParkingData = stNetParkingInfo.parkingData;
//	    // fill stParkingData
//	    {
//	        stParkingData.parkingChargeType = (enum_ParkingChargeType)(index % 5);
//	        _snwprintf(stParkingData.chargeInfo1, PARKING_CHARGE_STRING_MAX_LENGTH, L"(charge info %03u)", 1);
//	        _snwprintf(stParkingData.chargeInfo2, PARKING_CHARGE_STRING_MAX_LENGTH, L"(charge info %03u)", 2);
//	        _snwprintf(stParkingData.chargeInfo3, PARKING_CHARGE_STRING_MAX_LENGTH, L"(charge info %03u)", 3);
//	        _snwprintf(stParkingData.chargeInfo4, PARKING_CHARGE_STRING_MAX_LENGTH, L"(charge info %03u)", 4);
//	    }
#else
    short index = command_GetNetPOISearchElementDetails__index;
	
    struct_NetParkingInfo &stNetParkingInfo = *element;
    memset(&stNetParkingInfo, 0, sizeof(struct_NetParkingInfo));
	
    struct_NetPoiInfo &stNetPoiInfo = stNetParkingInfo.netPOIInfo;
    // fill stNetPoiInfo
    {
        stNetPoiInfo.type = command_StartNetworkSearch__type;
	
        struct_PoiInfo &stPoiInfo = stNetPoiInfo.poiInfo;
        // fill stPoiInfo
        {
            stPoiInfo.categoryID = 0; // ????
            struct_LocationInfo &stLocationInfo = stPoiInfo.location;
            // fill stLocationInfo
            {
                stLocationInfo.geoLocation.latitude = 31223501;
                stLocationInfo.geoLocation.longitude = 121445299;
                _snwprintf(stLocationInfo.name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"(name %03u)", index);
                _snwprintf(stLocationInfo.city, MAX_CITY_STRING_LENGTH, L"(city %03u)", index);
                _snwprintf(stLocationInfo.province, MAX_PROVINCE_STRING_LENGTH, L"(province %03u)", index);
                _snwprintf(stLocationInfo.addr, MAX_ADDR_STRING_LENGTH, L"(addr %03u)", index);
                _snwprintf(stLocationInfo.tel, MAX_TEL_STRING_LENGTH, L"02160804724", index);
                _snwprintf(stLocationInfo.town, MAX_TOWN_STRING_LENGTH, L"(town %03u)", index);
            }
            stPoiInfo.index = index;
        }

        stNetPoiInfo.rate = index % 6;
        _snwprintf(stNetPoiInfo.poiDescription, MAX_NET_POI_DESCRIPTION_STRING_LENGTH, 
            L"(parking station info %03u)\n"
            L"(parking station info %03u)\n"
            L"(parking station info %03u)\n"
            L"(parking station info %03u)\n",
            1, 2, 3, 4
        );
    }
	
    stNetParkingInfo.parkingStatus = (enum_ParkingStatus)(index % 3);
    stNetParkingInfo.avePrice = (index % 6) + 100;
#endif // #if 0
    
    return TRUE;
}

#if 0
typedef struct tstruct_NetGasStationInfo
{
    struct_NetPoiInfo netPOIInfo;
    unsigned short gasStar;
    unsigned short numOfGasData;
    struct_GasData gasData[GAS_DATA_LIST_SIZE];
}struct_NetGasStationInfo;
typedef struct tstruct_GasData
{
    TCHAR gasName[GAS_TYPE_MAX_LENGTH];
    unsigned short gasPrice;
}struct_GasData;
#endif
UINT8_T command_GetNetGasStationSearchElementDetailsData__nFunctionCode = (UINT8_T)NAV_FUN_GAS_DETAIL;
UINT8_T command_GetNetGasStationSearchElementDetailsData__nErrorCode = (UINT8_T)NAV_REQ_TIMEOUT;
BOOL HMINavigationAccessLibUser::command_GetNetGasStationSearchElementDetailsData(struct_NetGasStationInfo* element) const
{
    if (!NAVSIMOPT_ConnNavi) {
        return RET_NOT_PROCESSED;
    }

    if (element == NULL) {
        return TRUE;
    }

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__OK_WITHOUT_RESULT) {
        memset(element, 0, sizeof(struct_NetGasStationInfo));
        return TRUE;
    }

    if (NS_SetNetworkSearchResultType__nResultType == NS_NSRT__FAILED) {
        uint8_t data[2];
        memset(&data, 0, sizeof(data));
        data[0] = command_GetNetGasStationSearchElementDetailsData__nFunctionCode;
        data[1] = command_GetNetGasStationSearchElementDetailsData__nErrorCode;
        SendUmsgToNaviHmi(entNavSend_SendErrorData, (const uint8_t *)&data, sizeof(data), 500);
    
        return FALSE;
    }

    NAVSIN_MSG("%s():\n", __FUNCTION__);

#if 0
//	    short index = command_GetNetPOISearchElementDetails__index;
//	    unsigned short i;
//	
//	    struct_NetGasStationInfo &stNetGasStationInfo = *element;
//	    memset(&stNetGasStationInfo, 0, sizeof(struct_NetGasStationInfo));
//	
//	    struct_NetPoiInfo &stNetPoiInfo = stNetGasStationInfo.netPOIInfo;
//	    // fill stNetPoiInfo
//	    {
//	        stNetPoiInfo.type = command_StartNetworkSearch__type;
//	
//	        struct_PoiInfo &stPoiInfo = stNetPoiInfo.poiInfo;
//	        // fill stPoiInfo
//	        {
//	            stPoiInfo.categoryID = 0; // ????
//	            struct_LocationInfo &stLocationInfo = stPoiInfo.location;
//	            // fill stLocationInfo
//	            {
//	                stLocationInfo.geoLocation.latitude = 31223501;
//	                stLocationInfo.geoLocation.longitude = 121445299;
//	                _snwprintf(stLocationInfo.name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"(name %03u)", index);
//	                _snwprintf(stLocationInfo.city, MAX_CITY_STRING_LENGTH, L"(city %03u)", index);
//	                _snwprintf(stLocationInfo.province, MAX_PROVINCE_STRING_LENGTH, L"(province %03u)", index);
//	                _snwprintf(stLocationInfo.addr, MAX_ADDR_STRING_LENGTH, L"(addr %03u)", index);
//	                _snwprintf(stLocationInfo.tel, MAX_TEL_STRING_LENGTH, L"02160804724", index);
//	                _snwprintf(stLocationInfo.town, MAX_TOWN_STRING_LENGTH, L"(town %03u)", index);
//	            }
//	            stPoiInfo.index = index;
//	        }
//	    }
//	
//	    stNetGasStationInfo.gasStar = index % 6;
//	    stNetGasStationInfo.numOfGasData = index % (GAS_DATA_LIST_SIZE + 1);
//	    for (i = 0; i < stNetGasStationInfo.numOfGasData; i++) {
//	        struct_GasData &stGasData = stNetGasStationInfo.gasData[i];
//	        // fill stGasData
//	        {
//	            _snwprintf(stGasData.gasName, GAS_TYPE_MAX_LENGTH, L"(gas name %03u)", i);
//	            stGasData.gasPrice = 30 + (index % 30);
//	        }
//	    }
#else
    short index = command_GetNetPOISearchElementDetails__index;
	
    struct_NetGasStationInfo &stNetGasStationInfo = *element;
    memset(&stNetGasStationInfo, 0, sizeof(struct_NetGasStationInfo));
	
    struct_NetPoiInfo &stNetPoiInfo = stNetGasStationInfo.netPOIInfo;
    // fill stNetPoiInfo
    {
        stNetPoiInfo.type = command_StartNetworkSearch__type;
	
        struct_PoiInfo &stPoiInfo = stNetPoiInfo.poiInfo;
        // fill stPoiInfo
        {
            stPoiInfo.categoryID = 0; // ????
            struct_LocationInfo &stLocationInfo = stPoiInfo.location;
            // fill stLocationInfo
            {
                stLocationInfo.geoLocation.latitude = 31223501;
                stLocationInfo.geoLocation.longitude = 121445299;
                _snwprintf(stLocationInfo.name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, L"(name %03u)", index);
                _snwprintf(stLocationInfo.city, MAX_CITY_STRING_LENGTH, L"(city %03u)", index);
                _snwprintf(stLocationInfo.province, MAX_PROVINCE_STRING_LENGTH, L"(province %03u)", index);
                _snwprintf(stLocationInfo.addr, MAX_ADDR_STRING_LENGTH, L"(addr %03u)", index);
                _snwprintf(stLocationInfo.tel, MAX_TEL_STRING_LENGTH, L"02160804724", index);
                _snwprintf(stLocationInfo.town, MAX_TOWN_STRING_LENGTH, L"(town %03u)", index);
            }
            stPoiInfo.index = index;
        }

        stNetPoiInfo.rate = index % 6;
        _snwprintf(stNetPoiInfo.poiDescription, MAX_NET_POI_DESCRIPTION_STRING_LENGTH, 
            L"(gas station info %03u)\n"
            L"(gas station info %03u)\n"
            L"(gas station info %03u)\n"
            L"(gas station info %03u)\n",
            1, 2, 3, 4
        );
    }
#endif // #if 0
    
    return TRUE;
}

static BOOL command_SetGlobalSearchType__bDefNetworkSearch = TRUE;
BOOL HMINavigationAccessLibUser::command_SetDefaultPoiSearchSetting(BOOL bDefNetworkSearch) const
{
    if (!NAVSIMOPT_ConnNavi) {
        return RET_NOT_PROCESSED;
    }

    command_SetGlobalSearchType__bDefNetworkSearch = bDefNetworkSearch;
    NAVSIN_MSG("%s(): bDefNetworkSearch=%u\n", __FUNCTION__, 
        bDefNetworkSearch);

    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_GetDefaultPoiSearchSetting(BOOL *bDefNetworkSearch) const
{
    if (!NAVSIMOPT_ConnNavi) {
        return RET_NOT_PROCESSED;
    }

    if (bDefNetworkSearch == NULL) {
        return TRUE;
    }

    *bDefNetworkSearch = command_SetGlobalSearchType__bDefNetworkSearch;
    NAVSIN_MSG("%s(): bDefNetworkSearch=%u\n", __FUNCTION__, 
        *bDefNetworkSearch);

    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_GetPushObjectShortName(struct_PushObject * po) const
{
    if (!NAVSIMOPT_PushObjectEnable) {
        return RET_NOT_PROCESSED;
    }
    
    if (po == NULL) {
        return FALSE;
    }
    
    switch (TriggerPushObject_type) {
    case NAV_USER_PO_GUIDANCE:
        _sntprintf(po->name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, 
            _T("Push Guidance name (1)"));
        po->type = PO_GUIDANCE;
        break;
    case NAV_USER_PO_POI:
        _sntprintf(po->name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, 
            _T("Push POI name (1)"));
        po->type = PO_POI;
        break;
    default:
        return FALSE;
        break;
    }

     NAVSIN_MSG("%s(): name=\"%S\", type=%u\n", __FUNCTION__, po->name, po->type);
    
    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_GetPushObjectDetail() const
{
    if (!NAVSIMOPT_PushObjectEnable) {
        return RET_NOT_PROCESSED;
    }
    
    struct {
        uint8_t c0;
        uint8_t c1;
    } data;
    
    memset(&data, 0, sizeof(data));
    
    switch (TriggerPushObject_type) {
    case NAV_USER_PO_GUIDANCE:
        data.c0 = (const uint8_t)NavSend_NavDataAcknowledgement;
        data.c1 = (const uint8_t)_ND_CMD_GUIDANCE_PO_ELEMENT;
        SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 0);
        break;
    case NAV_USER_PO_POI:
        data.c0 = (const uint8_t)NavSend_NavDataAcknowledgement;
        data.c1 = (const uint8_t)_ND_CMD_POI_PO_ELEMENT;
        SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 0);
        break;
    default:
        return FALSE;
        break;
    }

    NAVSIN_MSG("%s()\n", __FUNCTION__);
    
    return TRUE;
}

static struct_POIPushObjectData command_GetPOIPushObjectDetailData_element;
BOOL HMINavigationAccessLibUser::command_GetPOIPushObjectDetailData(struct_POIPushObjectData * element) const
{
    if (!NAVSIMOPT_PushObjectEnable) {
        return RET_NOT_PROCESSED;
    }
    
    memset(&command_GetPOIPushObjectDetailData_element, 0, sizeof(command_GetPOIPushObjectDetailData_element));
    _sntprintf(command_GetPOIPushObjectDetailData_element.name, MAX_SHORT_LOCATION_NAME_STRING_LENGTH, 
        _T("Push POI name (1)"));
    _sntprintf(command_GetPOIPushObjectDetailData_element.address, MAX_ADDR_STRING_LENGTH, 
        _T("Push POI address (1)"));
    _sntprintf(command_GetPOIPushObjectDetailData_element.number, MAX_TEL_STRING_LENGTH, 
        _T("18616620337"));

    memcpy(element, &command_GetPOIPushObjectDetailData_element, sizeof(struct_POIPushObjectData));

    NAVSIN_MSG("%s()\n", __FUNCTION__);
        
    return TRUE;
}

static struct_GuidancePushObjectData command_GetGuidancePushObjectDetailData_element;
static int command_GetGuidancePushObjectDetailData_BlankId = 1;
BOOL HMINavigationAccessLibUser::command_GetGuidancePushObjectDetailData(struct_GuidancePushObjectData * element) const
{
    if (!NAVSIMOPT_PushObjectEnable) {
        return RET_NOT_PROCESSED;
    }
    
    int i;
    
    memset(&command_GetGuidancePushObjectDetailData_element, 0, sizeof(command_GetGuidancePushObjectDetailData_element));
    command_GetGuidancePushObjectDetailData_element.sizeofList = 5;
    _sntprintf(command_GetGuidancePushObjectDetailData_element.locationName[0], MAX_SHORT_LOCATION_NAME_STRING_LENGTH, 
        _T("Push Guidance Dest (1)"));
    for (i = 1; i < command_GetGuidancePushObjectDetailData_element.sizeofList; i++) {
        if (i != command_GetGuidancePushObjectDetailData_BlankId) {
            _sntprintf(command_GetGuidancePushObjectDetailData_element.locationName[i], MAX_SHORT_LOCATION_NAME_STRING_LENGTH, 
                _T("Push Guidance State %u"), i);
        } else {
            _sntprintf(command_GetGuidancePushObjectDetailData_element.locationName[i], MAX_SHORT_LOCATION_NAME_STRING_LENGTH, 
                _T(""));
        }
    }

    memcpy(element, &command_GetGuidancePushObjectDetailData_element, sizeof(struct_GuidancePushObjectData));

    NAVSIN_MSG("%s()\n", __FUNCTION__);
    
    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_DeletePushObject() const
{
    if (!NAVSIMOPT_PushObjectEnable) {
        return RET_NOT_PROCESSED;
    }

    NAVSIN_MSG("%s()\n", __FUNCTION__);
    
    return TRUE;
}

static enum_DynaMode command_GetPushObjectDynaMode_dynaMode = DM_OFF;
BOOL HMINavigationAccessLibUser::command_GetPushObjectDynaMode(enum_DynaMode * dynaMode) const
{
    if (!NAVSIMOPT_PushObjectEnable) {
        return RET_NOT_PROCESSED;
    }
    
    *dynaMode = command_GetPushObjectDynaMode_dynaMode;

    NAVSIN_MSG("%s(): dynaMode=%u\n", __FUNCTION__ , 
        command_GetPushObjectDynaMode_dynaMode);
    
    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_SetPushObjectDynaMode(enum_DynaMode dynaMode) const
{
    if (!NAVSIMOPT_PushObjectEnable) {
        return RET_NOT_PROCESSED;
    }
    
    command_GetPushObjectDynaMode_dynaMode = dynaMode;

    NAVSIN_MSG("%s(): dynaMode=%u\n", __FUNCTION__, 
        command_GetPushObjectDynaMode_dynaMode);

    return TRUE;
}

static struct_FavouriteListStatus s_FavListStatus;
static BOOL ChoicesValue_isFavouriteFull[] = {
    TRUE,
    FALSE,
};
static unsigned int ChoicesNo_isFavouriteFull = 0;
BOOL HMINavigationAccessLibUser::command_GetFavouriteListStatus() const
{
    if (!NAVSIMOPT_FavoriteEnable) {
        return RET_NOT_PROCESSED;
    }

    memset(&s_FavListStatus, 0, sizeof(s_FavListStatus));
    s_FavListStatus.isHomeExist = FALSE;
    s_FavListStatus.isOfficeExist = FALSE;
    s_FavListStatus.isFavouriteFull = ChoicesValue_isFavouriteFull[ChoicesNo_isFavouriteFull];

    SendUmsgToNaviHmi(entNavSend_FavListStatus, (const uint8_t *)&s_FavListStatus, sizeof(s_FavListStatus), 2000);

    NAVSIN_MSG("%s(): s_FavListStatus.isFavouriteFull=%u\n", __FUNCTION__, 
        s_FavListStatus.isFavouriteFull);

    return TRUE;
}

static uint8_t ChoicesValue_AddFavourite_status[] = {
    FS_ERROR_DUPLICATE,
    FS_ADD_SUCCESS,
};
static unsigned int ChoicesNo_AddFavourite_status = 0;
BOOL HMINavigationAccessLibUser::command_AddFavourite(struct_FavouriteElement* element) const
{
    if (!NAVSIMOPT_FavoriteEnable) {
        return RET_NOT_PROCESSED;
    }

    uint8_t data[2];

    data[0] = NavSend_SendFavouriteListChangeStatus;
    data[1] = ChoicesValue_AddFavourite_status[ChoicesNo_AddFavourite_status];
    
    SendUmsgToNaviHmi(entNaviStatusUpdate, data, sizeof(data), 2000);

    NAVSIN_MSG("%s(): status=%u\n", __FUNCTION__, ChoicesValue_AddFavourite_status[ChoicesNo_AddFavourite_status]);

    return TRUE;
}

static enum_DestinationStageErrorCode ChoicesValue_SetDestination_error[] = {
    DSEC_Success,
    DSEC_Error_Generic,
};
static unsigned int ChoicesNo_SetDestination_error = 0;
BOOL HMINavigationAccessLibUser::command_SetDestination(struct_DestinationInfo* destinationInfo) const
{
    if (!NAVSIMOPT_RouteEnable) {
        return RET_NOT_PROCESSED;
    }

    struct_DestinationStageStatus status;
    memset(&status, 0, sizeof(status));
    //status.error = ChoicesValue_SetDestination_error[ChoicesNo_SetDestination_error];
    status.error = (enum_DestinationStageErrorCode)0;
    SendUmsgToNaviHmi(entNavSend_DestinationStageStatus, (const uint8_t *)&status, sizeof(status), 2000);

    NAVSIN_MSG("%s(): status.error=%u\n", __FUNCTION__, status.error);

    return TRUE;
}

static bool command_AbortRouteCalculation_bIsCanceled = false;
BOOL HMINavigationAccessLibUser::command_AbortRouteCalculation() const
{
    if (!NAVSIMOPT_RouteEnable) {
        return RET_NOT_PROCESSED;
    }

    command_AbortRouteCalculation_bIsCanceled = true;

    NAVSIN_MSG("%s()\n", __FUNCTION__);

    return TRUE;
}

static bool command_StartRouteCalculation_IsCanceled(void)
{
    return command_AbortRouteCalculation_bIsCanceled;
}

static enum_RouteCalcStatus ChoicesValue_routeCalculationStatus[] = {
    RCS_STARTED,
    RCS_FINISHED,
    RCS_FINISHED_AVOIDANCE_BYPASS,
    //SWDD_09b change
    //RCS_FINISHED_DETOUR_BYPASS,
    RCS_FINISHED_DETOUR_FAILURE,
    RCS_ABORTED,
    RCS_ERROR,
    RCS_ERROR_DUPLICATE_STAGE,
    //SWDD07i
    RCS_RECOVERED,
    RCS_RECOVER_ERROR
};
static unsigned int ChoicesNo_routeCalculationStatus = 1;
BOOL HMINavigationAccessLibUser::command_StartRouteCalculation() const
{
    if (!NAVSIMOPT_RouteEnable) {
        return RET_NOT_PROCESSED;
    }

    command_AbortRouteCalculation_bIsCanceled = false;

    RouteCalStatus routeCalStatus;
    memset(&routeCalStatus, 0, sizeof(routeCalStatus));
    routeCalStatus.routeCalculationStatus = ChoicesValue_routeCalculationStatus[ChoicesNo_routeCalculationStatus];
    SendUmsgToNaviHmi(entNavSend_SendRouteCalculationStatus_, (const uint8_t *)&routeCalStatus, sizeof(routeCalStatus), 5000,
        &command_StartRouteCalculation_IsCanceled);

    NAVSIN_MSG("%s(): routeCalStatus.routeCalculationStatus=%u\n", __FUNCTION__, 
        routeCalStatus.routeCalculationStatus);

    return TRUE;
}

#define MAKE_TEST_GPS(lat_base,lng_base,i)      { (lat_base) + 100000 + 1000 * (i), (lng_base) + 100000 - 1000 * (i) }
static struct_GeoLocation command_GetDestinationByGeo_locationArray[] = {
    MAKE_TEST_GPS(31223501, 121445299, 0),
    MAKE_TEST_GPS(31223501, 121445299, 1),
    MAKE_TEST_GPS(31223501, 121445299, 2),
    MAKE_TEST_GPS(31223501, 121445299, 3),
    MAKE_TEST_GPS(31223501, 121445299, 4),
    MAKE_TEST_GPS(31223501, 121445299, 5),
};
static unsigned int command_GetDestinationByGeo_nLocationArrayLen = 1;
BOOL HMINavigationAccessLibUser::command_GetDestinationByGeo(struct_GeoArray * pstGeoBuffer) const
{    
    if (!NAVSIMOPT_Route2Enable) {
        return RET_NOT_PROCESSED;
    }

    if (pstGeoBuffer == NULL) {
        return FALSE;
    }

    memset(pstGeoBuffer, 0, sizeof(struct_GeoArray));
    pstGeoBuffer->isShifted = 1;

    unsigned int nMaxLen = sizeof(command_GetDestinationByGeo_locationArray) / sizeof(struct_GeoLocation);
    unsigned int nCopyLen = command_GetDestinationByGeo_nLocationArrayLen <= nMaxLen ? command_GetDestinationByGeo_nLocationArrayLen : nMaxLen;
    nCopyLen = nCopyLen <= (MAX_NO_STAGES - 1) ? nCopyLen : (MAX_NO_STAGES - 1);
    memcpy(pstGeoBuffer->locationArray, command_GetDestinationByGeo_locationArray, nCopyLen * sizeof(struct_GeoLocation));

    return TRUE;
}


BOOL HMINavigationAccessLibUser::command_MoveMap(struct_MapCoordinates* coordinates) const
{
    if (!NAVSIMOPT_MapEnable) {
        return RET_NOT_PROCESSED;
    }

    NAVSIN_MSG("%s(): (x,y)=(%u,%u)\n", __FUNCTION__, coordinates->x_coord, coordinates->y_coord);
    
    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_StopMoveMap() const
{
    if (!NAVSIMOPT_MapEnable) {
        return RET_NOT_PROCESSED;
    }

    NAVSIN_MSG("%s()\n", __FUNCTION__);
    
    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_SelectPointOnMap(struct_MapCoordinates* coordinate) const
{
    if (!NAVSIMOPT_MapEnable) {
        return RET_NOT_PROCESSED;
    }

    NAVSIN_MSG("%s(): (x,y)=(%u,%u)\n", __FUNCTION__, coordinate->x_coord, coordinate->y_coord);
    
    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_SetCCPDisplay(BOOL showCCP) const
{
    if (!NAVSIMOPT_MapEnable) {
        return RET_NOT_PROCESSED;
    }

    NAVSIN_MSG("%s(): showCCP=%u\n", __FUNCTION__, showCCP);
    
    return TRUE;
}

static enum_ZoomLevel command_SetZoomLevel_zoomLevel = ZL_10KM;
BOOL HMINavigationAccessLibUser::command_SetZoomLevel(enum_ZoomLevel zoomLevel) const
{
    if (!NAVSIMOPT_MapEnable) {
        return RET_NOT_PROCESSED;
    }

    command_SetZoomLevel_zoomLevel = zoomLevel;
    
    uint8_t data[2];
    memset(&data, 0, sizeof(data));
    data[0] = NavSend_SendZoomLevel;
    data[1] = command_SetZoomLevel_zoomLevel;
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 0);
    
    NAVSIN_MSG("%s(): zoomLevel=%u\n", __FUNCTION__, zoomLevel);
    
    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_GetZoomLevel() const
{
    if (!NAVSIMOPT_MapEnable) {
        return RET_NOT_PROCESSED;
    }

    uint8_t data[2];
    memset(&data, 0, sizeof(data));
    data[0] = NavSend_SendZoomLevel;
    data[1] = command_SetZoomLevel_zoomLevel;
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 0);

    NAVSIN_MSG("%s(): zoomLevel=%u\n", __FUNCTION__, command_SetZoomLevel_zoomLevel);
    
    return TRUE;
}

static struct_ConnNavState command_GetConnNavEnableSetting_ConnNavState = {
    1, 1, 1, 1, 1, 1
};
BOOL HMINavigationAccessLibUser::command_GetConnNavEnableSetting(struct_ConnNavState * connNavState) const
{
    if (!NAVSIMOPT_NaviEnableSetting) {
        return RET_NOT_PROCESSED;
    }
    
    connNavState->ConnNav_Local = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Local;
    connNavState->ConnNav_Gas = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Gas;
    connNavState->ConnNav_Parking = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Parking;
    connNavState->ConnNav_Weather = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Weather;
    connNavState->ConnNav_Traffic = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Traffic;
    connNavState->ConnNav_PushObject = command_GetConnNavEnableSetting_ConnNavState.ConnNav_PushObject;

    NAVSIN_MSG("%s(): Local=%u, Gas=%u, Parking=%u, Weather=%u, Traffic=%u, PushObject=%u\n", __FUNCTION__, 
        command_GetConnNavEnableSetting_ConnNavState.ConnNav_Local,
        command_GetConnNavEnableSetting_ConnNavState.ConnNav_Gas,
        command_GetConnNavEnableSetting_ConnNavState.ConnNav_Parking,
        command_GetConnNavEnableSetting_ConnNavState.ConnNav_Weather,
        command_GetConnNavEnableSetting_ConnNavState.ConnNav_Traffic,
        command_GetConnNavEnableSetting_ConnNavState.ConnNav_PushObject
    );


    return TRUE;
}

static short command_GetTrafficBargraph_index = 0;
static short command_GetTrafficBargraph_noOfElements = 0;
BOOL HMINavigationAccessLibUser::command_GetTrafficBargraph(short index,short noOfElements) const
{
    if (!NAVSIMOPT_TpegEnable) {
        return RET_NOT_PROCESSED;
    }

    command_GetTrafficBargraph_index = index >= 0 ? index : 0;
    command_GetTrafficBargraph_noOfElements = noOfElements >= 0 ? noOfElements : 0;

    uint8_t data[2];
    memset(&data, 0, sizeof(data));
    data[0] = NavSend_NavDataAcknowledgement;
    data[1] = _ND_CMD_TPEG_BARGRAPH_LIST;
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 0);

    NAVSIN_MSG("%s(): index=%d, noOfElements=%d\n", __FUNCTION__, index, noOfElements);
    
    return TRUE;
}

//	typedef enum tenum_TrafficFlowBargraph{
//	    TRAFFIC_FLOW_BG_CHOKING,
//	    TRAFFIC_FLOW_BG_CROWDED,
//	    TRAFFIC_FLOW_BG_FREE,
//	    TRAFFIC_FLOW_BG_SLOW,
//	    TRAFFIC_FLOW_BG_UNDEFINED
//	}enum_TrafficFlowBargraph;
//	
//	typedef struct tstruct_TrafficFlowInfo{
//	    int nNextDist;
//	    enum_TrafficFlowBargraph nTrafficEvent;
//	}struct_TrafficFlowInfo;
//
#define TRAFFIC_FLOW_LIST_ITEM_COUNT    35
static struct_TrafficFlowInfo command_GetTrafficBargraphData__TrafficFlowInfos[TRAFFIC_FLOW_LIST_ITEM_COUNT] = {
    { 200, TRAFFIC_FLOW_BG_CHOKING }, 
    { 100, TRAFFIC_FLOW_BG_CROWDED }, 
    { 20 , TRAFFIC_FLOW_BG_FREE }, 
    { 400, TRAFFIC_FLOW_BG_SLOW }, 
    { 100, TRAFFIC_FLOW_BG_CHOKING }, 
    
    { 200, TRAFFIC_FLOW_BG_CROWDED }, 
    { 100, TRAFFIC_FLOW_BG_FREE }, 
    { 10 , TRAFFIC_FLOW_BG_SLOW }, 
    { 50 , TRAFFIC_FLOW_BG_CHOKING }, 
    { 100, TRAFFIC_FLOW_BG_CROWDED }, 
    
    { 200, TRAFFIC_FLOW_BG_FREE }, 
    { 100, TRAFFIC_FLOW_BG_SLOW }, 
    { 200, TRAFFIC_FLOW_BG_CHOKING }, 
    { 100, TRAFFIC_FLOW_BG_CROWDED }, 
    { 200, TRAFFIC_FLOW_BG_FREE }, 
    
    { 100, TRAFFIC_FLOW_BG_SLOW }, 
    { 200, TRAFFIC_FLOW_BG_CHOKING }, 
    { 100, TRAFFIC_FLOW_BG_CROWDED }, 
    { 70 , TRAFFIC_FLOW_BG_FREE }, 
    { 120, TRAFFIC_FLOW_BG_SLOW }, 
    
    { 250, TRAFFIC_FLOW_BG_CHOKING }, 
    { 100, TRAFFIC_FLOW_BG_CROWDED }, 
    { 200, TRAFFIC_FLOW_BG_FREE }, 
    { 100, TRAFFIC_FLOW_BG_SLOW }, 
    { 100, TRAFFIC_FLOW_BG_CHOKING }, 
    
    { 30 , TRAFFIC_FLOW_BG_CROWDED }, 
    { 300, TRAFFIC_FLOW_BG_FREE }, 
    { 100, TRAFFIC_FLOW_BG_SLOW }, 
    { 200, TRAFFIC_FLOW_BG_CHOKING }, 
    { 100, TRAFFIC_FLOW_BG_CROWDED }, 
    
    { 100, TRAFFIC_FLOW_BG_FREE }, 
    { 200, TRAFFIC_FLOW_BG_SLOW }, 
    { 100, TRAFFIC_FLOW_BG_CHOKING }, 
    { 40 , TRAFFIC_FLOW_BG_CROWDED }, 
    { 100, TRAFFIC_FLOW_BG_FREE }, 
};
//	typedef struct tstruct_TrafficFlowList{
//	
//		unsigned short 				sizeOfList;
//		unsigned long				distanceToStage[MAX_NO_STAGES];
//		struct_TrafficFlowInfo		element[MAX_TRAFFIC_BARGRAPH_LENGTH];
//	}struct_TrafficFlowList;
static unsigned int command_GetTrafficBargraphData__TrafficFlowInfoCount = 13;
BOOL HMINavigationAccessLibUser::command_GetTrafficBargraphData(struct_TrafficFlowList * flowList) const
{   
    if (!NAVSIMOPT_TpegEnable) {
        return RET_NOT_PROCESSED;
    }
    
    memset(flowList, 0, sizeof(struct_TrafficFlowList));
	
    flowList->sizeOfList = command_GetTrafficBargraphData__TrafficFlowInfoCount;
    memcpy(flowList->element, &command_GetTrafficBargraphData__TrafficFlowInfos[0], 
        sizeof(struct_TrafficFlowInfo) * flowList->sizeOfList);
    flowList->distanceToStage[0] = 0;
    flowList->distanceToStage[1] = 20;
    flowList->distanceToStage[2] = 30;
    flowList->distanceToStage[3] = 100;
    flowList->distanceToStage[4] = 200;
    flowList->distanceToStage[5] = 250;
	
    NAVSIN_MSG("%s(): sizeOfList=%d; stage[0]=%d; stage[1]=%d; stage[2]=%d; stage[3]=%d; stage[4]=%d; stage[4]=%d; \n", 
        __FUNCTION__, flowList->sizeOfList,
        flowList->distanceToStage[0],
        flowList->distanceToStage[1],
        flowList->distanceToStage[2],
        flowList->distanceToStage[3],
        flowList->distanceToStage[4],
        flowList->distanceToStage[5]
    );

    return TRUE;
}

//
// Update Traffic Bargraph Info
//
static BOOL NS_UpdateTrafficBargraph(std::vector<std::string> &vCmdLine)
{
    if (!NAVSIMOPT_TpegEnable) {
        return RET_NOT_PROCESSED;
    }
    
    BOOL ret = FALSE;
    unsigned int nItemCount = 0;

    if (vCmdLine.size() >= 3 
        && TK_Tools::CompareNoCase(vCmdLine[0], "Update") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "TrafficBargraph") == 0
        && TK_Tools::CompareNoCase(vCmdLine[2], "-") == 0) {
        if (vCmdLine.size() >= 4) {
            nItemCount = strtoul(vCmdLine[3].c_str(), NULL, 10);
            if (nItemCount >= TK_ARR_LEN(command_GetTrafficBargraphData__TrafficFlowInfos)) {
                nItemCount = TK_ARR_LEN(command_GetTrafficBargraphData__TrafficFlowInfos);
            }
        }
    }

    ret = TRUE;

    command_GetTrafficBargraphData__TrafficFlowInfoCount = nItemCount;
    command_GetTrafficBargraph_index = 0;
    command_GetTrafficBargraph_noOfElements = MAX_LIST_SIZE;

    uint8_t data[2];
    memset(&data, 0, sizeof(data));
    data[0] = NavSend_NavDataAcknowledgement;
    data[1] = _ND_CMD_TPEG_BARGRAPH_LIST;
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 0);

    NAVSIN_MSG("%s(): index=%d, noOfElements=%d, TrafficFlowInfoCount=%d\n", __FUNCTION__, 
        command_GetTrafficBargraph_index, 
        command_GetTrafficBargraph_noOfElements,
        command_GetTrafficBargraphData__TrafficFlowInfoCount
    );
    
    return ret;
}

static unsigned long command_GetJourneyInfoData__distanceToDestination = (unsigned long)-1;
BOOL HMINavigationAccessLibUser::command_GetJourneyInfoData(struct_JourneyInfo2 * pstrJourneyInfo)
{
    if (!NAVSIMOPT_TpegEnable) {
        return RET_NOT_PROCESSED;
    }

    if (pstrJourneyInfo == NULL) {
        return FALSE;
    }

    memset(pstrJourneyInfo, 0, sizeof(struct_JourneyInfo2));
    pstrJourneyInfo->distanceToDestinationRaw = command_GetJourneyInfoData__distanceToDestination;

    return TRUE;
}

//
// Update Journey Info: <nDistanceFromCcpToDest>
//
static BOOL NS_UpdateJourneyInfo(std::vector<std::string> &vCmdLine)
{
    tstruct_JourneyInfo2 stJourneyInfo;
    
    if (!NAVSIMOPT_TpegEnable) {
        return RET_NOT_PROCESSED;
    }
    
    BOOL ret = FALSE;
    unsigned long nDistanceFromCcpToDest = (unsigned long)-1;

    if (vCmdLine.size() >= 3 
        && TK_Tools::CompareNoCase(vCmdLine[0], "Update") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "JourneyInfo") == 0
        && TK_Tools::CompareNoCase(vCmdLine[2], "-") == 0) {
        if (vCmdLine.size() >= 4) {
            nDistanceFromCcpToDest = strtoul(vCmdLine[3].c_str(), NULL, 10);
        }
    }

    ret = TRUE;

    command_GetJourneyInfoData__distanceToDestination = nDistanceFromCcpToDest;
    
    memset(&stJourneyInfo, 0, sizeof(tstruct_JourneyInfo2));
    stJourneyInfo.distanceToDestinationRaw = command_GetJourneyInfoData__distanceToDestination;
    SendUmsgToNaviHmi(entNavSend_SendJourneyInfo, (const uint8_t *)&stJourneyInfo, sizeof(tstruct_JourneyInfo2), 0);
    NAVSIN_MSG("%s(): distanceToDestinationRaw=%u\n", __FUNCTION__, command_GetJourneyInfoData__distanceToDestination);
    
    return ret;
}

static enum_TPEGMode command_GetTPEGMode__TPEGMode = TPEGM_MAP_ON;
BOOL HMINavigationAccessLibUser::command_GetTPEGMode(enum_TPEGMode * mode) const
{
    if (!NAVSIMOPT_TpegEnable) {
        return RET_NOT_PROCESSED;
    }

    NAVSIN_MSG("%s(): mode=%u\n", __FUNCTION__, command_GetTPEGMode__TPEGMode);

    *mode = command_GetTPEGMode__TPEGMode;
    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_SetTPEGMode(enum_TPEGMode mode) const
{
    if (!NAVSIMOPT_TpegEnable) {
        return RET_NOT_PROCESSED;
    }

    uint8_t data[2];
    memset(&data, 0, sizeof(data));
    data[0] = NavSend_SendTPEGMode;
    data[1] = (uint8_t)mode;
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 0);

    NAVSIN_MSG("%s(): mode=%u\n", __FUNCTION__, mode);

    command_GetTPEGMode__TPEGMode = mode;
    return TRUE;
}

//	===>
//	typedef struct tstruct_TPEGEvent
//	{
//	  int distance;
//	  int iconID;
//	  TCHAR shortDescription[MAX_STRING_LENGTH];
//	  //SWDD_09b remove
//	  //unsigned short index;
//	  bool isOnRoute;
//	}struct_TPEGEvent;
//	
//	/**
//	 * \struct  tstruct_TPEGEventList
//	 *
//	 * \brief     Structure containing information on the TPEG event list
//	 */
//	typedef struct tstruct_TPEGEventList
//	{
//	  //SWDD_09b add
//	  unsigned short index;
//	
//	  short sizeOfList;
//	  short noOfElementsRetreived;
//	  struct_TPEGEvent elements[MAX_LIST_SIZE];
//	}struct_TPEGEventList;
//	===<

static struct_TPEGEvent command_GetTPEGListElements__TPEGEvents[31] = { 0 };
static void command_GetTPEGListElements__TPEGEvents_Init()
{
    static bool bInit = false;
    unsigned int i;

    if (!bInit) {
        bInit = true;
    } else {
        return;
    }

    for (i = 0; i < TK_ARR_LEN(command_GetTPEGListElements__TPEGEvents); i++) {
        struct_TPEGEvent &rTpegEvent = command_GetTPEGListElements__TPEGEvents[i];
        memset(&rTpegEvent, 0, sizeof(struct_TPEGEvent));
        rTpegEvent.distance= i;
        rTpegEvent.iconID= i % 10;
        _snwprintf(rTpegEvent.shortDescription, MAX_STRING_LENGTH, L"(Tpeg Event %04u)", i);
        rTpegEvent.isOnRoute = (i % 2) == 1;
    }
}
static short command_GetTPEGListElements__index = 0;
static unsigned int command_GetTPEGListElements__sizeOfList = TK_ARR_LEN(command_GetTPEGListElements__TPEGEvents);
BOOL HMINavigationAccessLibUser::command_GetTPEGListElements(short index, short noOfElements) const
{
    if (!NAVSIMOPT_TpegEnable) {
        return RET_NOT_PROCESSED;
    }

    command_GetTPEGListElements__index = index;

    UINT8_T data[2];
	
    data[0] = (const uint8_t)NavSend_NavDataAcknowledgement;
    data[1] = (const uint8_t)_ND_CMD_TPEG_LIST;
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 50);
    NAVSIN_MSG("%s(): index=%u, noOfElements=%u\n", __FUNCTION__, 
        index, noOfElements);

    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_GetTPEGListElementsData(struct_TPEGEventList* TPEGListElements) const
{
    if (!NAVSIMOPT_TpegEnable) {
        return RET_NOT_PROCESSED;
    }

    unsigned int i;

    command_GetTPEGListElements__TPEGEvents_Init();

    if (TPEGListElements == NULL) {
        return FALSE;
    }

    memset(TPEGListElements, 0, sizeof(struct_TPEGEventList));
    TPEGListElements->index = command_GetTPEGListElements__index;
    TPEGListElements->sizeOfList = command_GetTPEGListElements__sizeOfList;
    
    TPEGListElements->noOfElementsRetreived = 0;
    for (i = command_GetTPEGListElements__index
        ; 
        i < command_GetTPEGListElements__index + 5 
        && i < TK_ARR_LEN(command_GetTPEGListElements__TPEGEvents)
        && i < command_GetTPEGListElements__sizeOfList
        ;
        i++) {
        memcpy(
            &TPEGListElements->elements[TPEGListElements->noOfElementsRetreived], 
            &command_GetTPEGListElements__TPEGEvents[i],
            sizeof(struct_TPEGEvent)
        );

        TPEGListElements->noOfElementsRetreived++;
    }

    return TRUE;
}

static unsigned int command_GetWeatherData__WeatherType = WT_INVALID;
BOOL HMINavigationAccessLibUser::command_GetWeatherData( enum_WeatherType* weatherType) const
{
    if (!NAVSIMOPT_WeatherEnable) {
        return RET_NOT_PROCESSED;
    }

    if (weatherType == NULL) {
        return FALSE;
    }

    *weatherType = (enum_WeatherType)command_GetWeatherData__WeatherType;

    return TRUE;
}

//
// Update CcpWeatherType - <nCcpWeatherType>
//
static BOOL NS_UpdateCcpWeatherType(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    
    UINT_T nCcpWeatherType;

    if (vCmdLine.size() >= 4 
        && TK_Tools::CompareNoCase(vCmdLine[0], "Update") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "CcpWeatherType") == 0) {

        nCcpWeatherType = (UINT_T)TK_Tools::StrToUL(vCmdLine[3]);
        command_GetWeatherData__WeatherType = nCcpWeatherType;
        
        ret = TRUE;
    }
    
    return ret;
}

static struct_WeatherElement command_GetRouteWeatherData__WeatherElements[300] = { 0 };
static unsigned int command_GetRouteWeatherData__WeatherElementsCount = 210;
BOOL HMINavigationAccessLibUser::command_GetRouteWeatherData(struct_RouteWeatherList* weatherList) const
{
    static bool bInit = false;
    unsigned int nWeatherElementsCount = command_GetRouteWeatherData__WeatherElementsCount;
    unsigned int i;
    unsigned int nRetrievedCount = 0;
    
    if (!NAVSIMOPT_WeatherEnable) {
        return RET_NOT_PROCESSED;
    }

    if (weatherList == NULL) {
        return FALSE;
    }

    if (!bInit) {
        for (i = 0; i < nWeatherElementsCount; i++) {
            struct_WeatherElement &rWeatherElement = command_GetRouteWeatherData__WeatherElements[i];
            _snwprintf(rWeatherElement.cityName, MAX_CITY_STRING_LENGTH, L"(city name %03u)", i);
            rWeatherElement.tempLow = 10 + (i % 30);
            rWeatherElement.tempHigh = rWeatherElement.tempLow + ((i * 2) % 10);
            rWeatherElement.weatherType = (enum_WeatherType)(i % (WT_LIGHT_FOG + 1));
            _snwprintf(rWeatherElement.weatherData.windData, MAX_WEATHER_CONTENT_LENGTH, L"(wind data %03u)", i);
            _snwprintf(rWeatherElement.weatherData.humidityData, MAX_WEATHER_CONTENT_LENGTH, L"(humidity data %03u)", i);
        }
        bInit = true;
    }

    for (i = weatherList->index; i < (weatherList->index + MAX_LIST_SIZE) && i < nWeatherElementsCount; i++) {
        memcpy(&weatherList->elements[i - weatherList->index], &command_GetRouteWeatherData__WeatherElements[i], sizeof(struct_WeatherElement));
    }
    weatherList->sizeOfList = nWeatherElementsCount;
    weatherList->noOfElementsRetreived = i - weatherList->index;
    
    return true;
}

//
// Update WeatherElementsCount - <nCount>
//
static BOOL NS_UpdateWeatherElementsCount(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    
    UINT_T nWeatherElementsCount;

    if (vCmdLine.size() >= 4 
        && TK_Tools::CompareNoCase(vCmdLine[0], "Update") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "WeatherElementsCount") == 0) {

        nWeatherElementsCount = (UINT_T)TK_Tools::StrToUL(vCmdLine[3]);
        if (nWeatherElementsCount > TK_ARR_LEN(command_GetRouteWeatherData__WeatherElements)) {
            nWeatherElementsCount = TK_ARR_LEN(command_GetRouteWeatherData__WeatherElements);
        }
        command_GetRouteWeatherData__WeatherElementsCount = nWeatherElementsCount;

        UINT8_T data[2];
	
        data[0] = (const uint8_t)NavSend_NavDataAcknowledgement;
        data[1] = (const uint8_t)_ND_CMD_WEATHER_LIST_ELEMENT_DATA;
        SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 1);
        NAVSIN_MSG("%s(): nWeatherElementsCount=%u\n", __FUNCTION__, nWeatherElementsCount);
        
        ret = TRUE;
    }
    
    return ret;
}

//
// DictaphoneOp - <nOperation>
//
static BOOL NS_DictaphoneOp(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    unsigned int nOperation;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "DictaphoneOp") == 0) {
        if (vCmdLine.size() >= 3) {
            nOperation = TK_Tools::StrToUL(vCmdLine[2]);
            switch (nOperation) {
            case 0:
                {
                    dictaphone::accesslib::CDictaPhone::getInstance()->entIDictaphone_StartRecord();
                }
                break;
            case 1:
                {
                    dictaphone::accesslib::CDictaPhone::getInstance()->entIDictaphone_StopRecord();
                }
                break;
            default:
                {
                    NAVSIN_MSG("%s(): Unknown operation: %u\n", __FUNCTION__, nOperation);
                }
            }
            
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// TriggerBleep - <nBleepType>
//
static BOOL NS_TriggerBleep(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    unsigned int nBleepType;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "TriggerBleep") == 0) {
        if (vCmdLine.size() >= 3) {
            nBleepType = TK_Tools::StrToUL(vCmdLine[2]);
            if (nBleepType < BleepMax) {
                entIAV_GenerateBleep(nBleepType);
            } else {
                NAVSIN_MSG("%s(): Invalid BleepType: %u\n", __FUNCTION__, nBleepType);
            }
            
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// SetSetTpegMsgListSize - <nSize>
//
static BOOL NS_SetTpegMsgListSize(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SetTpegMsgListSize") == 0) {
        if (vCmdLine.size() >= 3) {
            command_GetTPEGListElements__sizeOfList = TK_Tools::StrToUL(vCmdLine[2]);
            if (command_GetTPEGListElements__sizeOfList > TK_ARR_LEN(command_GetTPEGListElements__TPEGEvents)) {
                command_GetTPEGListElements__sizeOfList = TK_ARR_LEN(command_GetTPEGListElements__TPEGEvents);
            }
            NAVSIN_MSG("%s(): command_GetTPEGListElements__sizeOfList=%u\n", __FUNCTION__, command_GetTPEGListElements__sizeOfList);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// UpdateSpeedLimit - <nSpeedLimit> <bOverSpeed>
//
static BOOL NS_UpdateSpeedLimit(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    int nSpeedLimit = 0;
    BOOL bOverSpeed = FALSE;
    struct_SpeedLimitNotification sln;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "UpdateSpeedLimit") == 0) {
        if (vCmdLine.size() >= 3) {
            nSpeedLimit = TK_Tools::StrToL(vCmdLine[2]);
        }

        if (vCmdLine.size() >= 4) {
            bOverSpeed = TK_Tools::StrToUL(vCmdLine[3]) != 0;
        }

        memset(&sln, 0, sizeof(struct_SpeedLimitNotification));
        sln.speedLimitValue = nSpeedLimit;
        sln.overspeeding = bOverSpeed;
        SendUmsgToNaviHmi(entNavSend_SpeedLimitStatus, (const uint8_t *)&sln, sizeof(sln), 0);

        NAVSIN_MSG("%s(): nSpeedLimit=%d, bOverSpeed=%u\n", __FUNCTION__, nSpeedLimit, bOverSpeed);
        ret = TRUE;
    }
    
    return ret;
}

//
// SendNaviDatabaseStatus - <nDBStatus>
//
//	typedef enum tenum_DatabaseStatus{
//	    DS_OK,
//	    DS_NOT_ACTIVATED,
//	    DS_READ_ERROR,
//	    DS_ACTIVATION_BYPASS,
//	    DS_NOT_SUPPORTED,
//	}enum_DatabaseStatus;
static BOOL NS_SendNaviDatabaseStatus(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    uint8_t nDBStatus = 0;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SendNaviDatabaseStatus") == 0) {
        if (vCmdLine.size() >= 3) {
            nDBStatus = (uint8_t)TK_Tools::StrToUL(vCmdLine[2]);
            NAVSIN_MSG("%s(): nDBStatus=%u\n", __FUNCTION__, nDBStatus);

            uint8_t data[2];
            data[0] = NavSend_SendDatabaseStatus;
            data[1] = nDBStatus;
            SendUmsgToNaviHmi(entNaviStatusUpdate, data, sizeof(data), 0);
    
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// WriteScreenGrpID - <nScreenGrpID>
//
//	   enum screenGroupID
//	   {
//	      GRP_NONE_ENUM = 0,      // First time startup should be 0, after restored upon nx lifecycle, should set to 0
//	      GRP_TUNER_ENUM,
//	      GRP_MEDIA_AUDIO_ENUM,
//	      GRP_MEDIA_VIDEO_ENUM,
//	      GRP_DICTA_ENUM,
//	      GRP_BTPHONE_ENUM,
//	      GRP_NAVI_ENUM,
//	      GRP_VF_ENUM,
//	      GRP_SETTINGS_ENUM,
//	      GRP_CLIMATE_ENUM,
//	      GRP_INVALID_ENUM,
//	      GRP_MAX_GRP_ID
//	   };
extern unsigned char isShowSourceScreenChecked;
static BOOL NS_WriteScreenGrpID(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    uint8_t nScreenGrpID = 0;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "WriteScreenGrpID") == 0) {
        if (vCmdLine.size() >= 3) {
            nScreenGrpID = (uint8_t)TK_Tools::StrToUL(vCmdLine[2]);
            NAVSIN_MSG("%s(): nScreenGrpID=%u\n", __FUNCTION__, nScreenGrpID);

            TK_Tools::Configuration_writeScreenGrpID(nScreenGrpID);
            isShowSourceScreenChecked = 0;
    
            ret = TRUE;
        }
    }
    
    return ret;
}

BOOL HMINavigationAccessLibUser::command_GetElevatedRoadList() const
{
    if (!NAVSIMOPT_ElevatedRouteEnable) {
        return RET_NOT_PROCESSED;
    }

    UINT8_T data[2];
	
    data[0] = (const uint8_t)NavSend_NavDataAcknowledgement;
    data[1] = (const uint8_t)_ND_CMD_ELEVATED_ROAD_LIST;
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 1);
    NAVSIN_MSG("%s():\n", __FUNCTION__);

    return TRUE;
}

static StreetName command_GetElevatedRoadListElementsData__ElevatedRouteElements[MAX_SIZE_ELEVATED_ROADS];

BOOL HMINavigationAccessLibUser::command_GetElevatedRoadListElementsData(struct_ElevatedRoadsList* pElevatedRouteList) const
{
    if (!NAVSIMOPT_ElevatedRouteEnable) {
        return RET_NOT_PROCESSED;
    }
    
    static bool bInit = false;
    unsigned int i;

    if (pElevatedRouteList == NULL) {
        return FALSE;
    }

    if (!bInit) {
        bInit = true;

        for (i = 0; i < MAX_SIZE_ELEVATED_ROADS; i++) {
            StreetName &rStreetName = command_GetElevatedRoadListElementsData__ElevatedRouteElements[i];
            _snwprintf(rStreetName, MAX_SHORT_LOCATION_NAME_STRING_LENGTH - 1, L"(Elevated Route %03u)", i);
        }
    }

    memset(pElevatedRouteList, 0, sizeof(struct_ElevatedRoadsList));
    pElevatedRouteList->noOfElementsRetreived = MAX_SIZE_ELEVATED_ROADS;
    memcpy(pElevatedRouteList->elements, command_GetElevatedRoadListElementsData__ElevatedRouteElements, 
        sizeof(command_GetElevatedRoadListElementsData__ElevatedRouteElements));

    return TRUE;
}

#define DEF_STAGE_CONTEXT(i,stage_name,stage_type) \
    { \
        (i), \
        (stage_name), \
        { 31223501 + 20000 - 1000 * (i), 121445299 + 20000 + 1000 * (i) }, \
        (stage_type), \
    }
    
static struct_StageList command_GetStageListElements_StageList = {
    0,
    MAX_NO_STAGES,
    MAX_NO_STAGES,
    {
        {
            0,
            L"(Dest_Name)",
            { 31223501, 121445299 },
            ST_DESTINATION,
        },
        DEF_STAGE_CONTEXT(1, L"(Stage_1)", ST_STAGE),
        DEF_STAGE_CONTEXT(2, L"(Stage_2)", ST_STAGE),
        DEF_STAGE_CONTEXT(3, L"(Stage_3)", ST_CODRIVE),
        DEF_STAGE_CONTEXT(4, L"(Stage_4)", ST_CODRIVE),
        DEF_STAGE_CONTEXT(5, L"(Stage_5)", ST_CODRIVE),
    },
};

static unsigned int command_GetStageListElements_nTotalDestAndStageCount = 3;
static short command_GetStageListElements_Arg_noOfElements = 0;
BOOL HMINavigationAccessLibUser::command_GetStageListElements(short index, short noOfElements) const
{
    if (!NAVSIMOPT_StageEnable) {
        return RET_NOT_PROCESSED;
    }

    command_GetStageListElements_Arg_noOfElements = noOfElements;

    UINT8_T data[2];
	
    data[0] = (const uint8_t)NavSend_NavDataAcknowledgement;
    data[1] = (const uint8_t)_ND_CMD_STAGE_LIST;
    SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 1);
    NAVSIN_MSG("%s():\n", __FUNCTION__);

    return TRUE;
}

BOOL HMINavigationAccessLibUser::command_GetStageListElementsData(struct_StageList* stages) const
{
    if (!NAVSIMOPT_StageEnable) {
        return RET_NOT_PROCESSED;
    }  

    unsigned int i;
    unsigned int nCount;

    if (stages == NULL) {
        return FALSE;
    }

    if (stages->index < 0 || stages->index >= MAX_NO_STAGES) {
        return FALSE;
    }

    for (i = stages->index; 
            i < MAX_NO_STAGES 
            && i < command_GetStageListElements_nTotalDestAndStageCount
            && i < command_GetStageListElements_Arg_noOfElements
        ; i++) {
        memcpy(&stages->elements[i - stages->index], &command_GetStageListElements_StageList.elements[i], sizeof(struct_StageElement));
    }

    nCount = i - stages->index;
    stages->sizeOfList = command_GetStageListElements_nTotalDestAndStageCount;
    stages->noOfElementsRetrieved = nCount;

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Navi Simulator Configuration
//

//
// TriggerPushObject
//
static BOOL NS_TriggerPushObject(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 1) {
        if (TK_Tools::CompareNoCase(vCmdLine[0], "PP") == 0) {
            TriggerPushObject(0);
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[0], "PG") == 0) {
            TriggerPushObject(1);
            ret = TRUE;
        } else {
            // do nothing
        }
    }

    return ret;
}

//
// PushObjectDynaMode: OFF
//
static BOOL NSC_001_001(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "001" && vCmdLine[2] == "001") {
            command_GetPushObjectDynaMode_dynaMode = DM_OFF;

            ret = TRUE;
        }
    }

    return ret;
}

//
// PushObjectDynaMode: DM_AUTOMATIC
//
static BOOL NSC_001_002(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "001" && vCmdLine[2] == "002") {
            command_GetPushObjectDynaMode_dynaMode = DM_AUTOMATIC;

            ret = TRUE;
        }
    }

    return ret;
}

//
// Favourite: suc
//
static BOOL NSC_002_001(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "002" && vCmdLine[2] == "001") {
            ChoicesNo_isFavouriteFull = 1;
            ChoicesNo_AddFavourite_status = 1;

            ret = TRUE;
        }
    }
    
    return ret;
}

//
// Favourite: duplicated
//
static BOOL NSC_002_002(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "002" && vCmdLine[2] == "002") {
            ChoicesNo_isFavouriteFull = 1;
            ChoicesNo_AddFavourite_status = 0;

            ret = TRUE;
        }
    }
    
    return ret;
}

//
// Favourite: full
//
static BOOL NSC_002_003(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "002" && vCmdLine[2] == "003") {
            ChoicesNo_isFavouriteFull = 0;

            ret = TRUE;
        }
    }

    return ret;
}

//
// SetDestination: suc
//
static BOOL NSC_003_001(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "003" && vCmdLine[2] == "001") {
            ChoicesNo_SetDestination_error = 0;

            ret = TRUE;
        }
    }

    return ret;
}

//
// SetDestination: fail
//
static BOOL NSC_003_002(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "003" && vCmdLine[2] == "002") {
            ChoicesNo_SetDestination_error = 1;

            ret = TRUE;
        }
    }

    return ret;
}

//
// routeCalculationStatus: FINISHED
//
static BOOL NSC_004_001(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "004" && vCmdLine[2] == "001") {
            ChoicesNo_SetDestination_error = 0;
            ChoicesNo_routeCalculationStatus = 1;

            ret = TRUE;
        }
    }
    
    return ret;
}

//
// routeCalculationStatus: ABORTED
//
static BOOL NSC_004_002(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "004" && vCmdLine[2] == "002") {
            ChoicesNo_SetDestination_error = 0;
            ChoicesNo_routeCalculationStatus = 4;

            ret = TRUE;
        }
    }

    return ret;
}

//
// routeCalculationStatus: ERROR
//
static BOOL NSC_004_003(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "004" && vCmdLine[2] == "003") {
            ChoicesNo_SetDestination_error = 0;
            ChoicesNo_routeCalculationStatus = 5;

            ret = TRUE;
        }
    }

    return ret;
}

//
// ConnNavEnableSetting
//
static BOOL NSC_005(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;
    
    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0) {
        if (vCmdLine[1] == "005") {
            if (vCmdLine[2] == "001") {                                 // ALL ON
                memset(&command_GetConnNavEnableSetting_ConnNavState, 0xFF, sizeof(command_GetConnNavEnableSetting_ConnNavState));

                ret = TRUE;
            } else if (vCmdLine[2] == "002") {                          // ALL OFF
                memset(&command_GetConnNavEnableSetting_ConnNavState, 0x00, sizeof(command_GetConnNavEnableSetting_ConnNavState));

                ret = TRUE;
            }
        }
    }

    return ret;
}

//
// Switch
//
static BOOL NSC_Switch(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0 
        && TK_Tools::CompareNoCase(vCmdLine[1], "SWITCH") == 0) {
        if (TK_Tools::CompareNoCase(vCmdLine[2], "ALL_ON") == 0) {
            NAVSIMOPT_PushObjectEnable = 1;
            NAVSIMOPT_FavoriteEnable = 1;
            NAVSIMOPT_RouteEnable = 1;
            NAVSIMOPT_Route2Enable = 1;
            NAVSIMOPT_StageEnable = 1;
            NAVSIMOPT_MapEnable = 1;
            NAVSIMOPT_TpegEnable = 1;
            NAVSIMOPT_WeatherEnable = 1;
            NAVSIMOPT_ElevatedRouteEnable = 1;
            NAVSIMOPT_AlertPointEnable = 1;
            NAVSIMOPT_Telematics = 1;
            NAVSIMOPT_NaviEnableSetting = 1;
            NAVSIMOPT_OthersEnable = 1;
            NAVSIMOPT_ConnNavi = 1;

            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "ALL_OFF") == 0) {
            NAVSIMOPT_PushObjectEnable = 0;
            NAVSIMOPT_FavoriteEnable = 0;
            NAVSIMOPT_RouteEnable = 0;
            NAVSIMOPT_Route2Enable = 0;
            NAVSIMOPT_StageEnable = 0;
            NAVSIMOPT_MapEnable = 0;
            NAVSIMOPT_TpegEnable = 0;
            NAVSIMOPT_WeatherEnable = 0;
            NAVSIMOPT_ElevatedRouteEnable = 0;
            NAVSIMOPT_AlertPointEnable = 0;
            NAVSIMOPT_Telematics = 0;
            NAVSIMOPT_NaviEnableSetting = 0;
            NAVSIMOPT_OthersEnable = 0;
            NAVSIMOPT_ConnNavi = 0;

            ret = TRUE;
        } else {
            
        }
    }
    
    return ret;
}

//
// Switch 2
//
//    { "NSC SWITCH help",                    &NSC_Switch2,                           "NSC SWITCH help" },
//    { "NSC SWITCH ON - <s>",                &NSC_Switch2,                           "NSC SWITCH ON - <FuncName>" },
//    { "NSC SWITCH OFF - <s>",               &NSC_Switch2,                           "NSC SWITCH OFF - <FuncName>" },
static BOOL NSC_Switch2(std::vector<std::string> &vCmdLine)
{
    static struct {
        const char *pFuncName;
        unsigned int *pFuncValue;
    } funcInfoTable[] = {
        { "PushObjectEnable",           &NAVSIMOPT_PushObjectEnable },
        { "FavoriteEnable",             &NAVSIMOPT_FavoriteEnable },
        { "RouteEnable",                &NAVSIMOPT_RouteEnable },
        { "Route2Enable",               &NAVSIMOPT_Route2Enable },
        { "StageEnable",                &NAVSIMOPT_StageEnable },
        { "MapEnable",                  &NAVSIMOPT_MapEnable },
        { "TpegEnable",                 &NAVSIMOPT_TpegEnable },
        { "WeatherEnable",              &NAVSIMOPT_WeatherEnable },
        { "ElevatedRouteEnable",        &NAVSIMOPT_ElevatedRouteEnable },
        { "AlertPointEnable",           &NAVSIMOPT_AlertPointEnable },
        { "Telematics",                 &NAVSIMOPT_Telematics },
        { "NaviEnableSetting",          &NAVSIMOPT_NaviEnableSetting },
        { "ConnNavi",                   &NAVSIMOPT_ConnNavi },
        { "OthersEnable",               &NAVSIMOPT_OthersEnable },
    };
    BOOL ret = FALSE;
    int i;
    int nFuncInfoTableItemCount = TK_ARR_LEN(funcInfoTable);

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0 
        && TK_Tools::CompareNoCase(vCmdLine[1], "SWITCH") == 0) {
        if (TK_Tools::CompareNoCase(vCmdLine[2], "help") == 0) {
            for (i = 0; i < nFuncInfoTableItemCount; i++) {
                NAVSIN_MSG("%s(): %s=%d\n", __FUNCTION__, funcInfoTable[i].pFuncName, *funcInfoTable[i].pFuncValue);    
            }
            
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "ON") == 0) {
            i = nFuncInfoTableItemCount;
            if (vCmdLine.size() >= 5) {
                for (i = 0; i < nFuncInfoTableItemCount; i++) {
                    if (TK_Tools::CompareNoCase(vCmdLine[4], funcInfoTable[i].pFuncName) == 0) {
                        *funcInfoTable[i].pFuncValue = 1;
                        NAVSIN_MSG("%s(): set %s=%d\n", __FUNCTION__, funcInfoTable[i].pFuncName, *funcInfoTable[i].pFuncValue);
                        break;
                    }
                }
                if (i == nFuncInfoTableItemCount) {
                    NAVSIN_MSG("%s(): *** the switch \"%s\" does not exist!\n", __FUNCTION__, vCmdLine[4].c_str()); 
                }
            } else {
                NAVSIN_MSG("%s(): *** no switch!\n", __FUNCTION__); 
            }            

            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "OFF") == 0) {
            i = nFuncInfoTableItemCount;
            if (vCmdLine.size() >= 5) {
                for (i = 0; i < nFuncInfoTableItemCount; i++) {
                    if (TK_Tools::CompareNoCase(vCmdLine[4], funcInfoTable[i].pFuncName) == 0) {
                        *funcInfoTable[i].pFuncValue = 0;
                        NAVSIN_MSG("%s(): set %s=%d\n", __FUNCTION__, funcInfoTable[i].pFuncName, *funcInfoTable[i].pFuncValue);
                        break;
                    }
                    
                }
                if (i == nFuncInfoTableItemCount) {
                    NAVSIN_MSG("%s(): *** the switch \"%s\" does not exist!\n", __FUNCTION__, vCmdLine[4].c_str()); 
                }
            } else {
                NAVSIN_MSG("%s(): *** no switch!\n", __FUNCTION__); 
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
static BOOL NSC_Msg(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "NSC") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "MSG") == 0) {
        if (TK_Tools::CompareNoCase(vCmdLine[2], "ON") == 0) {
            NAVSIN_Msg_bOn = true;

            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "OFF") == 0) {
            NAVSIN_Msg_bOn = false;

            ret = TRUE;
        } else {
            
        }
    }
    
    return ret;
}

//
// SYS exit
//
static void NS_Sys_StackOverflow()
{
    int nArr[1024 * 1024 * 1] = { 0 };
}
static BOOL NS_Sys(std::vector<std::string> &vCmdLine)
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
            NAVSIN_MSG("%s(): DataAbort\n", __FUNCTION__);
            int *p = (int *)0;
            *p = 1;
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "PrefetchAbort") == 0) {
            NAVSIN_MSG("%s(): PrefetchAbort\n", __FUNCTION__);
            void (*pFunc)(void) = NULL;
            pFunc();
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "StackOverflow") == 0) {
            NAVSIN_MSG("%s(): StackOverflow\n", __FUNCTION__);
            NS_Sys_StackOverflow();
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "StackRewrite") == 0) {
            NAVSIN_MSG("%s(): StackRewrite\n", __FUNCTION__);
            int nArr[1];
            memset(nArr, 0, 1024 * 1024 * 1);
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "DivZero") == 0) {
            NAVSIN_MSG("%s(): DivZero\n", __FUNCTION__);
            int nZero = 0;
            int n = 5 / nZero;
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[3], "CppException") == 0) {
            NAVSIN_MSG("%s(): CppException\n", __FUNCTION__);
            throw new std::string("TestCppException");
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// Update NaviEnableSetting
//
static BOOL NS_UpdateNaviEnableSetting(std::vector<std::string> &vCmdLine)
{
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 3 
        && TK_Tools::CompareNoCase(vCmdLine[0], "Update") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "NaviEnableSetting") == 0) {
        if (TK_Tools::CompareNoCase(vCmdLine[2], "ALL_ON") == 0) {
            memset(&command_GetConnNavEnableSetting_ConnNavState, 0xFF, sizeof(command_GetConnNavEnableSetting_ConnNavState));
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "ALL_OFF") == 0) {
            memset(&command_GetConnNavEnableSetting_ConnNavState, 0x00, sizeof(command_GetConnNavEnableSetting_ConnNavState));
            ret = TRUE;
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "ON") == 0) {
            if (vCmdLine.size() >= 5) {
                if (TK_Tools::CompareNoCase(vCmdLine[4], "Local") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Local = 1;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "Gas") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Gas = 1;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "Parking") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Parking = 1;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "Weather") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Weather = 1;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "Traffic") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Traffic = 1;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "PushObject") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_PushObject = 1;
                }

                ret = TRUE;
            }
        } else if (TK_Tools::CompareNoCase(vCmdLine[2], "OFF") == 0) {
            if (vCmdLine.size() >= 5) {
                if (TK_Tools::CompareNoCase(vCmdLine[4], "Local") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Local = 0;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "Gas") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Gas = 0;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "Parking") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Parking = 0;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "Weather") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Weather = 0;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "Traffic") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_Traffic = 0;
                } else if (TK_Tools::CompareNoCase(vCmdLine[4], "PushObject") == 0) {
                    command_GetConnNavEnableSetting_ConnNavState.ConnNav_PushObject = 0;
                }

                ret = TRUE;
            }
        } else {
            // do nothing
        }
    }

    if (ret) {
        uint8_t data[1 + sizeof(struct_ConnNavState)];
        memset(&data, 0, sizeof(data));
        data[0] = NavSend_SendConnNavEnableSetting;
        
        struct_ConnNavState connNavState;
        memset(&connNavState, 0, sizeof(connNavState));
        connNavState.ConnNav_Local = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Local;
        connNavState.ConnNav_Gas = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Gas;
        connNavState.ConnNav_Parking = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Parking;
        connNavState.ConnNav_Weather = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Weather;
        connNavState.ConnNav_Traffic = command_GetConnNavEnableSetting_ConnNavState.ConnNav_Traffic;
        connNavState.ConnNav_PushObject= command_GetConnNavEnableSetting_ConnNavState.ConnNav_PushObject;

        memcpy(&data[1], &connNavState, sizeof(connNavState));

        SendUmsgToNaviHmi(entNaviStatusUpdate, (const uint8_t *)&data, sizeof(data), 0);
    }
    
    return ret;
}

//
// Update TpegModeStatus - <bOnOff>
//
static BOOL NS_TpegModeStatus(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    
    UINT_T nOnOff = 1;

    if (vCmdLine.size() >= 4 
        && TK_Tools::CompareNoCase(vCmdLine[0], "Update") == 0
        && TK_Tools::CompareNoCase(vCmdLine[1], "TpegModeStatus") == 0) {

        nOnOff = (UINT_T)TK_Tools::StrToUL(vCmdLine[3]);
        
        SendUmsgToNaviHmi(NavSend_SendTPEGMode, (const uint8_t *)&nOnOff, sizeof(nOnOff), 0);
        ret = TRUE;
    }
    
    return ret;
}

//
// SendAlertPoints - <cameraEnable> <schoolEnable> <railwayEnable>
//
static BOOL NS_SendAlertPoints(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    struct_AlertPointNotification alertPointNotification;
    memset(&alertPointNotification, 0, sizeof(struct_AlertPointNotification));
    alertPointNotification.camera = FALSE;
    alertPointNotification.schoolzone = FALSE;
    alertPointNotification.railwaycrossing = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SendAlertPoints") == 0) {
        if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[2], "1") == 0) {
            alertPointNotification.camera = TRUE;
        }

        if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[3], "1") == 0) {
            alertPointNotification.schoolzone = TRUE;
        }

        if (vCmdLine.size() >= 5 && TK_Tools::CompareNoCase(vCmdLine[4], "1") == 0) {
            alertPointNotification.railwaycrossing = TRUE;
        }
    }

    SendUmsgToNaviHmi(entNavSend_AlertPointStatus, (const uint8_t *)&alertPointNotification, sizeof(alertPointNotification), 0);
    ret = TRUE;
    
    return ret;
}

//
// ECallBCall - <isEcall> <on>
//
static BOOL NS_ECallBCall(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    BOOL bIsEcall = FALSE;
    BOOL bOn = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "ECallBCall") == 0) {
        if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[2], "1") == 0) {
            bIsEcall = TRUE;
        }

        if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[3], "1") == 0) {
            bOn = TRUE;
        }
    }

    {
        HWND hXTWnd = ::FindWindow(L"XT Window Common", NULL);
        if (hXTWnd != NULL) {
#define WIN_MSG_ID_HARD_KEY                     0x8000u
#define PARAM1_ECALL                            66
#define PARAM1_BCALL                            67
#define PARAM2_SINGLE_PRESS                     0
#define PARAM2_RELEASE                          4
            WPARAM wParam = bIsEcall ? PARAM1_ECALL : PARAM1_BCALL;
            LPARAM lParam = bOn ? PARAM2_SINGLE_PRESS : PARAM2_RELEASE;
            ::SendMessage(hXTWnd, WIN_MSG_ID_HARD_KEY, wParam, lParam);
#undef WIN_MSG_ID_HARD_KEY
#undef PARAM1_ECALL
#undef PARAM1_BCALL
#undef PARAM2_SINGLE_PRESS
#undef PARAM2_RELEASE
        }
    }

    ret = TRUE;
    
    return ret;
}

//
// SetGlobalNetworkSeachOn - <bOn>
//
static BOOL NS_SetGlobalNetworkSeachOn(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SetGlobalNetworkSeachOn") == 0) {
        if (vCmdLine.size() >= 3) {
            command_SetGlobalSearchType__bDefNetworkSearch = (UINT_T)TK_Tools::StrToUL(vCmdLine[2]);
            NAVSIN_MSG("%s(): command_SetGlobalSearchType__bDefNetworkSearch=%u\n", __FUNCTION__, command_SetGlobalSearchType__bDefNetworkSearch);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// SetNetworkSeachPhoneNo - <sPhoneNo>
//
static BOOL NS_SetNetworkSeachPhoneNo(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SetNetworkSeachPhoneNo") == 0) {
        if (vCmdLine.size() >= 3) {
            command_GetNetPOISearchElementDetailsData_sPhoneNo = TK_Tools::str2wstr(vCmdLine[2]);
            NAVSIN_MSG("%s(): command_SetGlobalSearchType__bDefNetworkSearch=%u\n", __FUNCTION__, vCmdLine[2]);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// SetGeoDestPlusStagesCount - <nCount>
//
static BOOL NS_SetGeoDestPlusStagesCount(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SetGeoDestPlusStagesCount") == 0) {
        if (vCmdLine.size() >= 3) {
            command_GetDestinationByGeo_nLocationArrayLen = TK_Tools::StrToUL(vCmdLine[2]);
            NAVSIN_MSG("%s(): command_GetDestinationByGeo_nLocationArrayLen=%u\n", __FUNCTION__, command_GetDestinationByGeo_nLocationArrayLen);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// SetDestPlusStagesCount - <nCount>
//
static BOOL NS_SetDestPlusStagesCount(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SetDestPlusStagesCount") == 0) {
        if (vCmdLine.size() >= 3) {
            command_GetStageListElements_nTotalDestAndStageCount = TK_Tools::StrToUL(vCmdLine[2]);
            NAVSIN_MSG("%s(): command_GetStageListElements_nTotalDestAndStageCount=%u\n", __FUNCTION__, command_GetStageListElements_nTotalDestAndStageCount);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// HardKey - <hkValue> <keyStatus>
//
static BOOL NS_HardKey(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    unsigned long nHardKey = -1;            // ref: static const uint8_t mapping_table[]
    unsigned long nKeyStatus = 0;           // ref: enum mieInputTrigger_KeyPushType

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "HardKey") == 0) {
        if (vCmdLine.size() >= 3) {
            nHardKey = TK_Tools::StrToUL(vCmdLine[2]);
        }

        if (vCmdLine.size() >= 4) {
            nKeyStatus = TK_Tools::StrToUL(vCmdLine[3]);
        }
    }

    {
        HWND hXTWnd = ::FindWindow(L"XT Window Common", NULL);
        if (hXTWnd != NULL) {
#define WIN_MSG_ID_HARD_KEY                     0x8000u
            WPARAM wParam = (WPARAM)nHardKey;
            LPARAM lParam = (LPARAM)nKeyStatus;
            ::SendMessage(hXTWnd, WIN_MSG_ID_HARD_KEY, wParam, lParam);
#undef WIN_MSG_ID_HARD_KEY
        }
    }

    ret = TRUE;
    
    return ret;
}

//
// HardKeyClick - <hkValue>
//
static BOOL NS_HardKeyClick(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    unsigned long nHardKey = -1;            // ref: static const uint8_t mapping_table[]

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "HardKeyClick") == 0) {
        if (vCmdLine.size() >= 2) {
            nHardKey = TK_Tools::StrToUL(vCmdLine[2]);
        }
    }

    {
        HWND hXTWnd = ::FindWindow(L"XT Window Common", NULL);
        if (hXTWnd != NULL) {
#define WIN_MSG_ID_HARD_KEY                     0x8000u
            WPARAM wParam = (WPARAM)nHardKey;
            LPARAM lParam = (LPARAM)0;
            ::SendMessage(hXTWnd, WIN_MSG_ID_HARD_KEY, wParam, lParam);

            wParam = (WPARAM)nHardKey;
            lParam = (LPARAM)4;
            ::SendMessage(hXTWnd, WIN_MSG_ID_HARD_KEY, wParam, lParam);
#undef WIN_MSG_ID_HARD_KEY
        }
    }

    ret = TRUE;
    
    return ret;
}


//
// NS_CreateLotsOfSockets - <nSocketCount>
//
static BOOL NS_CreateLotsOfSockets(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    UINT nSocketCount = 1;
    UINT i;
    
    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "CreateLotsOfSockets") == 0) {
        if (vCmdLine.size() >= 3) {
            nSocketCount = TK_Tools::StrToUL(vCmdLine[2]);
        }
    }

    if (nSocketCount == 0) {
        nSocketCount = 1;
    }

    for (i = 0; i < nSocketCount; i++) {
        TK_Tools::Socket *p = new TK_Tools::Socket();
        if (p != NULL) {
            if (!p->create()) {
                NAVSIN_MSG("%s(): [%lu] Failed to p->create()!\n", __FUNCTION__, i);
                break;
            }
        } else {
            NAVSIN_MSG("%s(): [%lu] Failed to new TK_Tools::Socket()!\n", __FUNCTION__, i);
        }
    }

    ret = TRUE;
    
    return ret;
}

//
// SendErrorData - <functionCode> <errorCode>
//
static BOOL NS_SendErrorData(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    //
    // REF:
    //     enum_Navi_Fun_Code functionCode
    //     enum_Navi_Error_Code errorCode
    //
    UINT8_T nFunctionCode = (UINT8_T)-1;
    UINT8_T nErrorCode = (UINT8_T)-1;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SendErrorData") == 0) {
        if (vCmdLine.size() >= 3) {
            nFunctionCode = (UINT8_T)TK_Tools::StrToUL(vCmdLine[2]);
        }

        if (vCmdLine.size() >= 4) {
            nErrorCode = (UINT8_T)TK_Tools::StrToUL(vCmdLine[3]);
        }
    }

    if (nFunctionCode != (UINT8_T)-1 && nErrorCode != (UINT8_T)-1)
    {
        uint8_t data[2];
        memset(&data, 0, sizeof(data));
        data[0] = nFunctionCode;
        data[1] = nErrorCode;
        SendUmsgToNaviHmi(entNavSend_SendErrorData, (const uint8_t *)&data, sizeof(data), 0);

        ret = TRUE;
    }
    
    return ret;
}

//
// PostHmiEvent - <id> <value> <data>
//
// 1) set language:                         2560    0       <language_id> # 0: Chinese; 1: English
// 2) set engineering mode:                 37      0       1
// 3) set engineering mode(internal):       38      0       1
// 4) start Ecall:                          54      0       0
// 5) end Ecall:                            54      0       4
// 6) start Bcall:                          65      0       0
// 7) end Bcall:                            65      0       4
static BOOL NS_PostHmiEvent(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    UINT16_T id;
    INT16_T value;
    UINT32_T data;

    if (vCmdLine.size() >= 5 && TK_Tools::CompareNoCase(vCmdLine[0], "PostHmiEvent") == 0) {
        id = (UINT16_T)TK_Tools::StrToUL(vCmdLine[2]);
        value = (INT16_T)TK_Tools::StrToL(vCmdLine[3]);
        data = (UINT32_T)TK_Tools::StrToUL(vCmdLine[4]);

        TK_Tools::entHmiBLAdaptor_postEvent(id, value, data);
        ret = TRUE;
    }
    
    return ret;
}

//
// NOTE:
// 1)  If the function NS_MyGenericGUI() is not in use:
//     For test, please call this function in:
//         void VXTCLSGENERICGUI_SPEC MyGenericGUI(uint16_t id, sint16_t value, uint32_t data);
//     The code:
//            extern void NS_MyGenericGUIExt(unsigned short id, signed short value, unsigned int data);
//            if (id == (unsigned short)-1) {
//                NS_MyGenericGUIExt(id, value, data);
//            }
//
// 2)  Some screens:
//         ENGMODE_MAIN_ENUM = 1024,
//         ENGMODE_INTERNAL_ENUM = 1027,
void NS_MyGenericGUIExt(unsigned short id, signed short value, unsigned int data)
{
    if (id != (UINT16_T)-1) {
        return;
    }

    switch (value) {
    case 1:
        NAVSIN_MSG("%s(): entHmiHMIManager_showScreen(%u)\n", __FUNCTION__, data);
        TK_Tools::entHmiHMIManager_showScreen(UINT16_T(data));
        break;
    case 2:
        NAVSIN_MSG("%s(): entHmiHMIManager_showAppScreen(%u)\n", __FUNCTION__, data);
        TK_Tools::entHmiHMIManager_showAppScreen(UINT8_T(data));
        break;
    case 3:
        NAVSIN_MSG("%s(): entHmiHMIManager_showLastScreen()\n", __FUNCTION__);
        TK_Tools::entHmiHMIManager_showLastScreen();
        break;
    case 4:
        NAVSIN_MSG("%s(): entHmiHMIManager_showLastScreenByID(%u)\n", __FUNCTION__, data);
        TK_Tools::entHmiHMIManager_showLastScreenByID(UINT16_T(data));
        break;
    case 5:
        NAVSIN_MSG("%s(): entHmiHMIManager_clearScreenStackExceptingCurrent()\n", __FUNCTION__);
        TK_Tools::entHmiHMIManager_clearScreenStackExceptingCurrent();
        break;

    default:
        break;
    }
}

static MyGenericGUI_FuncType s_OldMyGenericGUI = NULL;
static void NS_MyGenericGUI(unsigned short id, signed short value, unsigned int data)
{
    if (s_OldMyGenericGUI != NULL) {
        s_OldMyGenericGUI(id, value, data);
    }

    NS_MyGenericGUIExt(id, value, data);
}

//
// PostTestHmiEvent - <value> <modifiers>
//
static BOOL NS_PostTestHmiEvent(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (s_OldMyGenericGUI == NULL) {
        extern void MyGenericGUI(unsigned short id, signed short value, unsigned int data);
        s_OldMyGenericGUI = &MyGenericGUI;

        TK_Tools::entHmiHMIManager_registerGenericGUI(&NS_MyGenericGUI);
    }

    INT16_T value = (INT16_T)-1;
    UINT32_T modifiers = (UINT32_T)-1;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "PostTestHmiEvent") == 0) {
        if (vCmdLine.size() >= 3) {
            value = (short)TK_Tools::StrToL(vCmdLine[2]);
        }

        if (vCmdLine.size() >= 4) {
            modifiers = (unsigned int)TK_Tools::StrToUL(vCmdLine[3]);
        }
    }
    TK_Tools::entHmiBLAdaptor_postEvent((UINT16_T)-1, value, modifiers);

    ret = TRUE;
    
    return ret;
}

extern "C" extern int UserDefinedWinMsgHandler(uint32_t id, uint32_t param1, uint32_t param2);
#define WIN_MSG_ID_HARD_KEY             (0x8000u)
#define WIN_MSG_ID_APP_EVENT            (0x8001u)
#define WIN_MSG_ID_WATCH_DOG            (0x8002u)

//
// GenerateAppEvent - <nEventId> <value> <modifiers>
//    nEventId: any of vxtTypEvtID.
//
static BOOL NS_GenerateAppEvent(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    uint32_t param1;
    uint32_t param2;
    
    UINT16_T nEventId = 0;
    UINT16_T value = 0;
    UINT32_T modifiers = 0;

    if (vCmdLine.size() == 5 && TK_Tools::CompareNoCase(vCmdLine[0], "GenerateAppEvent") == 0) {
        nEventId = (UINT16_T)TK_Tools::StrToUL(vCmdLine[2]);
        value = (UINT16_T)TK_Tools::StrToUL(vCmdLine[3]);
        modifiers = (UINT32_T)TK_Tools::StrToUL(vCmdLine[4]);

        param1 = (nEventId << 16) | value;
        param2 = modifiers;
        UserDefinedWinMsgHandler(WIN_MSG_ID_APP_EVENT, param1, param2);

        ret = TRUE;
    }
    
    return ret;
}

//
// GenerateHardKeyEvent - <nKeyIndex> <nKeyState>
//     nKeyIndex: the index in the array 
//                    const uint8_t mapping_table[];
//                in entHmiLifecycle.cpp.
//     nKeyState: refer to:
//	                  enum mieInputTrigger_KeyPushType {
//	                      mieInputTrigger_KeyPushType_SinglePress = 0x00,
//	                      mieInputTrigger_KeyPushType_ShortPress,
//	                      mieInputTrigger_KeyPushType_LongPress,
//	                      mieInputTrigger_KeyPushType_ContinuePress,
//	                      mieInputTrigger_KeyPushType_Release,
//	                      mieInputTrigger_KeyPushType_EnumMax
//	                  };
// NOTE:
// 1) One short press sequence: SinglePress -> Release.
// 2) One long press sequence: SinglePress -> Release -> Release.
// 3) One short press sequence: ShortPress.
//
static BOOL NS_GenerateHardKeyEvent(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    uint32_t param1;
    uint32_t param2;
    
    UINT16_T nKeyIndex = 0;
    UINT32_T nKeyState = 0;

    if (vCmdLine.size() == 4 && TK_Tools::CompareNoCase(vCmdLine[0], "GenerateHardKeyEvent") == 0) {
        nKeyIndex = (UINT16_T)TK_Tools::StrToUL(vCmdLine[2]);
        nKeyState = (UINT32_T)TK_Tools::StrToUL(vCmdLine[3]);

        param1 = nKeyIndex;
        param2 = nKeyState;
        UserDefinedWinMsgHandler(WIN_MSG_ID_HARD_KEY, param1, param2);

        ret = TRUE;
    }
    
    return ret;
}

//
// SetHMITheme - <theme>
//
//    enum Theme
//    {
//        Common,
//        Blue,
//        Red,
//        White,
//        WhiteRed,
//        DF
//    };
static BOOL NS_SetHMITheme(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    UINT8_T theme = (UINT8_T)-1;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SetHMITheme") == 0) {
        if (vCmdLine.size() >= 3) {
            theme = (UINT8_T)TK_Tools::StrToUL(vCmdLine[2]);
            TK_Tools::hmiImageManager_setTheme(theme);
            NAVSIN_MSG("%s(): theme=%u\n", __FUNCTION__, theme);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// SetHMILanguage - <language>
//
static BOOL NS_SetHMILanguage(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    UINT8_T language = (UINT8_T)-1;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "SetHMILanguage") == 0) {
        if (vCmdLine.size() >= 3) {
            language = (UINT8_T)TK_Tools::StrToUL(vCmdLine[2]);
            TK_Tools::SetSystemLanguage(language);
            NAVSIN_MSG("%s(): language=%u\n", __FUNCTION__, language);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// ShowHMIScreen - <nScreenId>
//
static BOOL NS_ShowHMIScreen(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    UINT16_T nScreenId = (UINT16_T)-1;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "ShowHMIScreen") == 0) {
        if (vCmdLine.size() >= 3) {
            nScreenId = (UINT16_T)TK_Tools::StrToUL(vCmdLine[2]);
            TK_Tools::entHmiHMIManager_showScreen(nScreenId);
            NAVSIN_MSG("%s(): nScreenId=%u\n", __FUNCTION__, nScreenId);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// CloseHMIScreen - <nScreenId>
//
static BOOL NS_CloseHMIScreen(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    UINT16_T nScreenId = (UINT16_T)-1;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "CloseHMIScreen") == 0) {
        if (vCmdLine.size() >= 3) {
            nScreenId = (UINT16_T)TK_Tools::StrToUL(vCmdLine[2]);
            TK_Tools::entHmiHMIManager_closeScreen(nScreenId);
            NAVSIN_MSG("%s(): nScreenId=%u\n", __FUNCTION__, nScreenId);
            ret = TRUE;
        }
    }
    
    return ret;
}

//
// CloseAllHMIPopups
//
static BOOL NS_CloseAllHMIPopups(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "CloseAllHMIPopups") == 0) {
        TK_Tools::entHmiHMIManager_CloseAllPopups();
        NAVSIN_MSG("%s(): \n", __FUNCTION__);
        ret = TRUE;
    }
    
    return ret;
}

//
// ShowNowPlaying
//
static BOOL NS_ShowNowPlaying(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "ShowNowPlaying") == 0) {
        TK_Tools::entHmiHMIManager_showNowPlaying();
        NAVSIN_MSG("%s(): \n", __FUNCTION__);
        ret = TRUE;
    }
    
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// some ERGs for NS_ReadErgData() and NS_WriteErgData()
//

// <<< Car Brand >>>
//  #define ERG_DPCA_HMI_STARTUP_SCREEN						0x01001281 /* 1 byte */                                               
//  typedef enum tsysConfigBrand
//	{
//	    Default = 0,
//	    Peugeot,
//	    Citroen,
//	    DS,
//	    DF
//	}tsysConfigBrand; 

// <<< theme with car brand >>>
// #define ERG_DPCA_HMI_ACTIVE_THEME						0x01001293 /* 1 byte */
// typedef enum tsysConfigTheme
//	{
//		BRAND_TH1_RED_COLOR=0,      //Red
//		BRAND_TH2_BLUE_COLOR,       //Blue
//	    COMMON_ORGE_COLOR,          //Orange
//	    BRAND_TH2_WHITE_COLOR,      //White
//	    BRAND_TH2_DF_COLOR			// for Dongfeng
//	}tsysConfigTheme;

// <<< use the theme with car brand >>>
// #define ERG_DPCA_HMI_CURRENT_USER_THEME						0x01001297 /* 1 byte */  

// <<< Car Line >>>
// #define ERG_DPCA2_VF_VHL_PICTO							0x010012b4 /* 1 byte */ 
//	const uint8_t VHL_PICTO_E3 = 0x00;
//	const uint8_t VHL_PICTO_A94 = 0x01;
//	const uint8_t VHL_PICTO_B78 = 0x30;
//	const uint8_t VHL_PICTO_T9 = 0x31;
//	const uint8_t VHL_PICTO_M4 = 0x02;
//	const uint8_t VHL_PICTO_M33 = 0x03;
//	const uint8_t VHL_PICTO_B73 = 0x04;
//	const uint8_t VHL_PICTO_W23 = 0x06;
//	const uint8_t VHL_PICTO_B81 = 0x07;
//	const uint8_t VHL_PICTO_B75 = 0x60;
//	const uint8_t VHL_PICTO_X7 = 0x05;
//	const uint8_t VHL_PICTO_T8 = 0X32;
//	const uint8_t VHL_PICTO_BZ3 = 0x08;
//	const uint8_t VHL_PICTO_G9 = 0x61;

// <<< City Park >>>
//	#define ERG_DPCA2_VF_CITYPARK_BTN						0x010012a7 /* 1 byte */       

// <<< Language >>>
// #define ERG_DPCA_SYS_CONFIG_LANGUAGE						0x8400801f /* 1 byte */    
//	enum EnumValues
//	{
//	  CH_ENUM = 2,
//	  EN_ENUM = 0,
//	  FR_ENUM = 1
//	};

//
// ReadErgData - <nRegId>
//
static BOOL NS_ReadErgData(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    uint32_t nRegId = (UINT32_T)-1;
    uint32_t nDataSize = 0;
    uint32_t nDataSizeRet = 0;
    uint8_t *pDataBuf = NULL;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "ReadErgData") == 0) {
        if (vCmdLine.size() >= 3) {
            nRegId = (uint32_t)TK_Tools::StrToUL(vCmdLine[2]);

            if (ssw_ergUserGetDataSize(ERG_USER_SYSTEM, nRegId, &nDataSize) == 0) {
                pDataBuf = new uint8_t[nDataSize];
                if (pDataBuf != NULL) {
                    if (ssw_ergUserReadData(ERG_USER_SYSTEM, nRegId, pDataBuf, nDataSize, &nDataSizeRet) == 0) {
                        NAVSIN_MSG("%s(): [0x%08X] Read ERG value (hex):", __FUNCTION__, nRegId);
                        for (unsigned int i = 0; i < nDataSizeRet; i++) {
                            NAVSIN_MSG(" %02X", pDataBuf[i]);
                        }
                        NAVSIN_MSG("\n");
                    } else {
                        NAVSIN_MSG("*%s(): ssw_ergUserReadData() is failed!\n", __FUNCTION__);
                    }
                } else {
                    NAVSIN_MSG("*%s(): new is failed!\n", __FUNCTION__);
                }
            } else {
                NAVSIN_MSG("*%s(): ssw_ergUserGetDataSize() is failed!\n", __FUNCTION__);
            }
            
            ret = TRUE;
        }
    }

    if (pDataBuf != NULL) {
        delete [] pDataBuf;
    }
    
    return ret;
}

//
// WriteErgData - <nRegId> <nValueByte1> ...
//
static BOOL NS_WriteErgData(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    uint32_t nRegId = (uint32_t)-1;
    uint32_t nDataSize = 0;
    uint8_t *pDataBuf = NULL;

    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "WriteErgData") == 0) {
        if (vCmdLine.size() >= 4) {
            nRegId = (uint32_t)TK_Tools::StrToUL(vCmdLine[2]);

            nDataSize = vCmdLine.size() - 3;
            pDataBuf = new uint8_t[nDataSize];
            if (pDataBuf != NULL) {
                for (unsigned int i = 0; i < nDataSize; i++) {
                    pDataBuf[i] = (uint8_t)TK_Tools::StrToUL(vCmdLine[i + 3]);
                }

                if (ssw_ergUserWriteData(ERG_USER_SYSTEM, nRegId, pDataBuf, nDataSize) == 0) {
                    NAVSIN_MSG("%s(): [0x%08X] Write ERG value (hex):", __FUNCTION__, nRegId);
                    for (unsigned int i = 0; i < nDataSize; i++) {
                        NAVSIN_MSG(" %02X", pDataBuf[i]);
                    }
                    NAVSIN_MSG("\n");
                } else {
                    NAVSIN_MSG("*%s(): ssw_ergUserWriteData() is failed!\n", __FUNCTION__);
                }
            } else {
                NAVSIN_MSG("*%s(): new is failed!\n", __FUNCTION__);
            }
            
            ret = TRUE;
        }
    }

    if (pDataBuf != NULL) {
        delete [] pDataBuf;
    }
    
    return ret;
}

//
// ShowWindow - <hWinHex> <nShow>
//
//     nShow: SW_HIDE, SW_SHOW, SW_SHOWNA, SW_SHOWNORMAL
static BOOL NS_ShowWindow(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    HWND hWnd = NULL;
    int nShow = 1;

    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "ShowWindow") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        nShow = (int)TK_Tools::StrToL(vCmdLine[3]);

        BOOL bPrevVisible = ::ShowWindow(hWnd, nShow);
        NAVSIN_MSG("%s(): bPrevVisible=%d, nShow=%d\n", __FUNCTION__, bPrevVisible, nShow);

        ret = TRUE;
    }
    
    return ret;
}

//
// SaveWindowToBmpFile - <BmpFilePath>
//
static BOOL NS_SaveUsbVideoToBmpFile(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    BOOL bSuc;

    std::string strBmpFilePath;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "SaveUsbVideoToBmpFile") == 0) {
        strBmpFilePath = vCmdLine[2];

        TK_Tools::UsbVideoWindowCapture uvwc;
        bSuc = uvwc.DoCapture();
        if (bSuc) {
            bSuc = TK_Tools::SaveBmpToFile_RGBA32(
                strBmpFilePath.c_str(), 
                uvwc.GetBits(), 
                uvwc.GetWidth(), 
                uvwc.GetHeight(), 
                uvwc.GetPitch(), 
                TRUE, 
                "bgra"
           );
           if (bSuc) {
                NAVSIN_MSG("%s(): strBmpFilePath=%s\n", __FUNCTION__, strBmpFilePath.c_str());
           } else {
                NAVSIN_MSG("%s(): TK_Tools::SaveBmpToFile_RGBA32() is failed!\n", __FUNCTION__);
           }
        } else {
            NAVSIN_MSG("%s(): uvwc.DoCapture(hWnd) is failed!\n", __FUNCTION__);
        }
        
        ret = TRUE;
    }
    
    return ret;
}

//
// SaveWindowToBmpFile - <hWnd> <BmpFilePath>
//
static BOOL NS_SaveWindowToBmpFile(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    BOOL bSuc;

    HWND hWnd = NULL;
    std::string strBmpFilePath;

    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "SaveWindowToBmpFile") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        strBmpFilePath = vCmdLine[3];

        TK_Tools::WindowBitsCapture wbc;
        bSuc = wbc.DoCapture(hWnd);
        if (bSuc) {
            ::BringWindowToTop(hWnd);
            ::SetForegroundWindow(hWnd);
            ::ShowWindow(hWnd, 1);
            ::RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
            Sleep(2000);
            bSuc = TK_Tools::SaveBmpToFile_RGBA32(
                strBmpFilePath.c_str(), 
                wbc.GetBits(), 
                wbc.GetWidth(), 
                wbc.GetHeight(), 
                wbc.GetPitch(), 
                TRUE, 
                "bgra"
           );
           if (bSuc) {
                NAVSIN_MSG("%s(): hWnd=0x%08X, strBmpFilePath=%s\n", __FUNCTION__, hWnd, strBmpFilePath.c_str());
           } else {
                NAVSIN_MSG("%s(): TK_Tools::SaveBmpToFile_RGBA32() is failed!\n", __FUNCTION__);
           }
        } else {
            NAVSIN_MSG("%s(): wbc.DoCapture(hWnd) is failed!\n", __FUNCTION__);
        }
        
        ret = TRUE;
    }
    
    return ret;
}

//
// SaveLayerToBmpFile - <nLayerID> <bBgSurface> <BmpFilePath>
//
//        typedef enum 
//        {
//            LayerIDHmi = 0,
//            LayerIDAlpha,
//            LayerIDMapView1,
//            LayerIDBrowser,
//            LayerIDVideo1,
//            LayerIDVideo2,
//            LayerIDSplashscreen,
//            LayerIDCustom1,
//            LayerIDCustom2,
//            LayerIDCustom3,
//            LayerIDNum
//        } ILayerManager_LayerEnum;
static BOOL NS_SaveLayerToBmpFile(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    BOOL bSuc;

    unsigned long nLayerId = 0;
    BOOL bBgSurface = TRUE;
    std::string strBmpFilePath;

    if (vCmdLine.size() >= 5 && TK_Tools::CompareNoCase(vCmdLine[0], "SaveLayerToBmpFile") == 0) {
        nLayerId = TK_Tools::StrToUL(vCmdLine[2]);
        bBgSurface = (BOOL)TK_Tools::StrToUL(vCmdLine[3]);
        strBmpFilePath = vCmdLine[4];

        TK_Tools::LayerBitsCapture lbc;
        bSuc = lbc.DoCapture(nLayerId, bBgSurface);
        if (bSuc) {
            Sleep(2000);
            bSuc = TK_Tools::SaveBmpToFile_RGBA32(
                strBmpFilePath.c_str(), 
                lbc.GetBits(), 
                lbc.GetWidth(), 
                lbc.GetHeight(), 
                lbc.GetPitch(), 
                TRUE, 
                "bgra"
           );
           if (bSuc) {
                NAVSIN_MSG("%s(): nLayerId=%u, bBgSurface=%u, strBmpFilePath=%s\n", __FUNCTION__, nLayerId, bBgSurface, strBmpFilePath.c_str());
           } else {
                NAVSIN_MSG("%s(): TK_Tools::SaveBmpToFile_RGBA32() is failed!\n", __FUNCTION__);
           }
        } else {
            NAVSIN_MSG("%s(): lbc.DoCapture(hWnd) is failed!\n", __FUNCTION__);
        }
        
        ret = TRUE;
    }
    
    return ret;
}

//
// EnumWindows
//
static BOOL NS_EnumWindows(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    
    HWND hWndForSearch = NULL;
    std::string strWindowsInfo;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "EnumWindows") == 0) {
        if (vCmdLine.size() >= 3) {
            hWndForSearch = (HWND)TK_Tools::StrToUL(vCmdLine[2]);
            TK_Tools::PrintWindowsInfo(hWndForSearch);
        } else {
            TK_Tools::PrintAllWindowsInfo();
        }

        ret = TRUE;
    }
    
    return ret;
}

//
// kp - <ExeName>
//
static BOOL NS_KillProce(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    BOOL bSuc;
    
    HWND hWndForSearch = NULL;
    std::string strWindowsInfo;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "kp") == 0) {
        std::string sExeName(vCmdLine[2]);
        if (sExeName == "-1") {
            sExeName = "TestLoadExternalSymbal_debugee.exe";
            bSuc = TK_Tools::KillProcessByName(TK_Tools::str2tstr(sExeName));
            if (bSuc) {
                NAVSIN_MSG("%s(): process \'%s\' is killed!\n", __FUNCTION__, sExeName.c_str());
            } else {
                NAVSIN_MSG("*** %s(): process \'%s\' is not killed!\n", __FUNCTION__, sExeName.c_str());
            }

            sExeName = "TestLoadExternalSymbal.exe";
            bSuc = TK_Tools::KillProcessByName(TK_Tools::str2tstr(sExeName));
            if (bSuc) {
                NAVSIN_MSG("%s(): process \'%s\' is killed!\n", __FUNCTION__, sExeName.c_str());
            } else {
                NAVSIN_MSG("*** %s(): process \'%s\' is not killed!\n", __FUNCTION__, sExeName.c_str());
            }
        } else {
            bSuc = TK_Tools::KillProcessByName(TK_Tools::str2tstr(sExeName));
            if (bSuc) {
                NAVSIN_MSG("%s(): process \'%s\' is killed!\n", __FUNCTION__, sExeName.c_str());
            } else {
                NAVSIN_MSG("*** %s(): process \'%s\' is not killed!\n", __FUNCTION__, sExeName.c_str());
            }
        }

        ret = TRUE;
    }
    
    return ret;
}

//
// exec - <ExeName> <Arg1> <Arg2> ...
//
static BOOL NS_Execute(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;
    unsigned int i;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "exec") == 0) {
        std::string sExePath(vCmdLine[2]);
        std::string sArgs;
        for (i = 3; i < vCmdLine.size(); i++) {
            if (!sArgs.empty()) {
                sArgs += " ";
                sArgs += vCmdLine[i];
            } else {
                sArgs = vCmdLine[i];
            }
        }
        std::string sCmdLine = TK_Tools::FormatStr("%s %s", sExePath.c_str(), sArgs.c_str());
        NAVSIN_MSG("%s(): sCmdLine: %s\n", __FUNCTION__, sCmdLine.c_str());
        {
            if (!TK_Tools::Execute(sExePath, sArgs, false)) {
                NAVSIN_MSG("%s(): TK_Tools::Execute() is failed!\n", __FUNCTION__);
            }
        }

        ret = TRUE;
    }
    
    return ret;
}

//
// FillWindow - <hWinHex> <r> <g> <b> <a>
// FillWindowRect - <hWinHex> <r> <g> <b> <a> <x> <y> <w> <h>
//
static BOOL NS_FillWindow(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    HWND hWnd = NULL;
    unsigned char r, g, b, a;
    long x, y;
    unsigned long w, h;

    if (vCmdLine.size() >= 7 && TK_Tools::CompareNoCase(vCmdLine[0], "FillWindow") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        r = (unsigned char)TK_Tools::StrToUL(vCmdLine[3]);
        g = (unsigned char)TK_Tools::StrToUL(vCmdLine[4]);
        b = (unsigned char)TK_Tools::StrToUL(vCmdLine[5]);
        a = (unsigned char)TK_Tools::StrToUL(vCmdLine[6]);

        TK_Tools::FillWindow(hWnd, r, g, b, a);

        ret = TRUE;
    } else if (vCmdLine.size() >= 11 && TK_Tools::CompareNoCase(vCmdLine[0], "FillWindowRect") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        r = (unsigned char)TK_Tools::StrToUL(vCmdLine[3]);
        g = (unsigned char)TK_Tools::StrToUL(vCmdLine[4]);
        b = (unsigned char)TK_Tools::StrToUL(vCmdLine[5]);
        a = (unsigned char)TK_Tools::StrToUL(vCmdLine[6]);
        x = (long)TK_Tools::StrToL(vCmdLine[7]);
        y = (long)TK_Tools::StrToL(vCmdLine[8]);
        w = (unsigned long)TK_Tools::StrToUL(vCmdLine[9]);
        h = (unsigned long)TK_Tools::StrToUL(vCmdLine[10]);

        TK_Tools::FillWindow(hWnd, r, g, b, a, x, y, w, h);

        ret = TRUE;
    }
    
    return ret;
}

//
// DrawBmpToWindow - <hWinHex> <sBmpFilePath>
// DrawBmpToWindow - <hWinHex> <sBmpFilePath> <wx> <wy> <ww> <wh>
// DrawBmpToWindow - <hWinHex> <sBmpFilePath> <bx> <by> <bw> <bh> <wx> <wy>                     # no scaling
// DrawBmpToWindow - <hWinHex> <sBmpFilePath> <bx> <by> <bw> <bh> <wx> <wy> <ww> <wh>
//
static BOOL NS_DrawBmpToWindow(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    HWND hWnd = NULL;
    std::string sBmpFilePath;
    int bx;
    int by;
    unsigned int bw;
    unsigned int bh;
    int wx;
    int wy;
    unsigned int ww;
    unsigned int wh;

    if (vCmdLine.size() == 4 && TK_Tools::CompareNoCase(vCmdLine[0], "DrawBmpToWindow") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        sBmpFilePath = vCmdLine[3];

        if (!TK_Tools::FileExists(sBmpFilePath)) {
            NAVSIN_MSG("%s(): The file can't be read: %s\n", __FUNCTION__, sBmpFilePath.c_str());
        } else {
            TK_Tools::DrawBmpToWindow(hWnd, sBmpFilePath, 0, 0, TK_MAX_UINT, TK_MAX_UINT, 0, 0, TK_MAX_UINT, TK_MAX_UINT);
        }

        ret = TRUE;
    } else if (vCmdLine.size() == 8 && TK_Tools::CompareNoCase(vCmdLine[0], "DrawBmpToWindow") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        sBmpFilePath = vCmdLine[3];

        wx = (int)TK_Tools::StrToL(vCmdLine[4]);
        wy = (int)TK_Tools::StrToL(vCmdLine[5]);
        ww = (unsigned int)TK_Tools::StrToL(vCmdLine[6]);
        wh = (unsigned int)TK_Tools::StrToL(vCmdLine[7]);

        if (!TK_Tools::FileExists(sBmpFilePath)) {
            NAVSIN_MSG("%s(): The file can't be read: %s\n", __FUNCTION__, sBmpFilePath.c_str());
        } else {
            TK_Tools::DrawBmpToWindow(hWnd, sBmpFilePath, 0, 0, TK_MAX_UINT, TK_MAX_UINT, wx, wy, ww, wh);
        }

        ret = TRUE;
    } else if (vCmdLine.size() == 10 && TK_Tools::CompareNoCase(vCmdLine[0], "DrawBmpToWindow") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        sBmpFilePath = vCmdLine[3];
        
        bx = (int)TK_Tools::StrToL(vCmdLine[4]);
        by = (int)TK_Tools::StrToL(vCmdLine[5]);
        bw = (unsigned int)TK_Tools::StrToL(vCmdLine[6]);
        bh = (unsigned int)TK_Tools::StrToL(vCmdLine[7]);

        wx = (int)TK_Tools::StrToL(vCmdLine[8]);
        wy = (int)TK_Tools::StrToL(vCmdLine[9]);

        if (!TK_Tools::FileExists(sBmpFilePath)) {
            NAVSIN_MSG("%s(): The file can't be read: %s\n", __FUNCTION__, sBmpFilePath.c_str());
        } else {
            TK_Tools::DrawBmpToWindow(hWnd, sBmpFilePath, bx, by, bw, bh, wx, wy, TK_MAX_UINT - 1, TK_MAX_UINT - 1);
        }

        ret = TRUE;
    } else if (vCmdLine.size() == 12 && TK_Tools::CompareNoCase(vCmdLine[0], "DrawBmpToWindow") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        sBmpFilePath = vCmdLine[3];
        
        bx = (int)TK_Tools::StrToL(vCmdLine[4]);
        by = (int)TK_Tools::StrToL(vCmdLine[5]);
        bw = (unsigned int)TK_Tools::StrToL(vCmdLine[6]);
        bh = (unsigned int)TK_Tools::StrToL(vCmdLine[7]);

        wx = (int)TK_Tools::StrToL(vCmdLine[8]);
        wy = (int)TK_Tools::StrToL(vCmdLine[9]);
        ww = (unsigned int)TK_Tools::StrToL(vCmdLine[10]);
        wh = (unsigned int)TK_Tools::StrToL(vCmdLine[11]);

        if (!TK_Tools::FileExists(sBmpFilePath)) {
            NAVSIN_MSG("%s(): The file can't be read: %s\n", __FUNCTION__, sBmpFilePath.c_str());
        } else {
            TK_Tools::DrawBmpToWindow(hWnd, sBmpFilePath, bx, by, bw, bh, wx, wy, ww, wh);
        }

        ret = TRUE;
    }
    
    return ret;
}

//
// MoveWindow - <hWinHex> <x> <y> <w> <h>
//
static BOOL NS_MoveWindow(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    HWND hWnd = NULL;
    int nX;
    int nY;
    int nWidth;
    int nHeight;

    if (vCmdLine.size() >= 7 && TK_Tools::CompareNoCase(vCmdLine[0], "MoveWindow") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        nX = (int)TK_Tools::StrToL(vCmdLine[3]);
        nY = (int)TK_Tools::StrToL(vCmdLine[4]);
        nWidth = (int)TK_Tools::StrToL(vCmdLine[5]);
        nHeight = (int)TK_Tools::StrToL(vCmdLine[6]);

        TK_Tools::MoveWindow(hWnd, nX, nY, nWidth, nHeight);

        ret = TRUE;
    }
    
    return ret;
}

//
// RedrawWindow - <hWinHex>
//
static BOOL NS_RedrawWindow(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    HWND hWnd = NULL;

    if (vCmdLine.size() >= 3 && TK_Tools::CompareNoCase(vCmdLine[0], "RedrawWindow") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);

        TK_Tools::RedrawWindow(hWnd);

        ret = TRUE;
    }
    
    return ret;
}

//
// SetWindowZOrder - <hWinHex> <zorder>
//
static BOOL NS_SetWindowZOrder(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    HWND hWnd = NULL;
    unsigned long nZorder = (unsigned long)-1;

    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "SetWindowZOrder") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        nZorder = (unsigned long)TK_Tools::StrToUL(vCmdLine[3]);

        TK_Tools::SetWindowZOrder(hWnd, (TK_Tools::ZOrder)nZorder);

        ret = TRUE;
    }
    
    return ret;
}

//
// SetWindowZOrderAfter - <hWinHex> <hWndRefHex>
//
static BOOL NS_SetWindowZOrderAfter(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    HWND hWnd = NULL;
    HWND hWndRef = NULL;

    if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[0], "SetWindowZOrderAfter") == 0) {
        hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[2], 16);
        hWndRef = (HWND)TK_Tools::StrToUL(vCmdLine[3], 16);

        TK_Tools::SetWindowZOrderAfter(hWnd, hWndRef);

        ret = TRUE;
    }
    
    return ret;
}

//	    { "DeskBgWnd Create",                   &NS_DeskBgWnd,                          "DeskBgWnd Create" },
//	    { "DeskBgWnd Destroy",                  &NS_DeskBgWnd,                          "DeskBgWnd Destroy" },
//	    { "DeskBgWnd SetBgColor - <n> <n> <n> <n>", &NS_DeskBgWnd,                      "DeskBgWnd SetBgColor - <r> <g> <b> <a>" },
//	    { "DeskBgWnd SetRect - <n> <n> <n> <n>",&NS_DeskBgWnd,                          "DeskBgWnd SetRect - <x> <y> <w> <h>" },
//	    { "DeskBgWnd SetZorder - <n>",          &NS_DeskBgWnd,                          "DeskBgWnd SetZorder - <zorder>" },
//      { "DeskBgWnd SetZorderAfter - <n>",     &NS_DeskBgWnd,                          "DeskBgWnd SetZorderAfter - <hWnd>" },
//	    { "DeskBgWnd Redraw",                   &NS_DeskBgWnd,                          "DeskBgWnd Redraw" },
//	    { "DeskBgWnd SetVisible - <n>",         &NS_DeskBgWnd,                          "DeskBgWnd SetVisible - <bVisible>" },

//	enum ZOrder {
//	    ZO_BOTTOM = 0,
//	    ZO_TOP,
//	    ZO_NOTOPMOST,
//	    ZO_TOPMOST,
//	};

static BOOL NS_DeskBgWnd(std::vector<std::string> &vCmdLine)
{    
    BOOL ret = FALSE;

    if (vCmdLine.size() >= 1 && TK_Tools::CompareNoCase(vCmdLine[0], "DeskBgWnd") == 0) {
        TK_Tools::DeskBgWnd &rDeskBgWnd = TK_Tools::DeskBgWnd::GetInstance();
        
        if (vCmdLine.size() >= 2 && TK_Tools::CompareNoCase(vCmdLine[1], "Create") == 0) {
            rDeskBgWnd.BeginWorkThread();
        } else if (vCmdLine.size() >= 2 && TK_Tools::CompareNoCase(vCmdLine[1], "Destroy") == 0) {
            rDeskBgWnd.EndWorkThread();
        } else if (vCmdLine.size() >= 7 && TK_Tools::CompareNoCase(vCmdLine[1], "SetBgColor") == 0) {
            TK_Tools::Color32 color;
            
            color.r = (unsigned char)TK_Tools::StrToUL(vCmdLine[3]);
            color.g = (unsigned char)TK_Tools::StrToUL(vCmdLine[4]);
            color.b = (unsigned char)TK_Tools::StrToUL(vCmdLine[5]);
            color.a = (unsigned char)TK_Tools::StrToUL(vCmdLine[6]);
            
            rDeskBgWnd.SetBgColor(color);
        } else if (vCmdLine.size() >= 7 && TK_Tools::CompareNoCase(vCmdLine[1], "SetRect") == 0) {
            LONG x, y;
            ULONG w, h;
            RECT rect;
            
            x = (LONG)TK_Tools::StrToL(vCmdLine[3]);
            y = (LONG)TK_Tools::StrToL(vCmdLine[4]);
            w = (ULONG)TK_Tools::StrToUL(vCmdLine[5]);
            h = (ULONG)TK_Tools::StrToUL(vCmdLine[6]);
            rect.left = x;
            rect.right = x + w;
            rect.top = y;
            rect.bottom = y + h;
            
            rDeskBgWnd.SetWindowRect(rect);
        } else if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[1], "SetZorder") == 0) {
            TK_Tools::ZOrder zorder;

            zorder = (TK_Tools::ZOrder)TK_Tools::StrToUL(vCmdLine[3]);
            
            rDeskBgWnd.SetZOrder(zorder);
        } else if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[1], "SetZorderAfter") == 0) {
            HWND hWnd = NULL;

            hWnd = (HWND)TK_Tools::StrToUL(vCmdLine[3], 16);
            
            rDeskBgWnd.SetZOrder(hWnd);
        } else if (vCmdLine.size() >= 2 && TK_Tools::CompareNoCase(vCmdLine[1], "Redraw") == 0) {
            rDeskBgWnd.RedrawWindow();
        } else if (vCmdLine.size() >= 4 && TK_Tools::CompareNoCase(vCmdLine[1], "SetVisible") == 0) {
            BOOL bVisible;

            bVisible = (BOOL)TK_Tools::StrToUL(vCmdLine[3]);
            
            rDeskBgWnd.SetVisible(bVisible);
        }

        ret = TRUE;
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
    { "SYS exit",                           &NS_Sys,                                "SYS exit: exit process" },
    { "SYS Test - <s>",                     &NS_Sys,                                "SYS Test - [DataAbort|CppException]" },
    
    { "PP",                                 &NS_TriggerPushObject,                  "TriggerPushObject: 0" },
    { "PG",                                 &NS_TriggerPushObject,                  "TriggerPushObject: 1" },

    { "Update TrafficBargraph - <n>",       &NS_UpdateTrafficBargraph,              "Update Traffic Bargraph Info: <nSegmentCount>" },
    { "Update JourneyInfo - <n>",           &NS_UpdateJourneyInfo,                  "Update Journey Info: <nDistanceFromCcpToDest>" },
    
    { "Update NaviEnableSetting ALL_ON",    &NS_UpdateNaviEnableSetting,            "Update NaviEnableSetting ALL_ON" },
    { "Update NaviEnableSetting ALL_OFF",   &NS_UpdateNaviEnableSetting,            "Update NaviEnableSetting ALL_OFF" },
    { "Update NaviEnableSetting ON - <s>",  &NS_UpdateNaviEnableSetting,            "Update NaviEnableSetting ON Local|Gas|Parking|Weather|Traffic|PushObject" },
    { "Update NaviEnableSetting OFF - <s>", &NS_UpdateNaviEnableSetting,            "Update NaviEnableSetting OFF Local|Gas|Parking|Weather|Traffic|PushObject" },
    { "Update TpegModeStatus - <n>",        &NS_TpegModeStatus,                     "Update TpegModeStatus - <bOnOff>" },

    { "Update CcpWeatherType - <n>",        &NS_UpdateCcpWeatherType,               "Update CcpWeatherType - <nCcpWeatherType>" },
    { "Update WeatherElementsCount - <n>",  &NS_UpdateWeatherElementsCount,         "Update WeatherElementsCount - <nCount>" },

    { "SendAlertPoints - <b> <b> <b>",      &NS_SendAlertPoints,                    "SendAlertPoints - <cameraEnable> <schoolEnable> <railwayEnable>" },
    { "ECallBCall - <b> <b>",               &NS_ECallBCall,                         "ECallBCall - <isEcall> <on>" },
    { "HardKey - <n> <n>",                  &NS_HardKey,                            "HardKey - <hkValue> <keyStatus>" },
    { "HardKeyClick - <n>",                 &NS_HardKeyClick,                       "HardKeyClick - <hkValue>" },

    { "CreateLotsOfSockets - <n>",          &NS_CreateLotsOfSockets,                "NS_CreateLotsOfSockets - <nSocketCount>" },

    { "SendErrorData - <n> <n>",            &NS_SendErrorData,                      "SendErrorData - <functionCode> <errorCode>" },

    { "PostHmiEvent - <n> <n> <n>",         &NS_PostHmiEvent,                       "PostHmiEvent - <id> <value> <data>" },
    { "PostTestHmiEvent - <n> <n>",         &NS_PostTestHmiEvent,                   "PostTestHmiEvent - <value> <modifiers>" },
    { "GenerateAppEvent - <n> <n> <n>",     &NS_GenerateAppEvent,                   "GenerateAppEvent - <nEventId> <value> <modifiers>" },
    { "GenerateHardKeyEvent - <n> <n>",     &NS_GenerateHardKeyEvent,               "GenerateHardKeyEvent - <nKeyIndex> <nKeyState>" },
    { "SetHMITheme - <n>",                  &NS_SetHMITheme,                        "SetHMITheme - <theme>" },
    { "SetHMILanguage - <n>",               &NS_SetHMILanguage,                     "SetHMILanguage - <language>" },
    { "ShowHMIScreen - <n>",                &NS_ShowHMIScreen,                      "ShowHMIScreen - <nScreenId>" },
    { "CloseHMIScreen - <n>",               &NS_CloseHMIScreen,                     "CloseHMIScreen - <nScreenId>" },
    { "CloseAllHMIPopups",                  &NS_CloseAllHMIPopups,                  "CloseAllHMIPopups" },
    { "ShowNowPlaying",                     &NS_ShowNowPlaying,                     "ShowNowPlaying" },

    { "ReadErgData - <n>",                  &NS_ReadErgData,                        "ReadErgData - <nRegId>" },
    { "WriteErgData - <n> <n> ...",         &NS_WriteErgData,                       "WriteErgData - <nRegId> <nValueByte1> ..." },

    { "SetNetworkSearchResultType - <n>",   &NS_SetNetworkSearchResultType,         "SetNetworkSearchResultType - <nResultType>" },
    { "NetworkSearchDelayedDataIsAvailable",&NS_NetworkSearchDelayedDataIsAvailable,"NetworkSearchDelayedDataIsAvailable" },
    { "SetGlobalNetworkSeachOn - <b>",      &NS_SetGlobalNetworkSeachOn,            "SetGlobalNetworkSeachOn - <bOn>" },
    { "SetNetworkSeachPhoneNo - <s>",       &NS_SetNetworkSeachPhoneNo,             "SetNetworkSeachPhoneNo - <sPhoneNo>" },
    { "SetGeoDestPlusStagesCount - <n>",    &NS_SetGeoDestPlusStagesCount,          "SetGeoDestPlusStagesCount - <nCount>" },
    { "SetDestPlusStagesCount - <n>",       &NS_SetDestPlusStagesCount,             "SetDestPlusStagesCount - <nCount>" },

    { "SetTpegMsgListSize - <n>",           &NS_SetTpegMsgListSize,                 "SetSetTpegMsgListSize - <nSize>" },
    { "UpdateSpeedLimit - <n> <n>",         &NS_UpdateSpeedLimit,                   "UpdateSpeedLimit - <nSpeedLimit> <bOverSpeed>" },
    { "SendNaviDatabaseStatus - <n>",       &NS_SendNaviDatabaseStatus,             "SendNaviDatabaseStatus - <nDBStatus>" },
    { "WriteScreenGrpID - <n>",             &NS_WriteScreenGrpID,                   "WriteScreenGrpID - <nScreenGrpID>" },

    { "DictaphoneOp - <n>",                 &NS_DictaphoneOp,                       "DictaphoneOp - <nOperation>" },
    { "TriggerBleep - <n>",                 &NS_TriggerBleep,                       "TriggerBleep - <nBleepType>" },

    { "ShowWindow - <n> <n>",               &NS_ShowWindow,                         "ShowWindow - <hWinHex> <nShow>" },
    { "SaveWindowToBmpFile - <n> <s>",      &NS_SaveWindowToBmpFile,                "SaveWindowToBmpFile - <hWnd> <BmpFilePath>" },
    { "SaveUsbVideoToBmpFile - <s>",        &NS_SaveUsbVideoToBmpFile,              "SaveWindowToBmpFile - <BmpFilePath>" },
    { "SaveLayerToBmpFile - <n> <n> <s>",   &NS_SaveLayerToBmpFile,                 "SaveLayerToBmpFile - <nLayerID> <bBgSurface> <BmpFilePath>" },
    { "EnumWindows",                        &NS_EnumWindows,                        "EnumWindows" },
    { "EnumWindows - <n>",                  &NS_EnumWindows,                        "EnumWindows - <hWinHex>" },
    { "FillWindow - <n> <n> <n> <n> <n>",   &NS_FillWindow,                         "FillWindow - <hWinHex> <r> <g> <b> <a>" },
    { "FillWindowRect - <n> <n> <n> <n> <n> <n> <n> <n> <n>",   &NS_FillWindow,     "FillWindowRect - <hWinHex> <r> <g> <b> <a> <x> <y> <w> <h>" },
    { "DrawBmpToWindow - <n> <s>",          &NS_DrawBmpToWindow,                    "DrawBmpToWindow - <hWinHex> <sBmpFilePath>" },
    { "DrawBmpToWindow - <n> <s> <n> <n> <n> <n>", 
                                            &NS_DrawBmpToWindow,                    "DrawBmpToWindow - <hWinHex> <sBmpFilePath> <wx> <wy> <ww> <wh>" },
    { "DrawBmpToWindow - <n> <s> <n> <n> <n> <n> <n> <n>", 
                                            &NS_DrawBmpToWindow,                    "DrawBmpToWindow - <hWinHex> <sBmpFilePath> <bx> <by> <bw> <bh> <wx> <wy>" },
    { "DrawBmpToWindow - <n> <s> <n> <n> <n> <n> <n> <n> <n> <n>", 
                                            &NS_DrawBmpToWindow,                    "DrawBmpToWindow - <hWinHex> <sBmpFilePath> <bx> <by> <bw> <bh> <wx> <wy> <ww> <wh>" },
    { "MoveWindow - <n> <n> <n> <n> <n>",   &NS_MoveWindow,                         "MoveWindow - <hWinHex> <x> <y> <w> <h>" },
    { "RedrawWindow - <n>",                 &NS_RedrawWindow,                       "RedrawWindow - <hWinHex>" },
    { "SetWindowZOrder - <n> <n>",          &NS_SetWindowZOrder,                    "SetWindowZOrder - <hWinHex> <zorder>" },
    { "SetWindowZOrderAfter - <n> <n>",     &NS_SetWindowZOrderAfter,               "SetWindowZOrderAfter - <hWinHex> <hWndRefHex>" },



    { "DeskBgWnd Create",                   &NS_DeskBgWnd,                          "DeskBgWnd Create" },
    { "DeskBgWnd Destroy",                  &NS_DeskBgWnd,                          "DeskBgWnd Destroy" },
    { "DeskBgWnd SetBgColor - <n> <n> <n> <n>", &NS_DeskBgWnd,                      "DeskBgWnd SetBgColor - <r> <g> <b> <a>" },
    { "DeskBgWnd SetRect - <n> <n> <n> <n>",&NS_DeskBgWnd,                          "DeskBgWnd SetRect - <x> <y> <w> <h>" },
    { "DeskBgWnd SetZorder - <n>",          &NS_DeskBgWnd,                          "DeskBgWnd SetZorder - <zorder>" },
    { "DeskBgWnd SetZorderAfter - <n>",     &NS_DeskBgWnd,                          "DeskBgWnd SetZorderAfter - <hWnd>" },
    { "DeskBgWnd Redraw",                   &NS_DeskBgWnd,                          "DeskBgWnd Redraw" },
    { "DeskBgWnd SetVisible - <n>",         &NS_DeskBgWnd,                          "DeskBgWnd SetVisible - <bVisible>" },

    { "kp - <s>",                           &NS_KillProce,                          "kp - <ExeName>" },
    { "exec - <s> ...",                     &NS_Execute,                            "exec - <ExeName> <Arg1> <Arg2> ..." },

    { "NSC SWITCH ALL_ON",                  &NSC_Switch,                            "Switch: All ON" },
    { "NSC SWITCH ALL_OFF",                 &NSC_Switch,                            "Switch: All OFF" },
    { "NSC SWITCH help",                    &NSC_Switch2,                           "NSC SWITCH help" },
    { "NSC SWITCH ON - <s>",                &NSC_Switch2,                           "NSC SWITCH ON - <FuncName>" },
    { "NSC SWITCH OFF - <s>",               &NSC_Switch2,                           "NSC SWITCH OFF - <FuncName>" },

    { "NSC MSG ON",                         &NSC_Msg,                               "MSG: ON" },
    { "NSC MSG OFF",                        &NSC_Msg,                               "MSG: OFF" },
    
    { "NSC 001 001",                        &NSC_001_001,                           "PushObjectDynaMode: OFF" },
    { "NSC 001 002",                        &NSC_001_002,                           "PushObjectDynaMode: DM_AUTOMATIC" },
    { "NSC 002 001",                        &NSC_002_001,                           "Favourite: suc" },
    { "NSC 002 002",                        &NSC_002_002,                           "Favourite: duplicated" },
    { "NSC 002 003",                        &NSC_002_003,                           "Favourite: full" },
    { "NSC 003 001",                        &NSC_003_001,                           "SetDestination: suc" },
    { "NSC 003 002",                        &NSC_003_002,                           "SetDestination: fail" },
    { "NSC 004 001",                        &NSC_004_001,                           "routeCalculationStatus: FINISHED" }, 
    { "NSC 004 002",                        &NSC_004_002,                           "routeCalculationStatus: ABORTED" },
    { "NSC 004 003",                        &NSC_004_003,                           "routeCalculationStatus: ERROR" },
    { "NSC 005 001",                        &NSC_005,                               "ConnNavEnableSetting: ALL ON" },
    { "NSC 005 002",                        &NSC_005,                               "ConnNavEnableSetting: ALL OFF" },
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
        NAVSIN_PRINT_TO_PEER1("Remote Commands\n");
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
                        NAVSIN_PRINT_TO_PEER1("  %s: %s\n", mCmdFuncMap[i].pCmdLine, mCmdFuncMap[i].pDesc);
                    }
                }
            }
        } else {
            for (i = 0; i < nItemCount; i++) {
                NAVSIN_PRINT_TO_PEER1("  %s: %s\n", mCmdFuncMap[i].pCmdLine, mCmdFuncMap[i].pDesc);
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

