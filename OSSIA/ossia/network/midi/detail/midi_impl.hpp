#pragma once
#include <string>

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/domain.hpp>
#include <ossia/network/midi/detail/channel.hpp>
#include <ossia/editor/value/value.hpp>
#include <iostream>
namespace OSSIA
{
namespace net
{
class MIDI;
class MIDINodeImpl :
    public virtual OSSIA::net::Node
{
  protected:
    Device& mDevice;
    Node& mParent;
    std::unique_ptr<Address> mAddress;

  public:
    using iterator = OSSIA::Container<OSSIA::net::Node>::iterator;
    using const_iterator = OSSIA::Container<OSSIA::net::Node>::const_iterator;
    MIDINodeImpl() = default;
    MIDINodeImpl(
        Device& aDevice,
        OSSIA::net::Node& aParent):
      mDevice{aDevice},
      mParent{aParent}
    {

    }

    Device& getDevice() const final override
    {
      return mDevice;
    }

    Node* getParent() const final override
    {
      return &mParent;
    }

    Node& setName(std::string) override
    {
      return *this;
    }

    Address* getAddress() const final override
    {
      return mAddress.get();
    }

    Address* createAddress(Type) final override
    {
      return mAddress.get();
    }

    bool removeAddress() final override
    {
      return false;
    }

    std::unique_ptr<Node> makeChild(const std::string& name) final override
    {
      return nullptr;
    }

    void removingChild(Node& node) final override
    {

    }
};

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


    Address::iterator addCallback(ValueCallback callback) override;

    void removeCallback(Address::iterator callback) override;

    void valueCallback(const OSSIA::Value& val);
    const std::string& getTextualAddress() const;
};

class MIDINoteOn_N final : public MIDINodeImpl
{
    const std::string mName;
  public:
    MIDINoteOn_N(
        midi_size_t note,
        Device& aDevice,
        OSSIA::net::Node& aParent):
      MIDINodeImpl{aDevice, aParent},
      mName{std::to_string(note)}
    {
    }

    std::string getName() const final override
    {
      return mName;
    }

    void init(midi_size_t channel, midi_size_t note)
    {
      mAddress = std::make_unique<MIDIAddress>(
                   MIDIAddressInfo{channel, MIDIAddressInfo::Type::NoteOn_N, note},
                   *this);
    }
};
class MIDINoteOff_N final : public MIDINodeImpl
{
    const std::string mName;
  public:
    MIDINoteOff_N(
        midi_size_t note,
        Device& aDevice,
        OSSIA::net::Node& aParent):
      MIDINodeImpl{aDevice, aParent},
      mName{std::to_string(note)}
    {
    }

    std::string getName() const final override
    {
      return mName;
    }

    void init(midi_size_t channel, midi_size_t note)
    {
      mAddress = std::make_unique<MIDIAddress>(
                   MIDIAddressInfo{channel, MIDIAddressInfo::Type::NoteOff_N, note},
                   *this);
    }
};

class MIDI_CC_N final : public MIDINodeImpl
{
    const std::string mName;
  public:
    MIDI_CC_N(
        midi_size_t param,
        Device& aDevice,
        OSSIA::net::Node& aParent):
      MIDINodeImpl{aDevice, aParent},
      mName{std::to_string(param)}
    {
    }

    std::string getName() const final override
    {
      return mName;
    }

    void init(midi_size_t channel, midi_size_t param)
    {
      mAddress = std::make_unique<MIDIAddress>(
                   MIDIAddressInfo{channel, MIDIAddressInfo::Type::CC_N, param}, *this);
    }
};

class MIDI_PC_N final : public MIDINodeImpl
{
    const std::string mName;
  public:
    MIDI_PC_N(
        midi_size_t param,
        Device& aDevice,
        OSSIA::net::Node& aParent):
      MIDINodeImpl{aDevice, aParent},
      mName{std::to_string(param)}
    {
    }

    std::string getName() const final override
    {
      return mName;
    }

    void init(midi_size_t channel, midi_size_t param)
    {
      mAddress = std::make_unique<MIDIAddress>(
                   MIDIAddressInfo{channel, MIDIAddressInfo::Type::PC_N, param},
                   *this);
    }
};

class MIDI_PC final : public MIDINodeImpl
{
  public:
    MIDI_PC(Device& aDevice):
      MIDINodeImpl(aDevice, aDevice)
    {
    }

    std::string getName() const final override
    {
      return "program";
    }

