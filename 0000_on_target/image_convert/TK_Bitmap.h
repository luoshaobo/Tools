#ifndef TK_BITMAP_H__3789239762382385723487376576228435982
#define TK_BITMAP_H__3789239762382385723487376576228435982

#include "TK_Tools.h"

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

BOOL SaveBmpToFile_RGBA32(
    const char *pFilePath,
    const void *pBmpBits,
    unsigned long nBmpWidth,
    unsigned long nBmpHeight,
    unsigned long nBmpPitch,
    BOOL bBottomUp,
    const char *pColorOrder
);

} //namespace TK_Tools {

#endif // #ifndef TK_BITMAP_H__3789239762382385723487376576228435982
