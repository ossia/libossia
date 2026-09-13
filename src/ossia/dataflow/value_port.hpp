#pragma once
#include <ossia/dataflow/timed_value.hpp>
#include <ossia/dataflow/typed_value.hpp>
#include <ossia/dataflow/value_vector.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain_base.hpp>

#include <optional>

namespace ossia
{
enum data_mix_method : int8_t
{
  mix_append,
  mix_replace,
  mix_merge
};

struct OSSIA_EXPORT value_port
{
  static const constexpr int which = 2;
  //! Use this function to write from a node to an output port
  void write_value(const ossia::value& v, int64_t timestamp);

  void write_value(ossia::value&& v, int64_t timestamp);

  void add_local_value(const ossia::typed_value& other);

  void add_port_values(const ossia::value_port& other);

  void add_global_values(
      const ossia::net::parameter_base& param, const value_vector<ossia::value>& vec);

  void add_global_value(const ossia::net::parameter_base& other, ossia::value v);

  void set_data(const value_vector<ossia::timed_value>& vec);

  void clear();

  [[nodiscard]] const value_vector<ossia::timed_value>& get_data() const noexcept
  {
    return data;
  }
  value_vector<ossia::timed_value>& get_data() noexcept { return data; }

  ossia::domain domain;

  //! What the node declares its values are. Set when the node is built.
  ossia::complex_type type;

  //! The @[unit] of the port's address, and what the parameter at the other end
  //! speaks. Goes away with the address.
  ossia::unit_t address_unit;

  //! What values on this port are actually expressed in.
  //!
  //! A port that says what it is comes first. `type` is what its owner reads --
  //! a colour control that works in rgba, a slider in decibels -- and a value
  //! reaching it is converted into that. The address only says what is spoken at
  //! the far end, which is where a value is converted *from*; letting it decide
  //! here left the value in the remote unit while everything reading the port
  //! went on believing the declared one, so an argb colour arrived through a
  //! port that meant rgba and came out with its channels one place along. It
  //! answers for ports that declare nothing of their own, which is what it is
  //! for.
  [[nodiscard]] ossia::complex_type effective_type() const noexcept
  {
    if(type)
      return type;
    return address_unit;
  }

  //! Cheaper than building the effective type just to test it.
  [[nodiscard]] bool has_effective_type() const noexcept
  {
    return bool(address_unit) || bool(type);
  }

  //! Same, for the callers that only need the unit.
  [[nodiscard]] const ossia::unit_t* effective_unit() const noexcept
  {
    if(auto u = type.target<ossia::unit_t>(); u && bool(*u))
      return u;
    if(address_unit)
      return &address_unit;
    return type.target<ossia::unit_t>();
  }

  ossia::destination_index index;
  // std::optional<ossia::time_value> tween_date;

  bool is_event{};
  data_mix_method mix_method{};

private:
  value_vector<ossia::timed_value> data;
};

struct value_delay_line
{
  std::vector<value_vector<ossia::typed_value>> data;
};

OSSIA_EXPORT
void process_control_value(
    ossia::value& v, const ossia::domain& source_domain,
    const ossia::domain& sink_domain) noexcept;
OSSIA_EXPORT
void process_control_value(
    ossia::value& v, const ossia::complex_type& source_type,
    const ossia::complex_type& sink_type) noexcept;
OSSIA_EXPORT
void process_control_value(
    ossia::value& v, const ossia::domain& source_domain,
    const ossia::domain& sink_domain, const ossia::complex_type& source_type,
    const ossia::complex_type& sink_type) noexcept;
}
