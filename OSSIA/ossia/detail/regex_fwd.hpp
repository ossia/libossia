#pragma once
#include <ciso646>

namespace std
{
    #if defined(_LIBCPP_VERSION)
    inline namespace __1 {
   #elif defined(__GLIBCXX__)
    _GLIBCXX_BEGIN_NAMESPACE_CXX11
   #else
   #endif
    template<typename T>
    class regex_traits;
    template<typename T, typename Traits>
    class basic_regex;

    using regex = basic_regex<char, regex_traits<char>>;

    #if defined(_LIBCPP_VERSION)
    }
   #elif defined(__GLIBCXX__)
    _GLIBCXX_END_NAMESPACE_CXX11
   #else
   #endif
}

#if defined(_MSC_VER)
  static const constexpr auto sizeof_regex = 40;
#elif defined(_LIBCPP_VERSION)
  static const constexpr auto sizeof_regex = 64;
#elif defined(__GLIBCXX__)
  static const constexpr auto sizeof_regex = 32;
#else
  #include <regex>
  #define sizeof_regex  sizeof(std::regex);
#endif
