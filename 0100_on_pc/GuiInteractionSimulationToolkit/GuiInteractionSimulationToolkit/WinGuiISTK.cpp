#include "stdafx.h"
#include "Psapi.h"
#include "TK_Tools.h"
#include "WinGuiISTK.h"

#define THIS_PROG_BIN_NAME_DEF              _T("guiistk")

namespace GuiISTk {

const unsigned int WAIT_IMAGE_SHOWN_INTERVAL = 500;
const unsigned int WAIT_IMAGE_HIDDEN_INTERVAL = 2000;

VOID SendMouseEvent(
    DWORD dwFlags,
    DWORD dx,
    DWORD dy,
    DWORD dwData,
    ULONG_PTR dwExtraInfo
)
{
    INPUT input;

    input.type = INPUT_MOUSE;
    input.mi.dx = dx;
    input.mi.dy = dy;
    input.mi.mouseData = dwData;
    input.mi.dwFlags = dwFlags;
    input.mi.time = 0;
    input.mi.dwExtraInfo = dwExtraInfo;

    ::SendInput(1, &input, sizeof(INPUT));
}

VOID SendKeyboardEvent(
    BYTE bVk,
    BYTE bScan,
    DWORD dwFlags,
    ULONG_PTR dwExtraInfo
)
{
    INPUT input;

    input.type = INPUT_KEYBOARD;
    input.ki.wVk = bVk;
    input.ki.wScan = bScan;
    input.ki.dwFlags = dwFlags;
    input.ki.time = 0;
    input.ki.dwExtraInfo = dwExtraInfo;

    ::SendInput(1, &input, sizeof(INPUT));
}

WinGuiISTK::WinGuiISTK() : m_sExeFileName(), m_sEnvVarScreenPictureFilePath()
{
    TCHAR szProcessName[MAX_PATH] = {0};

    const char *pEnvVarScreenPictureFilePath = getenv("DESKTOP_PICTURE_FILE_PATH");
    m_sEnvVarScreenPictureFilePath = pEnvVarScreenPictureFilePath == NULL ? "" : pEnvVarScreenPictureFilePath;

    ::GetModuleBaseName(::GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));
    m_sExeFileName = szProcessName;
    int nPos = m_sExeFileName.ReverseFind(_T('.'));
    if (nPos != -1) {
        m_sExeFileName = m_sExeFileName.Left(nPos);
    }

    if (m_sExeFileName.IsEmpty()) {
        m_sExeFileName = THIS_PROG_BIN_NAME_DEF;
    }
}

WinGuiISTK::~WinGuiISTK()
{

}

void WinGuiISTK::Delay(unsigned int milliSecond)
{
    LOG_GEN_PRINTF("milliSecond=%u\n", milliSecond);

    ::Sleep(milliSecond);
}

BOOL CALLBACK WinGuiISTK::getMatchedWindows_EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    Arguments_getMatchedWindows_EnumWindowsProc &arguments = *(Arguments_getMatchedWindows_EnumWindowsProc *)lParam;
    WinGuiISTK *thiz = arguments.thiz;
    CString sTitle(TK_Tools::str2tstr(arguments.screenInfo.title).c_str());
    CWnd window;
    CString sWindowText;
    bool bMatched = false;
    bool bContinueEnumerating = true;

    window.Attach(hwnd);
    window.GetWindowText(sWindowText);
    if (arguments.screenInfo.fullMatched) {
        if (sTitle == sWindowText) {
            bMatched = true;
        }
    } else {
        if (sTitle.IsEmpty()) {
            bMatched = true;
        } else if (sWindowText.Find(sTitle) != -1) {
            if (sWindowText.Find(thiz->m_sExeFileName) == -1) {
                bMatched = true;
            }
        }
    }

    if (bMatched) {
        TCHAR szProcessName[MAX_PATH] = {0};
        DWORD dwProcessId = -1;
        HANDLE hProcess = NULL;
        ::GetWindowThreadProcessId(hwnd, &dwProcessId);
        if (dwProcessId != -1) {
            hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, dwProcessId);
            if (hProcess != NULL) {
                ::GetModuleBaseName(hProcess, NULL, szProcessName, sizeof(szProcessName));
            }
        }

        DWORD dwWndStyle = ::GetWindowLong(hwnd, GWL_STYLE);
        DWORD dwWndExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);

        LOG_GEN_PRINTF("sWindowText=\"%s\", exe=\"%s\", bVisible=%s, bTopMost=%s, bToolWindow=%s\n", 
            TK_Tools::tstr2str(sWindowText.GetString()).c_str(),
            TK_Tools::tstr2str(szProcessName).c_str(),
            ::IsWindowVisible(hwnd) ? "true" : "false",
            (dwWndExStyle & WS_EX_TOPMOST) ? "true" : "false", 
            (dwWndExStyle & WS_EX_TOOLWINDOW) ? "true" : "false"
        );
        arguments.winHandles.push_back(hwnd);
        if (!arguments.screenInfo.allMatched) {
            bContinueEnumerating = false;
        }
    }

    window.Detach();
    ::SetLastError(0);
    return bContinueEnumerating;
}

bool WinGuiISTK::getMatchedWindows(std::vector<HWND> &winHandles, const ScreenInfo &screenInfo)
{
    bool bSuc = true;
    Arguments_getMatchedWindows_EnumWindowsProc argumens(this, screenInfo, winHandles);

    if (bSuc) {
        if (!::EnumWindows(&getMatchedWindows_EnumWindowsProc, (LPARAM)&argumens)) {
            if (::GetLastError() != 0) {
                bSuc = false;
            }
        }
    }

    return bSuc;
}

unsigned int WinGuiISTK::wndCount(const ScreenInfo &screenInfo)
{
    unsigned int nScreenCount = 0;
    std::vector<HWND> winHandles;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched);

    getMatchedWindows(winHandles, screenInfo);
    nScreenCount = winHandles.size();

    return nScreenCount;
}

bool WinGuiISTK::wndShow(const ScreenInfo &screenInfo, ScreenShowingMode mode)
{
    bool bSuc = true;
    std::vector<HWND> winHandles;
    HWND hWnd;
    WINDOWPLACEMENT wndpl;
    unsigned int i;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d, mode=%u\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched, mode);

    if (bSuc) {
        bSuc = getMatchedWindows(winHandles, screenInfo);
    }

    if (bSuc) {
        for (i = 0; i < winHandles.size(); ++i) {
            hWnd = winHandles[i];
            switch (mode) {
            case SSM_RESTORE:
                ::ShowWindow(hWnd, SW_RESTORE);
                break;
            case SSM_NORMAL:
                ::ShowWindow(hWnd, SW_SHOWNORMAL);
                break;
            case SSM_MIN:
                ::ShowWindow(hWnd, SW_SHOWMINIMIZED);
                break;
            case SSM_MAX:
                ::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
                break;
            case SSM_FG:
                memset(&wndpl, 0, sizeof(WINDOWPLACEMENT));
                ::GetWindowPlacement(hWnd, &wndpl);
                if (wndpl.showCmd == SW_SHOWMINIMIZED) {
                    ::ShowWindow(hWnd, SW_SHOWNORMAL);
                    ::Sleep(200);
                }
                ::SetForegroundWindow(hWnd);
                break;
            default:
                ::ShowWindow(hWnd, SW_SHOWNORMAL);
                break;
            }
        }
    }

    return bSuc;
}

