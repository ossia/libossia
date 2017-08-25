// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_utils.hpp"
#include <ossia/editor/dataspace/dataspace_visitors.hpp>

extern "C" {

ossia_node_t ossia_parameter_get_node(
        ossia_parameter_t address)
{
    return safe_function(__func__, [=] () -> ossia_node_t {
      if (!address)
      {
        ossia_log_error("ossia_parameter_get_node: address is null");
        return nullptr;
      }

      return convert(&convert_parameter(address)->get_node());
    });
}

void ossia_parameter_set_access_mode(
    ossia_parameter_t address, ossia_access_mode am)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_set_access_mode: address is null");
      return;
    }

    convert_parameter(address)->set_access(convert(am));
  });
}

ossia_access_mode ossia_parameter_get_access_mode(ossia_parameter_t address)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_get_access_mode: address is null");
      return BI;
    }

    return convert(convert_parameter(address)->get_access());
  });
}

void ossia_parameter_set_bounding_mode(
    ossia_parameter_t address, ossia_bounding_mode am)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_set_bounding_mode: address is null");
      return;
    }

    convert_parameter(address)->set_bounding(convert(am));
  });
}

ossia_bounding_mode ossia_parameter_get_bounding_mode(ossia_parameter_t address)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_get_bounding_mode: address is null");
      return FREE;
    }

    return convert(convert_parameter(address)->get_bounding());
  });
}

void ossia_parameter_set_domain(ossia_parameter_t address, ossia_domain_t domain)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_set_domain: address is null");
      return;
    }
    if (!domain)
    {
      ossia_log_error("ossia_parameter_set_domain: domain is null");
      return;
    }

    convert_parameter(address)->set_domain(domain->domain);
  });
}

ossia_domain_t ossia_parameter_get_domain(ossia_parameter_t address)
{
  return safe_function(__func__, [=]() -> ossia_domain_t {
    if (!address)
    {
      ossia_log_error("ossia_parameter_get_domain: address is null");
      return nullptr;
    }

    return new ossia_domain{convert_parameter(address)->get_domain()};
  });
}

void ossia_parameter_set_value(ossia_parameter_t address, ossia_value_t value)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_set_value: address is null");
      return;
    }
    if (!value)
    {
      ossia_log_error("ossia_parameter_set_value: value is null");
      return;
    }

    convert_parameter(address)->set_value(value->value);
  });
}

ossia_value_t ossia_parameter_get_value(ossia_parameter_t address)
{
  return safe_function(__func__, [=]() -> ossia_value_t {
    if (!address)
    {
      ossia_log_error("ossia_parameter_get_value: address is null");
      return nullptr;
    }

    return convert(convert_parameter(address)->value());
  });
}

void ossia_parameter_push_value(ossia_parameter_t address, ossia_value_t value)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }
    if (!value)
    {
      ossia_log_error("ossia_parameter_push_value: value is null");
      return;
    }

    convert_parameter(address)->push_value(value->value);
  });
}

void ossia_parameter_push_impulse(ossia_parameter_t address)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    convert_parameter(address)->push_value(ossia::impulse{});
  });
}
void ossia_parameter_push_i(ossia_parameter_t address, int i)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    convert_parameter(address)->push_value(i);
  });
}
void ossia_parameter_push_b(ossia_parameter_t address, int i)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    convert_parameter(address)->push_value(bool(i != 0));
  });
}
void ossia_parameter_push_f(ossia_parameter_t address, float f)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    convert_parameter(address)->push_value(f);
  });
}
void ossia_parameter_push_2f(ossia_parameter_t address, float f1, float f2)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    convert_parameter(address)->push_value(ossia::make_vec(f1, f2));
  });
}
void ossia_parameter_push_3f(ossia_parameter_t address, float f1, float f2, float f3)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    convert_parameter(address)->push_value(ossia::make_vec(f1, f2, f3));
  });
}
void ossia_parameter_push_4f(ossia_parameter_t address, float f1, float f2, float f3, float f4)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    convert_parameter(address)->push_value(ossia::make_vec(f1, f2, f3, f4));
  });
}
void ossia_parameter_push_c(ossia_parameter_t address, char c)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    convert_parameter(address)->push_value(c);
  });
}
void ossia_parameter_push_s(ossia_parameter_t address, const char* s)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    if(s)
      convert_parameter(address)->push_value(std::string(s));
    else
      convert_parameter(address)->push_value(std::string());
  });
}

