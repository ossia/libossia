#include <ossia/audio/audio_device.hpp>
#include <ossia/audio/audio_protocol.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/nodes/rand_float.hpp>
#include <ossia/dataflow/nodes/sine.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/value/format_value.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>

#include <boost/lexical_cast.hpp>

#include <iostream>

std::string osc_address;
uint16_t listen_port{};
float value_min{0.}, value_max{1.};

struct tick
{
  ossia::execution_state& e;
  ossia::nodes::sine& node;
  ossia::net::parameter_base& osc_param;
  void operator()(const ossia::audio_tick_state& st)
  {
    using namespace ossia;
    ossia::token_request req{};
    req.date = ossia::time_value{int64_t(st.frames)};

    float val = ossia::convert<float>(osc_param.value());
    float freq = (val - value_min) / (value_max - value_min);
    freq = freq * 200 + 100;
    node.freq = freq;
    node.run(req, {&e});

    for(int i = 0; i < st.n_out; i++)
    {
      std::copy_n(node.audio_out->channel(0).data(), st.frames, st.outputs[i]);
    }
  }
};

int main(int argc, char** argv)
{
  using namespace std::literals;
  if(argc < 5)
  {
    fprintf(stderr, R"_(Usage:

  ./SynthOSC <OSC Address> <port> <min input> <max input>

Example:

  ./SynthOSC /arduino/my/sensor 4567 -1.0 1.0
)_");
    return 1;
  }

  osc_address = argv[1];

  listen_port = boost::lexical_cast<uint16_t>(argv[2]);
  value_min = boost::lexical_cast<float>(argv[3]);
  value_max = boost::lexical_cast<float>(argv[4]);

  /// Setup our datastructures ///
  ossia::audio_device audio{
      "minisine", // name, for APIs like JACK
      256,        // buffer size
      48000,      // sample rate
      0, 2        // inputs, outputs
  };

  /// Set-up the network device ///
  auto ctx = std::make_shared<ossia::net::network_context>();
  using conf = ossia::net::osc_protocol_configuration;
  ossia::net::generic_device device{
      ossia::net::make_osc_protocol(
          ctx, {conf::HOST, conf::OSC1_1, conf::SLIP, conf::NEVER_BUNDLE,
                ossia::net::udp_configuration{
                    {ossia::net::inbound_socket_configuration{
                         .bind = "0.0.0.0", .port = listen_port},
                     ossia::net::outbound_socket_configuration{
                         .host = "127.0.0.1", .port = 9977}}}}),
      "P"};

  auto b = [](const std::string s, const ossia::value& val) {
    std::cerr << "Received unhandled message: " << s << ":" << fmt::format("{}", val)
              << std::endl;
  };
  device.on_unhandled_message.connect(b);
  auto param = ossia::create_parameter(device, osc_address, "float");
  std::cerr << "Listening on: " << param->get_node().osc_address() << std::endl;
  param->push_value(value_min);
  param->set_domain(ossia::make_domain(value_min, value_max));

  ossia::execution_state e;
  ossia::nodes::sine node;
  node.freq = 200.;
  tick t{e, node, *param};
  t.e.bufferSize = audio.engine->effective_buffer_size;
  t.e.sampleRate = audio.engine->effective_sample_rate;

  audio.engine->set_tick(std::move(t));

  ctx->run();

  audio.engine->stop();
}
