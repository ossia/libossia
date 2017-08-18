// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/midi/detail/midi_impl.hpp>
#include <ossia/network/midi/midi.hpp>

#include <ossia/detail/logger.hpp>
#include <ModernMIDI/midi_input.h>
#include <ModernMIDI/midi_output.h>
namespace ossia
{
namespace net
{
namespace midi
{
midi_protocol::midi_protocol()
    : m_input{std::make_unique<mm::MidiInput>("ossia-in")}
    , m_output{std::make_unique<mm::MidiOutput>("ossia-out")}
{
}

midi_protocol::midi_protocol(midi_info m) : midi_protocol()
{
  set_info(m);
}

midi_protocol::~midi_protocol()
{
  try
  {
    m_input->closePort();
    m_output->closePort();
  }
  catch (...)
  {
    logger().error("midi_protocol::~midi_protocol() error");
  }
}

bool midi_protocol::set_info(midi_info m)
{
  try
  {
    // Close current ports
    if (m_info.type == midi_info::Type::RemoteOutput)
    {
      m_input->closePort();
    }
    else if (m_info.type == midi_info::Type::RemoteInput)
    {
      m_output->closePort();
    }

    m_info = m;

    if (m_info.type == midi_info::Type::RemoteOutput)
    {
      m_input->openPort(m_info.port);
      m_input->messageCallback = [=](mm::MidiMessage mess) {
        midi_channel& c = m_channels[mess.getChannel()];
        switch (mess.getMessageType())
        {
          case mm::MessageType::NOTE_ON:
            c.note_on.first = mess.data[1];
            c.note_on.second = mess.data[2];
            c.note_on_N[c.note_on.first] = c.note_on.second;
            if (auto ptr = c.callback_note_on)
            {
              std::vector<ossia::value> t{int32_t{c.note_on.first},
                                          int32_t{c.note_on.second}};
              ptr->value_callback(t);
            }
            if (auto ptr = c.callback_note_on_N[c.note_on.first])
            {
              int32_t val{c.note_on_N[c.note_on.first]};
              ptr->value_callback(val);
            }
            break;
          case mm::MessageType::NOTE_OFF:
            c.note_off.first = mess.data[1];
            c.note_off.second = mess.data[2];
            c.note_off_N[c.note_off.first] = c.note_off.second;
            if (auto ptr = c.callback_note_off)
            {
              std::vector<ossia::value> t{int32_t{c.note_off.first},
                                          int32_t{c.note_off.second}};
              ptr->value_callback(t);
            }
            if (auto ptr = c.callback_note_off_N[c.note_off.first])
            {
              int32_t val{c.note_off_N[c.note_off.first]};
              ptr->value_callback(val);
            }
            break;
          case mm::MessageType::CONTROL_CHANGE:
            c.cc.first = mess.data[1];
            c.cc.second = mess.data[2];
            c.cc_N[c.cc.first] = c.cc.second;
            if (auto ptr = c.callback_cc)
            {
              std::vector<ossia::value> t{int32_t{c.cc.first},
                                          int32_t{c.cc.second}};
              ptr->value_callback(t);
            }
            if (auto ptr = c.callback_cc_N[c.cc.first])
            {
              int32_t val{c.cc_N[c.cc.first]};
              ptr->value_callback(val);
            }
            break;
          case mm::MessageType::PROGRAM_CHANGE:
            c.pc = mess.data[1];
            if (auto ptr = c.callback_pc)
            {
              ptr->value_callback(int32_t{c.pc});
            }
            if (auto ptr = c.callback_pc_N[c.pc])
            {
              ptr->value_callback(ossia::impulse{});
            }
            break;
          default:
            break;
        }
      };
    }
    else if (m_info.type == midi_info::Type::RemoteInput)
    {
      m_output->openPort(m_info.port);
    }

    return true;
  }
  catch (...)
  {
    logger().error("midi_protocol::~setInfo() error");
    return false;
  }
}

midi_info midi_protocol::get_info() const
{
  return m_info;
}

bool midi_protocol::pull(parameter_base& address)
{
  midi_address& adrs = dynamic_cast<midi_address&>(address);
  if (m_info.type != midi_info::Type::RemoteOutput)
    return false;

  auto& adrinfo = adrs.info();
  const midi_channel& chan = m_channels[adrinfo.channel];
  switch (adrinfo.type)
  {
    case address_info::Type::NoteOn_N:
    {
      int32_t val{chan.note_on_N[adrinfo.note]};
      address.set_value(val);
      return true;
    }

    case address_info::Type::NoteOn:
    {
      std::vector<ossia::value> val{int32_t{chan.note_on.first},
                                    int32_t{chan.note_on.second}};
      address.set_value(val);
      return true;
    }

    case address_info::Type::NoteOff_N:
    {
      int32_t val{chan.note_off_N[adrinfo.note]};
      address.set_value(val);
      return true;
    }

    case address_info::Type::NoteOff:
    {
      std::vector<ossia::value> val{int32_t{chan.note_off.first},
                                    int32_t{chan.note_off.second}};
      address.set_value(val);
      return true;
    }

    case address_info::Type::CC_N:
    {
      int32_t val{chan.cc_N[adrinfo.note]};
      address.set_value(val);
      return true;
    }

    case address_info::Type::CC:
    {
      std::vector<ossia::value> val{int32_t{chan.cc.first},
                                    int32_t{chan.cc.second}};
      address.set_value(val);
      return true;
    }

    case address_info::Type::PC:
    {
      int32_t val{chan.pc};
      address.set_value(val);
      return true;
    }
    default:
      return false;
  }
}

bool midi_protocol::push(const parameter_base& address)
{
  try
  {
    const midi_address& adrs = dynamic_cast<const midi_address&>(address);
    if (m_info.type != midi_info::Type::RemoteInput)
      return false;

    auto& adrinfo = adrs.info();
    switch (adrinfo.type)
    {
      case address_info::Type::NoteOn_N:
      {
        m_output->send(mm::MakeNoteOn(
            adrinfo.channel, adrinfo.note, adrs.getValue().get<int32_t>()));
        return true;
      }

      case address_info::Type::NoteOn:
      {
        auto& val = adrs.getValue().get<std::vector<ossia::value>>();
        m_output->send(mm::MakeNoteOn(
            adrinfo.channel, val[0].get<int32_t>(), val[1].get<int32_t>()));
        return true;
      }

      case address_info::Type::NoteOff_N:
      {
        m_output->send(mm::MakeNoteOff(
            adrinfo.channel, adrinfo.note, adrs.getValue().get<int32_t>()));
        return true;
      }

      case address_info::Type::NoteOff:
      {
        auto& val = adrs.getValue().get<std::vector<ossia::value>>();
        m_output->send(mm::MakeNoteOff(
            adrinfo.channel, val[0].get<int32_t>(), val[1].get<int32_t>()));
        return true;
      }

      case address_info::Type::CC_N:
      {
        m_output->send(mm::MakeControlChange(
            adrinfo.channel, adrinfo.note, adrs.getValue().get<int32_t>()));
        return true;
      }

      case address_info::Type::CC:
      {
        auto& val = adrs.getValue().get<std::vector<ossia::value>>();
        m_output->send(mm::MakeControlChange(
            adrinfo.channel, val[0].get<int32_t>(), val[1].get<int32_t>()));
        return true;
      }

      case address_info::Type::PC:
      {
        m_output->send(mm::MakeProgramChange(
            adrinfo.channel, adrs.getValue().get<int32_t>()));
        return true;
      }

      case address_info::Type::PC_N:
      {
        m_output->send(mm::MakeProgramChange(adrinfo.channel, adrinfo.note));
        return true;
      }
      default:
        return false;
    }

    return true;
  }
  catch (const std::exception& e)
  {
    ossia::logger().error("Error when pushing midi message: {}", e.what());
    return false;
  }
  catch (...)
  {
    ossia::logger().error("Error when pushing midi message");
    return false; // TODO log error.
  }
}

bool midi_protocol::push_raw(const full_parameter_data& parameter_base)
{ return false; }

bool midi_protocol::observe(parameter_base& address, bool enable)
{
  enable = true;
  midi_address& adrs = dynamic_cast<midi_address&>(address);
  if (m_info.type != midi_info::Type::RemoteOutput)
    return false;

  auto adrs_ptr = &adrs;
  auto& adrinfo = adrs.info();
  midi_channel& chan = m_channels[adrinfo.channel];
  switch (adrinfo.type)
  {
    case address_info::Type::NoteOn_N:
    {
      chan.callback_note_on_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case address_info::Type::NoteOff_N:
    {
      chan.callback_note_off_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case address_info::Type::CC_N:
    {
      chan.callback_cc_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case address_info::Type::PC_N:
    {
      chan.callback_pc_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    default:
      // TODO do the non-N version
      return false;
  }

  return true;
}

bool midi_protocol::update(node_base& node)
{
  return false;
}

std::vector<midi_info> midi_protocol::scan()
{
  std::vector<midi_info> vec;

  {
    // Input devices are those on which we do output
    auto& in = *m_input;
    auto dev = in.getInputDevice();
    auto portcount = dev->getPortCount();
    for (auto i = 0u; i < portcount; i++)
    {
      vec.emplace_back(midi_info::Type::RemoteOutput, dev->getPortName(i), i);
    }
  }

  {
    // Output devices are those that will send data to us
    auto& out = *m_output;
    auto dev = out.getOutputDevice();
    auto portcount = dev->getPortCount();
    for (auto i = 0u; i < portcount; i++)
    {
      vec.emplace_back(midi_info::Type::RemoteInput, dev->getPortName(i), i);
    }
  }

  return vec;
}
}
}
}
