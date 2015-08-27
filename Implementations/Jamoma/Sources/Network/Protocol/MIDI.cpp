#include "Network/Protocol/JamomaMIDI.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

shared_ptr<MIDI> MIDI::create()
{
  return make_shared<JamomaMIDI>();
}

JamomaMIDI::JamomaMIDI()
{}

JamomaMIDI::~JamomaMIDI()
{}

# pragma mark -
# pragma mark Operation

bool JamomaMIDI::pullAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool JamomaMIDI::pushAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool JamomaMIDI::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}

vector<std::shared_ptr<MIDI>> JamomaMIDI::scan()
{
    return {};
}
