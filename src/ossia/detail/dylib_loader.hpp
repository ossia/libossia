#pragma once
#include <ossia/detail/config.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
namespace ossia
{
class dylib_loader
{
public:
  explicit dylib_loader(const char* const so)
  {
#ifdef _WIN32
    impl = (void*)LoadLibraryA(so);
#else
    impl = dlopen(so, RTLD_LAZY | RTLD_LOCAL | RTLD_NODELETE);
#endif
    if(!impl)
    {
      throw std::runtime_error(std::string(so) + ": not found. ");
    }
  }

  explicit dylib_loader(std::vector<std::string_view> sos)
  {
    if(sos.empty())
      throw std::runtime_error("No shared object specified");

    for(const auto so : sos)
    {
#ifdef _WIN32
      impl = (void*)LoadLibraryA(so.data());
#else
      impl = dlopen(so.data(), RTLD_LAZY | RTLD_LOCAL | RTLD_NODELETE);
#endif
      if(impl)
        return;
    }

    throw std::runtime_error(std::string(sos[0]) + ": not found. ");
  }

  dylib_loader(const dylib_loader&) noexcept = delete;
  dylib_loader& operator=(const dylib_loader&) noexcept = delete;
  dylib_loader(dylib_loader&& other) noexcept
  {
    impl = other.impl;
    other.impl = nullptr;
  }

  dylib_loader& operator=(dylib_loader&& other) noexcept
  {
    impl = other.impl;
    other.impl = nullptr;
    return *this;
  }

  ~dylib_loader()
  {
    if(impl)
    {
#ifdef _WIN32
      FreeLibrary((HMODULE)impl);
#else
      dlclose(impl);
#endif
    }
  }

  template <typename T>
  T symbol(const char* const sym) const noexcept
  {
#ifdef _WIN32
    return (T)GetProcAddress((HMODULE)impl, sym);
#else
    return (T)dlsym(impl, sym);
#endif
  }

  operator bool() const { return bool(impl); }

private:
  void* impl{};
};
}
