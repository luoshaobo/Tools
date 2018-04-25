#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/mxcfb.h>
#include <linux/ipu.h>
#include <sys/mman.h>
#include "TK_Tools.h"

using namespace TK_Tools;

#define fourcc(a, b, c, d)\
     (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

#define IPU_PIX_FMT_RGB332  fourcc('R', 'G', 'B', '1')  /*!<  8  RGB-3-3-2    */
#define IPU_PIX_FMT_RGB555  fourcc('R', 'G', 'B', 'O')  /*!< 16  RGB-5-5-5    */
#define IPU_PIX_FMT_RGB565  fourcc('R', 'G', 'B', 'P')  /*!< 16  RGB-5-6-5   */
#define IPU_PIX_FMT_BGRA4444 fourcc('4', '4', '4', '4') /*!< 16  RGBA-4-4-4-4 */
#define IPU_PIX_FMT_BGRA5551 fourcc('5', '5', '5', '1') /*!< 16  RGBA-5-5-5-1 */
#define IPU_PIX_FMT_RGB666  fourcc('R', 'G', 'B', '6')  /*!< 18  RGB-6-6-6    */
#define IPU_PIX_FMT_BGR666  fourcc('B', 'G', 'R', '6')  /*!< 18  BGR-6-6-6    */
#define IPU_PIX_FMT_BGR24   fourcc('B', 'G', 'R', '3')  /*!< 24  BGR-8-8-8    */
#define IPU_PIX_FMT_RGB24   fourcc('R', 'G', 'B', '3')  /*!< 24  RGB-8-8-8    */
#define IPU_PIX_FMT_GBR24   fourcc('G', 'B', 'R', '3')  /*!< 24  GBR-8-8-8    */
#define IPU_PIX_FMT_BGR32   fourcc('B', 'G', 'R', '4')  /*!< 32  BGR-8-8-8-8  */
#define IPU_PIX_FMT_BGRA32  fourcc('B', 'G', 'R', 'A')  /*!< 32  BGR-8-8-8-8  */
#define IPU_PIX_FMT_RGB32   fourcc('R', 'G', 'B', '4')  /*!< 32  RGB-8-8-8-8  */
#define IPU_PIX_FMT_RGBA32  fourcc('R', 'G', 'B', 'A')  /*!< 32  RGB-8-8-8-8  */
#define IPU_PIX_FMT_ABGR32  fourcc('A', 'B', 'G', 'R')  /*!< 32  ABGR-8-8-8-8 */
#define IPU_PIX_FMT_TEST    fourcc('B', 'G', 'R', 'A')  /*!< 32  BGR-8-8-8-8  */

#define IPU_PIX_FMT_YUYV    fourcc('Y', 'U', 'Y', 'V')  /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_UYVY    fourcc('U', 'Y', 'V', 'Y')  /*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_YVYU    fourcc('Y', 'V', 'Y', 'U')  /*!< 16 YVYU 4:2:2 */
#define IPU_PIX_FMT_VYUY    fourcc('V', 'Y', 'U', 'Y')  /*!< 16 VYYU 4:2:2 */
#define IPU_PIX_FMT_Y41P    fourcc('Y', '4', '1', 'P')  /*!< 12 YUV 4:1:1 */
#define IPU_PIX_FMT_YUV444  fourcc('Y', '4', '4', '4')  /*!< 24 YUV 4:4:4 */
#define IPU_PIX_FMT_VYU444  fourcc('V', '4', '4', '4')  /*!< 24 VYU 4:4:4 */
/* two planes -- one Y, one Cb + Cr interleaved  */
#define IPU_PIX_FMT_NV12    fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0  */
/* two planes -- 12  tiled Y/CbCr 4:2:0  */
#define IPU_PIX_FMT_TILED_NV12    fourcc('T', 'N', 'V', 'P')
#define IPU_PIX_FMT_TILED_NV12F   fourcc('T', 'N', 'V', 'F')

#define IPU_PIX_FMT_GREY    fourcc('G', 'R', 'E', 'Y')  /*!< 8  Greyscale */
#define IPU_PIX_FMT_YVU410P fourcc('Y', 'V', 'U', '9')  /*!< 9  YVU 4:1:0 */
#define IPU_PIX_FMT_YUV410P fourcc('Y', 'U', 'V', '9')  /*!< 9  YUV 4:1:0 */
#define IPU_PIX_FMT_YVU420P fourcc('Y', 'V', '1', '2')  /*!< 12 YVU 4:2:0 */
#define IPU_PIX_FMT_YUV420P fourcc('I', '4', '2', '0')  /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV420P2 fourcc('Y', 'U', '1', '2') /*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YVU422P fourcc('Y', 'V', '1', '6')  /*!< 16 YVU 4:2:2 */
#define IPU_PIX_FMT_YUV422P fourcc('4', '2', '2', 'P')  /*!< 16 YUV 4:2:2 */

struct PixelFormatInfo {
    __u32 pixel_fmt;
    const char *pixel_fmt_str;
} g_pixelFormatInfoTable[] = {
    { IPU_PIX_FMT_RGB332, "8  RGB-3-3-2" },
    { IPU_PIX_FMT_RGB555, "16  RGB-5-5-5" },
    { IPU_PIX_FMT_RGB565, "16  RGB-5-6-5" },
    { IPU_PIX_FMT_BGRA4444, "16  RGBA-4-4-4-4" },
    { IPU_PIX_FMT_BGRA5551, "16  RGBA-5-5-5-1" },
    { IPU_PIX_FMT_RGB666, "18  RGB-6-6-6" },
    { IPU_PIX_FMT_BGR666, "18  BGR-6-6-6" },
    { IPU_PIX_FMT_BGR24, "24  BGR-8-8-8" },
    { IPU_PIX_FMT_RGB24, "24  RGB-8-8-8" },
    { IPU_PIX_FMT_GBR24, "24  GBR-8-8-8" },
    { IPU_PIX_FMT_BGR32, "32  BGR-8-8-8-8" },
    { IPU_PIX_FMT_BGRA32, "32  BGR-8-8-8-8" },
    { IPU_PIX_FMT_RGB32, "32  RGB-8-8-8-8" },
    { IPU_PIX_FMT_RGBA32, "32  RGB-8-8-8-8" },
    { IPU_PIX_FMT_ABGR32, "32  ABGR-8-8-8-8" },
    { IPU_PIX_FMT_TEST, "32  BGR-8-8-8-8" },

    { IPU_PIX_FMT_YUYV, "16 YUV 4:2:2" },
    { IPU_PIX_FMT_UYVY, "16 YUV 4:2:2" },
    { IPU_PIX_FMT_YVYU, "16 YVYU 4:2:2" },
    { IPU_PIX_FMT_VYUY, "16 VYYU 4:2:2" },
    { IPU_PIX_FMT_Y41P, "12 YUV 4:1:1" },
    { IPU_PIX_FMT_YUV444, "24 YUV 4:4:4" },
    { IPU_PIX_FMT_VYU444, "24 VYU 4:4:4" },
    { IPU_PIX_FMT_NV12, "12  Y/CbCr 4:2:0" },
    { IPU_PIX_FMT_TILED_NV12, "12  tiled Y/CbCr 4:2:0" },
    { IPU_PIX_FMT_TILED_NV12F, "12  tiled Y/CbCr 4:2:0" },
    { IPU_PIX_FMT_GREY, "8  Greyscale" },
    { IPU_PIX_FMT_YVU410P, "9  YVU 4:1:0" },
    { IPU_PIX_FMT_YUV410P, "9  YUV 4:1:0" },
    { IPU_PIX_FMT_YVU420P, "12 YVU 4:2:0" },
    { IPU_PIX_FMT_YUV420P, "12 YUV 4:2:0" },
    { IPU_PIX_FMT_YUV420P2, "12 YUV 4:2:0" },
    { IPU_PIX_FMT_YVU422P, "16 YVU 4:2:2" },
    { IPU_PIX_FMT_YUV422P, "16 YUV 4:2:2" },
};

struct {
    unsigned long nColorFormat;
    unsigned int bitCount;
    struct fb_bitfield redBitField;
    struct fb_bitfield greenBitField;
    struct fb_bitfield blueBitField;
    struct fb_bitfield transpBitField;
} arrColorFormats[] = {
    /*     pixel            bpp    red         green        blue      transp */
    { IPU_PIX_FMT_RGB565,   16, {11, 5, 0}, { 5, 6, 0}, { 0, 5, 0}, {  0, 0, 0} },
    { IPU_PIX_FMT_BGRA4444, 16, { 8, 4, 0}, { 4, 4, 0}, { 0, 4, 0}, { 12, 4, 0} },
    { IPU_PIX_FMT_BGRA5551, 16, {10, 5, 0}, { 5, 5, 0}, { 0, 5, 0}, { 15, 1, 0} },
    { IPU_PIX_FMT_RGB24,    24, { 0, 8, 0}, { 8, 8, 0}, {16, 8, 0}, { 0, 0, 0} },
    { IPU_PIX_FMT_BGR24,    24, {16, 8, 0}, { 8, 8, 0}, { 0, 8, 0}, { 0, 0, 0} },
    { IPU_PIX_FMT_RGB32,    32, { 0, 8, 0}, { 8, 8, 0}, {16, 8, 0}, {24, 8, 0} },
    { IPU_PIX_FMT_BGR32,    32, {16, 8, 0}, { 8, 8, 0}, { 0, 8, 0}, {24, 8, 0} },
    { IPU_PIX_FMT_ABGR32,   32, {24, 8, 0}, {16, 8, 0}, { 8, 8, 0}, { 0, 8, 0} },
    { IPU_PIX_FMT_BGRA32,   32, {16, 8, 0}, { 8, 8, 0}, { 0, 8, 0}, {24, 8, 0} },
    { IPU_PIX_FMT_TEST,     32, {0, 0, 0}, { 0, 0, 0}, { 0, 0, 0}, {0, 0, 0} },
};

int g_nFbDevFd = -1;

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
    printf("    %s <fb_dev> set local_alpha <local_alpha_onoff>\n", basename(argv[0]));
    printf("    %s <fb_dev> set global_alpha <global_alpha_onoff> [<global_alpha_value>]\n", basename(argv[0]));
    printf("    %s <fb_dev> set color_key <color_key_onoff> [<color_value_32>]\n", basename(argv[0]));
    printf("    %s <fb_dev> set rect <x> <y> <width> <height>\n", basename(argv[0]));
    printf("    %s <fb_dev> set res <x_resolution> <y_resolution>\n", basename(argv[0]));
    printf("    %s <fb_dev> set vres <virtual_x_resolution> <virtual_y_resolution>\n", basename(argv[0]));
    printf("    %s <fb_dev> set offset <x_offset> <y_offset>\n", basename(argv[0]));
    printf("    %s <fb_dev> set blank <blank_on_off>\n", basename(argv[0]));
    printf("    %s <fb_dev> set color_space <four_cc>\n", basename(argv[0]));
    printf("    %s <fb_dev> set standalone_local_alpha_test <local_alpha_onoff> [<aplha_value_1>] [<aplha_value_2>]\n", basename(argv[0]));
    printf("    %s <fb_dev> get fb_info\n", basename(argv[0]));
    printf("    %s <fb_dev> swap buffers\n", basename(argv[0]));
}

