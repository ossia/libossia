#include "Misc/CallbackContainer.h"

#include "Network/Address.h"
#include "Editor/Expression.h"

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  // explicit instantiation for ValueCallback
  template class CallbackContainer<ValueCallback>;
  
  // explicit instantiation for ResultCallback
  template class CallbackContainer<ResultCallback>;
}
