#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "sys_flag_mgr.hpp"

#ifndef UNUSED
#define UNUSED(var) (&var)
#endif // #ifndef UNUSED

enum ERROR_CODE {
    ERROR_CODE_NO_ERROR = 0,
    ERROR_CODE_TOO_FEW_ARGS,
    ERROR_CODE_INVALID_CMD_OR_ARGS,
};

static std::string trim(const std::string &str, const std::string& whitespaces /*= " \t"*/)
{
    const auto strBegin = str.find_first_not_of(whitespaces);
    if (strBegin == std::string::npos) {
        return std::string("");
    }

    const auto strEnd = str.find_last_not_of(whitespaces);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

static std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

static std::vector<std::string> splitWithTrim(const std::string& s, char delimiter, const std::string& whitespaces /*= " \t"*/)
{
    std::vector<std::string> rss;
    std::vector<std::string> ss = split(s, delimiter);

    for (std::string &s : ss) {
        s = trim(s, whitespaces);
        if (!s.empty()) {
            rss.push_back(s);
        }
    }

    return rss;
}

static void ItemValueChanged(const std::string &name, const SysFlagMgr::Value &value)
{
    switch (value.type) {
        case SysFlagMgr::Value::Type::I32:
            {
                printf("ItemValueChanged(\"%s\", I32, %d)\n", name.c_str(), value.value.intValue.i32);
            }
            break;
        case SysFlagMgr::Value::Type::U32:
            {
                printf("ItemValueChanged(\"%s\", U32, %u)\n", name.c_str(), value.value.intValue.u32);
            }
            break;
        case SysFlagMgr::Value::Type::S16:
             {
                printf("ItemValueChanged(\"%s\", S16, \"%s\")\n", name.c_str(), value.value.strValue.c_str());
            }
            break;
        case SysFlagMgr::Value::Type::S32:
             {
                printf("ItemValueChanged(\"%s\", S32, \"%s\")\n", name.c_str(), value.value.strValue.c_str());
            }
            break;
        case SysFlagMgr::Value::Type::S64:
            {
                printf("ItemValueChanged(\"%s\", S64, \"%s\")\n", name.c_str(), value.value.strValue.c_str());
            }
            break;
        default:
            {
                printf("ItemValueChanged(\"%s\", UNKNOWN_TYPE)\n", name.c_str());
            }
            break;
    }
}

static void usage(const std::string &argv0)
{
    printf("Usage:\n");

    if (argv0 != " ") {
        printf("  %s [-i] [-h|-H|--help]\n", argv0.c_str());
    } else {
        printf("  %s exit|quit\n", argv0.c_str());
    }

    printf("  %s SetItemValue_I32 <item_name> <item_value>\n", argv0.c_str());
    printf("  %s GetItemValue_I32 <item_name>\n", argv0.c_str());
    printf("  %s WatchItem_I32 <item_name>\n", argv0.c_str());

    printf("  %s SetItemValue_U32 <item_name> <item_value>\n", argv0.c_str());
    printf("  %s GetItemValue_U32 <item_name>\n", argv0.c_str());
    printf("  %s WatchItem_U32 <item_name>\n", argv0.c_str());

    printf("  %s SetItemValue_S16 <item_name> <item_value>\n", argv0.c_str());
    printf("  %s GetItemValue_S16 <item_name>\n", argv0.c_str());
    printf("  %s WatchItem_S16 <item_name>\n", argv0.c_str());

    printf("  %s SetItemValue_S32 <item_name> <item_value>\n", argv0.c_str());
    printf("  %s GetItemValue_S32 <item_name>\n", argv0.c_str());
    printf("  %s WatchItem_S32 <item_name>\n", argv0.c_str());

    printf("  %s SetItemValue_S64 <item_name> <item_value>\n", argv0.c_str());
    printf("  %s GetItemValue_S64 <item_name>\n", argv0.c_str());
    printf("  %s WatchItem_S64 <item_name>\n", argv0.c_str());

    printf("\n");
}

static int handleCmdLine(const std::string &argv0, std::vector<std::string> args)
{
    SysFlagMgr &sysFlagManager = SysFlagMgr::GetInstance();
    std::string itemName;
    
    UNUSED(argv0);
    
    if (args.size() == 0) {
        return ERROR_CODE_NO_ERROR;
    }
    else if (args[0] == "h" || args[0] == "H" || args[0] == "help") {
        usage(" ");
        return ERROR_CODE_NO_ERROR;
    }

    //
    // for I32
    //
    else if (args[0] == "GetItemValue_I32" && args.size() >= 2) {
        itemName = args[1];
        std::int32_t value = sysFlagManager.GetItemValue_I32(itemName);
        printf("GetItemValue_I32(\"%s\"): %d\n", itemName.c_str(), value);
    }
    else if (args[0] == "SetItemValue_I32" && args.size() >= 3) {
        itemName = args[1];
        std::string strValue = args[2];
        std::int32_t value = std::strtol(strValue.c_str(), NULL, 0);
        printf("SetItemValue_I32(\"%s\", %d)\n", itemName.c_str(), value);
        sysFlagManager.SetItemValue_I32(itemName, value);
    }
    else if (args[0] == "WatchItem_I32" && args.size() >= 2) {
        itemName = args[1];
        printf("WatchItem_I32(\"%s\")\n", itemName.c_str());
        sysFlagManager.SetItemValueChangedCallback_I32(itemName, &ItemValueChanged);
    }

    //
    // for U32
    //
    else if (args[0] == "GetItemValue_U32" && args.size() >= 2) {
        itemName = args[1];
        std::uint32_t value = sysFlagManager.GetItemValue_U32(itemName);
        printf("GetItemValue_U32(\"%s\"): %u\n", itemName.c_str(), value);
    }
    else if (args[0] == "SetItemValue_U32" && args.size() >= 3) {
        itemName = args[1];
        std::string strValue = args[2];
        std::uint32_t value = std::strtoul(strValue.c_str(), NULL, 0);
        printf("SetItemValue_U32(\"%s\", %u)\n", itemName.c_str(), value);
        sysFlagManager.SetItemValue_U32(itemName, value);
    }
    else if (args[0] == "WatchItem_U32" && args.size() >= 2) {
        itemName = args[1];
        printf("WatchItem_U32(\"%s\")\n", itemName.c_str());
        sysFlagManager.SetItemValueChangedCallback_U32(itemName, &ItemValueChanged);
    }

    //
    // for S16
    //
    else if (args[0] == "GetItemValue_S16" && args.size() >= 2) {
        itemName = args[1];
        std::string value = sysFlagManager.GetItemValue_S16(itemName);
        printf("GetItemValue_S16(\"%s\"): \"%s\"\n", itemName.c_str(), value.c_str());
    }
    else if (args[0] == "SetItemValue_S16" && args.size() >= 3) {
        itemName = args[1];
        std::string strValue = args[2];
        std::string value = strValue;
        printf("SetItemValue_S16(\"%s\", \"%s\")\n", itemName.c_str(), value.c_str());
        sysFlagManager.SetItemValue_S16(itemName, value);
    }
    else if (args[0] == "WatchItem_S16" && args.size() >= 2) {
        itemName = args[1];
        printf("WatchItem_S16(\"%s\")\n", itemName.c_str());
        sysFlagManager.SetItemValueChangedCallback_S16(itemName, &ItemValueChanged);
    }

    //
    // for S32
    //
    else if (args[0] == "GetItemValue_S32" && args.size() >= 2) {
        itemName = args[1];
        std::string value = sysFlagManager.GetItemValue_S32(itemName);
        printf("GetItemValue_S32(\"%s\"): \"%s\"\n", itemName.c_str(), value.c_str());
    }
    else if (args[0] == "SetItemValue_S32" && args.size() >= 3) {
        itemName = args[1];
        std::string strValue = args[2];
        std::string value = strValue;
        printf("SetItemValue_S32(\"%s\", \"%s\")\n", itemName.c_str(), value.c_str());
        sysFlagManager.SetItemValue_S32(itemName, value);
    }
    else if (args[0] == "WatchItem_S32" && args.size() >= 2) {
        itemName = args[1];
        printf("WatchItem_S32(\"%s\")\n", itemName.c_str());
        sysFlagManager.SetItemValueChangedCallback_S32(itemName, &ItemValueChanged);
    }

    //
    // for S64
    //
    else if (args[0] == "GetItemValue_S64" && args.size() >= 2) {
        itemName = args[1];
        std::string value = sysFlagManager.GetItemValue_S64(itemName);
        printf("GetItemValue_S64(\"%s\"): \"%s\"\n", itemName.c_str(), value.c_str());
    }
    else if (args[0] == "SetItemValue_S64" && args.size() >= 3) {
        itemName = args[1];
        std::string strValue = args[2];
        std::string value = strValue;
        printf("SetItemValue_S64(\"%s\", \"%s\")\n", itemName.c_str(), value.c_str());
        sysFlagManager.SetItemValue_S64(itemName, value);
    }
    else if (args[0] == "WatchItem_S64" && args.size() >= 2) {
        itemName = args[1];
        printf("WatchItem_S64(\"%s\")\n", itemName.c_str());
        sysFlagManager.SetItemValueChangedCallback_S64(itemName, &ItemValueChanged);
    }
    
    else {
        printf("*** Invalid commands or arguments!\n");
        return ERROR_CODE_INVALID_CMD_OR_ARGS;
    }
    
    return ERROR_CODE_NO_ERROR;
}

int main(int argc, char* argv[])
{
    int ret = ERROR_CODE_NO_ERROR;
    std::vector<std::string> args;

    SysFlagMgr::GetInstance();
 
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    if (args.size() == 0 || args[0] == "-h" || args[0] == "-H" || args[0] == "--help") {
        usage(argv[0]);
        return ERROR_CODE_NO_ERROR;
    }
    
    if (args[0] == "-i") {
        while (true) {
            printf("> ");
            std::string line;
            std::getline(std::cin, line);
            args = splitWithTrim(line, ' ', " ");
            if (args.size() >= 1 && (args[0] == "exit" || args[0] == "quit")) {
                break;
            } else {
                ret = handleCmdLine(argv[0], args);
            }
        }
    } else {
        ret = handleCmdLine(argv[0], args);
    }
    
    return ret;
}