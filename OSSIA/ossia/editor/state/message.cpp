#include <ossia/editor/state/message.hpp>
#include <ossia/network/v1/Address.hpp>

namespace OSSIA
{
  void Message::launch() const
  {
      if(address)
          address->pushValue(value);
  }
}
