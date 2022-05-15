#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/value_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
#include <ossia/detail/nullable_variant.hpp>

namespace ossia
{

struct immediate_glutton_connection
{
};
struct immediate_strict_connection
{
  enum required_sides_t
  {
    inbound = 1 << 0,
    outbound = 1 << 1,
    both = inbound | outbound
  };

  required_sides_t required_sides{both};
};

struct delayed_glutton_connection
{
  // delayed at the source or at the target
  delay_line_type buffer;
  std::size_t pos{};
};
struct delayed_strict_connection
{
  // same
  delay_line_type buffer;
  std::size_t pos{};
};

// An explicit dependency
struct dependency_connection
{
};

using connection = ossia::nullable_variant<
    immediate_glutton_connection, immediate_strict_connection,
    delayed_glutton_connection, delayed_strict_connection,
    dependency_connection>;
}
