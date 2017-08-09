// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_utils.hpp"
template <typename Requested_T>
Requested_T get_value(ossia_value_t val)
{
  if (!val)
  {
    ossia_log_error("get_value<T>: val is null");
    return {};
  }

  if (auto casted_val = convert(val).target<Requested_T>())
  {
    return *casted_val;
  }

  return {};
}

template <typename Requested_T>
Requested_T convert_value(ossia_value_t val)
{
  if (!val)
  {
    ossia_log_error("get_value<T>: val is null");
    return {};
  }

  return ossia::convert<Requested_T>(convert(val));
}

extern "C" {

ossia_value_t ossia_value_create_impulse()
{
  return convert(ossia::impulse{});
}

ossia_value_t ossia_value_create_int(int value)
{
  return convert(int32_t{value});
}

ossia_value_t ossia_value_create_float(float value)
{
  return convert(float{value});
}

ossia_value_t ossia_value_create_2f(float v1, float v2)
{
  return convert(ossia::make_vec(v1, v2));
}
ossia_value_t ossia_value_create_3f(float v1, float v2, float v3)
{
  return convert(ossia::make_vec(v1, v2, v3));
}
ossia_value_t ossia_value_create_4f(float v1, float v2, float v3, float v4)
{
  return convert(ossia::make_vec(v1, v2, v3, v4));
}

ossia_value_t ossia_value_create_bool(int value)
{
  return convert(static_cast<bool>(value));
}

ossia_value_t ossia_value_create_char(char value)
{
  return convert(char{value});
}

ossia_value_t ossia_value_create_string(const char* value)
{
  return convert(std::string{value});
}

ossia_value_t ossia_value_create_tuple(ossia_value_t* values, int size)
{
  std::vector<ossia::value> t;
  for (int i = 0; i < size; i++)
  {
    t.push_back(convert(values[i]));
  }
  return convert(std::move(t));
}

void ossia_value_free(ossia_value_t value)
{
  return safe_function(__func__, [=] { delete value; });
}

ossia_type ossia_value_get_type(ossia_value_t val)
{
  if (!val)
  {
    ossia_log_error("ossia_value_get_type: val is null");
    return static_cast<ossia_type>(-1);
  }

  return convert(convert(val).getType());
}

int ossia_value_to_int(ossia_value_t val)
{
  return get_value<int32_t>(val);
}

float ossia_value_to_float(ossia_value_t val)
{
  return get_value<float>(val);
}

vec2f ossia_value_to_2f(ossia_value_t val)
{
  auto v = get_value<std::array<float, 2>>(val);
  return {v[0], v[1]};
}
vec3f ossia_value_to_3f(ossia_value_t val)
{
  auto v = get_value<std::array<float, 3>>(val);
  return {v[0], v[1], v[2]};
}
vec4f ossia_value_to_4f(ossia_value_t val)
{
  auto v = get_value<std::array<float, 4>>(val);
  return {v[0], v[1], v[2], v[3]};
}

int ossia_value_to_bool(ossia_value_t val)
{
  return get_value<bool>(val);
}

char ossia_value_to_char(ossia_value_t val)
{
  return get_value<char>(val);
}

const char* ossia_value_to_string(ossia_value_t val)
{
  if (!val)
  {
    ossia_log_error("ossia_value_to_string: val is null");
    return nullptr;
  }

  if (auto casted_val = convert(val).target<std::string>())
  {
    return copy_string(*casted_val);
  }

  return nullptr;
}

void ossia_value_free_string(const char* str)
{
  delete[] str;
}

void ossia_value_to_tuple(ossia_value_t val, ossia_value_t** out, int* size)
{
  if (!val)
  {
    ossia_log_error("ossia_value_to_tuple: val is null");
  }
  else if (auto casted_val = convert(val).target<std::vector<ossia::value>>())
  {
    int N = casted_val->size();
    *out = new ossia_value_t[N];
    *size = N;
    for(int i = 0; i < N; i++)
    {
      *out[i] = convert((*casted_val)[i]);
    }
    return;
  }

  *out = nullptr;
  *size = 0;
}
void ossia_value_free_tuple(ossia_value_t* out)
{
  delete[] out;
}


int ossia_value_convert_int(ossia_value_t val)
{
  return convert_value<int32_t>(val);
}
float ossia_value_convert_float(ossia_value_t val)
{
  return convert_value<float>(val);
}
char ossia_value_convert_char(ossia_value_t val)
{
  return convert_value<char>(val);
}
int ossia_value_convert_bool(ossia_value_t val)
{
  return convert_value<bool>(val);
}

const char* ossia_value_convert_string(ossia_value_t val)
{
  if (!val)
  {
    ossia_log_error("ossia_value_convert_string: val is null");
    return nullptr;
  }

  return copy_string(ossia::convert<std::string>(convert(val)));
}

void ossia_value_convert_tuple(ossia_value_t val, ossia_value_t** out, int* size)
{
  if (!val)
  {
    ossia_log_error("ossia_value_convert_tuple: val is null");

    *out = nullptr;
    *size = 0;
    return;
  }
  auto v = ossia::convert<std::vector<ossia::value>>(convert(val));

  int N = v.size();
  *out = new ossia_value_t[N];
  *size = N;

  for(int i = 0; i < N; i++)
  {
    *out[i] = convert(v[i]);
  }
}

}
