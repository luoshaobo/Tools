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

    virtual void delay(unsigned int milliSecond);

    virtual unsigned int scnGetCount(const ScreenInfo &screenInfo);
    virtual bool scnShow(const ScreenInfo &screenInfo, ScreenShowingMode mode);
    virtual bool scnHide(const ScreenInfo &screenInfo);
    virtual bool scnClose(const ScreenInfo &screenInfo);
    virtual bool scnMove(const ScreenInfo &screenInfo, const Point &point);
    virtual bool scnResize(const ScreenInfo &screenInfo, const Rect &rect);
    virtual bool scnSetZorder(const ScreenInfo &screenInfo, ScreenZorder zorder);
    virtual bool scnSaveAsPictures(const ScreenInfo &screenInfo, const std::string &pictureFilePath);
    virtual bool scnSaveDesktopAsPicture(const std::string &pictureFilePath);

    virtual bool cbdPutString(const std::string &s);
    virtual bool cbdGetString(std::string &s);
    
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
    
    virtual bool findImageRect(const std::vector<Image> &images, Rect &rect, int &index);
    virtual bool findImageRect(const std::vector<Image> &images, Rect &rect, int &index, const Rect &searchRect);
    virtual bool findImageRect(const std::vector<Image> &images, Rect &rect, int &index, const Point &searchBeginningPoint);
    
    virtual bool waitImageShown(const std::vector<Image> &images, Rect &rect, int &index, unsigned int timeout = INFINITE_TIME);
    virtual bool waitImageShown(const std::vector<Image> &images, Rect &rect, int &index, const Rect &searchRect, unsigned int timeout = INFINITE_TIME);
    virtual bool waitImageShown(const std::vector<Image> &images, Rect &rect, int &index, const Point &searchBeginningPoint, unsigned int timeout = INFINITE_TIME);

private:
    CBitmap *getDesktopWindowAsBitmap();
    CBitmap *loadImageAsBitmap(const std::string &imageFilePath);
    bool findBitmapInBitmap(Rect &matchedRect, const Rect& searchRect, CBitmap *partBitmap, CBitmap *wholeBitmap);
    bool findBitmapInBitmap_unsafe(Rect &matchedRect, const Rect& searchRect, const BITMAP &partBitmapInfo, const BITMAP &wholeBitmapInfo);
    template <unsigned int BYTES_PER_PIXEL>
    bool findBitmapInBitmap_bytes_unsafe(Rect &matchedRect, const Rect& searchRect, const BITMAP &partBitmapInfo, const BITMAP &wholeBitmapInfo);
    bool findImageRect_impl(const std::vector<Image> &images, Rect &rect, int &index);
    bool findImageRect_impl(const std::vector<Image> &images, Rect &rect, int &index, const Rect &searchRect);
    bool findImageRect_impl(const std::vector<Image> &images, Rect &rect, int &index, const Point &searchBeginningPoint);
    bool fixBitmapAlphaBits(const BITMAP &bitmapInfo);

    struct Arguments_getMatchedWindows_EnumWindowsProc {
        Arguments_getMatchedWindows_EnumWindowsProc(WinGuiISTK *a_thiz, const ScreenInfo &a_screenInfo, std::vector<HWND> &a_winHandles) : thiz(a_thiz), screenInfo(a_screenInfo), winHandles(a_winHandles) {}
        
        WinGuiISTK *thiz;
        const ScreenInfo &screenInfo;
        std::vector<HWND> &winHandles;
    };
    bool getMatchedWindows(std::vector<HWND> &winHandles, const ScreenInfo &screenInfo);
    static BOOL CALLBACK getMatchedWindows_EnumWindowsProc(HWND hwnd, LPARAM lParam);

    bool saveWindowAsPicture(HWND hWnd, const std::string &path);
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
