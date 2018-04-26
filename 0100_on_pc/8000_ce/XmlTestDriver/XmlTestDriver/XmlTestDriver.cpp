// TestXML001.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <msxml2.h>
#include <dbgapi.h>
#include <sock.h>

#define XTD_DEVICE_TYPE             45889

#define XTD_IOCTL_PARSERXML         CTL_CODE(XTD_DEVICE_TYPE, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Macro that calls a COM method returning HRESULT value:
#define HRCALL(a, errmsg) \
do { \
    hr = (a); \
    if (FAILED(hr)) { \
        dprintf( "%s:%d  HRCALL Failed: %s\n  0x%.8x = %s\n", \
                __FILE__, __LINE__, errmsg, hr, #a ); \
        goto clean; \
    } \
} while (0)

#define PRINT_CURRENT_PROCESS_AND_THREAD_ID() PrintCurrentProcessIdAndThreadId(__FILE__, __FUNCTION__, __LINE__)

// Helper function that put output in stdout and debug window
// in Visual Studio:
void dprintf( char * format, ...)
{
    static char buf[1024];
    va_list args;
    va_start( args, format );
    vsprintf( buf, format, args );
    va_end( args);
    NKDbgPrintfW(L"%S\n", buf);
    //printf("%s", buf);
}

static void PrintCurrentProcessIdAndThreadId(const char *file, const char *function, unsigned int line)
{
    dprintf("### INFO: [in %s:%s():%u] CurrentProcessId=0x%08X, CurrentThreadId=0x%08X\n",
        file, function, line,
        GetCurrentProcessId(), GetCurrentThreadId());
}

// Helper function to create a DOM instance:
IXMLDOMDocument * DomFromCOM()
{
   HRESULT hr;
   IXMLDOMDocument *pxmldoc = NULL;

   HRCALL( CoCreateInstance(__uuidof(/*MSXML2::*/DOMDocument30),
                  NULL,
                  CLSCTX_INPROC_SERVER,
                  __uuidof(IXMLDOMDocument),
                  (void**)&pxmldoc),
                  "Create a new DOMDocument");

    HRCALL( pxmldoc->put_async(VARIANT_FALSE),
            "should never fail");
    HRCALL( pxmldoc->put_validateOnParse(VARIANT_FALSE),
            "should never fail");
    HRCALL( pxmldoc->put_resolveExternals(VARIANT_FALSE),
            "should never fail");

    return pxmldoc;
clean:
    if (pxmldoc)
    {
        pxmldoc->Release();
    }
    return NULL;
}

int EmumChildren(IXMLDOMDocument *pXMLDom)
{
    HRESULT hr;
    IXMLDOMNodeList *pChildList = NULL;
    IXMLDOMNode *pNode = NULL;
    BSTR pBSTemp = NULL;

    HRCALL(pXMLDom->get_childNodes(&pChildList), "pXMLDom->get_childNodes(&pChildList)");
    while (true) {
        HRCALL(pChildList->nextNode(&pNode), "pChildList->nextNode(&pNode)");
        if (pNode == NULL) {
            break;
        }

        HRCALL(pNode->get_baseName(&pBSTemp), "pNode->get_baseName(&pBSTemp)");
        dprintf("### %S\n", pBSTemp);

        SysFreeString(pBSTemp);
        pBSTemp = NULL;

        pNode->Release();
        pNode = NULL;
    }

clean:
    if (pBSTemp != NULL) {
        SysFreeString(pBSTemp);
        pBSTemp = NULL;
    }

    if (pNode != NULL) {
        pNode->Release();
        pNode = NULL;
    }

    if (pChildList != NULL) {
        pChildList->Release();
        pChildList = NULL;
    }
    return 0;
}

int QueryByXPath(IXMLDOMDocument *pXMLDom)
{
    HRESULT hr;
    BSTR bsQuery = NULL;
    IXMLDOMNodeList *pNodeList = NULL;
    IXMLDOMNode *pNode = NULL;
    BSTR pBSTemp = NULL;
    VARIANT vValue = { 0 };

    bsQuery = SysAllocString(L"//price");
    HRCALL(pXMLDom->selectNodes(bsQuery, &pNodeList), "pXMLDom->selectNodes(bsQuery, &pNodeList)s");
    while (true) {
        HRCALL(pNodeList->nextNode(&pNode), "pNodeList->nextNode(&pNode)");
        if (pNode == NULL) {
            break;
        }

        HRCALL(pNode->get_baseName(&pBSTemp), "pNode->get_baseName(&pBSTemp)");
        dprintf("### %S: ", pBSTemp);

        HRCALL(pNode->get_nodeValue(&vValue), "pNode->get_nodeValue(&vValue)");
        dprintf("%S\n", vValue.bstrVal);
        VariantClear(&vValue);

        SysFreeString(pBSTemp);
        pBSTemp = NULL;

        pNode->Release();
        pNode = NULL;
    }

clean:
    VariantClear(&vValue);

    if (pBSTemp != NULL) {
        SysFreeString(pBSTemp);
        pBSTemp = NULL;
    }

    if (bsQuery != NULL) {
        SysFreeString(bsQuery);
        bsQuery = NULL;
    }

    if (pNode != NULL) {
        pNode->Release();
        pNode = NULL;
    }

    if (pNodeList != NULL) {
        pNodeList->Release();
        pNodeList = NULL;
    }

    return 0;
}

int _tmain_001(int argc, _TCHAR* argv[])
{
    IXMLDOMDocument *pXMLDom=NULL;
    IXMLDOMParseError *pXMLErr=NULL;
    BSTR bstr = NULL;
    VARIANT_BOOL status;
    VARIANT var;
    HRESULT hr;

    PRINT_CURRENT_PROCESS_AND_THREAD_ID();

    CoInitialize(NULL);

    pXMLDom = DomFromCOM();
    if (!pXMLDom) goto clean;

    VariantInit(&var);
    V_BSTR(&var) = SysAllocString(L"/release/stocks.xml");
    V_VT(&var) = VT_BSTR;
    HRCALL(pXMLDom->load(var, &status), "");

    if (status!=VARIANT_TRUE) {
        HRCALL(pXMLDom->get_parseError(&pXMLErr),"");
        HRCALL(pXMLErr->get_reason(&bstr),"");
        dprintf("*** Failed to load DOM from stocks.xml. %S\n",
                    bstr);
        goto clean;
    }
//    EmumChildren(pXMLDom);
    QueryByXPath(pXMLDom);
//    HRCALL(pXMLDom->get_xml(&bstr), "");
    dprintf("XML DOM loaded from stocks.xml:\n%S\n",bstr);

clean:
    if (bstr) SysFreeString(bstr);
    if (&var) VariantClear(&var);
    if (pXMLErr) pXMLErr->Release();
    if (pXMLDom) pXMLDom->Release();

    CoUninitialize();
    return 0;
}

DWORD WINAPI XTD_UserThreadProc(void *arg)
{
#define MAX_BUF_SIZE		1024
#define MAX_STR_LEN			255
#define PORT				1059

	char buf[MAX_BUF_SIZE];

#ifdef WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
        int n = WSAGetLastError();
        printf("Error at WSAStartup()\n");
        return -1;
    }
#endif // #ifdef WIN32
	
	Socket sock;
	if (!sock.create(SOCK_DGRAM))
	{
		printf("Fatal Error: sock.create() failed!\n");
		return -1;
	}
	sock.reuse_addr();
	if (!sock.bind(PORT))
	{
		printf("Fatal Error: sock.bind() failed!\n");
		return -1;
	}	

	for (;;)
	{
		printf("#############################\n");
		size_t size = sizeof(buf);
		if (!sock.recvfrom(buf, size))
		{
			printf("Error: sock.recvfrom() error!\n");
			continue;
		}

		buf[size] = '\0';
		printf("Server Recv: %s\n", buf);

		//system(buf);
	}

	return 0;
}