void ossia_parameter_push_in(ossia_parameter_t address, const int* in, int sz)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    std::vector<ossia::value> v;
    v.resize(sz);
    for(int i = 0; i < sz; i++)
    {
      v[i] = in[i];
    }
    convert_parameter(address)->push_value(std::move(v));
  });
}
void ossia_parameter_push_fn(ossia_parameter_t address, const float* in, int sz)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    std::vector<ossia::value> v;
    v.resize(sz);
    for(int i = 0; i < sz; i++)
    {
      v[i] = in[i];
    }
    convert_parameter(address)->push_value(std::move(v));
  });
}
void ossia_parameter_push_cn(ossia_parameter_t address, const char* in, int sz)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_push_value: address is null");
      return;
    }

    convert_parameter(address)->push_value(std::string(in, sz));
  });
}

ossia_value_t ossia_parameter_fetch_value(ossia_parameter_t address)
{
  return safe_function(__func__, [=]() -> ossia_value_t {
    if (!address)
    {
      ossia_log_error("ossia_parameter_fetch_value: address is null");
      return nullptr;
    }

    return convert(convert_parameter(address)->fetch_value());
  });
}

void ossia_parameter_set_listening(
    ossia_parameter_t address,
    int listening)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_set_listening: address is null");
      return;
    }

    auto addr = convert_parameter(address);
    if(listening)
    {
      auto cb_it = addr->get_node().get_attribute(ossia::string_view("_impl_callback"));
      auto cb_ptr = ossia::any_cast<ossia::net::parameter_base::callback_index>(&cb_it);
      if(cb_ptr)
      {
        addr->remove_callback(*cb_ptr);
      }
    }
    else
    {
      auto it = addr->add_callback([] (const ossia::value&) { });
      ossia::set_attribute(
            (ossia::extended_attributes&)addr->get_node(),
            ossia::string_view("_impl_callback"),
            it);
    }
  });
}
ossia_value_callback_idx_t ossia_parameter_add_callback(
    ossia_parameter_t address, ossia_value_callback_t callback, void* ctx)
{
  return safe_function(__func__, [=]() -> ossia_value_callback_idx_t {
    if (!address)
    {
      ossia_log_error("ossia_parameter_add_callback: address is null");
      return nullptr;
    }
    if (!callback)
    {
      ossia_log_error("ossia_parameter_add_callback: callback is null");
      return nullptr;
    }

    return new ossia_value_callback_index{
        convert_parameter(address)->add_callback(
              [=] (const ossia::value& val) {
          callback(ctx, convert(val));
        })};
  });
}

void ossia_parameter_push_callback(
    ossia_parameter_t address, ossia_value_callback_t callback, void* ctx)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_add_callback: address is null");
      return;
    }
    if (!callback)
    {
      ossia_log_error("ossia_parameter_add_callback: callback is null");
      return;
    }

    convert_parameter(address)->add_callback(
          [=] (const ossia::value& val) {
      callback(ctx, convert(val));
    });
  });
}

void ossia_parameter_free_callback_idx(
        ossia_value_callback_idx_t cb)
{
  delete cb;
}

void ossia_parameter_remove_callback(
    ossia_parameter_t address, ossia_value_callback_idx_t index)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_parameter_remove_callback: address is null");
      return;
    }
    if (!index)
    {
      ossia_log_error("ossia_parameter_remove_callback: index is null");
      return;
    }

    convert_parameter(address)->remove_callback(index->it);
    delete index;
  });
}

void ossia_parameter_set_unit(
    ossia_parameter_t address,
    const char* unit)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_node_set_unit: address is null");
      return;
    }

    auto u = ossia::parse_pretty_unit(unit);
    convert_parameter(address)->set_unit(u);
  });
}

const char* ossia_parameter_get_unit(
    ossia_parameter_t address)
{
  return safe_function(__func__, [=]() -> const char* {
    if (!address)
    {
      ossia_log_error("ossia_node_get_unit: address is null");
      return nullptr;
    }

    return copy_string(ossia::get_pretty_unit_text(convert_parameter(address)->get_unit()));
  });
}

void ossia_parameter_set_repetition_filter(
    ossia_parameter_t address,
    int rf)
{
  return safe_function(__func__, [=] {
    if (!address)
    {
      ossia_log_error("ossia_node_set_hidden: node is null");
      return;
    }

    convert_parameter(address)->set_repetition_filter(
          rf
          ? ossia::repetition_filter::ON
          : ossia::repetition_filter::OFF);
  });
}

int ossia_parameter_get_repetition_filter(
    ossia_parameter_t address)
{
  return safe_function(__func__, [=]() -> int {
    if (!address)
    {
      ossia_log_error("ossia_node_get_hidden: node is null");
      return 0;
    }

    auto rf =  convert_parameter(address)->get_repetition_filter();
    return (rf == ossia::repetition_filter::ON) ? 1 : 0;
  });
}

}
