#include "Network/Protocol/MIDI.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

MIDI::MIDI()
{}

# pragma mark -
# pragma mark Operation

bool MIDI::pullAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool MIDI::pushAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool MIDI::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}

# pragma mark -
# pragma mark Specific Operation

std::vector<MIDI> MIDI::scan()
{}