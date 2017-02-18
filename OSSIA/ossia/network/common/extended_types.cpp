#include "extended_types.hpp"


namespace ossia
{

extended_type generic_buffer_type()
{
  return "buffer";
}

extended_type filesystem_path_type()
{
  return "filepath";
}

extended_type float_array_type()
{
  return "vecf";
}

std::vector<ossia::val_type> underlying_type(const extended_type& e)
{
  if(e == generic_buffer_type()) return {ossia::val_type::STRING};
  if(e == filesystem_path_type()) return {ossia::val_type::STRING};
  if(e == float_array_type()) return {ossia::val_type::TUPLE, ossia::val_type::VEC2F, ossia::val_type::VEC3F, ossia::val_type::VEC4F};
  return {};
}

}
