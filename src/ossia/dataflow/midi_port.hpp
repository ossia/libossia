#pragma once
#include <rtmidi17/message.hpp>
#include <ossia/dataflow/value_vector.hpp>

namespace ossia
{

struct midi_port
{
  static const constexpr int which = 1;

  value_vector<rtmidi::message> messages;

  using message = rtmidi::message;
  using message_type = rtmidi::message_type;
  using midi_bytes = rtmidi::midi_bytes;


  message& note_on(uint8_t channel, uint8_t note, uint8_t velocity) noexcept
  {
    return create(make_command(message_type::NOTE_ON, channel), note, velocity);
  }

  message& note_off(uint8_t channel, uint8_t note, uint8_t velocity) noexcept
  {
    return create(make_command(message_type::NOTE_OFF, channel), note, velocity);
  }

  message& control_change(uint8_t channel, uint8_t control, uint8_t value) noexcept
  {
    return create(make_command(message_type::CONTROL_CHANGE, channel), control, value);
  }

  message& program_change(uint8_t channel, uint8_t value) noexcept
  {
    return create(make_command(message_type::PROGRAM_CHANGE, channel), value);
  }

  message& pitch_bend(uint8_t channel, int value) noexcept
  {
    return create(make_command(message_type::PITCH_BEND, channel),
                       (unsigned char)(value & 0x7F), (uint8_t)((value >> 7) & 0x7F));
  }

  message& pitch_bend(uint8_t channel, uint8_t lsb, uint8_t msb) noexcept
  {
    return create(make_command(message_type::PITCH_BEND, channel), lsb, msb);
  }

  message& poly_pressure(uint8_t channel, uint8_t note, uint8_t value) noexcept
  {
    return create(make_command(message_type::POLY_PRESSURE, channel), note, value);
  }

  message& aftertouch(uint8_t channel, uint8_t value) noexcept
  {
    return create(make_command(message_type::AFTERTOUCH, channel), value);
  }
private:
  static uint8_t make_command(const message_type type, const int channel) noexcept
  {
    return (uint8_t)((uint8_t)type | rtmidi::clamp(channel, 0, channel - 1));
  }

  message& create(uint8_t b0, uint8_t b1) noexcept
  {
    return messages.emplace_back(b0, b1);
  }
  message& create(uint8_t b0, uint8_t b1, uint8_t b2) noexcept
  {
    return messages.emplace_back(b0, b1, b2);
  }
};

struct midi_delay_line
{
  std::vector<value_vector<rtmidi::message>> messages;
};

}
