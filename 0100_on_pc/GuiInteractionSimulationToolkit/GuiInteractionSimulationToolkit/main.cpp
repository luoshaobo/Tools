// GuiInteractionSimulationToolkit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <vector>
#include <string>
#include "TK_Tools.h"
#include "WinGuiISTK.h"
#include "RemoteCmd.h"

#define COMMAND_HANDLER_PAIR(command)               { #command, &CommandHandler_ ## command }
#define VK_INFO_PAIR(vk)                            { #vk, vk }
#define VK_INFO_PAIR2(vk,vk_code)                   { #vk, vk_code }

void help(int argc, char* argv[])
{
    FPRINTF(stdout, "Usage:\n");
    FPRINTF(stdout, "  %s RemoteServer [<port>=8888] [<ip>=0.0.0.0]\n", basename(argv[0]));
    FPRINTF(stdout, "  %s Delay <sMilliseconds>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s scnCount <sTitle> <bFullMatched>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s scnShow <sTitle> <bFullMatched> <bAllMatched> SSM_RESTORE|SSM_NORMAL|SSM_MIN|SSM_MAX|SSM_FG\n", basename(argv[0]));
    FPRINTF(stdout, "  %s scnHide <sTitle> <bFullMatched> <bAllMatched>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s scnClose <sTitle> <bFullMatched> <bAllMatched>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s scnMove <sTitle> <bFullMatched> <bAllMatched> <x,y>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s scnResize <sTitle> <bFullMatched> <bAllMatched> <x,y,w,h>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s scnSetZorder <sTitle> <bFullMatched> <bAllMatched> SZO_BOTTOM|SZO_TOP\n", basename(argv[0]));
    FPRINTF(stdout, "  %s scnSaveAsPics <sTitle> <bFullMatched> <bAllMatched> <pictureFilePath>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s scnSaveDesktopAsPic <pictureFilePath>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s cbdPutStr <sString>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s cbdGetStr\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdVkList\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdKeyDown <sVirtualKey>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdKeyUp <sVirtualKey>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdKeyOn <sVirtualKey>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdKeyOff <sVirtualKey>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdCtrlA\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdCtrlC\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdCtrlX\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdCtrlV\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdChar <nChar>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s kbdStr <sString>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s mseMove <x,y> [<bAbsolute>=true]\n", basename(argv[0]));
    FPRINTF(stdout, "  %s mseClick <x,y>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s mseDClick <x,y>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s mseDrag <xSrc,ySrc> <xDst,yDst>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s mseRClick <x,y>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s mseDRClick <x,y>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s mseRDrag <xSrc,ySrc> <xDst,yDst>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s mseScroll <x,y> <nSteps>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s imgFindRect <sImagePath[,sImagePath[,...]]>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s imgFindRect <sImagePath[,sImagePath[,...]]> <xRegion,yRegion,wRegion,hRegion>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s imgFindRect <sImagePath[,sImagePath[,...]]> <xBeginning,yBeginning>\n", basename(argv[0]));
    FPRINTF(stdout, "  %s imgWaitShown <sImagePath[,sImagePath[,...]]> [<nTimeout>=-1]\n", basename(argv[0]));
    FPRINTF(stdout, "  %s imgWaitShown <sImagePath[,sImagePath[,...]]> <xRegion,yRegion,wRegion,hRegion> [<nTimeout>=-1]\n", basename(argv[0]));
    FPRINTF(stdout, "  %s imgWaitShown <sImagePath[,sImagePath[,...]]> <xBeginning,yBeginning> [<nTimeout>=-1]\n", basename(argv[0]));

    FPRINTF(stdout, "Note:\n");
    FPRINTF(stdout, "1) The environment variables REMOTE_SERVER_IP and REMOTE_SERVER_PORT can be set to run in remote client mode.\n");
    FPRINTF(stdout, "2) The environment variable DESKTOP_PICTURE_FILE_PATH can be set to simulate the bitmap of the desktop.\n");

    FPRINTF(stdout, "\n");
}

bool parseBoolFromStr(bool &result, const std::string s)
{
    bool bRet = true;
    std::string lowerS = TK_Tools::LowerCase(s);

    result = false;

    if (bRet) {
        if (lowerS == "true" 
            || lowerS == "1" 
            || lowerS == "on"
        ) {
            result = true;
        } else if (lowerS == "false" 
            || lowerS == "0" 
            || lowerS == "off"
        ) {
            result = false;
        } else {
            bRet = false;
        }
    }

    return bRet;
}

bool parsePointFromStr(GuiISTk::Point &point, const std::string s)
{
    bool bRet = true;
    std::vector<std::string> ss;

    if (bRet) {
        ss = TK_Tools::SplitString(s, ",");
        if (ss.size() != 2) {
            bRet = false;
        }
    }

    if (bRet) {
        point.x = (int)TK_Tools::StrToL(ss[0]);
        point.y = (int)TK_Tools::StrToL(ss[1]);
    }

    return bRet;
}

bool parseSizeFromStr(GuiISTk::Size &size, const std::string s)
{
    bool bRet = true;
    std::vector<std::string> ss;

    if (bRet) {
        ss = TK_Tools::SplitString(s, ",");
        if (ss.size() != 2) {
            bRet = false;
        }
    }

    if (bRet) {
        size.width = (unsigned int)TK_Tools::StrToL(ss[0]);
        size.height = (unsigned int)TK_Tools::StrToL(ss[1]);
    }

    return bRet;
}

bool parseRectFromStr(GuiISTk::Rect &rect, const std::string s)
{
    bool bRet = true;
    std::vector<std::string> ss;

    if (bRet) {
        ss = TK_Tools::SplitString(s, ",");
        if (ss.size() != 4) {
            bRet = false;
        }
    }

    if (bRet) {
        rect.x = (int)TK_Tools::StrToL(ss[0]);
        rect.y = (int)TK_Tools::StrToL(ss[1]);
        rect.width = (unsigned int)TK_Tools::StrToL(ss[2]);
        rect.height = (unsigned int)TK_Tools::StrToL(ss[3]);
    }

    return bRet;
}

namespace {
    struct Argument {
        Argument() : isStrArray(false), str(), strArray() {}
        bool isStrArray;
        std::string str;
        std::vector<std::string> strArray;
    };

