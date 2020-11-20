#include "dynamiclibrary.h"

DynamicLibrary::DynamicLibrary()
{
}

DynamicLibrary::~DynamicLibrary()
{
}

DynamicLibrary::DynamicLibrary(const std::string& path)
{
  loadLibraryImpl(path, 0);
}

DynamicLibrary::DynamicLibrary(const std::string& path, int flags)
{
  loadLibraryImpl(path, flags);
}

bool DynamicLibrary::loadLibrary(const std::string& path)
{
  return (loadLibraryImpl(path, 0));
}

bool DynamicLibrary::loadLibrary(const std::string& path, int flags)
{
  return (loadLibraryImpl(path, flags));
}

void DynamicLibrary::unloadLibrary()
{
  unloadLibraryImpl();
}

bool DynamicLibrary::isLoaded() const
{
	return isLoadedImpl();
}

bool DynamicLibrary::hasSymbol(const std::string& name)
{
	return findSymbolImpl(name) != 0;
}

void* DynamicLibrary::getSymbol(const std::string& name)
{
	void* result = findSymbolImpl(name);
  return result;
}

const std::string& DynamicLibrary::getPath() const
{
	return getPathImpl();
}

std::string DynamicLibrary::getFileExtension()
{
  return getFileExtensionImpl();
}