bool WinGuiISTK::wndHide(const ScreenInfo &screenInfo)
{
    bool bSuc = true;
    std::vector<HWND> winHandles;
    HWND hWnd;
    unsigned int i;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched);

    if (bSuc) {
        bSuc = getMatchedWindows(winHandles, screenInfo);
    }

    if (bSuc) {
        for (i = 0; i < winHandles.size(); ++i) {
            hWnd = winHandles[i];
            ::ShowWindow(hWnd, SW_HIDE);
        }
    }

    return bSuc;
}

bool WinGuiISTK::wndClose(const ScreenInfo &screenInfo)
{
    bool bSuc = true;
    std::vector<HWND> winHandles;
    HWND hWnd;
    unsigned int i;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched);

    if (bSuc) {
        bSuc = getMatchedWindows(winHandles, screenInfo);
    }

    if (bSuc) {
        for (i = 0; i < winHandles.size(); ++i) {
            hWnd = winHandles[i];
            ::PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
    }

    return bSuc;
}

bool WinGuiISTK::wndMove(const ScreenInfo &screenInfo, const Point &point)
{
    bool bSuc = true;
    std::vector<HWND> winHandles;
    HWND hWnd;
    unsigned int i;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d, point=(%d,%d)\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched, point.x, point.y);

    if (bSuc) {
        bSuc = getMatchedWindows(winHandles, screenInfo);
    }

    if (bSuc) {
        for (i = 0; i < winHandles.size(); ++i) {
            hWnd = winHandles[i];
         
            ::SetWindowPos(hWnd, NULL, point.x, point.y, 0, 0, SWP_ASYNCWINDOWPOS|SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);
        }
    }

    return bSuc;
}

bool WinGuiISTK::wndSetSize(const ScreenInfo &screenInfo, const Rect &rect)
{
    bool bSuc = true;
    std::vector<HWND> winHandles;
    HWND hWnd;
    unsigned int i;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d, rect=(%d,%d,%u,%u)\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched, rect.x, rect.y, rect.width, rect.height);
    
    if (bSuc) {
        bSuc = getMatchedWindows(winHandles, screenInfo);
    }

    if (bSuc) {
        for (i = 0; i < winHandles.size(); ++i) {
            hWnd = winHandles[i];
         
            ::SetWindowPos(hWnd, NULL, rect.x, rect.y, rect.width, rect.height, SWP_ASYNCWINDOWPOS|SWP_NOZORDER|SWP_SHOWWINDOW);
        }
    }

    return bSuc;
}

bool WinGuiISTK::wndGetSize(const ScreenInfo &screenInfo, std::vector<Rect> &rects)
{
    bool bSuc = true;
    std::vector<HWND> winHandles;
    HWND hWnd;
    unsigned int i;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched);

    if (bSuc) {
        bSuc = getMatchedWindows(winHandles, screenInfo);
    }

    if (bSuc) {
        for (i = 0; i < winHandles.size(); ++i) {
            hWnd = winHandles[i];
         
            CRect rectTmp;
            if (!::GetWindowRect(hWnd, (LPRECT)rectTmp)) {
                bSuc = false;
                break;
            }

            rects.push_back(Rect(rectTmp.left, rectTmp.top, rectTmp.Width(), rectTmp.Height()));
        }
    }

    return bSuc;
}

bool WinGuiISTK::wndSetZorder(const ScreenInfo &screenInfo, ScreenZorder zorder)
{
    bool bSuc = true;
    std::vector<HWND> winHandles;
    HWND hWnd;
    unsigned int i;
    HWND hWndInsertAfter;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d, zorder=%u\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched, zorder);

    if (bSuc) {
        bSuc = getMatchedWindows(winHandles, screenInfo);
    }

    if (bSuc) {
        for (i = 0; i < winHandles.size(); ++i) {
            hWnd = winHandles[i];
            switch (zorder) {
            case SZO_BOTTOM:
                hWndInsertAfter = HWND_BOTTOM;
                break;
            case SZO_TOP:
                hWndInsertAfter = HWND_TOP;
                break;
            default:
                hWndInsertAfter = HWND_TOP;
                break;
            }
         
            ::SetWindowPos(hWnd, hWndInsertAfter, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS|SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
        }
    }

    return bSuc;
}

