#include "BasicLocal.h"
#include "BasicAddress.h"
namespace impl
{
bool Local2::pullAddressValue(OSSIA::Address2& address) const
{
  auto& adrs = dynamic_cast<impl::BasicAddress&>(address);
  return false;
}

bool Local2::pushAddressValue(const OSSIA::Address2& address) const
{
  // Update the address and notify

  return false;
}

bool Local2::observeAddressValue(OSSIA::Address2& address, bool enable) const
{
  return false;
}

bool Local2::updateChildren(Node& node) const
{
  return false;
}
}
