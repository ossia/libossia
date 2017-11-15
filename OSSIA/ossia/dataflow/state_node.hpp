#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia {

class state_node : public ossia::graph_node
{
  public:
    state_node()
    {

    }

    void run(ossia::token_request, ossia::execution_state& e) override
    {
      struct {
          ossia::execution_state& e;
          void operator()(const ossia::state& st)
          {

          }
          void operator()(const ossia::message& msg)
          {
            e.insert(&msg.dest.address(), value_port{{}, { msg.message_value }});
          }
          void operator()(const ossia::custom_state& st)
          {
            e.launch();
          }
          template<std::size_t N>
          void operator()(const ossia::piecewise_vec_message<N>& st)
          {
            e.launch();
          }
          void operator()(const ossia::piecewise_message& st)
          {
          }

      } vis;
      for(auto& msg : data)
      {
        ossia::apply(vis, msg);
      }
    }

    ossia::state data;
};

}

