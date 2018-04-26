#pragma once
#include "stdafx.h"
#include "FileNameTracker.h"

class CWorker
{
public:
    struct ImageInfoData {
        ULONG nWidth;
        ULONG nHeight;
        int x, y;
        BYTE a, r, g, b;
        float fScaleFactor;
    };
    struct DrawingInfoData {
        SCROLLINFO siHorizon;
        SCROLLINFO siVertical;
    };
    struct BITMAPINFO_3DWORD {
        BITMAPINFOHEADER bmih;
        DWORD BitsMask1;
        DWORD BitsMask2;
        DWORD BitsMask3;
    };
    struct ColorB8G8R8A8 {
        BYTE b;
        BYTE g;
        BYTE r;
        BYTE a;
    };

private:
    enum ImageDrawingFlags {
        IDF_IMG_FILE_PATH_CHANGED               = 0x0000001,
        IDF_IMG_BG_COLOR_CHANGED                = 0x0000002,
        IDF_IMG_TRANSPARECY_CHANGED             = 0x0000004,
        IDF_IMG_DC_RECREATED                    = 0x0000008,
    };

public:
    ~CWorker(void);

    static CWorker &GetInstance();

    BOOL CheckImageFilePathValid(const CString &strImageFilePath);
    void OnPaint(CPaintDC &dc);
    void RegisterImageInfoMonitor(HWND hWnd, UINT Msg);
    void RegisterDrawingInfoMonitor(HWND hWnd, UINT Msg);
    void MoveOrigin(int xDelta, int yDelta);
    void SetOriginX(int xPos);
    void SetOriginY(int yPos);
    void SetWatchPoint(int x, int y);
    void SetPictureBgColor(BYTE a, BYTE r, BYTE g, BYTE b);
    void SetGlobalAlpha(BYTE a);
    BYTE GetGlobalAlpha();
    void SetScaleFactor(float fScaleFactor);
    float GetScaleFactor();
    void SetImageFilePath(const CString &strImageFilePath);
    CString GetImageFilePath();
    BOOL ReloadConfig();
    BOOL ShowPreviousPicture();
    BOOL ShowNextPicture();
    BOOL RefreshPictureDir();
    BOOL SaveAsBmpFileWithColor(const CString &sSavedBmpFilePath);
    BOOL SaveAsRawFileArgb32(const CString &sSavedBmpFilePath);
    BOOL SaveAsRawFileRgb16(const CString &sSavedBmpFilePath);
    BOOL SaveAsRawFileYuv16(const CString &sSavedBmpFilePath);
    BOOL SaveAsRawFileYuv12(const CString &sSavedBmpFilePath);
    BOOL SaveAsImageFile(const CString &sMimeType, const CString &sSavedBmpFilePath);
    BOOL CloseCurrentFile();

private:
    CWorker(void);

    void CreateInternalMemDC();
    void SendImageInfoMessage();
    void SendDrawingInfoMessage();
    void DrawOnBgBuffDc(CPaintDC &dc);
    void DrawInternalMemDC();
    BOOL GetPixelAtPos(int x, int y, BYTE &a, BYTE &r, BYTE &g, BYTE &b);
    static BOOL IsPictureFile(const CString &strPathName);
    bool IsBmpFileWithAlpha();

    static BOOL ParseColorRGBAOffset(const char *pColorOrder, int *nRedOffset, int *nGreenOffset, int *nBlueOffset, int *nAlphaOffset);
    static BOOL ParseColorRGBOffset(const char *pColorOrder, int *nRedOffset, int *nGreenOffset, int *nBlueOffset);
    static BOOL SaveBmpToFile_RGBA32(
        const char *pFilePath,
        const void *pBmpBits,
        unsigned long nBmpWidth,
        unsigned long nBmpHeight,
        unsigned long nBmpPitch,
        BOOL bBottomUp,
        const char *pInputColorOrder,
        const char *pOutputColorOrder
    );
    static BOOL SaveBmpToRawFile_RGBA32(
        const char *pFilePath,
        const void *pBmpBits,
        unsigned long nBmpWidth,
        unsigned long nBmpHeight,
        unsigned long nBmpPitch,
        BOOL bBottomUp,
        const char *pInputColorOrder,
        const char *pOutputColorOrder
    );
    static BOOL SaveBmpToRawFile_RGBA16(
        const char *pFilePath,
        const void *pBmpBits,
        unsigned long nBmpWidth,
        unsigned long nBmpHeight,
        unsigned long nBmpPitch,
        BOOL bBottomUp,
        const char *pInputColorOrder,
        const char *pOutputColorOrder
    );
    static BOOL SaveBmpToRawFile_RGBA12(
        const char *pFilePath,
        const void *pBmpBits,
        unsigned long nBmpWidth,
        unsigned long nBmpHeight,
        unsigned long nBmpPitch,
        BOOL bBottomUp,
        const char *pInputColorOrder,
        const char *pOutputColorOrder
    );
    static BOOL SaveBmpToRawFile_YUV16(
        const char *pFilePath,
        const void *pBmpBits,
        unsigned long nBmpWidth,
        unsigned long nBmpHeight,
        unsigned long nBmpPitch,
        BOOL bBottomUp,
        const char *pInputColorOrder
    );
    static BOOL SaveBmpToRawFile_YUV12(
        const char *pFilePath,
        const void *pBmpBits,
        unsigned long nBmpWidth,
        unsigned long nBmpHeight,
        unsigned long nBmpPitch,
        BOOL bBottomUp,
        const char *pInputColorOrder
    );
    static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

private:
    DWORD m_nImageDrawingFlags;

    BYTE m_globalAlpha;
    BYTE m_bgA;
    BYTE m_bgR;
    BYTE m_bgG;
    BYTE m_bgB;
    CString m_strImageFilePath;
    std::vector<unsigned char> m_vBmpWithAlphaFileContent;

    int m_xOriginOffset;
    int m_yOriginOffset;

    CDC *m_pBgBuffDC1;
    CDC *m_pBgBuffDC2;
    CBitmap *m_pBgBuffBitmap1;                  
    CBitmap *m_pBgBuffBitmap2;                  // the size is equal to ChildView.
    UINT m_nBgBuffBitmapWidth1;
    UINT m_nBgBuffBitmapHeight1;
    UINT m_nBgBuffBitmapWidth2;
    UINT m_nBgBuffBitmapHeight2;

    CBitmap *m_pBgPattern;
    CBrush *m_pBgBrush;

    float m_fScaleFactor;
    HDC m_hInternalMemDC;
    HBITMAP m_hInternalMemBmp;
    BITMAPINFO_3DWORD m_BitmapInfo;
    BYTE *m_pInternalBmpBits;

    int m_xWatchPoint;
    int m_yWatchPoint;

    HWND m_hImageInfoMonitorWnd;
    UINT m_ImageInfoMonitorMsg;
    HWND m_hDrawingInfoMonitorWnd;
    UINT m_DrawingInfoMonitorMsg;

    Image *m_pImage;
    ULONG m_nImageWidth;
    ULONG m_nImageHeight;

    CFileNameTracker m_fileNameTracker;

    BOOL m_bExportedBmpBottomUp;
    CString m_sExportedBmpFileColorOrder;
};
