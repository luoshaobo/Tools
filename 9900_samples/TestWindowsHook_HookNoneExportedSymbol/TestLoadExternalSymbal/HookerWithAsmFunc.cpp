#include "HookerWithAsmFunc.h"

////////////////////////////////////////////////////////////////////////////////////////

#if 0
static HookerWithAsmFunc::HookExtInfo oHookExtInfo;
//void HookDummyClass::HookFunc(int arg1, int arg2, int arg3)
int __stdcall HookFunc(int arg1, int arg2, int arg3, int arg4)
{
    int nRet;
    HookerWithAsmFunc::HookExtInfo *pHookExtInfo = &oHookExtInfo;

    pHookExtInfo->nLoadLibraryRet = pHookExtInfo->pLoadLibrary(pHookExtInfo->pLoadLibraryArg_LibPath);
    //nRet = (this->*(pHookExtInfo->pOrigHookFunc))(arg1, arg2, arg3);
    nRet = (pFunc4_stdcall)pHookExtInfo->pOrigHookFunc(arg1, arg2, arg3, arg4);
    pHookExtInfo->pSetEvent(pHookExtInfo->hEvent);

    return nRet;
}
#endif // #if 0

HookerWithAsmFunc::HookerWithAsmFunc() 
    : m_pHookExtInfo(NULL) 
{

}

////////////////////////////////////////////////////////////////////////////////////////

class HookerWithAsmFunc_Win32_x86_cdecl : public HookerWithAsmFunc
{
public:
    virtual BOOL GetDetourFuncContent(std::vector<unsigned char> &vDetourFuncContent);

public:
    static const unsigned char m_DetoureFuncContentTemplate[];
};

