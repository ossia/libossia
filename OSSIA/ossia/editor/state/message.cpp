#include <ossia/editor/state/message.hpp>
#include <ossia/network/base/address.hpp>

namespace ossia
{
void message::launch() const
{
  if(destination.value)
    destination.value->pushValue(value);
}
}
