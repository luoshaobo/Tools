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

#define LFLF()        fprintf(stderr, "%s: %d: %s\n", __FILE__, __LINE__, __FUNCTION__)

struct Size {
    Size() : width(0), height(0) {}
    Size(unsigned int a_width, unsigned int a_height) : width(a_width), height(a_height) {}

    unsigned int width;
    unsigned int height;
};

struct Region {
    Region() : x(0), y(0), width(0), height(0) {}
    Region(int a_x, int a_y, unsigned int a_width, unsigned int a_height) : x(a_x), y(a_y), width(a_width), height(a_height) {}

    int x;
    int y;
    unsigned int width;
    unsigned int height;
};

struct IOSizeAndRegion {
    Size inputSize;
    Size outputSize;
    Region inputRegion;
    Region outputRegion;
};

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

    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    %s "
            "[--help|-H] "
            "[--input_file|-i <input_picture_file_path>] "
            "[--output_file|-o <output_picture_file_path>] "
            "[--output_size|-S <output_width>,<output_height>] "
            "[--input_region|-r <input_region_x>,<input_region_y>,<input_region_width>,<input_region_height>] "
            "[--output_region|-R <output_region_x>,<output_region_y>[,<output_region_width>,<output_region_height>]] "
            "[--clear_color|-C <clear_color>] "
            "[<input_file>[, <output_file>]]"
            "\n", 
            basename(argv[0]));
            
    fprintf(stderr, "Comments:\n");
    fprintf(stderr, "    --help|-H: to ouput this help information.\n");
    fprintf(stderr, "    --input_file|-i <input_picture_file_path>: the path of the input image file.\n");
    fprintf(stderr, "        <input_picture_file_path> can be a normal file path, or \'-\'(for stdin, as default).\n");
    fprintf(stderr, "    --output_file|-o <output_picture_file_path>: the path of the output image file.\n");
    fprintf(stderr, "        <output_picture_file_path> can be a normal file path, or \'-.<picture_file_suffix>\'(for stdout, \'-.png\' as default).\n");
    fprintf(stderr, "    --output_size|-S <output_width>,<output_height>: the size of the output image.\n");
    fprintf(stderr, "        The default output size is equal to the size of the input region.\n");
    fprintf(stderr, "    --input_region|-r <input_region_x>,<input_region_y>,<input_region_width>,<input_region_height>: the region of the input image to be copied from.\n");
    fprintf(stderr, "        The default input region is equal to the whole region of the input image.\n");
    fprintf(stderr, "    --output_region|-R <output_region_x>,<output_region_y>[,<output_region_width>,<output_region_height>]: the region of the output image to be copied to.\n");
    fprintf(stderr, "        The default output region is equal to the whole region of the output image.\n");
    fprintf(stderr, "        The default width and height of the output region are equal to those of the input region.\n");
    fprintf(stderr, "    --clear_color|-C <clear_color>: to clear the output with the given color.\n");
    fprintf(stderr, "        <clear_color> can be \"red\", \"blue\", ect, or \'#AARRGGBB\'. \'#00000000\' as default.\n");
}

bool CalcInputRegin(const std::string &sInputRegion, IOSizeAndRegion &ioSizeAndRegion)
{
    bool bSuc = true;
    std::vector<std::string> reginParts;

    if (bSuc) {
        if (sInputRegion.empty()) {
            ioSizeAndRegion.inputRegion.x = 0;
            ioSizeAndRegion.inputRegion.y = 0;
            ioSizeAndRegion.inputRegion.width = ioSizeAndRegion.inputSize.width;
            ioSizeAndRegion.inputRegion.height = ioSizeAndRegion.inputSize.height;
        } else {
            reginParts = SplitString(sInputRegion, ",");
            if (reginParts.size() == 4) {
                ioSizeAndRegion.inputRegion.x = StrToL(reginParts[0]);
                ioSizeAndRegion.inputRegion.y = StrToL(reginParts[1]);
                ioSizeAndRegion.inputRegion.width = StrToUL(reginParts[2]);
                ioSizeAndRegion.inputRegion.height = StrToUL(reginParts[3]);
            } else {
                bSuc = false;
            }
        }
    }

    return bSuc;
}

bool CalcOutputSize(const std::string &sOutputSize, IOSizeAndRegion &ioSizeAndRegion)
{
    bool bSuc = true;
    std::vector<std::string> sizeParts;

    if (bSuc) {
        if (sOutputSize.empty()) {
            ioSizeAndRegion.outputSize.width = ioSizeAndRegion.inputRegion.width;
            ioSizeAndRegion.outputSize.height = ioSizeAndRegion.inputRegion.height;
        } else {
            sizeParts = SplitString(sOutputSize, ",");
            if (sizeParts.size() == 2) {
                ioSizeAndRegion.outputSize.width = StrToUL(sizeParts[0]);
                ioSizeAndRegion.outputSize.height = StrToUL(sizeParts[1]);
            } else {
                bSuc = false;
            }
        }
    }


    return bSuc;
}

