#include "Network/Protocol/JamomaMIDI.h"
using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

shared_ptr<MIDI> MIDI::create()
{
  return make_shared<JamomaMIDI>();
}

JamomaMIDI::JamomaMIDI():
  mInput{"ossia-in"},
  mOutput{"ossia-out"}
{}

JamomaMIDI::~JamomaMIDI()
{
  try {
    mInput.closePort();
    mOutput.closePort();
  }
  catch(...) {
    std::cerr << "dtor MIDI error\n";
  }
}

MIDI::~MIDI()
{}

bool JamomaMIDI::setInfo(MidiInfo m)
{
  try {
    // Close current ports
    if(mInfo.type == MidiInfo::Type::RemoteOutput)
    {
      mInput.closePort();
    }
    else if(mInfo.type == MidiInfo::Type::RemoteInput)
    {
      mOutput.closePort();
    }

    mInfo = m;

    if(mInfo.type == MidiInfo::Type::RemoteOutput)
    {
      mInput.openPort(mInfo.port);
      mInput.messageCallback = [=] (mm::MidiMessage mess) {
        Channel& c = mChannels[mess.getChannel()];
        switch(mess.getMessageType())
        {
          case mm::MessageType::NOTE_ON:
            c.mNoteOn.first = mess.data[1];
            c.mNoteOn.second = mess.data[2];
            c.mNoteOn_N[c.mNoteOn.first] = c.mNoteOn.second;
            if(auto ptr = c.mCallbackNoteOn)
            {
              OSSIA::Tuple t{OSSIA::Int{c.mNoteOn.first}, OSSIA::Int{c.mNoteOn.second}};
              ptr->valueCallback(t);
            }
            if(auto ptr = c.mCallbackNoteOn_N[c.mNoteOn.first])
            {
              OSSIA::Int val{c.mNoteOn_N[c.mNoteOn.first]};
              ptr->valueCallback(val);
            }
            break;
          case mm::MessageType::NOTE_OFF:
            c.mNoteOff.first = mess.data[1];
            c.mNoteOff.second = mess.data[2];
            c.mNoteOff_N[c.mNoteOff.first] = c.mNoteOff.second;
            if(auto ptr = c.mCallbackNoteOff)
            {
              OSSIA::Tuple t{OSSIA::Int{c.mNoteOff.first}, OSSIA::Int{c.mNoteOff.second}};
              ptr->valueCallback(t);
            }
            if(auto ptr = c.mCallbackNoteOff_N[c.mNoteOff.first])
            {
              OSSIA::Int val{c.mNoteOff_N[c.mNoteOff.first]};
              ptr->valueCallback(val);
            }
            break;
          case mm::MessageType::CONTROL_CHANGE:
            c.mCC.first = mess.data[1];
            c.mCC.second = mess.data[2];
            c.mCC_N[c.mCC.first] = c.mCC.second;
            if(auto ptr = c.mCallbackCC)
            {
              OSSIA::Tuple t{OSSIA::Int{c.mCC.first}, OSSIA::Int{c.mCC.second}};
              ptr->valueCallback(t);
            }
            if(auto ptr = c.mCallbackCC_N[c.mCC.first])
            {
              OSSIA::Int val{c.mCC_N[c.mCC.first]};
              ptr->valueCallback(val);
            }
            break;
          case mm::MessageType::PROGRAM_CHANGE:
            c.mPC = mess.data[1];
            if(auto ptr = c.mCallbackPC)
            {
              ptr->valueCallback(OSSIA::Int{c.mPC});
            }
            if(auto ptr = c.mCallbackPC_N[c.mPC])
            {
              ptr->valueCallback(OSSIA::Impulse{});
            }
            break;
          default:
            break;
        }
      };
    }
    else if(mInfo.type == MidiInfo::Type::RemoteInput)
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

MidiInfo JamomaMIDI::getInfo() const
{
  return mInfo;
}

# pragma mark -
# pragma mark Operation

bool JamomaMIDI::pullAddressValue(Address& address) const
{
  MIDIAddress& adrs = dynamic_cast<MIDIAddress&>(address);
  if(mInfo.type != MidiInfo::Type::RemoteOutput)
    return false;

  auto& adrinfo = adrs.info();
  const Channel& chan = mChannels[adrinfo.channel];
  switch(adrinfo.type)
  {
    case MIDIAddressInfo::Type::NoteOn_N:
    {
      OSSIA::Int val{chan.mNoteOn_N[adrinfo.note]};
      address.setValue(val);
      return true;
    }

    case MIDIAddressInfo::Type::NoteOn:
    {
      OSSIA::Tuple val{
        OSSIA::Int{chan.mNoteOn.first},
        OSSIA::Int{chan.mNoteOn.second}};
      address.setValue(val);
      return true;
    }

    case MIDIAddressInfo::Type::NoteOff_N:
    {
      OSSIA::Int val{chan.mNoteOff_N[adrinfo.note]};
      address.setValue(val);
      return true;
    }

    case MIDIAddressInfo::Type::NoteOff:
    {
      OSSIA::Tuple val{
        OSSIA::Int{chan.mNoteOff.first},
        OSSIA::Int{chan.mNoteOff.second}};
      address.setValue(val);
      return true;
    }

    case MIDIAddressInfo::Type::CC_N:
    {
      OSSIA::Int val{chan.mCC_N[adrinfo.note]};
      address.setValue(val);
      return true;
    }

    case MIDIAddressInfo::Type::CC:
    {
      OSSIA::Tuple val{
        OSSIA::Int{chan.mCC.first},
        OSSIA::Int{chan.mCC.second}};
      address.setValue(val);
      return true;
    }

    case MIDIAddressInfo::Type::PC:
    {
      OSSIA::Int val{chan.mPC};
      address.setValue(val);
      return true;
    }
    default:
      return false;
  }

}

bool JamomaMIDI::pushAddressValue(const Address& address) const
{
  const MIDIAddress& adrs = dynamic_cast<const MIDIAddress&>(address);
  if(mInfo.type != MidiInfo::Type::RemoteInput)
    return false;

  auto& adrinfo = adrs.info();
  switch(adrinfo.type)
  {
    case MIDIAddressInfo::Type::NoteOn_N:
    {
      mOutput.send(mm::MakeNoteOn(
                     adrinfo.channel,
                     adrinfo.note,
                     adrs.getValue().get<Int>().value));
      return true;
    }

    case MIDIAddressInfo::Type::NoteOn:
    {
      auto& val = adrs.getValue().get<Tuple>().value;
      mOutput.send(mm::MakeNoteOn(
                     adrinfo.channel,
                     val[0].get<Int>().value,
                     val[1].get<Int>().value));
      return true;
    }

    case MIDIAddressInfo::Type::NoteOff_N:
    {
      mOutput.send(mm::MakeNoteOff(
                     adrinfo.channel,
                     adrinfo.note,
                     adrs.getValue().get<Int>().value));
      return true;
    }

    case MIDIAddressInfo::Type::NoteOff:
    {
      auto& val = adrs.getValue().get<Tuple>().value;
      mOutput.send(mm::MakeNoteOff(
                     adrinfo.channel,
                     val[0].get<Int>().value,
                     val[1].get<Int>().value));
      return true;
    }

    case MIDIAddressInfo::Type::CC_N:
    {
      mOutput.send(mm::MakeControlChange(
                     adrinfo.channel,
                     adrinfo.note,
                     adrs.getValue().get<Int>().value));
      return true;
    }

    case MIDIAddressInfo::Type::CC:
    {
      auto& val = adrs.getValue().get<Tuple>().value;
      mOutput.send(mm::MakeControlChange(
                     adrinfo.channel,
                     val[0].get<Int>().value,
                     val[1].get<Int>().value));
      return true;
    }

    case MIDIAddressInfo::Type::PC:
    {
      mOutput.send(mm::MakeProgramChange(
                     adrinfo.channel,
                     adrs.getValue().get<Int>().value));
      return true;
    }

    case MIDIAddressInfo::Type::PC_N:
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

bool JamomaMIDI::observeAddressValue(std::shared_ptr<Address> address, bool enable) const
{
  enable = true;
  MIDIAddress& adrs = dynamic_cast<MIDIAddress&>(*address);
  if(mInfo.type != MidiInfo::Type::RemoteOutput)
    return false;

  auto adrs_ptr = dynamic_pointer_cast<MIDIAddress>(address);
  auto& adrinfo = adrs.info();
  Channel& chan = mChannels[adrinfo.channel];
  switch(adrinfo.type)
  {
    case MIDIAddressInfo::Type::NoteOn_N:
    {
      chan.mCallbackNoteOn_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case MIDIAddressInfo::Type::NoteOff_N:
    {
      chan.mCallbackNoteOff_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case MIDIAddressInfo::Type::CC_N:
    {
      chan.mCallbackCC_N[adrinfo.note] = enable ? adrs_ptr : nullptr;
      return true;
    }

    case MIDIAddressInfo::Type::PC_N:
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

bool JamomaMIDI::updateChildren(Node& node) const
{
  return false;
}

vector<MidiInfo> JamomaMIDI::scan()
{
  std::vector<MidiInfo> vec;

  {
    // Input devices are those on which we do output
    auto& in = mInput;
    auto dev = in.getInputDevice();
    auto portcount = dev->getPortCount();
    for(int i = 0; i < portcount; i++)
    {
      vec.emplace_back(MidiInfo::Type::RemoteOutput, dev->getPortName(i), i);
    }
  }

  {
    // Output devices are those that will send data to us
    auto& out = mOutput;
    auto dev = out.getOutputDevice();
    auto portcount = dev->getPortCount();
    for(int i = 0; i < portcount; i++)
    {
      vec.emplace_back(MidiInfo::Type::RemoteInput, dev->getPortName(i), i);
    }
  }

  return vec;
}

std::shared_ptr<OSSIA::Device> OSSIA::createMIDIDevice(std::shared_ptr<MIDI> mid)
{
  return std::make_shared<MIDIDeviceImpl>(mid);
}
