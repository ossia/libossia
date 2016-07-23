#include "Editor/Message_impl.h"

namespace OSSIA
{
  shared_ptr<Message> Message::create(shared_ptr<Address> a, const Value& v)
  {
    return make_shared<impl::JamomaMessage>(a, v);
  }


  Message::~Message() = default;
}
