#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include <list>
#include <map>
#include <QCoreApplication>
#include <QImage>
#include <QColor>
#include "TK_Tools.h"
#include "TK_Bitmap.h"

using namespace TK_Tools;

const char *basename(const char *path)
{
    const char *pRet = path;

    if (path != NULL) {
        char *p = strrchr((char *)path, '/');
        if (p != NULL) {
            pRet = (const char *)(p + 1);
        }
    }

    return pRet;
}

void usage(int argc, char *argv[])
{
    TK_UNUSED_VAR(argc);

    printf("Usage:\n");
    printf("  %s <src_alpha_min> <src_red_min> <src_green_min> <src_blue_min> <src_alpha_max> <src_red_max> <src_green_max> <src_blue_max> <dst_alpha> <dst_red> <dst_green> <dst_blue> <src_image_path> <dst_bmp_path>\n", basename(argv[0]));

    printf("Note:\n");

    printf("  <src_alpha_min>: [0, 255] or \"-\" to ignore to compare.\n");
    printf("  <src_red_min>: [0, 255] or \"-\" to ignore to compare.\n");
    printf("  <src_green_min>: [0, 255] or \"-\" to ignore to compare.\n");
    printf("  <src_blue_min>: [0, 255] or \"-\" to ignore to compare.\n");

    printf("  <src_alpha_max>: [0, 255] or \"-\" to ignore to compare.\n");
    printf("  <src_red_max>: [0, 255] or \"-\" to ignore to compare.\n");
    printf("  <src_green_max>: [0, 255] or \"-\" to ignore to compare.\n");
    printf("  <src_blue_max>: [0, 255] or \"-\" to ignore to compare.\n");

    printf("  <dst_alpha>: [0, 255] or \"-\" to ignore to write.\n");
    printf("  <dst_red>: [0, 255] or \"-\" to ignore to write.\n");
    printf("  <dst_green>: [0, 255] or \"-\" to ignore to write.\n");
    printf("  <dst_blue>: [0, 255] or \"-\" to ignore to write.\n");

    printf("  <src_image_path>: the path of the input image file or \"-\" to read from stdin.\n");
    printf("  <dst_bmp_path>: the path of the output bmp file or \"-\" to write to stdout.\n");
}

#define MAX_U32     (0xFFFFFFFF)

