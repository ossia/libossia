#include <ossia/dataflow/audio_protocol.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/device.hpp>

#if defined(USE_JACK)
#define __x86_64__ 1
#include <weak_libjack.h>
#endif

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

#if defined(USE_SDL)
  //SDL_Init(SDL_INIT_AUDIO);
#elif defined(USE_PORTAUDIO)
  if(Pa_Initialize() != paNoError)
    throw std::runtime_error("Audio error");
#elif defined(USE_JACK)
  std::cerr << "JACK: " << have_libjack() << std::endl;
#endif
}

audio_protocol::~audio_protocol()
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  stop();

#if defined(USE_SDL)
  //SDL_Quit();

#elif defined(USE_PORTAUDIO)
  Pa_Terminate();

#elif defined(USE_JACK)

#else

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
#if defined(USE_SDL)
#elif defined(USE_PORTAUDIO)
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
#elif defined(USE_JACK)
  if(client)
  {
    jack_client_close(client);
    client = nullptr;
    input_ports.clear();
    output_ports.clear();
  }
#endif
}

void audio_protocol::reload()
{
  stop();

#if defined(USE_SDL)

#elif defined(USE_JACK)

  client = jack_client_open("score", JackNullOption, NULL);
  if (!client)
  {
    std::cerr << "JACK server not running?" << std::endl;
    exit(0);
    return;
  }

  jack_set_process_callback(
        client,
        [] (jack_nframes_t nframes, void* ctx) -> int {
      auto self = (audio_protocol*) ctx;
      return self->process(nframes, ctx);
    },
        this);

  jack_set_sample_rate_callback (client, JackSampleRateCallback{}, this);

  jack_on_shutdown (client, JackShutdownCallback{}, this);


  constexpr const int count = 32;
  inputs = 32;
  outputs = 32;
  audio_ins.clear();
  audio_outs.clear();

  for(int i = 0; i < count; i++)
  {
    auto in = jack_port_register (client, ("in-" + std::to_string(i)).c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    auto out = jack_port_register (client, ("out-" + std::to_string(i)).c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    assert(in);
    input_ports.push_back(in);
    assert(out);
    output_ports.push_back(out);
    audio_ins.push_back(ossia::net::find_or_create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/in/" + std::to_string(i)));
    audio_outs.push_back(ossia::net::find_or_create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/out/" + std::to_string(i)));
  }

  main_audio_in = ossia::net::find_or_create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/in/main");
  main_audio_out = ossia::net::find_or_create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/out/main");

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


  std::cerr << "=== stream start ===\n";

  int err = jack_activate (client);
  if (err != 0)
  {
    std::cerr << "JACK error: " << err << std::endl;
  }


#elif defined(USE_PORTAUDIO)

  int card_in_idx = -1;
  int card_out_idx = -1;

  for(int i = 0; i < Pa_GetDeviceCount(); i++)
  {
    auto raw_name = Pa_GetDeviceInfo(i)->name;
    if(raw_name == card_in)
    {
      card_in_idx = i;
    }
    if(raw_name == card_out)
    {
      card_out_idx = i;
    }
  }
  if(card_in_idx == -1 || card_out_idx == -1)
    return;

  auto devInInfo = Pa_GetDeviceInfo(card_in_idx);
  if(!devInInfo)
    return;
  auto devOutInfo = Pa_GetDeviceInfo(card_out_idx);
  if(!devOutInfo)
    return;

  inputs = devInInfo->maxInputChannels;
  outputs = devOutInfo->maxOutputChannels;

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
  inputParameters.device = card_in_idx;
  inputParameters.channelCount = inputs;
  inputParameters.sampleFormat = paFloat32 | paNonInterleaved;
  inputParameters.suggestedLatency = 0.01;
  inputParameters.hostApiSpecificStreamInfo = nullptr;

  PaStreamParameters outputParameters;
  outputParameters.device = card_out_idx;
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

void audio_protocol::process_generic(audio_protocol& self, float** float_input, float** float_output, uint64_t frameCount)
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

#if defined(USE_SDL)

#elif defined(USE_PORTAUDIO)
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
  auto float_input = ((float **) input);
  auto float_output = ((float **) output);

  process_generic(self, float_input, float_output, frameCount);
  self.audio_tick(frameCount, timeInfo->currentTime);

  return paContinue;
}
#elif defined(USE_JACK)

int audio_protocol::process(jack_nframes_t nframes, void* arg)
{

  auto& self = *static_cast<audio_protocol*>(arg);
  auto float_input = (float**)alloca(sizeof(float*) * self.inputs);
  auto float_output = (float**) alloca(sizeof(float*) * self.outputs);
  for(int i = 0; i < self.inputs; i++)
  {
    float_input[i] = (jack_default_audio_sample_t *) jack_port_get_buffer(self.input_ports[i], nframes);
  }
  for(int i = 0; i < self.outputs; i++)
  {
    float_output[i] = (jack_default_audio_sample_t *) jack_port_get_buffer(self.output_ports[i], nframes);
  }

  process_generic(self, float_input, float_output, nframes);
  self.audio_tick(nframes, 0);

  return 0;
}
#endif
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


}

#if defined(USE_JACK)
#include <../weakjack/weak_libjack.c>
#endif
