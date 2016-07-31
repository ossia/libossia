#include <ossia/network/midi/midi.hpp>
#include <ossia/network/midi/detail/midi_impl.hpp>

namespace ossia
{
namespace net
{
namespace midi
{
midi_protocol::midi_protocol():
  mInput{"ossia-in"},
  mOutput{"ossia-out"}
{}

midi_protocol::~midi_protocol()
{
  try {
    mInput.closePort();
    mOutput.closePort();
  }
  catch(...) {
    std::cerr << "dtor MIDI error\n";
  }
}

bool midi_protocol::setInfo(midi_info m)
{
  try {
    // Close current ports
    if(mInfo.type == midi_info::Type::RemoteOutput)
    {
      mInput.closePort();
    }
    else if(mInfo.type == midi_info::Type::RemoteInput)
    {
      mOutput.closePort();
    }

    mInfo = m;

    if(mInfo.type == midi_info::Type::RemoteOutput)
    {
      mInput.openPort(mInfo.port);
      mInput.messageCallback = [=] (mm::MidiMessage mess) {
        midi_channel& c = mChannels[mess.getChannel()];
        switch(mess.getMessageType())
        {
          case mm::MessageType::NOTE_ON:
            c.mNoteOn.first = mess.data[1];
            c.mNoteOn.second = mess.data[2];
            c.mNoteOn_N[c.mNoteOn.first] = c.mNoteOn.second;
            if(auto ptr = c.mCallbackNoteOn)
            {
              ossia::Tuple t{ossia::Int{c.mNoteOn.first}, ossia::Int{c.mNoteOn.second}};
              ptr->valueCallback(t);
            }
            if(auto ptr = c.mCallbackNoteOn_N[c.mNoteOn.first])
            {
              ossia::Int val{c.mNoteOn_N[c.mNoteOn.first]};
              ptr->valueCallback(val);
            }
            break;
          case mm::MessageType::NOTE_OFF:
            c.mNoteOff.first = mess.data[1];
            c.mNoteOff.second = mess.data[2];
            c.mNoteOff_N[c.mNoteOff.first] = c.mNoteOff.second;
            if(auto ptr = c.mCallbackNoteOff)
            {
              ossia::Tuple t{ossia::Int{c.mNoteOff.first}, ossia::Int{c.mNoteOff.second}};
              ptr->valueCallback(t);
            }
            if(auto ptr = c.mCallbackNoteOff_N[c.mNoteOff.first])
            {
              ossia::Int val{c.mNoteOff_N[c.mNoteOff.first]};
              ptr->valueCallback(val);
            }
            break;
          case mm::MessageType::CONTROL_CHANGE:
            c.mCC.first = mess.data[1];
            c.mCC.second = mess.data[2];
            c.mCC_N[c.mCC.first] = c.mCC.second;
            if(auto ptr = c.mCallbackCC)
            {
              ossia::Tuple t{ossia::Int{c.mCC.first}, ossia::Int{c.mCC.second}};
              ptr->valueCallback(t);
            }
            if(auto ptr = c.mCallbackCC_N[c.mCC.first])
            {
              ossia::Int val{c.mCC_N[c.mCC.first]};
              ptr->valueCallback(val);
            }
            break;
          case mm::MessageType::PROGRAM_CHANGE:
            c.mPC = mess.data[1];
            if(auto ptr = c.mCallbackPC)
            {
              ptr->valueCallback(ossia::Int{c.mPC});
            }
            if(auto ptr = c.mCallbackPC_N[c.mPC])
            {
              ptr->valueCallback(ossia::Impulse{});
            }
            break;
          default:
            break;
        }
      };
    }
    else if(mInfo.type == midi_info::Type::RemoteInput)
    {
      mOutput.openPort(mInfo.port);
    }

    return true;
  }
  catch(...) {
    std::cerr << "setInfo MIDI error\n";
    return false;
  }
}

midi_info midi_protocol::getInfo() const
{
  return mInfo;
}

# pragma mark -
# pragma mark Operation

bool midi_protocol::pull(address_base& address)
{
  midi_address& adrs = dynamic_cast<midi_address&>(address);
  if(mInfo.type != midi_info::Type::RemoteOutput)
    return false;

  auto& adrinfo = adrs.info();
  const midi_channel& chan = mChannels[adrinfo.channel];
  switch(adrinfo.type)
  {
    case address_info::Type::NoteOn_N:
    {
      ossia::Int val{chan.mNoteOn_N[adrinfo.note]};
      address.setValue(val);
      return true;
    }

    case address_info::Type::NoteOn:
    {
      ossia::Tuple val{
        ossia::Int{chan.mNoteOn.first},
        ossia::Int{chan.mNoteOn.second}};
      address.setValue(val);
      return true;
    }

    case address_info::Type::NoteOff_N:
    {
      ossia::Int val{chan.mNoteOff_N[adrinfo.note]};
      address.setValue(val);
      return true;
    }

    case address_info::Type::NoteOff:
    {
      ossia::Tuple val{
        ossia::Int{chan.mNoteOff.first},
        ossia::Int{chan.mNoteOff.second}};
      address.setValue(val);
      return true;
    }

    case address_info::Type::CC_N:
    {
      ossia::Int val{chan.mCC_N[adrinfo.note]};
      address.setValue(val);
      return true;
    }

    case address_info::Type::CC:
    {
      ossia::Tuple val{
        ossia::Int{chan.mCC.first},
        ossia::Int{chan.mCC.second}};
      address.setValue(val);
      return true;
    }

    case address_info::Type::PC:
    {
      ossia::Int val{chan.mPC};
      address.setValue(val);
      return true;
    }
    default:
      return false;
  }

}

bool midi_protocol::push(const address_base& address)
{
  const midi_address& adrs = dynamic_cast<const midi_address&>(address);
  if(mInfo.type != midi_info::Type::RemoteInput)
    return false;

  auto& adrinfo = adrs.info();
  switch(adrinfo.type)
  {
    case address_info::Type::NoteOn_N:
    {
      mOutput.send(mm::MakeNoteOn(
                     adrinfo.channel,
                     adrinfo.note,
                     adrs.getValue().get<Int>().value));
      return true;
    }

    case address_info::Type::NoteOn:
    {
      auto& val = adrs.getValue().get<Tuple>().value;
      mOutput.send(mm::MakeNoteOn(
                     adrinfo.channel,
                     val[0].get<Int>().value,
                     val[1].get<Int>().value));
      return true;
    }

    case address_info::Type::NoteOff_N:
    {
      mOutput.send(mm::MakeNoteOff(
                     adrinfo.channel,
                     adrinfo.note,
                     adrs.getValue().get<Int>().value));
      return true;
    }

    case address_info::Type::NoteOff:
    {
      auto& val = adrs.getValue().get<Tuple>().value;
      mOutput.send(mm::MakeNoteOff(
                     adrinfo.channel,
                     val[0].get<Int>().value,
                     val[1].get<Int>().value));
      return true;
    }

    case address_info::Type::CC_N:
    {
      mOutput.send(mm::MakeControlChange(
                     adrinfo.channel,
                     adrinfo.note,
                     adrs.getValue().get<Int>().value));
      return true;
    }

    case address_info::Type::CC:
    {
      auto& val = adrs.getValue().get<Tuple>().value;
      mOutput.send(mm::MakeControlChange(
                     adrinfo.channel,
                     val[0].get<Int>().value,
                     val[1].get<Int>().value));
      return true;
    }

    case address_info::Type::PC:
    {
      mOutput.send(mm::MakeProgramChange(
                     adrinfo.channel,
                     adrs.getValue().get<Int>().value));
      return true;
    }

    case address_info::Type::PC_N:
    {
      mOutput.send(mm::MakeProgramChange(
                     adrinfo.channel,
                     adrinfo.note));
      return true;
    }
    default:
      return false;
  }

  return true;
}

bool midi_protocol::observe(address_base& address, bool enable)
{
  enable = true;
  midi_address& adrs = dynamic_cast<midi_address&>(address);
  if(mInfo.type != midi_info::Type::RemoteOutput)
    return false;

  auto adrs_ptr = &adrs;
  auto& adrinfo = adrs.info();
  midi_channel& chan = mChannels[adrinfo.channel];
  switch(adrinfo.type)
  {
    case address_info::Type::NoteOn_N:
    {
      chan.mCallbackNoteOn_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case address_info::Type::NoteOff_N:
    {
      chan.mCallbackNoteOff_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case address_info::Type::CC_N:
    {
      chan.mCallbackCC_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case address_info::Type::PC_N:
    {
      chan.mCallbackPC_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
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
    auto& in = mInput;
    auto dev = in.getInputDevice();
    auto portcount = dev->getPortCount();
    for(int i = 0; i < portcount; i++)
    {
      vec.emplace_back(midi_info::Type::RemoteOutput, dev->getPortName(i), i);
    }
  }

  {
    // Output devices are those that will send data to us
    auto& out = mOutput;
    auto dev = out.getOutputDevice();
    auto portcount = dev->getPortCount();
    for(int i = 0; i < portcount; i++)
    {
      vec.emplace_back(midi_info::Type::RemoteInput, dev->getPortName(i), i);
    }
  }

  return vec;
}

}
}
}
