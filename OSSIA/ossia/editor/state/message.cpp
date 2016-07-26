#include <Editor/Message.h>
#include <Network/Address.h>

namespace OSSIA
{
  void Message::launch() const
  {
      if(address)
          address->pushValue(value);
  }
}
