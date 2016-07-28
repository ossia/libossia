#include <ossia/editor/state/message.hpp>
#include <ossia/network/base/address.hpp>

namespace OSSIA
{
  void Message::launch() const
  {
      address.get().pushValue(value);
  }
}
