#include <ossia/audio/audio_protocol.hpp>
#include <ossia/audio/dummy_protocol.hpp>
#include <ossia/audio/jack_protocol.hpp>
#include <ossia/audio/portaudio_protocol.hpp>
#include <ossia/audio/pulseaudio_protocol.hpp>
#include <ossia/audio/sdl_protocol.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node_functions.hpp>

namespace ossia
{

ossia::audio_engine* make_audio_engine(
    std::string proto, std::string name, std::string req_in,
    std::string req_out, int& inputs, int& outputs, int& rate, int& bs)
{
  ossia::audio_engine* p{};

#if defined(__EMSCRIPTEN__)
  rate = 44100;
  bs = 8192;
  inputs = 0;
  outputs = 2;
  return new ossia::sdl_protocol{rate, bs};
#endif

  if (0)
  {
  }
#if __has_include(<pulse/pulseaudio.h>)
  else if (proto == "PulseAudio")
  {
    p = new ossia::pulseaudio_engine{name, req_in, req_out, inputs, outputs, rate,   bs};
  }
#endif
#if __has_include(<portaudio.h>)
  else if (proto == "PortAudio")
  {
    p = new ossia::portaudio_engine{name,    req_in, req_out, inputs,
                                    outputs, rate,   bs};
  }
#endif

#if __has_include(<jack/jack.h>) && defined(USE_WEAK_JACK)
  else if (proto == "JACK")
  {
    p = new ossia::jack_engine{std::make_shared<jack_client>(name), inputs, outputs};
  }
#endif

#if defined(OSSIA_AUDIO_SDL)
  else if (proto == "SDL")
  {
    inputs = 0;
    outputs = 2;
    return new ossia::sdl_protocol{rate, bs};
  }
#endif

  else if (proto == "Dummy")
  {
    p = new ossia::dummy_engine{rate, bs};
  }

  if (!p)
  {
#if __has_include(<pulse/pulseaudio.h>)
    p = new ossia::pulseaudio_engine{name, req_in, req_out, inputs, outputs, rate,   bs};
#endif
  }

  if (!p)
  {
#if __has_include(<portaudio.h>)
    p = new ossia::portaudio_engine{name,    req_in, req_out, inputs,
                                    outputs, rate,   bs};
#endif
  }

  if (!p)
  {
#if __has_include(<jack/jack.h>) && defined(USE_WEAK_JACK)
    p = new ossia::jack_engine{std::make_shared<jack_client>(name), inputs, outputs};
#endif
  }

  if (!p)
  {
#if defined(OSSIA_AUDIO_SDL)
    inputs = 0;
    outputs = 2;
    p = new ossia::sdl_protocol{rate, bs};
#endif
  }

  if (!p)
  {
    p = new ossia::dummy_engine{rate, bs};
  }

  return p;
}

audio_protocol::audio_protocol()
{
}

audio_engine::audio_engine()
{
  audio_tick = [](auto&&...) {};
}

audio_engine::~audio_engine()
{
}

void audio_engine::set_tick(audio_engine::fun_type&& t)
{
  tick_funlist.enqueue(std::move(t));
}

void audio_engine::load_audio_tick()
{
  while(tick_funlist.try_dequeue(audio_tick))
  {

  }
}

audio_protocol::~audio_protocol()
{
  if (engine)
  {
    engine->stop();
    engine->protocol = nullptr;
  }
}

void audio_protocol::stop()
{
  if (engine)
  {
    engine->stop();
  }

  smallfun::function<void()> f;
  while (funlist.try_dequeue(f))
    f();

  engine = nullptr;
}

void audio_protocol::setup_tree(int inputs, int outputs)
{
  auto& dev = get_device();
  auto& root = dev.get_root_node();

  audio_ins.clear();
  audio_outs.clear();

  main_audio_in = ossia::net::find_or_create_parameter<ossia::audio_parameter>(
      root, "/in/main");
  main_audio_out
      = ossia::net::find_or_create_parameter<ossia::audio_parameter>(
          root, "/out/main");
  for (int i = 0; i < inputs; i++)
  {
    audio_ins.push_back(
        ossia::net::find_or_create_parameter<ossia::audio_parameter>(
            root, "/in/" + std::to_string(i)));
  }
  for (int i = 0; i < outputs; i++)
  {
    audio_outs.push_back(
        ossia::net::find_or_create_parameter<ossia::audio_parameter>(
            root, "/out/" + std::to_string(i)));
  }

  main_audio_in->audio.resize(inputs);
  for (int i = 0; i < inputs; i++)
  {
    audio_ins[i]->audio.resize(1);
  }

  main_audio_out->audio.resize(outputs);
  for (int i = 0; i < outputs; i++)
  {
    audio_outs[i]->audio.resize(1);
  }
}

void audio_protocol::advance_tick(std::size_t count)
{
  for (auto& chan : main_audio_in->audio)
  {
    if (!chan.empty())
    {
      chan = chan.subspan(count);
    }
  }
  for (auto& chan : main_audio_out->audio)
  {
    if (!chan.empty())
    {
      chan = chan.subspan(count);
    }
  }

  for (auto in : audio_ins)
  {
    for (auto& chan : in->audio)
    {
      if (!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for (auto in : in_mappings)
  {
    for (auto& chan : in->audio)
    {
      if (!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for (auto out : audio_outs)
  {
    for (auto& chan : out->audio)
    {
      if (!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for (auto out : out_mappings)
  {
    for (auto& chan : out->audio)
    {
      if (!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }
}

bool audio_protocol::pull(ossia::net::parameter_base&)
{
  return false;
}

bool audio_protocol::push(const ossia::net::parameter_base&, const ossia::value& v)
{
  return false;
}

bool audio_protocol::push_bundle(
    const std::vector<const ossia::net::parameter_base*>&)
{
  return false;
}

bool audio_protocol::push_raw(const ossia::net::full_parameter_data&)
{
  return false;
}

bool audio_protocol::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>&)
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
    if (p.is_output)
      out_mappings.push_back(&p);
    else
      in_mappings.push_back(&p);
  });
}

void audio_protocol::unregister_parameter(mapped_audio_parameter& p)
{
  funlist.enqueue([&] {
    if (p.is_output)
    {
      auto it = ossia::find(out_mappings, &p);
      if (it != out_mappings.end())
        out_mappings.erase(it);
    }
    else
    {
      auto it = ossia::find(in_mappings, &p);
      if (it != in_mappings.end())
        in_mappings.erase(it);
    }
  });
}

void audio_protocol::register_parameter(virtual_audio_parameter& p)
{
  funlist.enqueue([&] { virtaudio.push_back(&p); });
}

void audio_protocol::unregister_parameter(virtual_audio_parameter& p)
{
  funlist.enqueue([&] {
    auto it = ossia::find(virtaudio, &p);
    if (it != virtaudio.end())
      virtaudio.erase(it);
  });
}

void audio_protocol::process_generic(
    audio_protocol& self, ossia::audio_tick_state state)
{
  {
    smallfun::function<void()> f;
    while (self.funlist.try_dequeue(f))
      f();
  }

  //using idx_t = gsl::span<float>::index_type;
  const gsl::span<float>::size_type fc = state.frames;

  // Prepare virtual audio inputs
  for (auto virt : self.virtaudio)
  {
    virt->set_buffer_size(state.frames);
  }

  // Prepare audio inputs
  for (int i = 0; i < state.n_in; i++)
  {
    self.main_audio_in->audio[i] = {state.inputs[i], fc};
    self.audio_ins[i]->audio[0] = {state.inputs[i], fc};
  }

  for (auto mapped : self.in_mappings)
  {
    mapped->audio.resize(mapped->mapping.size());
    for (std::size_t i = 0; i < mapped->mapping.size(); i++)
    {
      auto map_channel = mapped->mapping[i];
      if (map_channel < state.n_in)
        mapped->audio[i] = {state.inputs[map_channel], fc};
      else
        mapped->audio[i] = {};
    }
  }

  // Prepare audio outputs
  for (int i = 0; i < state.n_out; i++)
  {
    self.main_audio_out->audio[i] = {state.outputs[i], fc};
    self.audio_outs[i]->audio[0] = {state.outputs[i], fc};

    for (int j = 0; j < (int)state.frames; j++)
    {
      state.outputs[i][j] = 0;
    }
  }

  for (auto mapped : self.out_mappings)
  {
    mapped->audio.resize(mapped->mapping.size());
    for (std::size_t i = 0; i < mapped->mapping.size(); i++)
    {
      auto map_channel = mapped->mapping[i];
      if (map_channel < state.n_out)
        mapped->audio[i] = {state.outputs[map_channel], fc};
      else
        mapped->audio[i] = {};
    }
  }
}

audio_device::audio_device(std::string name, int bufferSize, int sampleRate, int inputs, int outputs)
    : audio_device{std::make_unique<audio_protocol>(), name, bufferSize, sampleRate, inputs, outputs}
{
}

audio_device::audio_device(
    std::unique_ptr<audio_protocol> proto, std::string name, int bs, int rate, int ins, int outs)
    : device{std::move(proto), name}
    , protocol{static_cast<audio_protocol&>(device.get_protocol())}
{
  std::string default_protocol, default_in, default_out;
#if defined(__EMSCRIPTEN__)
  default_protocol = "SDL";
#elif __has_include(<portaudio.h>)
  default_protocol = "PortAudio";
  Pa_Initialize();
  {
    auto in = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());
    auto out = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());
    if(in)
      default_in = in->name;
    if(out)
      default_out = out->name;
  }
  Pa_Terminate();
#elif defined(_MSC_VER) || __has_include(<weak_libjack.h>)
  default_protocol = "JACK";
#else
  default_protocol = "";
#endif
  engine = std::unique_ptr<ossia::audio_engine>(make_audio_engine(
      default_protocol, name, default_in, default_out, ins, outs, rate, bs));
  engine->reload(&protocol);
  m_bs = bs;
  m_sr = rate;
}

audio_device::~audio_device()
{
}

int audio_device::get_buffer_size() const
{
  return m_bs;
}

int audio_device::get_sample_rate() const
{
  return m_sr;
}

ossia::audio_parameter& audio_device::get_main_in()
{
  return static_cast<ossia::audio_parameter&>(
      *ossia::net::find_node(device.get_root_node(), "/in/main")
           ->get_parameter());
}

ossia::audio_parameter& audio_device::get_main_out()
{
  return static_cast<ossia::audio_parameter&>(
      *ossia::net::find_node(device.get_root_node(), "/out/main")
           ->get_parameter());
}
}
