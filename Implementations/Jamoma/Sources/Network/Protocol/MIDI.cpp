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

vector<std::shared_ptr<MIDI>> JamomaMIDI::scan()
{
    return {};
}