bool WinGuiISTK::wndSaveAsPic(const ScreenInfo &screenInfo, const std::string &sPictureFilePath)
{
    bool bSuc = true;
    std::string sPictureFilePathTemplate;
    std::vector<HWND> winHandles;
    HWND hWnd;
    WINDOWPLACEMENT wndpl;
    unsigned int i;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d, sPictureFilePath=\"%s\"\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched, sPictureFilePath.c_str());

    if (bSuc) {
        bSuc = getMatchedWindows(winHandles, screenInfo);
    }

    if (bSuc) {
        if (!makeFilePathTemplateByIndex(sPictureFilePathTemplate, sPictureFilePath)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        for (i = 0; i < winHandles.size(); ++i) {
            hWnd = winHandles[i];

            memset(&wndpl, 0, sizeof(WINDOWPLACEMENT));
            ::GetWindowPlacement(hWnd, &wndpl);
            if (wndpl.showCmd == SW_SHOWMINIMIZED) {
                ::ShowWindow(hWnd, SW_SHOWNORMAL);
                ::Sleep(200);
            }
            ::SetForegroundWindow(hWnd);
            ::Sleep(200);

            std::string path = sPictureFilePath;
            if (winHandles.size() > 1) {
                path = TK_Tools::FormatStr(sPictureFilePathTemplate.c_str(), i + 1);
            }
            if (!saveWindowAsPicture(hWnd, path)) {
                bSuc = false;
                break;
            }
        }
    }

    return bSuc;
}

bool WinGuiISTK::dspSaveAllAsPics(const std::string &sPictureFilePath)
{
    bool bSuc = true;
    std::string sPictureFilePathTemplate;
    std::vector<Rect> rects;
    unsigned int i;

    LOG_GEN_PRINTF("sPictureFilePath=\"%s\"\n", sPictureFilePath.c_str());

    if (bSuc) {
        if (!makeFilePathTemplateByIndex(sPictureFilePathTemplate, sPictureFilePath)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!getDesktopRects(rects)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        for (i = 0; i < rects.size(); ++i) {
            std::string path = sPictureFilePath;
            if (rects.size() > 1) {
                path = TK_Tools::FormatStr(sPictureFilePathTemplate.c_str(), i + 1);
            }

            CRect rectTmp(rects[i].x, rects[i].y, rects[i].x + rects[i].width, rects[i].y + rects[i].height);
            if (!saveWindowAsPicture(::GetDesktopWindow(), path, &rectTmp)) {
                bSuc = false;
                break;
            }
        }
    }

    return bSuc;
}

void WinGuiISTK::dspGetAllRects(std::vector<Rect> &rects)
{
    bool bSuc = true;

    LOG_GEN_PRINTF("\n");

    rects.clear();

    if (bSuc) {
        if (!getDesktopRects(rects)) {
            bSuc = false;
        }
    }
}

bool WinGuiISTK::dspSavePrimaryAsPic(const std::string &sPictureFilePath)
{
    bool bSuc = true;
    HWND hWnd;

    LOG_GEN_PRINTF("sPictureFilePath=\"%s\"\n", sPictureFilePath.c_str());

    if (bSuc) {
        hWnd = ::GetDesktopWindow();
        if (!saveWindowAsPicture(hWnd, sPictureFilePath)) {
            bSuc = false;
        }
    }

    return bSuc;
}

void WinGuiISTK::dspGetPrimaryRect(Rect &rect)
{
    bool bSuc = true;
    HWND hWnd = NULL;
    CRect rectTmp;

    LOG_GEN_PRINTF("\n");

    rect = Rect();

    if (bSuc) {
        hWnd = ::GetDesktopWindow();
        if (!::GetWindowRect(hWnd, (LPRECT)rectTmp)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        rect.x = 0;
        rect.y = 0;
        rect.width = rectTmp.Width();
        rect.height = rectTmp.Height();
    }
}

bool WinGuiISTK::dspSaveVirtualAsPic(const std::string &sPictureFilePath)
{
    bool bSuc = true;
    HWND hWnd;
    CRect virtualDesktopRect;

    LOG_GEN_PRINTF("sPictureFilePath=\"%s\"\n", sPictureFilePath.c_str());

    if (bSuc) {
        if (!getVirtualDesktopRect(virtualDesktopRect)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        hWnd = ::GetDesktopWindow();
        if (!saveWindowAsPicture(hWnd, sPictureFilePath, &virtualDesktopRect)) {
            bSuc = false;
        }
    }

    return bSuc;
}

void WinGuiISTK::dspGetVirtualRect(Rect &rect)
{
    bool bSuc = true;
    HWND hWnd = NULL;
    CRect virtualDesktopRect;

    LOG_GEN_PRINTF("\n");

    rect = Rect();

    if (bSuc) {
        if (!getVirtualDesktopRect(virtualDesktopRect)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        rect.x = virtualDesktopRect.left;
        rect.y = virtualDesktopRect.top;
        rect.width = virtualDesktopRect.Width();
        rect.height = virtualDesktopRect.Height();
    }
}

bool WinGuiISTK::wndGetFgWnd(ScreenInfo &screenInfo)
{
    bool bSuc = true;
    HWND hWnd = NULL;
    HWND hWnd2 = NULL;
    CString sWindowText;

    if (bSuc) {
        hWnd = ::GetForegroundWindow();
        if (hWnd == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        CWnd::FromHandle(hWnd)->GetWindowText(sWindowText);
        screenInfo.title = TK_Tools::tstr2str((LPCTSTR)sWindowText);
    }

    return bSuc;
}

bool WinGuiISTK::wndGetWndAtPoint(ScreenInfo &screenInfo, const Point &point)
{
    bool bSuc = true;
    HWND hWnd = NULL;
    CString sWindowText;
    POINT pt;

    if (bSuc) {
        pt.x = point.x;
        pt.y = point.y;
        hWnd = ::WindowFromPoint(pt);
        if (hWnd == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        CWnd::FromHandle(hWnd)->GetWindowText(sWindowText);
        screenInfo.title = TK_Tools::tstr2str((LPCTSTR)sWindowText);
    }

    return bSuc;
}

bool WinGuiISTK::saveWindowAsPicture(HWND hWnd, const std::string &path, const CRect *rect /*= NULL*/)
{
    bool bSuc = true;
    CLSID encoderClsId;
    CRect windowRect;
    Gdiplus::Bitmap *pBitmap = NULL;
    Gdiplus::Graphics *pGraphics = NULL;

    if (bSuc) {
        if (!getImageGetEncoderFormPath(encoderClsId, path)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (rect != NULL) {
            windowRect = *rect;
        } else {
            if (!::GetWindowRect(hWnd, (LPRECT)windowRect)) {
                bSuc = false;
            }
        }
    }

    if (bSuc) {
        pBitmap = new Gdiplus::Bitmap(windowRect.Width(), windowRect.Height(), PixelFormat32bppARGB);
        if (pBitmap == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pGraphics = new Gdiplus::Graphics(pBitmap);
        if (pGraphics == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        HDC hGraphicsDC = pGraphics->GetHDC();
        if (!::BitBlt(
            hGraphicsDC,
            0, 0, windowRect.Width(), windowRect.Height(),
            ::GetWindowDC(::GetDesktopWindow()),
            windowRect.left, windowRect.top,
            SRCCOPY
        )) {
            bSuc = false;
        }
        pGraphics->ReleaseHDC(hGraphicsDC);
    }

    if (bSuc) {
        if (pBitmap->Save(TK_Tools::str2wstr(path).c_str(), &encoderClsId, NULL) != Gdiplus::Ok) {
            bSuc = false;
        }
    }

    if (pGraphics != NULL) {
        delete pGraphics;
        pGraphics = NULL;
    }

    if (pBitmap != NULL) {
        delete pBitmap;
        pBitmap = NULL;
    }

    return bSuc;
}

bool WinGuiISTK::getImageGetEncoderFormPath(CLSID &encoderClsId, const std::string &path)
{
    bool bSuc = true;
    std::string sExtName;

    if (bSuc) {
        std::string::size_type nPos = path.rfind('.');
        if (nPos != std::string::npos) {
            sExtName = path.substr(nPos + 1);
            sExtName = TK_Tools::LowerCase(sExtName);
        }
    }

    if (bSuc) {
        if (sExtName == "bmp") {
            if (GetEncoderClsid(L"image/bmp", &encoderClsId) < 0) {
                bSuc = false;
            }
        } else if (sExtName == "jpg" || sExtName == "jpeg") {
            if (GetEncoderClsid(L"image/jpeg", &encoderClsId) < 0) {
                bSuc = false;
            }
        } else if (sExtName == "gif") {
            if (GetEncoderClsid(L"image/gif", &encoderClsId) < 0) {
                bSuc = false;
            }
        } else if (sExtName == "tiff" || sExtName == "tif") {
            if (GetEncoderClsid(L"image/tiff", &encoderClsId) < 0) {
                bSuc = false;
            }
        } else if (sExtName == "png") {
            if (GetEncoderClsid(L"image/png", &encoderClsId) < 0) {
                bSuc = false;
            }
        } else {
            bSuc = false;
        }
    }

    return bSuc;
}

bool WinGuiISTK::makeFilePathTemplateByIndex(std::string &pathTemplate, const std::string &path)
{
    bool bSuc = true;
    std::string sPathWithoutDotExt;
    std::string sExtName;

    if (bSuc) {
        std::string::size_type nPos = path.rfind('.');
        if (nPos != std::string::npos) {
            sPathWithoutDotExt = path.substr(0, nPos);
            sExtName = path.substr(nPos + 1);
            pathTemplate = sPathWithoutDotExt + "_%04u." + sExtName;
        } else {
            pathTemplate = path + "_%04u";
        }
    }

    return bSuc;
}

int WinGuiISTK::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   if (format == NULL || pClsid == NULL) {
       return -1;
   }

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

bool WinGuiISTK::cbdPutStr(const std::string &s)
{
    bool bSuc = true;
    std::wstring ws;
    HGLOBAL hClip = NULL;
    wchar_t *pBuff = NULL;

    LOG_GEN_PRINTF("s=\"%s\"\n", s.c_str());

    if (bSuc) {
        if (!::OpenClipboard(NULL)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        ws = TK_Tools::str2wstr(s);
    }

    if (bSuc) {
        ::EmptyClipboard();
        hClip = ::GlobalAlloc(GMEM_MOVEABLE, (ws.length() + 1) * sizeof(wchar_t));
        if (hClip != NULL) {
            pBuff = (wchar_t *)::GlobalLock(hClip);
            if (pBuff != NULL) {
                wcscpy(pBuff, ws.c_str());
            } else {
                bSuc = false;
            }
            ::GlobalUnlock(hClip);
            if (pBuff != NULL) {
                ::SetClipboardData(CF_UNICODETEXT, hClip);
            }
        } else {
            bSuc = false;
        }
        ::CloseClipboard();
    }

    return bSuc;
}

bool WinGuiISTK::cbdGetStr(std::string &s)
{
    bool bSuc = true;
    std::wstring ws;
    HGLOBAL hClip = NULL;
    wchar_t *pBuff = NULL;

    LOG_GEN();

    if (bSuc) {
        if (!::OpenClipboard(NULL)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        hClip = ::GetClipboardData(CF_UNICODETEXT);
        if (hClip != NULL) {
            pBuff = (wchar_t *)::GlobalLock(hClip);
            if (pBuff != NULL) {
                ws = pBuff;
            } else {
                bSuc = false;
            }
            ::GlobalUnlock(hClip);
        } else {
            bSuc = false;
        }
        ::CloseClipboard();
    }

    if (bSuc) {
        s = TK_Tools::wstr2str(ws);
    }

    return bSuc;
}

void WinGuiISTK::kbdKey(unsigned char vk)
{
    LOG_GEN_PRINTF("vk=%u\n", vk);

    SendKeyboardEvent(vk, 0, 0, 0);
    SendKeyboardEvent(vk, 0, KEYEVENTF_KEYUP, 0);
}

void WinGuiISTK::kbdCombKey(std::vector<unsigned char> vks)
{
    int i;

    LOG_GEN_PRINTF("vks=", );

    for (i = 0; i < (int)vks.size(); ++i) {
        if (i > 0) {
            LOG_PRINTF(",");
        }
        LOG_PRINTF("%u(0x%02X)", vks[i], vks[i]);
    }
    LOG_PRINTF("\n");

    for (i = 0; i < (int)vks.size(); ++i) {
        SendKeyboardEvent(vks[i], 0, 0, 0);
    }
    for (i = vks.size() - 1; i >= 0; --i) {
        SendKeyboardEvent(vks[i], 0, KEYEVENTF_KEYUP, 0);
    }
}

void WinGuiISTK::kbdKeyDown(unsigned char vk)
{
    LOG_GEN_PRINTF("vk=%u\n", vk);

    SendKeyboardEvent(vk, 0, 0, 0);
}

void WinGuiISTK::kbdKeyUp(unsigned char vk)
{
    LOG_GEN_PRINTF("vk=%u\n", vk);

    SendKeyboardEvent(vk, 0, KEYEVENTF_KEYUP, 0);
}

void WinGuiISTK::kbdKeyOn(unsigned char vk)
{
    LOG_GEN_PRINTF("vk=%u\n", vk);

    if (!::GetKeyState(vk) & 0x1) {
        SendKeyboardEvent(vk, 0, 0, 0);
        SendKeyboardEvent(vk, 0, KEYEVENTF_KEYUP, 0);
    }
}

void WinGuiISTK::kbdKeyOff(unsigned char vk)
{
    LOG_GEN_PRINTF("vk=%u\n", vk);

    if (::GetKeyState(vk) & 0x1) {
        SendKeyboardEvent(vk, 0, 0, 0);
        SendKeyboardEvent(vk, 0, KEYEVENTF_KEYUP, 0);
    }
}

void WinGuiISTK::kbdChar(char ch)
{
    std::wstring ws;

    LOG_GEN_PRINTF("ch=%u\n", ch);

    ws = TK_Tools::str2wstr(std::string(1, ch));

    int words_num = ws.length();
    KEYBDINPUT *keyinput = new KEYBDINPUT[words_num];
    memset(keyinput, 0, words_num * sizeof(KEYBDINPUT));
    for (int i = 0; i < words_num; i++)
    {
        keyinput[i].wScan = ws[i];
        keyinput[i].dwFlags = KEYEVENTF_UNICODE;
    }
    INPUT input;
    for (int i = 0; i < words_num; i++)
    {
        input.type = INPUT_KEYBOARD;
        input.ki = keyinput[i];
        ::SendInput(1, &input, sizeof(INPUT));
        ::Sleep(50);
    }
    
    delete[] keyinput;
}

void WinGuiISTK::kbdStr(const std::string &s)
{
    std::wstring ws;

    LOG_GEN_PRINTF("s=%s\n", s.c_str());

    ws = TK_Tools::str2wstr(s);

    int words_num = ws.length();
    KEYBDINPUT *keyinput = new KEYBDINPUT[words_num];
    memset(keyinput, 0, words_num * sizeof(KEYBDINPUT));
    for (int i = 0; i < words_num; i++)
    {
        keyinput[i].wScan = ws[i];
        keyinput[i].dwFlags = KEYEVENTF_UNICODE;
    }
    INPUT input;
    for (int i = 0; i < words_num; i++)
    {
        input.type = INPUT_KEYBOARD;
        input.ki = keyinput[i];
        ::SendInput(1, &input, sizeof(INPUT));
        ::Sleep(50);
    }
    
    delete[] keyinput;
}

void WinGuiISTK::mseMove(const Point &point, bool absolute /*= true*/)
{
    LOG_GEN_PRINTF("point=(%d,%d), absolute=%d\n", point.x, point.y, absolute);

    if (absolute) {
        SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);
    } else {
        SendMouseEvent(MOUSEEVENTF_MOVE, point.x, point.y, 0, NULL);
    }
}

void WinGuiISTK::mseClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    SendMouseEvent(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
    SendMouseEvent(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
}

void WinGuiISTK::mseDClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    SendMouseEvent(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
    SendMouseEvent(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);

    SendMouseEvent(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
    SendMouseEvent(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
}

void WinGuiISTK::mseDrag(const Point &srcPoint, const Point &dstPoint)
{
    LOG_GEN_PRINTF("srcPoint=(%d,%d), destPoint=(%d,%d)\n", srcPoint.x, srcPoint.y, dstPoint.x, dstPoint.y);

    SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    SendMouseEvent(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
    SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
    SendMouseEvent(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
}

void WinGuiISTK::mseRClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);
    SendMouseEvent(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
    SendMouseEvent(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
}

void WinGuiISTK::mseDRClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    SendMouseEvent(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
    SendMouseEvent(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);

    SendMouseEvent(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
    SendMouseEvent(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
}

void WinGuiISTK::mseRDrag(const Point &srcPoint, const Point &dstPoint)
{
    LOG_GEN_PRINTF("srcPoint=(%d,%d), destPoint=(%d,%d)\n", srcPoint.x, srcPoint.y, dstPoint.x, dstPoint.y);

    SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    SendMouseEvent(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
    SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
    SendMouseEvent(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
}

void WinGuiISTK::mseScroll(const Point &point, int steps)
{
    LOG_GEN_PRINTF("point=(%d,%d), steps=%d\n", point.x, point.y, steps);

    SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);
    SendMouseEvent(MOUSEEVENTF_WHEEL, 0, 0, steps, NULL);
}

std::string WinGuiISTK::getImagesPaths(const std::vector<Image> &images)
{
    std::string sImagesPaths;
    unsigned int i;

    for (i = 0; i < images.size(); ++i) {
        if (i > 0) {
            sImagesPaths += ',';
        }
        sImagesPaths += images[i].getPath();
    }

    return sImagesPaths;
}

bool WinGuiISTK::imgFindRect(const std::vector<Image> &images, Rect &rect, int &index)
{
    bool bSuc = true;
    std::vector<Rect> rects;

    LOG_GEN_PRINTF("images=\"%s\"\n", 
        getImagesPaths(images).c_str()
    );

    if (bSuc) {
        if (!imgFindRect_impl(images, rects, index, false)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (rects.size() != 1) {
            bSuc = false;
        }
    }

    if (bSuc) {
        rect = rects[0];
    }

    return bSuc;
}

bool WinGuiISTK::imgFindRect_impl(const std::vector<Image> &images, std::vector<Rect> &rects, int &index, bool findAll)
{
    bool bSuc = true;
    CBitmap *pDesktopWindowBitmap = NULL;
    CBitmap *pImageBitmap = NULL;
    Rect searchRectNormalized;
    BITMAP wholeBitmapInfo;
    unsigned int i, j;
    Rect virtualDesktopRect;

    rects.clear();
    index = -1;

    if (bSuc) {
        if (!m_sEnvVarScreenPictureFilePath.empty()) {
            pDesktopWindowBitmap = loadImageAsBitmap(m_sEnvVarScreenPictureFilePath);
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        } else {
            pDesktopWindowBitmap = getVirtualDesktopAsBitmap(virtualDesktopRect);
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        }
    }

    if (bSuc) {
        if (!pDesktopWindowBitmap->GetBitmap(&wholeBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        for (i = 0; i < images.size(); ++i) {
            if (bSuc) {
                pImageBitmap = loadImageAsBitmap(images[i].getPath());
                if (pImageBitmap == NULL) {
                    bSuc = false;
                }
            }

            if (bSuc) {
                searchRectNormalized.x = 0;
                searchRectNormalized.y = 0;
                searchRectNormalized.width = wholeBitmapInfo.bmWidth;
                searchRectNormalized.height = wholeBitmapInfo.bmHeight;
                bSuc = findBitmapInBitmap(rects, searchRectNormalized, pImageBitmap, pDesktopWindowBitmap, findAll);
            }

            if (bSuc) {
                for (j = 0; j < rects.size(); ++j) {
                    rects[j].Offset(virtualDesktopRect.x, virtualDesktopRect.y);
                }
            }

            if (pImageBitmap != NULL) {
                delete pImageBitmap;
                pImageBitmap = NULL;
            }

            if (bSuc) {
                index = i;
                break;
            }

            bSuc = true;
        }
    }

    if (pDesktopWindowBitmap != NULL) {
        delete pDesktopWindowBitmap;
        pDesktopWindowBitmap = NULL;
    }

    if (index == -1) {
        bSuc = false;
    }

    return bSuc;
}

bool WinGuiISTK::imgFindRect(const std::vector<Image> &images, Rect &rect, int &index, const Rect &searchRect)
{
    bool bSuc = true;
    std::vector<Rect> rects;

    LOG_GEN_PRINTF("images=\"%s\", searchRect=(%d,%d,%u,%u)\n", 
        getImagesPaths(images).c_str(), 
        searchRect.x, searchRect.y, searchRect.width, searchRect.height
    );

    if (bSuc) {
        if (!imgFindRect_impl(images, rects, index, searchRect, false)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (rects.size() != 1) {
            bSuc = false;
        }
    }
    
    if (bSuc) {
        rect = rects[0];
    }

    return bSuc;
}

bool WinGuiISTK::imgFindRect_impl(const std::vector<Image> &images, std::vector<Rect> &rects, int &index, const Rect &searchRect, bool findAll)
{
    bool bSuc = true;
    CBitmap *pDesktopWindowBitmap = NULL;
    CBitmap *pImageBitmap = NULL;
    BITMAP wholeBitmapInfo;
    Rect searchRectNormalized = searchRect;
    unsigned int i, j;
    Rect virtualDesktopRect;

    rects.clear();
    index = -1;

    if (bSuc) {
        if (!m_sEnvVarScreenPictureFilePath.empty()) {
            pDesktopWindowBitmap = loadImageAsBitmap(m_sEnvVarScreenPictureFilePath);
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        } else {
            pDesktopWindowBitmap = getVirtualDesktopAsBitmap(virtualDesktopRect);
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        }
    }

    if (bSuc) {
        if (!pDesktopWindowBitmap->GetBitmap(&wholeBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        for (i = 0; i < images.size(); ++i) {
            if (bSuc) {
                pImageBitmap = loadImageAsBitmap(images[i].getPath());
                if (pImageBitmap == NULL) {
                    bSuc = false;
                }
            }

            if (bSuc) {
                searchRectNormalized.Intersect(Rect(0, 0, wholeBitmapInfo.bmWidth, wholeBitmapInfo.bmHeight));
                bSuc = findBitmapInBitmap(rects, searchRectNormalized, pImageBitmap, pDesktopWindowBitmap, findAll);
            }

            if (bSuc) {
                for (j = 0; j < rects.size(); ++j) {
                    rects[j].Offset(virtualDesktopRect.x, virtualDesktopRect.y);
                }
            }

            if (pImageBitmap != NULL) {
                delete pImageBitmap;
                pImageBitmap = NULL;
            }

            if (bSuc) {
                index = i;
                break;
            }

            bSuc = true;
        }
    }

    if (pDesktopWindowBitmap != NULL) {
        delete pDesktopWindowBitmap;
        pDesktopWindowBitmap = NULL;
    }

    if (index == -1) {
        bSuc = false;
    }

    return bSuc;
}

bool WinGuiISTK::imgFindRect(const std::vector<Image> &images, Rect &rect, int &index, const Point &searchBeginningPoint)
{
    bool bSuc = true;
    std::vector<Rect> rects;

    LOG_GEN_PRINTF("images=\"%s\", searchBeginningPoint=(%d,%d)\n", 
        getImagesPaths(images).c_str(), 
        searchBeginningPoint.x, searchBeginningPoint.y
    );

    if (bSuc) {
        if (!imgFindRect_impl(images, rects, index, searchBeginningPoint, false)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (rects.size() != 1) {
            bSuc = false;
        }
    }
    
    if (bSuc) {
        rect = rects[0];
    }

    return bSuc;
}

bool WinGuiISTK::imgFindRect_impl(const std::vector<Image> &images, std::vector<Rect> &rects, int &index, const Point &searchBeginningPoint, bool findAll)
{
    bool bSuc = true;
    CBitmap *pDesktopWindowBitmap = NULL;
    CBitmap *pImageBitmap = NULL;
    Rect searchRectNormalized;
    BITMAP wholeBitmapInfo;
    unsigned int i, j;
    Rect virtualDesktopRect;

    rects.clear();
    index = -1;

    if (bSuc) {
        if (!m_sEnvVarScreenPictureFilePath.empty()) {
            pDesktopWindowBitmap = loadImageAsBitmap(m_sEnvVarScreenPictureFilePath);
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        } else {
            pDesktopWindowBitmap = getVirtualDesktopAsBitmap(virtualDesktopRect);
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        }
    }

    if (bSuc) {
        if (!pDesktopWindowBitmap->GetBitmap(&wholeBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        for (i = 0; i < images.size(); ++i) {
            if (bSuc) {
                pImageBitmap = loadImageAsBitmap(images[i].getPath());
                if (pImageBitmap == NULL) {
                    bSuc = false;
                }
            }

            if (bSuc) {
                searchRectNormalized.x = searchBeginningPoint.x;
                searchRectNormalized.y = searchBeginningPoint.y;
                searchRectNormalized.width = wholeBitmapInfo.bmWidth;
                searchRectNormalized.height = wholeBitmapInfo.bmHeight;
                searchRectNormalized.Intersect(Rect(0, 0, wholeBitmapInfo.bmWidth, wholeBitmapInfo.bmHeight));
                bSuc = findBitmapInBitmap(rects, searchRectNormalized, pImageBitmap, pDesktopWindowBitmap, findAll);
            }

            if (bSuc) {
                for (j = 0; j < rects.size(); ++j) {
                    rects[j].Offset(virtualDesktopRect.x, virtualDesktopRect.y);
                }
            }

            if (pImageBitmap != NULL) {
                delete pImageBitmap;
                pImageBitmap = NULL;
            }

            if (bSuc) {
                index = i;
                break;
            }

            bSuc = true;
        }
    }

    if (pDesktopWindowBitmap != NULL) {
        delete pDesktopWindowBitmap;
        pDesktopWindowBitmap = NULL;
    }

    if (index == -1) {
        bSuc = false;
    }

    return bSuc;
}

bool WinGuiISTK::imgFindAllRects(const Image &image, std::vector<Rect> &rects)
{
    bool bSuc = true;
    std::vector<Image> images;
    int index;

    LOG_GEN_PRINTF("image=\"%s\"\n", 
        image.getPath().c_str()
    );

    if (bSuc) {
        images.push_back(image);
    }

    if (bSuc) {
        if (!imgFindRect_impl(images, rects, index, true)) {
            bSuc = false;
        }
    }

    return bSuc;
}

bool WinGuiISTK::imgFindAllRects(const Image &image, std::vector<Rect> &rects, const Rect &searchRect)
{
    bool bSuc = true;
    std::vector<Image> images;
    int index;

    LOG_GEN_PRINTF("image=\"%s\"\n", 
        image.getPath().c_str()
    );

    if (bSuc) {
        images.push_back(image);
    }

    if (bSuc) {
        if (!imgFindRect_impl(images, rects, index, searchRect, true)) {
            bSuc = false;
        }
    }

    return bSuc;
}

bool WinGuiISTK::imgFindAllRects(const Image &image, std::vector<Rect> &rects, const Point &searchBeginningPoint)
{
    bool bSuc = true;
    std::vector<Image> images;
    int index;

    LOG_GEN_PRINTF("image=\"%s\"\n", 
        image.getPath().c_str()
    );

    if (bSuc) {
        images.push_back(image);
    }

    if (bSuc) {
        if (!imgFindRect_impl(images, rects, index, searchBeginningPoint, true)) {
            bSuc = false;
        }
    }

    return bSuc;
}

bool WinGuiISTK::imgWaitShown(const std::vector<Image> &images, Rect &rect, int &index, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;
    std::vector<Rect> rects;

    LOG_GEN_PRINTF("images=\"%s\", timeout=%u\n", 
        getImagesPaths(images).c_str(), 
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            bSuc = false;
            break;
        }

        bSuc = imgFindRect_impl(images, rects, index, false);
        if (bSuc) {
            if (rects.size() == 1) {
                rect = rects[0];
                break;
            }
        }

        elapsedTime += WAIT_IMAGE_SHOWN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    return bSuc;
}

bool WinGuiISTK::imgWaitShown(const std::vector<Image> &images, Rect &rect, int &index, const Rect &searchRect, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;
    std::vector<Rect> rects;

    LOG_GEN_PRINTF("images=\"%s\", searchRect=(%d,%d,%u,%u), timeout=%u\n", 
        getImagesPaths(images).c_str(), 
        searchRect.x, searchRect.y, searchRect.width, searchRect.height,
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            bSuc = false;
            break;
        }

        bSuc = imgFindRect_impl(images, rects, index, searchRect, false);
        if (bSuc) {
            if (rects.size() == 1) {
                rect = rects[0];
                break;
            }
        }

        elapsedTime += WAIT_IMAGE_SHOWN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    return bSuc;
}

bool WinGuiISTK::imgWaitShown(const std::vector<Image> &images, Rect &rect, int &index, const Point &searchBeginningPoint, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;
    std::vector<Rect> rects;

    LOG_GEN_PRINTF("images=\"%s\", searchBeginningPoint=(%d,%d), timeout=%u\n", 
        getImagesPaths(images).c_str(), 
        searchBeginningPoint.x, searchBeginningPoint.y, 
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            bSuc = false;
            break;
        }

        bSuc = imgFindRect_impl(images, rects, index, searchBeginningPoint, false);
        if (bSuc) {
            if (rects.size() == 1) {
                rect = rects[0];
                break;
            }
        }

        elapsedTime += WAIT_IMAGE_SHOWN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    return bSuc;
}

bool WinGuiISTK::imgWaitHidden(const std::vector<Image> &images, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;
    std::vector<Rect> rects;
    int index;

    LOG_GEN_PRINTF("images=\"%s\", timeout=%u\n", 
        getImagesPaths(images).c_str(), 
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            bSuc = false;
            break;
        }

        bSuc = imgFindRect_impl(images, rects, index, false);
        if (!bSuc) {
            bSuc = true;
            break;
        }

        elapsedTime += WAIT_IMAGE_HIDDEN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    return bSuc;
}

bool WinGuiISTK::imgWaitHidden(const std::vector<Image> &images, const Rect &searchRect, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;
    std::vector<Rect> rects;
    int index;

    LOG_GEN_PRINTF("images=\"%s\", searchRect=(%d,%d,%u,%u), timeout=%u\n", 
        getImagesPaths(images).c_str(), 
        searchRect.x, searchRect.y, searchRect.width, searchRect.height,
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            bSuc = false;
            break;
        }

        bSuc = imgFindRect_impl(images, rects, index, searchRect, false);
        if (bSuc) {
            bSuc = true;
            break;
        }

        elapsedTime += WAIT_IMAGE_HIDDEN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    return bSuc;
}

bool WinGuiISTK::imgWaitHidden(const std::vector<Image> &images, const Point &searchBeginningPoint, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;
    std::vector<Rect> rects;
    int index;

    LOG_GEN_PRINTF("images=\"%s\", searchBeginningPoint=(%d,%d), timeout=%u\n", 
        getImagesPaths(images).c_str(), 
        searchBeginningPoint.x, searchBeginningPoint.y, 
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            bSuc = false;
            break;
        }

        bSuc = imgFindRect_impl(images, rects, index, searchBeginningPoint, false);
        if (bSuc) {
            bSuc = true;
            break;
        }

        elapsedTime += WAIT_IMAGE_HIDDEN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    return bSuc;
}

bool WinGuiISTK::imgWaitAllShown(const std::vector<Image> &images, std::vector<Rect> &rects, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;
    std::vector<Image> imagesTmp;
    std::vector<Rect> rectsTmp;
    std::vector<Rect> rectsResult;
    int index;
    unsigned int i;

    LOG_GEN_PRINTF("images=\"%s\", timeout=%u\n", 
        getImagesPaths(images).c_str(), 
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            bSuc = false;
            break;
        }

        rectsResult.clear();
        for (i = 0; i < images.size(); ++i) {
            imagesTmp.clear();
            imagesTmp.push_back(images[i]);
            rectsTmp.clear();
            bSuc = imgFindRect_impl(imagesTmp, rectsTmp, index, false);
            if (!bSuc || rectsTmp.size() != 1) {
                break;
            } else {
                rectsResult.push_back(rectsTmp[0]);
            }
        }
        if (rectsResult.size() == images.size()) {
            bSuc = true;
            break;
        }

        elapsedTime += WAIT_IMAGE_SHOWN_INTERVAL;
        ::Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    rects = rectsResult;

    return bSuc;
}

bool WinGuiISTK::imgWaitAllShown(const std::vector<Image> &images, std::vector<Rect> &rects, const Rect &searchRect, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;
    std::vector<Image> imagesTmp;
    std::vector<Rect> rectsTmp;
    std::vector<Rect> rectsResult;
    int index;
    unsigned int i;

    LOG_GEN_PRINTF("images=\"%s\", searchRect=(%d,%d,%u,%u), timeout=%u\n", 
        getImagesPaths(images).c_str(), 
        searchRect.x, searchRect.y, searchRect.width, searchRect.height,
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            bSuc = false;
            break;
        }

        rectsResult.clear();
        for (i = 0; i < images.size(); ++i) {
            imagesTmp.clear();
            imagesTmp.push_back(images[i]);
            rectsTmp.clear();
            bSuc = imgFindRect_impl(imagesTmp, rectsTmp, index, searchRect, false);
            if (!bSuc || rectsTmp.size() != 1) {
                break;
            } else {
                rectsResult.push_back(rectsTmp[0]);
            }
        }
        if (rectsResult.size() == images.size()) {
            bSuc = true;
            break;
        }

        elapsedTime += WAIT_IMAGE_SHOWN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    rects = rectsResult;

    return bSuc;
}

bool WinGuiISTK::imgWaitAllShown(const std::vector<Image> &images, std::vector<Rect> &rects, const Point &searchBeginningPoint, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;
    std::vector<Image> imagesTmp;
    std::vector<Rect> rectsTmp;
    std::vector<Rect> rectsResult;
    int index;
    unsigned int i;

    LOG_GEN_PRINTF("images=\"%s\", searchBeginningPoint=(%d,%d), timeout=%u\n", 
        getImagesPaths(images).c_str(), 
        searchBeginningPoint.x, searchBeginningPoint.y, 
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            bSuc = false;
            break;
        }

        rectsResult.clear();
        for (i = 0; i < images.size(); ++i) {
            imagesTmp.clear();
            imagesTmp.push_back(images[i]);
            rectsTmp.clear();
            bSuc = imgFindRect_impl(imagesTmp, rectsTmp, index, searchBeginningPoint, false);
            if (!bSuc || rectsTmp.size() != 1) {
                break;
            } else {
                rectsResult.push_back(rectsTmp[0]);
            }
        }
        if (rectsResult.size() == images.size()) {
            bSuc = true;
            break;
        }

        elapsedTime += WAIT_IMAGE_SHOWN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    rects = rectsResult;

    return bSuc;
}

DWORD WinGuiISTK::sx(DWORD x)
{
    DWORD result = 0;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);

    if (screenWidth > 0) {
        result = x * 65535 / screenWidth;
    }

    return result;
}

DWORD WinGuiISTK::sy(DWORD y)
{
    DWORD result = 0;
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (screenHeight > 0) {
        result = y * 65535 / screenHeight;
    }

    return result;
}

CBitmap *WinGuiISTK::getPrimaryDesktopAsBitmap()
{
    bool bSuc = true;
    CBitmap *pBitmap = NULL;
    CWnd* pDesktopWnd = NULL;
    CDC *pDesktopDC = NULL;
    CRect desktopWndRect;
    CDC bitmapDC;
    CBitmap* pOldBitmap = NULL;

    if (bSuc) {
        pDesktopWnd = CWnd::GetDesktopWindow();
        if (pDesktopWnd == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pDesktopWnd->GetWindowRect(&desktopWndRect);
    }

    if (bSuc) {
        pDesktopDC = pDesktopWnd->GetWindowDC();
        if (pDesktopDC == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pBitmap = new CBitmap();
        if (pBitmap == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!bitmapDC.CreateCompatibleDC(pDesktopDC)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!pBitmap->CreateCompatibleBitmap(pDesktopDC, desktopWndRect.Width(), desktopWndRect.Height())) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pOldBitmap = bitmapDC.SelectObject(pBitmap);
        if (!bitmapDC.BitBlt(0, 0, desktopWndRect.Width(), desktopWndRect.Height(), pDesktopDC, 0, 0, SRCCOPY)) {
            bSuc = false;
        }
        bitmapDC.SelectObject(pOldBitmap);
    }

    if (pDesktopWnd != NULL && pDesktopDC != NULL) {
        pDesktopWnd->ReleaseDC(pDesktopDC);
        pDesktopDC = NULL;
    }

    if (!bSuc) {
        delete pBitmap;
        pBitmap = NULL;
    }

    return pBitmap;
}

CBitmap *WinGuiISTK::getVirtualDesktopAsBitmap(Rect &virtualDesktopRect)
{
    bool bSuc = true;
    CBitmap *pBitmap = NULL;
    CWnd* pDesktopWnd = NULL;
    CDC *pDesktopDC = NULL;
    CRect desktopWndRect;
    CDC bitmapDC;
    CBitmap* pOldBitmap = NULL;

    if (bSuc) {
        pDesktopWnd = CWnd::GetDesktopWindow();
        if (pDesktopWnd == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!getVirtualDesktopRect(desktopWndRect)) {
            bSuc = false;
        } else {
            virtualDesktopRect.x = desktopWndRect.left;
            virtualDesktopRect.y = desktopWndRect.top;
            virtualDesktopRect.width = desktopWndRect.Width();
            virtualDesktopRect.height = desktopWndRect.Height();
        }
    }

    if (bSuc) {
        pDesktopDC = pDesktopWnd->GetWindowDC();
        if (pDesktopDC == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pBitmap = new CBitmap();
        if (pBitmap == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!bitmapDC.CreateCompatibleDC(pDesktopDC)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!pBitmap->CreateCompatibleBitmap(pDesktopDC, desktopWndRect.Width(), desktopWndRect.Height())) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pOldBitmap = bitmapDC.SelectObject(pBitmap);
        if (!bitmapDC.BitBlt(0, 0, desktopWndRect.Width(), desktopWndRect.Height(), pDesktopDC, desktopWndRect.left, desktopWndRect.top, SRCCOPY)) {
            bSuc = false;
        }
        bitmapDC.SelectObject(pOldBitmap);
    }

    if (pDesktopWnd != NULL && pDesktopDC != NULL) {
        pDesktopWnd->ReleaseDC(pDesktopDC);
        pDesktopDC = NULL;
    }

    if (!bSuc) {
        delete pBitmap;
        pBitmap = NULL;
    }

    return pBitmap;
}

bool WinGuiISTK::getVirtualDesktopRect(CRect &virtualDesktopRect)
{
    bool bSuc = true;

    if (bSuc) {
        virtualDesktopRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        virtualDesktopRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        virtualDesktopRect.right = virtualDesktopRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        virtualDesktopRect.bottom = virtualDesktopRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }

    return bSuc;
}

bool WinGuiISTK::getDesktopRects(std::vector<Rect> &rects)
{
    bool bSuc = true;

    if (bSuc) {
        Arguments_getDesktopRects_MonitorEnumProc arguments = { this, rects };
        if (!::EnumDisplayMonitors(NULL, NULL, &getDesktopRects_MonitorEnumProc, (LPARAM)&arguments)) {
            bSuc = false;
        }
    }

    return bSuc;
}

BOOL CALLBACK WinGuiISTK::getDesktopRects_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    BOOL bSuc = TRUE;
    Arguments_getDesktopRects_MonitorEnumProc &arguments = *(Arguments_getDesktopRects_MonitorEnumProc *)dwData;
    MONITORINFOEX mi;

    memset(&mi, 0, sizeof(MONITORINFOEX));

    if (bSuc) {
        mi.cbSize = sizeof(MONITORINFOEX);
        if (!::GetMonitorInfo(hMonitor, &mi)) {
            bSuc = FALSE;
        }
    }

    if (bSuc) {
        CRect rectTmp(mi.rcMonitor);
        Rect rect(rectTmp.left, rectTmp.top, rectTmp.Width(), rectTmp.Height());
        arguments.rects.push_back(rect);
    }

    return bSuc;
}

CBitmap *WinGuiISTK::loadImageAsBitmap(const std::string &imageFilePath)
{
    bool bSuc = true;
    CBitmap *pBitmap = NULL;
    Gdiplus::Image *pImage = NULL;
    CWnd* pDesktopWnd = NULL;
    CDC *pDesktopDC = NULL;
    CDC bitmapDC;
    CBitmap* pOldBitmap = NULL;
    Gdiplus::Graphics *pGraphics = NULL;

    if (bSuc) {
        pImage = Gdiplus::Image::FromFile(TK_Tools::str2wstr(imageFilePath).c_str(), FALSE);
        if (pImage == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pDesktopWnd = CWnd::GetDesktopWindow();
        if (pDesktopWnd == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pDesktopDC = pDesktopWnd->GetWindowDC();
        if (pDesktopDC == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pBitmap = new CBitmap();
        if (pBitmap == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!bitmapDC.CreateCompatibleDC(pDesktopDC)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!pBitmap->CreateCompatibleBitmap(pDesktopDC, pImage->GetWidth(), pImage->GetHeight())) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pOldBitmap = bitmapDC.SelectObject(pBitmap);
        pGraphics = new Gdiplus::Graphics(bitmapDC.GetSafeHdc());
        if (pGraphics == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pGraphics->DrawImage(pImage, 0, 0, pImage->GetWidth(), pImage->GetHeight());
        bitmapDC.SelectObject(pOldBitmap);
    }

    if (pGraphics != NULL) {
        delete pGraphics;
        pGraphics = NULL;
    }

    if (pDesktopWnd != NULL && pDesktopDC != NULL) {
        pDesktopWnd->ReleaseDC(pDesktopDC);
        pDesktopDC = NULL;
    }

    if (pImage != NULL) {
        delete pImage;
        pImage = NULL;
    }

    if (!bSuc) {
        delete pBitmap;
        pBitmap = NULL;
    }

    return pBitmap;
}

bool WinGuiISTK::fixBitmapAlphaBits(const BITMAP &bitmapInfo)
{
    bool bSuc = true;
    RGBQUAD *pColor;
    int x, y;

    if (bSuc) {
        if (bitmapInfo.bmBitsPixel != 32) {
            return true;
        }
    }

    if (bSuc) {
        if (bitmapInfo.bmBits == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        pColor = (RGBQUAD *)bitmapInfo.bmBits;
        for (y = 0; y < bitmapInfo.bmHeight; ++y) {
            for (x = 0; x < bitmapInfo.bmWidth; ++x) {
                pColor->rgbReserved = 0xFF;
                pColor++;
            }
        }
    }

    return bSuc;
}

bool WinGuiISTK::findBitmapInBitmap(std::vector<Rect> &matchedRects, const Rect& searchRect, CBitmap *partBitmap, CBitmap *wholeBitmap, bool findAll)
{
    bool bSuc = true;
    BITMAP partBitmapInfo;
    BITMAP wholeBitmapInfo;
    Rect searchRectNormalized = searchRect;

    if (bSuc) {
        if (partBitmap == NULL || wholeBitmap == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!partBitmap->GetBitmap(&partBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        partBitmapInfo.bmBits = new DWORD [partBitmapInfo.bmWidth * partBitmapInfo.bmHeight];
        if (partBitmapInfo.bmBits == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        partBitmap->GetBitmapBits(partBitmapInfo.bmWidth * partBitmapInfo.bmHeight * 4, partBitmapInfo.bmBits);
        if (!fixBitmapAlphaBits(partBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!wholeBitmap->GetBitmap(&wholeBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        wholeBitmapInfo.bmBits = new DWORD [wholeBitmapInfo.bmWidth * wholeBitmapInfo.bmHeight];
        if (wholeBitmapInfo.bmBits == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        wholeBitmap->GetBitmapBits(wholeBitmapInfo.bmWidth * wholeBitmapInfo.bmHeight * 4, wholeBitmapInfo.bmBits);
        if (!fixBitmapAlphaBits(wholeBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        searchRectNormalized.Intersect(Rect(0, 0, wholeBitmapInfo.bmWidth, wholeBitmapInfo.bmHeight));
        if (searchRectNormalized.width == 0 || searchRectNormalized.height == 0) {
            bSuc = false;
        }
    }

    if (bSuc) {
        bSuc = findBitmapInBitmap_unsafe(matchedRects, searchRectNormalized, partBitmapInfo, wholeBitmapInfo, findAll);
    } 
    
    if (partBitmapInfo.bmBits != NULL) {
        delete [] (DWORD *)partBitmapInfo.bmBits;
        partBitmapInfo.bmBits = NULL;
    }

    if (wholeBitmapInfo.bmBits != NULL) {
        delete [] (DWORD *)wholeBitmapInfo.bmBits;
        wholeBitmapInfo.bmBits = NULL;
    }

    return bSuc;
}

bool WinGuiISTK::findBitmapInBitmap_unsafe(std::vector<Rect> &matchedRects, const Rect& searchRect, const BITMAP &partBitmapInfo, const BITMAP &wholeBitmapInfo, bool findAll)
{
    bool bSuc = true;

    if (bSuc) {
        if (partBitmapInfo.bmBitsPixel == 32 && wholeBitmapInfo.bmBitsPixel == 32) {
            bSuc = findBitmapInBitmap_bytes_unsafe<4>(matchedRects, searchRect, partBitmapInfo, wholeBitmapInfo, findAll);
        } else if (partBitmapInfo.bmBitsPixel == 24 && wholeBitmapInfo.bmBitsPixel == 24) {
            bSuc = findBitmapInBitmap_bytes_unsafe<3>(matchedRects, searchRect, partBitmapInfo, wholeBitmapInfo, findAll);
        } else if (partBitmapInfo.bmBitsPixel == 16 && wholeBitmapInfo.bmBitsPixel == 16) {
            bSuc = findBitmapInBitmap_bytes_unsafe<2>(matchedRects, searchRect, partBitmapInfo, wholeBitmapInfo, findAll);
        } else {
            bSuc = false;
        }
    }

    return bSuc;
}

template <unsigned int BYTES_PER_PIXEL>
bool WinGuiISTK::findBitmapInBitmap_bytes_unsafe(std::vector<Rect> &matchedRects, const Rect& searchRect, const BITMAP &partBitmapInfo, const BITMAP &wholeBitmapInfo, bool findAll)
{
    bool bSuc = false;
    unsigned int bytesPerPixel = BYTES_PER_PIXEL;
    unsigned int partBitmapBytesPerLine = bytesPerPixel * partBitmapInfo.bmWidth;
    unsigned int x, y;
    unsigned char i;
    unsigned char *pSrc, *pDst;
    Rect matchedRect;

    matchedRects.clear();

    for (y = searchRect.y; y <= searchRect.y + searchRect.height - partBitmapInfo.bmHeight; ++y) {
        for (x = searchRect.x; x <= searchRect.x + searchRect.width - partBitmapInfo.bmWidth; ++x) {
            pSrc = (unsigned char *)partBitmapInfo.bmBits;
            pDst = (unsigned char *)wholeBitmapInfo.bmBits + wholeBitmapInfo.bmWidthBytes * y + bytesPerPixel * x;
            if (memcmp(pDst, pSrc, partBitmapBytesPerLine) != 0) {
                continue;
            } else {
                bool matched = true;
                for (i = 1; i < partBitmapInfo.bmHeight; ++i) {
                    pSrc += partBitmapInfo.bmWidthBytes;
                    pDst += wholeBitmapInfo.bmWidthBytes;
                    if (memcmp(pDst, pSrc, partBitmapBytesPerLine) != 0) {
                        matched = false;
                        break;
                    }
                }
                if (!matched) {
                    continue;
                } else {
                    bSuc = true;
                    matchedRect.x = x;
                    matchedRect.y = y;
                    matchedRect.width = partBitmapInfo.bmWidth;
                    matchedRect.height = partBitmapInfo.bmHeight;
                    matchedRects.push_back(matchedRect);

                    if (!findAll) {
                        goto END;
                    }
                }
            }
        }
    }

END:
    return bSuc;
}

} // namespace GuiISTk {
