#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fb.h>
#include <string>
#include <list>
#include <map>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QBuffer>
#include "TK_Tools.h"
#include "TK_Bitmap.h"

using namespace TK_Tools;

#define PFL()        fprintf(stderr, "%s: %d: %s\n", __FILE__, __LINE__, __FUNCTION__)
#define CLAMP(n,min,max) (((n) < (min)) ? (min) : ((n) > (max)) ? (max) : (n))

struct LocalColor_Bgra {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};

struct OutputInfo {
    OutputInfo() : format(), x(0), y(0), width(0), height(0) {}

    std::string format;
    int x;
    int y;
    unsigned int width;
    unsigned int height;
};

std::vector<unsigned char> g_vStdinInputFileContent;

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

    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    %s "
            "[--help|-H] "
            "[--input_format|-f <input_picture_file_format>] "
            "[--output_format|-F <output_picture_file_format>] "
            "[--input_file|-i <input_picture_file_path>] "
            "[--output_file|-o <output_picture_file_path>] "
            "[--output_info_file|-O <output_info_file_path>] "
            "[--output_info_position|-P <x>,<y>] "
            "[--clear|-C <clear_color>] "
            "[--width|-w <picture_width>] "
            "[--height|-h <picture_heght>] "
            "[--from_current_foreground_frame_buffer|-c] "
            "[--to_next_background_frame_buffer|-n] "
            "[--input_skipped_frame_count|-s <input_skipped_frame_count>] "
            "[--output_skipped_frame_count|-S <output_skipped_frame_count>] "
            "[<input_picture_file_path>[, <output_picture_file_path>[, <output_info_file_path>]]]"
            "\n", 
            basename(argv[0]));
            
    fprintf(stderr, "Comments:\n");
    fprintf(stderr, "    --help|-H: to ouput this help information.\n");
    fprintf(stderr, "    --input_format|-f <input_picture_file_format>: the picture format of the input image file.\n");
    fprintf(stderr, "        <input_picture_file_format> can be \"raw_bgra\", \"raw_uyvy\", or \"normal\"(default).\n");
    fprintf(stderr, "    --output_format|-F <output_picture_file_format>: the picture format of the output image file.\n");
    fprintf(stderr, "        <output_picture_file_format> can be \"raw_bgra\", \"raw_uyvy\", or \"normal\"(default).\n");
    fprintf(stderr, "    --input_file|-i <input_picture_file_path>: the path of the input image file.\n");
    fprintf(stderr, "        <input_picture_file_path> can be a normal file path, a device file path, or -(stdin, as default).\n");
    fprintf(stderr, "    --output_file|-o <output_picture_file_path>: the path of the output image file.\n");
    fprintf(stderr, "        <output_picture_file_path> can be a normal file path, a device file path, -(stdout, as default when raw_bgra or raw_uyvy), or \'-.<picture_file_suffix>\'(stdout, \'-.png\' as default otherwise).\n");
    fprintf(stderr, "    --output_info_file|-O <output_info_file_path>: the path of the information file of the output image file.\n");
    fprintf(stderr, "        <output_info_file_path> should be a normal file path.\n");
    fprintf(stderr, "    --output_info_position|-P <x>,<y>: the position of the output picture file to be drawn to.\n");
    fprintf(stderr, "        The information is just to put into <output_info_file_path>.\n");
    fprintf(stderr, "    --clear|-C <clear_color>: to clear the output with the given color.\n");
    fprintf(stderr, "        <clear_color> can be \"red\", \"blue\", ect, or \'#AARRGGBB\'.\n");
    fprintf(stderr, "    --width|-w <picture_width>: the width of the input/output image file.\n");
    fprintf(stderr, "        It is just used when the picture format of the input/output image file is \"raw_bgra\" or \"raw_uyvy\", 1280 as default.\n");
    fprintf(stderr, "    --height|-h <picture_heght>: the height of the input/output image file.\n");
    fprintf(stderr, "        It is just used when the picture format of the input/output image file is \"raw_bgra\" or \"raw_uyvy\", 720 as default.\n");
    fprintf(stderr, "    --from_current_foreground_frame_buffer|-c: to use only the current foreground frame buffer as the input.\n");
    fprintf(stderr, "        It is just used when the input file is a fb device file.\n");
    fprintf(stderr, "    --to_next_background_frame_buffer|-n: to use only the next background frame buffer as the output.\n");
    fprintf(stderr, "        It is just used when the output file is a fb device file.\n");
    fprintf(stderr, "    --input_skipped_frame_count|-s <input_skipped_frame_count>: to skip some frames from the beginning of the input file.\n");
    fprintf(stderr, "    --output_skipped_frame_count|-S <output_skipped_frame_count>: to skip some frames to the beginning of the output file.\n");
}

