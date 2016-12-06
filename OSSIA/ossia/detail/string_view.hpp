#pragma once
#include <boost/version.hpp>

#if BOOST_VERSION >= 106100
#define HAS_BOOST_STRING_VIEW
#include <boost/utility/string_view.hpp>
namespace ossia
{
using string_view = boost::string_view;
}
#else
#define HAS_BOOST_STRING_REF
#include <boost/utility/string_ref.hpp>
namespace ossia
{
using string_view = boost::string_ref;
}
#endif