    struct VkInfoItem {
        const char* vkStr;
        unsigned char vk;
    } vkInfo[] = {
        VK_INFO_PAIR(VK_LBUTTON),
        VK_INFO_PAIR(VK_RBUTTON),
        VK_INFO_PAIR(VK_CANCEL),
        VK_INFO_PAIR(VK_MBUTTON),
        VK_INFO_PAIR(VK_XBUTTON1),
        VK_INFO_PAIR(VK_XBUTTON2),
        VK_INFO_PAIR(VK_BACK),
        VK_INFO_PAIR(VK_TAB),
        VK_INFO_PAIR(VK_CLEAR),
        VK_INFO_PAIR(VK_RETURN),
        VK_INFO_PAIR(VK_SHIFT),
        VK_INFO_PAIR(VK_CONTROL),
        VK_INFO_PAIR(VK_MENU),
        VK_INFO_PAIR(VK_PAUSE),
        VK_INFO_PAIR(VK_CAPITAL),
        VK_INFO_PAIR(VK_KANA),
        VK_INFO_PAIR(VK_HANGUL),
        VK_INFO_PAIR(VK_JUNJA),
        VK_INFO_PAIR(VK_FINAL),
        VK_INFO_PAIR(VK_HANJA),
        VK_INFO_PAIR(VK_KANJI),
        VK_INFO_PAIR(VK_ESCAPE),
        VK_INFO_PAIR(VK_CONVERT),
        VK_INFO_PAIR(VK_NONCONVERT),
        VK_INFO_PAIR(VK_ACCEPT),
        VK_INFO_PAIR(VK_MODECHANGE),
        VK_INFO_PAIR(VK_SPACE),
        VK_INFO_PAIR(VK_PRIOR),
        VK_INFO_PAIR(VK_NEXT),
        VK_INFO_PAIR(VK_END),
        VK_INFO_PAIR(VK_HOME),
        VK_INFO_PAIR(VK_LEFT),
        VK_INFO_PAIR(VK_UP),
        VK_INFO_PAIR(VK_RIGHT),
        VK_INFO_PAIR(VK_DOWN),
        VK_INFO_PAIR(VK_SELECT),
        VK_INFO_PAIR(VK_PRINT),
        VK_INFO_PAIR(VK_EXECUTE),
        VK_INFO_PAIR(VK_SNAPSHOT),
        VK_INFO_PAIR(VK_INSERT),
        VK_INFO_PAIR(VK_DELETE),
        VK_INFO_PAIR(VK_HELP),
        VK_INFO_PAIR2(0, '0'),
        VK_INFO_PAIR2(1, '1'),
        VK_INFO_PAIR2(2, '2'),
        VK_INFO_PAIR2(3, '3'),
        VK_INFO_PAIR2(4, '4'),
        VK_INFO_PAIR2(5, '5'),
        VK_INFO_PAIR2(6, '6'),
        VK_INFO_PAIR2(7, '7'),
        VK_INFO_PAIR2(8, '8'),
        VK_INFO_PAIR2(9, '9'),
        VK_INFO_PAIR2(A, 'A'),
        VK_INFO_PAIR2(B, 'B'),
        VK_INFO_PAIR2(C, 'C'),
        VK_INFO_PAIR2(D, 'D'),
        VK_INFO_PAIR2(E, 'E'),
        VK_INFO_PAIR2(F, 'F'),
        VK_INFO_PAIR2(G, 'G'),
        VK_INFO_PAIR2(H, 'H'),
        VK_INFO_PAIR2(I, 'I'),
        VK_INFO_PAIR2(J, 'J'),
        VK_INFO_PAIR2(K, 'K'),
        VK_INFO_PAIR2(L, 'L'),
        VK_INFO_PAIR2(M, 'M'),
        VK_INFO_PAIR2(N, 'N'),
        VK_INFO_PAIR2(O, 'O'),
        VK_INFO_PAIR2(P, 'P'),
        VK_INFO_PAIR2(Q, 'Q'),
        VK_INFO_PAIR2(R, 'R'),
        VK_INFO_PAIR2(S, 'S'),
        VK_INFO_PAIR2(T, 'T'),
        VK_INFO_PAIR2(U, 'U'),
        VK_INFO_PAIR2(V, 'V'),
        VK_INFO_PAIR2(W, 'W'),
        VK_INFO_PAIR2(X, 'X'),
        VK_INFO_PAIR2(Y, 'Y'),
        VK_INFO_PAIR2(Z, 'Z'),
        VK_INFO_PAIR(VK_LWIN),
        VK_INFO_PAIR(VK_RWIN),
        VK_INFO_PAIR(VK_APPS),
        VK_INFO_PAIR(VK_SLEEP),
        VK_INFO_PAIR(VK_NUMPAD0),
        VK_INFO_PAIR(VK_NUMPAD1),
        VK_INFO_PAIR(VK_NUMPAD2),
        VK_INFO_PAIR(VK_NUMPAD3),
        VK_INFO_PAIR(VK_NUMPAD4),
        VK_INFO_PAIR(VK_NUMPAD5),
        VK_INFO_PAIR(VK_NUMPAD6),
        VK_INFO_PAIR(VK_NUMPAD7),
        VK_INFO_PAIR(VK_NUMPAD8),
        VK_INFO_PAIR(VK_NUMPAD9),
        VK_INFO_PAIR(VK_MULTIPLY),
        VK_INFO_PAIR(VK_ADD),
        VK_INFO_PAIR(VK_SEPARATOR),
        VK_INFO_PAIR(VK_SUBTRACT),
        VK_INFO_PAIR(VK_DECIMAL),
        VK_INFO_PAIR(VK_DIVIDE),
        VK_INFO_PAIR(VK_F1),
        VK_INFO_PAIR(VK_F2),
        VK_INFO_PAIR(VK_F3),
        VK_INFO_PAIR(VK_F4),
        VK_INFO_PAIR(VK_F5),
        VK_INFO_PAIR(VK_F6),
        VK_INFO_PAIR(VK_F7),
        VK_INFO_PAIR(VK_F8),
        VK_INFO_PAIR(VK_F9),
        VK_INFO_PAIR(VK_F10),
        VK_INFO_PAIR(VK_F11),
        VK_INFO_PAIR(VK_F12),
        VK_INFO_PAIR(VK_F13),
        VK_INFO_PAIR(VK_F14),
        VK_INFO_PAIR(VK_F15),
        VK_INFO_PAIR(VK_F16),
        VK_INFO_PAIR(VK_F17),
        VK_INFO_PAIR(VK_F18),
        VK_INFO_PAIR(VK_F19),
        VK_INFO_PAIR(VK_F20),
        VK_INFO_PAIR(VK_F21),
        VK_INFO_PAIR(VK_F22),
        VK_INFO_PAIR(VK_F23),
        VK_INFO_PAIR(VK_F24),
        VK_INFO_PAIR(VK_NUMLOCK),
        VK_INFO_PAIR(VK_SCROLL),
        VK_INFO_PAIR(VK_LSHIFT),
        VK_INFO_PAIR(VK_RSHIFT),
        VK_INFO_PAIR(VK_LCONTROL),
        VK_INFO_PAIR(VK_RCONTROL),
        VK_INFO_PAIR(VK_LMENU),
        VK_INFO_PAIR(VK_RMENU),
        VK_INFO_PAIR(VK_BROWSER_BACK),
        VK_INFO_PAIR(VK_BROWSER_FORWARD),
        VK_INFO_PAIR(VK_BROWSER_REFRESH),
        VK_INFO_PAIR(VK_BROWSER_STOP),
        VK_INFO_PAIR(VK_BROWSER_SEARCH),
        VK_INFO_PAIR(VK_BROWSER_FAVORITES),
        VK_INFO_PAIR(VK_BROWSER_HOME),
        VK_INFO_PAIR(VK_VOLUME_MUTE),
        VK_INFO_PAIR(VK_VOLUME_DOWN),
        VK_INFO_PAIR(VK_VOLUME_UP),
        VK_INFO_PAIR(VK_MEDIA_NEXT_TRACK),
        VK_INFO_PAIR(VK_MEDIA_PREV_TRACK),
        VK_INFO_PAIR(VK_MEDIA_STOP),
        VK_INFO_PAIR(VK_MEDIA_PLAY_PAUSE),
        VK_INFO_PAIR(VK_LAUNCH_MAIL),
        VK_INFO_PAIR(VK_LAUNCH_MEDIA_SELECT),
        VK_INFO_PAIR(VK_LAUNCH_APP1),
        VK_INFO_PAIR(VK_LAUNCH_APP2),
        VK_INFO_PAIR(VK_OEM_1),
        VK_INFO_PAIR(VK_OEM_PLUS),
        VK_INFO_PAIR(VK_OEM_COMMA),
        VK_INFO_PAIR(VK_OEM_MINUS),
        VK_INFO_PAIR(VK_OEM_PERIOD),
        VK_INFO_PAIR(VK_OEM_2),
        VK_INFO_PAIR(VK_OEM_3),
        VK_INFO_PAIR(VK_OEM_4),
        VK_INFO_PAIR(VK_OEM_5),
        VK_INFO_PAIR(VK_OEM_6),
        VK_INFO_PAIR(VK_OEM_7),
        VK_INFO_PAIR(VK_OEM_8),
        VK_INFO_PAIR(VK_OEM_102),
        VK_INFO_PAIR(VK_PROCESSKEY),
        VK_INFO_PAIR(VK_PACKET),
        VK_INFO_PAIR(VK_ATTN),
        VK_INFO_PAIR(VK_CRSEL),
        VK_INFO_PAIR(VK_EXSEL),
        VK_INFO_PAIR(VK_EREOF),
        VK_INFO_PAIR(VK_PLAY),
        VK_INFO_PAIR(VK_ZOOM),
        VK_INFO_PAIR(VK_NONAME),
        VK_INFO_PAIR(VK_PA1),
        VK_INFO_PAIR(VK_OEM_CLEAR),
    };
    unsigned int vkInfoCount = sizeof(vkInfo) / sizeof(vkInfo[0]);
} // namespace {