#if 0
bool ConvertFromYuv12ToBgra(unsigned int width, unsigned height, const std::vector<unsigned char> &input, std::vector<unsigned char> &output)
{
   bool ret = true;
   unsigned int i, j;
   unsigned char *pSrcY, *pSrcU, *pSrcV;
   LocalColor_Bgra *pDest;

   output.clear();
   output.resize(width * height * 4);

   if (ret) {
       if ((width % 2) != 0 || (height % 2) != 0) {
           fprintf(stderr, "*** ERROR: both the width and height should be even.\n");
           ret = false;
       }
   }

   if (ret) {
       if (width * height * 3 / 2 > input.size()) {
           fprintf(stderr, "*** ERROR: the conent of the input file is too short.\n");
           ret = false;
       }
   }

   if (ret) {
       for (i = 0; i < height; i++) {
           for (j = 0; j < width; j++) {
               pSrcY = (unsigned char *)&input[0] + width * 1 * i + j * 1;
               pSrcU = (unsigned char *)&input[0] + width * 1 * height + width * 1 * (i / 2) + (j / 2) * 2;
               pSrcV = pSrcU + 1;
               pDest = (LocalColor_Bgra *)((unsigned char *)&output[0] + width * 4 * i + j * 4);

#if 0
               double c = *pSrcY - 16;
               double d = *pSrcU - 128;
               double e = *pSrcV - 128;

               double r = (298 * c + 409 * e + 128) / 255;
               double g = (298 * c - 100 * d - 208 * e + 128) / 255;
               double b = (298 * c + 516 * d + 128) / 255;
               double a = 255;
#else
               double r = *pSrcY + 1.370705 * (*pSrcV - 128);
               double g = *pSrcY - 0.698001 * (*pSrcV - 128) - 0.337633 * (*pSrcU - 128);
               double b = *pSrcY + 1.732446 * (*pSrcU - 128);
               double a = 255;
#endif // if 0

               pDest->r = CLAMP(r, 0, 255);
               pDest->g = CLAMP(g, 0, 255);
               pDest->b = CLAMP(b, 0, 255);
               pDest->a = CLAMP(a, 0, 255);
           }
       }
   }

   return ret;
}
#endif // #if 0

bool ConvertFromUyvyToBgra(unsigned int width, unsigned height, const std::vector<unsigned char> &input, std::vector<unsigned char> &output)
{
    bool ret = true;
    unsigned int i, j;
    unsigned char *pSrcY, *pSrcU, *pSrcV;
    LocalColor_Bgra *pDest;

    output.clear();
    output.resize(width * height * 4);

    if (ret) {
        if ((width % 2) != 0 || (height % 2) != 0) {
            fprintf(stderr, "*** ERROR: both the width and height should be even.\n");
            ret = false;
        }
    }

    if (ret) {
        if (width * height * 2 > input.size()) {
            fprintf(stderr, "*** ERROR: the conent of the input file is too short.\n");
            ret = false;
        }
    }

    if (ret) {
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                pSrcY = (unsigned char *)&input[0] + width * 2 * i + j * 2 + 1;
                if ((j % 2) == 0) {
                    pSrcU = (unsigned char *)pSrcY - 1;
                    pSrcV = (unsigned char *)pSrcY + 1;
                } else {
                    pSrcU = (unsigned char *)pSrcY + 1;
                    pSrcV = (unsigned char *)pSrcY - 1;
                }
                pDest = (LocalColor_Bgra *)((unsigned char *)&output[0] + width * 4 * i + j * 4);

#if 0
                double c = *pSrcY - 16;
                double d = *pSrcU - 128;
                double e = *pSrcV - 128;

                double r = (298 * c + 409 * e + 128) / 255;
                double g = (298 * c - 100 * d - 208 * e + 128) / 255;
                double b = (298 * c + 516 * d + 128) / 255;
                double a = 255;
#else
                double r = *pSrcY + 1.370705 * (*pSrcV - 128);
                double g = *pSrcY - 0.698001 * (*pSrcV - 128) - 0.337633 * (*pSrcU - 128);
                double b = *pSrcY + 1.732446 * (*pSrcU - 128);
                double a = 255;
#endif // if 0

                pDest->r = CLAMP(r, 0, 255);
                pDest->g = CLAMP(g, 0, 255);
                pDest->b = CLAMP(b, 0, 255);
                pDest->a = CLAMP(a, 0, 255);
            }
        }
    }

    return ret;
}

