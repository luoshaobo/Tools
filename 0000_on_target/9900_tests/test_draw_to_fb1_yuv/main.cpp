#include <main.h>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QStaticText>
#include <g2d.h>

#define SCREEN_WIDTH                1280
#define SCREEN_HEIGHT               720

#define CLAMP(n,min,max)                        (((n) < (min)) ? (min) : ((n) > (max)) ? (max) : (n))

extern "C"
{
    struct Rect {
        int x, y;
        unsigned int w, h;
    };
    extern void colorSpaceConvert(const void *buf_argb, void *buf_yuv, const struct Rect *rect);
}

AnimationScreen g_AnimationScreen;
QImage *g_pScreenImage = NULL;
QPainter *g_pScreenPainter = NULL;
unsigned char g_yuvBuf[SCREEN_WIDTH * 2 * SCREEN_HEIGHT];

//void colorSpaceConvert(const void *buf_argb, void *buf_yuv, const Rect *rect)
//{
//    unsigned long i, j;
//    int nInputOffsetR = 2;
//    int nInputOffsetG = 1;
//    int nInputOffsetB = 0;
//    int nInputOffsetA = 3;
//    unsigned long nBmpPitch = SCREEN_WIDTH * 4;
//    unsigned long nYuvBitsPitch = SCREEN_WIDTH;
//    unsigned char *pDstPixelY, *pDstPixelU, *pDstPixelV;
//    unsigned char *pSrcPixel;

//    for (i = rect->y; i < rect->y + rect->h; i++) {
//        for (j = rect->x; j < rect->x + rect->w; j++) {
//            pDstPixelY = (unsigned char *)((unsigned char *)buf_yuv + nYuvBitsPitch * i * 2 + j * 2 + 1);
//            if ((j % 2) == 0) {
//                pDstPixelU = (unsigned char *)pDstPixelY - 1;
//                pDstPixelV = NULL;
//            } else {
//                pDstPixelU = NULL;
//                pDstPixelV = (unsigned char *)pDstPixelY - 1;
//            }
//            pSrcPixel = (unsigned char *)((unsigned char *)buf_argb + nBmpPitch * i + j * 4);

//            double lyuvY = ((0.256788 * pSrcPixel[nInputOffsetR] + 0.504129 * pSrcPixel[nInputOffsetG] + 0.097906 * pSrcPixel[nInputOffsetB])) + 16;
//            double lyuvU = ((-0.148223 * pSrcPixel[nInputOffsetR] - 0.290993 * pSrcPixel[nInputOffsetG] + 0.439216 * pSrcPixel[nInputOffsetB])) + 128;
//            double lyuvV = ((0.439216 * pSrcPixel[nInputOffsetR] - 0.367788 * pSrcPixel[nInputOffsetG] - 0.071427 * pSrcPixel[nInputOffsetB])) + 128;

//            unsigned char yuvY, yuvU, yuvV;
//            yuvY = (unsigned char)CLAMP(lyuvY, 16, 235);
//            yuvU = (unsigned char)CLAMP(lyuvU, 16, 240);
//            yuvV = (unsigned char)CLAMP(lyuvV, 16, 240);

//            *pDstPixelY = yuvY;
//            if (pDstPixelU != NULL) {
//                *pDstPixelU = yuvU;
//            }
//            if (pDstPixelV != NULL) {
//                *pDstPixelV = yuvV;
//            }
//        }
//    }
//}

//void colorSpaceConvert(const void *buf_argb, void *buf_yuv, const Rect *rect)
//{
//    unsigned long i, j;
//    int nInputOffsetR = 2;
//    int nInputOffsetG = 1;
//    int nInputOffsetB = 0;
//    int nInputOffsetA = 3;
//    unsigned long nBmpPitch = SCREEN_WIDTH * 4;
//    unsigned long nYuvBitsPitch = SCREEN_WIDTH;
//    unsigned char *pDstPixelY, *pDstPixelU, *pDstPixelV;
//    unsigned char *pSrcPixel;