bool parseVkFromStr(unsigned char &vk, const std::string &vkStr)
{
    bool bRet = false;
    unsigned int i;

    for (i = 0; i < vkInfoCount; ++i) {
        if (vkStr == vkInfo[i].vkStr) {
            vk = vkInfo[i].vk;
            bRet = true;
            break;
        }
    }

   return bRet;
}

bool parseScreenShowingModeFromStr(GuiISTk::ScreenShowingMode &mode, const std::string &modeStr)
{
    bool bRet = true;
    std::string modeStrUpper = TK_Tools::UpperCase(modeStr);
    
    if (modeStrUpper == "SSM_RESTORE") {
        mode = GuiISTk::SSM_RESTORE;
    } else if (modeStrUpper == "SSM_NORMAL") {
        mode = GuiISTk::SSM_NORMAL;
    } else if (modeStrUpper == "SSM_MIN") {
        mode = GuiISTk::SSM_MIN;
    } else if (modeStrUpper == "SSM_MAX") {
        mode = GuiISTk::SSM_MAX;
    } else if (modeStrUpper == "SSM_FG") {
        mode = GuiISTk::SSM_FG;
    } else {
        bRet = false;
    }

   return bRet;
}

bool parseScreenZorderFromStr(GuiISTk::ScreenZorder &zorder, const std::string &zorderStr)
{
    bool bRet = true;
    std::string zorderStrUpper = TK_Tools::UpperCase(zorderStr);
    
    if (zorderStrUpper == "SZO_BOTTOM") {
        zorder = GuiISTk::SZO_BOTTOM;
    } else if (zorderStrUpper == "SZO_TOP") {
        zorder = GuiISTk::SZO_TOP;
    } else {
        bRet = false;
    }

   return bRet;
}

bool parseImagesFromStr(std::vector<GuiISTk::Image> &images, const std::string &s)
{
    bool bSuc = true;
    std::vector<std::string> imageStrs;
    unsigned i;

    imageStrs = TK_Tools::SplitString(s, ",");
    for (i = 0; i < imageStrs.size(); ++i) {
        images.push_back(GuiISTk::Image(imageStrs[i]));
    }

    return bSuc;
}

std::vector<Argument> build_argument_array(int argc, char* argv[])
{
    std::vector<Argument> arguments;
    int i;

    for (i = 0; i < argc; ++i) {
        Argument argument;
        argument.isStrArray = false;
        argument.str = std::string(argv[i]);
        argument.strArray = TK_Tools::SplitString(argument.str, ",");
        if (argument.strArray.size() > 1) {
            argument.isStrArray = true;
        }
        arguments.push_back(argument);
    }

    return arguments;
}

