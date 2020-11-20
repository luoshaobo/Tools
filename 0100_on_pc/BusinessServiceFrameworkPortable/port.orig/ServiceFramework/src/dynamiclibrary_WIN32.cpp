#include "dynamiclibrary_WIN32.h"
#include "scopedlock.h"
#include "comalog.h"

//Mutex DynamicLibraryImpl::m_mutex;


DynamicLibraryImpl::DynamicLibraryImpl() :m_handle(0)
{
}


DynamicLibraryImpl::~DynamicLibraryImpl()
{
}


bool DynamicLibraryImpl::loadLibraryImpl(const std::string& path, int /*flags*/)
{
	ScopedLock scopedLock(m_mutex);

  bool bRet = false;

  if (m_handle == 0)
  {
    DWORD flags = 0;

    m_handle = LoadLibraryExA(path.c_str(), 0, flags);

    if (m_handle != 0)
    {
      m_sFilePath = path;
      bRet = true;
    }
  }

  return (bRet);
}


void DynamicLibraryImpl::unloadLibraryImpl()
{
  ScopedLock scopedlock(m_mutex);

	if (m_handle != 0)
	{
		FreeLibrary((HMODULE) m_handle);
		m_handle = 0;
	}

  m_sFilePath.clear();
}


bool DynamicLibraryImpl::isLoadedImpl() const
{
	return (m_handle != 0);
}


void* DynamicLibraryImpl::findSymbolImpl(const std::string& name)
{
  ScopedLock scopedLock(m_mutex);

  void* pRet = 0;
  if (m_handle)
	{
		pRet = (void*) GetProcAddress((HMODULE) m_handle, name.c_str());
	}

  return (pRet);
}


const std::string& DynamicLibraryImpl::getPathImpl() const
{
  return (m_sFilePath);
}


std::string DynamicLibraryImpl::getFileExtensionImpl()
{
#if defined(_DEBUG)
	return "d.dll";
#else
	return ".dll";
#endif
}
