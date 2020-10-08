#include <ossia/audio/audio_device.hpp>
#include <ossia/audio/portaudio_protocol.hpp>

namespace ossia
{

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
