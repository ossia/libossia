#pragma once
#include <ossia/network/base/address.hpp>
#include <ossia/network/midi/detail/channel.hpp>

namespace ossia
{
namespace net
{
class MIDI;
struct MIDIAddressInfo
{
    enum class Type : midi_size_t
    {
      NoteOn, // /12/note/on 64 127 -> C3 at 127 velocity
      NoteOn_N, // /12/note/on/64 127 -> C3 at 127 velocity
      NoteOff, // /12/note/off 64 127
      NoteOff_N, // /12/note/off/64 127
      CC, // /12/CC 64 123
      CC_N, // /12/CC/64 123,
      PC, // /12/PC 32
      PC_N // /12/PC/32 Impulse
    };

    ossia::Type matchingType()
    {
      switch(type)
      {
        case Type::NoteOn:
        case Type::NoteOff:
        case Type::CC:
          return ossia::Type::TUPLE;
        case Type::NoteOn_N:
        case Type::NoteOff_N:
        case Type::CC_N:
        case Type::PC:
          return ossia::Type::INT;
        case Type::PC_N:
          return ossia::Type::IMPULSE;
      }
      return {};
    }

    std::string address()
    {
      switch(type)
      {
        case Type::NoteOn:
          return "/" + std::to_string(channel) + "/note/on";
        case Type::NoteOff:
          return "/" + std::to_string(channel) + "/note/off";
        case Type::CC:
          return "/" + std::to_string(channel) + "/CC";
        case Type::NoteOn_N:
          return "/" + std::to_string(channel) + "/note/on/" + std::to_string(note);
        case Type::NoteOff_N:
          return "/" + std::to_string(channel) + "/note/off/" + std::to_string(note);
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
      switch(type)
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

    ossia::net::Domain defaultDomain()
    {

      return ossia::net::makeDomain(
            defaultValue(0),
            defaultValue(127)
            );
    }

    MIDIAddressInfo(Type t):
      type{t}
    {

    }

    MIDIAddressInfo(Type t, midi_size_t n):
      type{t},
      note{n}
    {

    }

    MIDIAddressInfo(midi_size_t chan, Type t, midi_size_t n):
      channel{chan},
      type{t},
      note{n}
    {

    }
    midi_size_t channel{};
    Type type{};
    midi_size_t note{};
};

class MIDIAddress final :
    public ossia::net::address
{
    MIDIAddressInfo mInfo;
    ossia::net::node& mParent;
    MIDI& mProtocol;
    ossia::net::Domain mDomain;

    ossia::Type mType = ossia::Type::INT;
    value mValue;
    std::string mAddress;
  public:
    MIDIAddress(MIDIAddressInfo info, ossia::net::node& parent);

    const MIDIAddressInfo& info() const;

    const ossia::net::node& getNode() const override;

    void pullValue() override;
    address& pushValue(const value& val) override;
    address& pushValue() override;
    const value& getValue() const;


    value cloneValue(destination_index) const override;
    address& setValue(const value& v) override;


    ossia::Type getValueType() const override;
    address& setValueType(ossia::Type) override;

    ossia::AccessMode getAccessMode() const override;
    address& setAccessMode(ossia::AccessMode) override;

    const ossia::net::Domain& getDomain() const override;
    address& setDomain(const ossia::net::Domain&) override;


    ossia::BoundingMode getBoundingMode() const override;
    address&setBoundingMode(ossia::BoundingMode) override;

    ossia::RepetitionFilter getRepetitionFilter() const override;
    address&setRepetitionFilter(ossia::RepetitionFilter) override;

    void onFirstCallbackAdded() override;
    void onRemovingLastCallback() override;

    const std::string& getTextualAddress() const override;

    void valueCallback(const ossia::value& val);
};
}
}
