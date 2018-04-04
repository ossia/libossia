#pragma once
#if defined(_MSC_VER) || __has_include(<jack/jack.h>)
#include <ossia/audio/audio_protocol.hpp>
#define __x86_64__ 1
#define USE_WEAK_JACK 1
#define NO_JACK_METADATA 1
#include <weak_libjack.h>

namespace ossia
{
class jack_engine final
    : public audio_engine
{
  public:
    jack_engine(std::string name, int& inputs, int& outputs, int& rate, int& bs);
    ~jack_engine() override;

    void reload(audio_protocol* cur) override;
    void stop() override;

  private:
    static int process(jack_nframes_t nframes, void* arg);

    std::atomic<jack_client_t*> client{};
    std::vector<jack_port_t*> input_ports;
    std::vector<jack_port_t*> output_ports;
};

}

namespace ossia
{
jack_engine::jack_engine(std::string name, int& inputs, int& outputs, int& rate, int& bs)
{
  std::cerr << "JACK: " << have_libjack() << std::endl;
  client = jack_client_open(
             (!name.empty() ? name.c_str() : "score")
             , JackNullOption
             , nullptr);
  if (!client)
  {
    std::cerr << "JACK server not running?" << std::endl;
    throw std::runtime_error("Audio error: no JACK server");
  }
  jack_set_process_callback(client, process, this);
  jack_set_sample_rate_callback (client, JackSampleRateCallback{}, this);
  jack_on_shutdown (client, JackShutdownCallback{}, this);
  jack_set_error_function([] (const char* str) {
    std::cerr << "JACK ERROR: " << str << std::endl;
  });
  jack_set_info_function([] (const char* str) {
    std::cerr << "JACK INFO: " << str << std::endl;
  });

  for(int i = 0; i < inputs; i++)
  {
    auto in = jack_port_register (client, ("in_" + std::to_string(i + 1)).c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    assert(in);
    input_ports.push_back(in);
  }
  for(int i = 0; i < outputs; i++)
  {
    auto out = jack_port_register (client, ("out_" + std::to_string(i + 1)).c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    assert(out);
    output_ports.push_back(out);
  }

  std::cerr << "=== stream start ===\n";

  rate = jack_get_sample_rate(client);
  bs = jack_get_buffer_size(client);

  int err = jack_activate(client);
  if (err != 0)
  {
    std::cerr << "JACK error: " << err << std::endl;
    throw std::runtime_error("Audio error: JACK cannot activate");
  }

  {
    auto ports = jack_get_ports(client, nullptr, JACK_DEFAULT_AUDIO_TYPE, JackPortIsPhysical | JackPortIsOutput);
    if(ports)
    {
      for (std::size_t i = 0; i < input_ports.size() && ports[i]; i++)
        jack_connect(client, ports[i], jack_port_name(input_ports[i]));

      jack_free(ports);
    }
  }
  {
    auto ports = jack_get_ports(client, nullptr, JACK_DEFAULT_AUDIO_TYPE, JackPortIsPhysical | JackPortIsInput);
    if(ports)
    {
      for (std::size_t i = 0; i < output_ports.size() && ports[i]; i++)
        jack_connect(client, jack_port_name(output_ports[i]), ports[i]);

      jack_free(ports);
    }
  }
}

jack_engine::~jack_engine()
{
  stop();
  if(protocol)
    protocol.load()->engine = nullptr;
  jack_deactivate(client.load());
  jack_client_close(client);
}

void jack_engine::reload(ossia::audio_protocol* p)
{
  stop();
  this->protocol = p;

  if(!p)
    return;
  auto& proto = *p;
  proto.engine = this;
  std::cerr << "=== STARTING PROCESS ==" << std::endl;

  proto.setup_tree((int)input_ports.size(), (int)output_ports.size());

  stop_processing = false;
}

void jack_engine::stop()
{
  std::cerr << "=== STOPPED PROCESS ==" << std::endl;
  if(this->protocol)
    this->protocol.load()->engine = nullptr;
  stop_processing = true;
  protocol = nullptr;
  while(processing) std::this_thread::sleep_for(std::chrono::milliseconds(100)) ;
}

int jack_engine::process(jack_nframes_t nframes, void* arg)
{
  auto& self = *static_cast<jack_engine*>(arg);
  const auto inputs = self.input_ports.size();
  const auto outputs = self.output_ports.size();
  if(self.stop_processing)
  {
    for(std::size_t i = 0; i < outputs; i++)
    {
      auto chan = (jack_default_audio_sample_t *) jack_port_get_buffer(self.output_ports[i], nframes);
      for(std::size_t j = 0; j < nframes; j++)
        chan[j] = 0.;
    }

    return 0;
  }

  auto proto = self.protocol.load();
  if(proto)
  {
    self.processing = true;
    auto float_input = (float**)alloca(sizeof(float*) * inputs);
    auto float_output = (float**) alloca(sizeof(float*) * outputs);
    for(std::size_t i = 0; i < inputs; i++)
    {
      float_input[i] = (jack_default_audio_sample_t *) jack_port_get_buffer(self.input_ports[i], nframes);
    }
    for(std::size_t i = 0; i < outputs; i++)
    {
      float_output[i] = (jack_default_audio_sample_t *) jack_port_get_buffer(self.output_ports[i], nframes);
    }

    proto->process_generic(*proto, float_input, float_output, (int)inputs, (int)outputs, nframes);
    proto->audio_tick(nframes, 0);
    self.processing = false;
  }
  return 0;
}
}

#include <../weakjack/weak_libjack.c>
#endif
