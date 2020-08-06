#pragma once
#include <ciso646>

// Macro to open the C++ std namespace
#if defined(_LIBCPP_VERSION)
  #define OSSIA_STD_BEGIN_NAMESPACE _LIBCPP_BEGIN_NAMESPACE_STD
#elif defined(__GLIBCXX__)
  #if _GLIBCXX_USE_CXX11_ABI
    #define OSSIA_STD_BEGIN_NAMESPACE namespace std { _GLIBCXX_BEGIN_NAMESPACE_VERSION
  #else
    #define OSSIA_STD_BEGIN_NAMESPACE namespace std { _GLIBCXX_BEGIN_NAMESPACE_VERSION
  #endif
#else
  #define OSSIA_STD_BEGIN_NAMESPACE namespace std {
#endif

#if defined(_LIBCPP_VERSION)
  #define OSSIA_STD_BEGIN_NAMESPACE_CXX11 _LIBCPP_BEGIN_NAMESPACE_STD
#elif defined(__GLIBCXX__)
  #if _GLIBCXX_USE_CXX11_ABI
    #define OSSIA_STD_BEGIN_NAMESPACE_CXX11 namespace std { _GLIBCXX_BEGIN_NAMESPACE_VERSION inline _GLIBCXX_BEGIN_NAMESPACE_CXX11
  #else
    #define OSSIA_STD_BEGIN_NAMESPACE_CXX11 namespace std { _GLIBCXX_BEGIN_NAMESPACE_VERSION _GLIBCXX_BEGIN_NAMESPACE_CXX11
  #endif
#else
  #define OSSIA_STD_BEGIN_NAMESPACE_CXX11 namespace std {
#endif

// Macro to close the C++ std namespace
#if defined(_LIBCPP_VERSION)
  #define OSSIA_STD_END_NAMESPACE _LIBCPP_END_NAMESPACE_STD
#elif defined(__GLIBCXX__)
  #define OSSIA_STD_END_NAMESPACE }
#else
  #define OSSIA_STD_END_NAMESPACE }
#endif


#if defined(_LIBCPP_VERSION)
  #define OSSIA_STD_END_NAMESPACE_CXX11 _LIBCPP_END_NAMESPACE_STD
#elif defined(__GLIBCXX__)
  #define OSSIA_STD_END_NAMESPACE_CXX11 _GLIBCXX_END_NAMESPACE_CXX11 }
#else
  #define OSSIA_STD_END_NAMESPACE_CXX11 }
#endif

