// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include "ossia_utils.hpp"
#include <ossia/network/domain/domain.hpp>

extern "C" {

ossia_domain_t ossia_domain_make_min_max(
        ossia_value_t min,
        ossia_value_t max)
{
    return safe_function(__func__, [=]() -> ossia_domain_t {
        ossia::value vmin, vmax;
      if (min)
        vmin = min->value;
      if(max)
        vmax = max->value;

      return convert(ossia::make_domain(vmin, vmax));
    });
}

ossia_value_t ossia_domain_get_min(ossia_domain_t domain)
{
  return safe_function(__func__, [=]() -> ossia_value_t {
    if (!domain)
      return nullptr;

    return convert(ossia::get_min(domain->domain));
  });
}

void ossia_domain_set_min(ossia_domain_t domain, ossia_value_t value)
{
  return safe_function(__func__, [=] {
    if (!domain)
      return;
    if (!value)
      return;

    ossia::set_min(domain->domain, value->value);
  });
}

ossia_value_t ossia_domain_get_max(ossia_domain_t domain)
{
  return safe_function(__func__, [=]() -> ossia_value_t {
    if (!domain)
      return nullptr;

    return convert(ossia::get_max(domain->domain));
  });
}

void ossia_domain_set_max(ossia_domain_t domain, ossia_value_t value)
{
  return safe_function(__func__, [=] {
    if (!domain)
      return;
    if (!value)
      return;

    ossia::set_max(domain->domain, value->value);
  });
}

void ossia_domain_get_values(
    ossia_domain_t domain,
    ossia_value_t** values,
    size_t* size)
{
  if (!domain || !values|| !size)
  {
    ossia_log_error("ossia_domain_get_values: a parameter is null");
  }
  else
  {
    auto vals = ossia::get_values(domain->domain);
    size_t N = vals.size();
    auto ptr = new ossia_value_t[N];
    *size = N;
    for(size_t i = 0; i < N; i++)
    {
      ptr[i] = convert(vals[i]);
    }
    *values = ptr;
    return;
  }

  *values = nullptr;
  *size = 0;
}

void ossia_domain_set_values(
    ossia_domain_t domain,
    const ossia_value_t* in,
    size_t size)
{
  return safe_function(__func__, [=] {
    if (!domain || !in)
    {
      ossia_log_error("ossia_domain_set_values: a parameter is null");
      return;
    }

    std::vector<ossia::value> v;
    v.resize(size);
    for(size_t i = 0; i < size; i++)
    {
      v[i] = in[i];
    }
    ossia::set_values(domain->domain, std::move(v));
  });
}

void ossia_domain_free(ossia_domain_t domain)
{
  return safe_function(__func__, [=] { delete domain; });
}
}