std::string getPixelFormatStr(__u32 pixel_format)
{
    std::string sRet = "unknown";

    if (pixel_format == 0) {
        sRet = "std color";
    } else {
        for (unsigned int i = 0; i < TK_ARR_LEN(g_pixelFormatInfoTable); i++) {
            if (pixel_format == g_pixelFormatInfoTable[i].pixel_fmt) {
                sRet = g_pixelFormatInfoTable[i].pixel_fmt_str;
            }
        }
    }

    return sRet;
}

std::string getActivateFlagsStr(__u32 activate_flags)
{
    std::string sRet;

    __u32 activate_cmd = (activate_flags & FB_ACTIVATE_MASK);
    if (activate_cmd == FB_ACTIVATE_NOW) {
        if (!sRet.empty()) {
            sRet += "|";
        }
        sRet += "FB_ACTIVATE_NOW";
    } else if (activate_cmd == FB_ACTIVATE_NXTOPEN) {
        if (!sRet.empty()) {
            sRet += "|";
        }
        sRet += "FB_ACTIVATE_NXTOPEN";
    } else if (activate_cmd == FB_ACTIVATE_TEST) {
        if (!sRet.empty()) {
            sRet += "|";
        }
        sRet += "FB_ACTIVATE_TEST";
    }

    if (activate_flags & FB_ACTIVATE_VBL) {
        if (!sRet.empty()) {
            sRet += "|";
        }
        sRet += "FB_ACTIVATE_VBL";
    }
    if (activate_flags & FB_CHANGE_CMAP_VBL) {
        if (!sRet.empty()) {
            sRet += "|";
        }
        sRet += "FB_CHANGE_CMAP_VBL";
    }
    if (activate_flags & FB_ACTIVATE_ALL) {
        if (!sRet.empty()) {
            sRet += "|";
        }
        sRet += "FB_ACTIVATE_ALL";
    }
    if (activate_flags & FB_ACTIVATE_FORCE) {
        if (!sRet.empty()) {
            sRet += "|";
        }
        sRet += "FB_ACTIVATE_FORCE";
    }
    if (activate_flags & FB_ACTIVATE_INV_MODE) {
        if (!sRet.empty()) {
            sRet += "|";
        }
        sRet += "FB_ACTIVATE_INV_MODE";
    }

    if (sRet.empty()) {
        sRet = "none";
    }

    return sRet;
}

