/**
* @file
*          dynamiclibrary_WIN32.h
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


#ifndef DYNAMIC_LIBRARY_WIN32_H__
#define DYNAMIC_LIBRARY_WIN32_H__


#include "mutex.h"
#include <string>


class Lib_API DynamicLibraryImpl
{
protected:
  DynamicLibraryImpl();
  ~DynamicLibraryImpl();

  bool isLoadedImpl() const;
  bool loadLibraryImpl(const std::string& path, int flags);
  void unloadLibraryImpl();

	void* findSymbolImpl(const std::string& name);
	const std::string& getPathImpl() const;

  static std::string getFileExtensionImpl();

private:
	std::string   m_sFilePath;
	void*         m_handle;
	Mutex         m_mutex;
};

#endif // DYNAMIC_LIBRARY_WIN32_H__
