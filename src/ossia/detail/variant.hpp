#pragma once
#if __has_include(<boost/variant2/variant.hpp>)
#include <boost/variant2/variant.hpp>
namespace ossia
{
using namespace boost::variant2;
}
#else
#include <variant>
namespace ossia
{
template<typename... Args>
using variant = std::variant<Args...>;
}
#endif
