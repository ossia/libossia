#pragma once

#include <ossia/audio/audio_engine.hpp>
#include <ossia/audio/audio_protocol.hpp>

namespace ossia
{
class OSSIA_EXPORT audio_device
{
public:
  audio_device(std::string name = "audio", int bufferSize = 512, int sampleRate = 44100, int inputs = 2, int outputs = 2);
  audio_device(std::unique_ptr<audio_protocol>, std::string name = "audio", int bufferSize = 512, int sampleRate = 44100, int inputs = 2, int outputs = 2);
  ~audio_device();

  ossia::audio_parameter& get_main_in();
  ossia::audio_parameter& get_main_out();

  int get_buffer_size() const;
  int get_sample_rate() const;

  std::unique_ptr<ossia::audio_engine> engine;
  ossia::net::generic_device device;
  ossia::audio_protocol& protocol;

private:
  int m_bs{}, m_sr{};
};
}
