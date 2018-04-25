#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TK_Tools.h"
#include "TK_Bitmap.h"

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

#if 1
#define BI_BITFIELDS  3L

#pragma pack(push, 1)
struct BITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} __attribute__((packed,aligned(1)));
#pragma pack(pop)

#pragma pack(push, 1)
struct BITMAPINFOHEADER {
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} __attribute__((packed,aligned(1)));
#pragma pack(pop)
#endif // #if 1

namespace TK_Tools {

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

    if (strcmp(pFilePath, "-") == 0) {
        pFile = stdout;
    } else {
        pFile = fopen(pFilePath, "w+b");
        if (pFile == NULL) {
            LOG("*** ERROR: %s(): fopen(pFilePath, \"w+b\") is failed!\n", __FUNCTION__);
            bRet = FALSE;
            goto FAILED;
        }
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
        if (pFile != stdout) {
            fclose(pFile);
        }
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

}// namespace TK_Tools {
