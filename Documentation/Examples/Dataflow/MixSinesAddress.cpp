#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/dataflow/audio_protocol.hpp>
#include <ossia/dataflow/common_nodes.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/dataflow/graph/tick_methods.hpp>

class OSSIA_EXPORT simple_device
{
  public:
    ossia::net::generic_device device;
};
int main(int argc, char** argv) {
  int nodes = 60;
  if(argc > 1)
    nodes = std::atoi(argv[1]);
  using namespace ossia;
  using namespace std::literals;
  oscquery_device osc; // a device to expose parameters over the network
  tc_graph g; // graph implementation with static scheduling
  execution_state e;
  audio_device audio; // a device that maps to the sound card inputs & outputs

  e.sampleRate = audio.protocol.rate;
  e.register_device(&audio.device);
  e.register_device(&osc.device);

  // multiplies the inputs by a float value
  auto gain = std::make_shared<gain_node>();
  g.add_node(gain);

  // the gain node can be controlled through the OSC address /volume,
  // and sends data to the sound card
  auto gain_param = ossia::create_parameter(osc.device, "/volume", "dB");
  gain_param->push_value(-60);
  gain->inputs()[1]->address = gain_param;
  gain->outputs()[0]->address = &audio.get_main_out();

  // 60 sine generators, each with their frequency controllable by
  // an OSC address of the form /freq.1 /freq.2 /freq.3 ...
  for(int i = 0; i < nodes; i++) {
    auto node = std::make_shared<sine_node>();
    g.connect(make_strict_edge(0, 0, node, gain));

    auto freq_param = ossia::create_parameter(osc.device, "/freq", "hz");
    freq_param->push_value(200 + 12 * i);
    node->inputs()[0]->address = freq_param;
  }

  // start callback-based soundcard-driven execution : here
  // the tick algorithm adds a token of the buffer size to every node
  audio.protocol.set_tick(tick_all_nodes{e, g});

  std::this_thread::sleep_for(10s);
}
