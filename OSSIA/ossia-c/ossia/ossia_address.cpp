// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_utils.hpp"

extern "C" {
void ossia_address_set_access_mode(
    ossia_address_t address, ossia_access_mode am)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_set_access_mode: address is null");
      return;
    }

    convert_address(address)->set_access(convert(am));
  });
}

ossia_access_mode ossia_address_get_access_mode(ossia_address_t address)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_get_access_mode: address is null");
      return BI;
    }

    return convert(convert_address(address)->get_access());
  });
}

void ossia_address_set_bounding_mode(
    ossia_address_t address, ossia_bounding_mode am)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_set_bounding_mode: address is null");
      return;
    }

    convert_address(address)->set_bounding(convert(am));
  });
}

ossia_bounding_mode ossia_address_get_bounding_mode(ossia_address_t address)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_get_bounding_mode: address is null");
      return FREE;
    }

    return convert(convert_address(address)->get_bounding());
  });
}

void ossia_address_set_domain(ossia_address_t address, ossia_domain_t domain)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_set_domain: address is null");
      return;
    }
    if (!domain)
    {
      ossia_log_error("ossia_address_set_domain: domain is null");
      return;
    }

    convert_address(address)->set_domain(domain->domain);
  });
}

ossia_domain_t ossia_address_get_domain(ossia_address_t address)
{
  return safe_function(__func__, [=]() -> ossia_domain_t {
    if (!address)
    {
      ossia_log_error("ossia_address_get_domain: address is null");
      return nullptr;
    }

    return new ossia_domain{convert_address(address)->get_domain()};
  });
}

void ossia_address_set_value(ossia_address_t address, ossia_value_t value)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_set_value: address is null");
      return;
    }
    if (!value)
    {
      ossia_log_error("ossia_address_set_value: value is null");
      return;
    }

    convert_address(address)->set_value(convert(value));
  });
}

ossia_value_t ossia_address_clone_value(ossia_address_t address)
{
  return safe_function(__func__, [=]() -> ossia_value_t {
    if (!address)
    {
      ossia_log_error("ossia_address_clone_value: address is null");
      return nullptr;
    }

    return convert(convert_address(address)->value());
  });
}

void ossia_address_push_value(ossia_address_t address, ossia_value_t value)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }
    if (!value)
    {
      ossia_log_error("ossia_address_push_value: value is null");
      return;
    }

    convert_address(address)->push_value(convert(value));
  });
}

void ossia_address_push_impulse(ossia_address_t address)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    convert_address(address)->push_value(ossia::impulse{});
  });
}
void ossia_address_push_i(ossia_address_t address, int i)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    convert_address(address)->push_value(i);
  });
}
void ossia_address_push_b(ossia_address_t address, int i)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    convert_address(address)->push_value(bool(i != 0));
  });
}
void ossia_address_push_f(ossia_address_t address, float f)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    convert_address(address)->push_value(f);
  });
}
void ossia_address_push_2f(ossia_address_t address, float f1, float f2)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    convert_address(address)->push_value(ossia::make_vec(f1, f2));
  });
}
void ossia_address_push_3f(ossia_address_t address, float f1, float f2, float f3)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    convert_address(address)->push_value(ossia::make_vec(f1, f2, f3));
  });
}
void ossia_address_push_4f(ossia_address_t address, float f1, float f2, float f3, float f4)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    convert_address(address)->push_value(ossia::make_vec(f1, f2, f3, f4));
  });
}
void ossia_address_push_c(ossia_address_t address, char c)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    convert_address(address)->push_value(c);
  });
}
void ossia_address_push_s(ossia_address_t address, const char* s)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    if(s)
      convert_address(address)->push_value(std::string(s));
    else
      convert_address(address)->push_value(std::string());
  });
}

void ossia_address_push_in(ossia_address_t address, const int* in, int sz)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    std::vector<ossia::value> v;
    v.resize(sz);
    for(int i = 0; i < sz; i++)
    {
      v[i] = in[i];
    }
    convert_address(address)->push_value(std::move(v));
  });
}
void ossia_address_push_fn(ossia_address_t address, const float* in, int sz)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    std::vector<ossia::value> v;
    v.resize(sz);
    for(int i = 0; i < sz; i++)
    {
      v[i] = in[i];
    }
    convert_address(address)->push_value(std::move(v));
  });
}
void ossia_address_push_cn(ossia_address_t address, const char* in, int sz)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_push_value: address is null");
      return;
    }

    convert_address(address)->push_value(std::string(in, sz));
  });
}

ossia_value_t ossia_address_pull_value(ossia_address_t address)
{
  return safe_function(__func__, [=]() -> ossia_value_t {
    if (!address)
    {
      ossia_log_error("ossia_address_pull_value: address is null");
      return nullptr;
    }

    return convert(convert_address(address)->fetch_value());
  });
}

ossia_value_callback_index_t ossia_address_add_callback(
    ossia_address_t address, ossia_value_callback_t callback)
{
  return safe_function(__func__, [=]() -> ossia_value_callback_index_t {
    if (!address)
    {
      ossia_log_error("ossia_address_add_callback: address is null");
      return nullptr;
    }
    if (!callback)
    {
      ossia_log_error("ossia_address_add_callback: callback is null");
      return nullptr;
    }

    return new ossia_value_callback_index{
        convert_address(address)->add_callback([=](const ossia::value& val) {
          DEBUG_LOG_FMT("inside added callback");
          callback(convert(val));
        })};
  });
}

void ossia_address_remove_callback(
    ossia_address_t address, ossia_value_callback_index_t index)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_address_remove_callback: address is null");
      return;
    }
    if (!index)
    {
      ossia_log_error("ossia_address_remove_callback: index is null");
      return;
    }

    convert_address(address)->remove_callback(index->it);
    delete index;
  });
}
}
