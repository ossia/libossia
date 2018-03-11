#include <ossia/dataflow/audio_protocol.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/device.hpp>
#if defined(__MACH__)
#include <mach/mach_init.h>
#include <mach/thread_policy.h>
#include <mach/clock.h>
#include <mach/sync.h>
#include <mach/mach_time.h>
#endif
namespace ossia
{
audio_protocol::audio_protocol():
  bufferSize{128}
{
  audio_tick = [] (auto&&...) { };
#if defined(OSSIA_PROTOCOL_AUDIO)
#if defined(__EMSCRIPTEN__)
  //SDL_Init(SDL_INIT_AUDIO);
#else
  if(Pa_Initialize() != paNoError)
    throw std::runtime_error("Audio error");
#endif
#endif
}

audio_protocol::~audio_protocol()
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  stop();
#if defined(__EMSCRIPTEN__)
  //SDL_Quit();
#else
  Pa_Terminate();
#endif
#endif
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

void audio_protocol::stop()
{
#if defined(OSSIA_PROTOCOL_AUDIO)
#if defined(__EMSCRIPTEN__)
#else
  if(m_stream)
  {
    auto ec = Pa_StopStream(m_stream);
    std::cerr << "=== stream stop ===\n";

    if(ec != PaErrorCode::paNoError)
    {
      std::cerr << "Error while stopping audio stream: " << Pa_GetErrorText(ec) << std::endl;
    }

    m_stream = nullptr;
  }
#endif
#endif
}

void audio_protocol::reload()
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  stop();

#if defined(__EMSCRIPTEN__)
#else
  auto inputInfo = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());
  if(!inputInfo)
    return;
  auto outputInfo = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());
  if(!outputInfo)
    return;
  inputs = inputInfo->maxInputChannels;
  outputs = outputInfo->maxOutputChannels;

  audio_ins.clear();
  audio_outs.clear();

  main_audio_in = ossia::net::find_or_create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/in/main");
  main_audio_out = ossia::net::find_or_create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/out/main");
  for(int i = 0; i < inputs; i++)
  {
    audio_ins.push_back(ossia::net::find_or_create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/in/" + std::to_string(i)));
  }
  for(int i = 0; i < outputs; i++)
  {
    audio_outs.push_back(ossia::net::find_or_create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/out/" + std::to_string(i)));
  }

  main_audio_in->audio.resize(inputs);
  for(int i = 0; i < inputs; i++)
  {
    audio_ins[i]->audio.resize(1);
  }

  main_audio_out->audio.resize(outputs);
  for(int i = 0; i < outputs; i++)
  {
    audio_outs[i]->audio.resize(1);
  }


  PaStreamParameters inputParameters;
  inputParameters.device = Pa_GetDefaultInputDevice();
  inputParameters.channelCount = inputs;
  inputParameters.sampleFormat = paFloat32 | paNonInterleaved;
  inputParameters.suggestedLatency = 0.01;
  inputParameters.hostApiSpecificStreamInfo = nullptr;

  PaStreamParameters outputParameters;
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputParameters.channelCount = outputs;
  outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
  outputParameters.suggestedLatency = 0.01;
  outputParameters.hostApiSpecificStreamInfo = nullptr;

  std::cerr << "=== stream start ===\n";
  auto ec = Pa_OpenStream(&m_stream,
                          &inputParameters,
                          &outputParameters,
                          rate,
                          bufferSize, //paFramesPerBufferUnspecified,
                          paNoFlag,
                          &PortAudioCallback,
                          this);
  if(ec == PaErrorCode::paNoError)
  {
    ec = Pa_StartStream( m_stream );
    if(ec != PaErrorCode::paNoError)
    {
      std::cerr << "Error while starting audio stream: " << Pa_GetErrorText(ec) << std::endl;
    }
  }
  else
    std::cerr << "Error while opening audio stream: " << Pa_GetErrorText(ec) << std::endl;
#endif
#endif
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


#if defined(__EMSCRIPTEN__)
#else
PaStream*audio_protocol::stream() { return m_stream; }
int audio_protocol::PortAudioCallback(
    const void* input,
    void* output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
  auto& self = *static_cast<audio_protocol*>(userData);
  {
    smallfun::function<void()> f;
    while(self.funlist.try_dequeue(f))
      f();
  }

#if defined(OSSIA_PROTOCOL_AUDIO)
  using idx_t = gsl::span<float>::index_type;
  const idx_t fc = frameCount;

  auto float_input = ((float **) input);
  auto float_output = ((float **) output);

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

  self.audio_tick(frameCount, timeInfo->currentTime);
  return paContinue;
#else
  return {};
#endif
}

audio_device::audio_device(std::string name)
  : device{std::make_unique<audio_protocol>(), name}
  , protocol{static_cast<audio_protocol&>(device.get_protocol())}
{
  protocol.reload();
}


audio_device::~audio_device()
{
  protocol.stop();
}


ossia::audio_parameter& audio_device::get_main_in()
{
  return static_cast<ossia::audio_parameter&>(*ossia::net::find_node(device.get_root_node(), "/in/main")->get_parameter());
}

ossia::audio_parameter& audio_device::get_main_out()
{
  return static_cast<ossia::audio_parameter&>(*ossia::net::find_node(device.get_root_node(), "/out/main")->get_parameter());
}

#endif
}