void XTD_LauchUserThread(void)
{
    
    static bool bFirst = true;
    if (bFirst)
    {
        DWORD dwThreadId;
        //_beginthread(&XTD_UserThreadProc, 0, NULL);
        CreateThread(NULL, 0, &XTD_UserThreadProc, NULL, 0, &dwThreadId);

        bFirst = false;
    }
}

BOOL WINAPI XTD_DllEntry(HANDLE hInstDll, DWORD dwReason, LPVOID lpvReserved)
{
    PRINT_CURRENT_PROCESS_AND_THREAD_ID();

    switch ( dwReason ) 
    {
        case DLL_PROCESS_ATTACH:
            //XTD_LauchUserThread();
        break;
    }
    return TRUE;
}


BOOL XTD_Close(DWORD dwHandle)
{	
	return TRUE;
}

DWORD XTD_Init(DWORD dwContext)
{
	RETAILMSG(TRUE,(TEXT("Hello Driver!")));
		
	return TRUE;
}

BOOL XTD_Deinit(DWORD dwContext)
{
	RETAILMSG(TRUE,(TEXT("Goodbye Driver!")));
	return TRUE;
}

DWORD XTD_Open(
    DWORD dwData,
    DWORD dwAccess,
    DWORD dwShareMode
    )
{	
    XTD_LauchUserThread();
	return TRUE;
}


BOOL XTD_IOControl(
    DWORD dwHandle,
    DWORD dwIoControlCode,
    PBYTE pBufIn,
    DWORD dwBufInSize,
    PBYTE pBufOut,
    DWORD dwBufOutSize,
    PDWORD pBytesReturned
    )
{	
    BOOL bResult = FALSE;

    switch (dwIoControlCode)
    {
    case XTD_IOCTL_PARSERXML:
        _tmain_001(0, NULL);
        bResult = TRUE;
        break;
    default:
        break;
    }
	return bResult;
}

DWORD XTD_Read(DWORD dwHandle, LPVOID pBuffer, DWORD dwNumBytes)
{
	return 0;
}

DWORD XTD_Write(DWORD dwHandle, LPCVOID pBuffer, DWORD dwNumBytes)
{
	return 0;
}


DWORD XTD_Seek(DWORD dwHandle, long lDistance, DWORD dwMoveMethod)
{
	return FALSE;
}

void XTD_PowerUp(void)
{
	return;
}

void XTD_PowerDown(void)
{
	return;
}