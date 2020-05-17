#pragma once
#include <ossia/dataflow/timed_value.hpp>
#include <ossia/dataflow/typed_value.hpp>
#include <ossia/dataflow/value_vector.hpp>
#include <ossia/network/domain/domain_base.hpp>
#include <ossia/editor/scenario/time_value.hpp>
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

  ossia::value filter_value(
      const ossia::value& source,
      const ossia::destination_index& source_idx, // TODO handle me
      const ossia::complex_type& source_type) const;

  void add_local_value(const ossia::typed_value& other);

  void add_port_values(const ossia::value_port& other);

  void add_global_values(
      const ossia::net::parameter_base& param,
      const value_vector<ossia::value>& vec);

  void add_global_value(
      const ossia::net::parameter_base& other, const ossia::value& v);

  void set_data(const value_vector<ossia::timed_value>& vec);

  void clear();

  const value_vector<ossia::timed_value>& get_data() const;
  value_vector<ossia::timed_value>& get_data();

  ossia::domain domain;
  ossia::complex_type type;
  ossia::destination_index index;
  std::optional<ossia::time_value> tween_date;

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
    ossia::value& v,
    const ossia::domain& source_domain,
    const ossia::domain& sink_domain) noexcept;
OSSIA_EXPORT
void process_control_value(
    ossia::value& v,
    const ossia::complex_type& source_type,
    const ossia::complex_type& sink_type) noexcept;
OSSIA_EXPORT
void process_control_value(
    ossia::value& v,
    const ossia::domain& source_domain,
    const ossia::domain& sink_domain,
    const ossia::complex_type& source_type,
    const ossia::complex_type& sink_type) noexcept;
}
