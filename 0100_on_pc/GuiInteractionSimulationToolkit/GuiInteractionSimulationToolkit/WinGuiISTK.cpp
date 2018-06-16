#include "stdafx.h"
#include "Psapi.h"
#include "TK_Tools.h"
#include "WinGuiISTK.h"

#define THIS_PROG_BIN_NAME_DEF              L"guiistk"

namespace GuiISTk {

const unsigned int WAIT_IMAGE_SHOWN_INTERVAL = 500;

VOID local_mouse_event(
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

void Rect::intersect(const Rect &other)
{
    CRect rect1(x, y, x + width, y + height);
    CRect rect2(other.x, other.y, other.x + other.width, other.y + other.height);
    CRect resultRect;
    resultRect.IntersectRect(&rect1, &rect2);

    x = resultRect.left;
    y = resultRect.top;
    width = resultRect.Width();
    height = resultRect.Height();
}

WinGuiISTK::WinGuiISTK() : m_pPartBitmapMem(NULL), m_nPartBitmapMemSize(0), m_pWholeBitmapMem(NULL), m_nWholeBitmapMemSize(0), m_sEnvVarScreenPictureFilePath()
{
    TCHAR szProcessName[MAX_PATH] = {0};

    m_nPartBitmapMemSize = 1920 * 1080 * 4 * 4;
    m_nWholeBitmapMemSize = 1920 * 1080 * 4 * 4;

    m_pPartBitmapMem = new DWORD[m_nPartBitmapMemSize / 4];
    m_pWholeBitmapMem = new DWORD[m_nWholeBitmapMemSize / 4];

    const char *pEnvVarScreenPictureFilePath = getenv("DESKTOP_PICTURE_FILE_PATH");
    m_sEnvVarScreenPictureFilePath = pEnvVarScreenPictureFilePath == NULL ? "" : pEnvVarScreenPictureFilePath;

    ::GetModuleBaseName(::GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));
    m_sExeFileName = szProcessName;
    int nPos = m_sExeFileName.ReverseFind(L'.');
    if (nPos != -1) {
        m_sExeFileName = m_sExeFileName.Left(nPos);
    }

    if (m_sExeFileName.IsEmpty()) {
        m_sExeFileName = THIS_PROG_BIN_NAME_DEF;
    }
}

WinGuiISTK::~WinGuiISTK()
{
    delete [] m_pPartBitmapMem;
    delete [] m_pWholeBitmapMem;
}

void WinGuiISTK::delay(unsigned int milliSecond)
{
    LOG_GEN_PRINTF("milliSecond=%u\n", milliSecond);

    Sleep(milliSecond);
}

BOOL CALLBACK WinGuiISTK::getMatchedWindows_EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    Arguments_getMatchedWindows_EnumWindowsProc &arguments = *(Arguments_getMatchedWindows_EnumWindowsProc *)lParam;
    WinGuiISTK *thiz = arguments.thiz;
    CString sTitle(TK_Tools::str2wstr(arguments.screenInfo.title).c_str());
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
        if (sWindowText.Find(sTitle) != -1) {
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

        LOG_GEN_PRINTF("sWindowText=\"%s\", exe=\"%s\"\n", 
            TK_Tools::wstr2str(sWindowText.GetString()).c_str(),
            TK_Tools::wstr2str(szProcessName).c_str()
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

unsigned int WinGuiISTK::scnGetCount(const ScreenInfo &screenInfo)
{
    unsigned int nScreenCount = 0;
    std::vector<HWND> winHandles;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched);

    getMatchedWindows(winHandles, screenInfo);
    nScreenCount = winHandles.size();

    return nScreenCount;
}

bool WinGuiISTK::scnShow(const ScreenInfo &screenInfo, ScreenShowingMode mode)
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
                }
                ::SetForegroundWindow(hWnd);
                Sleep(500);
                break;
            default:
                ::ShowWindow(hWnd, SW_SHOWNORMAL);
                break;
            }
        }
    }

    return bSuc;
}

bool WinGuiISTK::scnHide(const ScreenInfo &screenInfo)
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

bool WinGuiISTK::scnClose(const ScreenInfo &screenInfo)
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
            ::CloseWindow(hWnd);
        }
    }

    return bSuc;
}

bool WinGuiISTK::scnMove(const ScreenInfo &screenInfo, const Point &point)
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

bool WinGuiISTK::scnResize(const ScreenInfo &screenInfo, const Rect &rect)
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

bool WinGuiISTK::scnSetZorder(const ScreenInfo &screenInfo, ScreenZorder zorder)
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