int CommandHandler_kbdVkList(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    static const char *sAllVksInfo = 
        "  Virtual Key Name              Key Code            Comment\n" 
        "  VK_LBUTTON                    1                   Left mouse button\n" 
        "  VK_RBUTTON                    2                   Right mouse button\n" 
        "  VK_CANCEL                     3                   Control-break processing\n" 
        "  VK_MBUTTON                    4                   Middle mouse button\n" 
        "  VK_XBUTTON1                   5                   X1 mouse button\n" 
        "  VK_XBUTTON2                   6                   X2 mouse button\n" 
        "  VK_BACK                       8                   BACKSPACE\n" 
        "  VK_TAB                        9                   TAB\n" 
        "  VK_CLEAR                      12                  CLEAR\n" 
        "  VK_RETURN                     13                  ENTER\n" 
        "  VK_SHIFT                      16                  SHIFT\n" 
        "  VK_CONTROL                    17                  CTRL\n" 
        "  VK_MENU                       18                  ALT\n" 
        "  VK_PAUSE                      19                  PAUSE\n" 
        "  VK_CAPITAL                    20                  CAPS LOCK\n" 
        "  VK_KANA                       21                  IME Kana mode\n" 
        "  VK_HANGUL                     21                  IME Hanguel mode\n" 
        "  VK_JUNJA                      23                  IME Junja mode\n" 
        "  VK_FINAL                      24                  IME final mode\n" 
        "  VK_HANJA                      25                  IME Hanja mode\n" 
        "  VK_KANJI                      25                  IME Kanji mode\n" 
        "  VK_ESCAPE                     27                  ESC\n" 
        "  VK_CONVERT                    28                  IME convert\n" 
        "  VK_NONCONVERT                 29                  IME nonconvert\n" 
        "  VK_ACCEPT                     30                  IME accept\n" 
        "  VK_MODECHANGE                 31                  IME mode change request\n" 
        "  VK_SPACE                      32                  SPACE\n" 
        "  VK_PRIOR                      33                  PAGE UP\n" 
        "  VK_NEXT                       34                  PAGE DOWN\n" 
        "  VK_END                        35                  END\n" 
        "  VK_HOME                       36                  HOME\n" 
        "  VK_LEFT                       37                  LEFT ARROW\n" 
        "  VK_UP                         38                  UP ARROW\n" 
        "  VK_RIGHT                      39                  RIGHT ARROW\n" 
        "  VK_DOWN                       40                  DOWN ARROW\n" 
        "  VK_SELECT                     41                  SELECT\n" 
        "  VK_PRINT                      42                  PRINT\n" 
        "  VK_EXECUTE                    43                  EXECUTE\n" 
        "  VK_SNAPSHOT                   44                  PRINT SCREEN\n" 
        "  VK_INSERT                     45                  INS\n" 
        "  VK_DELETE                     46                  DEL \n" 
        "  VK_HELP                       47                  HELP\n" 
        "  0                             48                  0\n" 
        "  1                             49                  1\n" 
        "  2                             50                  2\n" 
        "  3                             51                  3\n" 
        "  4                             52                  4\n" 
        "  5                             53                  5\n" 
        "  6                             54                  6\n" 
        "  7                             55                  7\n" 
        "  8                             56                  8\n" 
        "  9                             57                  9\n" 
        "  A                             65                  A\n" 
        "  B                             66                  B\n" 
        "  C                             67                  C\n" 
        "  D                             68                  D\n" 
        "  E                             69                  E\n" 
        "  F                             70                  F\n" 
        "  G                             71                  G\n" 
        "  H                             72                  H\n" 
        "  I                             73                  I\n" 
        "  J                             74                  J\n" 
        "  K                             75                  K\n" 
        "  L                             76                  L\n" 
        "  M                             77                  M\n" 
        "  N                             78                  N\n" 
        "  O                             79                  O\n" 
        "  P                             80                  P\n" 
        "  Q                             81                  Q\n" 
        "  R                             82                  R\n" 
        "  S                             83                  S\n" 
        "  T                             84                  T\n" 
        "  U                             85                  U\n" 
        "  V                             86                  V\n" 
        "  W                             87                  W\n" 
        "  X                             88                  X\n" 
        "  Y                             89                  Y\n" 
        "  Z                             90                  Z\n" 
        "  VK_LWIN                       91                  Left Windows key\n" 
        "  VK_RWIN                       92                  Right Windows key\n" 
        "  VK_APPS                       93                  Applications key (to popup right-click memu)\n" 
        "  VK_SLEEP                      95                  Computer Sleep key\n" 
        "  VK_NUMPAD0                    96                  Numeric keypad 0 key\n" 
        "  VK_NUMPAD1                    97                  Numeric keypad 1 key\n" 
        "  VK_NUMPAD2                    98                  Numeric keypad 2 key\n" 
        "  VK_NUMPAD3                    99                  Numeric keypad 3 key\n" 
        "  VK_NUMPAD4                    100                 Numeric keypad 4 key\n" 
        "  VK_NUMPAD5                    101                 Numeric keypad 5 key\n" 
        "  VK_NUMPAD6                    102                 Numeric keypad 6 key\n" 
        "  VK_NUMPAD7                    103                 Numeric keypad 7 key\n" 
        "  VK_NUMPAD8                    104                 Numeric keypad 8 key\n" 
        "  VK_NUMPAD9                    105                 Numeric keypad 9 key\n" 
        "  VK_MULTIPLY                   106                 Numeric keypad * key\n" 
        "  VK_ADD                        107                 Numeric keypad + key\n" 
        "  VK_SEPARATOR                  108                 Numeric keypad Enter key\n" 
        "  VK_SUBTRACT                   109                 Numeric keypad - key\n" 
        "  VK_DECIMAL                    110                 Numeric keypad . key\n" 
        "  VK_DIVIDE                     111                 Numeric keypad / key\n" 
        "  VK_F1                         112                 F1\n" 
        "  VK_F2                         113                 F2\n" 
        "  VK_F3                         114                 F3\n" 
        "  VK_F4                         115                 F4\n" 
        "  VK_F5                         116                 F5\n" 
        "  VK_F6                         117                 F6\n" 
        "  VK_F7                         118                 F7\n" 
        "  VK_F8                         119                 F8\n" 
        "  VK_F9                         120                 F9\n" 
        "  VK_F10                        121                 F10\n" 
        "  VK_F11                        122                 F11\n" 
        "  VK_F12                        123                 F12\n" 
        "  VK_F13                        124                 F13 \n" 
        "  VK_F14                        125                 F14\n" 
        "  VK_F15                        126                 F15\n" 
        "  VK_F16                        127                 F16\n" 
        "  VK_F17                        128                 F17\n" 
        "  VK_F18                        129                 F18\n" 
        "  VK_F19                        130                 F19\n" 
        "  VK_F20                        131                 F20\n" 
        "  VK_F21                        132                 F21\n" 
        "  VK_F22                        133                 F22\n" 
        "  VK_F23                        134                 F23\n" 
        "  VK_F24                        135                 F24\n" 
        "  VK_NUMLOCK                    144                 NUM LOCK\n" 
        "  VK_SCROLL                     145                 SCROLL LOCK\n" 
        "  VK_LSHIFT                     160                 Left SHIFT key\n" 
        "  VK_RSHIFT                     161                 Right SHIFT key\n" 
        "  VK_LCONTROL                   162                 Left CONTROL key\n" 
        "  VK_RCONTROL                   163                 Right CONTROL key\n" 
        "  VK_LMENU                      164                 Left ALT key\n" 
        "  VK_RMENU                      165                 Right ALT key\n" 
        "  VK_BROWSER_BACK               166                 Browser Back key\n" 
        "  VK_BROWSER_FORWARD            167                 Browser Forward key\n" 
        "  VK_BROWSER_REFRESH            168                 Browser Refresh key\n" 
        "  VK_BROWSER_STOP               169                 Browser Stop key\n" 
        "  VK_BROWSER_SEARCH             170                 Browser Search key\n" 
        "  VK_BROWSER_FAVORITES          171                 Browser Favorites key\n" 
        "  VK_BROWSER_HOME               172                 Browser Start and Home key\n" 
        "  VK_VOLUME_MUTE                173                 Volume Mute key\n" 
        "  VK_VOLUME_DOWN                174                 Volume Down key\n" 
        "  VK_VOLUME_UP                  175                 Volume Up key\n" 
        "  VK_MEDIA_NEXT_TRACK           176                 Next Track key\n" 
        "  VK_MEDIA_PREV_TRACK           177                 Previous Track key\n" 
        "  VK_MEDIA_STOP                 178                 Stop Media key\n" 
        "  VK_MEDIA_PLAY_PAUSE           179                 Play/Pause Media key\n" 
        "  VK_LAUNCH_MAIL                180                 Start Mail key\n" 
        "  VK_LAUNCH_MEDIA_SELECT        181                 Select Media key\n" 
        "  VK_LAUNCH_APP1                182                 Start Application 1 key\n" 
        "  VK_LAUNCH_APP2                183                 Start Application 2 key\n" 
        "  VK_OEM_1                      186                 ;:\n" 
        "  VK_OEM_PLUS                   187                 =+\n" 
        "  VK_OEM_COMMA                  188                 ,<\n" 
        "  VK_OEM_MINUS                  189                 -_\n" 
        "  VK_OEM_PERIOD                 190                 .>\n" 
        "  VK_OEM_2                      191                 /?\n" 
        "  VK_OEM_3                      192                 `~\n" 
        "  VK_OEM_4                      219                 [{\n" 
        "  VK_OEM_5                      220                 \\|\n" 
        "  VK_OEM_6                      221                 ]}\n" 
        "  VK_OEM_7                      222                 \'\"\n" 
        "  VK_OEM_8                      223                 \n" 
        "  VK_OEM_102                    226                 Either the angle bracket key or the backslash key on the RT 102-key keyboard\n" 
        "  VK_PACKET                     231                 Used to pass Unicode characters as if they were keystrokes\n" 
        "  VK_PROCESSKEY                 229                 IME PROCESS key\n" 
        "  VK_ATTN                       246                 Attn key\n" 
        "  VK_CRSEL                      247                 CrSel key\n" 
        "  VK_EXSEL                      248                 ExSel key\n" 
        "  VK_EREOF                      249                 Erase EOF key\n" 
        "  VK_PLAY                       250                 Play key\n" 
        "  VK_ZOOM                       251                 Zoom key\n" 
        "  VK_NONAME                     252                 \n" 
        "  VK_PA1                        253                 PA1 key\n" 
        "  VK_OEM_CLEAR                  254                 Clear key\n"    
    ;

    int nRet = 0;

    if (nRet == 0) {
        FPRINTF(stdout, "%s\n", sAllVksInfo);
    }

    return nRet;
}

