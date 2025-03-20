#pragma once
#include <ossia/dataflow/value_vector.hpp>

#include <libremidi/cmidi2.hpp>
#include <libremidi/ump.hpp>
#include <libremidi/ump_events.hpp>

namespace ossia
{

struct midi_port
{
  static const constexpr int which = 1;

  value_vector<libremidi::ump> messages;
};

struct midi_delay_line
{
  std::vector<value_vector<libremidi::ump>> messages;
};

}
