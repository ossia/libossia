// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

extended_type url_type()
{
  return "url";
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

extended_type u8_blob_type()
{
  return "u8Blob";
}

extended_type list_type()
{
  return "list";
}

extended_type nil_type()
{
  return "nil";
}

extended_type empty_type()
{
  return "empty";
}

std::vector<ossia::val_type> underlying_type(const extended_type& e)
{
  if(e == generic_buffer_type() || e == filesystem_path_type() || e == url_type())
    return {ossia::val_type::STRING};

  if(e == float_array_type())
    return {
        ossia::val_type::LIST, ossia::val_type::VEC2F, ossia::val_type::VEC3F,
        ossia::val_type::VEC4F};

  if(e == list_type() || e == float_list_type() || e == integer_list_type()
     || e == string_list_type() || e == u8_blob_type())
    return {ossia::val_type::LIST};

  if(e == nil_type() || e == empty_type())
    return {ossia::val_type::IMPULSE};

  return {};
}
}
