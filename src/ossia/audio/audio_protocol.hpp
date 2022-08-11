#pragma once
#include <ossia/audio/audio_tick.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>

namespace ossia
{
class audio_parameter;
class virtual_audio_parameter;
class mapped_audio_parameter;
class OSSIA_EXPORT audio_protocol final : public ossia::net::protocol_base
{
public:
  audio_protocol();
  ~audio_protocol() override;

  void setup_tree(int inputs, int outputs);
  void advance_tick(std::size_t count);
  void setup_buffers(audio_tick_state state);

  bool pull(ossia::net::parameter_base&) override;
  bool push(const ossia::net::parameter_base&, const ossia::value& v) override;
  bool push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool push_raw_bundle(const std::vector<ossia::net::full_parameter_data>&) override;
  bool observe(ossia::net::parameter_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;
  void set_device(ossia::net::device_base& dev) override;

  void register_parameter(mapped_audio_parameter& p);
  void unregister_parameter(mapped_audio_parameter& p);
  void register_parameter(virtual_audio_parameter& p);
  void unregister_parameter(virtual_audio_parameter& p);

  [[nodiscard]] ossia::net::device_base& get_device() const { return *m_dev; }
  ossia::audio_parameter* main_audio_in{};
  ossia::audio_parameter* main_audio_out{};
  std::vector<ossia::audio_parameter*> audio_ins;
  std::vector<ossia::audio_parameter*> audio_outs;

  std::vector<ossia::mapped_audio_parameter*> in_mappings;
  std::vector<ossia::mapped_audio_parameter*> out_mappings;
  std::vector<ossia::virtual_audio_parameter*> virtaudio;

protected:
  ossia::net::device_base* m_dev{};
};

}
