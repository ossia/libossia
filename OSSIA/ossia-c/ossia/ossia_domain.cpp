// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_utils.hpp"

extern "C" {
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

void ossia_domain_free(ossia_domain_t domain)
{
  return safe_function(__func__, [=] { delete domain; });
}
}
