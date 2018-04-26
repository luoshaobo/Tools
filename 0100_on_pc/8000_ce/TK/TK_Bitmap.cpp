#include <windows.h>
#include <Imaging.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TK_Tools.h"
#include "TK_Bitmap.h"
#include <ssw_ILayerManager.h>

#ifdef ASSERT
#undef ASSERT
#endif

#ifndef ASSERT
#define ASSERT          TK_ASSERT
#endif

#ifdef LOG
#undef LOG
#endif

#ifndef LOG
#define LOG             TK_PRINTF
#endif

#ifdef WINCE
#define HGDI_ERROR (LongToHandle(0xFFFFFFFFL))
#endif

#ifdef WINCE
#define RDW_FRAME               0x0000
#endif

namespace TK_Tools {

////////////////////////////////////////////////////////////////////////////////////////////////////
// UsbVideoWindowCapture
//
// IBasicVideo::GetCurrentImage()
//
UsbVideoWindowCapture::UsbVideoWindowCapture()
    : m_pBits(NULL), m_nWidth(0), m_nHeight(0), m_nPitch(0)
{

}

UsbVideoWindowCapture::~UsbVideoWindowCapture()
{

}

BOOL UsbVideoWindowCapture::DoCapture()
{
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageDecoder for WinCE
//
class ImageDecoder
{
public:
    enum ColorFormat {
        CF_ARGB32 = 0,
        CF_RGB24,
        CF_MAX
    };

public:
    ImageDecoder(const std::tstring &sImageFilePath);
    ~ImageDecoder();

public:
    bool Decode();
    bool Scale(uint nWidth, uint nHeight);
    bool Lock();
    bool Unlock();
    HBITMAP CreateBitmap();

public:
    uint GetWidth();
    uint GetHeight();
    uint GetStride();
    void *GetBmpBits();
    ColorFormat GetBmpOrigColorFormat() const { return m_nColorFormat; }
    ColorFormat GetBmpLockedColorFormat() const { return CF_ARGB32; }

private:
    bool COMInit();
    bool COMDeinit();
    bool CreateStreamOnFile(const std::tstring &sImageFilePath, IStream **ppStream);

private:
    IImagingFactory *m_pImagingFactory;
    IBitmapImage *m_pBitmapImage;

    ImageInfo m_ii;
    BitmapData m_bd;
    std::wstring m_sImageFilePath;
    int m_nLockCount;
    ColorFormat m_nColorFormat;
};

#define TK_PIC_READFILE_BLOCK_SIZE      (1024 * 1024 * 2)

ImageDecoder::ImageDecoder(const std::tstring &sImageFilePath)
    : m_pImagingFactory(NULL)
    , m_pBitmapImage(NULL)
    , m_ii()
    , m_bd()
    , m_sImageFilePath(sImageFilePath)
    , m_nLockCount(0)
    , m_nColorFormat(CF_MAX)
{

}

ImageDecoder::~ImageDecoder(void)
{
    Unlock();

    if (m_pBitmapImage != NULL) {
		m_pBitmapImage->Release();
		m_pBitmapImage = NULL;
    }

    COMDeinit();
}

uint ImageDecoder::GetWidth()
{
    bool bFuncResult = true;
    uint nWidth = 0;
    SIZE size;
    HRESULT hr;

    if (bFuncResult) {
        if (m_pBitmapImage == NULL) {
            bFuncResult = false;
        }
    }

    if (bFuncResult) {
        hr = m_pBitmapImage->GetSize(&size);
        if (hr != S_OK) {
            bFuncResult = false;
        }
    }

    if (bFuncResult) {
        nWidth = size.cx;
    }
    
    return nWidth;
}

uint ImageDecoder::GetHeight()
{
    bool bFuncResult = true;
    uint nHeight = 0;
    SIZE size;
    HRESULT hr;

    if (bFuncResult) {
        if (m_pBitmapImage == NULL) {
            bFuncResult = false;
        }
    }

    if (bFuncResult) {
        hr = m_pBitmapImage->GetSize(&size);
        if (hr != S_OK) {
            bFuncResult = false;
        }
    }

    if (bFuncResult) {
        nHeight = size.cy;
    }
    
    return nHeight;
}

uint ImageDecoder::GetStride()
{
    return m_bd.Stride;
}

void *ImageDecoder::GetBmpBits()
{
    return m_bd.Scan0;
}

bool ImageDecoder::COMInit()
{
	HRESULT hr;

	if (m_pImagingFactory == NULL) {
        hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (hr == S_OK) {
            hr = ::CoCreateInstance(CLSID_ImagingFactory, NULL, CLSCTX_INPROC_SERVER,
								    IID_IImagingFactory, reinterpret_cast<void **>(&m_pImagingFactory));
			if (hr != S_OK) {
                ::CoUninitialize();
			}
		}
	}
	return (m_pImagingFactory != NULL);
}

bool ImageDecoder::COMDeinit()
{
    if (m_pImagingFactory != NULL) {
        m_pImagingFactory->Release();
        m_pImagingFactory = NULL;
        ::CoUninitialize();
    }
    
    return true;
}

bool ImageDecoder::CreateStreamOnFile(const std::tstring &sImageFilePath, IStream **ppStream)
{
    bool bFuncRet = true;
	HRESULT hrRet;
	HGLOBAL hGlobal = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwSize, dwToRead, dwRead, dwTotalRead;
	BYTE *pGlobalMem = NULL;
    int nBlockCount = 0;
    BOOL bRet;

    if (bFuncRet) {
        if (ppStream == NULL) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
        hFile = ::CreateFile(sImageFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	    if (hFile == INVALID_HANDLE_VALUE) {
		    LOG("*** ERROR: %s(): CreateFile failed with GLE = %u\n", __FUNCTION__, ::GetLastError());
            bFuncRet = false;
	    }
    }

	if (bFuncRet) {
        dwSize = ::GetFileSize(hFile, NULL);
		if (dwSize == 0xFFFFFFFF) {
			LOG("*** ERROR: %s(): GetFileSize failed with GLE = %u\n", __FUNCTION__, ::GetLastError());
            bFuncRet = false;
		}
	}

	if (bFuncRet) {
        hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
		if (hGlobal == NULL) {
			LOG("*** ERROR: %s(): GlobalAlloc failed with GLE = %u\n", __FUNCTION__, ::GetLastError());
            bFuncRet = false;
		}
	}

	if (bFuncRet) {
        pGlobalMem = reinterpret_cast<BYTE *>(GlobalLock(hGlobal));
		if (pGlobalMem == NULL) {
			LOG("*** ERROR: %s(): GlobalLock failed with GLE = %u\n", __FUNCTION__, ::GetLastError());
            bFuncRet = false;
		}
	}

	if (bFuncRet) {
	    dwTotalRead = 0;
        while (true) {
            if (dwTotalRead == dwSize) {
                break;
            } else if (dwTotalRead < dwSize) {
                dwToRead = TK_PIC_READFILE_BLOCK_SIZE;
                if (dwToRead > dwSize - dwTotalRead) {
                    dwToRead = dwSize - dwTotalRead;
                }
            } else {
                LOG("*** ERROR: %s(): ReadFile error\n", __FUNCTION__);
                bFuncRet = false;
                break;
            }

            bRet = ::ReadFile(hFile, pGlobalMem + dwTotalRead, dwToRead, &dwRead, NULL);
            if (!bRet) {
                LOG("*** ERROR: %s(): ReadFile failed with GLE = %u\n", __FUNCTION__, ::GetLastError());
                bFuncRet = false;
                break;
            } else if (dwRead == 0){
                LOG("*** ERROR: %s(): ReadFile error\n", __FUNCTION__);
                bFuncRet = false;
                break;
            }
            dwTotalRead += dwRead;
        }
    }

	if (bFuncRet) {
        GlobalUnlock(hGlobal);
        pGlobalMem = NULL;

        hrRet = ::CreateStreamOnHGlobal(hGlobal, TRUE, ppStream);
		if (hrRet != S_OK) {
			LOG("*** ERROR: %s(): CreateStreamOnHGlobal failed with GLE = %u\n", __FUNCTION__, ::GetLastError());
            hrRet = ::GetLastError();
            bFuncRet = false;
		}
	}

	if (!bFuncRet) {
		if (pGlobalMem != NULL) {
            GlobalUnlock(hGlobal);
		}
		if (hGlobal != NULL) {
            GlobalFree(hGlobal);
            hGlobal = NULL;
		}
	}
	if (hFile != INVALID_HANDLE_VALUE) {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
	}
	return bFuncRet;
}

bool ImageDecoder::Decode()
{
    bool bFuncRet = true;
	HRESULT hr;
	IStream *pStream = NULL;
	IImageDecoder *pImageDecoder = NULL;
	IImageSink *pImageSink = NULL;
    bool bRet;

    if (bFuncRet) {
	    bRet = COMInit();
	    if (!bRet) {
		    LOG("*** ERROR: %s(): COMInit() failed\n", __FUNCTION__);
            bFuncRet = false;
	    }
    }

    if (bFuncRet) {
	    bRet = CreateStreamOnFile(m_sImageFilePath.c_str(), &pStream);
	    if (!bRet) {
		    LOG("*** ERROR: %s(): CreateStreamOnFile() failed\n", __FUNCTION__);
            bFuncRet = false;
	    }
    }

    if (bFuncRet) {
		hr = m_pImagingFactory->CreateImageDecoder(pStream, DecoderInitFlagBuiltIn1st, &pImageDecoder);
		if (hr != S_OK) {
			LOG("*** ERROR: %s(): CreateImageDecoder failed, hr: 0x%08x\n", __FUNCTION__, hr);
            bFuncRet = false;
		}
	}

    if (bFuncRet) {
		pImageDecoder->GetImageInfo(&m_ii);

        if ((m_ii.PixelFormat & PixelFormatAlpha) != 0) {
            hr = m_pImagingFactory->CreateNewBitmap(m_ii.Width, m_ii.Height, PixelFormat32bppARGB, &m_pBitmapImage);
            m_nColorFormat = CF_ARGB32;
        } else {
            hr = m_pImagingFactory->CreateNewBitmap(m_ii.Width, m_ii.Height, PixelFormat24bppRGB, &m_pBitmapImage);
            m_nColorFormat = CF_RGB24;
        }
		if (hr != S_OK) {
			LOG("*** ERROR: %s(): CreateNewBitmap failed, hr: 0x%08x\n", __FUNCTION__, hr);
            bFuncRet = false;
		}
	}

    if (bFuncRet) {
		hr = m_pBitmapImage->QueryInterface(IID_IImageSink, reinterpret_cast<void **>(&pImageSink));
		if (hr != S_OK) {
			LOG("*** ERROR: %s(): QueryInterface for IImageSink failed, hr: 0x%08x\n", __FUNCTION__, hr);
            bFuncRet = false;
		}
	}

    if (bFuncRet) {
		hr = pImageDecoder->BeginDecode(pImageSink, NULL);
		if (hr != S_OK) {
			LOG("*** ERROR: %s(): BeginDecode failed, hr: 0x%08x\n", __FUNCTION__, hr);
            bFuncRet = false;
		}
	}

    if (bFuncRet) {
		do {
			hr = pImageDecoder->Decode();
		} while (hr == E_PENDING);

		hr = pImageDecoder->EndDecode(hr);
		if (hr != S_OK) {
			LOG("*** ERROR: %s(): Decoding failed, hr: 0x%08x\n", __FUNCTION__, hr);
            bFuncRet = false;
		}
	}

	if (pImageSink != NULL) {
		pImageSink->Release();
        pImageSink = NULL;
	}
	if (pImageDecoder != NULL) {
		pImageDecoder->Release();
        pImageSink = NULL;
	}
	if (pStream != NULL) {
		pStream->Release();
        pStream = NULL;
	}

	return bFuncRet;
}

bool ImageDecoder::Scale(uint nWidth, uint nHeight)
{
    bool bFuncRet = true;
	HRESULT hr;
	IBasicBitmapOps *pBBitmapOps = NULL;

    if (bFuncRet) {
        if (m_pBitmapImage == NULL) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
    	hr = m_pBitmapImage->QueryInterface(IID_IBasicBitmapOps, reinterpret_cast<void **>(&pBBitmapOps));
	    if (hr != S_OK) {
		    LOG("*** ERROR: %s(): QueryInterface for IBasicBitmapOps failed, hr: 0x%08x\n", __FUNCTION__, hr);
            bFuncRet = false;
	    }
    }

    if (bFuncRet) {
		m_pBitmapImage->Release();
		m_pBitmapImage = NULL;

		hr = pBBitmapOps->Resize(nWidth, nHeight, PixelFormatDontCare, InterpolationHintDefault, &m_pBitmapImage);
		if (hr != S_OK) {
			LOG("*** ERROR: %s(): Resizing failed, hr: 0x%08x\n", __FUNCTION__, hr);
            bFuncRet = false;
		}
	}

	if (pBBitmapOps != NULL) {
		pBBitmapOps->Release();
		pBBitmapOps = NULL;
	}

	return bFuncRet;
}

bool ImageDecoder::Lock()
{
    bool bFuncRet = true;
    bool bRetDirectly = false;
    HRESULT hr;

    if (bFuncRet) {
        if (m_nLockCount >= 1) {
            m_nLockCount++;
            bRetDirectly = true;
            bFuncRet = true;
        }
    }

    if (!bRetDirectly) {
        if (bFuncRet) {
            if (m_pBitmapImage == NULL) {
                bFuncRet = false;
            }
        }

        if (bFuncRet) {
            hr = m_pBitmapImage->LockBits(NULL, ImageLockModeRead, PixelFormat32bppARGB, &m_bd);
            if (hr != S_OK) {
                bFuncRet = false;
            }
        }

        if (bFuncRet) {
            m_nLockCount++;
        }
    }

    return bFuncRet;
}

bool ImageDecoder::Unlock()
{
    bool bFuncRet = true;
    bool bRetDirectly = false;
    HRESULT hr;

    if (bFuncRet) {
        if (m_nLockCount <= 0) {
            m_nLockCount = 0;
            bRetDirectly = true;
            bFuncRet = true;
        } else if (m_nLockCount > 1) {
            m_nLockCount--;
            bRetDirectly = true;
            bFuncRet = true;
        }
    }

    if (!bRetDirectly) {
        if (bFuncRet) {
            if (m_pBitmapImage == NULL) {
                bFuncRet = false;
            }
        }

        if (bFuncRet) {
            hr = m_pBitmapImage->UnlockBits(&m_bd);
            if( hr != S_OK) {
                bFuncRet = false;
            }
        }

        if (bFuncRet) {
            m_nLockCount--;
        }
    }

    return bFuncRet;
}

HBITMAP ImageDecoder::CreateBitmap()
{
    bool bFuncRet = true;
    HBITMAP hBitMap = NULL;
    bool bRet;

    if (bFuncRet) {
        bRet = Lock();
        if (!bRet) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
        hBitMap = ::CreateBitmap(
            m_bd.Width, m_bd.Height,
            1,
            32,
            m_bd.Scan0
        );
    }

    return hBitMap;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//

WindowBitsCapture::WindowBitsCapture()
    : m_pBits(NULL)
    , m_nWidth(0)
    , m_nHeight(0)
    , m_nPitch(0)
    , m_hMemDC(NULL)
    , m_hMemBmp(NULL) 
{

}

WindowBitsCapture::~WindowBitsCapture()
{
    if (m_hMemDC != NULL) {
        DeleteDC(m_hMemDC);
        m_hMemDC = NULL;
    }

    if (m_hMemBmp != NULL) {
        DeleteObject(m_hMemBmp);
        m_hMemBmp = NULL;
    }
}

BOOL WindowBitsCapture::DoCapture(HWND hWnd)
{
    BOOL bSuc;
    RECT rectWindow;
    
    bSuc = ::GetWindowRect(hWnd, &rectWindow);
    if (!bSuc) {
        LOG("*** ERROR: %s(): ::GetWindowRect(hWnd, &rectWindow) is failed!\n", __FUNCTION__);
        return FALSE;
    }
    m_nWidth = rectWindow.right - rectWindow.left;
    m_nHeight = rectWindow.bottom - rectWindow.top;
    m_nPitch = m_nWidth * 4;

    bSuc = DoCaptureImpl(::GetWindowDC(hWnd));
    if (!bSuc) {
        LOG("*** ERROR: %s(): DoCaptureImpl(::GetWindowDC(hWnd)) is failed!\n", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
}

BOOL WindowBitsCapture::DoCapture(HDC hDC, unsigned long nWidth, unsigned long nHeight)
{
    BOOL bSuc;
    
    m_nWidth = nWidth;
    m_nHeight = nHeight;
    m_nPitch = m_nWidth * 4;

    bSuc = DoCaptureImpl(hDC);
    if (!bSuc) {
        LOG("*** ERROR: %s(): DoCaptureImpl(hDC) is failed!\n", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
}

BOOL WindowBitsCapture::DoCaptureImpl(HDC hDC)
{
    BOOL bSuc;
    
    if (m_hMemDC != NULL) {
        DeleteDC(m_hMemDC);
        m_hMemDC = NULL;
    }
    m_hMemDC = ::CreateCompatibleDC(NULL);
    if (m_hMemDC == NULL) {
        LOG("*** ERROR: %s(): ::CreateCompatibleDC(NULL) is failed!\n", __FUNCTION__);
        return FALSE;
    }

    if (m_hMemBmp != NULL) {
        DeleteObject(m_hMemBmp);
        m_hMemBmp = NULL;
    }
    unsigned char *pFileHeaderInfo = new unsigned char[sizeof(BITMAPINFOHEADER) + sizeof(DWORD) * 3];
    ASSERT((unsigned long)pFileHeaderInfo % 4 == 0);
    if (pFileHeaderInfo == NULL) {
        LOG("*** ERROR: %s(): new unsigned char[sizeof(BITMAPINFOHEADER) + sizeof(DWORD) * 3] is failed!\n", __FUNCTION__);
        return FALSE;
    }
    AutoFreeArrayPtr<unsigned char> afapFileHeaderInfo(pFileHeaderInfo);
    BITMAPINFOHEADER &bmih = *(BITMAPINFOHEADER *)pFileHeaderInfo;
    memset(&bmih, 0, sizeof(BITMAPINFOHEADER));
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = m_nWidth;
    bmih.biHeight = m_nHeight;
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = BI_BITFIELDS;
    bmih.biSizeImage = m_nPitch * bmih.biHeight;
    bmih.biXPelsPerMeter = 0;
    bmih.biYPelsPerMeter = 0;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;
    DWORD *pBitsFields = (DWORD *)(&bmih + 1);
    pBitsFields[0] = 0x00FF0000;              // r
    pBitsFields[1] = 0x0000FF00;              // g
    pBitsFields[2] = 0x000000FF;              // b
    m_hMemBmp = ::CreateDIBSection(
        m_hMemDC,
        (CONST BITMAPINFO *)pFileHeaderInfo,
        DIB_RGB_COLORS,
        (VOID **)&m_pBits,
        NULL,
        NULL
    );
    if (m_hMemBmp == NULL || m_pBits == NULL) {
        LOG("*** ERROR: %s(): ::CreateDIBSection() is failed!\n", __FUNCTION__);
        return FALSE;
    }

    HGDIOBJ hGdiObj = ::SelectObject(m_hMemDC, m_hMemBmp);
    if (hGdiObj == HGDI_ERROR) {
        LOG("*** ERROR: %s(): ::SelectObject(m_hMemDC, m_hMemBmp) is failed!\n", __FUNCTION__);
        return FALSE;
    }

    bSuc = ::BitBlt(
        m_hMemDC,
        0,
        0,
        bmih.biWidth,
        abs(bmih.biHeight),
        hDC,
        0,
        0,
        SRCCOPY
    );
    if (!bSuc) {
        LOG("*** ERROR: %s(): ::BitBlt() is failed!\n", __FUNCTION__);
        return FALSE;
    }

#ifndef WINCE
    ::GdiFlush();
#endif

    return TRUE;
}

BOOL DrawBmpToWindow(HWND hWnd, const std::string &sBmpFilePath, 
    int bx, int by, unsigned int bw, unsigned int bh, 
    int wx, int wy, unsigned int ww, unsigned int wh)
{
    BOOL bRet = FALSE;
    DWORD nLastErrorCode = 0;
    BOOL bSuc;
    const std::wstring wsBmpFilePath = str2wstr(sBmpFilePath);
    HBITMAP hBmpFromFile = NULL;
    HDC hMemDC = NULL;
    HDC hWindowDC = NULL;
    RECT rectWindow;
    ImageDecoder oImageDecoder(str2tstr(sBmpFilePath));
    uint nImageWidth = 0;
    uint nImageHeight = 0;

    bSuc = ::GetWindowRect(hWnd, &rectWindow);
    if (!bSuc) {
        nLastErrorCode = ::GetLastError();
        LOG("*** ERROR: %s(): ::GetWindowRect(hWnd, &rectWindow) is failed!\n", __FUNCTION__);
        goto ERR1;
    }
    if (ww == TK_MAX_UINT) {
        ww = (rectWindow.right - rectWindow.left) > wx ? (rectWindow.right - rectWindow.left - wx) : 0;
    }
    if (wh == TK_MAX_UINT) {
        wh = (rectWindow.bottom - rectWindow.top) > wy ? (rectWindow.bottom - rectWindow.top - wy) : 0;
    }

    if (!oImageDecoder.Decode()) {
        LOG("*** ERROR: %s(): oImageDecoder.Decode(nWidth, nHeight) is failed!\n", __FUNCTION__);
        goto ERR1;
    }
    nImageWidth = oImageDecoder.GetWidth();
    nImageHeight = oImageDecoder.GetHeight();
    hBmpFromFile = oImageDecoder.CreateBitmap();
    if (hBmpFromFile == NULL) {
        LOG("*** ERROR: %s(): oImageDecoder.CreateBitmap() is failed!\n", __FUNCTION__);
        goto ERR1;
    }
    if (bw == TK_MAX_UINT) {
        bw = nImageWidth > bx ? (nImageWidth - bx) : 0;
    }
    if (bh == TK_MAX_UINT) {
        bh = nImageHeight > by ? (nImageHeight - by) : 0;
    }

    if (ww == TK_MAX_UINT - 1) {
        ww = bw;
    }
    if (wh == TK_MAX_UINT - 1) {
        wh = bh;
    }

    hWindowDC = ::GetWindowDC(hWnd);
    if (hWindowDC == NULL) {
        nLastErrorCode = ::GetLastError();
        LOG("*** ERROR: %s(): ::GetWindowDC(hWnd) is failed!\n", __FUNCTION__);
        goto ERR1;
    }

    hMemDC = ::CreateCompatibleDC(NULL);
    if (hMemDC == NULL) {
        nLastErrorCode = ::GetLastError();
        LOG("*** ERROR: %s(): ::CreateCompatibleDC(NULL) is failed!\n", __FUNCTION__);
        goto ERR1;
    }

    HGDIOBJ hGdiObj = ::SelectObject(hMemDC, hBmpFromFile);
    if (hGdiObj == HGDI_ERROR) {
        nLastErrorCode = ::GetLastError();
        LOG("*** ERROR: %s(): ::SelectObject(hMemDC, hBmpFromFile) is failed!\n", __FUNCTION__);
        goto ERR1;
    }

    if (oImageDecoder.GetBmpOrigColorFormat() == ImageDecoder::CF_RGB24) {
        bSuc = ::StretchBlt(
            hWindowDC,
            wx,
            wy,
            ww,
            wh,
            hMemDC,
            bx,
            by,
            bw,
            bh,
            SRCCOPY
        );
    } else if (oImageDecoder.GetBmpOrigColorFormat() == ImageDecoder::CF_ARGB32) {
        BLENDFUNCTION bf;
        memset(&bf, 0, sizeof(BLENDFUNCTION));
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat = AC_SRC_ALPHA;
        bSuc = ::AlphaBlend(
            hWindowDC,
            wx,
            wy,
            ww,
            wh,
            hMemDC,
            bx,
            by,
            bw,
            bh,
            bf
        );
    } else {
        LOG("*** ERROR: %s(): The bitmap format is not supported.\n", __FUNCTION__);
        goto ERR1;
    }
    if (!bSuc) {
        nLastErrorCode = ::GetLastError();
        LOG("*** ERROR: %s(): ::StretchBlt() is failed!\n", __FUNCTION__);
        return FALSE;
    }

#ifndef WINCE
    ::GdiFlush();
#endif

    bRet = TRUE;
ERR1:
    if (hMemDC != NULL && hGdiObj != NULL) {
        ::SelectObject(hMemDC, hGdiObj);
        hGdiObj = NULL;
    }
    if (hMemDC != NULL) {
        ::DeleteDC(hMemDC);
        hMemDC = NULL;
    }
    if (hWindowDC != NULL) {
        ::DeleteDC(hWindowDC);
        hWindowDC = NULL;
    }
    if (hBmpFromFile != NULL) {
        ::DeleteObject(hBmpFromFile);
        hBmpFromFile = NULL;
    }
    return bRet;
}

struct LayerBitsCapturePrivData
{
    ILayerManagerData_sPtr pLayerManager;
    LPDIRECTDRAW pDD;
    LPDIRECTDRAWSURFACE pDDSPrimary;
    LPDIRECTDRAWSURFACE pDDSOverlay;
    LPDIRECTDRAWSURFACE pDDS;
    DDSURFACEDESC ddsd;
};

LayerBitsCapture::LayerBitsCapture()
    : m_pPrivData(NULL)
{
    m_pPrivData = new LayerBitsCapturePrivData;
    ASSERT(m_pPrivData != NULL);
    memset(m_pPrivData, 0, sizeof(LayerBitsCapturePrivData));
}

LayerBitsCapture::~LayerBitsCapture()
{
    ASSERT(m_pPrivData != NULL);
    
    FreeResources();
    delete m_pPrivData;
}

void LayerBitsCapture::FreeResources()
{
    ASSERT(m_pPrivData != NULL);
    
    if (m_pPrivData->pDDS != NULL) {
        m_pPrivData->pDDS->Unlock(0);
        m_pPrivData->pDDS = NULL;
    }
    
    if (m_pPrivData->pLayerManager != NULL) {
        ILayerManager_Deinit(m_pPrivData->pLayerManager);
        m_pPrivData->pLayerManager = NULL;
    }

    memset(m_pPrivData, 0, sizeof(LayerBitsCapturePrivData));
}

//	typedef enum 
//	{
//	    LayerIDHmi = 0,
//	    LayerIDAlpha,
//	    LayerIDMapView1,
//	    LayerIDBrowser,
//	    LayerIDVideo1,
//	    LayerIDVideo2,
//	    LayerIDSplashscreen,
//	    LayerIDCustom1,
//	    LayerIDCustom2,
//	    LayerIDCustom3,
//	    LayerIDNum
//	} ILayerManager_LayerEnum;
//	typedef enum
//	{
//	    OGLES = 0,
//	    GDI, 
//	    VIDEO,
//	    CUSTOM1,
//	    CUSTOM2,
//	    CUSTOM3,
//	} ILayerManager_TypeEnum;
//
// NOTE: 
// From:
//     bool_t ILayerManager::CreateLayer(LayerEnum eLayer, TypeEnum eType);
// we can see that:
// 1)  For these layers:
//         LayerIDHmi
//         LayerIDVideo1
//         LayerIDCustom1
//         LayerIDCustom2
//         LayerIDCustom3
//     LayerManager will create a DirectDraw surface for each of them respectively.
// 2)  For a layer with one of these types:
//         OGLES
//         VIDEO
//         CUSTOM1
//     if it is in 1), LayerManager will create a DirectDraw surface for it, or else LayerManager 
//     will create a window with OpenGL on primary surface (widows desttop) for it.
// 3) Other layer types are not supported.
// 4) In fact that only HMI and Map uses the Layer Manager, and Map uses OpenGL.

unsigned long LayerBitsCapture::GetLayerType(unsigned long nLayerId)
{
    unsigned long nLayerType = (unsigned long)-1;
    
    switch (nLayerId) {
    case LayerIDHmi:
        nLayerType = (unsigned long)OGLES;
        break;
    case LayerIDAlpha:
        nLayerType = (unsigned long)OGLES;
        break;
    case LayerIDMapView1:
        nLayerType = (unsigned long)OGLES;
        break;
    case LayerIDBrowser:
        nLayerType = (unsigned long)GDI;
        break;
    case LayerIDVideo1:
    case LayerIDVideo2:
    case LayerIDSplashscreen:
        nLayerType = (unsigned long)VIDEO;
        break;
    case LayerIDCustom1:
        nLayerType = (unsigned long)CUSTOM1;
        break;
    case LayerIDCustom2:
        nLayerType = (unsigned long)CUSTOM2;
        break;
    case LayerIDCustom3:
        nLayerType = (unsigned long)CUSTOM3;
        break;
    default:
        break;
    }

    return nLayerType;
}

BOOL LayerBitsCapture::DoCapture(unsigned int nLayerId, BOOL bBgBuffer)
{
	BOOL bRet = FALSE;
	bool_t bSuc = FALSE;
    HRESULT hRet;
    HDC hDC = NULL;
    unsigned long nLayerType;
    ILayerManager *pLayerManager = NULL;
    BOOL bLayerCreated = FALSE;

    FreeResources();

    if (nLayerId >= LayerIDNum) {
        LOG("*** ERROR: %s(): nLayerID is too large!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    nLayerType = GetLayerType(nLayerId);
    if (nLayerType == (unsigned long)-1) {
        LOG("*** ERROR: %s(): GetLayerType(nLayerId) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    bSuc = ILayerManager_Init(&m_pPrivData->pLayerManager);
    if (!bSuc || m_pPrivData->pLayerManager == NULL) {
        LOG("*** ERROR: %s(): ILayerManager_Init(&m_pPrivData->pLayerManager) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }
    pLayerManager = (ILayerManager*)m_pPrivData->pLayerManager;
    
//	    bSuc = pLayerManager->CreateLayer((LayerEnum)nLayerId, (TypeEnum)nLayerType);
//	    if (!bSuc) {
//	        LOG("*** ERROR: %s(): pLayerManager->CreateLayer((LayerEnum)nLayerId, (TypeEnum)nLayerType) is failed!\n", __FUNCTION__);
//	        ASSERT(0);
//	        bRet = FALSE;
//	        goto FAILED;
//	    }
//	    bLayerCreated = TRUE;
    
    pLayerManager->LayerGetDDSurface((LayerEnum)nLayerId, &m_pPrivData->pDD, &m_pPrivData->pDDSPrimary, &m_pPrivData->pDDSOverlay);
    if (m_pPrivData->pDD == NULL || m_pPrivData->pDDSPrimary == NULL || m_pPrivData->pDDSOverlay == NULL) {
        LOG("*** ERROR: %s(): pLayerManager->LayerGetDDSurface(nLayerId, &m_pPrivData->pDD, &m_pPrivData->pDDSPrimary, &m_pPrivData->pDDSOverlay) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    m_pPrivData->pDDS = bBgBuffer ? m_pPrivData->pDDSPrimary : m_pPrivData->pDDSOverlay;
    
    memset(&m_pPrivData->ddsd, 0, sizeof(DDSURFACEDESC));
    m_pPrivData->ddsd.dwSize = sizeof(DDSURFACEDESC);
    hRet = m_pPrivData->pDDS->GetSurfaceDesc(&m_pPrivData->ddsd);
    if (hRet != DD_OK)
    {
        LOG("*** ERROR: %s(): pDDS->GetSurfaceDesc(&m_pPrivData->ddsd) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    hRet = m_pPrivData->pDDS->Lock(0, &m_pPrivData->ddsd, DDLOCK_WAITNOTBUSY, 0);
    if (hRet != DD_OK || m_pPrivData->ddsd.lpSurface == NULL)
    {
        LOG("*** ERROR: %s(): pDDS->Lock(0, &m_pPrivData->ddsd, DDLOCK_WAITNOTBUSY, 0) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }
    
    hRet = m_pPrivData->pDDS->Unlock(0);
    if (hRet != DD_OK)
    {
        LOG("*** ERROR: %s(): m_pPrivData->pDDS->Unlock(0) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }
    
    hRet = m_pPrivData->pDDS->GetDC(&hDC);
    if (hRet != DD_OK || hDC == NULL)
    {
        LOG("*** ERROR: %s(): m_pPrivData->pDDS->GetDC(&hDC) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }
    
    bRet = m_wbc.DoCapture(hDC, m_pPrivData->ddsd.dwWidth, m_pPrivData->ddsd.dwHeight);
    if (!bRet) {
        LOG("*** ERROR: %s(): m_wbc.DoCapture(hDC, m_pPrivData->ddsd.dwWidth, m_pPrivData->ddsd.dwHeight) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    bRet = TRUE;

FAILED:
    if (m_pPrivData->pDDS != NULL && hDC != NULL) {
        m_pPrivData->pDDS->ReleaseDC(hDC);
        hDC = NULL;
    }
    if (pLayerManager != NULL && bLayerCreated) {
        pLayerManager->DestroyLayer((LayerEnum)nLayerId);
        bLayerCreated = FALSE;
    }
    return bRet;
}

const void *LayerBitsCapture::GetBits()
{
    return m_wbc.GetBits();
}

const unsigned long LayerBitsCapture::GetWidth()
{
    return m_wbc.GetWidth();
}

const unsigned long LayerBitsCapture::GetHeight()
{
    return m_wbc.GetHeight();
}

const unsigned long LayerBitsCapture::GetPitch()
{
    return m_wbc.GetPitch();
}

BOOL ParseColor32Offset(const char *pColorOrder, int *nRedOffset, int *nGreenOffset, int *nBlueOffset, int *nAlphaOffset)
{
    const char *pCh;

    ASSERT(nRedOffset != NULL);
    ASSERT(nGreenOffset != NULL);
    ASSERT(nBlueOffset != NULL);
    ASSERT(nAlphaOffset != NULL);

    if (pColorOrder == NULL) {
        pColorOrder = "bgra";
    }

    pCh = strchr(pColorOrder, 'r');
    if (pCh == NULL) {
        goto FAILED;
    }
    *nRedOffset = pCh - pColorOrder;
    if (*nRedOffset >= 4) {
        goto FAILED;
    }

    pCh = strchr(pColorOrder, 'g');
    if (pCh == NULL) {
        goto FAILED;
    }
    *nGreenOffset = pCh - pColorOrder;
    if (*nGreenOffset >= 4) {
        goto FAILED;
    }

    pCh = strchr(pColorOrder, 'b');
    if (pCh == NULL) {
        goto FAILED;
    }
    *nBlueOffset = pCh - pColorOrder;
    if (*nBlueOffset >= 4) {
        goto FAILED;
    }

    pCh = strchr(pColorOrder, 'a');
    if (pCh == NULL) {
        goto FAILED;
    }
    *nAlphaOffset = pCh - pColorOrder;
    if (*nAlphaOffset >= 4) {
        goto FAILED;
    }

    return TRUE;

FAILED:
    *nRedOffset = 2;
    *nGreenOffset = 1;
    *nBlueOffset = 0;
    *nAlphaOffset = 3;
    return FALSE;
}

BOOL SaveBmpToFile_RGBA32(
    const char *pFilePath,
    const void *pBmpBits,
    unsigned long nBmpWidth,
    unsigned long nBmpHeight,
    unsigned long nBmpPitch,
    BOOL bBottomUp,
    const char *pColorOrder
)
{
    BOOL bRet = FALSE;
    BOOL bSuc = FALSE;
    int nOffsetR, nOffsetG, nOffsetB, nOffsetA;
    BITMAPFILEHEADER *pbmfh = NULL;
    unsigned long nBmfh;
    unsigned char *pFileHeaderInfo = NULL;
    unsigned long nFileHeaderInfo;
    BITMAPINFOHEADER *pbmih = NULL;
    DWORD *pBitsFields = NULL;
    unsigned char *pLineBuf = NULL;
    FILE *pFile = NULL;
    size_t nWritten;
    unsigned long nBmpFileBitsPitch = nBmpWidth * 4;
    unsigned long nBmpFileBitsSize = nBmpFileBitsPitch * nBmpHeight;
    unsigned long i, j;
    Color_BGRA32 *pDstPixel;
    unsigned char *pSrcPixel;

    ASSERT(pFilePath != NULL);
    ASSERT(pBmpBits != NULL);

    bSuc = ParseColor32Offset(pColorOrder, &nOffsetR, &nOffsetG, &nOffsetB, &nOffsetA);
    if (!bSuc) {
        LOG("*** ERROR: %s(): ParseColor32Offset() is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    nFileHeaderInfo = sizeof(BITMAPINFOHEADER) + sizeof(DWORD) * 3;
    pFileHeaderInfo = (unsigned char *)malloc(nFileHeaderInfo);
    ASSERT((unsigned long)pFileHeaderInfo % 4 == 0);
    if (pFileHeaderInfo == NULL) {
        LOG("*** ERROR: %s(): malloc(nFileHeaderInfo) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }
    pbmih = (BITMAPINFOHEADER *)pFileHeaderInfo;
    memset(pbmih, 0, sizeof(BITMAPINFOHEADER));
    pbmih->biSize = sizeof(BITMAPINFOHEADER);
    pbmih->biWidth = nBmpWidth;
    pbmih->biHeight = bBottomUp ? nBmpHeight : (0 - nBmpHeight);
    pbmih->biPlanes = 1;
    pbmih->biBitCount = 32;
    pbmih->biCompression = BI_BITFIELDS;
    pbmih->biSizeImage = nBmpFileBitsSize;
    pbmih->biXPelsPerMeter = 0;
    pbmih->biYPelsPerMeter = 0;
    pbmih->biClrUsed = 0;
    pbmih->biClrImportant = 0;
    pBitsFields = (DWORD *)(pbmih + 1);
    memset(pBitsFields, 0, sizeof(DWORD) * 3);
    pBitsFields[0] = 0x00FF0000;              // r
    pBitsFields[1] = 0x0000FF00;              // g
    pBitsFields[2] = 0x000000FF;              // b

    nBmfh = sizeof(BITMAPFILEHEADER);
    pbmfh = (BITMAPFILEHEADER *)malloc(nBmfh);
    ASSERT((unsigned long)pbmfh % 4 == 0);
    if (pbmfh == NULL) {
        LOG("*** ERROR: %s(): malloc(nBmfh) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }
    memset(pbmfh, 0, nBmfh);
    pbmfh->bfType = 'B' | ('M' << 8);
    pbmfh->bfSize = nBmfh + nFileHeaderInfo + nBmpFileBitsSize;
    pbmfh->bfReserved1 = 0;
    pbmfh->bfReserved2 = 0;
    pbmfh->bfOffBits = nBmfh + nFileHeaderInfo;

    pLineBuf = (unsigned char *)malloc(nBmpFileBitsPitch);
    ASSERT((unsigned long)pLineBuf % 4 == 0);
    if (pLineBuf == NULL) {
        LOG("*** ERROR: %s(): malloc(nBmpFileBitsPitch) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    pFile = fopen(pFilePath, "w+b");
    if (pFile == NULL) {
        LOG("*** ERROR: %s(): fopen(pFilePath, \"w+b\") is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    nWritten = fwrite(pbmfh, 1, nBmfh, pFile);
    if (nWritten < nBmfh) {
        LOG("*** ERROR: %s(): fwrite(pbmfh, 1, nBmfh, pFile) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    nWritten = fwrite(pFileHeaderInfo, 1, nFileHeaderInfo, pFile);
    if (nWritten < nFileHeaderInfo) {
        LOG("*** ERROR: %s(): fwrite(pbmfh, 1, nFileHeaderInfo, pFile) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    for (i = 0; i < nBmpHeight; i++) {
        for (j = 0; j < nBmpWidth; j++) {
            pDstPixel = (Color_BGRA32 *)((unsigned char *)pLineBuf + j * 4);
            pSrcPixel = (unsigned char *)((unsigned char *)pBmpBits + nBmpPitch * i + j * 4);
            pDstPixel->r = pSrcPixel[nOffsetR];
            pDstPixel->g = pSrcPixel[nOffsetG];
            pDstPixel->b = pSrcPixel[nOffsetB];
            pDstPixel->a = pSrcPixel[nOffsetA];
        }
        nWritten = fwrite(pLineBuf, 1, nBmpFileBitsPitch, pFile);
        if (nWritten < nBmpFileBitsPitch) {
            LOG("*** ERROR: %s(): fwrite(pLineBuf, 1, nBmpFileBitsPitch, pFile) is failed!\n", __FUNCTION__);
            bRet = FALSE;
            goto FAILED;
        }
    }

    bRet = TRUE;

FAILED:
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }
    if (pLineBuf != NULL) {
        free(pLineBuf);
        pLineBuf = NULL;
    }
    if (pFileHeaderInfo != NULL) {
        free(pFileHeaderInfo);
        pFileHeaderInfo = NULL;
    }
    if (pbmfh != NULL) {
        free(pbmfh);
        pbmfh = NULL;
    }

    return bRet;
}

class EnumAllWindowsHelper
{
public:
    static BOOL EnumAllWindows();
    static BOOL EnumWindow(HWND hWnd);

    const unsigned long GetSessionId() { return m_nSessionId; }

private:
    EnumAllWindowsHelper(HWND hWndForSearch = NULL);
    ~EnumAllWindowsHelper();
    
    static DWORD WINAPI WorkThreadProcStatic(LPVOID lpParameter);
    DWORD WorkThreadProc();
    static BOOL CALLBACK EnumWindowsProcStatic(HWND hWnd, LPARAM lParam);
    BOOL EnumWindowsProc(HWND hWnd);
    
    std::string GenenrateWindowStyleStr(unsigned long nStyle);
    std::string GenenrateWindowExStyleStr(unsigned long nExStyle);

private:
    static unsigned long m_nGlobalSessionId;
    unsigned long m_nSessionId;
    HWND m_hWndForSearch;
};

unsigned long EnumAllWindowsHelper::m_nGlobalSessionId = 0;

EnumAllWindowsHelper::EnumAllWindowsHelper(HWND hWndForSearch /*= NULL*/) : m_hWndForSearch(hWndForSearch), m_nSessionId(0)
{
    m_nSessionId = m_nGlobalSessionId++;
}

EnumAllWindowsHelper::~EnumAllWindowsHelper()
{

}

DWORD WINAPI EnumAllWindowsHelper::WorkThreadProcStatic(LPVOID lpParameter)
{
    EnumAllWindowsHelper *pThis = (EnumAllWindowsHelper *)lpParameter;
    
    ASSERT(pThis != NULL);
    
    return pThis->WorkThreadProc();
}

DWORD EnumAllWindowsHelper::WorkThreadProc()
{
    DWORD nRet;

    TK_PRINTF("EnumAllWindowsHelper: session[%u]: windows enumeration begins.\n", m_nSessionId);
    nRet = ::EnumWindows(&EnumWindowsProcStatic, (LPARAM)this);
    TK_PRINTF("EnumAllWindowsHelper: session[%u]: windows enumeration ends.\n", m_nSessionId);
    delete this;
    
    return 0L;
}

BOOL EnumAllWindowsHelper::EnumAllWindows()
{
    BOOL bRet = FALSE;

    EnumAllWindowsHelper *pThis = new EnumAllWindowsHelper();
    ASSERT(pThis != NULL);

    HANDLE hThread = ::CreateThread(NULL, 0, &WorkThreadProcStatic, (LPVOID)pThis, 0, NULL);
    ASSERT(hThread != NULL);
    CloseHandle(hThread);

    bRet = TRUE;

    return bRet;
}

BOOL EnumAllWindowsHelper::EnumWindow(HWND hWnd)
{
    BOOL bRet = FALSE;

    EnumAllWindowsHelper *pThis = new EnumAllWindowsHelper(hWnd);
    ASSERT(pThis != NULL);

    HANDLE hThread = ::CreateThread(NULL, 0, &WorkThreadProcStatic, (LPVOID)pThis, 0, NULL);
    ASSERT(hThread != NULL);
    CloseHandle(hThread);

    bRet = TRUE;

    return bRet;
}

BOOL CALLBACK EnumAllWindowsHelper::EnumWindowsProcStatic(HWND hWnd, LPARAM lParam)
{
    EnumAllWindowsHelper *pThis = (EnumAllWindowsHelper *)lParam;
    
    ASSERT(pThis != NULL);
    
    return pThis->EnumWindowsProc(hWnd);
}

BOOL EnumAllWindowsHelper::EnumWindowsProc(HWND hWnd)
{
    TCHAR *pTcharBuf = NULL;
    char *pCharBuf = NULL;
    int nTextLen;
    BOOL bSuc;
    int nRet;
    HWND hParent;
    DWORD dwThreadId;
    DWORD dwProcessId;
    RECT rect;
    LONG nWinLong;
    std::string strWindowName = "";
    std::string strResult = "";

    if (!::IsWindow(hWnd)) {
        return TRUE;
    }

    if (m_hWndForSearch != NULL && m_hWndForSearch != hWnd) {
        return TRUE;
    }

    pTcharBuf = new TCHAR[TK_TEXT_BUF_SIZE];
    ASSERT(pTcharBuf != NULL);
    AutoFreeArrayPtr<TCHAR> afapTcharBuf(pTcharBuf);

    pCharBuf = new char[TK_TEXT_BUF_SIZE];
    ASSERT(pCharBuf != NULL);
    AutoFreeArrayPtr<char> afapCharBuf(pCharBuf);

    //
    // (Preview) Window Rect
    //
    bSuc = ::GetWindowRect(hWnd, &rect);
    if (bSuc) {
        if ((rect.right - rect.left) <= 0 || (rect.bottom - rect.top) <= 0) {
            return true;
        }
    } else {
        return true;
    }
    
    //
    // Window Name
    //
    nTextLen = ::GetWindowText(hWnd, pTcharBuf, TK_TEXT_BUF_SIZE);
    if (nTextLen == 0) {
        strWindowName = FormatStr("<No name>\n");
    } else if (nTextLen >= 0) {
        if (sizeof(TCHAR) == 2) {
            strWindowName = FormatStr("%s\n", wstr2str(pTcharBuf).c_str());
        } else {
            strWindowName = FormatStr("%s\n", pTcharBuf);
        }
    } else {
        return TRUE;
    }
    strResult += strWindowName;

    //
    // Window Handle
    //
    strResult += FormatStr("hwnd=%08x ", hWnd);

    //
    // Class Name
    //
    nRet = ::GetClassName(hWnd, pTcharBuf, TK_TEXT_BUF_SIZE);
    if (nRet > 0) {
        if (sizeof(TCHAR) == 2) {
            strResult += FormatStr("Class=%s ", wstr2str(pTcharBuf).c_str());
        } else {
            strResult += FormatStr("Class=%s ", pTcharBuf);
        }
    } else {
        strResult += FormatStr("Class=%s ", "<Unknown class>");
    }

    //
    // Parent Window Handle
    //
    hParent = ::GetParent(hWnd);
    strResult += FormatStr("parent=%08x ", hParent);

    //
    // Window thread and process
    //
    dwThreadId = ::GetWindowThreadProcessId(hWnd, &dwProcessId);
    strResult += FormatStr("thread=%08x ", dwThreadId);
    strResult += FormatStr("process=%08x\n", dwProcessId);

    //
    // Window Rect
    //
    bSuc = ::GetWindowRect(hWnd, &rect);
    if (bSuc) {
        strResult += FormatStr("x=%d y=%d width=%d height=%d\n", 
            rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    }

    //
    // Window Style
    //
    nWinLong = ::GetWindowLong(hWnd, GWL_STYLE);
    strResult += FormatStr("Style=%s\n", GenenrateWindowStyleStr(nWinLong).c_str());

    //
    // Window exstyle
    //
    nWinLong = ::GetWindowLong(hWnd, GWL_EXSTYLE);
    strResult += FormatStr("exstyle=%s\n", GenenrateWindowExStyleStr(nWinLong).c_str());
        
    strResult += FormatStr("\n");

    TK_PRINTF(strResult.c_str());

    if (m_hWndForSearch != NULL && m_hWndForSearch == hWnd) {
        return FALSE;
    }
        
    return TRUE;
}

std::string EnumAllWindowsHelper::GenenrateWindowStyleStr(unsigned long nStyle)
{
    std::string strResult;
    unsigned long i;

    static const struct {
        unsigned long nStyle;
        const char *pStyleName;
    } oStyleTable[] = {
        { WS_BORDER, "WS_BORDER" }, 
        { WS_CAPTION, "WS_CAPTION" }, 
        { WS_CHILD, "WS_CHILD" }, 
        { WS_CLIPCHILDREN, "WS_CLIPCHILDREN" }, 
        { WS_CLIPSIBLINGS, "WS_CLIPSIBLINGS" }, 
        { WS_DISABLED, "WS_DISABLED" }, 
        { WS_DLGFRAME, "WS_DLGFRAME" }, 
        { WS_GROUP, "WS_GROUP" }, 
        { WS_HSCROLL, "WS_HSCROLL" }, 
        { WS_OVERLAPPED, "WS_OVERLAPPED" }, 
        //{ WS_OVERLAPPEDWINDOW, "WS_OVERLAPPEDWINDOW" }, 
        { WS_POPUP, "WS_POPUP" }, 
        { WS_SIZEBOX, "WS_SIZEBOX" }, 
        { WS_SYSMENU, "WS_SYSMENU" }, 
        { WS_TABSTOP, "WS_TABSTOP" }, 
        { WS_THICKFRAME, "WS_THICKFRAME" }, 
        { WS_VISIBLE, "WS_VISIBLE" }, 
        { WS_VSCROLL, "WS_VSCROLL" }, 
    };
    unsigned long nStyleTableLen = TK_ARR_LEN(oStyleTable);

    for (i = 0; i < nStyleTableLen; i++) {
        if ((nStyle & oStyleTable[i].nStyle) == oStyleTable[i].nStyle) {
            strResult += FormatStr("%s%s", strResult.length() == 0 ? "" : " ", oStyleTable[i].pStyleName);
        }
    }

    return strResult;
}

std::string EnumAllWindowsHelper::GenenrateWindowExStyleStr(unsigned long nExStyle)
{
    std::string strResult;
    unsigned long i;

    static const struct {
        unsigned long nExStyle;
        const char *pExStyleName;
    } oExStyleTable[] = {
        //{ WS_EX_ACCEPTFILES, "WS_EX_ACCEPTFILES" }, 
        { WS_EX_CAPTIONOKBTN, "WS_EX_CAPTIONOKBTN" }, 
        { WS_EX_CLIENTEDGE, "WS_EX_CLIENTEDGE" }, 
        //{ WS_EX_CONTEXTMENU, "WS_EX_CONTEXTMENU" }, 
        { WS_EX_DLGMODALFRAME, "WS_EX_DLGMODALFRAME" }, 
        { WS_EX_INK, "WS_EX_INK" }, 
        { WS_EX_LAYOUTRTL, "WS_EX_LAYOUTRTL" }, 
        //{ WS_EX_LTRREADING, "WS_EX_LTRREADING" }, 
        { WS_EX_NOACTIVATE, "WS_EX_NOACTIVATE" }, 
        { WS_EX_NOANIMATION, "WS_EX_NOANIMATION" }, 
        { WS_EX_NODRAG, "WS_EX_NODRAG" }, 
        { WS_EX_NOINHERITLAYOUT, "WS_EX_NOINHERITLAYOUT" }, 
        { WS_EX_OVERLAPPEDWINDOW, "WS_EX_OVERLAPPEDWINDOW" }, 
        //{ WS_EX_PALETTEWINDOW, "WS_EX_PALETTEWINDOW" }, 
        { WS_EX_RTLREADING, "WS_EX_RTLREADING" }, 
        { WS_EX_STATICEDGE, "WS_EX_STATICEDGE" }, 
        { WS_EX_TOOLWINDOW, "WS_EX_TOOLWINDOW" }, 
        { WS_EX_TOPMOST, "WS_EX_TOPMOST" }, 
        { WS_EX_WINDOWEDGE, "WS_EX_WINDOWEDGE" }, 
    };
    unsigned long nExStyleTableLen = TK_ARR_LEN(oExStyleTable);

    for (i = 0; i < nExStyleTableLen; i++) {
        if ((nExStyle & oExStyleTable[i].nExStyle) == oExStyleTable[i].nExStyle) {
            strResult += FormatStr("%s%s", strResult.length() == 0 ? "" : " ", oExStyleTable[i].pExStyleName);
        }
    }

    return strResult;
}

BOOL PrintAllWindowsInfo()
{
    BOOL bSuc = FALSE;
    
    bSuc = EnumAllWindowsHelper::EnumAllWindows();

    return bSuc;
}

BOOL PrintWindowsInfo(HWND hWnd)
{
    BOOL bSuc = FALSE;
    
    bSuc = EnumAllWindowsHelper::EnumWindow(hWnd);

    return bSuc;
}

const TCHAR DeskBgWnd::szWindowTitle[] = _T("DeskBgWnd");
const TCHAR DeskBgWnd::szWindowClass[] = _T("DeskBgWndCls");

DeskBgWnd::DeskBgWnd() : m_bWindowReady(FALSE), m_hWorkThread(NULL), m_hWnd(NULL)
{
    m_wndRect.left = 0;
    m_wndRect.right = 800;
    m_wndRect.top = 0;
    m_wndRect.bottom = 480;

    m_bgColor.r = 255;
    m_bgColor.g = 128;
    m_bgColor.b = 255;
    m_bgColor.a = 0;
}

DeskBgWnd::~DeskBgWnd()
{
    EndWorkThread();
}

BOOL DeskBgWnd::BeginWorkThread()
{
    if (m_hWorkThread != NULL) {
        return TRUE;
    }

    m_hWorkThread = ::CreateThread(NULL, 0, &WorkThreadProcStatic, (LPVOID)this, 0, NULL);
    return (m_hWorkThread != NULL);
}

BOOL DeskBgWnd::EndWorkThread()
{
    if (m_hWorkThread == NULL) {
        return TRUE;
    }

    WaitForWindowReady();

    if (!::IsWindow(m_hWnd)) {
        return FALSE;
    }

    ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
    WaitForWorkThreadEnd();

    return TRUE;
}

BOOL DeskBgWnd::WaitForWindowReady()
{
    while (true) {
        if (m_hWorkThread == NULL) {
            return FALSE;
        }
        if (m_bWindowReady) {
            break;
        }
        Sleep(10);
    }

    return (m_hWnd != NULL);
}

BOOL DeskBgWnd::WaitForWorkThreadEnd()
{
    BOOL bRet = FALSE;
    DWORD dwRet;

    while (true) {
        dwRet = ::WaitForSingleObject(m_hWorkThread, 10);
        if (dwRet == WAIT_OBJECT_0) {
            bRet = TRUE;
            break;
        } else if (dwRet == WAIT_FAILED) {
            bRet = FALSE;
            break;
        }
    }

    return bRet;
}

DWORD WINAPI DeskBgWnd::WorkThreadProcStatic(LPVOID lpParameter)
{
    DeskBgWnd *pThis = (DeskBgWnd *)lpParameter;
    ASSERT(pThis != NULL);

    return pThis->WorkThreadProc();
}

DWORD DeskBgWnd::WorkThreadProc()
{
    DWORD dwRet = -1;
    WNDCLASS wc;
    ATOM atomWndClass;
    HWND hWnd;
    LONG nStyle;
    MSG msg;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc	    = &WndProcStatic;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= NULL;
    wc.hIcon			= NULL;
    wc.hCursor		    = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= NULL;
    wc.lpszMenuName	    = NULL;
    wc.lpszClassName	= szWindowClass;
    atomWndClass = ::RegisterClass(&wc);
    if (atomWndClass == 0) {
        dwRet = -1;
        goto FAILED;
    }

    hWnd = ::CreateWindow(szWindowClass, szWindowTitle, 0, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, NULL, NULL);
    if (hWnd == NULL)
    {
        dwRet = -1;
        goto FAILED;
    }
    LOG("INFO: %s(): hWnd=0x%08x\n", __FUNCTION__, hWnd);
    ::SetWindowLong(hWnd, GWL_USERDATA, (LONG)this);
    nStyle = ::GetWindowLong(hWnd, GWL_STYLE);
    nStyle &= ~(WS_CAPTION | WS_BORDER | WS_SIZEBOX | WS_SYSMENU | WS_HSCROLL | WS_VSCROLL);
    ::SetWindowLong(hWnd, GWL_STYLE, nStyle);
    ::MoveWindow(hWnd, m_wndRect.left, m_wndRect.top, m_wndRect.right - m_wndRect.left, m_wndRect.bottom - m_wndRect.top, FALSE);
    ::RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE);
    ::ShowWindow(hWnd, SW_SHOW);
    ::UpdateWindow(hWnd);
    ::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREPOSITION | SWP_NOSIZE);

    m_hWnd = hWnd;
    m_bWindowReady = TRUE;

    while (::GetMessage(&msg, NULL, 0, 0))
	{
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
	}
    dwRet = (DWORD)msg.wParam;
    
    m_hWorkThread = NULL;
    m_hWnd = NULL;
    m_bWindowReady = FALSE;

FAILED:
    if (atomWndClass != 0) {
        ::UnregisterClass(szWindowClass, NULL);
    }
    return dwRet;
}

LRESULT CALLBACK DeskBgWnd::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DeskBgWnd *pThis = NULL;

    if (!::IsWindow(hWnd)) {
        goto FAILED;
    }

    pThis = (DeskBgWnd *)::GetWindowLong(hWnd, GWL_USERDATA);
    if (pThis == NULL) {
        goto FAILED;
    }

    return pThis->WndProc(hWnd, message, wParam, lParam);

FAILED:
    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT DeskBgWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_CLOSE:
        {
            ::DestroyWindow(hWnd);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        {
            HBRUSH hBrush = NULL;

            hBrush = ::CreateSolidBrush(RGB(m_bgColor.r, m_bgColor.g, m_bgColor.b));
            ::FillRect(hdc, &ps.rcPaint, hBrush);

            DeleteObject(hBrush);
        }
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        {
            ::PostQuitMessage(0);
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    
    return 0;
}

DeskBgWnd &DeskBgWnd::GetInstance()
{
    static DeskBgWnd instance;

    return instance;
}

BOOL DeskBgWnd::SetWindowRect(const RECT &rect)
{
    if (!::IsWindow(m_hWnd)) {
        return FALSE;
    }

    memcpy(&m_wndRect, &rect, sizeof(RECT));
    ::MoveWindow(m_hWnd, m_wndRect.left, m_wndRect.top, m_wndRect.right - m_wndRect.left, m_wndRect.bottom - m_wndRect.top, FALSE);
    ::RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE);
    ::UpdateWindow(m_hWnd);

    return TRUE;
}

BOOL DeskBgWnd::SetBgColor(const Color32 &color)
{
    if (!::IsWindow(m_hWnd)) {
        return FALSE;
    }

    memcpy(&m_bgColor, &color, sizeof(Color32));
    ::MoveWindow(m_hWnd, m_wndRect.left, m_wndRect.top, m_wndRect.right - m_wndRect.left, m_wndRect.bottom - m_wndRect.top, FALSE);
    ::RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE);
    ::UpdateWindow(m_hWnd);

    return TRUE;
}

BOOL DeskBgWnd::SetVisible(BOOL bVisible)
{
    if (!::IsWindow(m_hWnd)) {
        return FALSE;
    }

    ::ShowWindow(m_hWnd, bVisible ? SW_SHOW : SW_HIDE);
    ::UpdateWindow(m_hWnd);

    return TRUE;
}

BOOL DeskBgWnd::SetZOrder(HWND hWndInsertAfter)
{
    BOOL bSuc;
    
    if (!::IsWindow(m_hWnd)) {
        return FALSE;
    }

    bSuc = ::SetWindowPos(m_hWnd, hWndInsertAfter, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREPOSITION | SWP_NOSIZE);
    if (!bSuc) {
        LOG("*** ERROR: %s(): ::SetWindowPos() is failed!\n", __FUNCTION__);
    }
    ::UpdateWindow(m_hWnd);

    return TRUE;
}

BOOL DeskBgWnd::SetZOrder(ZOrder zorder)
{
    HWND hWndInsertAfter = NULL;

    switch (zorder) {
    case ZO_BOTTOM:
        hWndInsertAfter = HWND_BOTTOM;
        break;
    case ZO_TOP:
        hWndInsertAfter = HWND_TOP;
        break;
    case ZO_NOTOPMOST:
        hWndInsertAfter = HWND_NOTOPMOST;
        break;
    case ZO_TOPMOST:
        hWndInsertAfter = HWND_TOPMOST;
        break;
    default:
        break;
    }

    if (hWndInsertAfter == NULL) {
        return FALSE;
    }

    return SetZOrder(hWndInsertAfter);
}

BOOL DeskBgWnd::RedrawWindow()
{
    if (!::IsWindow(m_hWnd)) {
        return FALSE;
    }

    ::RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE);
    ::UpdateWindow(m_hWnd);

    return TRUE;
}

//
// NOTE: the instances must be create by new, and DON'T be destroyed by the user for it will be destroyed automatically.
//
class TaskletThread
{
public:
    const HANDLE GetThreadHandle() { return m_hThreadHandle; }
    BOOL Start();

protected:
    TaskletThread(void *pArgs = NULL);
    virtual ~TaskletThread();
    virtual DWORD Run(void *pArgs) = 0;

private:
    static DWORD WINAPI WorkThreadProcStatic(LPVOID lpParameter);
    DWORD WorkThreadProc();

private:
    void *m_pArgs;
    HANDLE m_hThreadHandle;
    HANDLE m_hEvent;
};

TaskletThread::TaskletThread(void *pArgs /*= NULL*/) : m_pArgs(pArgs), m_hThreadHandle(NULL), m_hEvent(NULL)
{
    m_hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    ASSERT(m_hEvent != NULL);
    
    m_hThreadHandle = ::CreateThread(NULL, 0, &WorkThreadProcStatic, (LPVOID)this, 0, NULL);
    ASSERT(m_hThreadHandle != NULL);
}

TaskletThread::~TaskletThread()
{
    if (m_hEvent != NULL) {
        ::CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }
    
    if (m_hThreadHandle != NULL) {
        ::CloseHandle(m_hThreadHandle);
        m_hThreadHandle = NULL;
    }
}

BOOL TaskletThread::Start()
{
    ::SetEvent(m_hEvent);
    return TRUE;
}

DWORD WINAPI TaskletThread::WorkThreadProcStatic(LPVOID lpParameter)
{
    TaskletThread *pThis = (TaskletThread *)lpParameter;
    ASSERT(pThis != NULL);

    return pThis->WorkThreadProc();
}

DWORD TaskletThread::WorkThreadProc()
{
    DWORD dwRet;

    dwRet = ::WaitForSingleObject(m_hEvent, INFINITE);
    if (dwRet != WAIT_OBJECT_0) {
        TK_PRINTF("INFO: %s(): WaitForSingleObject() is failed!\n", __FUNCTION__);
    }

    dwRet = Run(m_pArgs);
    
    delete this;
    return dwRet;
}

static BOOL HW_FillWindow(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    BOOL bRet = FALSE;
    BOOL bSuc;
    HBRUSH hBrush = NULL;
    union {
        RECT rect;
        struct {
            POINT point1;
            POINT point2;
        } points;
    } rect;

    bSuc = ::IsWindow(hWnd);
    if (!bSuc) {
        TK_PRINTF("INFO: %s(): hWnd=0x%08X is invalied!\n", __FUNCTION__, hWnd);
        bRet = FALSE;
        goto FAILED;
    }
    
    bSuc = ::GetWindowRect(hWnd, &rect.rect);
    if (!bSuc) {
        bRet = FALSE;
        goto FAILED;
    }
    
    bSuc = ::ScreenToClient(hWnd, &rect.points.point1);
    if (!bSuc) {
        bRet = FALSE;
        goto FAILED;
    }
    bSuc = ::ScreenToClient(hWnd, &rect.points.point2);
    if (!bSuc) {
        bRet = FALSE;
        goto FAILED;
    }

    hBrush = ::CreateSolidBrush(RGB(r, g, b));
    if (hBrush == NULL) {
        bRet = FALSE;
        goto FAILED;
    }

    bSuc = ::FillRect(::GetDC(hWnd), &rect.rect, hBrush);
    if (!bSuc) {
        bRet = FALSE;
        goto FAILED;
    }

    bRet = TRUE;
    
FAILED:
    if (hBrush != NULL) {
        ::DeleteObject(hBrush);
    }
    
    return bRet;
}

class TaskletThread_FillWindow : public TaskletThread
{
public:
    static TaskletThread *NewInstance(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        return new TaskletThread_FillWindow(hWnd, r, g, b, a);
    }

protected:
    TaskletThread_FillWindow(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        : TaskletThread(NULL), m_hWnd(hWnd), m_r(r), m_g(g), m_b(b), m_a(a) {}
    virtual ~TaskletThread_FillWindow() {}
    virtual DWORD Run(void *pArgs) {
        return HW_FillWindow(m_hWnd, m_r, m_g, m_b, m_a);
    }

private:
    HWND m_hWnd;
    unsigned char m_r;
    unsigned char m_g;
    unsigned char m_b;
    unsigned char m_a;
};

BOOL FillWindow(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    TaskletThread *pTaskletThread = TaskletThread_FillWindow::NewInstance(hWnd, r, g, b, a);
    if (pTaskletThread != NULL) {
        return pTaskletThread->Start();
    } else {
        return FALSE;
    }
}

static BOOL HW_FillWindow(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a, long x, long y, unsigned long w, unsigned long h)
{
    BOOL bRet = FALSE;
    BOOL bSuc;
    HBRUSH hBrush = NULL;
    union {
        RECT rect;
        struct {
            POINT point1;
            POINT point2;
        } points;
    } rect;

    bSuc = ::IsWindow(hWnd);
    if (!bSuc) {
        TK_PRINTF("INFO: %s(): hWnd=0x%08X is invalied!\n", __FUNCTION__, hWnd);
        bRet = FALSE;
        goto FAILED;
    }
    
    rect.rect.left = x;
    rect.rect.right = x + w;
    rect.rect.top = y;
    rect.rect.bottom = y + h;

    hBrush = ::CreateSolidBrush(RGB(r, g, b));
    if (hBrush == NULL) {
        bRet = FALSE;
        goto FAILED;
    }

    bSuc = ::FillRect(::GetDC(hWnd), &rect.rect, hBrush);
    if (!bSuc) {
        bRet = FALSE;
        goto FAILED;
    }

    bRet = TRUE;
    
FAILED:
    if (hBrush != NULL) {
        ::DeleteObject(hBrush);
    }
    
    return bRet;
}

class TaskletThread_FillWindow2 : public TaskletThread
{
public:
    static TaskletThread *NewInstance(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a, 
        long x, long y, unsigned long w, unsigned long h) {
        return new TaskletThread_FillWindow2(hWnd, r, g, b, a, x, y, w, h);
    }

protected:
    TaskletThread_FillWindow2(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a, 
        long x, long y, unsigned long w, unsigned long h)
        : TaskletThread(NULL), m_hWnd(hWnd), m_r(r), m_g(g), m_b(b), m_a(a), m_x(x), m_y(y), m_w(w), m_h(h) {}
    virtual ~TaskletThread_FillWindow2() {}
    virtual DWORD Run(void *pArgs) {
        return HW_FillWindow(m_hWnd, m_r, m_g, m_b, m_a, m_x, m_y, m_w, m_h);
    }

private:
    HWND m_hWnd;
    unsigned char m_r;
    unsigned char m_g;
    unsigned char m_b;
    unsigned char m_a;
    long m_x;
    long m_y;
    unsigned long m_w;
    unsigned long m_h;
};

BOOL FillWindow(HWND hWnd, unsigned char r, unsigned char g, unsigned char b, unsigned char a, long x, long y, unsigned long w, unsigned long h)
{
    TaskletThread *pTaskletThread = TaskletThread_FillWindow2::NewInstance(hWnd, r, g, b, a, x, y, w, h);
    if (pTaskletThread != NULL) {
        return pTaskletThread->Start();
    } else {
        return FALSE;
    }
}

static BOOL HW_MoveWindow(HWND hWnd, int nX, int nY, int nWidth, int nHeight)
{
    BOOL bRet = FALSE;
    BOOL bSuc;

    bSuc = ::IsWindow(hWnd);
    if (!bSuc) {
        TK_PRINTF("INFO: %s(): hWnd=0x%08X is invalied!\n", __FUNCTION__, hWnd);
        bRet = FALSE;
        goto FAILED;
    }

    bSuc = ::MoveWindow(hWnd, nX, nY, nWidth, nHeight, FALSE);
    if (!bSuc) {
        bRet = FALSE;
        goto FAILED;
    }

    bRet = TRUE;
    
FAILED:   
    return bRet;    
}

class TaskletThread_MoveWindow : public TaskletThread
{
public:
    static TaskletThread *NewInstance(HWND hWnd, long x, long y, unsigned long w, unsigned long h) {
        return new TaskletThread_MoveWindow(hWnd, x, y, w, h);
    }

protected:
    TaskletThread_MoveWindow(HWND hWnd, long x, long y, unsigned long w, unsigned long h)
        : TaskletThread(NULL), m_hWnd(hWnd), m_x(x), m_y(y), m_w(w), m_h(h) {}
    virtual ~TaskletThread_MoveWindow() {}
    virtual DWORD Run(void *pArgs) {
        return HW_MoveWindow(m_hWnd, m_x, m_y, m_w, m_h);
    }

private:
    HWND m_hWnd;
    long m_x;
    long m_y;
    unsigned long m_w;
    unsigned long m_h;
};

BOOL MoveWindow(HWND hWnd, int nX, int nY, int nWidth, int nHeight)
{
    TaskletThread *pTaskletThread = TaskletThread_MoveWindow::NewInstance(hWnd, nX, nY, nWidth, nHeight);
    if (pTaskletThread != NULL) {
        return pTaskletThread->Start();
    } else {
        return FALSE;
    }
}
static BOOL HW_RedrawWindow(HWND hWnd)
{
    BOOL bRet = FALSE;
    BOOL bSuc;

    bSuc = ::IsWindow(hWnd);
    if (!bSuc) {
        TK_PRINTF("INFO: %s(): hWnd=0x%08X is invalied!\n", __FUNCTION__, hWnd);
        bRet = FALSE;
        goto FAILED;
    }

    ::RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE);
    ::UpdateWindow(hWnd);

    bRet = TRUE;

FAILED:
    return bRet;
}

class TaskletThread_RedrawWindow : public TaskletThread
{
public:
    static TaskletThread *NewInstance(HWND hWnd) {
        return new TaskletThread_RedrawWindow(hWnd);
    }

protected:
    TaskletThread_RedrawWindow(HWND hWnd)
        : TaskletThread(NULL), m_hWnd(hWnd) {}
    virtual ~TaskletThread_RedrawWindow() {}
    virtual DWORD Run(void *pArgs) {
        return HW_RedrawWindow(m_hWnd);
    }

private:
    HWND m_hWnd;
};

BOOL RedrawWindow(HWND hWnd)
{
    TaskletThread *pTaskletThread = TaskletThread_RedrawWindow::NewInstance(hWnd);
    if (pTaskletThread != NULL) {
        return pTaskletThread->Start();
    } else {
        return FALSE;
    }
}

static BOOL HW_SetWindowZOrderAfter(HWND hWnd, HWND hWndInsertAfter)
{
    BOOL bSuc;

    if (!::IsWindow(hWnd)) {
        TK_PRINTF("INFO: %s(): hWnd=0x%08X is invalied!\n", __FUNCTION__, hWnd);
        return FALSE;
    }

    bSuc = ::SetWindowPos(hWnd, hWndInsertAfter, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREPOSITION | SWP_NOSIZE);
    if (!bSuc) {
        LOG("*** ERROR: %s(): ::SetWindowPos() is failed!\n", __FUNCTION__);
    }
    ::UpdateWindow(hWnd);

    return TRUE;
}

class TaskletThread_SetWindowZOrderAfter : public TaskletThread
{
public:
    static TaskletThread *NewInstance(HWND hWnd, HWND hWndInsertAfter) {
        return new TaskletThread_SetWindowZOrderAfter(hWnd, hWndInsertAfter);
    }

protected:
    TaskletThread_SetWindowZOrderAfter(HWND hWnd, HWND hWndInsertAfter)
        : TaskletThread(NULL), m_hWnd(hWnd), m_hWndInsertAfter(hWndInsertAfter) {}
    virtual ~TaskletThread_SetWindowZOrderAfter() {}
    virtual DWORD Run(void *pArgs) {
        return HW_SetWindowZOrderAfter(m_hWnd, m_hWndInsertAfter);
    }

private:
    HWND m_hWnd;
    HWND m_hWndInsertAfter;
};

BOOL SetWindowZOrderAfter(HWND hWnd, HWND hWndInsertAfter)
{
    TaskletThread *pTaskletThread = TaskletThread_SetWindowZOrderAfter::NewInstance(hWnd, hWndInsertAfter);
    if (pTaskletThread != NULL) {
        return pTaskletThread->Start();
    } else {
        return FALSE;
    }
}

BOOL SetWindowZOrder(HWND hWnd, ZOrder zorder)
{
    HWND hWndInsertAfter = NULL;

    switch (zorder) {
    case ZO_BOTTOM:
        hWndInsertAfter = HWND_BOTTOM;
        break;
    case ZO_TOP:
        hWndInsertAfter = HWND_TOP;
        break;
    case ZO_NOTOPMOST:
        hWndInsertAfter = HWND_NOTOPMOST;
        break;
    case ZO_TOPMOST:
        hWndInsertAfter = HWND_TOPMOST;
        break;
    default:
        break;
    }

    if (hWndInsertAfter == NULL) {
        return FALSE;
    }

    return SetWindowZOrderAfter(hWnd, hWndInsertAfter);
}

BOOL GetProcessByName(const std::tstring &sProcessName, HANDLE &hProcess, DWORD &nProcessID)
{
    BOOL bSuc = FALSE;
    BOOL bEnumEnd = FALSE;
    HANDLE hToolhelp32Snapshot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;

    hToolhelp32Snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hToolhelp32Snapshot == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    pe.dwSize = sizeof(PROCESSENTRY32);
    bSuc = ::Process32First(hToolhelp32Snapshot, &pe);
    if (!bSuc) {
        if (::GetLastError() != ERROR_NO_MORE_FILES) {
            bSuc = FALSE;
            goto FAILED;
        } else {
            bEnumEnd = TRUE;
        }
    }
    while (true) {
        if (bEnumEnd) {
            break;
        }
        
        if (std::tstring(pe.szExeFile) == sProcessName) {
            hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
            if (hProcess == NULL) {
                bSuc = FALSE;
                goto FAILED;
            }
            nProcessID = pe.th32ProcessID;
            goto SUC;
        }
        
        pe.dwSize = sizeof(PROCESSENTRY32);
        bSuc = ::Process32Next(hToolhelp32Snapshot, &pe);
        if (!bSuc) {
            if (::GetLastError() != ERROR_NO_MORE_FILES) {
                bSuc = FALSE;
                goto FAILED;
            } else {
                bEnumEnd = TRUE;
            }
        }
    }

    bSuc = FALSE;
    goto FAILED;

SUC:
    bSuc = TRUE;
FAILED:
    if (hToolhelp32Snapshot != INVALID_HANDLE_VALUE) {
        ::CloseToolhelp32Snapshot(hToolhelp32Snapshot);
        hToolhelp32Snapshot = INVALID_HANDLE_VALUE;
    }
    return bSuc;
}

BOOL KillProcessByName(const std::tstring &sProcessName)
{
    BOOL bSuc = FALSE;
    HANDLE hProcess = NULL;
    DWORD nProcessID;

    bSuc = GetProcessByName(sProcessName, hProcess, nProcessID);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }

    bSuc = ::TerminateProcess(hProcess, -1);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }

    bSuc = TRUE;
FAILED:
    if (hProcess != NULL) {
        CloseHandle(hProcess);
    }
    return bSuc;
}

#if 0
static bool CapturePictureFromDirectShow(IGraphBuilder *pGraphBuilder, const std::string &sSaveBmpFilePathTemplate)
{
    bool bFuncRet = true;
    HRESULT hr;
    LONG nWaitToPausedTimeout = 10 * 1000;
    IMediaControl *pMediaControl = NULL;
    IBasicVideo *pBasicVideo = NULL;
    long nVideoWidth = 0;
    long nVideoHeight = 0;
    long nDIBBufferSize = 0;
    long *pDIBImage = NULL;
    FILE *pFile = NULL;
    size_t nWritten;
    BITMAPINFOHEADER *bih = NULL;
    BITMAPFILEHEADER bfh = { 0 };
    OAFilterState pfs;
    
    if (bFuncRet) {
        if (pGraphBuilder == NULL) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
        hr = pGraphBuilder->QueryInterface(IID_IMediaControl, reinterpret_cast<void **>(&pMediaControl));
        if (hr != S_OK) {
            bFuncRet = false;
        }
    }

//	    if (bFuncRet) {
//	        hr = pMediaControl->GetState(nWaitToPausedTimeout, &pfs);
//	        if (hr != S_OK) {
//	            bFuncRet = false;
//	        } else if (pfs != State_Paused) {
//	            bFuncRet = false;
//	        }
//	        ::Sleep(5 * 1000);
//	    }

    if (bFuncRet) {
        hr = pGraphBuilder->QueryInterface(IID_IBasicVideo, reinterpret_cast<void **>(&pBasicVideo));
        if (hr != S_OK) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
        hr = pBasicVideo->get_VideoWidth(&nVideoWidth);
        if (hr != S_OK) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
        hr = pBasicVideo->get_VideoHeight(&nVideoHeight);
        if (hr != S_OK) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
        hr = pBasicVideo->GetCurrentImage(&nDIBBufferSize, NULL);
        if (hr != S_OK) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
        pDIBImage = new long[(nDIBBufferSize + sizeof(long) - 1) / sizeof(long)];
        if (pDIBImage == NULL) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
        // (***)Pause the video renderer before calling this method. Calling this method in any other state 
        //      than paused returns VFW_E_NOT_PAUSED. 
        hr = pBasicVideo->GetCurrentImage(&nDIBBufferSize, pDIBImage);
        if (hr != S_OK) {
            bFuncRet = false;
        }
    }

    if (bFuncRet) {
        bih = (BITMAPINFOHEADER *)pDIBImage;
        
        memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
        bfh.bfType = 'B' | ('M' << 8);
        bfh.bfSize = sizeof(BITMAPFILEHEADER) + nDIBBufferSize;
        bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        static unsigned int nSerialNo = 0;
        char arrSaveBmpPathBuf[256];
        _snprintf(arrSaveBmpPathBuf, sizeof(arrSaveBmpPathBuf) - 1, sSaveBmpFilePathTemplate.c_str(), nSerialNo++);

        do {
            pFile = fopen(arrSaveBmpPathBuf, "w+b");
            if (pFile == NULL) {
                bFuncRet = false;
                break;
            }
            nWritten = fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), pFile);
            if (nWritten < sizeof(BITMAPFILEHEADER)) {
                bFuncRet = false;
                break;
            }
            nWritten = fwrite(pDIBImage, 1, nDIBBufferSize, pFile);
            if (nWritten < nDIBBufferSize) {
                bFuncRet = false;
                break;
            }
            fclose(pFile);
            pFile = NULL;
        } while (0);
    }

    if (pMediaControl != NULL) {
        pMediaControl->Release();
        pMediaControl = NULL;
    }

    if (pBasicVideo != NULL) {
        pBasicVideo->Release();
        pBasicVideo = NULL;
    }

    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }

    if (pDIBImage != NULL) {
        delete [] pDIBImage;
        pDIBImage = NULL;
    }

    return bFuncRet;
}
#endif // #if 0

}// namespace TK_Tools {
