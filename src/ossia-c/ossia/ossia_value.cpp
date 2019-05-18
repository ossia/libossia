// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include "ossia_utils.hpp"
template <typename Requested_T>
Requested_T get_value(ossia_value_t val)
{
  if (!val)
  {
    ossia_log_error("get_value<T>: val is null");
    return {};
  }

  if (auto casted_val = val->value.target<Requested_T>())
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

  return ossia::convert<Requested_T>(val->value);
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
ossia_value_t ossia_value_create_byte_array(const char* value, size_t size)
{
  return convert(std::string(value, size));
}

ossia_value_t ossia_value_create_list(const ossia_value_t* values, size_t size)
{
  std::vector<ossia::value> t;
  t.reserve(size);
  for (std::size_t i = 0; i < size; i++)
  {
    t.push_back(values[i]->value);
  }
  return convert(std::move(t));
}

ossia_value_t ossia_value_create_fn(const float* values, size_t size)
{
  std::vector<ossia::value> t;
  t.reserve(size);
  for (std::size_t i = 0; i < size; i++)
  {
    t.push_back(values[i]);
  }
  return convert(std::move(t));
}

ossia_value_t ossia_value_create_in(const int* values, size_t size)
{
  std::vector<ossia::value> t;
  t.reserve(size);
  for (size_t i = 0; i < size; i++)
  {
    t.push_back(values[i]);
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

  return convert(val->value.get_type());
}

int ossia_value_to_int(ossia_value_t val)
{
  return get_value<int32_t>(val);
}

float ossia_value_to_float(ossia_value_t val)
{
  return get_value<float>(val);
}

ossia_vec2f ossia_value_to_2f(ossia_value_t val)
{
  auto v = get_value<std::array<float, 2>>(val);
  return {v[0], v[1]};
}
ossia_vec3f ossia_value_to_3f(ossia_value_t val)
{
  auto v = get_value<std::array<float, 3>>(val);
  return {v[0], v[1], v[2]};
}
ossia_vec4f ossia_value_to_4f(ossia_value_t val)
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

void ossia_value_to_byte_array(ossia_value_t val, char** out, size_t* size)
{
  if (!val || !out || !size)
  {
    ossia_log_error("ossia_value_to_byte_array: a parameter is null");
  }
  else if (auto casted_val = val->value.target<std::string>())
  {
    copy_bytes(*casted_val, out, size);
    return;
  }

  *out = nullptr;
  *size = 0;
}

const char* ossia_value_to_string(ossia_value_t val)
{
  if (!val)
  {
    ossia_log_error("ossia_value_to_string: val is null");
  }
  else if (auto casted_val = val->value.target<std::string>())
  {
    return copy_string(*casted_val);
  }
  return nullptr;
}
void ossia_value_to_list(ossia_value_t val, ossia_value_t** out, size_t* size)
{
  if (!val || !out || !size)
  {
    ossia_log_error("ossia_value_to_list: a parameter is null");
  }
  else if (auto casted_val = val->value.target<std::vector<ossia::value>>())
  {
    size_t N = casted_val->size();
    auto ptr = new ossia_value_t[N];
    *size = N;
    for(size_t i = 0; i < N; i++)
    {
      ptr[i] = convert((*casted_val)[i]);
    }
    *out = ptr;
    return;
  }

  *out = nullptr;
  *size = 0;
}
void ossia_value_free_list(ossia_value_t* out)
{
  delete[] out;
}


void ossia_value_to_fn(ossia_value_t val, float** out, size_t* size)
{
  if (!val || !out || !size)
  {
    ossia_log_error("ossia_value_to_fn: a parameter is null");
  }
  else if (auto casted_val = val->value.target<std::vector<ossia::value>>())
  {
    const size_t N = casted_val->size();
    auto ptr = new float[N];
    *size = N;
    for(size_t i = 0; i < N; i++)
    {
      ptr[i] = (*casted_val)[i].get<float>();
    }
    *out = ptr;
    return;
  }

  *out = nullptr;
  *size = 0;
}
void ossia_value_free_fn(float* out)
{
  delete[] out;
}

void ossia_value_to_in(ossia_value_t val, int** out, size_t* size)
{
  if (!val || !out || !size)
  {
    ossia_log_error("ossia_value_to_fn: a parameter is null");
  }
  else if (auto casted_val = val->value.target<std::vector<ossia::value>>())
  {
    const size_t N = casted_val->size();
    auto ptr = new int[N];
    *size = N;
    for(size_t i = 0; i < N; i++)
    {
      ptr[i] = (*casted_val)[i].get<int>();
    }
    *out = ptr;
    return;
  }

  *out = nullptr;
  *size = 0;
}
void ossia_value_free_in(int* out)
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
ossia_vec2f ossia_value_convert_2f(ossia_value_t val)
{
  auto v = convert_value<ossia::vec2f>(val);
  return {v[0], v[1]};
}
ossia_vec3f ossia_value_convert_3f(ossia_value_t val)
{
  auto v = convert_value<ossia::vec3f>(val);
  return {v[0], v[1], v[2]};
}
ossia_vec4f ossia_value_convert_4f(ossia_value_t val)
{
  auto v = convert_value<ossia::vec4f>(val);
  return {v[0], v[1], v[2], v[3]};
}


void ossia_value_convert_byte_array(ossia_value_t val, char** str, size_t* size)
{
  if (!val || !str || !size)
  {
    ossia_log_error("ossia_value_convert_byte_array: a parameter is null");
    *str = nullptr;
    *size = 0;
    return;
  }

  copy_bytes(ossia::convert<std::string>(val->value), str, size);
}

void ossia_value_convert_list(ossia_value_t val, ossia_value_t** out, size_t* size)
{
  if (!val || !out || !size)
  {
    ossia_log_error("ossia_value_convert_list: a parameter is null");

    *out = nullptr;
    *size = 0;
    return;
  }
  auto v = ossia::convert<std::vector<ossia::value>>(val->value);

  size_t N = v.size();
  *out = new ossia_value_t[N];
  *size = N;

  for(size_t i = 0; i < N; i++)
  {
    *out[i] = convert(v[i]);
  }
}

}
