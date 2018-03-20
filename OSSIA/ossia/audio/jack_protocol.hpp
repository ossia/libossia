#pragma once
#if __has_include(<jack/jack.h>)
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
    jack_engine();
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
jack_engine::jack_engine()
{
  std::cerr << "JACK: " << have_libjack() << std::endl;
}

jack_engine::~jack_engine()
{
  stop();
  if(protocol)
    protocol.load()->engine = nullptr;
}

void jack_engine::reload(ossia::audio_protocol* p)
{
  if(this->protocol)
    this->protocol.load()->engine = nullptr;
  this->protocol = p;
  stop();

  if(!p)
    return;
  auto& proto = *p;
  proto.engine = this;
  auto& dev = proto.get_device();
  client = jack_client_open(dev.get_name().c_str(), JackNullOption, nullptr);
  if (!client)
  {
    std::cerr << "JACK server not running?" << std::endl;
    exit(0);
    return;
  }
  std::cerr << "=== STARTING PROCESS ==" << std::endl;

  jack_set_process_callback(client, process, this);

  jack_set_sample_rate_callback (client, JackSampleRateCallback{}, this);

  jack_on_shutdown (client, JackShutdownCallback{}, this);


  constexpr const int count = 32;
  proto.inputs = 32;
  proto.outputs = 32;
  proto.audio_ins.clear();
  proto.audio_outs.clear();

  for(int i = 0; i < count; i++)
  {
    auto in = jack_port_register (client, ("in-" + std::to_string(i)).c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    auto out = jack_port_register (client, ("out-" + std::to_string(i)).c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    assert(in);
    input_ports.push_back(in);
    assert(out);
    output_ports.push_back(out);
    proto.audio_ins.push_back(ossia::net::find_or_create_parameter<ossia::audio_parameter>(dev.get_root_node(), "/in/" + std::to_string(i)));
    proto.audio_outs.push_back(ossia::net::find_or_create_parameter<ossia::audio_parameter>(dev.get_root_node(), "/out/" + std::to_string(i)));
  }

  proto.main_audio_in = ossia::net::find_or_create_parameter<ossia::audio_parameter>(dev.get_root_node(), "/in/main");
  proto.main_audio_out = ossia::net::find_or_create_parameter<ossia::audio_parameter>(dev.get_root_node(), "/out/main");

  proto.main_audio_in->audio.resize(proto.inputs);

  for(int i = 0; i < proto.inputs; i++)
  {
    proto.audio_ins[i]->audio.resize(1);
  }

  proto.main_audio_out->audio.resize(proto.outputs);
  for(int i = 0; i < proto.outputs; i++)
  {
    proto.audio_outs[i]->audio.resize(1);
  }


  std::cerr << "=== stream start ===\n";

  int err = jack_activate (client);
  if (err != 0)
  {
    std::cerr << "JACK error: " << err << std::endl;
    return;
  }
  stop_processing = false;
}

void jack_engine::stop()
{
  //std::cerr << "=== STOPPED PROCESS ==" << std::endl;
  if(client)
  {
    stop_processing = true;
    auto clt = client.load();
    client = nullptr;
    protocol = nullptr;
    jack_deactivate(clt);
    jack_set_process_callback(clt, [] (jack_nframes_t nframes, void* arg) -> int { return 0; }, nullptr);
    jack_client_close(clt);
    while(processing) std::this_thread::sleep_for(std::chrono::milliseconds(100)) ;
    input_ports.clear();
    output_ports.clear();
  }
}

int jack_engine::process(jack_nframes_t nframes, void* arg)
{
  auto& self = *static_cast<jack_engine*>(arg);
  if(self.stop_processing)
  {
    std::cerr << "=== NO RUNNING ==" << std::endl;
    return 0;
  }

  auto clt = self.client.load();
  auto proto = self.protocol.load();
  if(clt && proto)
  {
    self.processing = true;
    auto float_input = (float**)alloca(sizeof(float*) * proto->inputs);
    auto float_output = (float**) alloca(sizeof(float*) * proto->outputs);
    for(int i = 0; i < proto->inputs; i++)
    {
      float_input[i] = (jack_default_audio_sample_t *) jack_port_get_buffer(self.input_ports[i], nframes);
    }
    for(int i = 0; i < proto->outputs; i++)
    {
      float_output[i] = (jack_default_audio_sample_t *) jack_port_get_buffer(self.output_ports[i], nframes);
    }

    proto->process_generic(*proto, float_input, float_output, nframes);
    proto->audio_tick(nframes, 0);
    self.processing = false;
  }
  return 0;
}
}

#include <../weakjack/weak_libjack.c>
#endif
