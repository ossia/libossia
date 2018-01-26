#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/dataflow/audio_protocol.hpp>
#include <ossia/dataflow/common_nodes.hpp>

int main(int argc, char** argv)
{
  int nodes = 60;
  if(argc > 1)
    nodes = std::atoi(argv[1]);
  using namespace ossia;
  using namespace std::literals;

  tc_graph g;
  execution_state e;
  audio_device audio;

  auto gain = std::make_shared<gain_node>();
  g.add_node(gain);
  gain->outputs()[0]->address = &audio.get_main_out();

  for(int i = 0; i < nodes; i++)
  {
    auto rand_node = std::make_shared<rand_float_node>(200, 600);
    auto sin_node = std::make_shared<sine_node>();
    g.connect(make_strict_edge(0, 0, rand_node, sin_node));
    g.connect(make_strict_edge(0, 0, sin_node, gain));
  }

  e.sampleRate = audio.protocol.rate;
  e.register_device(&audio.device);

  g.state(e);
  audio.protocol.set_tick(tick_all_nodes{e, g});

  std::this_thread::sleep_for(10s);
}