void output_fix_screeninfo(struct fb_fix_screeninfo *fsi)
{
    printf("fb_fix_screeninfo: \n");
    printf("    id: %s\n", std::string(fsi->id, 16).c_str());
    printf("    smem_start: 0x%08X\n", (unsigned int)fsi->smem_start);
    printf("    smem_len: %u\n", fsi->smem_len);
    printf("    type: %u\n", fsi->type);
    printf("    type_aux: %u\n", fsi->type_aux);
    printf("    visual: %u\n", fsi->visual);
    printf("    xpanstep: %u\n", fsi->xpanstep);
    printf("    ypanstep: %u\n", fsi->ypanstep);
    printf("    ywrapstep: %u\n", fsi->ywrapstep);
    printf("    line_length: %u\n", fsi->line_length);
    printf("    mmio_start: 0x%08X\n", (unsigned int)fsi->mmio_start);
    printf("    mmio_len: %u\n", fsi->mmio_len);
    printf("    accel: %u\n", fsi->accel);
    printf("    capabilities: %u\n", fsi->capabilities);
    printf("    reserved: [0x%08X, 0x%08X]\n", fsi->reserved[0], fsi->reserved[1]);
}

void output_var_screeninfo(struct fb_var_screeninfo *vsi)
{
    printf("fb_var_screeninfo: \n");
    printf("    xres: %u\n", vsi->xres);
    printf("    yres: %u\n", vsi->yres);
    printf("    xres_virtual: %u\n", vsi->xres_virtual);
    printf("    yres_virtual: %u\n", vsi->yres_virtual);
    printf("    xoffset: %u\n", vsi->xoffset);
    printf("    yoffset: %u\n", vsi->yoffset);
    printf("    bits_per_pixel: %u\n", vsi->bits_per_pixel);
    printf("    grayscale: %u\n", vsi->grayscale);
    printf("    red: {offset:%u; length:%u; right:%u} \n", vsi->red.offset, vsi->red.length, vsi->red.msb_right);
    printf("    green: {offset:%u; length:%u; right:%u} \n", vsi->green.offset, vsi->green.length, vsi->green.msb_right);
    printf("    blue: {offset:%u; length:%u; right:%u} \n", vsi->blue.offset, vsi->blue.length, vsi->blue.msb_right);
    printf("    transp: {offset:%u; length:%u; right:%u} \n", vsi->transp.offset, vsi->transp.length, vsi->transp.msb_right);
    printf("    nonstd: %u(%s)\n", vsi->nonstd, getPixelFormatStr(vsi->nonstd).c_str());
    printf("    activate: %u(%s)\n", vsi->activate, getActivateFlagsStr(vsi->activate).c_str());
    printf("    height: %u\n", vsi->height);
    printf("    width: %u\n", vsi->width);
    printf("    accel_flags: 0x%08X\n", vsi->accel_flags);
    printf("    pixclock: %u\n", vsi->pixclock);
    printf("    left_margin: %u\n", vsi->left_margin);
    printf("    right_margin: %u\n", vsi->right_margin);
    printf("    upper_margin: %u\n", vsi->upper_margin);
    printf("    lower_margin: %u\n", vsi->lower_margin);
    printf("    hsync_len: %u\n", vsi->hsync_len);
    printf("    vsync_len: %u\n", vsi->vsync_len);
    printf("    sync: %u\n", vsi->sync);
    printf("    vmode: 0x%08X\n", (unsigned int)vsi->vmode);
    printf("    rotate: %u\n", vsi->rotate);
    printf("    colorspace: %u\n", vsi->colorspace);
    printf("    reserved: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", vsi->reserved[0], vsi->reserved[1], vsi->reserved[2], vsi->reserved[3]);
}

