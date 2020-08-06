#pragma once
#include <ossia/detail/std_fwd.hpp>

OSSIA_STD_BEGIN_NAMESPACE_CXX11
#if defined(_MSC_VER)
  template <typename T>
  class regex_traits;
#else
  template <typename T>
  struct regex_traits;
#endif

template <typename T, typename Traits>
class basic_regex;

using regex = basic_regex<char, regex_traits<char>>;
OSSIA_STD_END_NAMESPACE_CXX11

#if defined(_MSC_VER)
#if defined(NDEBUG)
static const constexpr auto sizeof_regex = 40;
#else
static const constexpr auto sizeof_regex = 96;
#endif
#elif defined(_LIBCPP_VERSION)
static const constexpr auto sizeof_regex = 64;
#elif defined(__GLIBCXX__)
static const constexpr auto sizeof_regex = 32;
#else
#include <regex>
#define sizeof_regex sizeof(std::regex);
#endif