const unsigned char HookerWithAsmFunc_Win32_x86_cdecl::m_DetoureFuncContentTemplate[] = {
    //;	COMDAT ?HookFunc@@YAHHHHH@Z
    //_TEXT	SEGMENT
    //_pHookExtInfo$ = -8					; size = 4
    //_nRet$ = -4						; size = 4
    //_arg1$ = 8						; size = 4
    //_arg2$ = 12						; size = 4
    //_arg3$ = 16						; size = 4
    //_arg4$ = 20						; size = 4
    //?HookFunc@@YAHHHHH@Z PROC				; HookFunc, COMDAT

    //; 16   : {

      /*00000*/	0x55,		                                //push	 ebp
      /*00001*/	0x8b, 0xec,		                            //mov	 ebp, esp
      /*00003*/	0x83, 0xec, 0x08,	                        //sub	 esp, 8

    //; 17   :     int nRet;
    //; 18   :     HookerWithAsmFunc::HookExtInfo *pHookExtInfo = NULL;

      /*00006*/	0xc7, 0x45, 0xf8, 0x00, 0x00,
	    0x00, 0x00,		                                    //mov	 DWORD PTR _pHookExtInfo$[ebp], 0       // NOTE: should point to the real HookExtInfo data!

    //; 19   : 
    //; 20   :     pHookExtInfo->nLoadLibraryRet = pHookExtInfo->pLoadLibrary(pHookExtInfo->pLoadLibraryArg_LibPath);

      /*0000d*/	0x8b, 0x45, 0xf8,	                        //mov	 eax, DWORD PTR _pHookExtInfo$[ebp]
      /*00010*/	0x83, 0xc0, 0x14,	                        //add	 eax, 20			; 00000014H
      /*00013*/	0x50,		                                //push	 eax
      /*00014*/	0x8b, 0x4d, 0xf8,	                        //mov	 ecx, DWORD PTR _pHookExtInfo$[ebp]
      /*00017*/	0x8b, 0x51, 0x0c,	                        //mov	 edx, DWORD PTR [ecx+12]
      /*0001a*/	0xff, 0xd2,		                            //call	 edx
      /*0001c*/	0x8b, 0x4d, 0xf8,	                        //mov	 ecx, DWORD PTR _pHookExtInfo$[ebp]
      /*0001f*/	0x89, 0x41, 0x10,	                        //mov	 DWORD PTR [ecx+16], eax

    //; 21   :     nRet = ((pFunc4_cdecl)pHookExtInfo->pOrigHookFunc)(arg1, arg2, arg3, arg4);

      /*00022*/	0x8b, 0x55, 0x14,	                        //mov	 edx, DWORD PTR _arg4$[ebp]
      /*00025*/	0x52,		                                //push	 edx
      /*00026*/	0x8b, 0x45, 0x10,	                        //mov	 eax, DWORD PTR _arg3$[ebp]
      /*00029*/	0x50,		                                //push	 eax
      /*0002a*/	0x8b, 0x4d, 0x0c,	                        //mov	 ecx, DWORD PTR _arg2$[ebp]
      /*0002d*/	0x51,		                                //push	 ecx
      /*0002e*/	0x8b, 0x55, 0x08,	                        //mov	 edx, DWORD PTR _arg1$[ebp]
      /*00031*/	0x52,		                                //push	 edx
      /*00032*/	0x8b, 0x45, 0xf8,	                        //mov	 eax, DWORD PTR _pHookExtInfo$[ebp]
      /*00035*/	0x8b, 0x08,		                            //mov	 ecx, DWORD PTR [eax]
      /*00037*/	0xff, 0xd1,		                            //call	 ecx
      /*00039*/	0x83, 0xc4, 0x10,	                        //add	 esp, 16			; 00000010H
      /*0003c*/	0x89, 0x45, 0xfc,	                        //mov	 DWORD PTR _nRet$[ebp], eax

    //; 22   :     pHookExtInfo->pSetEvent(pHookExtInfo->hEvent);

      /*0003f*/	0x8b, 0x55, 0xf8,	                        //mov	 edx, DWORD PTR _pHookExtInfo$[ebp]
      /*00042*/	0x8b, 0x42, 0x08,	                        //mov	 eax, DWORD PTR [edx+8]
      /*00045*/	0x50,		                                //push	 eax
      /*00046*/	0x8b, 0x4d, 0xf8,	                        //mov	 ecx, DWORD PTR _pHookExtInfo$[ebp]
      /*00049*/	0x8b, 0x51, 0x04,	                        //mov	 edx, DWORD PTR [ecx+4]
      /*0004c*/	0xff, 0xd2,		                            //call	 edx

    //; 23   : 
    //; 24   :     return nRet;

      /*0004e*/	0x8b, 0x45, 0xfc,	                        //mov	 eax, DWORD PTR _nRet$[ebp]

    //; 25   : }

      /*00051*/	0x8b, 0xe5,		                            //mov	 esp, ebp
      /*00053*/	0x5d,		                                //pop	 ebp
      /*00054*/	0xc3,		                                //ret	 0
};

