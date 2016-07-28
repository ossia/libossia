#include <ossia/editor/state/message.hpp>
#include <ossia/network/base/Address.hpp>

namespace OSSIA
{
  void Message::launch() const
  {
      address.get().pushValue(value);
  }
}