void set_local_alpha_pixels(unsigned int width, unsigned int height, unsigned char *pPixels, unsigned char alpha)
{
    unsigned i, j;
    unsigned char *pPixel;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            pPixel = pPixels + width * 1 * i + j;
            *pPixel = alpha;
        }
    }
}

void get_color_system_bit_fileds(unsigned long nColorFormat,
                                 struct fb_bitfield *redBitField,
                                 struct fb_bitfield *greenBitField,
                                 struct fb_bitfield *blueBitField,
                                 struct fb_bitfield *transpBitField)
{
    memset(redBitField, 0, sizeof(struct fb_bitfield));
    memset(greenBitField, 0, sizeof(struct fb_bitfield));
    memset(blueBitField, 0, sizeof(struct fb_bitfield));
    memset(transpBitField, 0, sizeof(struct fb_bitfield));

    for (unsigned int i = 0; i < sizeof(arrColorFormats) / sizeof(arrColorFormats[0]); i++) {
        if (arrColorFormats[i].nColorFormat == nColorFormat) {
            memcpy(redBitField, &arrColorFormats[i].redBitField, sizeof(struct fb_bitfield));
            memcpy(greenBitField, &arrColorFormats[i].greenBitField, sizeof(struct fb_bitfield));
            memcpy(blueBitField, &arrColorFormats[i].blueBitField, sizeof(struct fb_bitfield));
            memcpy(transpBitField, &arrColorFormats[i].transpBitField, sizeof(struct fb_bitfield));
        }
    }
}

