#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_HAS_FMT)
#include <ossia/detail/fmt.hpp>
namespace ossia
{
using fmt::to_string;
}
#else
namespace ossia
{
using std::to_string;
}
#endif
