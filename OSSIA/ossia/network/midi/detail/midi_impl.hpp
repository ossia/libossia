#pragma once
#include <ossia/network/domain.hpp>
#include <ossia/network/midi/detail/channel.hpp>
#include <ossia/editor/value/value.hpp>

#include <ossia/network/midi/midi_node.hpp>
#include <ossia/network/midi/midi_address.hpp>
#include <ossia/network/midi/midi_device.hpp>

#include <iostream>

namespace ossia
{
namespace net
{

class MIDINoteOn_N final : public MIDINode
{
    const std::string mName;
  public:
    MIDINoteOn_N(
        midi_size_t note,
        MIDIDevice& aDevice,
        ossia::net::node& aParent):
      MIDINode{aDevice, aParent},
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

class MIDINoteOff_N final : public MIDINode
{
    const std::string mName;
  public:
    MIDINoteOff_N(
        midi_size_t note,
        MIDIDevice& aDevice,
        ossia::net::node& aParent):
      MIDINode{aDevice, aParent},
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

class MIDI_CC_N final : public MIDINode
{
    const std::string mName;
  public:
    MIDI_CC_N(
        midi_size_t param,
        MIDIDevice& aDevice,
        ossia::net::node& aParent):
      MIDINode{aDevice, aParent},
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

class MIDI_PC_N final : public MIDINode
{
    const std::string mName;
  public:
    MIDI_PC_N(
        midi_size_t param,
        MIDIDevice& aDevice,
        ossia::net::node& aParent):
      MIDINode{aDevice, aParent},
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

class MIDI_PC final : public MIDINode
{
  public:
    MIDI_PC(MIDIDevice& aDevice):
      MIDINode(aDevice, aDevice)
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

class MIDINoteOn final : public MIDINode
{
  public:
    MIDINoteOn(MIDIDevice& aDevice):
      MIDINode(aDevice, aDevice)
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

class MIDINoteOff final : public MIDINode
{
  public:
    MIDINoteOff(MIDIDevice& aDevice):
      MIDINode(aDevice, aDevice)
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

class MIDI_CC final : public MIDINode
{
  public:
    MIDI_CC(MIDIDevice& aDevice):
      MIDINode(aDevice, aDevice)
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

class MIDIChannel final : public MIDINode
{
    const midi_size_t mChannel;
    const std::string mName;
  public:
    MIDIChannel(midi_size_t channel, MIDIDevice& aDevice):
      MIDINode(aDevice, aDevice),
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

}
}
