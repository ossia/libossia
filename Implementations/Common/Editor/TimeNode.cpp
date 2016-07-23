#include "Editor/TimeNode_impl.h"

#include <assert.h>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<TimeNode> TimeNode::create(TimeNode::ExecutionCallback callback)
  {
    return make_shared<impl::JamomaTimeNode>(callback);
  }

  TimeNode::~TimeNode() = default;
}
