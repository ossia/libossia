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

extended_type float_list_type()
{
  return "floatArray";
}

extended_type integer_list_type()
{
  return "integerArray";
}

extended_type string_list_type()
{
  return "stringArray";
}

extended_type tuple_type()
{
  return "tuple";
}

std::vector<ossia::val_type> underlying_type(const extended_type& e)
{
  if (e == generic_buffer_type() || e == filesystem_path_type())
    return {ossia::val_type::STRING};

  if (e == float_array_type())
    return {ossia::val_type::TUPLE, ossia::val_type::VEC2F,
            ossia::val_type::VEC3F, ossia::val_type::VEC4F};

  if (e == tuple_type() || e == float_list_type() || e == integer_list_type()
      || e == string_list_type())
    return {ossia::val_type::TUPLE};
  return {};
}
}