bool ConvertFromBgraToUyvy(unsigned int width, unsigned height, const std::vector<unsigned char> &input, std::vector<unsigned char> &output)
{
    bool ret = true;
    unsigned long i, j;
    unsigned char *pDstPixelY, *pDstPixelU, *pDstPixelV;
    LocalColor_Bgra *pSrcPixel;

    output.clear();
    output.resize(width * height * 2);

    if (ret) {
        if ((width % 2) != 0 || (height % 2) != 0) {
            fprintf(stderr, "*** ERROR: both the width and height should be even.\n");
            ret = false;
        }
    }

    if (ret) {
        if (width * height * 2 > input.size()) {
            fprintf(stderr, "*** ERROR: the conent of the input file is too short.\n");
            ret = false;
        }
    }

    if (ret) {
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                pDstPixelY = (unsigned char *)((unsigned char *)&output[0] + width * 2 * i + j * 2 + 1);
                if ((j % 2) == 0) {
                    pDstPixelU = (unsigned char *)pDstPixelY - 1;
                    pDstPixelV = NULL;
                } else {
                    pDstPixelU = NULL;
                    pDstPixelV = (unsigned char *)pDstPixelY - 1;;
                }
                pSrcPixel = (LocalColor_Bgra *)((unsigned char *)&input[0] + width * 4 * i + j * 4);

                double lyuvY = 77 * pSrcPixel->r + 150 * pSrcPixel->g + 29 * pSrcPixel->b;
                double lyuvU = -43 * pSrcPixel->r - 84 * pSrcPixel->g + 127 * pSrcPixel->b;
                double lyuvV = 127 * pSrcPixel->r - 106 * pSrcPixel->g - 21 * pSrcPixel->b;

                lyuvY = (lyuvY + 128) / 256;
                lyuvU = (lyuvU + 128) / 256;
                lyuvV = (lyuvV + 128) / 256;

                lyuvY = lyuvY;
                lyuvU += 128;
                lyuvV += 128;

                unsigned char yuvY, yuvU, yuvV;
                yuvY = (unsigned char)CLAMP(lyuvY, 0, 255);
                yuvU = (unsigned char)CLAMP(lyuvU, 0, 255);
                yuvV = (unsigned char)CLAMP(lyuvV, 0, 255);

                *pDstPixelY = yuvY;
                if (pDstPixelU != NULL) {
                    *pDstPixelU = yuvU;
                }
                if (pDstPixelV != NULL) {
                    *pDstPixelV = yuvV;
                }
            }
        }
    }

    return ret;
}

bool IsBmpFileName(const std::string &sFileName)
{
    bool bRet = false;
    
    std::string::size_type nPos = sFileName.rfind(".");
    if (nPos != std::string::npos) {
        if (CompareNoCase(sFileName.substr(nPos), ".bmp") == 0) {
            bRet = true;
        }
    }
    
    return bRet;
}

int getFbVsi(const std::string &fb_file_path, struct fb_var_screeninfo &vsi)
{
    int nRet = 0;
    int nFbDevFd = -1;
    
    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));
    
    if (nRet == 0) {
        nFbDevFd = open(fb_file_path.c_str(), O_RDONLY, 0);
        if (nFbDevFd == -1) {
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        nRet = ioctl(nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            nRet = -1;
        }
    }
    
    if (nFbDevFd != -1) {
        close(nFbDevFd);
        nFbDevFd = -1;
    }
    
    return nRet;
}

