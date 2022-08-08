#pragma once
#include <cinttypes>
#include <string_view>
#include <ossia/detail/config.hpp>

namespace ossia
{
namespace net
{
class node_base;
class parameter_base;
}

struct execution_state;
class audio_parameter;
struct typed_value;
struct audio_port;
struct midi_port;
struct token_request;
class state;

struct OSSIA_EXPORT exec_state_facade
{
  ossia::execution_state* impl{};
  [[nodiscard]] int sampleRate() const noexcept;
  [[nodiscard]] int bufferSize() const noexcept;
  [[nodiscard]] double modelToSamples() const noexcept;
  [[nodiscard]] double samplesToModel() const noexcept;
  [[nodiscard]] int64_t samplesSinceStart() const noexcept;
  [[nodiscard]] double startDate() const noexcept;
  [[nodiscard]] double currentDate() const noexcept;
  [[nodiscard]] ossia::net::node_base*
  find_node(std::string_view name) const noexcept;

  struct sample_timings {
    int64_t start_sample;
    int64_t length;
  };

  [[nodiscard]] sample_timings timings(const token_request& t) const noexcept;

  void insert(ossia::net::parameter_base& dest, const typed_value& v);
  void insert(ossia::net::parameter_base& dest, typed_value&& v);
  void insert(ossia::audio_parameter& dest, const audio_port& v);
  void insert(ossia::net::parameter_base& dest, const midi_port& v);
  void insert(const ossia::state& v);
};

}
