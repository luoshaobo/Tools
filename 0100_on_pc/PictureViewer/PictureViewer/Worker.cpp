#include "StdAfx.h"
#include "Worker.h"
#include "Tools.h"
#include "entTlmXmlParser.h"

#define INTERNAL_BMP_WIDTH                      4096
#define INTERNAL_BMP_HEIGHT                     4096
#define INTERNAL_BMP_PITCH                      (INTERNAL_BMP_WIDTH * 4)

#define BG_BUF_WIDTH1                           4096
#define BG_BUF_HEIGHT1                          4096

static const char *g_strConfigFilePath = "PictureViewer.xml";

#define MIN(x,y)                                ((x) < (y) ? (x) : (y))

CWorker::CWorker(void)
{
    m_nImageDrawingFlags = 0;

    m_globalAlpha = 255;
    m_bgA = 255;
    m_bgR = 255;
    m_bgG = 0;
    m_bgB = 255;

    m_xOriginOffset = 0;
    m_yOriginOffset = 0;

    m_pBgBuffDC1 = NULL;
    m_pBgBuffBitmap1 = NULL;
    m_nBgBuffBitmapWidth1 = 0;
    m_nBgBuffBitmapHeight1 = 0;
    m_nBgBuffBitmapWidth2 = 0;
    m_nBgBuffBitmapHeight2 = 0;

    m_pBgPattern = NULL;
    m_pBgBrush = NULL;

    m_fScaleFactor = 1.0f;
    m_hInternalMemDC = NULL;
    m_hInternalMemBmp = NULL;
    memset(&m_BitmapInfo, 0, sizeof(BITMAPINFO_3DWORD));
    m_pInternalBmpBits = NULL;

    m_hImageInfoMonitorWnd = NULL;
    m_ImageInfoMonitorMsg = 0;
    m_hDrawingInfoMonitorWnd = NULL;
    m_DrawingInfoMonitorMsg = 0;

    m_strImageFilePath = "";
    m_pImage = NULL;
    m_nImageWidth = 0;
    m_nImageHeight = 0;

    m_xWatchPoint = 0;
    m_yWatchPoint = 0;

    CreateInternalMemDC();

    m_bExportedBmpBottomUp = FALSE;
    m_sExportedBmpFileColorOrder = "bgra";
}

CWorker::~CWorker(void)
{
    if (m_pImage != NULL) {
        delete m_pImage;
        m_pImage = NULL;
    }

    if (m_hInternalMemDC != NULL) {
        DeleteDC(m_hInternalMemDC);
        m_hInternalMemDC = NULL;
    }

    if (m_hInternalMemBmp != NULL) {
        DeleteObject(m_hInternalMemBmp);
        m_hInternalMemBmp = NULL;
    }

    if (m_pBgBuffDC1 != NULL) {
        delete m_pBgBuffDC1;
        m_pBgBuffDC1 = NULL;
    }

    if (m_pBgBuffDC2 != NULL) {
        delete m_pBgBuffDC2;
        m_pBgBuffDC2 = NULL;
    }

    if (m_pBgBuffBitmap1 != NULL) {
        delete m_pBgBuffBitmap1;
        m_pBgBuffBitmap1 = NULL;
    }

    if (m_pBgBuffBitmap2 != NULL) {
        delete m_pBgBuffBitmap2;
        m_pBgBuffBitmap2 = NULL;
    }

    if (m_pBgBrush != NULL) {
        delete m_pBgBrush;
        m_pBgBrush = NULL;
    }

    if (m_pBgPattern != NULL) {
        delete m_pBgPattern;
        m_pBgPattern = NULL;
    }
}

BOOL CWorker::CloseCurrentFile()
{
    m_strImageFilePath.Empty();
    m_nImageDrawingFlags |= IDF_IMG_FILE_PATH_CHANGED;

    return TRUE;
}

BOOL CWorker::SaveAsBmpFileWithColor(const CString &sSavedBmpFilePath)
{
    BOOL bRet = FALSE;
    unsigned long nWidth = MIN(m_nImageWidth, INTERNAL_BMP_WIDTH);
    unsigned long nHeight = MIN(m_nImageHeight, INTERNAL_BMP_HEIGHT);
    unsigned long nPitch = INTERNAL_BMP_PITCH;

    if (m_nImageWidth == 0 || m_nImageHeight == 0) {
        return FALSE;
    }

    if (m_pImage == NULL) {
        return FALSE;
    }

    if (m_pInternalBmpBits != NULL) {
        bRet = SaveBmpToFile_RGBA32(
            TK_Tools::wstr2str((const wchar_t *)sSavedBmpFilePath).c_str(),
            m_pInternalBmpBits,
            nWidth,
            nHeight,
            nPitch,
            m_bExportedBmpBottomUp,
            "bgra",
            TK_Tools::wstr2str((const wchar_t *)m_sExportedBmpFileColorOrder).c_str()
        );
    }

    return bRet;
}

BOOL CWorker::SaveAsRawFile(const CString &sSavedBmpFilePath)
{
    BOOL bRet = FALSE;
    unsigned long nWidth = MIN(m_nImageWidth, INTERNAL_BMP_WIDTH);
    unsigned long nHeight = MIN(m_nImageHeight, INTERNAL_BMP_HEIGHT);
    unsigned long nPitch = INTERNAL_BMP_PITCH;

    if (m_nImageWidth == 0 || m_nImageHeight == 0) {
        return FALSE;
    }

    if (m_pImage == NULL) {
        return FALSE;
    }

    if (m_pInternalBmpBits != NULL) {
        bRet = SaveBmpToRawFile_RGBA32(
            TK_Tools::wstr2str((const wchar_t *)sSavedBmpFilePath).c_str(),
            m_pInternalBmpBits,
            nWidth,
            nHeight,
            nPitch,
            m_bExportedBmpBottomUp,
            "bgra",
            TK_Tools::wstr2str((const wchar_t *)m_sExportedBmpFileColorOrder).c_str()
        );
    }

    return bRet;
}