//    for (i = rect->y; i < rect->y + rect->h; i++) {
//        for (j = rect->x; j < rect->x + rect->w; j++) {
//            pDstPixelY = (unsigned char *)((unsigned char *)buf_yuv + nYuvBitsPitch * i * 2 + j * 2 + 1);
//            if ((i % 2) == 0) {
//                pDstPixelU = (unsigned char *)pDstPixelY - 1;
//                pDstPixelV = (unsigned char *)pDstPixelU + nYuvBitsPitch * 2;
//            } else {
//                pDstPixelU = NULL;
//                pDstPixelV = NULL;
//            }
//            pSrcPixel = (unsigned char *)((unsigned char *)buf_argb + nBmpPitch * i + j * 4);

//            double lyuvY = 77 * pSrcPixel[nInputOffsetR] + 150 * pSrcPixel[nInputOffsetG] + 29 * pSrcPixel[nInputOffsetB];
//            double lyuvU = -43 * pSrcPixel[nInputOffsetR] - 84 * pSrcPixel[nInputOffsetG] + 127 * pSrcPixel[nInputOffsetB];
//            double lyuvV = 127 * pSrcPixel[nInputOffsetR] - 106 * pSrcPixel[nInputOffsetG] - 21 * pSrcPixel[nInputOffsetB];

//            lyuvY = (lyuvY + 128) / 256;
//            lyuvU = (lyuvU + 128) / 256;
//            lyuvV = (lyuvV + 128) / 256;

//            lyuvY = lyuvY;
//            lyuvU += 128;
//            lyuvV += 128;

//            unsigned char yuvY, yuvU, yuvV;
//            yuvY = (unsigned char)CLAMP(lyuvY, 0, 255);
//            yuvU = (unsigned char)CLAMP(lyuvU, 0, 255);
//            yuvV = (unsigned char)CLAMP(lyuvV, 0, 255);

//            *pDstPixelY = yuvY;
//            if (pDstPixelU != NULL) {
//                *pDstPixelU = yuvU;
//            }
//            if (pDstPixelV != NULL) {
//                *pDstPixelV = yuvV;
//            }
//        }
//    }
//}

//void colorSpaceConvert(const void *buf_argb, void *buf_yuv, const Rect *rect)
//{
//    void *handle = NULL;
//    struct g2d_surface srcSurface;
//    struct g2d_surface dstSurface;

//    g2d_open(&handle);

//    memset(&srcSurface, 0, sizeof(struct g2d_surface));
//    srcSurface.planes[0] = (int)buf_argb;
//    srcSurface.left = rect->x;
//    srcSurface.top = rect->y;
//    srcSurface.right = rect->x + rect->w;
//    srcSurface.bottom = rect->y + rect->h;
//    srcSurface.stride = SCREEN_WIDTH * 4;
//    srcSurface.width = SCREEN_WIDTH;
//    srcSurface.height = SCREEN_HEIGHT;
//    srcSurface.rot    = G2D_ROTATION_0;
//    srcSurface.format = G2D_RGBA8888;

//    memset(&dstSurface, 0, sizeof(struct g2d_surface));
//    dstSurface.planes[0] = (int)buf_yuv;
//    dstSurface.planes[1] = (int)buf_yuv;
//    dstSurface.planes[2] = (int)buf_yuv;
//    dstSurface.left = rect->x;
//    dstSurface.top = rect->y;
//    dstSurface.right = rect->x + rect->w;
//    dstSurface.bottom = rect->y + rect->h;
//    dstSurface.stride = SCREEN_WIDTH * 2;
//    dstSurface.width = SCREEN_WIDTH;
//    dstSurface.height = SCREEN_HEIGHT;
//    dstSurface.rot    = G2D_ROTATION_0;
//    dstSurface.format = G2D_UYVY;

//    g2d_blit(handle, &srcSurface, &dstSurface);
//    g2d_finish(handle);

//    g2d_close(handle);
//}