int CommandHandler_Delay(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    unsigned int milliSecond = 0;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "Delay");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        milliSecond = TK_Tools::StrToUL(arguments[2].str);
    }

    if (nRet == 0) {
        toolkit.Delay(milliSecond);
    }

    return nRet;
}

int CommandHandler_cbdPutStr(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string s;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "Delay");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        s = arguments[2].str;
    }

    if (nRet == 0) {
        if (!toolkit.cbdPutStr(s)) {
            nRet = 1;
        }
    }

    return nRet;
}

int CommandHandler_cbdGetStr(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string s;

    if (nRet == 0) {
        if (!toolkit.cbdGetStr(s)) {
            nRet = 1;
        } else {
            FPRINTF(stdout, "%s\n", s.c_str());
        }
    }

    return nRet;
}

int CommandHandler_kbdKeyDown(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    unsigned char vk = 0;
    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "kbdKeyDown");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseVkFromStr(vk, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "kbdKeyDown", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.kbdKeyDown(vk);
    }

    return nRet;
}

int CommandHandler_kbdKeyUp(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    unsigned char vk = 0;
        
    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "kbdKeyUp");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseVkFromStr(vk, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "kbdKeyUp", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.kbdKeyUp(vk);
    }

    return nRet;
}

int CommandHandler_kbdKeyOn(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    unsigned char vk = 0;
        
    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "kbdKeyOn");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseVkFromStr(vk, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "kbdKeyOn", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.kbdKeyOn(vk);
    }

    return nRet;
}

int CommandHandler_kbdKeyOff(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    unsigned char vk = 0;
    
    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "kbdKeyOff");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseVkFromStr(vk, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "kbdKeyOff", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.kbdKeyOff(vk);
    }

    return nRet;
}

int CommandHandler_kbdCtrlA(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;

    if (nRet == 0) {
        toolkit.kbdCtrlA();
    }

    return nRet;
}

int CommandHandler_kbdCtrlC(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;

    if (nRet == 0) {
        toolkit.kbdCtrlC();
    }

    return nRet;
}

int CommandHandler_kbdCtrlX(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;

    if (nRet == 0) {
        toolkit.kbdCtrlX();
    }

    return nRet;
}

int CommandHandler_kbdCtrlV(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;

    if (nRet == 0) {
        toolkit.kbdCtrlV();
    }

    return nRet;
}

int CommandHandler_kbdChar(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    unsigned char ch = 0;
    
    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "kbdChar");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        ch = arguments[2].str[0];
    }

    if (nRet == 0) {
        toolkit.kbdChar(ch);
    }

    return nRet;
}

int CommandHandler_kbdStr(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string s;
    
    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "kbdStr");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        s = arguments[2].str;
    }

    if (nRet == 0) {
        toolkit.kbdStr(s);
    }

    return nRet;
}

int CommandHandler_mseMove(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    GuiISTk::Point point;
    bool absolute = true;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "mseMove");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(point, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseMove", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (arguments.size() >= 4) {
            if (!parseBoolFromStr(absolute, arguments[3].str)) {
                FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseMove", arguments[3].str.c_str());
                nRet = 1;
            }
        }
    }

    if (nRet == 0) {
        toolkit.mseMove(point, absolute);
    }

    return nRet;
}

int CommandHandler_mseClick(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    GuiISTk::Point point;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "mseClick");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(point, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseClick", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.mseClick(point);
    }

    return nRet;
}

int CommandHandler_mseDClick(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    GuiISTk::Point point;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "mseDClick");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(point, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseDClick", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.mseDClick(point);
    }

    return nRet;
}

int CommandHandler_mseDrag(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    GuiISTk::Point srcPoint, dstPoint;

    if (nRet == 0) {
        if (arguments.size() < 4) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "mseDrag");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(srcPoint, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseDrag", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(dstPoint, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseDrag", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.mseDrag(srcPoint, dstPoint);
    }

    return nRet;
}

int CommandHandler_mseRClick(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    GuiISTk::Point point;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "mseRClick");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(point, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseRClick", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.mseRClick(point);
    }

    return nRet;
}

int CommandHandler_mseDRClick(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    GuiISTk::Point point;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "mseDRClick");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(point, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseDRClick", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.mseDRClick(point);
    }

    return nRet;
}