BOOL HookerWithAsmFunc_Win32_x86_cdecl::GetDetourFuncContent(std::vector<unsigned char> &vDetourFuncContent)
{
    BOOL bSuc = FALSE;

    vDetourFuncContent.clear();

    if (m_pHookExtInfo == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    
    vDetourFuncContent.assign(m_DetoureFuncContentTemplate, m_DetoureFuncContentTemplate + sizeof(m_DetoureFuncContentTemplate));
    memcpy(&vDetourFuncContent[9], &m_pHookExtInfo, sizeof(unsigned int));

    bSuc = TRUE;

FAILED:
    return bSuc;
}

////////////////////////////////////////////////////////////////////////////////////////

class HookerWithAsmFunc_Win32_x86_stdcall : public HookerWithAsmFunc
{
public:
    HookerWithAsmFunc_Win32_x86_stdcall(unsigned int nArgCount) : m_nArgCount(nArgCount) {}
public:
    virtual BOOL GetDetourFuncContent(std::vector<unsigned char> &vDetourFuncContent);

private:
    static const unsigned char m_DetoureFuncContentTemplate[];
    unsigned int m_nArgCount;
};

const unsigned char HookerWithAsmFunc_Win32_x86_stdcall::m_DetoureFuncContentTemplate[] = {
    //;	COMDAT ?HookFunc@@YGHHHHH@Z
    //_TEXT	SEGMENT
    //_pHookExtInfo$ = -8					; size = 4
    //_nRet$ = -4						; size = 4
    //_arg1$ = 8						; size = 4
    //_arg2$ = 12						; size = 4
    //_arg3$ = 16						; size = 4
    //_arg4$ = 20						; size = 4
    //?HookFunc@@YGHHHHH@Z PROC				; HookFunc, COMDAT

    //; 15   : {

      /*00000*/	0x55,		                                //push	 ebp
      /*00001*/	0x8b, 0xec,		                            //mov	 ebp, esp
      /*00003*/	0x83, 0xec, 0x08,	                        //sub	 esp, 8

    //; 16   :     int nRet;
    //; 17   :     HookerWithAsmFunc::HookExtInfo *pHookExtInfo = NULL;

      /*00006*/	0xc7, 0x45, 0xf8, 0x00, 0x00,
	    0x00, 0x00,		                                    //mov	 DWORD PTR _pHookExtInfo$[ebp], 0       // NOTE: should point to the real HookExtInfo data!

    //; 18   : 
    //; 19   :     pHookExtInfo->nLoadLibraryRet = pHookExtInfo->pLoadLibrary(pHookExtInfo->pLoadLibraryArg_LibPath);

      /*0000d*/	0x8b, 0x45, 0xf8,	                        //mov	 eax, DWORD PTR _pHookExtInfo$[ebp]
      /*00010*/	0x83, 0xc0, 0x14,	                        //add	 eax, 20			; 00000014H
      /*00013*/	0x50,		                                //push	 eax
      /*00014*/	0x8b, 0x4d, 0xf8,	                        //mov	 ecx, DWORD PTR _pHookExtInfo$[ebp]
      /*00017*/	0x8b, 0x51, 0x0c,	                        //mov	 edx, DWORD PTR [ecx+12]
      /*0001a*/	0xff, 0xd2,		                            //call	 edx
      /*0001c*/	0x8b, 0x4d, 0xf8,	                        //mov	 ecx, DWORD PTR _pHookExtInfo$[ebp]
      /*0001f*/	0x89, 0x41, 0x10,	                        //mov	 DWORD PTR [ecx+16], eax

    //; 20   :     nRet = ((pFunc4_stdcall)pHookExtInfo->pOrigHookFunc)(arg1, arg2, arg3, arg4);

      /*00022*/	0x8b, 0x55, 0x14,	                        //mov	 edx, DWORD PTR _arg4$[ebp]
      /*00025*/	0x52,		                                //push	 edx
      /*00026*/	0x8b, 0x45, 0x10,	                        //mov	 eax, DWORD PTR _arg3$[ebp]
      /*00029*/	0x50,		                                //push	 eax
      /*0002a*/	0x8b, 0x4d, 0x0c,	                        //mov	 ecx, DWORD PTR _arg2$[ebp]
      /*0002d*/	0x51,		                                //push	 ecx
      /*0002e*/	0x8b, 0x55, 0x08,	                        //mov	 edx, DWORD PTR _arg1$[ebp]
      /*00031*/	0x52,		                                //push	 edx
      /*00032*/	0x8b, 0x45, 0xf8,	                        //mov	 eax, DWORD PTR _pHookExtInfo$[ebp]
      /*00035*/	0x8b, 0x08,		                            //mov	 ecx, DWORD PTR [eax]
      /*00037*/	0xff, 0xd1,		                            //call	 ecx
      /*00039*/	0x83, 0xc4, 0x00,	                        //add	 esp, 0                                 // NOTE: should pop out the addtional arguments!
      /*00039*/	0x89, 0x45, 0xfc,	                        //mov	 DWORD PTR _nRet$[ebp], eax

    //; 21   :     pHookExtInfo->pSetEvent(pHookExtInfo->hEvent);

      /*0003c*/	0x8b, 0x55, 0xf8,	                        //mov	 edx, DWORD PTR _pHookExtInfo$[ebp]
      /*0003f*/	0x8b, 0x42, 0x08,	                        //mov	 eax, DWORD PTR [edx+8]
      /*00042*/	0x50,		                                //push	 eax
      /*00043*/	0x8b, 0x4d, 0xf8,	                        //mov	 ecx, DWORD PTR _pHookExtInfo$[ebp]
      /*00046*/	0x8b, 0x51, 0x04,	                        //mov	 edx, DWORD PTR [ecx+4]
      /*00049*/	0xff, 0xd2,		                            //call	 edx

    //; 22   : 
    //; 23   :     return nRet;

      /*0004b*/	0x8b, 0x45, 0xfc,	                        //mov	 eax, DWORD PTR _nRet$[ebp]

    //; 24   : }

      /*0004e*/	0x8b, 0xe5,		                            //mov	 esp, ebp
      /*00050*/	0x5d,		                                //pop	 ebp
      /*00051*/	0xc2, 0x10, 0x00,	                        //ret	 16			; 00000010H                 // NOTE: should pop out just the real arguments!
};

BOOL HookerWithAsmFunc_Win32_x86_stdcall::GetDetourFuncContent(std::vector<unsigned char> &vDetourFuncContent)
{
    BOOL bSuc = FALSE;

    vDetourFuncContent.clear();

    if (m_pHookExtInfo == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    if (m_nArgCount > 4) {
        bSuc = FALSE;
        goto FAILED;
    }
    
    vDetourFuncContent.assign(m_DetoureFuncContentTemplate, m_DetoureFuncContentTemplate + sizeof(m_DetoureFuncContentTemplate));
    memcpy(&vDetourFuncContent[9], &m_pHookExtInfo, sizeof(unsigned int));
    vDetourFuncContent[sizeof(m_DetoureFuncContentTemplate) - 28] = (4 - m_nArgCount) * 4;
    vDetourFuncContent[sizeof(m_DetoureFuncContentTemplate) - 2] = m_nArgCount * 4;

    bSuc = TRUE;

FAILED:
    return bSuc;
}

////////////////////////////////////////////////////////////////////////////////////////

class HookerWithAsmFunc_Win32_x86_thiscall : public HookerWithAsmFunc
{
public:
    HookerWithAsmFunc_Win32_x86_thiscall(unsigned int nArgCount) : m_nArgCount(nArgCount) {}
public:
    virtual BOOL GetDetourFuncContent(std::vector<unsigned char> &vDetourFuncContent);

private:
    static const unsigned char m_DetoureFuncContentTemplate[];
    unsigned int m_nArgCount;
};

const unsigned char HookerWithAsmFunc_Win32_x86_thiscall::m_DetoureFuncContentTemplate[] = {
    //;	COMDAT ?HookFunc@HookDummyClass@@QAEHHHH@Z
    //_TEXT	SEGMENT
    //_this$ = -12						; size = 4
    //_pHookExtInfo$ = -8					; size = 4
    //_nRet$ = -4						; size = 4
    //_arg1$ = 8						; size = 4
    //_arg2$ = 12						; size = 4
    //_arg3$ = 16						; size = 4
    //?HookFunc@HookDummyClass@@QAEHHHH@Z PROC		; HookDummyClass::HookFunc, COMDAT
    //; _this$ = ecx

    //; 7    : {

      /*00000*/	0x55,		                                //push	 ebp
      /*00001*/	0x8b, 0xec,		                            //mov	 ebp, esp
      /*00003*/	0x83, 0xec, 0x0c,	                        //sub	 esp, 12			; 0000000cH
      /*00006*/	0x89, 0x4d, 0xf4,	                        //mov	 DWORD PTR _this$[ebp], ecx

    //; 8    :     int nRet;
    //; 9    :     HookerWithAsmFunc::HookExtInfo *pHookExtInfo = NULL;

      /*00009*/	0xc7, 0x45, 0xf8, 0x00, 0x00,
	    0x00, 0x00,		                                    //mov	 DWORD PTR _pHookExtInfo$[ebp], 0       // NOTE: should point to the real HookExtInfo data!

    //; 10   : 
    //; 11   :     pHookExtInfo->nLoadLibraryRet = pHookExtInfo->pLoadLibrary(pHookExtInfo->pLoadLibraryArg_LibPath);

      /*00010*/	0x8b, 0x45, 0xf8,	                        //mov	 eax, DWORD PTR _pHookExtInfo$[ebp]
      /*00013*/	0x83, 0xc0, 0x14,	                        //add	 eax, 20			; 00000014H
      /*00016*/	0x50,		                                //push	 eax
      /*00017*/	0x8b, 0x4d, 0xf8,	                        //mov	 ecx, DWORD PTR _pHookExtInfo$[ebp]
      /*0001a*/	0x8b, 0x51, 0x0c,	                        //mov	 edx, DWORD PTR [ecx+12]
      /*0001d*/	0xff, 0xd2,		                            //call	 edx
      /*0001f*/	0x8b, 0x4d, 0xf8,	                        //mov	 ecx, DWORD PTR _pHookExtInfo$[ebp]
      /*00022*/	0x89, 0x41, 0x10,	                        //mov	 DWORD PTR [ecx+16], eax

    //; 12   :     nRet = (this->*(pHookExtInfo->pOrigHookFunc))(arg1, arg2, arg3);

      /*00025*/	0x8b, 0x55, 0x10,	                        //mov	 edx, DWORD PTR _arg3$[ebp]
      /*00028*/	0x52,		                                //push	 edx
      /*00029*/	0x8b, 0x45, 0x0c,	                        //mov	 eax, DWORD PTR _arg2$[ebp]
      /*0002c*/	0x50,		                                //push	 eax
      /*0002d*/	0x8b, 0x4d, 0x08,	                        //mov	 ecx, DWORD PTR _arg1$[ebp]
      /*00030*/	0x51,		                                //push	 ecx
      /*00031*/	0x8b, 0x55, 0xf8,	                        //mov	 edx, DWORD PTR _pHookExtInfo$[ebp]
      /*00034*/	0x8b, 0x4d, 0xf4,	                        //mov	 ecx, DWORD PTR _this$[ebp]
      /*00037*/	0x8b, 0x02,		                            //mov	 eax, DWORD PTR [edx]
      /*00039*/	0x83, 0xc4, 0x00,	                        //add	 esp, 0                                 // NOTE: should pop out the addtional arguments!
      /*00039*/	0xff, 0xd0,		                            //call	 eax
      /*0003b*/	0x89, 0x45, 0xfc,	                        //mov	 DWORD PTR _nRet$[ebp], eax

    //; 13   :     pHookExtInfo->pSetEvent(pHookExtInfo->hEvent);

      /*0003e*/	0x8b, 0x4d, 0xf8,	                        //mov	 ecx, DWORD PTR _pHookExtInfo$[ebp]
      /*00041*/	0x8b, 0x51, 0x08,	                        //mov	 edx, DWORD PTR [ecx+8]
      /*00044*/	0x52,		                                //push	 edx
      /*00045*/	0x8b, 0x45, 0xf8,	                        //mov	 eax, DWORD PTR _pHookExtInfo$[ebp]
      /*00048*/	0x8b, 0x48, 0x04,	                        //mov	 ecx, DWORD PTR [eax+4]
      /*0004b*/	0xff, 0xd1,		                            //call	 ecx

    //; 14   : 
    //; 15   :     return nRet;

      /*0004d*/	0x8b, 0x45, 0xfc,	                        //mov	 eax, DWORD PTR _nRet$[ebp]

    //; 16   : }

      /*00050*/	0x8b, 0xe5,		                            //mov	 esp, ebp
      /*00052*/	0x5d,		                                //pop	 ebp
      /*00053*/	0xc2, 0x0c, 0x00,	                        //ret	 12			; 0000000cH                 // NOTE: should pop out just the real arguments!
};

BOOL HookerWithAsmFunc_Win32_x86_thiscall::GetDetourFuncContent(std::vector<unsigned char> &vDetourFuncContent)
{
    BOOL bSuc = FALSE;

    vDetourFuncContent.clear();

    if (m_pHookExtInfo == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    if (m_nArgCount > 3) {
        bSuc = FALSE;
        goto FAILED;
    }
    
    vDetourFuncContent.assign(m_DetoureFuncContentTemplate, m_DetoureFuncContentTemplate + sizeof(m_DetoureFuncContentTemplate));
    memcpy(&vDetourFuncContent[12], &m_pHookExtInfo, sizeof(unsigned int));
    vDetourFuncContent[sizeof(m_DetoureFuncContentTemplate) - 30] = (3 - m_nArgCount) * 4;
    vDetourFuncContent[sizeof(m_DetoureFuncContentTemplate) - 2] = m_nArgCount * 4;

    bSuc = TRUE;

FAILED:
    return bSuc;
}

////////////////////////////////////////////////////////////////////////////////////////

class HookerWithAsmFunc_WinCE_ARM : public HookerWithAsmFunc
{
public:
    virtual BOOL GetDetourFuncContent(std::vector<unsigned char> &vDetourFuncContent);

private:
    static const unsigned int m_DetoureFuncContentTemplate[];
};

const unsigned int HookerWithAsmFunc_WinCE_ARM::m_DetoureFuncContentTemplate[] = {
    //  00000		 |?HookFunc@HookDummyClass@@QAAHHHH@Z| PROC ; HookDummyClass::HookFunc

    //; 8    : {

    //  00000		 |$LN5@HookFunc|
      /*00000*/	0xe1a0c00d,	 //mov         r12,sp
      /*00004*/	0xe92d000f,	 //push        {r0-r3}
      /*00008*/	0xe92d5010,	 //push        {r4,r12,lr}
      /*0000c*/	0xe24dd014,	 //sub         sp,sp,#0x14
    //  00010		 |$M39057|

    //; 9    :     int nRet;
    //; 10   :     HookerWithAsmFunc::HookExtInfo *pHookExtInfo = &oHookExtInfo;

      /*00010*/	//0xe59f3084,	 //ldr         r3,|$LN7@HookFunc|	; =|?oHookExtInfo@@3UHookExtInfo@HookerWithAsmFunc@@A|
      /*00010*/	0xe59f3088,	 //ldr         r3,|$LN7@HookFunc|	; =|?oHookExtInfo@@3UHookExtInfo@HookerWithAsmFunc@@A|      // NOTE: fix since add an instruction by hand.
      /*00014*/	0xe58d3004,	 //str         r3,[sp,#4]

    //; 11   : 
    //; 12   :     pHookExtInfo->nLoadLibraryRet = pHookExtInfo->pLoadLibrary(pHookExtInfo->pLoadLibraryArg_LibPath);

      /*00018*/	0xe59d3004,	 //ldr         r3,[sp,#4]
      /*0001c*/	0xe2830014,	 //add         r0,r3,#0x14
      /*00020*/	0xe59d3004,	 //ldr         r3,[sp,#4]
      /*00024*/	0xe283300c,	 //add         r3,r3,#0xC
      /*00028*/	0xe5933000,	 //ldr         r3,[r3]
      /*0002c*/	0xe12fff33,	 //blx         r3
      /*00030*/	0xe58d000c,	 //str         r0,[sp,#0xC]
      /*00034*/	0xe59d3004,	 //ldr         r3,[sp,#4]
      /*00038*/	0xe2832010,	 //add         r2,r3,#0x10
      /*0003c*/	0xe59d300c,	 //ldr         r3,[sp,#0xC]
      /*00040*/	0xe5823000,	 //str         r3,[r2]

    //; 13   :     nRet = (this->*(pHookExtInfo->pOrigHookFunc))(arg1, arg2, arg3);

      /*00044*/	0xe59d302c,	 //ldr         r3,[sp,#0x2C]
      /*00048*/	0xe59d2028,	 //ldr         r2,[sp,#0x28]
      /*0004c*/	0xe59d1024,	 //ldr         r1,[sp,#0x24]
      /*00050*/	0xe59d0020,	 //ldr         r0,[sp,#0x20]
      /*00054*/	0xe59de004,	 //ldr         lr,[sp,#4]
      /*00058*/	0xe59e4000,	 //ldr         r4,[lr]
      /*0005c*/	0xe12fff34,	 //blx         r4
      /*00060*/	0xe58d0010,	 //str         r0,[sp,#0x10]
      /*00064*/	0xe59d3010,	 //ldr         r3,[sp,#0x10]
      /*00068*/	0xe58d3000,	 //str         r3,[sp]

    //; 14   :     pHookExtInfo->pSetEvent(pHookExtInfo->hEvent);

      /*0006c*/	0xe59d3004,	 //ldr         r3,[sp,#4]
      /*00070*/	0xe2833008,	 //add         r3,r3,#8
      /*00074*/	0xe5930000,	 //ldr         r0,[r3]
                0xe3a01003,  //mov         r1, #3                                                           // NOTE: added by hand for:
                                                                                                            //    _inline BOOL SetEvent(HANDLE h) {
                                                                                                            //        return EventModify(h,EVENT_SET/*3*/);
                                                                                                            //    }
      /*00078*/	0xe59d3004,	 //ldr         r3,[sp,#4]
      /*0007c*/	0xe2833004,	 //add         r3,r3,#4
      /*00080*/	0xe5933000,	 //ldr         r3,[r3]
      /*00084*/	0xe12fff33,	 //blx         r3

    //; 15   : 
    //; 16   :     return nRet;

      /*00088*/	0xe59d3000,	 //ldr         r3,[sp]
      /*0008c*/	0xe58d3008,	 //str         r3,[sp,#8]

    //; 17   : }

      /*00090*/	0xe59d0008,	 //ldr         r0,[sp,#8]
      /*00094*/	0xe28dd014,	 //add         sp,sp,#0x14
      /*00098*/	0xe89da010,	 //ldm         sp,{r4,sp,pc}
    //  0009c		 |$LN6@HookFunc|
    //  0009c		 |$LN7@HookFunc|
      /*0009c*/	0x00000000,	 //DCD         |?oHookExtInfo@@3UHookExtInfo@HookerWithAsmFunc@@A|              // NOTE: should point to the real HookExtInfo data!
};

BOOL HookerWithAsmFunc_WinCE_ARM::GetDetourFuncContent(std::vector<unsigned char> &vDetourFuncContent)
{
    BOOL bSuc = FALSE;
    std::vector<unsigned int> vDetourFuncContentTemp(m_DetoureFuncContentTemplate, 
        m_DetoureFuncContentTemplate + sizeof(m_DetoureFuncContentTemplate) / sizeof(unsigned int));

    vDetourFuncContent.clear();

    if (m_pHookExtInfo == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    
    vDetourFuncContentTemp[vDetourFuncContentTemp.size() - 1] = (unsigned int)m_pHookExtInfo;
    vDetourFuncContent.assign((unsigned char *)&vDetourFuncContentTemp[0], (unsigned char *)&vDetourFuncContentTemp[0] + sizeof(m_DetoureFuncContentTemplate));

    bSuc = TRUE;

FAILED:
    return bSuc;
}

////////////////////////////////////////////////////////////////////////////////////////
HookerWithAsmFunc *HookerWithAsmFunc::CreateInstance()
{
    HookerWithAsmFunc *pHookerWithAsmFunc = NULL;

#if defined(WIN32_X86) // for Win32 X86
    pHookerWithAsmFunc = new HookerWithAsmFunc_Win32_x86_thiscall(3);
#elif defined(WINCE_ARM32) // for WinCE ARM32
    pHookerWithAsmFunc = new HookerWithAsmFunc_WinCE_ARM();
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

    return pHookerWithAsmFunc;
}
