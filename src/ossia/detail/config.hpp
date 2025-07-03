#pragma once
#if __has_include(<ossia-config.hpp>)
#include <ossia-config.hpp>
#endif

#if __has_include(<ossia_export.h>)
#include <ossia_export.h>
#else
#define OSSIA_EXPORT
#endif

#if defined(__APPLE__)
#include <MacTypes.h>
#if defined(nil)
#undef nil
#endif
#endif

#define OSSIA_EXTERN_EXPORT_HPP(EXPORT) EXPORT
#define OSSIA_EXTERN_EXPORT_CPP(EXPORT)

#undef OSSIA_DEPRECATED
#if defined(__clang__) || defined(_MSC_VER) || (defined(__GNUC__) && (__GNUC__ >= 13))
#define OSSIA_DEPRECATED(REASON) [[deprecated(REASON)]]
#else
#define OSSIA_DEPRECATED(REASON)
#endif

/// Constexpr support ///
#define constexpr_return(X)                 \
  do                                        \
  {                                         \
    constexpr auto constexpr_return_x_ = X; \
    return constexpr_return_x_;             \
  } while(0)

/// Exceptions ///
#if defined(__cpp_exceptions)
#define ossia_do_throw(E, X) throw E(X);
#else
#define ossia_do_throw(E, X) std::terminate();
#undef OSSIA_HAS_FMT
#undef OSSIA_HAS_RE2
#undef OSSIA_HAS_CTRE
#undef OSSIA_HAS_RAPIDFUZZ
#endif

/// Inline support ///
#if defined(__GNUC__)
#define OSSIA_INLINE inline __attribute__((always_inline))
#elif defined(__clang__)
#define OSSIA_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define OSSIA_INLINE inline __forceinline
#else
#define OSSIA_INLINE inline
#endif

#define _WEBSOCKETPP_CPP11_STRICT_ 1

#if defined(__SANITIZE_ADDRESS__)
#define OSSIA_ASAN 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define OSSIA_ASAN 1
#endif
#endif

///  RapidJSON
#if !defined(OSSIA_ASAN)
#if defined(__AVX__)
#define RAPIDJSON_SSE42 1
#elif defined(__SSE2__)
#define RAPIDJSON_SSE2 1
#endif

#if defined(__ARM_NEON)
#define RAPIDJSON_NEON 1
#endif
#endif

#if !defined(RAPIDJSON_HAS_STDSTRING)
#define RAPIDJSON_HAS_STDSTRING 1
#endif

// https://github.com/Tencent/rapidjson/issues/1015
#if !defined(RAPIDJSON_HAS_CXX11_RVALUE_REFS)
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1
#endif

#if defined(RAPIDJSON_PARSE_DEFAULT_FLAGS)
#error Include <ossia/detail/json.hpp> to use JSON
#endif
#define RAPIDJSON_PARSE_DEFAULT_FLAGS \
  kParseCommentsFlag | kParseTrailingCommasFlag | kParseNanAndInfFlag

#if !defined(BOOST_MATH_DISABLE_FLOAT128)
#define BOOST_MATH_DISABLE_FLOAT128
#endif

/// Boost
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 1
#define BOOST_ERROR_CODE_HEADER_ONLY 1
#define BOOST_SYSTEM_NO_DEPRECATED 1
#define BOOST_LEXICAL_CAST_ASSUME_C_LOCALE 1

// Disable parallel stl as otherwise
// we need to link against TBB on GCC Linux...
#define BOOST_UNORDERED_DISABLE_PARALLEL_ALGORITHMS 1

#if !defined(BOOST_REGEX_NO_LIB)
#define BOOST_REGEX_NO_LIB 1
#endif

#if !defined(BOOST_DATE_TIME_NO_LIB)
#define BOOST_DATE_TIME_NO_LIB 1
#endif

#if !defined(BOOST_SYSTEM_NO_LIB)
#define BOOST_SYSTEM_NO_LIB 1
#endif

// #if !defined(BOOST_GRAPH_NO_BUNDLED_PROPERTIES)
// #define BOOST_GRAPH_NO_BUNDLED_PROPERTIES 1
// #endif

#if !defined(QT_NO_KEYWORDS)
#define QT_NO_KEYWORDS 1
#endif

#if defined(OSSIA_TESTING)
#define OSSIA_TEST_EXPORT OSSIA_EXPORT
#else
#define OSSIA_TEST_EXPORT
#endif

#if defined(__clang__) && defined(__has_attribute)
#if __has_attribute(__no_sanitize__)
#define OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK \
  __attribute__((__no_sanitize__("integer")))
#endif
#endif

#if !defined(OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK)
#define OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
#endif
