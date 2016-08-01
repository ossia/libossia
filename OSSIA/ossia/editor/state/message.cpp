#include <ossia/editor/state/message.hpp>
#include <ossia/network/base/address.hpp>

namespace ossia
{
void message::launch() const
{
  address.get().pushValue(value);
}
}
