// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/logger.hpp>
#include <ossia/network/midi/detail/midi_impl.hpp>
#include <ossia/network/midi/midi.hpp>

#include <rtmidi17/message.hpp>
#if !defined(__EMSCRIPTEN__)
#include <rtmidi17/rtmidi17.hpp>
#else
namespace rtmidi
{
class midi_in
{
};
class midi_out
{
};
}
#endif
namespace ossia
{
namespace net
{
namespace midi
{
midi_protocol::midi_protocol()
#if !defined(__EMSCRIPTEN__)
    : m_input{std::make_unique<rtmidi::midi_in>(
          rtmidi::API::UNSPECIFIED, "ossia-in")}
    , m_output
{
  std::make_unique<rtmidi::midi_out>(rtmidi::API::UNSPECIFIED, "ossia-out")
}
#endif
{
}

midi_protocol::midi_protocol(midi_info m) : midi_protocol()
{
  set_info(m);
}

midi_protocol::~midi_protocol()
{
#if !defined(__EMSCRIPTEN__)
  try
  {
    m_input->close_port();
    m_output->close_port();
  }
  catch (...)
  {
    logger().error("midi_protocol::~midi_protocol() error");
  }
#endif
}

bool midi_protocol::set_info(midi_info m)
{
#if !defined(__EMSCRIPTEN__)
  try
  {
    // Close current ports
    if (m_info.type == midi_info::Type::RemoteOutput)
    {
      m_input->close_port();
    }
    else if (m_info.type == midi_info::Type::RemoteInput)
    {
      m_output->close_port();
    }

    m_info = m;

    if (m_info.type == midi_info::Type::RemoteOutput)
    {
      if (m_dev)
        m_input->open_port(m_info.port, m_dev->get_name());
      else
        m_input->open_port(m_info.port, "libossia MIDI input");

      m_input->set_callback([&] (const auto& m) { midi_callback(m); });
    }
    else if (m_info.type == midi_info::Type::RemoteInput)
    {
      if (m_dev)
        m_output->open_port(m_info.port, m_dev->get_name());
      else
        m_output->open_port(m_info.port, "libossia MIDI out");
    }

    return true;
  }
  catch (...)
  {
    logger().error("midi_protocol::~setInfo() error");
    return false;
  }
#else
  return false;
#endif
}

midi_info midi_protocol::get_info() const
{
  return m_info;
}

bool midi_protocol::pull(parameter_base& address)
{
#if !defined(__EMSCRIPTEN__)
  midi_parameter& adrs = dynamic_cast<midi_parameter&>(address);
  if (m_info.type != midi_info::Type::RemoteOutput)
    return false;

  const address_info& adrinfo = adrs.info();
  switch (adrinfo.type)
  {
    case address_info::Type::NoteOn_N:
    {
      const midi_channel& chan = m_channels[adrinfo.channel - 1];
      int32_t val{chan.note_on_N[adrinfo.note]};
      address.set_value(val);
      return true;
    }

    case address_info::Type::NoteOn:
    {
      const midi_channel& chan = m_channels[adrinfo.channel - 1];
      std::vector<ossia::value> val{int32_t{chan.note_on.first},
                                    int32_t{chan.note_on.second}};
      address.set_value(val);
      return true;
    }

    case address_info::Type::NoteOff_N:
    {
      const midi_channel& chan = m_channels[adrinfo.channel - 1];
      int32_t val{chan.note_off_N[adrinfo.note]};
      address.set_value(val);
      return true;
    }

    case address_info::Type::NoteOff:
    {
      const midi_channel& chan = m_channels[adrinfo.channel - 1];
      std::vector<ossia::value> val{int32_t{chan.note_off.first},
                                    int32_t{chan.note_off.second}};
      address.set_value(val);
      return true;
    }

    case address_info::Type::CC_N:
    {
      const midi_channel& chan = m_channels[adrinfo.channel - 1];
      int32_t val{chan.cc_N[adrinfo.note]};
      address.set_value(val);
      return true;
    }

    case address_info::Type::CC:
    {
      const midi_channel& chan = m_channels[adrinfo.channel - 1];
      std::vector<ossia::value> val{int32_t{chan.cc.first},
                                    int32_t{chan.cc.second}};
      address.set_value(val);
      return true;
    }

    case address_info::Type::PC:
    {
      const midi_channel& chan = m_channels[adrinfo.channel - 1];
      int32_t val{chan.pc};
      address.set_value(val);
      return true;
    }

    case address_info::Type::PB:
    {
      const midi_channel& chan = m_channels[adrinfo.channel - 1];
      int32_t val{chan.pb};
      address.set_value(val);
      return true;
    }
    default:
      return false;
  }
#else
  return false;
#endif
}

bool midi_protocol::push(const parameter_base& address, const ossia::value& v)
{
#if !defined(__EMSCRIPTEN__)
  try
  {
    const midi_parameter& adrs = dynamic_cast<const midi_parameter&>(address);
    if (m_info.type != midi_info::Type::RemoteInput)
      return false;

    auto& adrinfo = adrs.info();
    switch (adrinfo.type)
    {
      case address_info::Type::NoteOn_N:
      {
        m_output->send_message(rtmidi::message::note_on(
            adrinfo.channel, adrinfo.note, v.get<int32_t>()));
        return true;
      }

      case address_info::Type::NoteOn:
      {
        auto& val = v.get<std::vector<ossia::value>>();
        m_output->send_message(rtmidi::message::note_on(
            adrinfo.channel, val[0].get<int32_t>(), val[1].get<int32_t>()));
        return true;
      }

      case address_info::Type::NoteOff_N:
      {
        m_output->send_message(rtmidi::message::note_off(
            adrinfo.channel, adrinfo.note, v.get<int32_t>()));
        return true;
      }

      case address_info::Type::NoteOff:
      {
        auto& val = v.get<std::vector<ossia::value>>();
        m_output->send_message(rtmidi::message::note_off(
            adrinfo.channel, val[0].get<int32_t>(), val[1].get<int32_t>()));
        return true;
      }

      case address_info::Type::CC_N:
      {
        m_output->send_message(rtmidi::message::control_change(
            adrinfo.channel, adrinfo.note, v.get<int32_t>()));
        return true;
      }

      case address_info::Type::CC:
      {
        auto& val = v.get<std::vector<ossia::value>>();
        m_output->send_message(rtmidi::message::control_change(
            adrinfo.channel, val[0].get<int32_t>(), val[1].get<int32_t>()));
        return true;
      }

      case address_info::Type::PC:
      {
        m_output->send_message(rtmidi::message::program_change(
            adrinfo.channel, v.get<int32_t>()));
        return true;
      }

      case address_info::Type::PC_N:
      {
        m_output->send_message(
            rtmidi::message::program_change(adrinfo.channel, adrinfo.note));
        return true;
      }

      case address_info::Type::PB:
      {
        m_output->send_message(rtmidi::message::pitch_bend(
            adrinfo.channel, v.get<int32_t>()));
        return true;
      }

      case address_info::Type::Any:
      {
        if (auto vec = v.target<std::vector<ossia::value>>())
        {
          rtmidi::message m;
          m.bytes.reserve(vec->size());
          for (const auto& val : *vec)
          {
            m.bytes.push_back(ossia::convert<int32_t>(val));
          }
          m_output->send_message(std::move(m));
        }
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
#else
  return false;
#endif
}

bool midi_protocol::push_raw(const full_parameter_data& parameter_base)
{
  return false;
}

bool midi_protocol::observe(parameter_base& address, bool enable)
{
  enable = true;
  midi_parameter& adrs = dynamic_cast<midi_parameter&>(address);
  if (m_info.type != midi_info::Type::RemoteOutput)
    return false;

  auto adrs_ptr = &adrs;
  auto& adrinfo = adrs.info();
  switch (adrinfo.type)
  {
    case address_info::Type::NoteOn_N:
    {
      midi_channel& chan = m_channels[adrinfo.channel - 1];
      chan.callback_note_on_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case address_info::Type::NoteOff_N:
    {
      midi_channel& chan = m_channels[adrinfo.channel - 1];
      chan.callback_note_off_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case address_info::Type::CC_N:
    {
      midi_channel& chan = m_channels[adrinfo.channel - 1];
      chan.callback_cc_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case address_info::Type::PC_N:
    {
      midi_channel& chan = m_channels[adrinfo.channel - 1];
      chan.callback_pc_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }
    case address_info::Type::PB:
    {
      midi_channel& chan = m_channels[adrinfo.channel - 1];
      chan.callback_pb = enable ? adrs_ptr : nullptr;
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

void midi_protocol::set_device(device_base& dev)
{
  m_dev = static_cast<midi_device*>(&dev);
}

void midi_protocol::value_callback(parameter_base& param, const value& val)
{
  param.set_value(val);
  m_dev->on_message(param);
}

void midi_protocol::midi_callback(const rtmidi::message& mess)
{
  if(m_learning)
  {
    on_learn(mess);
    return;
  }

  const auto chan = mess.get_channel();
  if (chan == 0)
    return;

  if (m_registers)
    messages.enqueue(mess);

  midi_channel& c = m_channels[chan - 1];
  switch (mess.get_message_type())
  {
    case rtmidi::message_type::NOTE_ON:
      c.note_on.first = mess.bytes[1];
      c.note_on.second = mess.bytes[2];
      c.note_on_N[c.note_on.first] = c.note_on.second;
      if (auto ptr = c.callback_note_on)
      {
        std::vector<ossia::value> t{int32_t{c.note_on.first},
                                    int32_t{c.note_on.second}};
        value_callback(*ptr, t);
      }
      if (auto ptr = c.callback_note_on_N[c.note_on.first])
      {
        int32_t val{c.note_on_N[c.note_on.first]};
        value_callback(*ptr, val);
      }
      break;
    case rtmidi::message_type::NOTE_OFF:
      c.note_off.first = mess.bytes[1];
      c.note_off.second = mess.bytes[2];
      c.note_off_N[c.note_off.first] = c.note_off.second;
      if (auto ptr = c.callback_note_off)
      {
        std::vector<ossia::value> t{int32_t{c.note_off.first},
                                    int32_t{c.note_off.second}};
        value_callback(*ptr, t);
      }
      if (auto ptr = c.callback_note_off_N[c.note_off.first])
      {
        int32_t val{c.note_off_N[c.note_off.first]};
        value_callback(*ptr, val);
      }
      break;
    case rtmidi::message_type::CONTROL_CHANGE:
      c.cc.first = mess.bytes[1];
      c.cc.second = mess.bytes[2];
      c.cc_N[c.cc.first] = c.cc.second;
      if (auto ptr = c.callback_cc)
      {
        std::vector<ossia::value> t{int32_t{c.cc.first},
                                    int32_t{c.cc.second}};
        value_callback(*ptr, t);
      }
      if (auto ptr = c.callback_cc_N[c.cc.first])
      {
        int32_t val{c.cc_N[c.cc.first]};
        value_callback(*ptr, val);
      }
      break;
    case rtmidi::message_type::PROGRAM_CHANGE:
      c.pc = mess.bytes[1];
      if (auto ptr = c.callback_pc)
      {
        value_callback(*ptr, int32_t{c.pc});
      }
      if (auto ptr = c.callback_pc_N[c.pc])
      {
        value_callback(*ptr, ossia::impulse{});
      }
      break;
    case rtmidi::message_type::PITCH_BEND:
      c.pb = mess.bytes[2] * 128 + mess.bytes[1];
      if (auto ptr = c.callback_pb)
      {
        value_callback(*ptr, int32_t{c.pb});
      }
      break;
    default:
      break;
  }

}

class generic_node final : public midi_node, public midi_parameter
{
public:
  generic_node(address_info addr, midi_device& dev, ossia::net::node_base& p)
    : midi_node{dev, p}
    , midi_parameter{addr, *this}
  {
    using namespace std::literals;
    switch(addr.type)
    {
      case address_info::Type::NoteOn:
        m_name = "on"s;
        break;
      case address_info::Type::NoteOn_N:
        m_name = midi_node_name(addr.note);
        break;
      case address_info::Type::NoteOff:
        m_name = "off"s;
        break;
      case address_info::Type::NoteOff_N:
        m_name = midi_node_name(addr.note);
        break;
      case address_info::Type::CC:
        m_name = "control"s;
        break;
      case address_info::Type::CC_N:
        m_name = midi_node_name(addr.note);
        break;
      case address_info::Type::PC:
        m_name = "program"s;
        break;
      case address_info::Type::PC_N:
        m_name = midi_node_name(addr.note);
        break;
      case address_info::Type::PB:
        m_name = "pitchbend"s;
        break;
      case address_info::Type::Any:
        m_name = "TODO"s;
        break;
      default:
        break;
    }

    m_parameter.reset(this);
  }
};

void midi_protocol::on_learn(const rtmidi::message& mess)
{
  const midi_size_t chan = mess.get_channel();
  if (chan == 0)
    return;

  const auto& chan_name = midi_node_name(chan);
  auto channel_node = m_dev->find_child(chan_name);
  if(!channel_node)
  {
    channel_node = m_dev->add_child(std::make_unique<ossia::net::midi::channel_node>(
                       false, chan, *m_dev, *m_dev));
  }

  switch (mess.get_message_type())
  {
    case rtmidi::message_type::NOTE_ON:
    {
      auto n_on_node = channel_node->find_child("on");
      if(!n_on_node)
      {
        n_on_node = channel_node->add_child(std::make_unique<generic_node>(
                                  address_info{chan, address_info::Type::NoteOn, 0},
                                  *m_dev, *channel_node));
      }

      auto n = mess.bytes[1];
      const auto& n_name = midi_node_name(n);
      auto node = n_on_node->find_child(n_name);
      if(!node)
      {
        n_on_node->add_child(std::make_unique<generic_node>(
                               address_info{chan, address_info::Type::NoteOn_N, 0},
                               *m_dev, *n_on_node));
      }
      break;
    }

    case rtmidi::message_type::NOTE_OFF:
    {
      auto n = mess.bytes[1];
      break;
    }

    case rtmidi::message_type::CONTROL_CHANGE:
    {
      auto n = mess.bytes[1];
      break;
    }

    case rtmidi::message_type::PROGRAM_CHANGE:
    {
      auto n = mess.bytes[1];
      break;
    }

    case rtmidi::message_type::PITCH_BEND:
    {
      break;
    }
    default:
      break;

  }
}

std::vector<midi_info> midi_protocol::scan()
{
  std::vector<midi_info> vec;
#if !defined(__EMSCRIPTEN__)

  {
    // Input devices are those on which we do output
    rtmidi::midi_in& in = *m_input;
    auto portcount = in.get_port_count();
    for (auto i = 0u; i < portcount; i++)
    {
      vec.emplace_back(midi_info::Type::RemoteOutput, in.get_port_name(i), i);
    }
  }

  {
    // Output devices are those that will send data to us
    rtmidi::midi_out& out = *m_output;
    auto portcount = out.get_port_count();
    for (auto i = 0u; i < portcount; i++)
    {
      vec.emplace_back(midi_info::Type::RemoteInput, out.get_port_name(i), i);
    }
  }
#endif
  return vec;
}

void midi_protocol::push_value(const rtmidi::message& m)
{
#if !defined(__EMSCRIPTEN__)
  m_output->send_message(m);
#endif
}

void midi_protocol::enable_registration()
{
  m_registers = true;
}

bool midi_protocol::learning() const
{
  return m_learning;
}

void midi_protocol::set_learning(bool newLearn)
{
  m_learning = newLearn;
}
}
}
}
