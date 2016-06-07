#include "Network/Protocol/JamomaMIDI.h"
#include <ModernMIDI/midi_input.h>
#include <ModernMIDI/midi_output.h>
using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

static mm::MidiInput& input()
{
    static mm::MidiInput i{"ossia-in"};
    return i;
}

static mm::MidiOutput& output()
{
    static mm::MidiOutput o{"ossia-out"};
    return o;
}

shared_ptr<MIDI> MIDI::create()
{
  return make_shared<JamomaMIDI>();
}

JamomaMIDI::JamomaMIDI()
{}

JamomaMIDI::~JamomaMIDI()
{}

MIDI::~MIDI()
{}

# pragma mark -
# pragma mark Operation

bool JamomaMIDI::pullAddressValue(Address& address) const
{
  JamomaAddress& adrs = dynamic_cast<JamomaAddress&>(address);

  TTValue value;

  if (adrs.pullValue(value))
  {
    adrs.setValue(value);
    return true;
  }

  return false;
}

bool JamomaMIDI::pushAddressValue(const Address& address) const
{
  const JamomaAddress& adrs = dynamic_cast<const JamomaAddress&>(address);

  TTValue value;

  adrs.getValue(value);

  return adrs.pushValue(value);
}

bool JamomaMIDI::observeAddressValue(std::shared_ptr<Address> address, bool enable) const
{
  shared_ptr<JamomaAddress> adrs = dynamic_pointer_cast<JamomaAddress>(address);

  adrs->observeValue(enable);

  return true;
}

bool JamomaMIDI::updateChildren(Node& node) const
{
  return false;
}

vector<MidiInfo> JamomaMIDI::scan()
{
     std::vector<MidiInfo>  vec;

     {
         auto& in = input();
         auto dev = in.getInputDevice();
         auto portcount = dev->getPortCount();
         for(int i = 0; i < portcount; i++)
         {
             vec.emplace_back(MidiInfo::Type::Input, dev->getPortName(i), i);
         }
     }

     {
         auto& out = output();
         auto dev = out.getOutputDevice();
         auto portcount = dev->getPortCount();
         for(int i = 0; i < portcount; i++)
         {
             vec.emplace_back(MidiInfo::Type::Output, dev->getPortName(i), i);
         }
     }

     return vec;
}

std::shared_ptr<OSSIA::Device> OSSIA::createMIDIDevice(std::shared_ptr<MIDI> mid)
{
    return std::make_shared<MIDIDeviceImpl>(mid);
}