int handle_cmd_set_standalone_local_alpha_test(int argc, char *argv[])
{
    int nRet = 0;
    std::string sOnOff;
    std::string sAlphaValue = "255";
    unsigned char nAlphaValue;
    std::string sAlphaValue2 = "255";
    unsigned char nAlphaValue2;
    unsigned int nScreenWidth = 0;
    unsigned int nScreenHeight = 0;
    struct fb_var_screeninfo vsi;
    struct mxcfb_loc_alpha la;
    unsigned char *pAlphaAddr0 = NULL;
    unsigned char *pAlphaAddr1 = NULL;

    if (argc < 5) {
        usage(argc, argv);
        exit(1);
    }

    sOnOff = argv[4];
    if (sOnOff != "0" && sOnOff != "1") {
        usage(argc, argv);
        exit(1);
    }

    if (sOnOff == "1") {
        if (argc < 6) {
            usage(argc, argv);
            exit(1);
        } else {
            sAlphaValue = argv[5];
        }
    }
    nAlphaValue = StrToUL(sAlphaValue);
    fprintf(stdout, "alpha value: %d\n", nAlphaValue);

    if (argc >= 7) {
        sAlphaValue2 = argv[6];
    }
    nAlphaValue2 = StrToUL(sAlphaValue2);
    fprintf(stdout, "alpha value 2: %d\n", nAlphaValue2);

    if (nRet == 0) {
        int nPrefetch = 0;
        nRet = ioctl(g_nFbDevFd, MXCFB_SET_PREFETCH, &nPrefetch);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set prefetch to 0\n");
        }
    }

    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));
    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to get variable screen infomation\n");
        } else {
            fprintf(stdout, "To get fb_var_screeninfo\n");
            output_var_screeninfo(&vsi);
            nScreenWidth = vsi.xres;
            nScreenHeight = vsi.yres;
            fprintf(stdout, "nScreenWidth=%u\n", nScreenWidth);
            fprintf(stdout, "nScreenHeight=%u\n", nScreenHeight);
        }
    }
    if (nRet == 0) {
        unsigned long nColorFormat = IPU_PIX_FMT_ABGR32;

        vsi.bits_per_pixel = 32;
        vsi.grayscale = 0;
        get_color_system_bit_fileds(nColorFormat, &vsi.red, &vsi.green, &vsi.blue, &vsi.transp);
        vsi.nonstd = nColorFormat;
        vsi.activate = FB_ACTIVATE_FORCE|FB_ACTIVATE_ALL;
        nRet = ioctl(g_nFbDevFd, FBIOPUT_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set variable screen infomation\n");
        }
        fprintf(stdout, "After setting fb_var_screeninfo\n");
        output_var_screeninfo(&vsi);
    }
    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to get variable screen infomation\n");
        } else {
            fprintf(stdout, "To get fb_var_screeninfo again\n");
            output_var_screeninfo(&vsi);
        }
    }

    memset(&la, 0, sizeof(mxcfb_loc_alpha));
    if (nRet == 0) {
        la.enable = StrToL(sOnOff);
        la.alpha_in_pixel = 0;
        la.alpha_phy_addr0 = 0;
        la.alpha_phy_addr1 = 0;
        nRet = ioctl(g_nFbDevFd, MXCFB_SET_LOC_ALPHA, &la);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set local alpha\n");
        }
    }

    if (nRet == 0) {
        if (StrToL(sOnOff)) {
            for (int i = 0; i < 100; i++)
            {
                unsigned char nCurAlpha = ((i % 2) == 0) ? nAlphaValue : nAlphaValue2;

                fprintf(stdout, "la.alpha_phy_addr0=0x%08X\n", (unsigned int)la.alpha_phy_addr0);
                pAlphaAddr0 = (unsigned char*)mmap(0, nScreenWidth * nScreenHeight, PROT_READ | PROT_WRITE, MAP_SHARED, g_nFbDevFd, la.alpha_phy_addr0);
                if (pAlphaAddr0 != MAP_FAILED) {
                    fprintf(stdout, "pAlphaAddr0=0x%08X\n", (unsigned int)pAlphaAddr0);
                    set_local_alpha_pixels(nScreenWidth, nScreenHeight, pAlphaAddr0, nCurAlpha);
                    msync(pAlphaAddr0, nScreenWidth * nScreenHeight, MS_SYNC);
                    munmap(pAlphaAddr0, nScreenWidth * nScreenHeight);
                    nRet = ioctl(g_nFbDevFd, MXCFB_SET_LOC_ALP_BUF, &la.alpha_phy_addr0);
                    if (nRet < 0) {
                        fprintf(stderr, "*** Error: failed to set local alpha buf 0: nRet=%d\n", nRet);
                    } else {
                        nRet = 0;
                    }
                } else {
                    fprintf(stderr, "*** Error: failed to set mmap local alpha buf 1: errno=%d: %s\n", errno, strerror(errno));
                }

                fprintf(stdout, "la.alpha_phy_addr1=0x%08X\n", (unsigned int)la.alpha_phy_addr1);
                pAlphaAddr1 = (unsigned char*)mmap(0, nScreenWidth * nScreenHeight, PROT_READ | PROT_WRITE, MAP_SHARED, g_nFbDevFd, la.alpha_phy_addr1);
                if (pAlphaAddr1 != MAP_FAILED) {
                    fprintf(stdout, "pAlphaAddr1=0x%08X\n", (unsigned int)pAlphaAddr1);
                    set_local_alpha_pixels(nScreenWidth, nScreenHeight, pAlphaAddr1, nCurAlpha);
                    msync(pAlphaAddr0, nScreenWidth * nScreenHeight, MS_SYNC);
                    munmap(pAlphaAddr1, nScreenWidth * nScreenHeight);
                    nRet = ioctl(g_nFbDevFd, MXCFB_SET_LOC_ALP_BUF, &la.alpha_phy_addr1);
                    if (nRet < 0) {
                        fprintf(stderr, "*** Error: failed to set local alpha buf 1: nRet=%d\n", nRet);
                    } else {
                        nRet = 0;
                    }
                } else {
                    fprintf(stderr, "*** Error: failed to set mmap local alpha buf 1: errno=%d: %s\n", errno, strerror(errno));
                }

                if (nRet != 0) {
                    break;
                }

                if (argc <= 6) {
                    break;
                }

                sleep(3);
            };
        }
    }

    return nRet;
}

