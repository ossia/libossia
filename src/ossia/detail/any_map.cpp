#include "any_map.hpp"

namespace ossia
{

bool has_attribute(const any_map& e, string_view str) noexcept
{
  return e.find(str) != e.end();
}

void set_attribute(any_map& e, string_view str)
{
  auto it = e.find(str);
  if(it == e.end())
    e.insert(std::make_pair(std::string(str), ossia::any{}));
}

void set_attribute(any_map& e, string_view str, std::nullopt_t)
{
  unset_attribute(e, str);
}

void unset_attribute(any_map& e, string_view str)
{
  e.erase(str);
}

}
