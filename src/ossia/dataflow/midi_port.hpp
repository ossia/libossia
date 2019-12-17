#pragma once
#include <rtmidi17/message.hpp>
#include <ossia/dataflow/value_vector.hpp>

namespace ossia
{

struct midi_port
{
  static const constexpr int which = 1;

  value_vector<rtmidi::message> messages;
};

struct midi_delay_line
{
  std::vector<value_vector<rtmidi::message>> messages;
};

}
