#ifndef GUISTK_H
#define GUISTK_H

#include <string>

namespace GuiISTk {

struct Point
{
    Point() : x(0), y(0) {}
    Point(int a_x, int a_y) : x(a_x), y(a_y) {}
    Point &operator =(const Point &other) { x = other.x; y = other.y; return *this; }

    int x;
    int y;
};

struct Size
{
    Size() : width(0), height(0) {}
    Size(int a_width, int a_height) : width(a_width), height(a_height) {}
    Size &operator =(const Size &other) { width = other.width; height = other.height; return *this; }

    unsigned int width;
    unsigned int height;
};

struct Rect
{
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(int a_x, int a_y, int a_width, int a_height) : x(a_x), y(a_y), width(a_width), height(a_height) {}
    Rect &operator =(const Rect &other) { x = other.x; y = other.y; width = other.width; height = other.height; return *this; }

    void Intersect(const Rect &other)
    {
        int nTwiceCenterXDelta = (x + (x + width)) - (other.x + (other.x + other.width));
        nTwiceCenterXDelta = nTwiceCenterXDelta >= 0 ? nTwiceCenterXDelta : -nTwiceCenterXDelta;
        
        int nTwiceCenterYDelta = (y + (y + height)) - (other.y + (other.y + other.height));
        nTwiceCenterYDelta = nTwiceCenterYDelta >= 0 ? nTwiceCenterYDelta : -nTwiceCenterYDelta;

        if (nTwiceCenterXDelta < (int)(width + other.width) && nTwiceCenterYDelta < (int)(height + other.height)) {
            int resultX1, resultY1, resultX2, resultY2;
            resultX1 = x > other.x ? x : other.x;
            resultY1 = y > other.y ? y : other.y;
            resultX2 = (x + width) < (other.x + other.width) ? (x + width) : (other.x + other.width);
            resultY2 = (y + height) < (other.y + other.height) ? (y + height) : (other.y + other.height);

            x = resultX1;
            y = resultY1;
            width = resultX2 - resultX1;
            height = resultY2 - resultY1;
        } else {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
        }
    }

    void Union(const Rect &other)
    {
        int resultX1, resultY1, resultX2, resultY2;
        resultX1 = x < other.x ? x : other.x;
        resultY1 = y < other.y ? y : other.y;
        resultX2 = (x + width) > (other.x + other.width) ? (x + width) : (other.x + other.width);
        resultY2 = (y + height) > (other.y + other.height) ? (y + height) : (other.y + other.height);

        x = resultX1;
        y = resultY1;
        width = resultX2 - resultX1;
        height = resultY2 - resultY1;
    }

    void Offset(int dx, int dy)
    {
        x += dx;
        y += dy;
    }

    int x;
    int y;
    unsigned int width;
    unsigned int height;
};

class Image
{
public:
    Image() : m_imageFilePath() {}
    Image(const std::string &imageFilePath) : m_imageFilePath(imageFilePath) {}

    void setPath(const std::string &imageFilePath) { m_imageFilePath = imageFilePath; }
    std::string getPath() const { return m_imageFilePath; }

private:
    std::string m_imageFilePath;
};

struct ScreenInfo
{
    ScreenInfo() : title(), fullMatched(false), allMatched(false) {}
    ScreenInfo(const std::string &a_title, bool a_fullMatched = false, bool a_allMatched = false) : title(a_title), fullMatched(a_fullMatched), allMatched(a_allMatched) {}

    std::string title;
    bool fullMatched;
    bool allMatched;
};

enum ScreenShowingMode {
    SSM_RESTORE,
    SSM_NORMAL,
    SSM_MIN,
    SSM_MAX,
    SSM_FG,
};

enum ScreenZorder {
    SZO_BOTTOM,
    SZO_TOP,
};

const unsigned int INFINITE_TIME = (unsigned int)-1;

class IToolkit
{
public:
    virtual ~IToolkit() {}
    
    virtual void Delay(unsigned int milliSecond) = 0;

