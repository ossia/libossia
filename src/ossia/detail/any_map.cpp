#include "any_map.hpp"

namespace ossia
{

bool has_attribute(const any_map& e, std::string_view str) noexcept
{
  return e.find(str) != e.end();
}

void set_attribute(any_map& e, std::string_view str)
{
  auto it = e.find(str);
  if(it == e.end())
    e.insert(std::make_pair(std::string(str), ossia::any{}));
}

void set_attribute(any_map& e, std::string_view str, std::nullopt_t)
{
  unset_attribute(e, str);
}

void unset_attribute(any_map& e, std::string_view str)
{
#if defined(OSSIA_NO_FAST_CONTAINERS) \
    && !defined(__cpp_lib_associative_heterogeneous_erasure)
  if(auto it = e.find(str); it != e.end())
    e.erase(it);
#else
  e.erase(str);
#endif
}

}
