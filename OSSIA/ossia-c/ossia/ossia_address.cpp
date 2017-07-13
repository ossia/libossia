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
