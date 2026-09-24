#pragma once
#include <ossia/audio/audio_tick.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <atomic>

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

  // Allocates and mutates the node tree: must not be called from the audio thread.
  void setup_tree(int inputs, int outputs);
  void advance_tick(std::size_t count);
  void setup_buffers(audio_tick_state state);

  // Drops every span pointing into the driver's buffers.
  void clear_buffers() noexcept;

  // Scales every hardware output by the gain of /out/main, once the whole
  // tick has been summed into them: /out/main is the master, whichever address
  // a signal was sent to. A gain change ramps over the buffer.
  void apply_main_gain(const audio_tick_state& state) noexcept;
  //! The next apply_main_gain starts at the gain of /out/main instead of
  //! ramping from the one of its previous call, which may be long ago.
  void reset_main_gain_ramp() noexcept { m_main_gain_reset = true; }

  bool pull(ossia::net::parameter_base&) override;
  bool push(const ossia::net::parameter_base&, const ossia::value& v) override;
  bool push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool push_raw_bundle(const std::vector<ossia::net::full_parameter_data>&) override;
  bool observe(ossia::net::parameter_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;
  void set_device(ossia::net::device_base& dev) override;
  void stop() override;

  void register_parameter(mapped_audio_parameter& p);
  void unregister_parameter(mapped_audio_parameter& p);
  void register_parameter(virtual_audio_parameter& p);
  void unregister_parameter(virtual_audio_parameter& p);

  //! The mapped and virtual ports that the audio callback walks.
  struct ports
  {
    std::vector<ossia::mapped_audio_parameter*> in_mappings;
    std::vector<ossia::mapped_audio_parameter*> out_mappings;
    std::vector<ossia::virtual_audio_parameter*> virtaudio;
  };

  //! By default the callback walks the lists below as they are changed. Once
  //! deferred, it walks its own copy, which only changes through swap_ports:
  //! the lists can then change while the audio runs.
  void defer_port_changes(bool b);
  //! A copy of the lists below, made off the audio thread.
  [[nodiscard]] ports current_ports() const;
  //! Hands the lists to the callback: call it between two ticks. `p` gets the
  //! previous ones, to be freed off the audio thread.
  void swap_ports(ports& p) noexcept;

  [[nodiscard]] ossia::net::device_base& get_device() const { return *m_dev; }
  ossia::audio_parameter* main_audio_in{};
  ossia::audio_parameter* main_audio_out{};
  std::vector<ossia::audio_parameter*> audio_ins;
  std::vector<ossia::audio_parameter*> audio_outs;

  //! The mapped and virtual ports, as the interface sees them.
  std::vector<ossia::mapped_audio_parameter*> in_mappings;
  std::vector<ossia::mapped_audio_parameter*> out_mappings;
  std::vector<ossia::virtual_audio_parameter*> virtaudio;

protected:
  ossia::net::device_base* m_dev{};

private:
  ports m_live;
  bool m_deferred{};
  float m_main_gain{1.f};
  std::atomic_bool m_main_gain_reset{true};
};

}
