#include "Network/Protocol/Minuit.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

Minuit::Minuit(std::string ip, int in_port, int out_port) :
ip(ip),
in_port(in_port),
out_port(out_port)
{}

# pragma mark -
# pragma mark Operation

bool Minuit::pullAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool Minuit::pushAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool Minuit::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}
