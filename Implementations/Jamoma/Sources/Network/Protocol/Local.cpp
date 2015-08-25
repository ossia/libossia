#include "Network/Protocol/Local.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

Local::Local()
{}

# pragma mark -
# pragma mark Operation

bool Local::pullAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool Local::pushAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool Local::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}