int cmd_handler__convert_rgba(int argc, char *argv[])
{
    int nRet = 0;

    std::string sSrcAlphaMin;
    std::string sSrcRedMin;
    std::string sSrcGreenMin;
    std::string sSrcBlueMin;

    std::string sSrcAlphaMax;
    std::string sSrcRedMax;
    std::string sSrcGreenMax;
    std::string sSrcBlueMax;

    std::string sDstAlpha;
    std::string sDstRed;
    std::string sDstGreen;
    std::string sDstBlue;

    std::string sSrcImagePath;
    std::string sDstBmpPath;

    unsigned int nSrcAlphaMin;
    unsigned int nSrcRedMin;
    unsigned int nSrcGreenMin;
    unsigned int nSrcBlueMin;

    unsigned int nSrcAlphaMax;
    unsigned int nSrcRedMax;
    unsigned int nSrcGreenMax;
    unsigned int nSrcBlueMax;

    unsigned int nDstAlpha;
    unsigned int nDstRed;
    unsigned int nDstGreen;
    unsigned int nDstBlue;

    std::vector<unsigned char> vSrcImageFileContent;
    QImage srcImage;
    unsigned int nImageWidth = 0;
    unsigned int nImageHeight = 0;
    unsigned char *pOutBitsBuf = NULL;
    unsigned int i, j;

    if (nRet == 0) {
        if (argc < 15) {
            usage(argc, argv);
            nRet = -1;
        }
    }

    if (nRet == 0) {
        sSrcAlphaMin = argv[1];
        sSrcRedMin = argv[2];
        sSrcGreenMin = argv[3];
        sSrcBlueMin = argv[4];

        sSrcAlphaMax = argv[5];
        sSrcRedMax = argv[6];
        sSrcGreenMax = argv[7];
        sSrcBlueMax = argv[8];

        sDstAlpha = argv[9];
        sDstRed = argv[10];
        sDstGreen = argv[11];
        sDstBlue = argv[12];

        sSrcImagePath = argv[13];
        sDstBmpPath = argv[14];

        if (sSrcAlphaMin == "-") {
            nSrcAlphaMin = MAX_U32;
        } else {
            nSrcAlphaMin = StrToUL(sSrcAlphaMin);
        }

        if (sSrcRedMin == "-") {
            nSrcRedMin = MAX_U32;
        } else {
            nSrcRedMin = StrToUL(sSrcRedMin);
        }

        if (sSrcGreenMin == "-") {
            nSrcGreenMin = MAX_U32;
        } else {
            nSrcGreenMin = StrToUL(sSrcGreenMin);
        }

        if (sSrcBlueMin == "-") {
            nSrcBlueMin = MAX_U32;
        } else {
            nSrcBlueMin = StrToUL(sSrcBlueMin);
        }

        if (sSrcAlphaMax == "-") {
            nSrcAlphaMax = MAX_U32;
        } else {
            nSrcAlphaMax = StrToUL(sSrcAlphaMax);
        }

        if (sSrcRedMax == "-") {
            nSrcRedMax = MAX_U32;
        } else {
            nSrcRedMax = StrToUL(sSrcRedMax);
        }

        if (sSrcGreenMax == "-") {
            nSrcGreenMax = MAX_U32;
        } else {
            nSrcGreenMax = StrToUL(sSrcGreenMax);
        }

        if (sSrcBlueMax == "-") {
            nSrcBlueMax = MAX_U32;
        } else {
            nSrcBlueMax = StrToUL(sSrcBlueMax);
        }

        if (sDstAlpha == "-") {
            nDstAlpha = MAX_U32;
        } else {
            nDstAlpha = StrToUL(sDstAlpha);
        }

        if (sDstRed == "-") {
            nDstRed = MAX_U32;
        } else {
            nDstRed = StrToUL(sDstRed);
        }

        if (sDstGreen == "-") {
            nDstGreen = MAX_U32;
        } else {
            nDstGreen = StrToUL(sDstGreen);
        }

        if (sDstBlue == "-") {
            nDstBlue = MAX_U32;
        } else {
            nDstBlue = StrToUL(sDstBlue);
        }
    }

    if (nRet == 0) {
        if (!GetContentFromFile(sSrcImagePath, vSrcImageFileContent)) {
            fprintf(stderr, "*** Error: GetContentFromFile(sSrcImagePath,vSrcImageFileContent) is failed!\n");
            nRet = -1;
        }
    }

    if (nRet == 0) {
        if (!srcImage.loadFromData(&vSrcImageFileContent[0], vSrcImageFileContent.size())) {
            fprintf(stderr, "*** Error: srcImage.loadFromData(&vSrcImageFileContent[0], vSrcImageFileContent.size()) is failed!\n");
            nRet = -1;
        }
    }

    if (nRet == 0) {
        nImageWidth = srcImage.width();
        nImageHeight = srcImage.height();
        pOutBitsBuf = new unsigned char[nImageWidth * 4 * nImageHeight];
        if (pOutBitsBuf == NULL) {
            fprintf(stderr, "*** Error: new unsigned char[nImageWidth * 4 * nImageHeight] is failed!\n");
            nRet = -1;
        }
    }

    if (nRet == 0) {
        for (i = 0; i < nImageHeight; i++) {
            for (j = 0; j < nImageWidth; j++) {
                QRgb srcColor = srcImage.pixel(j, i);
                unsigned char in_a = qAlpha(srcColor);
                unsigned char in_r = qRed(srcColor);
                unsigned char in_g = qGreen(srcColor);
                unsigned char in_b = qBlue(srcColor);

                Color_BGRA32 *pDstColor = (Color_BGRA32 *)&pOutBitsBuf[nImageWidth * 4 * i + j * 4];
                unsigned char &out_a = pDstColor->a;
                unsigned char &out_r = pDstColor->r;
                unsigned char &out_g = pDstColor->g;
                unsigned char &out_b = pDstColor->b;

                if (    (nSrcAlphaMin == MAX_U32 || nSrcAlphaMax == MAX_U32 || (nSrcAlphaMin <= in_a && in_a <= nSrcAlphaMax))
                     && (nSrcRedMin == MAX_U32 || nSrcRedMax == MAX_U32 || (nSrcRedMin <= in_r && in_r <= nSrcRedMax))
                     && (nSrcGreenMin == MAX_U32 || nSrcGreenMax == MAX_U32 || (nSrcGreenMin <= in_g && in_g <= nSrcGreenMax))
                     && (nSrcBlueMin == MAX_U32 || nSrcBlueMax == MAX_U32 || (nSrcBlueMin <= in_b && in_b <= nSrcBlueMax))
                ) {
                    if (nDstAlpha == MAX_U32) {
                        out_a = in_a;
                    } else {
                        out_a = nDstAlpha;
                    }

                    if (nDstRed == MAX_U32) {
                        out_r = in_r;
                    } else {
                        out_r = nDstRed;
                    }

                    if (nDstGreen == MAX_U32) {
                        out_g = in_g;
                    } else {
                        out_g = nDstGreen;
                    }

                    if (nDstBlue == MAX_U32) {
                        out_b = in_b;
                    } else {
                        out_b = nDstBlue;
                    }
                } else {
                    out_a = in_a;
                    out_r = in_r;
                    out_g = in_g;
                    out_b = in_b;
                }
            }
        }
    }

    if (nRet == 0) {
        if (!SaveBmpToFile_RGBA32(
            sDstBmpPath.c_str(),
            (const void *)pOutBitsBuf,
            nImageWidth,
            nImageHeight,
            nImageWidth * 4,
            FALSE,
            "bgra"
        )) {
            fprintf(stderr, "*** Error: SaveBmpToFile_RGBA32() is failed!\n");
            nRet = -1;
        }
    }

    if (pOutBitsBuf != NULL) {
        delete [] pOutBitsBuf;
        pOutBitsBuf = NULL;
    }

    return nRet;
}

int main(int argc, char *argv[])
{
    int nRet = 0;

    if (nRet == 0) {
        nRet = cmd_handler__convert_rgba(argc, argv);
    }

    return nRet;
}
