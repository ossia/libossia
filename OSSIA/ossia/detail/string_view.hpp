#pragma once
#include <boost/version.hpp>
#if defined(__has_include)
  #if __has_include(<string_view>)
    #include <string_view>
    namespace ossia
    { using string_view = std::string_view; }
  #elif __has_include(<experimental/string_view>)
    #include <experimental/string_view>
    namespace ossia
    { using string_view = std::experimental::string_view; }
  #endif
#else
  #if BOOST_VERSION >= 106100
    #define HAS_BOOST_STRING_VIEW
    #include <boost/utility/string_view.hpp>
    namespace ossia
    { using string_view = boost::string_view; }
  #else
    #define HAS_BOOST_STRING_REF
    #include <boost/utility/string_ref.hpp>
    namespace ossia
    { using string_view = boost::string_ref; }
  #endif
#endif
