/**
* @file
*          dynamiclibrary.h
* @brief
*          Header file
* @author  (last changes):
*          - Elmar Weber
*          - elmar.weber@continental-corporation.com
*          - Continental AG
* @par Project:
* @par SW-Package:
*
* @par SW-Module:
*
* @note
*
* @par Module-History:
*  Date        Author                   Reason
*
* @par Copyright Notice:
* Copyright (C) Continental AG 2016
* Alle Rechte vorbehalten. All Rights Reserved.
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
*/


#ifndef DYNAMIC_LIBRARY_H__
#define DYNAMIC_LIBRARY_H__


#include "operatingsystem.h"
#include <string>


#if defined(OS_WINDOWS)
#include "dynamiclibrary_WIN32.h"
#else
#include "dynamiclibrary_POSIX.h"
#endif


class Lib_API DynamicLibrary : private DynamicLibraryImpl
{
public:
	enum Flags
	{
		GLOBAL = 1,
		LOCAL  = 2
	};

	DynamicLibrary();
	DynamicLibrary(const std::string& path);
	DynamicLibrary(const std::string& path, int flags);

	virtual ~DynamicLibrary();

  bool isLoaded() const;
  bool loadLibrary(const std::string& path);
  bool loadLibrary(const std::string& path, int flags);
	void unloadLibrary();

	bool hasSymbol(const std::string& name);
	void* getSymbol(const std::string& name);

  const std::string& getPath() const;
  static std::string getFileExtension();

protected:
  //do not implement!
	DynamicLibrary(const DynamicLibrary& rhs);
	DynamicLibrary& operator = (const DynamicLibrary& rhs);
};

#endif // DYNAMIC_LIBRARY_H__
