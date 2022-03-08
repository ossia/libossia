#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/scenario/time_process.hpp>

namespace ossia
{

class OSSIA_EXPORT node_process : public looping_process<node_process>
{
public:
  node_process(ossia::node_ptr n);
  ~node_process() override;
  void offset_impl(ossia::time_value) override;
  void transport_impl(ossia::time_value date) override;

  void state_impl(const ossia::token_request& req)
  {
    node->request(req);
  }

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;
  void mute_impl(bool) override;
};

}
