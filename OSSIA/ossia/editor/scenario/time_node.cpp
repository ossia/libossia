#include <ossia/editor/scenario/detail/TimeNode_impl.hpp>

#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  std::shared_ptr<TimeNode> TimeNode::create(TimeNode::ExecutionCallback callback)
  {
    return std::make_shared<impl::JamomaTimeNode>(callback);
  }

  TimeNode::~TimeNode() = default;
}