int getCurrentFrameIndexFromFb(const std::string &fb_file_path, unsigned int &frame_index)
{
    int nRet = 0;
    int nFbDevFd = -1;
    struct fb_var_screeninfo vsi;
    
    frame_index = 0;
    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));
    
    if (nRet == 0) {
        nFbDevFd = open(fb_file_path.c_str(), O_RDONLY, 0);
        if (nFbDevFd == -1) {
            fprintf(stderr, "*** ERROR: failed to open the fb file: %s\n", fb_file_path.c_str());
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        nRet = ioctl(nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** ERROR: failed to get variable screen infomation.\n");
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        frame_index = vsi.yoffset / vsi.yres;
    }
    
    if (nFbDevFd != -1) {
        close(nFbDevFd);
        nFbDevFd = -1;
    }
    
    return nRet;
}

int getNextBgFrameIndexFromFb(const std::string &fb_file_path, unsigned int &frame_index)
{
    int nRet = 0;
    int nFbDevFd = -1;
    struct fb_var_screeninfo vsi;

    frame_index = 0;
    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));

    if (nRet == 0) {
        nFbDevFd = open(fb_file_path.c_str(), O_RDONLY, 0);
        if (nFbDevFd == -1) {
            fprintf(stderr, "*** ERROR: failed to open the fb file: %s\n", fb_file_path.c_str());
            nRet = -1;
        }
    }

    if (nRet == 0) {
        nRet = ioctl(nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** ERROR: failed to get variable screen infomation.\n");
            nRet = -1;
        }
    }

    if (nRet == 0) {
        __u32 t = (vsi.yoffset + vsi.yres) % vsi.yres_virtual;
        frame_index = t / vsi.yres;
    }

    if (nFbDevFd != -1) {
        close(nFbDevFd);
        nFbDevFd = -1;
    }

    return nRet;
}

int swapFbBuffers(const std::string &fb_file_path)
{
    int nRet = 0;
    int nFbDevFd = -1;
    struct fb_var_screeninfo vsi;

    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));

    if (nRet == 0) {
        nFbDevFd = open(fb_file_path.c_str(), O_RDWR, 0);
        if (nFbDevFd == -1) {
            fprintf(stderr, "*** ERROR: failed to open the fb file: %s\n", fb_file_path.c_str());
            nRet = -1;
        }
    }

    if (nRet == 0) {
        nRet = ioctl(nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** ERROR: failed to get variable screen infomation.\n");
            nRet = -1;
        }
    }

    if (nRet == 0) {
        __u32 t = (vsi.yoffset + vsi.yres) % vsi.yres_virtual;
        vsi.yoffset = t;
        nRet = ioctl(nFbDevFd, FBIOPAN_DISPLAY, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** ERROR: failed to swap fb buffers.\n");
            nRet = -1;
        }
    }

    if (nFbDevFd != -1) {
        close(nFbDevFd);
        nFbDevFd = -1;
    }

    return nRet;
}

bool GetInputFileContentFromStdin(std::vector<unsigned char> &vFileContent)
{
    bool bSuc = true;

    vFileContent.clear();

    if (bSuc) {
        if (!GetContentFromFile("-", vFileContent)) {
            bSuc = false;
        }
    }

    return bSuc;
}

bool PutOntputFileContentToStdout(QByteArray &vFileContent)
{
    bool bSuc = true;

    if (bSuc) {
        if (!SaveContentToFile("-", (const unsigned char*)vFileContent.constData(), (unsigned int)vFileContent.size())) {
            bSuc = false;
        }
    }

    return bSuc;
}

int ConvertPictureFileFormat(const std::string &sInputPictureFilePath, const std::string &sOutputPictureFilePath)
{
    int nRet = 0;
    QImage image;
    
    if (nRet == 0) {
        if (sInputPictureFilePath == "-") {
            if (!image.loadFromData(&g_vStdinInputFileContent[0], g_vStdinInputFileContent.size())) {
                fprintf(stderr, "*** ERROR: failed to load picture file from stdin.\n");
                nRet = -1;
            }
        } else {
            if (!image.load(sInputPictureFilePath.c_str())) {
                fprintf(stderr, "*** ERROR: failed to load picture file: %s\n", sInputPictureFilePath.c_str());
                nRet = -1;
            }
        }
    }
    
    if (nRet == 0) {
        if (!image.save(sOutputPictureFilePath.c_str())) {
            fprintf(stderr, "*** ERROR: failed to save to picture file: %s\n", sOutputPictureFilePath.c_str());
            nRet = -1;
        }
    }
    
    return nRet;
}