int CommandHandler_mseRDrag(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    GuiISTk::Point srcPoint, dstPoint;

    if (nRet == 0) {
        if (arguments.size() < 4) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "mseRDrag");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(srcPoint, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseRDrag", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(dstPoint, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseRDrag", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        toolkit.mseRDrag(srcPoint, dstPoint);
    }

    return nRet;
}

int CommandHandler_mseScroll(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    GuiISTk::Point point;
    int steps = 0;

    if (nRet == 0) {
        if (arguments.size() < 4) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "mseScroll");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(point, arguments[2].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "mseScroll", arguments[2].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        steps = (int)TK_Tools::StrToL(arguments[3].str);
    }

    if (nRet == 0) {
        toolkit.mseScroll(point, steps);
    }

    return nRet;
}

int CommandHandler_imgFindRect1(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::vector<GuiISTk::Image> images;
    GuiISTk::Rect rect;
    int index = -1;
    int i;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "imgFindRect1");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseImagesFromStr(images, arguments[2].str)) {
            nRet = 1;
        }
    }

    if (nRet == 0) {
        for (i = 0; i < (int)images.size(); ++i) {
            if (!TK_Tools::FileExists(images[i].getPath())) {
                FPRINTF(stderr, "*** Error: %s: the file does not exist: %s\n", "imgFindRect1", images[i].getPath().c_str());
                nRet = 1;
                break;
            }
        }
    }

    if (nRet == 0) {
        if (toolkit.imgFindRect(images, rect, index)) {
            FPRINTF(stdout, "%d %d %u %u %d\n", rect.x, rect.y, rect.width, rect.height, index);
        } else {
            nRet = 1;
            FPRINTF(stderr, "not found!\n");
        }
    }

    return nRet;
}

int CommandHandler_imgFindRect2(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::vector<GuiISTk::Image> images;
    GuiISTk::Rect rect;
    GuiISTk::Rect searchRect;
    int index = -1;
    int i;

    if (nRet == 0) {
        if (arguments.size() < 4) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "imgFindRect2");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseImagesFromStr(images, arguments[2].str)) {
            nRet = 1;
        }
    }

    if (nRet == 0) {
        for (i = 0; i < (int)images.size(); ++i) {
            if (!TK_Tools::FileExists(images[i].getPath())) {
                FPRINTF(stderr, "*** Error: %s: the file does not exist: %s\n", "imgFindRect2", images[i].getPath().c_str());
                nRet = 1;
                break;
            }
        }
    }

    if (nRet == 0) {
        if (!parseRectFromStr(searchRect, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "imgFindRect2", arguments[4].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (toolkit.imgFindRect(images, rect, index, searchRect)) {
            FPRINTF(stdout, "%d %d %u %u %d\n", rect.x, rect.y, rect.width, rect.height, index);
        } else {
            nRet = 1;
            FPRINTF(stderr, "not found!\n");
        }
    }

    return nRet;
}

int CommandHandler_imgFindRect3(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::vector<GuiISTk::Image> images;
    GuiISTk::Rect rect;
    GuiISTk::Point searchBeginningPoint;
    int index = -1;
    int i;

    if (nRet == 0) {
        if (arguments.size() < 4) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "imgFindRect3");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseImagesFromStr(images, arguments[2].str)) {
            nRet = 1;
        }
    }

    if (nRet == 0) {
        for (i = 0; i < (int)images.size(); ++i) {
            if (!TK_Tools::FileExists(images[i].getPath())) {
                FPRINTF(stderr, "*** Error: %s: the file does not exist: %s\n", "imgFindRect3", images[i].getPath().c_str());
                nRet = 1;
                break;
            }
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(searchBeginningPoint, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "imgFindRect3", arguments[4].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (toolkit.imgFindRect(images, rect, index, searchBeginningPoint)) {
            FPRINTF(stdout, "%d %d %u %u %d\n", rect.x, rect.y, rect.width, rect.height, index);
        } else {
            nRet = 1;
            FPRINTF(stderr, "not found!\n");
        }
    }

    return nRet;
}

int CommandHandler_imgFindRect(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    if (arguments.size() < 4) {
        return CommandHandler_imgFindRect1(arguments, toolkit);
    } else {
        if (arguments[3].strArray.size() == 4) {
            return CommandHandler_imgFindRect2(arguments, toolkit);
        } else if (arguments[3].strArray.size() == 2) {
            return CommandHandler_imgFindRect3(arguments, toolkit);
        } else {
            return CommandHandler_imgFindRect1(arguments, toolkit);
        }
    }
}

int CommandHandler_imgWaitShown1(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::vector<GuiISTk::Image> images;
    unsigned int timeout = GuiISTk::INFINITE_TIME;
    GuiISTk::Rect rect;
    int index = -1;
    int i;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "imgWaitShown1");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseImagesFromStr(images, arguments[2].str)) {
            nRet = 1;
        }
    }

    if (nRet == 0) {
        for (i = 0; i < (int)images.size(); ++i) {
            if (!TK_Tools::FileExists(images[i].getPath())) {
                FPRINTF(stderr, "*** Error: %s: the file does not exist: %s\n", "imgWaitShown1", images[i].getPath().c_str());
                nRet = 1;
                break;
            }
        }
    }

    if (nRet == 0) {
        if (arguments.size() >= 4) {
            timeout = (unsigned int)TK_Tools::StrToUL(arguments[3].str);
        }
    }

    if (nRet == 0) {
        if (!toolkit.imgWaitShown(images, rect, index, timeout)) {
            nRet = 1;
        } else {
            FPRINTF(stdout, "%d %d %u %u %d\n", rect.x, rect.y, rect.width, rect.height, index);
        }
    }

    return nRet;
}