bool WinGuiISTK::scnSaveAsPictures(const ScreenInfo &screenInfo, const std::string &pictureFilePath)
{
    bool bSuc = true;
    std::string pictureFilePathTemplate;
    std::vector<HWND> winHandles;
    HWND hWnd;
    WINDOWPLACEMENT wndpl;
    unsigned int i;

    LOG_GEN_PRINTF("title=\"%s\", fullMatched=%d, allMatched=%d, pictureFilePath=\"%s\"\n", screenInfo.title.c_str(), (int)screenInfo.fullMatched, (int)screenInfo.allMatched, pictureFilePath.c_str());

    if (bSuc) {
        bSuc = getMatchedWindows(winHandles, screenInfo);
    }

    if (bSuc) {
        if (!makeFilePathTemplateByIndex(pictureFilePathTemplate, pictureFilePath)) {
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
            }
            ::SetForegroundWindow(hWnd);
            ::Sleep(500);

            std::string path = pictureFilePath;
            if (winHandles.size() > 1) {
                path = TK_Tools::FormatStr(pictureFilePathTemplate.c_str(), i + 1);
            }
            if (!saveWindowAsPicture(hWnd, path)) {
                bSuc = false;
                break;
            }
        }
    }

    return bSuc;
}

bool WinGuiISTK::scnSaveDesktopAsPicture(const std::string &pictureFilePath)
{
    bool bSuc = true;
    HWND hWnd;

    LOG_GEN_PRINTF("pictureFilePath=\"%s\"\n", pictureFilePath.c_str());

    if (bSuc) {
        hWnd = ::GetDesktopWindow();
        if (!saveWindowAsPicture(hWnd, pictureFilePath)) {
            bSuc = false;
        }
    }

    return bSuc;
}

bool WinGuiISTK::saveWindowAsPicture(HWND hWnd, const std::string &path)
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
        if (!::GetWindowRect(hWnd, (LPRECT)windowRect)) {
            bSuc = false;
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

bool WinGuiISTK::cbdPutString(const std::string &s)
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

bool WinGuiISTK::cbdGetString(std::string &s)
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

void WinGuiISTK::kbdKeyDown(unsigned char vk)
{
    LOG_GEN_PRINTF("vk=%u\n", vk);

    keybd_event(vk,0,0,0);
}

void WinGuiISTK::kbdKeyUp(unsigned char vk)
{
    LOG_GEN_PRINTF("vk=%u\n", vk);

    keybd_event(vk,0,KEYEVENTF_KEYUP,0);
}

void WinGuiISTK::kbdKeyOn(unsigned char vk)
{
    LOG_GEN_PRINTF("vk=%u\n", vk);

    if (!GetKeyState(vk) & 0x1) {
        keybd_event(vk,0,0,0);
        keybd_event(vk,0,KEYEVENTF_KEYUP,0);
    }
}

void WinGuiISTK::kbdKeyOff(unsigned char vk)
{
    LOG_GEN_PRINTF("vk=%u\n", vk);

    if (GetKeyState(vk) & 0x1) {
        keybd_event(vk,0,0,0);
        keybd_event(vk,0,KEYEVENTF_KEYUP,0);
    }
}

void WinGuiISTK::kbdCtrlA()
{
    LOG_GEN();

    keybd_event(VK_CONTROL,0,0,0);
    keybd_event('A',0,0,0);
    keybd_event('A',0,KEYEVENTF_KEYUP,0);
    keybd_event(VK_CONTROL,0,KEYEVENTF_KEYUP,0);
}

void WinGuiISTK::kbdCtrlC()
{
    LOG_GEN();

    keybd_event(VK_CONTROL,0,0,0);
    keybd_event('C',0,0,0);
    keybd_event('C',0,KEYEVENTF_KEYUP,0);
    keybd_event(VK_CONTROL,0,KEYEVENTF_KEYUP,0);
}

void WinGuiISTK::kbdCtrlX()
{
    LOG_GEN();

    keybd_event(VK_CONTROL,0,0,0);
    keybd_event('X',0,0,0);
    keybd_event('X',0,KEYEVENTF_KEYUP,0);
    keybd_event(VK_CONTROL,0,KEYEVENTF_KEYUP,0);
}

void WinGuiISTK::kbdCtrlV()
{
    LOG_GEN();

    keybd_event(VK_CONTROL,0,0,0);
    keybd_event('V',0,0,0);
    keybd_event('V',0,KEYEVENTF_KEYUP,0);
    keybd_event(VK_CONTROL,0,KEYEVENTF_KEYUP,0);
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
        Sleep(50);
    }
    
    delete[] keyinput;
}