    virtual unsigned int wndCount(const ScreenInfo &screenInfo) = 0;
    virtual bool wndShow(const ScreenInfo &screenInfo, ScreenShowingMode mode) = 0;
    virtual bool wndHide(const ScreenInfo &screenInfo) = 0;
    virtual bool wndClose(const ScreenInfo &screenInfo) = 0;
    virtual bool wndMove(const ScreenInfo &screenInfo, const Point &point) = 0;
    virtual bool wndSetSize(const ScreenInfo &screenInfo, const Rect &rect) = 0;
    virtual bool wndGetSize(const ScreenInfo &screenInfo, std::vector<Rect> &rects) = 0;
    virtual bool wndSetZorder(const ScreenInfo &screenInfo, ScreenZorder zorder) = 0;
    virtual bool wndSaveAsPic(const ScreenInfo &screenInfo, const std::string &sPictureFilePath) = 0;
    virtual bool wndGetFgWnd(ScreenInfo &screenInfo) = 0;
    virtual bool wndGetWndAtPoint(ScreenInfo &screenInfo, const Point &point) = 0;

    virtual bool dspSaveAllAsPics(const std::string &sPictureFilePath) = 0;
    virtual void dspGetAllRects(std::vector<Rect> &rects) = 0;
    virtual bool dspSavePrimaryAsPic(const std::string &sPictureFilePath) = 0;
    virtual void dspGetPrimaryRect(Rect &rect) = 0;
    virtual bool dspSaveVirtualAsPic(const std::string &sPictureFilePath) = 0;
    virtual void dspGetVirtualRect(Rect &rect) = 0;

    virtual bool cbdPutStr(const std::string &s) = 0;
    virtual bool cbdGetStr(std::string &s) = 0;
    
    virtual void kbdKey(unsigned char vk) = 0;
    virtual void kbdCombKey(std::vector<unsigned char> vks) = 0;
    virtual void kbdKeyDown(unsigned char vk) = 0;
    virtual void kbdKeyUp(unsigned char vk) = 0;
    virtual void kbdKeyOn(unsigned char vk) = 0;
    virtual void kbdKeyOff(unsigned char vk) = 0;
    virtual void kbdChar(char ch) = 0;
    virtual void kbdStr(const std::string &s) = 0;
    
    virtual void mseMove(const Point &point, bool absolute = true) = 0;
    virtual void mseClick(const Point &point) = 0;
    virtual void mseDClick(const Point &point) = 0;
    virtual void mseDrag(const Point &srcPoint, const Point &dstPoint) = 0;
    virtual void mseRClick(const Point &point) = 0;
    virtual void mseDRClick(const Point &point) = 0;
    virtual void mseRDrag(const Point &srcPoint, const Point &dstPoint) = 0;
    virtual void mseScroll(const Point &point, int steps) = 0;
    
    virtual bool imgFindRect(const std::vector<Image> &images, Rect &rect, int &index) = 0;
    virtual bool imgFindRect(const std::vector<Image> &images, Rect &rect, int &index, const Rect &searchRect) = 0;
    virtual bool imgFindRect(const std::vector<Image> &images, Rect &rect, int &index, const Point &searchBeginningPoint) = 0;
    virtual bool imgFindAllRects(const Image &image, std::vector<Rect> &rects) = 0;
    virtual bool imgFindAllRects(const Image &image, std::vector<Rect> &rects, const Rect &searchRect) = 0;
    virtual bool imgFindAllRects(const Image &image, std::vector<Rect> &rects, const Point &searchBeginningPoint) = 0;
    virtual bool imgWaitShown(const std::vector<Image> &images, Rect &rect, int &index, unsigned int timeout = INFINITE_TIME) = 0;
    virtual bool imgWaitShown(const std::vector<Image> &images, Rect &rect, int &index, const Rect &searchRect, unsigned int timeout = INFINITE_TIME) = 0;
    virtual bool imgWaitShown(const std::vector<Image> &images, Rect &rect, int &index, const Point &searchBeginningPoint, unsigned int timeout = INFINITE_TIME) = 0;
    virtual bool imgWaitAllShown(const std::vector<Image> &images, std::vector<Rect> &rects, unsigned int timeout = INFINITE_TIME) = 0;
    virtual bool imgWaitAllShown(const std::vector<Image> &images, std::vector<Rect> &rects, const Rect &searchRect, unsigned int timeout = INFINITE_TIME) = 0;
    virtual bool imgWaitAllShown(const std::vector<Image> &images, std::vector<Rect> &rects, const Point &searchBeginningPoint, unsigned int timeout = INFINITE_TIME) = 0;

};

} // namespace GuiISTk {

#endif // #ifndef GUISTK_H