int CommandHandler_imgWaitShown2(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::vector<GuiISTk::Image> images;
    GuiISTk::Rect searchRect;
    unsigned int timeout = GuiISTk::INFINITE_TIME;
    GuiISTk::Rect rect;
    int index = -1;
    int i;

    if (nRet == 0) {
        if (arguments.size() < 4) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "imgWaitShown2");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseImagesFromStr(images, arguments[2].str)) {
            nRet = 1;
        }
    }

    if (nRet == 0) {
        for (i = 0; i < (int)images.size(); ++i) {
            if (!TK_Tools::FileExists(images[i].getPath())) {
                FPRINTF(stderr, "*** Error: %s: the file does not exist: %s\n", "imgWaitShown2", images[i].getPath().c_str());
                nRet = 1;
                break;
            }
        }
    }

    if (nRet == 0) {
        if (!parseRectFromStr(searchRect, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "imgWaitShown2", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (arguments.size() >= 5) {
            timeout = (unsigned int)TK_Tools::StrToUL(arguments[4].str);
        }
    }

    if (nRet == 0) {
        if (!toolkit.imgWaitShown(images, rect, index, searchRect, timeout)) {
            nRet = 1;
        } else {
            FPRINTF(stdout, "%d %d %u %u %d\n", rect.x, rect.y, rect.width, rect.height, index);
        }
    }

    return nRet;
}

int CommandHandler_imgWaitShown3(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::vector<GuiISTk::Image> images;
    GuiISTk::Point searchBeginningPoint;
    unsigned int timeout = GuiISTk::INFINITE_TIME;
    GuiISTk::Rect rect;
    int index = -1;
    int i;

    if (nRet == 0) {
        if (arguments.size() < 4) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "imgWaitShown3");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseImagesFromStr(images, arguments[2].str)) {
            nRet = 1;
        }
    }

    if (nRet == 0) {
        for (i = 0; i < (int)images.size(); ++i) {
            if (!TK_Tools::FileExists(images[i].getPath())) {
                FPRINTF(stderr, "*** Error: %s: the file does not exist: %s\n", "imgWaitShown3", images[i].getPath().c_str());
                nRet = 1;
                break;
            }
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(searchBeginningPoint, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "imgWaitShown3", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (arguments.size() >= 5) {
            timeout = (unsigned int)TK_Tools::StrToUL(arguments[4].str);
        }
    }

    if (nRet == 0) {
        if (!toolkit.imgWaitShown(images, rect, index, searchBeginningPoint, timeout)) {
            nRet = 1;
        } else {
            FPRINTF(stdout, "%d %d %u %u %d\n", rect.x, rect.y, rect.width, rect.height, index);
        }
    }

    return nRet;
}

int CommandHandler_imgWaitShown(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    if (arguments.size() < 4) {
        return CommandHandler_imgWaitShown1(arguments, toolkit);
    } else {
        if (arguments[3].strArray.size() == 4) {
            return CommandHandler_imgWaitShown2(arguments, toolkit);
        } else if (arguments[3].strArray.size() == 2) {
            return CommandHandler_imgWaitShown3(arguments, toolkit);
        } else {
            return CommandHandler_imgWaitShown1(arguments, toolkit);
        }
    }
}

int CommandHandler_scnCount(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string sTitle;
    bool bFullMatched = false;
    int nScreenCount = 0;

    if (nRet == 0) {
        if (arguments.size() < 4) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "scnCount");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sTitle = arguments[2].str;
    }
    
    if (nRet == 0) {
        if (!parseBoolFromStr(bFullMatched, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnCount", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        nScreenCount = toolkit.scnCount(GuiISTk::ScreenInfo(sTitle, bFullMatched, true));
        FPRINTF(stdout, "%u\n", nScreenCount);
    }

    return nRet;
}

int CommandHandler_scnShow(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string sTitle;
    bool bFullMatched = false;
    bool allMatched = false;
    GuiISTk::ScreenShowingMode mode;

    if (nRet == 0) {
        if (arguments.size() < 6) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "scnShow");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sTitle = arguments[2].str;
    }
    
    if (nRet == 0) {
        if (!parseBoolFromStr(bFullMatched, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnShow", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseBoolFromStr(allMatched, arguments[4].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnShow", arguments[4].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseScreenShowingModeFromStr(mode, arguments[5].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnShow", arguments[5].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!toolkit.scnShow(GuiISTk::ScreenInfo(sTitle, bFullMatched, allMatched), mode)) {
            nRet = 1;
        }
    }

    return nRet;
}

int CommandHandler_scnHide(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string sTitle;
    bool bFullMatched = false;
    bool allMatched = false;

    if (nRet == 0) {
        if (arguments.size() < 5) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "scnHide");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sTitle = arguments[2].str;
    }
    
    if (nRet == 0) {
        if (!parseBoolFromStr(bFullMatched, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnHide", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseBoolFromStr(allMatched, arguments[4].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnHide", arguments[4].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!toolkit.scnHide(GuiISTk::ScreenInfo(sTitle, bFullMatched, allMatched))) {
            nRet = 1;
        }
    }

    return nRet;
}

int CommandHandler_scnClose(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string sTitle;
    bool bFullMatched = false;
    bool allMatched = false;

    if (nRet == 0) {
        if (arguments.size() < 5) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "scnClose");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sTitle = arguments[2].str;
    }
    
    if (nRet == 0) {
        if (!parseBoolFromStr(bFullMatched, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnClose", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseBoolFromStr(allMatched, arguments[4].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnClose", arguments[4].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!toolkit.scnClose(GuiISTk::ScreenInfo(sTitle, bFullMatched, allMatched))) {
            nRet = 1;
        }
    }

    return nRet;
}

int CommandHandler_scnMove(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string sTitle;
    bool bFullMatched = false;
    bool allMatched = false;
    GuiISTk::Point point;

    if (nRet == 0) {
        if (arguments.size() < 6) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "scnMove");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sTitle = arguments[2].str;
    }
    
    if (nRet == 0) {
        if (!parseBoolFromStr(bFullMatched, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnMove", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseBoolFromStr(allMatched, arguments[4].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnMove", arguments[4].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parsePointFromStr(point, arguments[5].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnMove", arguments[5].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!toolkit.scnMove(GuiISTk::ScreenInfo(sTitle, bFullMatched, allMatched), point)) {
            nRet = 1;
        }
    }

    return nRet;
}

int CommandHandler_scnResize(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string sTitle;
    bool bFullMatched = false;
    bool allMatched = false;
    GuiISTk::Rect rect;

    if (nRet == 0) {
        if (arguments.size() < 6) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "scnResize");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sTitle = arguments[2].str;
    }
    
    if (nRet == 0) {
        if (!parseBoolFromStr(bFullMatched, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnResize", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseBoolFromStr(allMatched, arguments[4].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnResize", arguments[4].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseRectFromStr(rect, arguments[5].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnResize", arguments[5].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!toolkit.scnResize(GuiISTk::ScreenInfo(sTitle, bFullMatched, allMatched), rect)) {
            nRet = 1;
        }
    }

    return nRet;
}

int CommandHandler_scnSetZorder(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string sTitle;
    bool bFullMatched = false;
    bool allMatched = false;
    GuiISTk::ScreenZorder zorder;

    if (nRet == 0) {
        if (arguments.size() < 6) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "scnSetZorder");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sTitle = arguments[2].str;
    }
    
    if (nRet == 0) {
        if (!parseBoolFromStr(bFullMatched, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnSetZorder", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseBoolFromStr(allMatched, arguments[4].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnSetZorder", arguments[4].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseScreenZorderFromStr(zorder, arguments[5].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnSetZorder", arguments[5].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!toolkit.scnSetZorder(GuiISTk::ScreenInfo(sTitle, bFullMatched, allMatched), zorder)) {
            nRet = 1;
        }
    }

    return nRet;
}

int CommandHandler_scnSaveAsPics(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string sTitle;
    bool bFullMatched = false;
    bool allMatched = false;
    std::string sPictureFilePath;

    if (nRet == 0) {
        if (arguments.size() < 6) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "scnSaveAsPics");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sTitle = arguments[2].str;
    }
    
    if (nRet == 0) {
        if (!parseBoolFromStr(bFullMatched, arguments[3].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnSaveAsPics", arguments[3].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        if (!parseBoolFromStr(allMatched, arguments[4].str)) {
            FPRINTF(stderr, "*** Error: %s: wrong format of argument: %s\n", "scnSaveAsPics", arguments[4].str.c_str());
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sPictureFilePath = arguments[5].str;
    }

    if (nRet == 0) {
        if (!toolkit.scnSaveAsPics(GuiISTk::ScreenInfo(sTitle, bFullMatched, allMatched), sPictureFilePath)) {
            nRet = 1;
        }
    }

    return nRet;
}

int CommandHandler_scnSaveDesktopAsPic(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit)
{
    int nRet = 0;
    std::string sPictureFilePath;

    if (nRet == 0) {
        if (arguments.size() < 3) {
            FPRINTF(stderr, "*** Error: %s: too few argument!\n", "scnSaveDesktopAsPic");
            nRet = 1;
        }
    }

    if (nRet == 0) {
        sPictureFilePath = arguments[2].str;
    }

    if (nRet == 0) {
        if (!toolkit.scnSaveDesktopAsPic(sPictureFilePath)) {
            nRet = 1;
        }
    }

    return nRet;
}

class LibraryInitializer
{
public:
    LibraryInitializer()
    {
        Gdiplus::GdiplusStartupInput StartupInput;
        GdiplusStartup(&gdiplusToken, &StartupInput,NULL);

        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != NO_ERROR) {
            fprintf(stdout, "*** Error at WSAStartup()\n");
        }
    }

    ~LibraryInitializer()
    {
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }
private:
    ULONG_PTR gdiplusToken;
};

int main_local(int argc, char* argv[])
{
    int nRet = 0;
    std::vector<Argument> arguments = build_argument_array(argc, argv);
    const struct {
        const char *sCommond;
        int (*commandHandler)(const std::vector<Argument> &arguments, GuiISTk::IToolkit &toolkit);
    } commandMap[] = {
        COMMAND_HANDLER_PAIR(Delay),
        COMMAND_HANDLER_PAIR(scnCount),
        COMMAND_HANDLER_PAIR(scnShow),
        COMMAND_HANDLER_PAIR(scnHide),
        COMMAND_HANDLER_PAIR(scnClose),
        COMMAND_HANDLER_PAIR(scnMove),
        COMMAND_HANDLER_PAIR(scnResize),
        COMMAND_HANDLER_PAIR(scnSetZorder),
        COMMAND_HANDLER_PAIR(scnSaveAsPics),
        COMMAND_HANDLER_PAIR(scnSaveDesktopAsPic),
        COMMAND_HANDLER_PAIR(cbdPutStr),
        COMMAND_HANDLER_PAIR(cbdGetStr),
        COMMAND_HANDLER_PAIR(kbdVkList),
        COMMAND_HANDLER_PAIR(kbdKeyDown),
        COMMAND_HANDLER_PAIR(kbdKeyUp),
        COMMAND_HANDLER_PAIR(kbdKeyOn),
        COMMAND_HANDLER_PAIR(kbdKeyOff),
        COMMAND_HANDLER_PAIR(kbdCtrlA),
        COMMAND_HANDLER_PAIR(kbdCtrlC),
        COMMAND_HANDLER_PAIR(kbdCtrlX),
        COMMAND_HANDLER_PAIR(kbdCtrlV),
        COMMAND_HANDLER_PAIR(kbdChar),
        COMMAND_HANDLER_PAIR(kbdStr),
        COMMAND_HANDLER_PAIR(mseMove),
        COMMAND_HANDLER_PAIR(mseClick),
        COMMAND_HANDLER_PAIR(mseDClick),
        COMMAND_HANDLER_PAIR(mseDrag),
        COMMAND_HANDLER_PAIR(mseRClick),
        COMMAND_HANDLER_PAIR(mseDRClick),
        COMMAND_HANDLER_PAIR(mseRDrag),
        COMMAND_HANDLER_PAIR(mseScroll),
        COMMAND_HANDLER_PAIR(imgFindRect),
        COMMAND_HANDLER_PAIR(imgWaitShown),
    };
    unsigned int commandMapCount = sizeof(commandMap) / sizeof(commandMap[0]);
    unsigned int i;
    GuiISTk::WinGuiISTK winGuiISTK;
    
    if (argc < 2) {
        nRet = 1;
        help(argc, argv);
        goto END;
    }

    for (i = 0; i < commandMapCount; ++i) {
        if (TK_Tools::LowerCase(std::string(commandMap[i].sCommond)) == TK_Tools::LowerCase(arguments[1].str)) {
            nRet = commandMap[i].commandHandler(arguments, winGuiISTK);
            if (nRet != 0) {
                goto END;
            } else {
                goto END;
            }
        }
    }
    
    nRet = 1;
    FPRINTF(stderr, "*** Error: unknown command: %s\n", arguments[1].str.c_str());
    goto END;

END:
    return nRet;
}

int main(int argc, char* argv[])
{
    LibraryInitializer libraryInitializer;
    int nRet = 0;
    const char *pRemoteServerIP = getenv("REMOTE_SERVER_IP");
    const char *pRemoteServerPort = getenv("REMOTE_SERVER_PORT");

    if (argc >= 2 && TK_Tools::LowerCase(argv[1]) == TK_Tools::LowerCase("RemoteServer")) {
        std::string sRemoteServerIP = "0.0.0.0";
        unsigned short nRemoteServerPort = 8888;

        if (argc >= 3) {
            nRemoteServerPort = (unsigned short)TK_Tools::StrToUL(argv[2]);
        }
        if (argc >= 4) {
            sRemoteServerIP = argv[3];
        }
        
        RemoteCmd::RemoteCmdServer remoteCmdServer(sRemoteServerIP, nRemoteServerPort);

        if (nRet == 0) {
            if (!remoteCmdServer.SetCmdLineHandlerProc(&main_local)) {
                nRet = 1;
            }
        }

        if (nRet == 0) {
            fprintf(stderr, "RemoteCmdServer is running!\n");
            if (!remoteCmdServer.Run()) {
                nRet = 1;
            }
        }
    } else if (pRemoteServerIP != NULL && pRemoteServerPort != NULL) {
        unsigned short nRemoteServerPort = (unsigned short)TK_Tools::StrToUL(pRemoteServerPort);
        std::string sRemoteServerIP = pRemoteServerIP;

        RemoteCmd::RemoteCmdClient remoteCmdClient(sRemoteServerIP, nRemoteServerPort);

        if (nRet == 0) {
            if (!remoteCmdClient.ExecCmdLineOnServer(argc, argv, nRet)) {
                nRet = 1;
            }
        }
    } else {
        nRet = main_local(argc, argv);
    }

    return nRet;
}
