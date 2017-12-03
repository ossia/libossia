#include <ossia/dataflow/audio_protocol.hpp>

namespace ossia
{
audio_protocol::audio_protocol():
  bufferSize{128}
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  if(Pa_Initialize() != paNoError)
    throw std::runtime_error("Audio error");
#endif
}

audio_protocol::~audio_protocol()
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  stop();
  Pa_Terminate();
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
  if(m_stream)
  {
    Pa_StopStream(m_stream);
    m_stream = nullptr;
  }
#endif
}

void audio_protocol::reload()
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  stop();

  auto inputInfo = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());
  auto outputInfo = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());
  inputs = inputInfo->maxInputChannels;
  outputs = outputInfo->maxOutputChannels;

  audio_ins.clear();
  audio_outs.clear();
  m_dev->get_root_node().clear_children();

  main_audio_in = ossia::net::create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/in/main");
  main_audio_out = ossia::net::create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/out/main");
  for(int i = 0; i < inputs; i++)
  {
    audio_ins.push_back(ossia::net::create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/in/" + std::to_string(i)));
  }
  for(int i = 0; i < outputs; i++)
  {
    audio_outs.push_back(ossia::net::create_parameter<ossia::audio_parameter>(m_dev->get_root_node(), "/out/" + std::to_string(i)));
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
}

PaStream*audio_protocol::stream() { return m_stream; }


int audio_protocol::PortAudioCallback(
    const void* input,
    void* output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  using idx_t = gsl::span<float>::index_type;
  const idx_t fc = frameCount;
  auto& self = *static_cast<audio_protocol*>(userData);

  auto float_input = ((float **) input);
  auto float_output = ((float **) output);

  // Prepare audio inputs
  const int n_in_channels = (int)self.audio_ins.size();
  self.main_audio_in->audio.resize(n_in_channels);
  for(int i = 0; i < n_in_channels; i++)
  {
    self.main_audio_in->audio[i] = {float_input[i], fc};

    self.audio_ins[i]->audio.resize(1);
    self.audio_ins[i]->audio[0] = {float_input[i], fc};
  }

  // Prepare audio outputs
  const int n_out_channels = (int)self.audio_outs.size();
  self.main_audio_out->audio.resize(n_out_channels);
  for(int i = 0; i < n_out_channels; i++)
  {
    self.main_audio_out->audio[i] = {float_output[i], fc};
    self.audio_outs[i]->audio.resize(1);
    self.audio_outs[i]->audio[0] = {float_output[i], fc};

    for(int j = 0; j < (int)frameCount; j++)
    {
      float_output[i][j] = 0;
    }
  }

  // Run a tick
  if(self.replace_tick)
  {
    self.audio_tick = std::move(self.ui_tick);
    self.ui_tick = {};
    self.replace_tick = false;
  }

  if(self.audio_tick)
  {
    self.audio_tick(frameCount);
  }
  return paContinue;
#else
  return {};
#endif
}
}
