#pragma once
#include <ossia/network/base/address.hpp>
#include <ossia/network/midi/detail/channel.hpp>

namespace ossia
{
namespace net
{
namespace midi
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
    PC_N       // /12/PC/32 Impulse
  };

  ossia::val_type matchingType()
  {
    switch (type)
    {
      case Type::NoteOn:
      case Type::NoteOff:
      case Type::CC:
        return ossia::val_type::TUPLE;
      case Type::NoteOn_N:
      case Type::NoteOff_N:
      case Type::CC_N:
      case Type::PC:
        return ossia::val_type::INT;
      case Type::PC_N:
        return ossia::val_type::IMPULSE;
    }
    return {};
  }

  std::string address()
  {
    switch (type)
    {
      case Type::NoteOn:
        return "/" + std::to_string(channel) + "/note/on";
      case Type::NoteOff:
        return "/" + std::to_string(channel) + "/note/off";
      case Type::CC:
        return "/" + std::to_string(channel) + "/CC";
      case Type::NoteOn_N:
        return "/" + std::to_string(channel) + "/note/on/"
               + std::to_string(note);
      case Type::NoteOff_N:
        return "/" + std::to_string(channel) + "/note/off/"
               + std::to_string(note);
      case Type::CC_N:
        return "/" + std::to_string(channel) + "/CC/" + std::to_string(note);
      case Type::PC:
        return "/" + std::to_string(channel) + "/PC";
      case Type::PC_N:
        return "/" + std::to_string(channel) + "/PC/" + std::to_string(note);
    }
    return {};
  }

  ossia::value defaultValue(midi_size_t val)
  {
    switch (type)
    {
      case Type::NoteOn:
      case Type::NoteOff:
      case Type::CC:
        return ossia::Tuple{ossia::Int{val}, ossia::Int{val}};
      case Type::NoteOn_N:
      case Type::NoteOff_N:
      case Type::CC_N:
      case Type::PC:
        return ossia::Int{val};
      case Type::PC_N:
        return ossia::Impulse{};
    }
    return {};
  }

  ossia::net::domain defaultDomain()
  {

    return ossia::net::make_domain(defaultValue(0), defaultValue(127));
  }

  address_info(Type t) : type{t}
  {
  }

  address_info(Type t, midi_size_t n) : type{t}, note{n}
  {
  }

  address_info(midi_size_t chan, Type t, midi_size_t n)
      : channel{chan}, type{t}, note{n}
  {
  }
  midi_size_t channel{};
  Type type{};
  midi_size_t note{};
};

class midi_address final : public ossia::net::address_base
{
  address_info mInfo;
  ossia::net::node_base& mParent;
  midi_protocol& mProtocol;
  ossia::net::domain mDomain;

  ossia::val_type mType = ossia::val_type::INT;
  value mValue;
  std::string mAddress;

public:
  midi_address(address_info info, ossia::net::node_base& parent);

  const address_info& info() const;

  const ossia::net::node_base& getNode() const override;

  void pullValue() override;
  address_base& pushValue(const value& val) override;
  address_base& pushValue() override;
  const value& getValue() const;

  value cloneValue(destination_index) const override;
  address_base& setValue(const value& v) override;

  ossia::val_type getValueType() const override;
  address_base& setValueType(ossia::val_type) override;

  ossia::access_mode getAccessMode() const override;
  address_base& setAccessMode(ossia::access_mode) override;

  const ossia::net::domain& getDomain() const override;
  address_base& setDomain(const ossia::net::domain&) override;

  ossia::bounding_mode getBoundingMode() const override;
  address_base& setBoundingMode(ossia::bounding_mode) override;

  ossia::repetition_filter getRepetitionFilter() const override;
  address_base& setRepetitionFilter(ossia::repetition_filter) override;

  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

  const std::string& getTextualAddress() const override;

  void valueCallback(const ossia::value& val);
};
}
}
}
