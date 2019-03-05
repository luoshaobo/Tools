// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s()]\n", __FILE__, __LINE__, __FUNCTION__); } while(0)
#define LOG_GEN_PRINTF(format,...) do { log_printf((std::string("=== LOG_GEN: [%s: %u: %s()] ")+format).c_str(), __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__); } while(0)
#define log_printf printf

#undef LOG_GEN
#define LOG_GEN()

#ifndef interface
#define interface struct
#endif

// TODO: reference additional headers your program requires here