int CWorker::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   if (format == NULL || pClsid == NULL) {
       return -1;
   }

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

/*
 * Possible MIME types:
 *     image/bmp 
 *     image/jpeg 
 *     image/gif 
 *     image/tiff 
 *     image/png 
 */
BOOL CWorker::SaveAsImageFile(const CString &sMimeType, const CString &sSavedBmpFilePath)
{
    CLSID clsid;

    if (GetEncoderClsid((const WCHAR *)sMimeType, &clsid) < 0) {
        return FALSE;
    }

    if (m_pImage == NULL) {
        return FALSE;
    }

    if (m_pImage->Save(sSavedBmpFilePath, &clsid, NULL) != Ok) {
        return FALSE;
    }

    return TRUE;
}

BOOL CWorker::ParseColor32Offset(const char *pColorOrder, int *nRedOffset, int *nGreenInputOffset, int *nBlueOffset, int *nAlphaOffset)
{
    const char *pCh;

    ASSERT(nRedOffset != NULL);
    ASSERT(nGreenInputOffset != NULL);
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
    *nGreenInputOffset = pCh - pColorOrder;
    if (*nGreenInputOffset >= 4) {
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
    *nGreenInputOffset = 1;
    *nBlueOffset = 0;
    *nAlphaOffset = 3;
    return FALSE;
}

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

BOOL CWorker::SaveBmpToFile_RGBA32(
    const char *pFilePath,
    const void *pBmpBits,
    unsigned long nBmpWidth,
    unsigned long nBmpHeight,
    unsigned long nBmpPitch,
    BOOL bBottomUp,
    const char *pInputColorOrder,
    const char *pOutputColorOrder
)
{
    BOOL bRet = FALSE;
    BOOL bSuc = FALSE;
    int nInputOffsetR, nInputOffsetG, nInputOffsetB, nInputOffsetA;
    int nOutputOffsetR, nOutputOffsetG, nOutputOffsetB, nOutputOffsetA;
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
    unsigned char *pDstPixel;
    unsigned char *pSrcPixel;

    ASSERT(pFilePath != NULL);
    ASSERT(pBmpBits != NULL);

    bSuc = ParseColor32Offset(pInputColorOrder, &nInputOffsetR, &nInputOffsetG, &nInputOffsetB, &nInputOffsetA);
    if (!bSuc) {
        LOG("*** ERROR: %s(): ParseColor32Offset(pInputColorOrder) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    bSuc = ParseColor32Offset(pOutputColorOrder, &nOutputOffsetR, &nOutputOffsetG, &nOutputOffsetB, &nOutputOffsetA);
    if (!bSuc) {
        LOG("*** ERROR: %s(): ParseColor32Offset(pOutputColorOrder) is failed!\n", __FUNCTION__);
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
    pBitsFields[0] = 0xFF << (nOutputOffsetR * 8);               // r
    pBitsFields[1] = 0xFF << (nOutputOffsetG * 8);               // g
    pBitsFields[2] = 0xFF << (nOutputOffsetB * 8);               // b

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
            pDstPixel = (unsigned char *)((unsigned char *)pLineBuf + j * 4);
            pSrcPixel = (unsigned char *)((unsigned char *)pBmpBits + nBmpPitch * i + j * 4);

            pDstPixel[nOutputOffsetR] = pSrcPixel[nInputOffsetR];
            pDstPixel[nOutputOffsetG] = pSrcPixel[nInputOffsetG];
            pDstPixel[nOutputOffsetB] = pSrcPixel[nInputOffsetB];
            pDstPixel[nOutputOffsetA] = pSrcPixel[nInputOffsetA];
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

BOOL CWorker::SaveBmpToRawFile_RGBA32(
    const char *pFilePath,
    const void *pBmpBits,
    unsigned long nBmpWidth,
    unsigned long nBmpHeight,
    unsigned long nBmpPitch,
    BOOL bBottomUp,
    const char *pInputColorOrder,
    const char *pOutputColorOrder
)
{
    BOOL bRet = FALSE;
    BOOL bSuc = FALSE;
    int nInputOffsetR, nInputOffsetG, nInputOffsetB, nInputOffsetA;
    int nOutputOffsetR, nOutputOffsetG, nOutputOffsetB, nOutputOffsetA;
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
    unsigned char *pDstPixel;
    unsigned char *pSrcPixel;

    ASSERT(pFilePath != NULL);
    ASSERT(pBmpBits != NULL);

    bSuc = ParseColor32Offset(pInputColorOrder, &nInputOffsetR, &nInputOffsetG, &nInputOffsetB, &nInputOffsetA);
    if (!bSuc) {
        LOG("*** ERROR: %s(): ParseColor32Offset(pInputColorOrder) is failed!\n", __FUNCTION__);
        bRet = FALSE;
        goto FAILED;
    }

    bSuc = ParseColor32Offset(pOutputColorOrder, &nOutputOffsetR, &nOutputOffsetG, &nOutputOffsetB, &nOutputOffsetA);
    if (!bSuc) {
        LOG("*** ERROR: %s(): ParseColor32Offset(pOutputColorOrder) is failed!\n", __FUNCTION__);
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
    pBitsFields[0] = 0xFF << (nOutputOffsetR * 8);               // r
    pBitsFields[1] = 0xFF << (nOutputOffsetG * 8);               // g
    pBitsFields[2] = 0xFF << (nOutputOffsetB * 8);               // b

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

    for (i = 0; i < nBmpHeight; i++) {
        for (j = 0; j < nBmpWidth; j++) {
            pDstPixel = (unsigned char *)((unsigned char *)pLineBuf + j * 4);
            pSrcPixel = (unsigned char *)((unsigned char *)pBmpBits + nBmpPitch * i + j * 4);

            pDstPixel[nOutputOffsetR] = pSrcPixel[nInputOffsetR];
            pDstPixel[nOutputOffsetG] = pSrcPixel[nInputOffsetG];
            pDstPixel[nOutputOffsetB] = pSrcPixel[nInputOffsetB];
            pDstPixel[nOutputOffsetA] = pSrcPixel[nInputOffsetA];
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

bool CWorker::IsBmpFileWithAlpha()
{
    std::string sExtFileName;
    std::vector<unsigned char> &vFileContent = m_vBmpWithAlphaFileContent;
    unsigned char *pFileContent = NULL;
    BITMAPFILEHEADER *pBitmapFileHearder = NULL;
    BITMAPINFOHEADER *pBitmapInfoHearder = NULL;
    DWORD *pBitsFields = NULL;

    sExtFileName = TK_Tools::GetExtFileName(TK_Tools::wstr2str((const wchar_t *)m_strImageFilePath));
    if (TK_Tools::LowerCase(sExtFileName) != "bmp") {
        return false;
    }

    if (!TK_Tools::LoadFile(TK_Tools::wstr2str((const wchar_t *)m_strImageFilePath), vFileContent)) {
        return false;
    }

    pFileContent = (unsigned char *)&vFileContent[0];
    pBitmapFileHearder = (BITMAPFILEHEADER *)pFileContent;

    if (pBitmapFileHearder == NULL || vFileContent.size() < sizeof(BITMAPFILEHEADER)) {
        return false;
    }

    if (pBitmapFileHearder->bfType != ('B' | ('M' << 8))) {
        return false;
    }

    pBitmapInfoHearder = (BITMAPINFOHEADER *)(pFileContent + sizeof(BITMAPFILEHEADER));

    if (pBitmapInfoHearder == NULL || vFileContent.size() < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) {
        return false;
    }

    if (pBitmapInfoHearder->biBitCount != 32) {
        return false;
    }

    pBitsFields = (DWORD *)(pBitmapInfoHearder + 1);

    if (pBitsFields == NULL || vFileContent.size() < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(DWORD) * 3) {
        return false;
    }

    if (!(     pBitsFields[0] == 0x00FF0000              // r
            && pBitsFields[1] == 0x0000FF00              // g
            && pBitsFields[2] == 0x000000FF              // b
    )) {
        return false;
    }

    if (pBitmapFileHearder->bfSize != vFileContent.size()) {
        return false;
    }

    if (pBitmapInfoHearder->biHeight < 0) {
        pBitmapInfoHearder->biHeight = -pBitmapInfoHearder->biHeight;
    }
    if (pBitmapFileHearder->bfOffBits + pBitmapInfoHearder->biWidth * 4 * pBitmapInfoHearder->biHeight != vFileContent.size()) {
        return false;
    }

    return true;
}

CWorker &CWorker::GetInstance()
{
    static CWorker worker;
    return worker;
}

BOOL CWorker::CheckImageFilePathValid(const CString &strImageFilePath)
{
    BOOL bRet;
    Image *pImage = NULL;
    int w, h;

    pImage = Image::FromFile((LPCTSTR)strImageFilePath, FALSE);
    if (pImage == NULL) {
        bRet = FALSE;
        goto FAILED;
    }

    w = pImage->GetWidth();
    h = pImage->GetHeight();

    if (w == 0 || h == 0) {
        bRet = FALSE;
        goto FAILED;
    }

    bRet = TRUE;

FAILED:
    if (pImage != NULL) {
        delete pImage;
        pImage = NULL;
    }

    return bRet;
}

void CWorker::OnPaint(CPaintDC &dc)
{
    DrawOnBgBuffDc(dc);

    if (m_pBgBuffDC2 != NULL) {
        dc.BitBlt(
            0, 0, m_nBgBuffBitmapWidth2, m_nBgBuffBitmapHeight2,
            m_pBgBuffDC2,
            0, 0, 
            SRCCOPY
        );
    }

    SendImageInfoMessage();
    SendDrawingInfoMessage();
    DrawInternalMemDC();
}

void CWorker::DrawOnBgBuffDc(CPaintDC &dc)
{
    BOOL bSuc;
    CRect rectWnd;

    CWnd *pWnd = dc.GetWindow();
    if (pWnd == NULL) {
        return;
    }

    pWnd->GetWindowRect(rectWnd);

    //-----------------------------------------------------------------------------------------------
    //-- create members: DC and BITMAP
    //--
    if (m_nBgBuffBitmapWidth1 != BG_BUF_WIDTH1 || m_nBgBuffBitmapHeight1 != BG_BUF_HEIGHT1) {
        m_nBgBuffBitmapWidth1 = BG_BUF_WIDTH1;
        m_nBgBuffBitmapHeight1 = BG_BUF_HEIGHT1;

        if (m_pBgBuffDC1 != NULL) {
            delete m_pBgBuffDC1;
            m_pBgBuffDC1 = NULL;
            m_nImageDrawingFlags |= IDF_IMG_DC_RECREATED;
        }

        if (m_pBgBuffBitmap1 != NULL) {
            delete m_pBgBuffBitmap1;
            m_pBgBuffBitmap1 = NULL;
            m_nImageDrawingFlags |= IDF_IMG_DC_RECREATED;
        }
    }

    if (m_pBgBuffDC1 == NULL) {
        m_pBgBuffDC1 = new CDC();
        if (m_pBgBuffDC1 == NULL) {
            return;
        }
        bSuc = m_pBgBuffDC1->CreateCompatibleDC(&dc);
        if (!bSuc) {
            return;
        }
    }

    if (m_pBgBuffBitmap1 == NULL) {
        m_pBgBuffBitmap1 = new CBitmap();
        if (m_pBgBuffBitmap1 == NULL) {
            return;
        }
        bSuc = m_pBgBuffBitmap1->CreateCompatibleBitmap(&dc, m_nBgBuffBitmapWidth1, m_nBgBuffBitmapHeight1);
        if (!bSuc) {
            return;
        }
    }
    
    m_pBgBuffDC1->SelectObject(m_pBgBuffBitmap1);

    if (m_nBgBuffBitmapWidth2 != rectWnd.Width() || m_nBgBuffBitmapHeight2 != rectWnd.Height()) {
        m_nBgBuffBitmapWidth2 = rectWnd.Width();
        m_nBgBuffBitmapHeight2 = rectWnd.Height();

        if (m_pBgBuffDC2 != NULL) {
            delete m_pBgBuffDC2;
            m_pBgBuffDC2 = NULL;
        }

        if (m_pBgBuffBitmap2 != NULL) {
            delete m_pBgBuffBitmap2;
            m_pBgBuffBitmap2 = NULL;
        }
    }

    if (m_pBgBuffDC2 == NULL) {
        m_pBgBuffDC2 = new CDC();
        if (m_pBgBuffDC2 == NULL) {
            return;
        }
        bSuc = m_pBgBuffDC2->CreateCompatibleDC(&dc);
        if (!bSuc) {
            return;
        }
    }

    if (m_pBgBuffBitmap2 == NULL) {
        m_pBgBuffBitmap2 = new CBitmap();
        if (m_pBgBuffBitmap2 == NULL) {
            return;
        }
        bSuc = m_pBgBuffBitmap2->CreateCompatibleBitmap(&dc, m_nBgBuffBitmapWidth2, m_nBgBuffBitmapHeight2);
        if (!bSuc) {
            return;
        }
    }
    
    m_pBgBuffDC2->SelectObject(m_pBgBuffBitmap2);

    //-----------------------------------------------------------------------------------------------
    //-- create member: image bg brush
    //--
    {
#define BG_PATTERN_HALF_WIDTH               10
#define BG_PATTERN_HALF_HEIGHT              10
#define BG_PATTERN_COLOR1                   RGB(225, 225, 225)
#define BG_PATTERN_COLOR2                   RGB(245, 245, 245)
        if (m_pBgBrush == NULL) {
            do {
                CDC dcPattern;
                CBitmap *pBitmapOld = NULL;
                CBrush patternBrush1;
                CBrush patternBrush2;
                int nPatternScaledWidth;
                int nPatternScaledHeight;

                nPatternScaledWidth = (int)(BG_PATTERN_HALF_WIDTH);
                if (nPatternScaledWidth <= 0) {
                    nPatternScaledWidth = 1;
                }
                nPatternScaledHeight = (int)(BG_PATTERN_HALF_HEIGHT);
                if (nPatternScaledHeight <= 0) {
                    nPatternScaledHeight = 1;
                }

                if (m_pBgPattern != NULL) {
                    delete m_pBgPattern;
                }
                m_pBgPattern = NULL;

                m_pBgPattern = new CBitmap();
                if (m_pBgPattern == NULL) {
                    goto DRAWING_BG_FAILDED;
                }

                bSuc = m_pBgPattern->CreateCompatibleBitmap(&dc, nPatternScaledWidth * 2, nPatternScaledHeight * 2);
                if (!bSuc) {
                    goto DRAWING_BG_FAILDED;
                }
                
                bSuc = dcPattern.CreateCompatibleDC(&dc);
                if (!bSuc) {
                    goto DRAWING_BG_FAILDED;
                }

                pBitmapOld = dcPattern.SelectObject(m_pBgPattern);
                if (pBitmapOld == NULL) {
                    goto DRAWING_BG_FAILDED;
                }

                bSuc = patternBrush1.CreateSolidBrush(BG_PATTERN_COLOR1);
                if (!bSuc) {
                    goto DRAWING_BG_FAILDED;
                }
                bSuc = patternBrush2.CreateSolidBrush(BG_PATTERN_COLOR2);
                if (!bSuc) {
                    goto DRAWING_BG_FAILDED;
                }
                
                dcPattern.FillRect(CRect(nPatternScaledWidth * 0, nPatternScaledHeight * 0, nPatternScaledWidth * 1, nPatternScaledHeight * 1), &patternBrush1);
                dcPattern.FillRect(CRect(nPatternScaledWidth * 1, nPatternScaledHeight * 1, nPatternScaledWidth * 2, nPatternScaledHeight * 2), &patternBrush1);
                dcPattern.FillRect(CRect(nPatternScaledWidth * 0, nPatternScaledHeight * 1, nPatternScaledWidth * 1, nPatternScaledHeight * 2), &patternBrush2);
                dcPattern.FillRect(CRect(nPatternScaledWidth * 1, nPatternScaledHeight * 0, nPatternScaledWidth * 2, nPatternScaledHeight * 1), &patternBrush2);

                dcPattern.SelectObject(pBitmapOld);
                pBitmapOld = NULL;

                m_pBgBrush = new CBrush(m_pBgPattern);
                if (m_pBgBrush == NULL) {
                    goto DRAWING_BG_FAILDED;
                }

DRAWING_BG_FAILDED:
                if (pBitmapOld != NULL) {
                    dcPattern.SelectObject(pBitmapOld);
                    pBitmapOld = NULL;
                }
            } while (0);
        }
    }

    //-----------------------------------------------------------------------------------------------
    //-- draw picture
    //--
    if (m_nImageDrawingFlags & (IDF_IMG_FILE_PATH_CHANGED | IDF_IMG_BG_COLOR_CHANGED | IDF_IMG_TRANSPARECY_CHANGED | IDF_IMG_DC_RECREATED)) {
        m_nImageDrawingFlags &= ~(IDF_IMG_FILE_PATH_CHANGED | IDF_IMG_BG_COLOR_CHANGED | IDF_IMG_TRANSPARECY_CHANGED | IDF_IMG_DC_RECREATED);

        DBG_TRACE("###INFO: [%s()] drawing m_pBgBuffDC1\n", __FUNCTION__);

        CRect rect(0, 0, m_nBgBuffBitmapWidth1, m_nBgBuffBitmapWidth1);
        if (m_pBgBrush != NULL) {
            m_pBgBuffDC1->FillRect(&rect, m_pBgBrush);
        } else {
            CBrush brush;
            brush.CreateSolidBrush(RGB(255, 255, 255));                 // the alpha will be 0
            m_pBgBuffDC1->FillRect(&rect, &brush);
        }

        Graphics graphics(m_pBgBuffDC1->GetSafeHdc());
        Color bgColor(m_bgA, m_bgR, m_bgG, m_bgB);
        SolidBrush bgBrush(bgColor);

        if (m_pImage != NULL) {
            delete m_pImage;
            m_pImage = NULL;
        }

        if (IsBmpFileWithAlpha()) {
            unsigned char *pFileContent = (unsigned char *)&m_vBmpWithAlphaFileContent[0];;
            BITMAPFILEHEADER *pBitmapFileHearder = (BITMAPFILEHEADER *)pFileContent;
            BITMAPINFOHEADER *pBitmapInfoHearder = (BITMAPINFOHEADER *)(pFileContent + sizeof(BITMAPFILEHEADER));
            if (pFileContent != NULL && pBitmapFileHearder != NULL && pBitmapInfoHearder != NULL) {
                m_pImage = new Bitmap(
                    pBitmapInfoHearder->biWidth, 
                    pBitmapInfoHearder->biHeight, 
                    pBitmapInfoHearder->biWidth * 4, 
                    PixelFormat32bppPARGB, 
                    &pFileContent[pBitmapFileHearder->bfOffBits]
                );
                ASSERT(m_pImage != NULL);
            }
        } else {
            m_pImage = Image::FromFile((LPCTSTR)m_strImageFilePath, FALSE);
            ASSERT(m_pImage != NULL);
        }
        
        m_nImageWidth = m_pImage->GetWidth();
        m_nImageHeight = m_pImage->GetHeight();
        if (m_nImageWidth != 0 || m_nImageHeight != 0) {
            graphics.FillRectangle(&bgBrush, 0, 0, (int)(m_nImageWidth), (int)(m_nImageHeight));

            if (m_globalAlpha == 255) {
                graphics.DrawImage(m_pImage, 0, 0, (int)(m_nImageWidth), (int)(m_nImageHeight));
            } else {
                ColorMatrix clrMatrix = { 
                    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f
                };
                ImageAttributes ImgAttr;
                Rect rectDest;

                clrMatrix.m[3][3] = (float)m_globalAlpha / (float)255;         // global alpha
                ImgAttr.SetColorMatrix(&clrMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
                rectDest = Rect(0, 0, (int)(m_nImageWidth), (int)(m_nImageHeight));
                graphics.DrawImage(m_pImage, rectDest, 0, 0, m_nImageWidth, m_nImageHeight, UnitPixel, &ImgAttr, NULL, NULL);
            }
        }
    }

    //-----------------------------------------------------------------------------------------------
    //-- draw dispaly DC
    //--
    {
        {
            CRect rect(0, 0, m_nBgBuffBitmapWidth2, m_nBgBuffBitmapHeight2);
            CBrush brush;
            brush.CreateSolidBrush(RGB(255, 255, 255));                 // the alpha will be 0
            m_pBgBuffDC2->FillRect(&rect, &brush);
        }
        m_pBgBuffDC2->SetStretchBltMode(COLORONCOLOR);
        if (m_fScaleFactor < 1.0f) {
            m_pBgBuffDC2->StretchBlt(
                m_xOriginOffset, m_yOriginOffset, (int)(m_nBgBuffBitmapWidth1 * m_fScaleFactor), (int)(m_nBgBuffBitmapHeight1 * m_fScaleFactor),
                m_pBgBuffDC1,
                0, 0, m_nBgBuffBitmapWidth1, m_nBgBuffBitmapHeight1,
                SRCCOPY
            );
        } else {
            int nNewWidth = m_nBgBuffBitmapWidth2 - m_xOriginOffset;
            int nNewHeight = m_nBgBuffBitmapHeight2 - m_yOriginOffset;
            m_pBgBuffDC2->StretchBlt(
                m_xOriginOffset, m_yOriginOffset, nNewWidth, nNewHeight,
                m_pBgBuffDC1,
                0, 0, (int)(nNewWidth / m_fScaleFactor), (int)(nNewHeight / m_fScaleFactor),
                SRCCOPY
            );
        }
    }
}

#define MEM_DC_BG_A                 0
#define MEM_DC_BG_R                 0
#define MEM_DC_BG_G                 0
#define MEM_DC_BG_B                 0

void CWorker::DrawInternalMemDC()
{
    Status status;
    Graphics graphics(m_hInternalMemDC);
    
    if (m_pImage == NULL || m_nImageWidth == 0 || m_nImageHeight == 0) {
        return;
    }

    if (0) {
        Color bgColor(MEM_DC_BG_A, MEM_DC_BG_R, MEM_DC_BG_G, MEM_DC_BG_B);
        SolidBrush bgBrush(bgColor);
        status = graphics.SetCompositingMode(CompositingModeSourceCopy);
        status = graphics.FillRectangle(&bgBrush, 0, 0, m_nImageWidth, m_nImageHeight);     // however graphics.FillRectangle() will alpha-blend with the background 
    } else {
        CRect rect(0, 0, m_nImageWidth, m_nImageHeight);
        CBrush brush;
        brush.CreateSolidBrush(RGB(MEM_DC_BG_R, MEM_DC_BG_G, MEM_DC_BG_B));                 // the alpha will be 0
        CDC::FromHandle(m_hInternalMemDC)->FillRect(&rect, &brush);
    }

    status = graphics.SetCompositingMode(CompositingModeSourceCopy);
    status = graphics.DrawImage(m_pImage, 0, 0, m_nImageWidth, m_nImageHeight);

    ::GdiFlush();
}

BOOL CWorker::GetPixelAtPos(int x, int y, BYTE &a, BYTE &r, BYTE &g, BYTE &b)
{
    if (m_pImage == NULL || m_nImageWidth == 0 || m_nImageHeight == 0 || m_pInternalBmpBits == NULL) {
        return FALSE;
    }

    if (x < 0 || x >= (int)m_nImageWidth || y < 0 || y >= (int)m_nImageHeight) {
        return FALSE;
    }

    if (x >= INTERNAL_BMP_WIDTH || y >= INTERNAL_BMP_HEIGHT) {
        return FALSE;
    }

    ColorB8G8R8A8 *pPixel = (ColorB8G8R8A8 *)((BYTE *)m_pInternalBmpBits + INTERNAL_BMP_PITCH * y + 4 * x);
    a = pPixel->a;
    r = pPixel->r;
    g = pPixel->g;
    b = pPixel->b;

    return TRUE;
}

void CWorker::RegisterImageInfoMonitor(HWND hWnd, UINT Msg)
{
    m_hImageInfoMonitorWnd = hWnd;
    m_ImageInfoMonitorMsg = Msg;
}

void CWorker::RegisterDrawingInfoMonitor(HWND hWnd, UINT Msg)
{
    m_hDrawingInfoMonitorWnd = hWnd;
    m_DrawingInfoMonitorMsg = Msg;
}

void CWorker::MoveOrigin(int xDelta, int yDelta)
{
    m_xOriginOffset += xDelta;
    m_yOriginOffset += yDelta;
}

void CWorker::SetOriginX(int xPos)
{
    m_xOriginOffset = xPos;
}

void CWorker::SetOriginY(int yPos)
{
    m_yOriginOffset = yPos;
}

void CWorker::SetWatchPoint(int x, int y)
{
    m_xWatchPoint = (int)((x - m_xOriginOffset)/ m_fScaleFactor);
    m_yWatchPoint = (int)((y - m_yOriginOffset) / m_fScaleFactor);

    SendImageInfoMessage();
}

void CWorker::SetPictureBgColor(BYTE a, BYTE r, BYTE g, BYTE b)
{
    m_bgA = a;
    m_bgR = r;
    m_bgG = g;
    m_bgB = b;

    m_nImageDrawingFlags |= IDF_IMG_BG_COLOR_CHANGED;
}

void CWorker::SetGlobalAlpha(BYTE a)
{
    m_globalAlpha = a;
    m_nImageDrawingFlags |= IDF_IMG_TRANSPARECY_CHANGED;
}

BYTE CWorker::GetGlobalAlpha()
{
    return m_globalAlpha;
}

void CWorker::SetScaleFactor(float fScaleFactor)
{
    m_fScaleFactor = fScaleFactor;
}

float CWorker::GetScaleFactor()
{
    return m_fScaleFactor;
}

void CWorker::SetImageFilePath(const CString &strImageFilePath)
{
    m_strImageFilePath = strImageFilePath;
    m_nImageDrawingFlags |= IDF_IMG_FILE_PATH_CHANGED;
}

CString CWorker::GetImageFilePath()
{
    return m_strImageFilePath;
}

BOOL CWorker::ReloadConfig()
{
    bool bRet;
    std::string sFileContent;
    entTlmXmlParser::E_ERR xmlErr;
    entTlmXmlParser::T_STRINGS vsResult;

    m_nImageDrawingFlags |= IDF_IMG_BG_COLOR_CHANGED;
    m_nImageDrawingFlags |= IDF_IMG_TRANSPARECY_CHANGED;

    m_xOriginOffset = 0;
    m_yOriginOffset = 0;

    if (m_pBgBrush != NULL) {
        delete m_pBgBrush;
        m_pBgBrush = NULL;
    }

    bRet = TK_Tools::LoadFile(g_strConfigFilePath, sFileContent);
    if (!bRet) {
        return FALSE;
    }
    std::string::size_type nPos = sFileContent.find("<?xml", 0);
    if (nPos != std::string::npos) {
        sFileContent = sFileContent.substr(nPos);
    }

    entTlmXmlParser xp(sFileContent.c_str());
    xmlErr = xp.getTextValuesByXPath("/Config/BgColor/a", vsResult);
    if (vsResult.size() >= 1) {
        m_bgA = (BYTE)TK_Tools::StrToUL(vsResult[0]);
    } else {
        return FALSE;
    }
    xmlErr = xp.getTextValuesByXPath("/Config/BgColor/r", vsResult);
    if (vsResult.size() >= 1) {
        m_bgR = (BYTE)TK_Tools::StrToUL(vsResult[0]);
    } else {
        return FALSE;
    }
    xmlErr = xp.getTextValuesByXPath("/Config/BgColor/g", vsResult);
    if (vsResult.size() >= 1) {
        m_bgG = (BYTE)TK_Tools::StrToUL(vsResult[0]);
    } else {
        return FALSE;
    }
    xmlErr = xp.getTextValuesByXPath("/Config/BgColor/b", vsResult);
    if (vsResult.size() >= 1) {
        m_bgB = (BYTE)TK_Tools::StrToUL(vsResult[0]);
    } else {
        return FALSE;
    }

    xmlErr = xp.getTextValuesByXPath("/Config/GlobalAlpha", vsResult);
    if (vsResult.size() >= 1) {
        m_globalAlpha = (BYTE)TK_Tools::StrToUL(vsResult[0]);
    } else {
        return FALSE;
    }

    xmlErr = xp.getTextValuesByXPath("/Config/ScaleFactor", vsResult);
    if (vsResult.size() >= 1) {
        m_fScaleFactor = (BYTE)TK_Tools::StrToFloat(vsResult[0]);
    } else {
        return FALSE;
    }

    xmlErr = xp.getTextValuesByXPath("/Config/ExportedBmpFileColorMasks/BottomUp", vsResult);
    if (vsResult.size() >= 1) {
        m_bExportedBmpBottomUp = (DWORD)TK_Tools::StrToL(vsResult[0]);
    } else {
        return FALSE;
    }

    xmlErr = xp.getTextValuesByXPath("/Config/ExportedBmpFileColorMasks/ColorOrder", vsResult);
    if (vsResult.size() >= 1) {
        m_sExportedBmpFileColorOrder = vsResult[0].c_str();
    } else {
        return FALSE;
    }

    return TRUE;
}

void CWorker::CreateInternalMemDC()
{   
    if (m_hInternalMemDC != NULL) {
        DeleteDC(m_hInternalMemDC);
        m_hInternalMemDC = NULL;
    }
    m_hInternalMemDC = ::CreateCompatibleDC(NULL);
    ASSERT(m_hInternalMemDC != NULL);

    if (m_hInternalMemBmp != NULL) {
        DeleteObject(m_hInternalMemBmp);
        m_hInternalMemBmp = NULL;
    }

    BITMAPINFOHEADER &bmih = m_BitmapInfo.bmih;
    memset(&bmih, 0, sizeof(BITMAPINFOHEADER));
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = INTERNAL_BMP_WIDTH;
    bmih.biHeight = -INTERNAL_BMP_HEIGHT;
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = BI_BITFIELDS;
    bmih.biSizeImage = INTERNAL_BMP_PITCH * INTERNAL_BMP_HEIGHT;
    bmih.biXPelsPerMeter = 0;
    bmih.biYPelsPerMeter = 0;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;
    DWORD *pBitsFields = (DWORD *)(&bmih + 1);
    pBitsFields[0] = 0x00FF0000;              // r
    pBitsFields[1] = 0x0000FF00;              // g
    pBitsFields[2] = 0x000000FF;              // b
    m_hInternalMemBmp = ::CreateDIBSection(
        m_hInternalMemDC,
        (CONST BITMAPINFO *)&m_BitmapInfo,
        DIB_RGB_COLORS,
        (VOID **)&m_pInternalBmpBits,
        NULL,
        NULL
    );
    ASSERT(m_pInternalBmpBits != NULL);

    HGDIOBJ hGdiObj = ::SelectObject(m_hInternalMemDC, m_hInternalMemBmp);
    ASSERT(hGdiObj != HGDI_ERROR);
}

void CWorker::SendImageInfoMessage()
{
    BOOL bSuc;
    ImageInfoData data;
    BYTE a, r, g, b;

    memset(&data, 0, sizeof(ImageInfoData));
    data.nWidth = m_nImageWidth;
    data.nHeight = m_nImageHeight;

    bSuc = GetPixelAtPos(m_xWatchPoint, m_yWatchPoint, a, r, g, b);
    if (!bSuc) {
        data.x = -1;
        data.y = -1;
        data.a = 0;
        data.r = 0;
        data.g = 0;
        data.b = 0;
    } else {
        data.x = m_xWatchPoint;
        data.y = m_yWatchPoint;
        data.a = a;
        data.r = r;
        data.g = g;
        data.b = b;
    }

    data.fScaleFactor = m_fScaleFactor;

    ::SendMessage(m_hImageInfoMonitorWnd, m_ImageInfoMonitorMsg, NULL, (LPARAM)&data);
}

template<typename T>
T ClipInScope(const T &value, const T &min, const T &max)
{
    T result = value;

    if (value < min) {
        result = min;
    }

    if (value > max) {
        result = max;
    }

    return result;
}

void CWorker::SendDrawingInfoMessage()
{
    DrawingInfoData did;

    memset(&did, 0, sizeof(DrawingInfoData));

    did.siHorizon.cbSize = sizeof(SCROLLINFO);
    did.siHorizon.fMask = SIF_ALL;
    did.siHorizon.nMin = 0;
    did.siHorizon.nMax = (int)(m_nBgBuffBitmapWidth1 * m_fScaleFactor);
    did.siHorizon.nPage = m_nBgBuffBitmapWidth2;
    did.siHorizon.nPos = ClipInScope(-m_xOriginOffset, did.siHorizon.nMin, did.siHorizon.nMax);

    DBG_TRACE("###INFO: [%s()] m_xOriginOffset=%d\n", __FUNCTION__, m_xOriginOffset);

    did.siVertical.cbSize = sizeof(SCROLLINFO);
    did.siVertical.fMask = SIF_ALL;
    did.siVertical.nMin = 0;
    did.siVertical.nMax = (int)(m_nBgBuffBitmapHeight1 * m_fScaleFactor);
    did.siVertical.nPage = m_nBgBuffBitmapHeight2;
    did.siVertical.nPos = ClipInScope(-m_yOriginOffset, did.siVertical.nMin, did.siVertical.nMax);

    DBG_TRACE("###INFO: [%s()] m_yOriginOffset=%d\n", __FUNCTION__, m_yOriginOffset);

    ::SendMessage(m_hDrawingInfoMonitorWnd, m_DrawingInfoMonitorMsg, NULL, (LPARAM)&did);
}

BOOL CWorker::ShowPreviousPicture()
{
    BOOL bRet = FALSE;
    CString strCurrentPathName;

    if (!m_fileNameTracker.IsSameDir(m_strImageFilePath)) {
        bRet = m_fileNameTracker.SetPathName(m_strImageFilePath);
        if (!bRet) {
            goto FAILED;
        }
    }

    bRet = m_fileNameTracker.GetCurrentPathName(strCurrentPathName);
    if (!bRet) {
        goto FAILED;
    }

    while (true) {
        bRet = m_fileNameTracker.GoPrevious(strCurrentPathName);
        if (!bRet) {
            goto FAILED;
        }

        if (IsPictureFile(strCurrentPathName)) {
            break;
        }
    }

    m_strImageFilePath = strCurrentPathName;
    m_nImageDrawingFlags |= IDF_IMG_FILE_PATH_CHANGED;
    bRet = TRUE;

FAILED:
    if (!bRet) {
        m_fileNameTracker.Reset();
    }
    return bRet;
}

BOOL CWorker::ShowNextPicture()
{
    BOOL bRet = FALSE;
    CString strCurrentPathName;

    if (!m_fileNameTracker.IsSameDir(m_strImageFilePath)) {
        bRet = m_fileNameTracker.SetPathName(m_strImageFilePath);
        if (!bRet) {
            goto FAILED;
        }
    }

    bRet = m_fileNameTracker.GetCurrentPathName(strCurrentPathName);
    if (!bRet) {
        goto FAILED;
    }

    while (true) {
        bRet = m_fileNameTracker.GoNext(strCurrentPathName);
        if (!bRet) {
            goto FAILED;
        }
        if (IsPictureFile(strCurrentPathName)) {
            break;
        }
    }

    m_strImageFilePath = strCurrentPathName;
    m_nImageDrawingFlags |= IDF_IMG_FILE_PATH_CHANGED;
    bRet = TRUE;

FAILED:
    if (!bRet) {
        m_fileNameTracker.Reset();
    }
    return bRet;
}

BOOL CWorker::RefreshPictureDir()
{
    BOOL bRet = FALSE;
    CString strCurrentPathName;

    bRet = m_fileNameTracker.SetPathName(m_strImageFilePath);
    if (!bRet) {
        goto FAILED;
    }
    m_fileNameTracker.ResetIndex();
    bRet = m_fileNameTracker.GetCurrentPathName(strCurrentPathName);
    if (!bRet) {
        goto FAILED;
    }

    while (true) {
        if (IsPictureFile(strCurrentPathName)) {
            break;
        }
        bRet = m_fileNameTracker.GoNext(strCurrentPathName);
        if (!bRet) {
            goto FAILED;
        }
    }

    m_strImageFilePath = strCurrentPathName;
    m_nImageDrawingFlags |= IDF_IMG_FILE_PATH_CHANGED;
    bRet = TRUE;

FAILED:
    if (!bRet) {
        m_fileNameTracker.Reset();
    }
    return bRet;
}

BOOL CWorker::IsPictureFile(const CString &strPathName)
{
    BOOL bRet = FALSE;
    Image *pImage = NULL;

    pImage = Image::FromFile((LPCTSTR)strPathName, FALSE);
    if (pImage == NULL) {
        bRet = FALSE;
        goto FAILED;
    }
    
    if (pImage->GetWidth() == 0 || pImage->GetHeight() == 0) {
        bRet = FALSE;
        goto FAILED;
    }

    bRet = TRUE;
    
FAILED:
    if (pImage != NULL) {
        delete pImage;
        pImage = NULL;
    }

    return bRet;
}