int handle_cmd_set_local_alpha(int argc, char *argv[])
{
    int nRet = 0;
    std::string sOnOff;
    struct mxcfb_loc_alpha la;

    memset(&la, 0, sizeof(mxcfb_loc_alpha));

    if (argc < 5) {
        usage(argc, argv);
        exit(1);
    }

    sOnOff = argv[4];
    if (sOnOff != "0" && sOnOff != "1") {
        usage(argc, argv);
        exit(1);
    }

    la.enable = StrToL(sOnOff);
    la.alpha_in_pixel = 1;
    la.alpha_phy_addr0 = 0;
    la.alpha_phy_addr1 = 0;
    nRet = ioctl(g_nFbDevFd, MXCFB_SET_LOC_ALPHA, &la);
    if (nRet != 0) {
        fprintf(stderr, "*** Error: failed to set local alpha\n");
    }

    return nRet;
}

int handle_cmd_set_global_alpha(int argc, char *argv[])
{
    int nRet = 0;
    std::string sOnOff;
    std::string sAlpha = "0";
    struct mxcfb_gbl_alpha ga;

    memset(&ga, 0, sizeof(mxcfb_gbl_alpha));

    if (argc < 5) {
        usage(argc, argv);
        exit(1);
    }

    sOnOff = argv[4];
    if (sOnOff != "0" && sOnOff != "1") {
        usage(argc, argv);
        exit(1);
    }

    if (sOnOff == "1") {
        if (argc < 6) {
            usage(argc, argv);
            exit(1);
        }
    }

    if (argc >= 6) {
        sAlpha = argv[5];
    }

    ga.enable = StrToL(sOnOff);
    ga.alpha = StrToL(sAlpha);
    nRet = ioctl(g_nFbDevFd, MXCFB_SET_GBL_ALPHA, &ga);
    if (nRet != 0) {
        fprintf(stderr, "*** Error: failed to set global alpha\n");
    }

    return nRet;
}

int handle_cmd_set_visible_rect(int argc, char *argv[])
{
    int nRet = 0;
    long x = 0;
    long y = 0;
    unsigned long width = 0;
    unsigned long height = 0;
    struct fb_var_screeninfo vsi;

    if (argc < 8) {
        usage(argc, argv);
        exit(1);
    }

    x = StrToL(argv[4]);
    y = StrToL(argv[5]);
    width = StrToUL(argv[6]);
    height = StrToUL(argv[7]);

    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to get variable screen infomation\n");
        }
    }

    vsi.xres = width;
    vsi.yres = height;
    vsi.xoffset = x;
    vsi.yoffset = y;

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOPUT_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set variable screen infomation\n");
        }
    }

    return nRet;
}

int handle_cmd_set_color_key(int argc, char *argv[])
{
    int nRet = 0;
    std::string sOnOff;
    std::string sColor;
    struct mxcfb_color_key ck;

    memset(&ck, 0, sizeof(mxcfb_color_key));

    if (argc < 5) {
        usage(argc, argv);
        exit(1);
    }

    sOnOff = argv[4];
    if (sOnOff != "0" && sOnOff != "1") {
        usage(argc, argv);
        exit(1);
    }

    if (sOnOff == "1") {
        if (argc < 6) {
            usage(argc, argv);
            exit(1);
        }
    }

    sColor = argv[5];

    ck.enable = StrToL(sOnOff);
    ck.color_key = StrToUL(sColor);
    nRet = ioctl(g_nFbDevFd, MXCFB_SET_CLR_KEY, &ck);
    if (nRet != 0) {
        fprintf(stderr, "*** Error: failed to set global alpha\n");
    }

    return nRet;
}

static unsigned int fmt_to_bpp(unsigned int pixelformat)
{
    unsigned int bpp;

    switch (pixelformat)
    {
    case IPU_PIX_FMT_RGB565:
    /*interleaved 422*/
    case IPU_PIX_FMT_YUYV:
    case IPU_PIX_FMT_UYVY:
    /*non-interleaved 422*/
    case IPU_PIX_FMT_YUV422P:
    case IPU_PIX_FMT_YVU422P:
            bpp = 16;
            break;
    case IPU_PIX_FMT_BGR24:
    case IPU_PIX_FMT_RGB24:
    case IPU_PIX_FMT_YUV444:
            bpp = 24;
            break;
    case IPU_PIX_FMT_BGR32:
    case IPU_PIX_FMT_BGRA32:
    case IPU_PIX_FMT_RGB32:
    case IPU_PIX_FMT_RGBA32:
    case IPU_PIX_FMT_ABGR32:
            bpp = 32;
            break;
    /*non-interleaved 420*/
    case IPU_PIX_FMT_YUV420P:
    case IPU_PIX_FMT_YVU420P:
    case IPU_PIX_FMT_YUV420P2:
    case IPU_PIX_FMT_NV12:
            bpp = 12;
            break;
    default:
            bpp = 8;
            break;
    }
    return bpp;
}

