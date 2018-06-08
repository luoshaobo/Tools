#pragma once
#include "GuiISTk.h"

namespace GuiISTk {

class WinGuiISTK : public IToolkit
{
public:
    WinGuiISTK(void);
    virtual ~WinGuiISTK(void);

    virtual void delay(unsigned int milliSecond);
    
    virtual void kbdKeyDown(unsigned char vk);
    virtual void kbdKeyUp(unsigned char vk);
    virtual void kbdKeyOn(unsigned char vk);
    virtual void kbdKeyOff(unsigned char vk);
    virtual void kbdCtrlA();
    virtual void kbdCtrlC();
    virtual void kbdCtrlX();
    virtual void kbdCtrlV();
    virtual void kbdChar(char ch);
    virtual void kbdString(const std::string &s);
    
    virtual void mouseMove(const Point &point, bool absolute = true);
    virtual void mouseClick(const Point &point);
    virtual void moustDoubleClick(const Point &point);
    virtual void mouseDrag(const Point &srcPoint, const Point &dstPoint);
    virtual void mouseRightClick(const Point &point);
    virtual void moustDoubleRightClick(const Point &point);
    virtual void mouseRightDrag(const Point &srcPoint, const Point &dstPoint);
    virtual void mouseScroll(const Point &point, int steps);
    
    virtual bool findImageRect(const Image &image, Rect &rect, unsigned int timeout = INFINITE_TIME);
    virtual bool findImageRect(const Image &image, Rect &rect, const Rect &searchRect, unsigned int timeout = INFINITE_TIME);
    virtual bool findImageRect(const Image &image, Rect &rect, const Point &searchBeginningPoint, unsigned int timeout = INFINITE_TIME);
    
    virtual bool waitImageShown(const Image &image, unsigned int timeout = INFINITE_TIME);
    virtual bool waitImageShown(const Image &image, const Rect &searchRect, unsigned int timeout = INFINITE_TIME);
    virtual bool waitImageShown(const Image &image, const Point &searchBeginningPoint, unsigned int timeout = INFINITE_TIME);

private:
    DWORD sx(DWORD x);
    DWORD sy(DWORD y);
};

} // namespace GuiISTk {