int GetWidthAndHeightFromPictureFile(const std::string &sInputPictureFilePath, unsigned long &nWidth, unsigned long &nHeight)
{
    int nRet = 0;
    QImage image;
    
    nWidth = 0;
    nHeight = 0;
    
    if (nRet == 0) {
        if (sInputPictureFilePath == "-") {
            if (!image.loadFromData(&g_vStdinInputFileContent[0], g_vStdinInputFileContent.size())) {
                fprintf(stderr, "*** ERROR: failed to load picture file from stdin.\n");
                nRet = -1;
            }
        } else {
            if (!image.load(sInputPictureFilePath.c_str())) {
                fprintf(stderr, "*** ERROR: failed to load picture file: %s\n", sInputPictureFilePath.c_str());
                nRet = -1;
            }
        }
    }
    
    if (nRet == 0) {
        nWidth = image.width();
        nHeight = image.height();
    }
    
    return nRet;
}

int GetContentFromPictureFile(const std::string &sInputPictureFilePath, std::vector<unsigned char> &vFileContent)
{
    int nRet = 0;
    QImage image;
    QImage imageBgra;
    
    vFileContent.clear();
    vFileContent.reserve(1024 * 1024);
        
    if (nRet == 0) {
        if (sInputPictureFilePath == "-") {
            if (!image.loadFromData(&g_vStdinInputFileContent[0], g_vStdinInputFileContent.size())) {
                fprintf(stderr, "*** ERROR: failed to load picture file from stdin.\n");
                nRet = -1;
            }
        } else {
            if (!image.load(sInputPictureFilePath.c_str())) {
                fprintf(stderr, "*** ERROR: failed to load picture file: %s\n", sInputPictureFilePath.c_str());
                nRet = -1;
            }
        }
    }
    
    if (nRet == 0) {
        imageBgra = image.convertToFormat(QImage::QImage::Format_ARGB32_Premultiplied);
        if (imageBgra.width() != image.width() || imageBgra.height() != image.height()) {
            fprintf(stderr, "*** ERROR: failed to convert picture file: %s\n", sInputPictureFilePath.c_str());
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        vFileContent.insert(vFileContent.end(), imageBgra.bits(), imageBgra.bits() + imageBgra.width() * imageBgra.height() * 4);
    }
    
    return nRet;
}

int GetContentFromSingleColor(unsigned long width, unsigned long height, const std::string &sColorStr, std::vector<unsigned char> &vFileContent)
{
    int nRet = 0;
    QColor color(sColorStr.c_str());
    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    
    vFileContent.clear();
    vFileContent.reserve(1024 * 1024);
    
    if (nRet == 0) {
        painter.fillRect(0, 0, width, height, color);
    }
    
    if (nRet == 0) {
        vFileContent.insert(vFileContent.end(), image.bits(), image.bits() + image.width() * image.height() * 4);
    }
    
    return nRet;
}

bool WriteOutputInfoFile(const std::string &outputInfoFile, const OutputInfo &outputInfo)
{
    bool bSuc = true;
    std::string sOutputInfoFileContent;

    if (bSuc) {
        sOutputInfoFileContent += FormatStr("imageFileInfo: \n");
        sOutputInfoFileContent += FormatStr("{\n");
        sOutputInfoFileContent += FormatStr("    fomat: \"%s\",\n", outputInfo.format.c_str());
        sOutputInfoFileContent += FormatStr("    x: %d,\n", outputInfo.x);
        sOutputInfoFileContent += FormatStr("    y: %d,\n", outputInfo.y);
        sOutputInfoFileContent += FormatStr("    width: %u,\n", outputInfo.width);
        sOutputInfoFileContent += FormatStr("    height: %u\n", outputInfo.height);
        sOutputInfoFileContent += FormatStr("}\n");
    }

    if (bSuc) {
        if (!SaveContentToFile(outputInfoFile, sOutputInfoFileContent)) {
            bSuc = false;
        }
    }

    return bSuc;
}

bool CalcOutputInfoPosition(const std::string &sOutputInfoPosition, OutputInfo &outputInfo)
{
    bool bSuc = true;
    std::vector<std::string> positionParts;

    if (bSuc) {
        positionParts = SplitString(sOutputInfoPosition, ",");
        if (positionParts.size() == 2) {
            outputInfo.x = StrToL(positionParts[0]);
            outputInfo.y = StrToL(positionParts[1]);
        } else if (positionParts.size() == 0) {
            outputInfo.x = 0;
            outputInfo.y = 0;
        } else {
            bSuc = false;
        }
    }

    return bSuc;
}

int main(int argc, char *argv[])
{
    BOOL suc;
    unsigned long width = 0;
    unsigned long height = 0;
    std::string input_format = "normal";
    std::string output_format = "normal";
    std::string input_file = "-";
    std::string output_file = "-";
    std::string output_info_file;
    std::string output_info_position;
    std::string clear_color = "";
    std::vector<unsigned char> vFileContent;
    unsigned int input_skipped_frame_count = 0;
    unsigned int output_skipped_frame_count = 0;
    bool from_current_foreground_frame_buffer = false;
    bool to_next_background_frame_buffer = false;
    int nRet;
    struct fb_var_screeninfo input_vsi;
    struct fb_var_screeninfo output_vsi;

    memset(&input_vsi, 0, sizeof(struct fb_var_screeninfo));
    memset(&output_vsi, 0, sizeof(struct fb_var_screeninfo));
    
    int opt;
    const char *short_options = "Hf:F:i:o:O:P:C:w:h:cns:S:";
    struct option long_options[] = {
        { "help", no_argument, NULL, 'H' },
        { "input_format",  required_argument, NULL, 'f' },
        { "output_format",  required_argument, NULL, 'F' },
        { "input_file",  required_argument, NULL, 'i' },
        { "output_file",  required_argument, NULL, 'o' },
        { "output_info_file",  required_argument, NULL, 'O' },
        { "output_info_position",  required_argument, NULL, 'P' },
        { "clear",  required_argument, NULL, 'C' },
        { "width",  required_argument, NULL, 'w' },
        { "height",  required_argument, NULL, 'h' },
        { "from_current_foreground_frame_buffer",  no_argument, NULL, 'c' },
        { "to_next_background_frame_buffer",  no_argument, NULL, 'n' },
        { "input_skipped_frame_count",  required_argument, NULL, 's' },
        { "output_skipped_frame_count",  required_argument, NULL, 'S' },
        { 0, 0, 0, 0 }
    };
    
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 'H':
                {
                    usage(argc, argv);
                    exit(0);
                }
                break;
            case 'f':
                {
                    input_format = std::string(optarg);
                }
                break;
            case 'F':
                {
                    output_format = std::string(optarg);
                }
                break;
            case 'i':
                {
                    input_file = std::string(optarg);
                }
                break;
            case 'o':
                {
                    output_file = std::string(optarg);
                }
                break;
            case 'O':
                {
                    output_info_file = std::string(optarg);
                }
                break;
            case 'P':
                {
                    output_info_position = std::string(optarg);
                }
                break;
            case 'C':
                {
                    clear_color = std::string(optarg);
                }
                break;
            case 'w':
                {
                    width = StrToUL(optarg);
                }
                break;
            case 'h':
                {
                    height = StrToUL(optarg);
                }
                break;
            case 'c':
                {
                    from_current_foreground_frame_buffer = true;
                }
                break;
            case 'n':
                {
                    to_next_background_frame_buffer = true;
                }
                break;
            case 's':
                {
                    input_skipped_frame_count = StrToUL(optarg);
                }
                break;
            case 'S':
                {
                    output_skipped_frame_count = StrToUL(optarg);
                }
                break;
            default:
                break;
        }
    }

    if (opt != -1) {
        usage(argc, argv);
        exit(0);
    }

    if (optind < argc) {
        input_file = std::string(argv[optind]);
        optind++;
    }

    if (optind < argc) {
        output_file = std::string(argv[optind]);
        optind++;
    }

    if (optind < argc) {
        output_info_file = std::string(argv[optind]);
        optind++;
    }
    
    if (input_file.empty()) {
        fprintf(stderr, "*** ERROR: input file can't be empty.\n");
        exit(-1);
    }
    
    if (!clear_color.empty()) {
        QColor color(clear_color.c_str());
        if (!color.isValid()) {
            fprintf(stderr, "*** Error: the color is not vailed: %s\n", clear_color.c_str());
            exit(-1);
        }
    }

    if (input_file == "-") {
        if (clear_color.empty()) {
            if (!GetInputFileContentFromStdin(g_vStdinInputFileContent)) {              // *** NOTE: the file from stdin can only be read one time!
                fprintf(stderr, "*** Error: failed to read the input picture from stdin.\n");
                exit(-1);
            }
        }
    }
    
    if (output_file.empty()) {
        fprintf(stderr, "*** ERROR: output file can't be empty.\n");
        exit(-1);
    }

    if (output_file == "-") {
        if (output_format == "normal") {
            output_file = "-.png";
        }
    }
    
    if (getFbVsi(input_file, input_vsi) == 0) {
        if (input_vsi.xres_virtual != 0 && input_vsi.yres_virtual != 0) {
            if (from_current_foreground_frame_buffer) {
                width = width != 0 ? width : input_vsi.xres;
                height = height != 0 ? height : input_vsi.yres;
            } else {
                width = input_vsi.xres_virtual;
                height = height != 0 ? height : input_vsi.yres_virtual;
            }
        }
    } else if (getFbVsi(output_file, output_vsi) == 0) {
        if (output_vsi.xres_virtual != 0 && output_vsi.yres_virtual != 0) {
            if (to_next_background_frame_buffer) {
                width = width != 0 ? width : output_vsi.xres;
                height = height != 0 ? height : output_vsi.yres;
            } else {
                width = width != 0 ? width : output_vsi.xres_virtual;
                height = height != 0 ? height : output_vsi.yres_virtual;
            }
        }
    }
    
    if (clear_color.empty()) {
        if (input_format == "normal") {
            nRet = GetWidthAndHeightFromPictureFile(input_file, width, height);
            if (nRet != 0) {
                fprintf(stderr, "*** ERROR: GetWidthAndHeightFromPictureFile() is failed.\n");
                exit(-1);
            }
        }
    }
    
    if (from_current_foreground_frame_buffer) {
        nRet = getCurrentFrameIndexFromFb(input_file, input_skipped_frame_count);
        if (nRet != 0) {
            fprintf(stderr, "*** ERROR: Frame 0 is the current frame.\n");
        } else {
            fprintf(stderr, "Frame %d is the current frame.\n", input_skipped_frame_count);
        }
    }

    width = width != 0 ? width : 1280;
    height = height != 0 ? height : 720;

    fprintf(stderr, "width=%lu\n", width);
    fprintf(stderr, "height=%lu\n", height);

    if (width == 0) {
        fprintf(stderr, "*** ERROR: width == 0.\n");
        exit(-1);
    }

    if (height == 0) {
        fprintf(stderr, "*** ERROR: height == 0.\n");
        exit(-1);
    }

    if (!FileExists(input_file)) {
        fprintf(stderr, "*** ERROR: the file does not exist: %s\n", input_file.c_str());
        exit(-1);
    }

    if (!clear_color.empty()) {
        if (GetContentFromSingleColor(width, height, clear_color, vFileContent) != 0) {
            fprintf(stderr, "*** ERROR: failed to read the file: %s\n", input_file.c_str());
            exit(-1);
        }
    } else {
        if (input_format == "normal") {
            if (GetContentFromPictureFile(input_file, vFileContent) != 0) {
                fprintf(stderr, "*** ERROR: failed to read the file: %s.\n", input_file.c_str());
                exit(-1);
            }
        } else {
            if (input_file == "-") {
                vFileContent = g_vStdinInputFileContent;
            } else {
                if (!TK_Tools::GetContentFromFile(input_file, vFileContent)) {
                    fprintf(stderr, "*** ERROR: failed to read the file: %s.\n", input_file.c_str());
                    exit(-1);
                }
            }
        }
    }

    if (input_format == "raw_uyvy") {
        std::vector<unsigned char> output;
        if (input_vsi.xres_virtual != 0 && input_vsi.yres_virtual != 0) {
            if (!ConvertFromUyvyToBgra(input_vsi.xres_virtual, input_vsi.yres_virtual, vFileContent, output)) {
                exit(-1);
            }
        } else {
            if (!ConvertFromUyvyToBgra(width, height, vFileContent, output)) {
                exit(-1);
            }
        }
        if (width * 2 * height * (input_skipped_frame_count + 1) > vFileContent.size()) {
            fprintf(stderr, "*** ERROR: the conent of the file is too short (%lu > %lu): %s\n", (unsigned long)(width * 2 * height * (input_skipped_frame_count + 1)), (unsigned long)vFileContent.size(), input_file.c_str());
            exit(-1);
        }
        vFileContent = output;
    } else if (input_format == "raw_bgra") {
        if (width * 4 * height * (input_skipped_frame_count + 1) > vFileContent.size()) {
            fprintf(stderr, "*** ERROR: the conent of the file is too short (%lu > %lu): %s\n", (unsigned long)(width * 4 * height * (input_skipped_frame_count + 1)), (unsigned long)vFileContent.size(), input_file.c_str());
            exit(-1);
        }
    } else if (input_format == "normal") {
        // do nothing
    } else {
        fprintf(stderr, "*** ERROR: the input format is not supported: %s\n", input_format.c_str());
        exit(-1);
    }

    if (to_next_background_frame_buffer) {
        if (getFbVsi(output_file, output_vsi) != 0) {
            fprintf(stderr, "*** ERROR: the file is not a fb device: %s\n", output_file.c_str());
            exit(-1);
        }

        if (getNextBgFrameIndexFromFb(output_file, output_skipped_frame_count) != 0) {
            fprintf(stderr, "*** ERROR: failed to get the next frame index of fb device: %s\n", output_file.c_str());
            exit(-1);
        }
    }

    fprintf(stderr, "input_skipped_frame_count=%u\n", input_skipped_frame_count);
    fprintf(stderr, "output_skipped_frame_count=%u\n", output_skipped_frame_count);

    if (output_format == "raw_bgra") {
        if (!SaveContentToFile(output_file, vFileContent, output_skipped_frame_count)) {
            fprintf(stderr, "*** ERROR: failed to write the file: %s\n", output_file.c_str());
            exit(-1);
        }

        if (to_next_background_frame_buffer) {
            if (swapFbBuffers(output_file) != 0) {
                exit(-1);
            }
        }
    } else if (output_format == "raw_uyvy") {
        std::vector<unsigned char> vFileContent2;
        if (!ConvertFromBgraToUyvy(width, height, vFileContent, vFileContent2)) {
            fprintf(stderr, "*** ERROR: failed to convert to raw_uyvy.\n");
            exit(-1);
        }

        if (!SaveContentToFile(output_file, vFileContent2, output_skipped_frame_count)) {
            fprintf(stderr, "*** ERROR: failed to write the file: %s\n", output_file.c_str());
            exit(-1);
        }

        if (to_next_background_frame_buffer) {
            if (swapFbBuffers(output_file) != 0) {
                exit(-1);
            }
        }
    } else if (output_format == "normal") {
        if (IsBmpFileName(output_file)) {
            suc = TK_Tools::SaveBmpToFile_RGBA32(
                    output_file.c_str(),
                    (const void *)(&vFileContent[0] + width * height * 4 * input_skipped_frame_count),
                    width,
                    height,
                    (width * 4),
                    FALSE,
                    "bgra"
                  );
            if (!suc) {
                fprintf(stderr, "*** ERROR: failed to write the file: %s\n", output_file.c_str());
                exit(-1);
            }
        } else {
            QImage image((const uchar *)(&vFileContent[0] + width * height * 4 * input_skipped_frame_count), width, height, (width * 4), QImage::QImage::Format_ARGB32_Premultiplied);
            if (output_file.length() >=2 && output_file[0] == '-' && output_file[1] == '.') {
                QByteArray ba;
                QBuffer buffer(&ba);
                buffer.open(QIODevice::WriteOnly);
                if (!image.save(&buffer, output_file.substr(2).c_str())) {
                    fprintf(stderr, "*** Error: failed to save to the picture file to byte array.\n");
                    exit(-1);
                } else {
                    if (!PutOntputFileContentToStdout(ba)) {
                        fprintf(stderr, "*** Error: failed to save to the picture file to stdout.\n");
                        exit(-1);
                    }
                }
            } else {
                suc = image.save(output_file.c_str());
                if (!suc) {
                    fprintf(stderr, "*** ERROR: failed to write the file: %s\n", output_file.c_str());
                    exit(-1);
                }
            }
        }
    } else {
        fprintf(stderr, "*** ERROR: the output format is not supported: %s\n", output_format.c_str());
        exit(-1);
    }

    if (!output_info_file.empty()) {
        OutputInfo outputInfo;
        outputInfo.format = output_format;
        outputInfo.width = width;
        outputInfo.height = height;

        if (!CalcOutputInfoPosition(output_info_position, outputInfo)) {
            fprintf(stderr, "*** ERROR: failed to get output information position: %s\n", output_info_position.c_str());
            exit(-1);
        }

        if (!WriteOutputInfoFile(output_info_file, outputInfo)) {
            fprintf(stderr, "*** ERROR: failed to write the file: %s\n", output_info_file.c_str());
            exit(-1);
        }
    }

    return 0;
}