void AnimationScreen::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId) {
        static unsigned int i = 0;

        LOG_GEN();
        if (g_pScreenImage == NULL) {
            LOG_GEN();
            g_pScreenImage = new QImage(1280, 720, QImage::Format_ARGB32);
        }
        LOG_GEN();
        if (g_pScreenPainter == NULL) {
            LOG_GEN();
            g_pScreenPainter = new QPainter(g_pScreenImage);
        }

        Rect rect;
        rect.x = 10;
        rect.y = 10;
        rect.w = 150; //SCREEN_WIDTH;
        rect.h = 100; //SCREEN_HEIGHT;

        LOG_GEN();
        if (g_pScreenPainter != NULL && g_pScreenImage != NULL) {
            LOG_GEN();
            QColor color((i++ % 256), 0, 0, 255);
            g_pScreenPainter->fillRect(rect.x, rect.y, rect.w, rect.h, color);
            //QImage image("/home/root/sbin/xihu_1280x720.jpg");
            //g_pScreenPainter->drawImage(0, 0, image);

            g_pScreenPainter->setCompositionMode(QPainter::CompositionMode_Source);

            QPen pen = g_pScreenPainter->pen();
            pen.setColor(Qt::white);
            QFont font = g_pScreenPainter->font();
            font.setFamily("Eurostile-CH_forGW");
            font.setBold(true);
            font.setPixelSize(50);
            g_pScreenPainter->setPen(pen);
            g_pScreenPainter->setFont(font);

            QString sText = QString::asprintf("%d", i++);
            g_pScreenPainter->drawLine(0, 0, 300, 300);
            g_pScreenPainter->drawText(0, 50, sText);

            const uchar *pBits = g_pScreenImage->bits();



            colorSpaceConvert((const void *)pBits, (void *)NULL, &rect);

//            colorSpaceConvert((const void *)pBits, (void *)g_yuvBuf, &rect);

//            LOG_GEN();
//            FILE *pFile = fopen("/dev/fb1", "w");
//            if (pFile != NULL) {
//                LOG_GEN();
//                fwrite(g_yuvBuf, 1, SCREEN_WIDTH * 2 * SCREEN_HEIGHT, pFile); // buffer one
//                fwrite(g_yuvBuf, 1, SCREEN_WIDTH * 2 * SCREEN_HEIGHT, pFile); // buffer two
//                fclose(pFile);
//            }
        }
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    system("echo 0 > /sys/devices/soc0/fb.20/graphics/fb1/blank");    // to show fb1
    AnimationScreen as;
    as.startShowing();

    return a.exec();
}

// log.cpp
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <libgen.h>

void log_printf(const char *pFormat, ...)
{
    int nError = 0;
    va_list args;
    int len = 0;
    char *buffer = NULL;

    if (nError == 0) {
        if (pFormat == NULL) {
            nError = 1;
        }
    }

    if (nError == 0) {
        va_start(args, pFormat);
        len = vsnprintf(NULL, 0, pFormat, args);
        if (len <= 0) {
            len = 1024;
        }
        buffer = (char *)malloc((len + 1) * sizeof(char));
        if (buffer == NULL) {
           nError = -1;
        }
        va_end(args);
    }

    if (nError == 0) {
        va_start(args, pFormat);
        vsnprintf(buffer, len + 1, pFormat, args);
        buffer[len] = '\0';
        printf(buffer);                                                       // to be changed to any function which can output a string
        va_end(args);
    }

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    return;
}

const char *log_basename(const char *path)
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

const char *log_strcat(unsigned int arg_count, ...)
{
    char *p = NULL;
    va_list arg_ptr;
    unsigned int len = 0;
    unsigned int i;
    char *pArg;

    va_start(arg_ptr, arg_count);
    for (i = 0; i < arg_count; i++) {
        pArg = va_arg(arg_ptr, char *);
        if (pArg != NULL) {
            len += strlen(pArg);
        }
    }
    va_end(arg_ptr);

    p = (char *)malloc(len + 1);
    if (p != NULL) {
        p[0] = '\0';
        va_start(arg_ptr, arg_count);
        for (i = 0; i < arg_count; i++) {
            pArg = va_arg(arg_ptr, char *);
            if (pArg != NULL) {
                strcat(p, pArg);
            }
        }
        va_end(arg_ptr);
    }

    return p;
}