void WinGuiISTK::kbdString(const std::string &s)
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
        Sleep(50);
    }
    
    delete[] keyinput;
}

void WinGuiISTK::mouseMove(const Point &point, bool absolute /*= true*/)
{
    LOG_GEN_PRINTF("point=(%d,%d), absolute=%d\n", point.x, point.y, absolute);

    if (absolute) {
        local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);
    } else {
        local_mouse_event(MOUSEEVENTF_MOVE, point.x, point.y, 0, NULL);
    }
}

void WinGuiISTK::mouseClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(point.x), sy(point.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::moustDoubleClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(point.x), sy(point.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(point.x), sy(point.y), 0, NULL);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(point.x), sy(point.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::mouseDrag(const Point &srcPoint, const Point &dstPoint)
{
    LOG_GEN_PRINTF("srcPoint=(%d,%d), destPoint=(%d,%d)\n", srcPoint.x, srcPoint.y, dstPoint.x, dstPoint.y);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
}

void WinGuiISTK::mouseRightClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(point.x), sy(point.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::moustDoubleRightClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(point.x), sy(point.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(point.x), sy(point.y), 0, NULL);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(point.x), sy(point.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::mouseRightDrag(const Point &srcPoint, const Point &dstPoint)
{
    LOG_GEN_PRINTF("srcPoint=(%d,%d), destPoint=(%d,%d)\n", srcPoint.x, srcPoint.y, dstPoint.x, dstPoint.y);

    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
    local_mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
}

void WinGuiISTK::mouseScroll(const Point &point, int steps)
{
    LOG_GEN_PRINTF("point=(%d,%d), steps=%d\n", point.x, point.y, steps);

    local_mouse_event(MOUSEEVENTF_WHEEL, 0, 0, steps, NULL);
}

bool WinGuiISTK::findImageRect(const Image &image, Rect &rect)
{
    LOG_GEN_PRINTF("image.path=\"%s\"\n", 
        image.getPath().c_str()
    );

    return findImageRect_impl(image, rect);
}

bool WinGuiISTK::findImageRect_impl(const Image &image, Rect &rect)
{
    bool bSuc = true;
    CBitmap *pDesktopWindowBitmap = NULL;
    CBitmap *pImageBitmap = NULL;
    Rect searchRectNormalized;
    BITMAP wholeBitmapInfo;

    if (bSuc) {
        if (!m_sEnvVarScreenPictureFilePath.empty()) {
            pDesktopWindowBitmap = loadImageAsBitmap(m_sEnvVarScreenPictureFilePath);
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        } else {
            pDesktopWindowBitmap = getDesktopWindowAsBitmap();
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        }
    }

    if (bSuc) {
        pImageBitmap = loadImageAsBitmap(image.getPath());
        if (pImageBitmap == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!pDesktopWindowBitmap->GetBitmap(&wholeBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        searchRectNormalized.x = 0;
        searchRectNormalized.y = 0;
        searchRectNormalized.width = wholeBitmapInfo.bmWidth;
        searchRectNormalized.height = wholeBitmapInfo.bmHeight;
        bSuc = findBitmapInBitmap(rect, searchRectNormalized, pImageBitmap, pDesktopWindowBitmap);
    }

    if (pImageBitmap != NULL) {
        delete pImageBitmap;
        pImageBitmap = NULL;
    }

    if (pDesktopWindowBitmap != NULL) {
        delete pDesktopWindowBitmap;
        pDesktopWindowBitmap = NULL;
    }

    return bSuc;
}

bool WinGuiISTK::findImageRect(const Image &image, Rect &rect, const Rect &searchRect)
{
    LOG_GEN_PRINTF("image.path=\"%s\", searchRect=(%d,%d,%u,%u)\n", 
        image.getPath().c_str(), 
        searchRect.x, searchRect.y, searchRect.width, searchRect.height
    );

    return findImageRect_impl(image, rect, searchRect);
}

bool WinGuiISTK::findImageRect_impl(const Image &image, Rect &rect, const Rect &searchRect)
{
    bool bSuc = true;
    CBitmap *pDesktopWindowBitmap = NULL;
    CBitmap *pImageBitmap = NULL;
    BITMAP wholeBitmapInfo;
    Rect searchRectNormalized = searchRect;

    if (bSuc) {
        if (!m_sEnvVarScreenPictureFilePath.empty()) {
            pDesktopWindowBitmap = loadImageAsBitmap(m_sEnvVarScreenPictureFilePath);
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        } else {
            pDesktopWindowBitmap = getDesktopWindowAsBitmap();
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        }
    }

    if (bSuc) {
        pImageBitmap = loadImageAsBitmap(image.getPath());
        if (pImageBitmap == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!pDesktopWindowBitmap->GetBitmap(&wholeBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        searchRectNormalized.intersect(Rect(0, 0, wholeBitmapInfo.bmWidth, wholeBitmapInfo.bmHeight));
        bSuc = findBitmapInBitmap(rect, searchRectNormalized, pImageBitmap, pDesktopWindowBitmap);
    }

    if (pImageBitmap != NULL) {
        delete pImageBitmap;
        pImageBitmap = NULL;
    }

    if (pDesktopWindowBitmap != NULL) {
        delete pDesktopWindowBitmap;
        pDesktopWindowBitmap = NULL;
    }

    return bSuc;
}

bool WinGuiISTK::findImageRect(const Image &image, Rect &rect, const Point &searchBeginningPoint)
{
    LOG_GEN_PRINTF("image.path=\"%s\", searchBeginningPoint=(%d,%d)\n", 
        image.getPath().c_str(), 
        searchBeginningPoint.x, searchBeginningPoint.y
    );

    return findImageRect_impl(image, rect, searchBeginningPoint);
}

bool WinGuiISTK::findImageRect_impl(const Image &image, Rect &rect, const Point &searchBeginningPoint)
{
    bool bSuc = true;
    CBitmap *pDesktopWindowBitmap = NULL;
    CBitmap *pImageBitmap = NULL;
    Rect searchRectNormalized;
    BITMAP wholeBitmapInfo;

    if (bSuc) {
        if (!m_sEnvVarScreenPictureFilePath.empty()) {
            pDesktopWindowBitmap = loadImageAsBitmap(m_sEnvVarScreenPictureFilePath);
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        } else {
            pDesktopWindowBitmap = getDesktopWindowAsBitmap();
            if (pDesktopWindowBitmap == NULL) {
                bSuc = false;
            }
        }
    }

    if (bSuc) {
        pImageBitmap = loadImageAsBitmap(image.getPath());
        if (pImageBitmap == NULL) {
            bSuc = false;
        }
    }

    if (bSuc) {
        if (!pDesktopWindowBitmap->GetBitmap(&wholeBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        searchRectNormalized.x = searchBeginningPoint.x;
        searchRectNormalized.y = searchBeginningPoint.y;
        searchRectNormalized.width = wholeBitmapInfo.bmWidth;
        searchRectNormalized.height = wholeBitmapInfo.bmHeight;
        searchRectNormalized.intersect(Rect(0, 0, wholeBitmapInfo.bmWidth, wholeBitmapInfo.bmHeight));
        bSuc = findBitmapInBitmap(rect, searchRectNormalized, pImageBitmap, pDesktopWindowBitmap);
    }

    if (pImageBitmap != NULL) {
        delete pImageBitmap;
        pImageBitmap = NULL;
    }

    if (pDesktopWindowBitmap != NULL) {
        delete pDesktopWindowBitmap;
        pDesktopWindowBitmap = NULL;
    }

    return bSuc;
}

bool WinGuiISTK::waitImageShown(const Image &image, Rect &rect, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;

    LOG_GEN_PRINTF("image.path=\"%s\", timeout=%u\n", 
        image.getPath().c_str(), 
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            break;
        }

        bSuc = findImageRect_impl(image, rect);
        if (bSuc) {
            break;
        }

        elapsedTime += WAIT_IMAGE_SHOWN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    return bSuc;
}

bool WinGuiISTK::waitImageShown(const Image &image, Rect &rect, const Rect &searchRect, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;

    LOG_GEN_PRINTF("image.path=\"%s\", searchRect=(%d,%d,%u,%u), timeout=%u\n", 
        image.getPath().c_str(), 
        searchRect.x, searchRect.y, searchRect.width, searchRect.height,
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            break;
        }

        bSuc = findImageRect_impl(image, rect, searchRect);
        if (bSuc) {
            break;
        }

        elapsedTime += WAIT_IMAGE_SHOWN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

    return bSuc;
}

bool WinGuiISTK::waitImageShown(const Image &image, Rect &rect, const Point &searchBeginningPoint, unsigned int timeout /*= INFINITE_TIME*/)
{
    bool bSuc = false;
    unsigned int elapsedTime = 0;

    LOG_GEN_PRINTF("image.path=\"%s\", searchBeginningPoint=(%d,%d), timeout=%u\n", 
        image.getPath().c_str(), 
        searchBeginningPoint.x, searchBeginningPoint.y, 
        timeout
    );

    for (;;) {
        if (timeout != INFINITE_TIME && elapsedTime > timeout) {
            break;
        }

        bSuc = findImageRect_impl(image, rect, searchBeginningPoint);
        if (bSuc) {
            break;
        }

        elapsedTime += WAIT_IMAGE_SHOWN_INTERVAL;
        Sleep(WAIT_IMAGE_SHOWN_INTERVAL);
    }

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

CBitmap *WinGuiISTK::getDesktopWindowAsBitmap()
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

bool WinGuiISTK::findBitmapInBitmap(Rect &matchedRect, const Rect& searchRect, CBitmap *partBitmap, CBitmap *wholeBitmap)
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
        partBitmapInfo.bmBits = m_pPartBitmapMem;
        partBitmap->GetBitmapBits(m_nPartBitmapMemSize, m_pPartBitmapMem);
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
        wholeBitmapInfo.bmBits = m_pWholeBitmapMem;
        wholeBitmap->GetBitmapBits(m_nWholeBitmapMemSize, m_pWholeBitmapMem);
        if (!fixBitmapAlphaBits(wholeBitmapInfo)) {
            bSuc = false;
        }
    }

    if (bSuc) {
        searchRectNormalized.intersect(Rect(0, 0, wholeBitmapInfo.bmWidth, wholeBitmapInfo.bmHeight));
        if (searchRectNormalized.width == 0 || searchRectNormalized.height == 0) {
            bSuc = false;
        }
    }

    if (bSuc) {
        bSuc = findBitmapInBitmap_unsafe(matchedRect, searchRectNormalized, partBitmapInfo, wholeBitmapInfo);
    }

    return bSuc;
}

bool WinGuiISTK::findBitmapInBitmap_unsafe(Rect &matchedRect, const Rect& searchRect, const BITMAP &partBitmapInfo, const BITMAP &wholeBitmapInfo)
{
    bool bSuc = true;

    if (bSuc) {
        if (partBitmapInfo.bmBitsPixel == 32 && wholeBitmapInfo.bmBitsPixel == 32) {
            bSuc = findBitmapInBitmap_bytes_unsafe<4>(matchedRect, searchRect, partBitmapInfo, wholeBitmapInfo);
        } else if (partBitmapInfo.bmBitsPixel == 24 && wholeBitmapInfo.bmBitsPixel == 24) {
            bSuc = findBitmapInBitmap_bytes_unsafe<3>(matchedRect, searchRect, partBitmapInfo, wholeBitmapInfo);
        } else if (partBitmapInfo.bmBitsPixel == 16 && wholeBitmapInfo.bmBitsPixel == 24) {
            bSuc = findBitmapInBitmap_bytes_unsafe<2>(matchedRect, searchRect, partBitmapInfo, wholeBitmapInfo);
        } else {
            bSuc = false;
        }
    }

    return bSuc;
}

template <unsigned int BYTES_PER_PIXEL>
bool WinGuiISTK::findBitmapInBitmap_bytes_unsafe(Rect &matchedRect, const Rect& searchRect, const BITMAP &partBitmapInfo, const BITMAP &wholeBitmapInfo)
{
    bool bSuc = false;
    unsigned int bytesPerPixel = BYTES_PER_PIXEL;
    unsigned int bytesPerLine = bytesPerPixel * partBitmapInfo.bmWidth;
    unsigned int x, y;
    unsigned char i;
    unsigned char *pSrc, *pDst;

    for (y = searchRect.y; y <= searchRect.y + searchRect.height - partBitmapInfo.bmHeight; ++y) {
        for (x = searchRect.x; x <= searchRect.x + searchRect.width - partBitmapInfo.bmWidth; ++x) {
            pSrc = (unsigned char *)partBitmapInfo.bmBits;
            pDst = (unsigned char *)wholeBitmapInfo.bmBits + wholeBitmapInfo.bmWidthBytes * y + bytesPerPixel * x;
            if (memcmp(pDst, pSrc, bytesPerLine) != 0) {
                continue;
            } else {
                bool matched = true;
                for (i = 1; i < partBitmapInfo.bmHeight; ++i) {
                    pSrc += partBitmapInfo.bmWidthBytes;
                    pDst += wholeBitmapInfo.bmWidthBytes;
                    if (memcmp(pDst, pSrc, bytesPerLine) != 0) {
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
                }
            }
        }
    }

    return bSuc;
}

} // namespace GuiISTk {