    void init(midi_size_t channel)
    {
      mAddress = std::make_unique<MIDIAddress>(
                   MIDIAddressInfo{channel, MIDIAddressInfo::Type::PC, 0},
                   *this);

      for(int i = 0; i < 127; i++)
      {
        auto ptr = std::make_unique<MIDI_PC_N>(i, mDevice, *this);
        ptr->init(channel, i);
        mChildren.push_back(std::move(ptr));
      }
    }
};

class MIDINoteOn final : public MIDINodeImpl
{
  public:
    MIDINoteOn(Device& aDevice):
      MIDINodeImpl(aDevice, aDevice)
    {
    }

    std::string getName() const final override
    {
      return "on";
    }

    void init(midi_size_t channel)
    {
      mAddress = std::make_unique<MIDIAddress>(
                   MIDIAddressInfo{channel, MIDIAddressInfo::Type::NoteOn, 0},
                   *this);

      for(int i = 0; i < 127; i++)
      {
        auto ptr = std::make_unique<MIDINoteOn_N>(i, mDevice, *this);
        ptr->init(channel, i);
        mChildren.push_back(std::move(ptr));
      }
    }
};


class MIDINoteOff final : public MIDINodeImpl
{
  public:
    MIDINoteOff(Device& aDevice):
      MIDINodeImpl(aDevice, aDevice)
    {
    }

    std::string getName() const final override
    {
      return "off";
    }

    void init(midi_size_t channel)
    {
      mAddress = std::make_unique<MIDIAddress>(
                   MIDIAddressInfo{channel, MIDIAddressInfo::Type::NoteOff, 0},
                   *this);

      for(int i = 0; i < 127; i++)
      {
        auto ptr = std::make_unique<MIDINoteOff_N>(i, mDevice, *this);
        ptr->init(channel, i);
        mChildren.push_back(std::move(ptr));
      }
    }
};

class MIDI_CC final : public MIDINodeImpl
{
  public:
    MIDI_CC(Device& aDevice):
      MIDINodeImpl(aDevice, aDevice)
    {
    }

    std::string getName() const final override
    {
      return "control";
    }

    void init(midi_size_t channel)
    {
      mAddress = std::make_unique<MIDIAddress>(
                   MIDIAddressInfo{channel, MIDIAddressInfo::Type::CC, 0},
                   *this);

      for(int i = 0; i < 127; i++)
      {
        auto ptr = std::make_unique<MIDI_CC_N>(i, mDevice, *this);
        ptr->init(channel, i);
        mChildren.push_back(std::move(ptr));
      }
    }
};

class MIDIChannel final : public MIDINodeImpl
{
    const midi_size_t mChannel;
    const std::string mName;
  public:
    MIDIChannel(midi_size_t channel, Device& aDevice):
      MIDINodeImpl(aDevice, aDevice),
      mChannel{channel},
      mName(std::to_string(channel))
    {
    }

    std::string getName() const final override
    {
      return mName;
    }

    void init()
    {
      {
        auto non = std::make_unique<MIDINoteOn>(mDevice);
        non->init(mChannel);
        mChildren.push_back(std::move(non));
      }
      {
        auto noff = std::make_unique<MIDINoteOff>(mDevice);
        noff->init(mChannel);
        mChildren.push_back(std::move(noff));
      }
      {
        auto cc = std::make_unique<MIDI_CC>(mDevice);
        cc->init(mChannel);
        mChildren.push_back(std::move(cc));
      }
      {
        auto pc = std::make_unique<MIDI_PC>(mDevice);
        pc->init(mChannel);
        mChildren.push_back(std::move(pc));
      }
    }
};

class MIDIDeviceImpl :
    public OSSIA::net::Device,
    public MIDINodeImpl
{
    std::string mName;
  public:
    MIDIDeviceImpl(std::unique_ptr<OSSIA::net::Protocol> prot):
      OSSIA::net::Device{std::move(prot)},
      MIDINodeImpl{*this, *this}
    {
    }

    std::string getName() const override
    { return mName; }

    Node & setName(std::string n) override
    {
      mName = n;
      return *this;
    }

    bool updateNamespace() override
    {
      clearChildren();

      try {
        for(int i = 1; i <= 16; i++)
        {
          auto ptr = std::make_unique<MIDIChannel>(i, *this);
          ptr->init();
          mChildren.push_back(std::move(ptr));
        }
      }
      catch(...)
      {
        std::cerr << "refresh failed\n";
        return false;
      }
      return true;
    }
};
}
}
