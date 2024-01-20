#pragma once

#if __has_include(<dlfcn.h>)
#include <ossia/detail/fmt.hpp>

#include <dlfcn.h>

#include <stdexcept>

namespace ossia
{
class dylib_loader
{
public:
  explicit dylib_loader(const char* const so)
  {
    impl = dlopen(so, RTLD_LAZY | RTLD_LOCAL | RTLD_NODELETE);
    if(!impl)
    {
      throw std::runtime_error(fmt::format("{}: not found. ", so));
    }
  }

  explicit dylib_loader(std::vector<std::string_view> sos)
  {
    if(sos.empty())
      throw std::runtime_error("No shared object specified");

    for(const auto so : sos)
    {
      impl = dlopen(so.data(), RTLD_LAZY | RTLD_LOCAL | RTLD_NODELETE);
      if(impl)
        return;
    }

    throw std::runtime_error(fmt::format("{}: not found. ", sos[0]));
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
      dlclose(impl);
    }
  }

  template <typename T>
  T symbol(const char* const sym) const noexcept
  {
    return (T)dlsym(impl, sym);
  }

  operator bool() const { return bool(impl); }

private:
  void* impl{};
};
}
#endif
