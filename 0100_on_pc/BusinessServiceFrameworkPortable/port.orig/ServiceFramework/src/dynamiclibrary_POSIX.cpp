#include "dynamiclibrary_POSIX.h"
#include "scopedlock.h"
#include "comalog.h"


// Note: cygwin is missing RTLD_LOCAL, set it to 0
#if defined(__CYGWIN__) && !defined(RTLD_LOCAL)
#define RTLD_LOCAL 0
#endif



Mutex DynamicLibraryImpl::m_mutex;


DynamicLibraryImpl::DynamicLibraryImpl()
{
  m_handle = 0;
}


DynamicLibraryImpl::~DynamicLibraryImpl()
{
}


bool DynamicLibraryImpl::loadLibraryImpl(const std::string& path, int flags)
{
  ScopedLock scopedLock(m_mutex);

  bool bRet = false;

  if (m_handle == 0)
  {
    int realFlags = RTLD_LAZY;
    if (flags & LOCAL)
    {
      realFlags |= RTLD_LOCAL;
    }
    else
    {
      realFlags |= RTLD_GLOBAL;
    }

    fflush(stdout);
    m_handle = dlopen(path.c_str(), realFlags);

    if (m_handle == 0)
    {
      const char* err = dlerror();
      COMALOG(COMALOG_ERROR, "Error loading lib: %s\n", err);
      bRet = false;
    }
    else
    {
      m_sFilePath = path;
      bRet = true;
    }
  }

  return (bRet);
}


void DynamicLibraryImpl::unloadLibraryImpl()
{
  ScopedLock scopedLock(m_mutex);

  if (m_handle != 0)
  {
    dlclose(m_handle);
    m_handle = 0;
  }
}


bool DynamicLibraryImpl::isLoadedImpl() const
{
  return (m_handle != 0);
}


void* DynamicLibraryImpl::findSymbolImpl(const std::string& name)
{
  ScopedLock scopedLock(m_mutex);

  void* result = 0;
  if (m_handle !=0)
  {
    result = dlsym(m_handle, name.c_str());
  }
  return (result);
}


const std::string& DynamicLibraryImpl::getPathImpl() const
{
  return (m_sFilePath);
}


std::string DynamicLibraryImpl::getFileExtensionImpl()
{
#if defined(OS_WINDOWS)
#if defined(_DEBUG)
    return "d.dll";
  #else
    return ".dll";
  #endif
#else
  #if defined(_DEBUG)
  return "d.so";
#else
  return ".so";
  #endif
#endif
}