bool CalcOutputRegin(const std::string &sOutputRegion, IOSizeAndRegion &ioSizeAndRegion)
{
    bool bSuc = true;
    std::vector<std::string> reginParts;

    if (bSuc) {
        if (sOutputRegion.empty()) {
            ioSizeAndRegion.outputRegion.x = 0;
            ioSizeAndRegion.outputRegion.y = 0;
            ioSizeAndRegion.outputRegion.width = ioSizeAndRegion.outputSize.width;
            ioSizeAndRegion.outputRegion.height = ioSizeAndRegion.outputSize.height;
        } else {
            reginParts = SplitString(sOutputRegion, ",");
            if (reginParts.size() == 4) {
                ioSizeAndRegion.outputRegion.x = StrToL(reginParts[0]);
                ioSizeAndRegion.outputRegion.y = StrToL(reginParts[1]);
                ioSizeAndRegion.outputRegion.width = StrToUL(reginParts[2]);
                ioSizeAndRegion.outputRegion.height = StrToUL(reginParts[3]);
            } else if (reginParts.size() == 2) {
                ioSizeAndRegion.outputRegion.x = StrToL(reginParts[0]);
                ioSizeAndRegion.outputRegion.y = StrToL(reginParts[1]);
                ioSizeAndRegion.outputRegion.width = ioSizeAndRegion.inputRegion.width;
                ioSizeAndRegion.outputRegion.height = ioSizeAndRegion.inputRegion.height;
            } else {
                bSuc = false;
            }
        }
    }

    return bSuc;
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

int main(int argc, char *argv[])
{
    int nRet = 0;
    std::string arg_input_file = "-";
    std::string arg_output_file = "-.png";
    std::string arg_output_size;
    std::string arg_input_region;
    std::string arg_output_region;
    std::string arg_clear_color = "#00000000";

    std::string sInputFilePath;
    std::string sOutputFilePath;
    IOSizeAndRegion ioSizeAndRegion;
    QColor clearColor;

    QImage *pInputImage = NULL;
    QImage *pOutputImage = NULL;
    QPainter *pOutPainter = NULL;
    
    int opt;
    const char *short_options = "Hi:o:S:r:R:C:";
    struct option long_options[] = {
        { "help", no_argument, NULL, 'H' },
        { "input_file",  required_argument, NULL, 'i' },
        { "output_file",  required_argument, NULL, 'o' },
        { "output_size",  required_argument, NULL, 'S' },
        { "input_region",  required_argument, NULL, 'r' },
        { "output_region",  required_argument, NULL, 'R' },
        { "clear_color",  required_argument, NULL, 'C' },
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
            case 'i':
                {
                    arg_input_file = std::string(optarg);
                }
                break;
            case 'o':
                {
                    arg_output_file = std::string(optarg);
                }
                break;
            case 'S':
                {
                    arg_output_size = std::string(optarg);
                }
                break;
            case 'r':
                {
                    arg_input_region = std::string(optarg);
                }
                break;
            case 'R':
                {
                    arg_output_region = std::string(optarg);
                }
                break;
            case 'C':
                {
                    arg_clear_color = std::string(optarg);
                }
                break;
            default:
                break;
        }
    }

    if (nRet == 0) {
        if (opt != -1) {
            usage(argc, argv);
            nRet = -1;
        }
    }

    if (nRet == 0) {
        if (optind < argc) {
            arg_input_file = std::string(argv[optind]);
            optind++;
        }

        if (optind < argc) {
            arg_output_file = std::string(argv[optind]);
            optind++;
        }
    }

    if (nRet == 0) {
        sInputFilePath = arg_input_file;
        if (sInputFilePath.empty()) {
            fprintf(stderr, "*** Error: the path of the input picture file can not be empty.\n");
            nRet = -1;
        }
    }

    if (nRet == 0) {
        sOutputFilePath = arg_output_file;
        if (sOutputFilePath.empty()) {
            fprintf(stderr, "*** Error: the path of the output picture file can not be empty.\n");
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        pInputImage = new QImage();
        if (pInputImage == NULL) {
            fprintf(stderr, "*** Error: failed to create the input image.\n");
            nRet = -1;
        } else {
            if (sInputFilePath == "-") {
                std::vector<unsigned char> vFileContent;
                if (!GetInputFileContentFromStdin(vFileContent)) {
                    fprintf(stderr, "*** Error: failed to read the input picture from stdin.\n");
                    nRet = -1;
                } else {
                    if (!pInputImage->loadFromData(&vFileContent[0], vFileContent.size())) {
                        fprintf(stderr, "*** Error: failed to load the input picture from stdin.\n");
                        nRet = -1;
                    }
                }
            } else {
                if (!pInputImage->load(sInputFilePath.c_str())) {
                    fprintf(stderr, "*** Error: failed to load the input picture file: %s.\n", sInputFilePath.c_str());
                    nRet = -1;
                }
            }

            if (nRet == 0) {
                ioSizeAndRegion.inputSize.width = pInputImage->width();
                ioSizeAndRegion.inputSize.height = pInputImage->height();
            }
        }
    }

    if (nRet == 0) {
        if (!CalcInputRegin(arg_input_region, ioSizeAndRegion)) {
            fprintf(stderr, "*** Error: failed to calculate the input region: %s.\n", arg_input_region.c_str());
            nRet = -1;
        }
    }

    if (nRet == 0) {
        if (!CalcOutputSize(arg_output_size, ioSizeAndRegion)) {
            fprintf(stderr, "*** Error: failed to calculate the output size: %s.\n", arg_output_size.c_str());
            nRet = -1;
        }
    }

    if (nRet == 0) {
        if (!CalcOutputRegin(arg_output_region, ioSizeAndRegion)) {
            fprintf(stderr, "*** Error: failed to calculate the output region: %s.\n", arg_output_region.c_str());
            nRet = -1;
        }
    }

    if (nRet == 0) {
        clearColor = QColor(arg_clear_color.c_str());
        if (!clearColor.isValid()) {
            fprintf(stderr, "*** Error: the clear color is invalid: %s.\n", arg_clear_color.c_str());
            nRet = -1;
        }
    }

    if (nRet == 0) {
        fprintf(stderr, "Input picture:  size=(%u,%u), region=(%d,%d,%u,%u)\n",
            ioSizeAndRegion.inputSize.width, ioSizeAndRegion.inputSize.height,
            ioSizeAndRegion.inputRegion.x, ioSizeAndRegion.inputRegion.y, ioSizeAndRegion.inputRegion.width, ioSizeAndRegion.inputRegion.height
        );
        fprintf(stderr, "Output picture: size=(%u,%u), region=(%d,%d,%u,%u)\n",
            ioSizeAndRegion.outputSize.width, ioSizeAndRegion.outputSize.height,
            ioSizeAndRegion.outputRegion.x, ioSizeAndRegion.outputRegion.y, ioSizeAndRegion.outputRegion.width, ioSizeAndRegion.outputRegion.height
        );
    }
    
    if (nRet == 0) {
        pOutputImage = new QImage(ioSizeAndRegion.outputSize.width, ioSizeAndRegion.outputSize.height, QImage::Format_ARGB32_Premultiplied);
        if (pOutputImage == NULL) {
            fprintf(stderr, "*** Error: failed to create the output image.\n");
            nRet = -1;
        }
    }

    if (nRet == 0) {
        pOutPainter = new QPainter(pOutputImage);
        if (pOutPainter == NULL) {
            fprintf(stderr, "*** Error: failed to create the output painter.\n");
            nRet = -1;
        }
    }

    if (nRet == 0) {
        pOutPainter->setCompositionMode(QPainter::CompositionMode_Source);
        pOutPainter->fillRect(0, 0, ioSizeAndRegion.outputSize.width, ioSizeAndRegion.outputSize.height, clearColor);
        QRect targetRect(ioSizeAndRegion.outputRegion.x, ioSizeAndRegion.outputRegion.y, ioSizeAndRegion.outputRegion.width, ioSizeAndRegion.outputRegion.height);
        QRect sourceRect(ioSizeAndRegion.inputRegion.x, ioSizeAndRegion.inputRegion.y, ioSizeAndRegion.inputRegion.width, ioSizeAndRegion.inputRegion.height);
        pOutPainter->drawImage(targetRect, *pInputImage, sourceRect);
    }

    if (nRet == 0) {
        if (sOutputFilePath.length() >= 2 && sOutputFilePath[0] == '-' && sOutputFilePath[1] == '.') {
            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);
            if (!pOutputImage->save(&buffer, sOutputFilePath.substr(2).c_str())) {
                fprintf(stderr, "*** Error: failed to save to the picture file to byte array.\n");
                nRet = -1;
            } else {
                if (!PutOntputFileContentToStdout(ba)) {
                    fprintf(stderr, "*** Error: failed to save to the picture file to stdout.\n");
                    nRet = -1;
                }
            }
        } else {
            if (!pOutputImage->save(sOutputFilePath.c_str())) {
                fprintf(stderr, "*** Error: failed to save to the picture file: %s.\n", sOutputFilePath.c_str());
                nRet = -1;
            }
        }
    }

    if (pOutPainter != NULL) {
        delete pOutPainter;
        pOutPainter = NULL;
    }

    if (pOutputImage != NULL) {
        delete pOutputImage;
        pOutputImage = NULL;
    }

    if (pInputImage != NULL) {
        delete pInputImage;
        pInputImage = NULL;
    }

    return 0;
}
