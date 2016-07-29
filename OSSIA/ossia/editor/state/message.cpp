#include <ossia/editor/state/message.hpp>
#include <ossia/network/base/address.hpp>

namespace ossia
{
  void Message::launch() const
  {
      address.get().pushValue(value);
  }
}
