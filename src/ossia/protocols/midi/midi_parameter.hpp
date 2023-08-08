#pragma once
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/protocols/midi/detail/channel.hpp>

namespace ossia::net
{
class protocol_base;
}
namespace ossia::net::midi
{
class midi_protocol;
struct address_info
{
  enum class Type : midi_size_t
  {
    NoteOn,    // /12/note/on 64 127 -> C3 at 127 velocity
    NoteOn_N,  // /12/note/on/64 127 -> C3 at 127 velocity
    NoteOff,   // /12/note/off 64 127
    NoteOff_N, // /12/note/off/64 127
    CC,        // /12/CC 64 123
    CC_N,      // /12/CC/64 123,
    PC,        // /12/PC 32
    PC_N,      // /12/PC/32 Impulse
    PB,        // /12/PB -8192 -> 8191
    Any
  };

  ossia::val_type matchingType()
  {
    switch(type)
    {
      case Type::NoteOn:
      case Type::NoteOff:
      case Type::CC:
      case Type::Any:
        return ossia::val_type::LIST;
      case Type::NoteOn_N:
      case Type::NoteOff_N:
      case Type::CC_N:
      case Type::PC:
      case Type::PB:
        return ossia::val_type::INT;
      case Type::PC_N:
        return ossia::val_type::IMPULSE;
    }
    return {};
  }

  static const std::string* num_table_init() noexcept
  {
    static std::string num_table[257];
    for(int i = 0; i < 257; i++)
    {
      num_table[i] = std::to_string(i);
    }
    return num_table;
  }
  static const std::string* num_table() noexcept
  {
    static auto ptr = num_table_init();
    return ptr;
  }

  std::string address()
  {
    auto nums = num_table();
    switch(type)
    {
      case Type::NoteOn:
        return "/" + nums[channel] + "/note/on";
      case Type::NoteOff:
        return "/" + nums[channel] + "/note/off";
      case Type::CC:
        return "/" + nums[channel] + "/CC";
      case Type::NoteOn_N:
        return "/" + nums[channel] + "/note/on/" + nums[note];
      case Type::NoteOff_N:
        return "/" + nums[channel] + "/note/off/" + nums[note];
      case Type::CC_N:
        return "/" + nums[channel] + "/CC/" + nums[note];
      case Type::PC:
        return "/" + nums[channel] + "/PC";
      case Type::PC_N:
        return "/" + nums[channel] + "/PC/" + nums[note];
      case Type::PB:
        return "/" + nums[channel] + "/PB";
      case Type::Any:
        return "/";
    }
    return {};
  }

  ossia::value defaultValue(int32_t val)
  {
    switch(type)
    {
      case Type::NoteOn:
      case Type::NoteOff:
      case Type::CC:
        return value{std::vector<ossia::value>{int32_t{val}, int32_t{val}}};
      case Type::Any:
        return value{std::vector<ossia::value>{}};
      case Type::NoteOn_N:
      case Type::NoteOff_N:
      case Type::CC_N:
      case Type::PC:
      case Type::PB:
        return int32_t{val};
      case Type::PC_N:
        return ossia::impulse{};
    }
    return {};
  }

  ossia::domain defaultDomain()
  {
    if(type != Type::PB && type != Type::Any)
      return ossia::make_domain(defaultValue(0), defaultValue(127));
    return ossia::make_domain(0, 16384);
  }

  address_info(Type t)
      : type{t}
  {
  }

  address_info(Type t, midi_size_t n)
      : type{t}
      , note{n}
  {
  }

  address_info(midi_size_t chan, Type t, midi_size_t n)
      : channel{chan}
      , type{t}
      , note{n}
  {
  }
  midi_size_t channel{};
  Type type{};
  midi_size_t note{};
};

class midi_parameter : public ossia::net::parameter_base
{
public:
  midi_parameter(address_info info, ossia::net::node_base& parent);
  ossia::net::protocol_base& get_protocol() const noexcept override
  {
    return m_protocol;
  }

  const address_info& info() const;

  void pull_value() final override;
  parameter_base& push_value(const ossia::value& val) final override;
  parameter_base& push_value(ossia::value&& val) final override;
  parameter_base& push_value() final override;
  const ossia::value& getValue() const;

  ossia::value value() const final override;
  ossia::value set_value(const ossia::value& v) final override;
  ossia::value set_value(ossia::value&& v) final override;

  ossia::val_type get_value_type() const noexcept final override;
  parameter_base& set_value_type(ossia::val_type) final override;

  ossia::access_mode get_access() const noexcept final override;
  parameter_base& set_access(ossia::access_mode) final override;

  const ossia::domain& get_domain() const noexcept final override;
  parameter_base& set_domain(const ossia::domain&) final override;

  ossia::bounding_mode get_bounding() const noexcept final override;
  parameter_base& set_bounding(ossia::bounding_mode) final override;

  void on_first_callback_added() final override;
  void on_removing_last_callback() final override;

  void value_callback(const ossia::value& val);

protected:
  address_info m_info;
  ossia::net::protocol_base& m_protocol;
  ossia::domain m_domain;

  ossia::val_type m_type = ossia::val_type::INT;
  ossia::value m_value;
};
}
