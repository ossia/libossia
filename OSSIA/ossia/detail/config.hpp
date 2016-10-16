#pragma once
#include <type_traits>
#if defined(_MSC_VER)
#define OSSIA_DECL_RELAXED_CONSTEXPR
#else
#define OSSIA_DECL_RELAXED_CONSTEXPR constexpr
#endif

// Remove with C++17
#if !defined(__cpp_lib_void_t)
namespace ossia
{
template<typename... Args>
using void_t = void;
}
#else
namespace ossia
{
template<typename... Args>
using void_t = std::void_t<Args...>;
}
#endif