int handle_cmd_set_color_space(int argc, char *argv[])
{
    static struct ColorSpace {
        const char *four_cc;
        const char *desc;
    } g_colorSpaceList[] = {
        { "RGBP", "RGB565" },
        { "BGR3", "BGR24" },
        { "RGB3", "RGB24" },
        { "BGR4", "BGR32" },
        { "BGRA", "BGRA32" },
        { "RGB4", "RGB32" },
        { "RGBA", "RGBA32" },
        { "ABGR", "ABGR32" },
        { "YUYV", "YUYV" },
        { "UYVY", "UYVY" },
        { "Y444", "YUV444" },
        { "NV12", "NV12" },
        { "I420", "YUV420P" },
        { "422P", "YUV422P" },
        { "YV16", "YVU422P" },
    };

    int nRet = 0;
    unsigned int count = sizeof(g_colorSpaceList) / sizeof(g_colorSpaceList[0]);
    unsigned int i;
    std::string sFourCC;
    struct fb_var_screeninfo fb_var;
    unsigned int pixelformat;

    if (argc < 5) {
        sFourCC = "?";
    } else {
        sFourCC = std::string(argv[4]);
    }

    if (nRet == 0) {
        for (i = 0; i < count; i++) {
            if (sFourCC == g_colorSpaceList[i].four_cc) {
                break;
            }
        }
        if (i == count) {
            fprintf(stderr, "*** Error: not suppored four CC: %s\n", sFourCC.c_str());
            fprintf(stderr, "Available four CC:\n");
            for (i = 0; i < count; i++) {
                fprintf(stderr, "    %s: %s\n", g_colorSpaceList[i].four_cc, g_colorSpaceList[i].desc);
            }

            nRet = -1;
        }
    }

    if (nRet == 0) {
        pixelformat = v4l2_fourcc(sFourCC[0], sFourCC[1], sFourCC[2], sFourCC[3]);
    }

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_VSCREENINFO, &fb_var);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set variable screen infomation\n");
        }
    }

    if (nRet == 0) {
        fb_var.activate |= FB_ACTIVATE_FORCE;
        fb_var.nonstd = pixelformat;
        fb_var.bits_per_pixel = pixelformat;
        nRet = ioctl(g_nFbDevFd, FBIOPUT_VSCREENINFO, &fb_var);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set variable screen infomation\n");
        }
    }

    return nRet;
}

//int handle_cmd_set_blank(int argc, char *argv[])
//{
//    int nRet = 0;
//    std::string sFbDevFilePath;
//    std::string sBlank;
//    int nFbDevNo = 0;

//    if (argc < 5) {
//        usage(argc, argv);
//        exit(1);
//    }
    
//    sFbDevFilePath = argv[1];
//    std::string sFbDevNo = std::string() + sFbDevFilePath[sFbDevFilePath.length() - 1];
//    nFbDevNo = StrToUL(sFbDevNo);

//    sBlank = argv[4];
//    if (sBlank != "0" && sBlank != "1") {
//        usage(argc, argv);
//        exit(1);
//    }
    
//    std::string sCmdLine = FormatStr("echo %s > /sys/class/graphics/fb%d/blank", sBlank.c_str(), nFbDevNo);
//    nRet = system(sCmdLine.c_str());
    
//    return nRet;
//}

int handle_cmd_set_blank(int argc, char *argv[])
{
    int nRet = 0;
    std::string sBlank;
    unsigned int nblankCmd;

    if (argc < 5) {
        usage(argc, argv);
        exit(1);
    }

    sBlank = argv[4];
    if (sBlank == "1") {
        nblankCmd = FB_BLANK_POWERDOWN;
    } else if (sBlank == "0") {
        nblankCmd = FB_BLANK_UNBLANK;
    } else {
        usage(argc, argv);
        exit(1);
    }

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOBLANK, nblankCmd);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set blank!\n");
        }
    }

    return nRet;
}

int handle_cmd_get_fb_info(int argc, char *argv[])
{
    TK_UNUSED_VAR(argc);
    TK_UNUSED_VAR(argv);

    int nRet = 0;
    struct fb_fix_screeninfo vfi;
    struct fb_var_screeninfo vsi;

    memset(&vfi, 0, sizeof(struct fb_fix_screeninfo));
    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_FSCREENINFO, &vfi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to get fixed screen infomation\n");
        } else {
            output_fix_screeninfo(&vfi);
        }
    }

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to get variable screen infomation\n");
        } else {
            output_var_screeninfo(&vsi);
        }
    }
    
    return nRet;
}

