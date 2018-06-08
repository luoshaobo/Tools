#include <Windows.h>
#include "TK_Tools.h"
#include "WinGuiISTK.h"


namespace GuiISTk {

WinGuiISTK::WinGuiISTK(void)
{

}

WinGuiISTK::~WinGuiISTK(void)
{

}

void WinGuiISTK::delay(unsigned int milliSecond)
{
    LOG_GEN();
    LOG_GEN_PRINTF("milliSecond=%u\n", milliSecond);

    Sleep(milliSecond);
}

void WinGuiISTK::kbdKeyDown(unsigned char vk)
{
    LOG_GEN();
    LOG_GEN_PRINTF("vk=%u\n", vk);

    keybd_event(vk,0,0,0);
}

void WinGuiISTK::kbdKeyUp(unsigned char vk)
{
    LOG_GEN();
    LOG_GEN_PRINTF("vk=%u\n", vk);

    keybd_event(vk,0,KEYEVENTF_KEYUP,0);
}

void WinGuiISTK::kbdKeyOn(unsigned char vk)
{
    LOG_GEN();
    LOG_GEN_PRINTF("vk=%u\n", vk);

    if (!GetKeyState(vk) & 0x1) {
        keybd_event(vk,0,0,0);
        keybd_event(vk,0,KEYEVENTF_KEYUP,0);
    }
}

void WinGuiISTK::kbdKeyOff(unsigned char vk)
{
    LOG_GEN();
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
    LOG_GEN();
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
    LOG_GEN();
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
    LOG_GEN();
    LOG_GEN_PRINTF("point=(%d,%d), absolute=%d\n", point.x, point.y, absolute);

    if (absolute) {
        mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);
    } else {
        mouse_event(MOUSEEVENTF_MOVE, point.x, point.y, 0, NULL);
    }
}

void WinGuiISTK::mouseClick(const Point &point)
{
    LOG_GEN();
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::moustDoubleClick(const Point &point)
{
    LOG_GEN();
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::mouseDrag(const Point &srcPoint, const Point &dstPoint)
{
    LOG_GEN();
    LOG_GEN_PRINTF("srcPoint=(%d,%d), destPoint=(%d,%d)\n", srcPoint.x, srcPoint.y, dstPoint.x, dstPoint.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
}

void WinGuiISTK::mouseRightClick(const Point &point)
{
    LOG_GEN();
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::moustDoubleRightClick(const Point &point)
{
    LOG_GEN();
    LOG_GEN_PRINTF("point=(%d,%d)\n", point.x, point.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(point.x), sy(point.y), 0, NULL);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(point.x), sy(point.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(point.x), sy(point.y), 0, NULL);
}

void WinGuiISTK::mouseRightDrag(const Point &srcPoint, const Point &dstPoint)
{
    LOG_GEN();
    LOG_GEN_PRINTF("srcPoint=(%d,%d), destPoint=(%d,%d)\n", srcPoint.x, srcPoint.y, dstPoint.x, dstPoint.y);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, sx(srcPoint.x), sy(srcPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, sx(dstPoint.x), sy(dstPoint.y), 0, NULL);
}

void WinGuiISTK::mouseScroll(const Point &point, int steps)
{
    LOG_GEN();
    LOG_GEN_PRINTF("point=(%d,%d), steps=%d\n", point.x, point.y, steps);

    mouse_event(MOUSEEVENTF_WHEEL, 0, 0, steps, NULL);
}

bool WinGuiISTK::findImageRect(const Image &image, Rect &rect, unsigned int timeout /*= INFINITE_TIME*/)
{
    LOG_GEN();
    LOG_GEN_PRINTF("image.path=\"%s\", rect=(%d,%d,%u,%u), timeout=%u\n", 
        image.getPath().c_str(), 
        rect.x, rect.y, rect.width, rect.height,
        timeout
    );

    return true;
}

bool WinGuiISTK::findImageRect(const Image &image, Rect &rect, const Rect &searchRect, unsigned int timeout /*= INFINITE_TIME*/)
{
    LOG_GEN();
    LOG_GEN_PRINTF("image.path=\"%s\", rect=(%d,%d,%u,%u), searchRect=(%d,%d,%u,%u), timeout=%u\n", 
        image.getPath().c_str(), 
        rect.x, rect.y, rect.width, rect.height,
        searchRect.x, searchRect.y, searchRect.width, searchRect.height,
        timeout
    );

    return true;
}

bool WinGuiISTK::findImageRect(const Image &image, Rect &rect, const Point &searchBeginningPoint, unsigned int timeout /*= INFINITE_TIME*/)
{
    LOG_GEN();
    LOG_GEN_PRINTF("image.path=\"%s\", rect=(%d,%d,%u,%u), searchBeginningPoint=(%d,%d), timeout=%u\n", 
        image.getPath().c_str(), 
        rect.x, rect.y, rect.width, rect.height,
        searchBeginningPoint.x, searchBeginningPoint.y, 
        timeout
    );

    return true;
}

bool WinGuiISTK::waitImageShown(const Image &image, unsigned int timeout /*= INFINITE_TIME*/)
{
    LOG_GEN();
    LOG_GEN_PRINTF("image.path=\"%s\", timeout=%u\n", 
        image.getPath().c_str(), 
        timeout
    );

    return true;
}

bool WinGuiISTK::waitImageShown(const Image &image, const Rect &searchRect, unsigned int timeout /*= INFINITE_TIME*/)
{
    LOG_GEN();
    LOG_GEN_PRINTF("image.path=\"%s\", searchRect=(%d,%d,%u,%u), timeout=%u\n", 
        image.getPath().c_str(), 
        searchRect.x, searchRect.y, searchRect.width, searchRect.height,
        timeout
    );

    return true;
}

bool WinGuiISTK::waitImageShown(const Image &image, const Point &searchBeginningPoint, unsigned int timeout /*= INFINITE_TIME*/)
{
    LOG_GEN();
    LOG_GEN_PRINTF("image.path=\"%s\", searchBeginningPoint=(%d,%d), timeout=%u\n", 
        image.getPath().c_str(), 
        searchBeginningPoint.x, searchBeginningPoint.y, 
        timeout
    );

    return true;
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

} // namespace GuiISTk {
