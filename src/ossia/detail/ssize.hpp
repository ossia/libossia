#pragma once

#if __has_include(<version>)
#include <version>
// MacOSX11.3 SDK provides ssize but does not define it...
#if __cpp_lib_ssize >= 201902L                                             \
    || (__MAC_OS_X_VERSION_MIN_REQUIRED >= 114000 && _LIBCPP_STD_VER > 17) \
    || defined(__EMSCRIPTEN__)
#include <iterator>
#define OSSIA_HAS_STD_SSIZE 1
#endif
#endif

#if !defined(OSSIA_HAS_STD_SSIZE)
#include <type_traits>
namespace std
{
template <class T>
static constexpr auto ssize(const T& t) noexcept
{
  return static_cast<common_type_t<ptrdiff_t, make_signed_t<decltype(t.size())>>>(
      t.size());
}

template <class T, ptrdiff_t S>
static constexpr ptrdiff_t ssize(const T (&)[S]) noexcept
{
  return S;
}
}
#endif
