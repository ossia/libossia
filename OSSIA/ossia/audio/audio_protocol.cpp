#include <ossia/audio/audio_protocol.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/device.hpp>

#include <ossia/audio/jack_protocol.hpp>
#include <ossia/audio/portaudio_protocol.hpp>
#include <ossia/audio/sdl_protocol.hpp>

#if __has_include(<weak_libjack.h>)
#define __x86_64__ 1
#define USE_WEAK_JACK 1
#define NO_JACK_METADATA 1
#include <weak_libjack.h>
#endif

namespace ossia
{

ossia::audio_engine* make_audio_engine(std::string proto)
{
  ossia::audio_engine* p{};
  if(0) { }
#if __has_include(<portaudio.h>)
  else if(proto == "PortAudio")
  {
    p = new ossia::portaudio_protocol;
  }
#endif

#if __has_include(<jack/jack.h>)
  else if(proto == "JACK")
  {
    p = new ossia::jack_engine;
  }
#endif

#if defined(__EMSCRIPTEN__)
  else if(proto == "SDL")
  {
    p = new ossia::sdl_protocol;
  }
#endif

  if(!p)
  {
#if __has_include(<portaudio.h>)
   p = new ossia::portaudio_protocol;
#endif
  }

  if(!p)
  {
#if __has_include(<jack/jack.h>)
    p = new ossia::jack_engine;
#endif
  }
  if(!p)
  {
#if defined(__EMSCRIPTEN__)
    p = new ossia::sdl_protocol;
#endif
  }

  return p;
}


audio_protocol::audio_protocol():
  bufferSize{128}
{
  audio_tick = [] (auto&&...) { };
}

audio_engine::~audio_engine()
{
}


audio_protocol::~audio_protocol()
{
  if(engine)
  {
    engine->stop();
    engine->protocol = nullptr;
  }
}

void audio_protocol::stop()
{
  if(engine)
  {
    engine->stop();
    engine->protocol = nullptr;
    engine = nullptr;
  }
}

bool audio_protocol::pull(ossia::net::parameter_base&)
{
  return false;
}

bool audio_protocol::push(const ossia::net::parameter_base&)
{
  return false;
}

bool audio_protocol::push_bundle(const std::vector<const ossia::net::parameter_base*>&)
{
  return false;
}

bool audio_protocol::push_raw(const ossia::net::full_parameter_data&)
{
  return false;
}

bool audio_protocol::push_raw_bundle(const std::vector<ossia::net::full_parameter_data>&)
{
  return false;
}

bool audio_protocol::observe(ossia::net::parameter_base&, bool)
{
  return false;
}

bool audio_protocol::update(ossia::net::node_base& node_base)
{
  return false;
}

void audio_protocol::set_device(ossia::net::device_base& dev)
{
  m_dev = &dev;
}

void audio_protocol::register_parameter(mapped_audio_parameter& p)
{
  funlist.enqueue([&] {
    if(p.is_output)
      out_mappings.push_back(&p);
    else
      in_mappings.push_back(&p);
  });
}

void audio_protocol::unregister_parameter(mapped_audio_parameter& p)
{
  funlist.enqueue([&] {
    if(p.is_output)
    {
      auto it = ossia::find(out_mappings, &p);
      if(it != out_mappings.end())
        out_mappings.erase(it);
    }
    else
    {
      auto it = ossia::find(in_mappings, &p);
      if(it != in_mappings.end())
        in_mappings.erase(it);
    }
  });
}

void audio_protocol::register_parameter(virtual_audio_parameter& p)
{
  funlist.enqueue([&] {
      virtaudio.push_back(&p);
  });
}

void audio_protocol::unregister_parameter(virtual_audio_parameter& p)
{
  funlist.enqueue([&] {
    auto it = ossia::find(virtaudio, &p);
    if(it != virtaudio.end())
      virtaudio.erase(it);
  });
}

void audio_protocol::process_generic(
    audio_protocol& self,
    float** float_input, float** float_output,
    uint64_t frameCount)
{
  {
    smallfun::function<void()> f;
    while(self.funlist.try_dequeue(f))
      f();
  }

  using idx_t = gsl::span<float>::index_type;
  const idx_t fc = frameCount;

  // Prepare virtual audio inputs
  for(auto virt : self.virtaudio)
  {
    virt->set_buffer_size(frameCount);
  }

  // Prepare audio inputs
  const int n_in_channels = self.inputs;
  for(int i = 0; i < n_in_channels; i++)
  {
    self.main_audio_in->audio[i] = {float_input[i], fc};
    self.audio_ins[i]->audio[0] = {float_input[i], fc};
  }

  for(auto mapped: self.in_mappings)
  {
    mapped->audio.resize(mapped->mapping.size());
    for(std::size_t i = 0; i < mapped->mapping.size(); i++)
    {
      mapped->audio[i] = {float_input[mapped->mapping[i]], fc};
    }
  }

  // Prepare audio outputs
  const int n_out_channels = self.outputs;
  for(int i = 0; i < n_out_channels; i++)
  {
    self.main_audio_out->audio[i] = {float_output[i], fc};
    self.audio_outs[i]->audio[0] = {float_output[i], fc};

    for(int j = 0; j < (int)frameCount; j++)
    {
      float_output[i][j] = 0;
    }
  }

  for(auto mapped: self.out_mappings)
  {
    mapped->audio.resize(mapped->mapping.size());
    for(std::size_t i = 0; i < mapped->mapping.size(); i++)
    {
      mapped->audio[i] = {float_output[mapped->mapping[i]], fc};
    }
  }

  // Run a tick
  if(self.replace_tick)
  {
    self.audio_tick = std::move(self.ui_tick);
    self.ui_tick = {};
    self.replace_tick = false;
  }

}

using default_audio_protocol =
  #if defined(__EMSCRIPTEN__)
    sdl_protocol
  #elif defined(_MSC_VER)
    #define USE_JACK 1
    jack_engine
  #else
    portaudio_protocol
  #endif
;
audio_device::audio_device(std::string name)
  : audio_device{std::make_unique<default_audio_protocol>(), name}
{
}

audio_device::audio_device(std::unique_ptr<audio_protocol> proto, std::string name)
  : device{std::move(proto), name}
  , protocol{static_cast<audio_protocol&>(device.get_protocol())}
{
}


audio_device::~audio_device()
{
}


ossia::audio_parameter& audio_device::get_main_in()
{
  return static_cast<ossia::audio_parameter&>(*ossia::net::find_node(device.get_root_node(), "/in/main")->get_parameter());
}

ossia::audio_parameter& audio_device::get_main_out()
{
  return static_cast<ossia::audio_parameter&>(*ossia::net::find_node(device.get_root_node(), "/out/main")->get_parameter());
}


}

#if defined(USE_JACK)

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
  this->protocol = p;
  stop();

  if(!p)
    return;
  auto& proto = *p;
  proto.engine = this;
  auto& dev = proto.get_device();
  client = jack_client_open(dev.get_name().c_str(), JackNullOption, NULL);
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
