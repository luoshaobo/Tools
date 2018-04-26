#ifndef TK_BITMAP_H__3789239762382385723487376576228435982
#define TK_BITMAP_H__3789239762382385723487376576228435982

namespace TK_Tools {

struct Color_BGRA32 {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};

struct Color32 {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

enum ZOrder {
    ZO_BOTTOM = 0,
    ZO_TOP,
    ZO_NOTOPMOST,
    ZO_TOPMOST,
};

class UsbVideoWindowCapture
{
public:
    UsbVideoWindowCapture();
    ~UsbVideoWindowCapture();

public:
    BOOL DoCapture();

public:
    const void *GetBits() { return m_pBits; }
    const unsigned long GetWidth() { return m_nWidth; }
    const unsigned long GetHeight() { return m_nHeight; }
    const unsigned long GetPitch() { return m_nPitch; }

private:
    void *m_pBits;
    unsigned long m_nWidth;
    unsigned long m_nHeight;
    unsigned long m_nPitch;
};

class WindowBitsCapture
{
public:
    WindowBitsCapture();
    ~WindowBitsCapture();

    BOOL DoCapture(HWND hWnd);
    BOOL DoCapture(HDC hDC, unsigned long nWidth, unsigned long nHeight);
    const void *GetBits() { return m_pBits; }
    const unsigned long GetWidth() { return m_nWidth; }
    const unsigned long GetHeight() { return m_nHeight; }
    const unsigned long GetPitch() { return m_nPitch; }

private:
	BOOL DoCaptureImpl(HDC hDC);

private:
    void *m_pBits;
    unsigned long m_nWidth;
    unsigned long m_nHeight;
    unsigned long m_nPitch;

    HDC m_hMemDC;
    HBITMAP m_hMemBmp;
};

struct LayerBitsCapturePrivData;
class LayerBitsCapture
{
public:
    LayerBitsCapture();
    ~LayerBitsCapture();

    BOOL DoCapture(unsigned int nLayerId, BOOL bBgBuffer);
    const void *GetBits();
    const unsigned long GetWidth();
    const unsigned long GetHeight();
    const unsigned long GetPitch();

private:
	void FreeResources();
	unsigned long GetLayerType(unsigned long nLayerId);

private:
	LayerBitsCapturePrivData *m_pPrivData;
	WindowBitsCapture m_wbc;
};

class DeskBgWnd
{
public:
    static const TCHAR szWindowTitle[];
    static const TCHAR szWindowClass[];

public:
    static DeskBgWnd &GetInstance();

    BOOL BeginWorkThread();
    BOOL EndWorkThread();

    BOOL WaitForWindowReady();
    BOOL WaitForWorkThreadEnd();
    
    BOOL SetWindowRect(const RECT &rect);
    BOOL SetBgColor(const Color32 &color);
    BOOL SetVisible(BOOL bVisible);
    BOOL SetZOrder(HWND hWndInsertAfter);
    BOOL SetZOrder(ZOrder zorder);
    BOOL RedrawWindow();

    const BOOL IsWindowReady() { return m_bWindowReady; }
    const HWND GetWindowHandle() { return m_hWnd; }
    const RECT GetWindowRect() { return m_wndRect; }
    const Color32 GetBgColor() { return m_bgColor; }
    
private:
    DeskBgWnd();
    ~DeskBgWnd();

    static DWORD WINAPI WorkThreadProcStatic(LPVOID lpParameter);
    DWORD WorkThreadProc();

    static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    BOOL m_bWindowReady;
    HANDLE m_hWorkThread;
    HWND m_hWnd;
    RECT m_wndRect;
    Color32 m_bgColor;
};

BOOL PrintAllWindowsInfo();
BOOL PrintWindowsInfo(HWND hWnd);
BOOL FillWindow(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
BOOL FillWindow(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a, long x, long y, unsigned long w, unsigned long h);
BOOL MoveWindow(HWND hWnd, int nX, int nY, int nWidth, int nHeight);
BOOL RedrawWindow(HWND hWnd);
BOOL SetWindowZOrder(HWND hWnd, ZOrder zorder);
BOOL SetWindowZOrderAfter(HWND hWnd, HWND hWndInsertAfter);
BOOL DrawBmpToWindow(HWND hWnd, const std::string &sBmpFilePath, int bx, int by, unsigned int bw, unsigned int bh, int wx, int wy, unsigned int ww, unsigned int wh);

BOOL SaveBmpToFile_RGBA32(
    const char *pFilePath,
    const void *pBmpBits,
    unsigned long nBmpWidth,
    unsigned long nBmpHeight,
    unsigned long nBmpPitch,
    BOOL bBottomUp,
    const char *pColorOrder
);

BOOL GetProcessByName(const std::tstring &sProcessName, HANDLE &hProcess, DWORD &nProcessID);
BOOL KillProcessByName(const std::tstring &sProcessName);

} //namespace TK_Tools {

#endif // #ifndef TK_BITMAP_H__3789239762382385723487376576228435982
