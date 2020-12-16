#pragma once
#include <cinttypes>
#include <string_view>
#include <ossia_export.h>

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
  int sampleRate() const noexcept;
  int bufferSize() const noexcept;
  double modelToSamples() const noexcept;
  double samplesToModel() const noexcept;
  int64_t samplesSinceStart() const noexcept;
  double startDate() const noexcept;
  double currentDate() const noexcept;
  ossia::net::node_base* find_node(std::string_view name) const noexcept;

  int64_t physical_start(const token_request& t) const noexcept;

  void insert(ossia::net::parameter_base& dest, const typed_value& v);
  void insert(ossia::net::parameter_base& dest, typed_value&& v);
  void insert(ossia::audio_parameter& dest, const audio_port& v);
  void insert(ossia::net::parameter_base& dest, const midi_port& v);
  void insert(const ossia::state& v);
};

}
