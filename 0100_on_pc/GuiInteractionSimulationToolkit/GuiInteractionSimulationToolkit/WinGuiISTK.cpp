#include "stdafx.h"
#include "TK_Tools.h"
#include "WinGuiISTK.h"

namespace GuiISTk {

const unsigned int WAIT_IMAGE_SHOWN_INTERVAL = 500;

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
    m_nPartBitmapMemSize = 1920 * 1080 * 4 * 4;
    m_nWholeBitmapMemSize = 1920 * 1080 * 4 * 4;;

    m_pPartBitmapMem = new DWORD[m_nPartBitmapMemSize / 4];
    m_pWholeBitmapMem = new DWORD[m_nWholeBitmapMemSize / 4];

    const char *pEnvVarScreenPictureFilePath = getenv("SCREEN_PICTURE_FILE_PATH");
    m_sEnvVarScreenPictureFilePath = pEnvVarScreenPictureFilePath == NULL ? "" : pEnvVarScreenPictureFilePath;
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
    LOG_GEN_PRINTF("ch=%u\n", ch);

    int words_num = 1;
    KEYBDINPUT *keyinput = new KEYBDINPUT[words_num];
    memset(keyinput, 0, words_num * sizeof(KEYBDINPUT));
    for (int i = 0; i < words_num; i++)
    {
        keyinput[i].wScan = ch;
        keyinput[i].dwFlags = KEYEVENTF_UNICODE;
    }
    INPUT *input = new INPUT[words_num];
    for (int i = 0; i < words_num; i++)
    {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki = keyinput[i];
    }
    SendInput(words_num, input, sizeof(INPUT));
    delete[] input;
    delete[] keyinput;
}

void WinGuiISTK::kbdString(const std::string &s)
{
    LOG_GEN_PRINTF("s=%s\n", s.c_str());

    int words_num = s.length();
    KEYBDINPUT *keyinput = new KEYBDINPUT[words_num];
    memset(keyinput, 0, words_num * sizeof(KEYBDINPUT));
    for (int i = 0; i < words_num; i++)
    {
        keyinput[i].wScan = s[i];
        keyinput[i].dwFlags = KEYEVENTF_UNICODE;
    }
    INPUT *input = new INPUT[words_num];
    for (int i = 0; i < words_num; i++)
    {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki = keyinput[i];
    }
    SendInput(words_num, input, sizeof(INPUT));
    delete[] input;
    delete[] keyinput;
}

void WinGuiISTK::mouseMove(const Point &point, bool absolute /*= true*/)
{
    LOG_GEN_PRINTF("point=(%d,%d), absolute=%d\n", point.x, point.y, absolute);

    if (absolute) {
        mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);
    } else {
        mouse_event(MOUSEEVENTF_MOVE, point.x, point.y, 0, NULL);
    }
}

void WinGuiISTK::mouseClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::moustDoubleClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::mouseDrag(const Point &srcPoint, const Point &dstPoint)
{
    LOG_GEN_PRINTF("srcPoint=(%d,%d), destPoint=(%d,%d)\n", srcPoint.x, srcPoint.y, dstPoint.x, dstPoint.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
}

void WinGuiISTK::mouseRightClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::moustDoubleRightClick(const Point &point)
{
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::mouseRightDrag(const Point &srcPoint, const Point &dstPoint)
{
    LOG_GEN_PRINTF("srcPoint=(%d,%d), destPoint=(%d,%d)\n", srcPoint.x, srcPoint.y, dstPoint.x, dstPoint.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
}

void WinGuiISTK::mouseScroll(const Point &point, int steps)
{
    LOG_GEN_PRINTF("point=(%d,%d), steps=%d\n", point.x, point.y, steps);

    mouse_event(MOUSEEVENTF_WHEEL, 0, 0, steps, NULL);
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
    Graphics *pGraphics = NULL;

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
        pGraphics = new Graphics(bitmapDC.GetSafeHdc());
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
