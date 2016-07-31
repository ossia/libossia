#include <ossia/editor/scenario/detail/TimeNode_impl.hpp>
namespace ossia
{
  std::shared_ptr<time_node> time_node::create(time_node::execution_callback callback)
  {
    return std::make_shared<detail::time_node_impl>(callback);
  }

  time_node::~time_node() = default;
}
