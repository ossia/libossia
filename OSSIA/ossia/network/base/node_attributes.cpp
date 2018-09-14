#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>

namespace ossia
{
namespace net
{
#define OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(Type, Name, String) \
  optional<Type> get_##Name(const extended_attributes& n)      \
  {                                                            \
    return get_optional_attribute<Type>(n, text_##Name());     \
  }                                                            \
                                                               \
  void set_##Name(extended_attributes& n, optional<Type> i)    \
  {                                                            \
    set_optional_attribute(n, String, std::move(i));           \
  }                                                            \
  void set_##Name(ossia::net::node_base& n, optional<Type> i)  \
  {                                                            \
    n.set(ossia::string_view(String), std::move(i));           \
  }                                                            \
  ossia::string_view text_##Name()                             \
  {                                                            \
    constexpr_return(ossia::make_string_view(String));         \
  }

#define OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_2(Type, Name, String) \
  optional<Type> get_##Name(const extended_attributes& n)        \
  {                                                              \
    return get_optional_attribute<Type>(n, text_##Name());       \
  }                                                              \
  void set_##Name(extended_attributes& n, Type&& i)              \
  {                                                              \
    set_attribute(n, String, std::move(i));                      \
  }                                                              \
  void set_##Name(extended_attributes& n, const Type& i)         \
  {                                                              \
    set_attribute(n, String, i);                                 \
  }                                                              \
  void set_##Name(extended_attributes& n, ossia::none_t i)       \
  {                                                              \
    set_attribute(n, String, std::move(i));                      \
  }                                                              \
  void set_##Name(ossia::net::node_base& n, Type&& i)            \
  {                                                              \
    n.set(ossia::string_view(String), std::move(i));             \
  }                                                              \
  void set_##Name(ossia::net::node_base& n, const Type& i)       \
  {                                                              \
    n.set(ossia::string_view(String), i);                        \
  }                                                              \
  void set_##Name(ossia::net::node_base& n, ossia::none_t i)     \
  {                                                              \
    n.set(ossia::string_view(String), optional<Type>{});         \
  }                                                              \
  ossia::string_view text_##Name()                               \
  {                                                              \
    constexpr_return(ossia::make_string_view(String));           \
  }

#define OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_BOOL(Type, Name, String) \
  Type get_##Name(const extended_attributes& n)                     \
  {                                                                 \
    return has_attribute(n, text_##Name());                         \
  }                                                                 \
                                                                    \
  void set_##Name(extended_attributes& n, Type i)                   \
  {                                                                 \
    if (i)                                                          \
      set_attribute(n, String);                                     \
    else                                                            \
      set_attribute(n, String, ossia::none);                        \
  }                                                                 \
  void set_##Name(ossia::net::node_base& n, Type i)                 \
  {                                                                 \
    n.set(ossia::string_view(String), i);                           \
  }                                                                 \
  ossia::string_view text_##Name()                                  \
  {                                                                 \
    constexpr_return(ossia::make_string_view(String));              \
  }

OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(
    instance_bounds, instance_bounds, "instanceBounds")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(tags, tags, "tags")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(description, description, "description")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(priority, priority, "priority")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(refresh_rate, refresh_rate, "refreshRate")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_BOOL(recall_safe, recall_safe, "recallSafe")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(
    value_step_size, value_step_size, "valueStepsize")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_BOOL(zombie, zombie, "zombie")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_BOOL(hidden, hidden, "hidden")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_name, app_name, "appName")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_version, app_version, "appVersion")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_creator, app_creator, "appCreator")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_2(
    default_value, default_value, "valueDefault")

ossia::string_view text_extended_type()
{
  constexpr_return(ossia::make_string_view("extended_type"));
}

optional<extended_type> get_extended_type(const ossia::net::node_base& n)
{
  auto opt = get_optional_attribute<extended_type>(n, text_extended_type());
  if (!opt)
  {
    if (parameter_base* addr = n.get_parameter())
    {
      switch (addr->get_value_type())
      {
        case ossia::val_type::VEC2F:
        case ossia::val_type::VEC3F:
        case ossia::val_type::VEC4F:
          return float_array_type();
        case ossia::val_type::LIST:
          return list_type();
        default:
          break;
      }
    }
  }
  return opt;
}

void set_extended_type(extended_attributes& n, optional<extended_type> i)
{
  set_optional_attribute(n, text_extended_type(), std::move(i));
}
void set_extended_type(ossia::net::node_base& n, optional<extended_type> i)
{
  n.set(text_extended_type(), std::move(i));
}

void set_description(extended_attributes& n, const char* arg)
{
  if (arg)
    set_description(n, std::string{arg});
  else
    set_description(n, ossia::none);
}

void set_description(ossia::net::node_base& n, const char* arg)
{
  if (arg)
    set_description(n, std::string{arg});
  else
    set_description(n, ossia::none);
}

void set_app_name(extended_attributes& n, const char* arg)
{
  if (arg)
    set_app_name(n, std::string{arg});
  else
    set_app_name(n, ossia::none);
}

void set_app_name(ossia::net::node_base& n, const char* arg)
{
  if (arg)
    set_app_name(n, std::string{arg});
  else
    set_app_name(n, ossia::none);
}

void set_app_creator(extended_attributes& n, const char* arg)
{
  if (arg)
    set_app_creator(n, std::string{arg});
  else
    set_app_creator(n, ossia::none);
}

void set_app_creator(ossia::net::node_base& n, const char* arg)
{
  if (arg)
    set_app_creator(n, std::string{arg});
  else
    set_app_creator(n, ossia::none);
}

void set_app_version(extended_attributes& n, const char* arg)
{
  if (arg)
    set_app_version(n, std::string{arg});
  else
    set_app_version(n, ossia::none);
}

void set_app_version(ossia::net::node_base& n, const char* arg)
{
  if (arg)
    set_app_version(n, std::string{arg});
  else
    set_app_version(n, ossia::none);
}

void set_default_value(extended_attributes& n, int arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, long arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, bool arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, char arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, float arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, double arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, const char* arg)
{
  set_default_value(n, ossia::value{std::string(arg)});
}

void set_default_value(ossia::net::node_base& n, int arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, long arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, bool arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, char arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, float arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, double arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, const char* arg)
{
  set_default_value(n, ossia::value{std::string(arg)});
}

// Address-related getters - setters
ossia::string_view text_value()
{
  constexpr_return(ossia::make_string_view("value"));
}
value clone_value(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->value();
  return {};
}
void set_value(ossia::net::node_base& n, value v)
{
  if (auto addr = n.get_parameter())
    addr->set_value(std::move(v));
}
void set_value(ossia::net::parameter_data& n, value v)
{
  n.value = std::move(v);
}

ossia::string_view text_value_type()
{
  constexpr_return(ossia::make_string_view("type"));
}
optional<val_type> get_value_type(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->get_value_type();
  return ossia::none;
}
void set_value_type(ossia::net::node_base& n, val_type v)
{
  if (auto addr = n.get_parameter())
    addr->set_value_type(std::move(v));
}

ossia::string_view text_domain()
{
  constexpr_return(ossia::make_string_view("domain"));
}
domain get_domain(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->get_domain();
  return {};
}
void set_domain(ossia::net::node_base& n, domain v)
{
  if (auto addr = n.get_parameter())
    addr->set_domain(std::move(v));
}
void set_domain(ossia::net::parameter_data& n, domain v)
{
  n.domain = std::move(v);
}

ossia::string_view text_access_mode()
{
  constexpr_return(ossia::make_string_view("access"));
}
optional<access_mode> get_access_mode(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->get_access();
  return ossia::none;
}
void set_access_mode(ossia::net::node_base& n, access_mode v)
{
  if (auto addr = n.get_parameter())
    addr->set_access(std::move(v));
}
void set_access_mode(ossia::net::parameter_data& n, access_mode v)
{
  n.access = v;
}

ossia::string_view text_bounding_mode()
{
  constexpr_return(ossia::make_string_view("bounding"));
}
optional<bounding_mode> get_bounding_mode(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->get_bounding();
  return ossia::none;
}
void set_bounding_mode(ossia::net::node_base& n, bounding_mode v)
{
  if (auto addr = n.get_parameter())
    addr->set_bounding(std::move(v));
}
void set_bounding_mode(ossia::net::parameter_data& n, bounding_mode v)
{
  n.bounding = v;
}

ossia::string_view text_muted()
{
  constexpr_return(ossia::make_string_view("muted"));
}
muted get_muted(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->get_muted();
  return false;
}
void set_muted(ossia::net::node_base& n, muted v)
{
  if (auto addr = n.get_parameter())
    addr->set_muted(v);
}
void set_muted(ossia::net::parameter_data& n, muted v)
{
  n.muted = v;
}

ossia::string_view text_disabled()
{
  constexpr_return(ossia::make_string_view("disabled"));
}
disabled get_disabled(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->get_disabled();
  return false;
}
void set_disabled(ossia::net::node_base& n, disabled v)
{
  if (auto addr = n.get_parameter())
    addr->set_disabled(v);
}
void set_disabled(ossia::net::parameter_data& n, disabled v)
{
  n.disabled = v;
}

ossia::string_view text_critical()
{
  constexpr_return(ossia::make_string_view("critical"));
}
critical get_critical(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->get_critical();
  return false;
}
void set_critical(ossia::net::node_base& n, critical v)
{
  if (auto addr = n.get_parameter())
    addr->set_critical(v);
}
void set_critical(ossia::net::parameter_data& n, critical v)
{
  n.critical = v;
}

ossia::string_view text_repetition_filter()
{
  constexpr_return(ossia::make_string_view("repetition_filter"));
}
repetition_filter get_repetition_filter(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->get_repetition_filter();
  return repetition_filter::OFF;
}
void set_repetition_filter(ossia::net::node_base& n, repetition_filter v)
{
  if (auto addr = n.get_parameter())
    addr->set_repetition_filter(std::move(v));
}

ossia::string_view text_unit()
{
  constexpr_return(ossia::make_string_view("unit"));
}
unit_t get_unit(const ossia::net::node_base& n)
{
  if (auto addr = n.get_parameter())
    return addr->get_unit();
  return {};
}
void set_unit(ossia::net::node_base& n, unit_t v)
{
  if (auto addr = n.get_parameter())
    addr->set_unit(std::move(v));
}
}
}
