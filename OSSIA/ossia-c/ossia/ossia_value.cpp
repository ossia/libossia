#include "ossia_utils.hpp"
template<typename Requested_T>
Requested_T get_value(ossia_value_t val)
{
  if(!val)
    return {};

  if(auto casted_val = convert(val).target<Requested_T>())
  {
    return *casted_val;
  }

  return {};
}

extern "C"
{

ossia_value_t ossia_value_create_impulse()
{
  return convert(ossia::Impulse{});
}

ossia_value_t ossia_value_create_int(
    int value)
{
  return convert(int32_t{value});
}

ossia_value_t ossia_value_create_float(
    float value)
{
  return convert(float{value});
}

ossia_value_t ossia_value_create_bool(
    bool value)
{
  return convert(bool{value});
}

ossia_value_t ossia_value_create_char(
    char value)
{
  return convert(char{value});
}

ossia_value_t ossia_value_create_string(
    const char* value)
{
  return convert(std::string{value});
}

ossia_value_t ossia_value_create_tuple(
    ossia_value_t* values,
    int size)
{
  std::vector<ossia::value> t;
  for(int i = 0; i < size; i++)
  {
    t.push_back(convert(values[i]));
  }
  return convert(std::move(t));
}

void ossia_value_free(ossia_value_t value)
{
  return safe_function(__func__, [=] {
    delete value;
  });
}

ossia_type ossia_value_get_type(
    ossia_value_t val)
{
  if(!val)
    return static_cast<ossia_type>(-1);

  return convert(convert(val).getType());
}

int ossia_value_to_int(
    ossia_value_t val)
{
  return get_value<int32_t>(val);
}

float ossia_value_to_float(
    ossia_value_t val)
{
  return get_value<float>(val);
}

bool ossia_value_to_bool(
    ossia_value_t val)
{
  return get_value<bool>(val);
}

char ossia_value_to_char(
    ossia_value_t val)
{
  return get_value<char>(val);
}

const char* ossia_value_to_string(
    ossia_value_t val)
{
  if(!val)
    return nullptr;

  if(auto casted_val = convert(val).target<std::string>())
  {
    auto& s = *casted_val;
    auto size = s.size();
    char *buf = new char[size + 1];
    std::strcpy(buf, s.c_str());
    return buf;
  }

  return nullptr;
}

void ossia_value_free_string(const char * str)
{
  delete[] str;
}

void ossia_value_to_tuple(
    ossia_value_t val_in,
    ossia_value_t* out,
    int* size)
{
  // todo;
}
}
