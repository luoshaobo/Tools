#pragma once
#include <Windows.h>
#include "GuiISTk.h"

class CBitmap;

namespace GuiISTk {

class WinGuiISTK : public IToolkit
{
public:
    WinGuiISTK();
    virtual ~WinGuiISTK();

    virtual void Delay(unsigned int milliSecond);

    virtual unsigned int wndCount(const ScreenInfo &screenInfo);
    virtual bool wndShow(const ScreenInfo &screenInfo, ScreenShowingMode mode);
    virtual bool wndHide(const ScreenInfo &screenInfo);
    virtual bool wndClose(const ScreenInfo &screenInfo);
    virtual bool wndMove(const ScreenInfo &screenInfo, const Point &point);
    virtual bool wndSetSize(const ScreenInfo &screenInfo, const Rect &rect);
    virtual bool wndGetSize(const ScreenInfo &screenInfo, std::vector<Rect> &rects);
    virtual bool wndSetZorder(const ScreenInfo &screenInfo, ScreenZorder zorder);
    virtual bool wndSaveAsPic(const ScreenInfo &screenInfo, const std::string &sPictureFilePath);
    virtual bool wndGetFgWnd(ScreenInfo &screenInfo);
    virtual bool wndGetWndAtPoint(ScreenInfo &screenInfo, const Point &point);

    virtual bool dspSavePrimaryAsPic(const std::string &sPictureFilePath);
    virtual void dspGetPrimaryRect(Rect &rect);
    virtual bool dspSaveVirtualAsPic(const std::string &sPictureFilePath);
    virtual void dspGetVirtualRect(Rect &rect);

    virtual bool cbdPutStr(const std::string &s);
    virtual bool cbdGetStr(std::string &s);
    
    virtual void kbdKey(unsigned char vk);
    virtual void kbdCombKey(std::vector<unsigned char> vks);
    virtual void kbdKeyDown(unsigned char vk);
    virtual void kbdKeyUp(unsigned char vk);
    virtual void kbdKeyOn(unsigned char vk);
    virtual void kbdKeyOff(unsigned char vk);
    virtual void kbdChar(char ch);
    virtual void kbdStr(const std::string &s);
    
    virtual void mseMove(const Point &point, bool absolute = true);
    virtual void mseClick(const Point &point);
    virtual void mseDClick(const Point &point);
    virtual void mseDrag(const Point &srcPoint, const Point &dstPoint);
    virtual void mseRClick(const Point &point);
    virtual void mseDRClick(const Point &point);
    virtual void mseRDrag(const Point &srcPoint, const Point &dstPoint);
    virtual void mseScroll(const Point &point, int steps);
    
    virtual bool imgFindRect(const std::vector<Image> &images, Rect &rect, int &index);
    virtual bool imgFindRect(const std::vector<Image> &images, Rect &rect, int &index, const Rect &searchRect);
    virtual bool imgFindRect(const std::vector<Image> &images, Rect &rect, int &index, const Point &searchBeginningPoint);
    virtual bool imgFindAllRects(const Image &image, std::vector<Rect> &rects);
    virtual bool imgFindAllRects(const Image &image, std::vector<Rect> &rects, const Rect &searchRect);
    virtual bool imgFindAllRects(const Image &image, std::vector<Rect> &rects, const Point &searchBeginningPoint);
    virtual bool imgWaitShown(const std::vector<Image> &images, Rect &rect, int &index, unsigned int timeout = INFINITE_TIME);
    virtual bool imgWaitShown(const std::vector<Image> &images, Rect &rect, int &index, const Rect &searchRect, unsigned int timeout = INFINITE_TIME);
    virtual bool imgWaitShown(const std::vector<Image> &images, Rect &rect, int &index, const Point &searchBeginningPoint, unsigned int timeout = INFINITE_TIME);
    virtual bool imgWaitAllShown(const std::vector<Image> &images, std::vector<Rect> &rects, unsigned int timeout = INFINITE_TIME);
    virtual bool imgWaitAllShown(const std::vector<Image> &images, std::vector<Rect> &rects, const Rect &searchRect, unsigned int timeout = INFINITE_TIME);
    virtual bool imgWaitAllShown(const std::vector<Image> &images, std::vector<Rect> &rects, const Point &searchBeginningPoint, unsigned int timeout = INFINITE_TIME);

private:
    CBitmap *getDesktopWindowAsBitmap();
    CBitmap *getVirtualDesktopWindowAsBitmap(Rect &virtualDesktopRect);
    CBitmap *loadImageAsBitmap(const std::string &imageFilePath);
    bool findBitmapInBitmap(std::vector<Rect> &matchedRects, const Rect& searchRect, CBitmap *partBitmap, CBitmap *wholeBitmap, bool findAll);
    bool findBitmapInBitmap_unsafe(std::vector<Rect> &matchedRects, const Rect& searchRect, const BITMAP &partBitmapInfo, const BITMAP &wholeBitmapInfo, bool findAll);
    template <unsigned int BYTES_PER_PIXEL> bool findBitmapInBitmap_bytes_unsafe(std::vector<Rect> &matchedRects, const Rect& searchRect, const BITMAP &partBitmapInfo, const BITMAP &wholeBitmapInfo, bool findAll);
    bool imgFindRect_impl(const std::vector<Image> &images, std::vector<Rect> &rects, int &index, bool findAll);
    bool imgFindRect_impl(const std::vector<Image> &images, std::vector<Rect> &rects, int &index, const Rect &searchRect, bool findAll);
    bool imgFindRect_impl(const std::vector<Image> &images, std::vector<Rect> &rects, int &index, const Point &searchBeginningPoint, bool findAll);
    bool fixBitmapAlphaBits(const BITMAP &bitmapInfo);

    struct Arguments_getMatchedWindows_EnumWindowsProc {
        Arguments_getMatchedWindows_EnumWindowsProc(WinGuiISTK *a_thiz, const ScreenInfo &a_screenInfo, std::vector<HWND> &a_winHandles) : thiz(a_thiz), screenInfo(a_screenInfo), winHandles(a_winHandles) {}
        
        WinGuiISTK *thiz;
        const ScreenInfo &screenInfo;
        std::vector<HWND> &winHandles;
    };
    bool getMatchedWindows(std::vector<HWND> &winHandles, const ScreenInfo &screenInfo);
    static BOOL CALLBACK getMatchedWindows_EnumWindowsProc(HWND hwnd, LPARAM lParam);

    bool getVirtualDesktopRect(CRect &virtualDesktopRect);
    struct Arguments_getVirtualDesktopRect_MonitorEnumProc {
        WinGuiISTK *thiz;
        CRect &virtualDesktopRect;
    };
    static BOOL CALLBACK getVirtualDesktopRect_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);


    bool saveWindowAsPicture(HWND hWnd, const std::string &path, const CRect *rect = NULL);
    bool getImageGetEncoderFormPath(CLSID &encoderClsId, const std::string &path);
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
    bool makeFilePathTemplateByIndex(std::string &pathTemplate, const std::string &path);

    std::string getImagesPaths(const std::vector<Image> &images);

private:
    DWORD sx(DWORD x);
    DWORD sy(DWORD y);

private:
    CString m_sExeFileName;
    std::string m_sEnvVarScreenPictureFilePath;
};

} // namespace GuiISTk {
