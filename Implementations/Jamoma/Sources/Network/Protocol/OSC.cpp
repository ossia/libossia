#include "Network/Protocol/OSC.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

OSC::OSC(std::string ip, int in_port, int out_port) :
ip(ip),
in_port(in_port),
out_port(out_port)
{}

# pragma mark -
# pragma mark Operation

bool OSC::pullAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool OSC::pushAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool OSC::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}