int handle_cmd_swap_buffers(int argc, char *argv[])
{
    TK_UNUSED_VAR(argc);
    TK_UNUSED_VAR(argv);

    int nRet = 0;
    struct fb_var_screeninfo vsi;

    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to get variable screen infomation\n");
        }
    }

    if (nRet == 0) {
        vsi.yoffset += vsi.yres;
        if (vsi.yoffset >= 1440) {
            vsi.yoffset = 0;
        }

        nRet = ioctl(g_nFbDevFd, FBIOPAN_DISPLAY, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to display the bg buffer\n");
        }
    }

    return nRet;
}

int handle_cmd_set_res(int argc, char *argv[])
{
    int nRet = 0;
    unsigned long xres = 0;
    unsigned long yres = 0;
    struct fb_var_screeninfo vsi;

    if (argc < 6) {
        usage(argc, argv);
        exit(1);
    }

    xres = StrToUL(argv[4]);
    yres = StrToUL(argv[5]);

    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to get variable screen infomation\n");
        }
    }

    vsi.xres = xres;
    vsi.yres = yres;

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOPUT_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set variable screen infomation\n");
        }
    }

    return nRet;
}

int handle_cmd_set_vres(int argc, char *argv[])
{
    int nRet = 0;
    unsigned long xres_virtual = 0;
    unsigned long yres_virtual = 0;
    struct fb_var_screeninfo vsi;

    if (argc < 6) {
        usage(argc, argv);
        exit(1);
    }

    xres_virtual = StrToUL(argv[4]);
    yres_virtual = StrToUL(argv[5]);

    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to get variable screen infomation\n");
        }
    }

    vsi.xres_virtual = xres_virtual;
    vsi.yres_virtual = yres_virtual;

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOPUT_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set variable screen infomation\n");
        }
    }

    return nRet;
}

int handle_cmd_set_offset(int argc, char *argv[])
{
    int nRet = 0;
    long xoffset = 0;
    long yoffset = 0;
    struct fb_var_screeninfo vsi;

    if (argc < 6) {
        usage(argc, argv);
        exit(1);
    }

    xoffset = StrToL(argv[4]);
    yoffset = StrToL(argv[5]);

    memset(&vsi, 0, sizeof(struct fb_var_screeninfo));

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOGET_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to get variable screen infomation\n");
        }
    }

    vsi.xoffset = xoffset;
    vsi.yoffset = yoffset;

    if (nRet == 0) {
        nRet = ioctl(g_nFbDevFd, FBIOPUT_VSCREENINFO, &vsi);
        if (nRet != 0) {
            fprintf(stderr, "*** Error: failed to set variable screen infomation\n");
        }
    }

    return nRet;
}

int main(int argc, char *argv[])
{
    TK_UNUSED_VAR(fmt_to_bpp);

    int nRet = 0;
    std::string sFbDevFilePath;
    std::string sCmd1;
    std::string sCmd2;

    if (argc < 4) {
        usage(argc, argv);
        exit(1);
    }

    sFbDevFilePath = argv[1];
    sCmd1 = argv[2];
    sCmd2 = argv[3];

    g_nFbDevFd = open(sFbDevFilePath.c_str(), O_RDWR, 0);
    if (g_nFbDevFd == -1) {
        fprintf(stderr, "*** Error: failed to open the fb file: %s\n", sFbDevFilePath.c_str());
        nRet = -1;
    }

    if (nRet == 0) {
        if (sCmd1 == "set" && sCmd2 == "local_alpha") {
            nRet = handle_cmd_set_local_alpha(argc, argv);
        } else if (sCmd1 == "set" && sCmd2 == "global_alpha") {
            nRet = handle_cmd_set_global_alpha(argc, argv);
        } else if (sCmd1 == "set" && sCmd2 == "color_key") {
            nRet = handle_cmd_set_color_key(argc, argv);
        } else if (sCmd1 == "set" && sCmd2 == "rect") {
            nRet = handle_cmd_set_visible_rect(argc, argv);
        } else if (sCmd1 == "set" && sCmd2 == "res") {
            nRet = handle_cmd_set_res(argc, argv);
        } else if (sCmd1 == "set" && sCmd2 == "vres") {
            nRet = handle_cmd_set_vres(argc, argv);
        } else if (sCmd1 == "set" && sCmd2 == "offset") {
            nRet = handle_cmd_set_offset(argc, argv);
        } else if (sCmd1 == "set" && sCmd2 == "blank") {
            nRet = handle_cmd_set_blank(argc, argv);
        } else if (sCmd1 == "set" && sCmd2 == "color_space") {
            nRet = handle_cmd_set_color_space(argc, argv);
        } else if (sCmd1 == "set" && sCmd2 == "standalone_local_alpha_test") {
            nRet = handle_cmd_set_standalone_local_alpha_test(argc, argv);
        } else if (sCmd1 == "get" && sCmd2 == "fb_info") {
            nRet = handle_cmd_get_fb_info(argc, argv);
        } else if (sCmd1 == "swap" && sCmd2 == "buffers") {
            nRet = handle_cmd_swap_buffers(argc, argv);
        } else {
            nRet = -1;
            usage(argc, argv);
            exit(1);
        }
    }

    return nRet;
}
