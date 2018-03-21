#include <ossia/audio/audio_protocol.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/device.hpp>

#include <ossia/audio/jack_protocol.hpp>
#include <ossia/audio/portaudio_protocol.hpp>
#include <ossia/audio/sdl_protocol.hpp>
#include <ossia/audio/dummy_protocol.hpp>

namespace ossia
{

ossia::audio_engine* make_audio_engine(std::string proto)
{
  ossia::audio_engine* p{};
  if(0) { }
#if __has_include(<portaudio.h>)
  else if(proto == "PortAudio")
  {
    p = new ossia::portaudio_engine;
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
   p = new ossia::portaudio_engine;
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

  if(!p)
  {
    return new ossia::dummy_engine;
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
  }
  set_tick([] (auto&&...) { });

  smallfun::function<void()> f;
  while(funlist.try_dequeue(f))
    f();

  engine = nullptr;
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
    jack_engine
  #elif __has_include(<portaudio.h>)
    portaudio_engine
  #else
    ossia::dummy_engine
  #endif
;
audio_device::audio_device(std::string name)
  : audio_device{std::make_unique<audio_protocol>(), name}
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
