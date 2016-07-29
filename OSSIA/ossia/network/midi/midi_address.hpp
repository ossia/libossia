#pragma once
#include <ossia/network/base/address.hpp>
#include <ossia/network/midi/detail/channel.hpp>

namespace OSSIA
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

    OSSIA::Type matchingType()
    {
      switch(type)
      {
        case Type::NoteOn:
        case Type::NoteOff:
        case Type::CC:
          return OSSIA::Type::TUPLE;
        case Type::NoteOn_N:
        case Type::NoteOff_N:
        case Type::CC_N:
        case Type::PC:
          return OSSIA::Type::INT;
        case Type::PC_N:
          return OSSIA::Type::IMPULSE;
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

    OSSIA::Value defaultValue(midi_size_t val)
    {
      switch(type)
      {
        case Type::NoteOn:
        case Type::NoteOff:
        case Type::CC:
          return OSSIA::Tuple{OSSIA::Int{val}, OSSIA::Int{val}};
        case Type::NoteOn_N:
        case Type::NoteOff_N:
        case Type::CC_N:
        case Type::PC:
          return OSSIA::Int{val};
        case Type::PC_N:
          return OSSIA::Impulse{};
      }
      return {};
    }

    OSSIA::net::Domain defaultDomain()
    {

      return OSSIA::net::makeDomain(
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
    public OSSIA::net::Address
{
    MIDIAddressInfo mInfo;
    OSSIA::net::Node& mParent;
    MIDI& mProtocol;
    OSSIA::net::Domain mDomain;

    OSSIA::Type mType = OSSIA::Type::INT;
    Value mValue;
    std::string mAddress;
  public:
    MIDIAddress(MIDIAddressInfo info, OSSIA::net::Node& parent);

    const MIDIAddressInfo& info() const;

    const OSSIA::net::Node& getNode() const override;

    void pullValue() override;
    Address& pushValue(const Value& val) override;
    Address& pushValue() override;
    const Value& getValue() const;


    Value cloneValue(DestinationIndex) const override;
    Address& setValue(const Value& v) override;


    OSSIA::Type getValueType() const override;
    Address& setValueType(OSSIA::Type) override;

    OSSIA::AccessMode getAccessMode() const override;
    Address& setAccessMode(OSSIA::AccessMode) override;

    const OSSIA::net::Domain& getDomain() const override;
    Address& setDomain(const OSSIA::net::Domain&) override;


    OSSIA::BoundingMode getBoundingMode() const override;
    Address&setBoundingMode(OSSIA::BoundingMode) override;

    OSSIA::RepetitionFilter getRepetitionFilter() const override;
    Address&setRepetitionFilter(OSSIA::RepetitionFilter) override;

    void onFirstCallbackAdded() override;
    void onRemovingLastCallback() override;

    const std::string& getTextualAddress() const override;

    void valueCallback(const OSSIA::Value& val);
};
}
}
