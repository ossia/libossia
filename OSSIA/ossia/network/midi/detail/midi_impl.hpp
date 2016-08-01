#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/midi/detail/channel.hpp>

#include <ossia/network/midi/midi_address.hpp>
#include <ossia/network/midi/midi_device.hpp>
#include <ossia/network/midi/midi_node.hpp>

#include <iostream>

namespace ossia
{
namespace net
{
namespace midi
{
class note_on_N_node final : public midi_node
{
  const std::string mName;

public:
  note_on_N_node(
      midi_size_t note, midi_device& aDevice, ossia::net::node_base& aParent)
      : midi_node{aDevice, aParent}, mName{std::to_string(note)}
  {
  }

  std::string getName() const final override
  {
    return mName;
  }

  void init(midi_size_t channel, midi_size_t note)
  {
    mAddress = std::make_unique<midi_address>(
        address_info{channel, address_info::Type::NoteOn_N, note}, *this);
  }
};

class note_off_N_node final : public midi_node
{
  const std::string mName;

public:
  note_off_N_node(
      midi_size_t note, midi_device& aDevice, ossia::net::node_base& aParent)
      : midi_node{aDevice, aParent}, mName{std::to_string(note)}
  {
  }

  std::string getName() const final override
  {
    return mName;
  }

  void init(midi_size_t channel, midi_size_t note)
  {
    mAddress = std::make_unique<midi_address>(
        address_info{channel, address_info::Type::NoteOff_N, note}, *this);
  }
};

class control_N_node final : public midi_node
{
  const std::string mName;

public:
  control_N_node(
      midi_size_t param, midi_device& aDevice, ossia::net::node_base& aParent)
      : midi_node{aDevice, aParent}, mName{std::to_string(param)}
  {
  }

  std::string getName() const final override
  {
    return mName;
  }

  void init(midi_size_t channel, midi_size_t param)
  {
    mAddress = std::make_unique<midi_address>(
        address_info{channel, address_info::Type::CC_N, param}, *this);
  }
};

class program_N_node final : public midi_node
{
  const std::string mName;

public:
  program_N_node(
      midi_size_t param, midi_device& aDevice, ossia::net::node_base& aParent)
      : midi_node{aDevice, aParent}, mName{std::to_string(param)}
  {
  }

  std::string getName() const final override
  {
    return mName;
  }

  void init(midi_size_t channel, midi_size_t param)
  {
    mAddress = std::make_unique<midi_address>(
        address_info{channel, address_info::Type::PC_N, param}, *this);
  }
};

class program_node final : public midi_node
{
public:
  program_node(midi_device& aDevice) : midi_node(aDevice, aDevice)
  {
  }

  std::string getName() const final override
  {
    return "program";
  }

  void init(midi_size_t channel)
  {
    mAddress = std::make_unique<midi_address>(
        address_info{channel, address_info::Type::PC, 0}, *this);

    for (int i = 0; i < 127; i++)
    {
      auto ptr = std::make_unique<program_N_node>(i, mDevice, *this);
      ptr->init(channel, i);
      mChildren.push_back(std::move(ptr));
    }
  }
};

class note_on_node final : public midi_node
{
public:
  note_on_node(midi_device& aDevice) : midi_node(aDevice, aDevice)
  {
  }

  std::string getName() const final override
  {
    return "on";
  }

  void init(midi_size_t channel)
  {
    mAddress = std::make_unique<midi_address>(
        address_info{channel, address_info::Type::NoteOn, 0}, *this);

    for (int i = 0; i < 127; i++)
    {
      auto ptr = std::make_unique<note_on_N_node>(i, mDevice, *this);
      ptr->init(channel, i);
      mChildren.push_back(std::move(ptr));
    }
  }
};

class note_off_node final : public midi_node
{
public:
  note_off_node(midi_device& aDevice) : midi_node(aDevice, aDevice)
  {
  }

  std::string getName() const final override
  {
    return "off";
  }

  void init(midi_size_t channel)
  {
    mAddress = std::make_unique<midi_address>(
        address_info{channel, address_info::Type::NoteOff, 0}, *this);

    for (int i = 0; i < 127; i++)
    {
      auto ptr = std::make_unique<note_off_N_node>(i, mDevice, *this);
      ptr->init(channel, i);
      mChildren.push_back(std::move(ptr));
    }
  }
};

class control_node final : public midi_node
{
public:
  control_node(midi_device& aDevice) : midi_node(aDevice, aDevice)
  {
  }

  std::string getName() const final override
  {
    return "control";
  }

  void init(midi_size_t channel)
  {
    mAddress = std::make_unique<midi_address>(
        address_info{channel, address_info::Type::CC, 0}, *this);

    for (int i = 0; i < 127; i++)
    {
      auto ptr = std::make_unique<control_N_node>(i, mDevice, *this);
      ptr->init(channel, i);
      mChildren.push_back(std::move(ptr));
    }
  }
};

class channel_node final : public midi_node
{
  const midi_size_t mChannel;
  const std::string mName;

public:
  channel_node(midi_size_t channel, midi_device& aDevice)
      : midi_node(aDevice, aDevice)
      , mChannel{channel}
      , mName(std::to_string(channel))
  {
  }

  std::string getName() const final override
  {
    return mName;
  }

  void init()
  {
    {
      auto non = std::make_unique<note_on_node>(mDevice);
      non->init(mChannel);
      mChildren.push_back(std::move(non));
    }
    {
      auto noff = std::make_unique<note_off_node>(mDevice);
      noff->init(mChannel);
      mChildren.push_back(std::move(noff));
    }
    {
      auto cc = std::make_unique<control_node>(mDevice);
      cc->init(mChannel);
      mChildren.push_back(std::move(cc));
    }
    {
      auto pc = std::make_unique<program_node>(mDevice);
      pc->init(mChannel);
      mChildren.push_back(std::move(pc));
    }
  }
};
}
}
}
