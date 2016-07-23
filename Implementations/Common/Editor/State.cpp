#include "Editor/State_impl.h"

namespace OSSIA
{
  shared_ptr<State> State::create()
  {
    return make_shared<impl::JamomaState>();
  }

  State::~State()
  {}
}
