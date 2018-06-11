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

    void intersect(const Rect &other);

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

const unsigned int INFINITE_TIME = (unsigned int)-1;

class IToolkit
{
public:
    virtual ~IToolkit() {}
    
    virtual void delay(unsigned int milliSecond) = 0;
    
    virtual void kbdKeyDown(unsigned char vk) = 0;
    virtual void kbdKeyUp(unsigned char vk) = 0;
    virtual void kbdKeyOn(unsigned char vk) = 0;
    virtual void kbdKeyOff(unsigned char vk) = 0;
    virtual void kbdCtrlA() = 0;
    virtual void kbdCtrlC() = 0;
    virtual void kbdCtrlX() = 0;
    virtual void kbdCtrlV() = 0;
    virtual void kbdChar(char ch) = 0;
    virtual void kbdString(const std::string &s) = 0;
    
    virtual void mouseMove(const Point &point, bool absolute = true) = 0;
    virtual void mouseClick(const Point &point) = 0;
    virtual void moustDoubleClick(const Point &point) = 0;
    virtual void mouseDrag(const Point &srcPoint, const Point &dstPoint) = 0;
    virtual void mouseRightClick(const Point &point) = 0;
    virtual void moustDoubleRightClick(const Point &point) = 0;
    virtual void mouseRightDrag(const Point &srcPoint, const Point &dstPoint) = 0;
    virtual void mouseScroll(const Point &point, int steps) = 0;
    
    virtual bool findImageRect(const Image &image, Rect &rect) = 0;
    virtual bool findImageRect(const Image &image, Rect &rect, const Rect &searchRect) = 0;
    virtual bool findImageRect(const Image &image, Rect &rect, const Point &searchBeginningPoint) = 0;
    
    virtual bool waitImageShown(const Image &image, unsigned int timeout = INFINITE_TIME) = 0;
    virtual bool waitImageShown(const Image &image, const Rect &searchRect, unsigned int timeout = INFINITE_TIME) = 0;
    virtual bool waitImageShown(const Image &image, const Point &searchBeginningPoint, unsigned int timeout = INFINITE_TIME) = 0;
};

} // namespace GuiISTk {

#endif // #ifndef GUISTK_H
