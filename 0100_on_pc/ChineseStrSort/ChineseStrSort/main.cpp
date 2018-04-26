// ChineseStrSort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <locale.h>
#include "UserConfig.h"
#include "UnicodePinyinTable.h"
#include "PhrasePinyinTable.h"
#include "TestClass001.h"
#include "TestClass002.h"

using namespace CSS;

void Tool_GenUnicodePingyinTableCppCode()
{
    ErrorCode ec = EC_OK;
    const char *generatedFilePath = "../UnicodePinyinTableData/generated/UnicodePingyinTableDataGenenrated.h";
    UnicodePinyinTable unicodePinyinTable;
    std::string cppCodeContent;
    bool suc;

    printf("Generate %s ...\n", generatedFilePath);

    if (ec == EC_OK) {
        ec = unicodePinyinTable.LoadData();
    }

    if (ec == EC_OK) {
        ec = unicodePinyinTable.GenCppCode(cppCodeContent);
    }

    if (ec == EC_OK) {
        suc = SaveToFile(generatedFilePath, cppCodeContent);
    }

    if (ec == EC_OK) {
        printf("successfully!\n");
    } else {
        printf("fail!\n");
    }
}

void Tool_GenPhrasePingyinTableCppCode()
{
    ErrorCode ec = EC_OK;
    const char *generatedFilePath = "../PhrasePinyinTableData/generated/PhrasePingyinTableDataGenenrated.h";
    PhrasePinyinTable phrasePinyinTable;
    std::string cppCodeContent;
    bool suc;

    printf("Generate %s ...\n", generatedFilePath);

    if (ec == EC_OK) {
        ec = phrasePinyinTable.LoadData();
    }

    if (ec == EC_OK) {
        ec = phrasePinyinTable.GenCppCode(cppCodeContent);
    }

    if (ec == EC_OK) {
        suc = SaveToFile(generatedFilePath, cppCodeContent);
    }

    if (ec == EC_OK) {
        printf("successfully!\n");
    } else {
        printf("fail!\n");
    }
}

void Test001_TypicalWords()
{
    TestClass001 testCase;

    testCase.ClearUserConfigFlags(USERCONF_SURNAME_FIRST);
    testCase.SetInputFilePath("input/input001_TypicalWords.txt");
    testCase.DoTest();
}

void Test001_LargeNameList()
{
    TestClass001 testCase;

    testCase.SetUserConfigFlags(USERCONF_SURNAME_FIRST);
    testCase.SetInputFilePath("input/input002_LargeNameList.txt");
    testCase.DoTest();
}

void Test001_LargeMusicList()
{
    TestClass001 testCase;

    testCase.SetUserConfigFlags(USERCONF_SURNAME_FIRST);
    testCase.SetInputFilePath("input/input003_LargeMusicList.txt");
    testCase.DoTest();
}

void Test002_RetrievePinyin_NonSurnameFirst()
{
    TestClass002 testCase;

    testCase.ClearUserConfigFlags(USERCONF_SURNAME_FIRST);
    testCase.DoTest();
}

void Test002_RetrievePinyin_SurnameFirst()
{
    TestClass002 testCase;

    testCase.SetUserConfigFlags(USERCONF_SURNAME_FIRST);
    testCase.DoTest();
}

void InitEnv()
{
#ifdef WIN32
    setlocale(LC_ALL,"chs");
#endif // #ifdef WIN32
}

static void PrintTestMenu()
{
    printf("=== Test Menu ===\n");
    printf("11    => Test001_TypicalWords\n");
    printf("12    => Test001_LargeNameList\n");
    printf("13    => Test001_LargeMusicList\n");
    printf("21    => Test002_RetrievePinyin_NonSurnameFirst\n");
    printf("22    => Test002_RetrievePinyin_SurnameFirst\n");
    printf("cls   => Clear screen\n");
    printf("h|?   => Show menu\n");
    printf("#     => Exit\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
    InitEnv();

    if (argc >=2 && std::wstring(argv[1]) == L"GenUnicodePingyinTableCppCode") {
        Tool_GenUnicodePingyinTableCppCode();
    } else if (argc >=2 && std::wstring(argv[1]) == L"GenPhrasePingyinTableCppCode") {
        Tool_GenPhrasePingyinTableCppCode();
    } else {
        UserConfig::GetInstance().SetFlag(USERCONF_UNICODEPINYIN_LOADED_FROM_PLUGIN);
        UserConfig::GetInstance().SetFlag(USERCONF_PHRASEPINYIN_LOADED_FROM_PLUGIN);
        
        PrintTestMenu();

        while (true) {
            char bufLine[1024];
            char *pLine;

            printf("Your choice: ");

            pLine = fgets(bufLine, TK_ARR_LEN(bufLine), stdin);
            std::string lineStr(pLine);
            lineStr = TrimAll(lineStr);
            if (lineStr.length() == 0) {
                continue;
            }

            lineStr = LowerCase(lineStr);
            if (lineStr == "#") {
                break;
            } else if (lineStr == "cls") {
                system("cls");
            } else if (lineStr == "h" || lineStr == "?") {
                PrintTestMenu();
            } else if (lineStr == "11") {
                Test001_TypicalWords();
            } else if (lineStr == "12") {
                Test001_LargeNameList();
            } else if (lineStr == "13") {
                Test001_LargeMusicList();
            } else if (lineStr == "21") {
                Test002_RetrievePinyin_NonSurnameFirst();
            } else if (lineStr == "22") {
                Test002_RetrievePinyin_SurnameFirst();
            }
        }
    }

	return 0;
}
